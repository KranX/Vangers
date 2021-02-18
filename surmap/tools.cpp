#include "../src/global.h"

#ifdef _NT
#include "..\root\win32f.h"
#endif

#include "sqint.h"

#include "../src/common.h"
#include "tools.h"
#include "sqexp.h"

#include "../src/terra/vmap.h"
#include "../src/terra/world.h"
#include "../src/terra/render.h"
#include "impass.h"
#include "../src/units/moveland.h"

/* ----------------------------- EXTERN SECTION ---------------------------- */
extern int LayerStatus;
extern int TunnelHeight;
extern int TunnelProof;
extern int RenderingLayer;
extern int SecondLayerPriority;

extern int shape_size;
extern int shape_shift;
extern unsigned char* upper_buffer;
extern unsigned char* lower_buffer;
extern int shape_x,shape_y;
extern int MLstatus;
extern int MLprocess;
extern int DirectLog;
extern int ViewX,ViewY;
extern char*VLCsign[4];
extern char* VLfilenames[];

const char pathSeparator =
#ifdef _WIN32
	'\\';
#else
	'/';
#endif

/* --------------------------- PROTOTYPE SECTION --------------------------- */
void load_shape(char* name,int x,int y);
void release_shape();
void ClipboardOperation(int slot,int save,int render = 1);
void LINE_render(int y);
char* GetTargetName(const char* name);
/* --------------------------- DEFINITION SECTION -------------------------- */

BitMap placeBMP;
BitMap mosaicBMP(1);

int S3Dmode,S3Dlevel,S3Dinverse,S3DnoiseLevel,S3DnoiseAmp,S3Dside;

static int* lyrT;
static const char* lyrFName = "_LAYER_.SSS";
static XStream flyr;

int VLstatus,VLmode;
BaseValoc* curVL;
TntValoc* TntVLobj;
MLCloneValoc* MLCVLobj;
SensorValoc* SnsVLobj;
DangerValoc* DngVLobj;

TntValoc TntVLpattern;
MLCloneValoc MLCVLpattern;
SensorValoc SnsVLpattern;
DangerValoc DngVLpattern;
int VLediting;
int TntZall = -1;
int SnsZall = -1;
int DngZall = -1;

int MosaicTypes[8] = { 0,1,2,3,4,5,6,7 };

BitMap::BitMap(int _mosaic)
{
	mosaic = _mosaic;
	if(!mosaic) palette = new uchar[768];
	data = NULL;
	force = 0;
	mode = 0;
	border = 0;
	xoffset = yoffset = 0;
}

void BitMap::load(char* name)
{
	if(data){
		delete data;
		data = NULL;
		}

	XBuffer buf;
	buf < "bitmap" < pathSeparator;
	if(mosaic) buf < "mosaic" < pathSeparator;
	buf < name;

	XStream ff(buf.GetBuf(),XS_IN);
	ff > sx > sy;
	data = new uchar[sz = sx*(int)sy];
	ff.read(data,sz);
	ff.close();

	if(!mosaic){
		memcpy(buf.GetBuf() + strlen(buf.GetBuf()) - 3,"pal",3);
		memset(palette,0,768);
		ff.open(buf.GetBuf(),XS_IN);
		ff.read(palette,ff.size());
		ff.close();
		}
}

void BitMap::convert(void)
{
	uchar* pb = palette;
	uchar* p = data;
	register int i,j;
	int v,max = 0;
	for(i = 0;i < sz;i++,p++){
		v = *p*3;
		j = (palette[v] + (int)palette[v + 1] + (int)palette[v + 2])/3;
		if(j > max) max = j;
		*p = j;
		}
	p = data;
	if(size) for(i = 0;i < sz;i++,p++) *p = (uchar)((int)*p*(max + size)/max);

	if(border > 0){
		p = data;
		int m,n;
		uchar* pp;
		for(j = 0;j < sy;j++)
			for(i = 0;i < sx;i++,p++){
				if(*p && *p != 0xFF)
					for(m = -border;m < border;m++)
						for(n = -border;n < border;n++)
							if(j + m >= 0 && j + m < sy && i + n >= 0 && i + n < sx){
								pp = data + (j + m)*sx + i + n;
								if(!*pp) *pp = 0xFF;
								}
				}
		}
}

void BitMap::place(char* name,int _force,int _mode,int _border,int _level,int _size)
{
	force = _force;
	mode = _mode;
	border = _border;
	level = _level;
	size = _size;

	load(name);
	convert();

	int x = XCYCL(curGMap -> CX - sx/2);
	int y = YCYCL(curGMap -> CY - sy/2);
	uchar** lt = vMap -> lineT;
	uchar* bmp = data;
	uchar* pa,*pa0;

	vMap -> increase(y,y + sy - 1);

	register int i,j;
	int v,xx,yy,vv;
	for(j = 0;j < sy;j++){
		pa0 = pa = lt[yy = YCYCL(y + j)];
		if(!pa0) continue;
		for(i = 0;i < sx;i++,bmp++){
			pa = pa0 + (xx = XCYCL(x + i));
			if(RenderingLayer == UP_LAYER)
				v = GET_UP_ALT(pa + H_SIZE,*pa,pa0,xx);
			else
				v = GET_DOWN_ALT(pa + H_SIZE,*pa,pa0,xx);
			if(force || *bmp){
				if(border > 0 && *bmp == 0xFF)
					vv = 0;
				else
					switch(mode){
						case 0:
							vv = *bmp;
							break;
						case 1:
							vv = MAX(*bmp,v);
							break;
						case 2:
							vv = MIN(*bmp,v);
							break;
						case 3:
							vv = *bmp + v >> 1;
							break;
						case 4:
							vv = v + *bmp;
							break;
						}
				vv += level;
				pixSet(xx,yy,vv - v);
				}
			}
		}
	regRender(x,y,x + sx,y + sy);
}

void BeginSecondLayerEditing(void)
{
	LayerStatus = DOWN_LAYER;

	if(!lyrT) lyrT = new int[V_SIZE];
	register uint i;
	for(i = 0;i < V_SIZE;i++) lyrT[i] = -1;

	flyr.open(lyrFName,XS_OUT);
	flyr.close();
	flyr.open(lyrFName,XS_IN | XS_OUT);
}

int LayerWrite(int n,uchar* p)
{
	if(LayerStatus == UP_LAYER) return 0;
	if(lyrT[n] == -1){
		flyr.seek(0,XS_END);
		lyrT[n] = flyr.tell();
		}
	else
		flyr.seek(lyrT[n],XS_BEG);
	flyr.write(p,H2_SIZE);
	return 1;
}

int LayerRead(int n,uchar** p)
{
	if(LayerStatus == UP_LAYER) return 0;
	if(lyrT[n] != -1){
		flyr.seek(lyrT[n],XS_BEG);
		flyr.read(*p,H2_SIZE);
		return 1;
		}
	return 0;
}

void CancelSecondLayerEditing(void)
{
	LayerStatus = UP_LAYER;

	flyr.open(lyrFName,XS_OUT);
	flyr.close();
}

void AcceptSecondLayer(void)
{
	vMap -> flush();
	LayerStatus = UP_LAYER;

	uchar** lt = vMap -> lineT;
	uchar** ltc = vMap -> lineTcolor;
	uchar* pch = vMap -> changedT;
	uchar* pa,*pa0,*pay,*pay0,*pf,*pfy,*pt,prepa,pret,*pc;
	pay0 = pay = new uchar[H2_SIZE];

	register int x,d;
	int h,hy;
	for(uint i = 0;i < V_SIZE;i++)
		if(lyrT[i] != -1){
			flyr.seek(lyrT[i],XS_BEG);
			flyr.read(pay = pay0,H2_SIZE);
			vMap -> readLine(i,lt[i]);
			pa0 = pa = lt[i];
			pf = pa + H_SIZE;
			pfy = pay + H_SIZE;
			pc = ltc[i];
			for(x = 0;x < H_SIZE;x++,pa++,pay++,pf++,pfy++,pc++){
				if(*pf & DOUBLE_LEVEL){
					if(SecondLayerPriority){
						*pf = *pfy;
						*pa = *pay;
						}
					continue;
					}
				h = GET_UP_ALT(pf,*pa,pa0,x);
				hy = GET_UP_ALT(pfy,*pay,pay0,x);
				d = abs(h - hy);
				if(d >= TunnelHeight){
					if(!TunnelProof)
						d = TunnelHeight;
					else {
						d -= TunnelProof;
						if(d > MAX_RDELTA) d = MAX_RDELTA;
						}
					d = (d >> DELTA_SHIFT) - 1;
					if(h > hy){
						if(!(x & 1)){
							prepa = *pa;
							pret = GET_TERRAIN_TYPE(*pf);
							*pa = *pay;
							*pf = *pfy;
							SET_DELTA(*pf,(d & 12) >> 2);
							SET_DELTA(*(pf + 1),(d & 3));
							}
						else {
							pt = pa0 + H_SIZE + x - 1;
							if(!(*pt & DOUBLE_LEVEL)){
								*pt |= DOUBLE_LEVEL;
								*(pt - H_SIZE) = *pay;
								SET_TERRAIN(*pt,GET_TERRAIN_TYPE(*pfy));
								SET_DELTA(*pt,(d & 12) >> 2);
								SET_DELTA(*pf,(d & 3));
								}
							}
						*pc = COL1;
						}
					else {
						if(!(x & 1)){
							prepa = *pay;
							pret = GET_TERRAIN_TYPE(*pfy);
							SET_DELTA(*pf,(d & 12) >> 2);
							SET_DELTA(*(pf + 1),(d & 3));
							}
						else {
							pt = pa0 + H_SIZE + x - 1;
							if(!(*pt & DOUBLE_LEVEL)){
								*pt |= DOUBLE_LEVEL;
								*(pt - H_SIZE) = *pa;
								SET_TERRAIN(*pt,GET_TERRAIN_TYPE(*pf));
								SET_DELTA(*pt,(d & 12) >> 2);
								SET_DELTA(*pf,(d & 3));
								}
							SET_TERRAIN(*pf,GET_TERRAIN_TYPE(*pfy));
							*pa = *pay;
							}
						*pc = COL2;
						}
					*pf |= DOUBLE_LEVEL;
					}
				else {
					if(SecondLayerPriority){
						*pa = *pay;
						*pf = *pfy;
						}
					if(!(*pf & DOUBLE_LEVEL) && (x & 1) && (*(pf - 1) & DOUBLE_LEVEL)){
						*pf |= DOUBLE_LEVEL;
						*pa = prepa;
						SET_TERRAIN(*pf,pret);
						}
					}
				}
			pch[i] = 1;
//			LINE_render(i);
			}
	delete pay0;
}

void ShowHiddenLayer(void)
{
	int y0 = vMap -> upLine;
	int y1 = vMap -> downLine;

	int x,y;
	uchar** lt = vMap -> lineT;
	uchar** ltc = vMap -> lineTcolor;

	uchar* pa,*pc,*pf,*pa0;

	for(y = y0;y != y1;y = (y + 1) & clip_mask_y){
		pa0 = pa = lt[y];
		pf = pa + H_SIZE;
		pc = ltc[y];
		for(x = 0;x < map_size_x;x++,pa++,pc++,pf++){
			if(!(*pf & 3) && !(*(pf + 1) & 3)){
				if(*pf & DOUBLE_LEVEL) *(pc + 1) = *pc = COL1;
				}
			else {
				if(*pf & DOUBLE_LEVEL) *(pc + 1) = *pc = COL3;
				}
			x++,pa++,pc++,pf++;
			}
		}
}

void S3Dload(char* name)
{
	XBuffer buf;
	buf < "shape3d" < pathSeparator < name;

	load_shape(buf.GetBuf(),curGMap -> CX,curGMap -> CY);
}

void S3Danalyze(void)
{
	uchar** lt = vMap -> lineT;
	int x = shape_x;
	int y = shape_y;
	int xysize = shape_size,v,vv,xx,yy,empty;
	uchar* p;
	uchar* pa0;
	uchar* surface;
	if(!S3Dside) surface = upper_buffer, empty = 0;
	else surface = lower_buffer, empty = 255;
	uchar* pa;

	register int i,j;
	for(j = 0;j < xysize;j++){
		p = surface + (j << shape_shift);
		pa0 = lt[yy = YCYCL(y + j)];
		if(!pa0) continue;
		for(i = 0;i < xysize;i++,p++)
			if(*p != empty){
				v = S3Dinverse ? -*p : *p;
				v += S3Dlevel;
				if(S3DnoiseLevel && S3DnoiseAmp)
					if((int)realRND(100) < S3DnoiseLevel) v += S3DnoiseAmp - realRND((S3DnoiseAmp << 1) + 1);
				pa = pa0 + (xx = XCYCL(x + i));
				switch(S3Dmode){
					case 0:
						vv = v;
						break;
					case 1:
						vv = MAX(v,*pa);
						if(vv != v) continue;
						break;
					case 2:
						vv = MIN(v,*pa);
						if(vv != v) continue;
						break;
					case 3:
						vv = v + *pa >> 1;
						break;
					case 4:
						vv = v + *pa;
						break;
					}
				pixSet(xx,yy,vv - *pa);
				}
		}

	release_shape();
	regRender(x,y,x + xysize,y + xysize);
}

void MLanalyze(uchar*& delta,uchar*& terrain,unsigned*& signBits,int& sx,int& sy,int& x0,int& y0,int log)
{
	uchar** lt = vMap -> lineT;
	uchar* pch = vMap -> changedT;
	uchar* pa,*pa0,*pay,*pay0;
	pay0 = pay = new uchar[H2_SIZE];

	int lx = H_SIZE;
	int rx = -(int)H_SIZE;
	int uy = V_SIZE;
	int dy = -(int)V_SIZE;
	int cx = curGMap -> CX;
	int cy = curGMap -> CY;
	int bx0 = XCYCL(cx - curGMap -> xside);
	int bx1 = XCYCL(cx + curGMap -> xside);
	int by0 = YCYCL(cy - curGMap -> yside);
	int by1 = YCYCL(cy + curGMap -> yside);
	
	register int i,j,x,y,d,ii,jj;
	int h,hy;
	for(i = by0,ii = 0;i != by1;i = YCYCL(i + 1),ii++)
		if(pch[i] || log){
			if(lyrT[i] == -1) continue;
			flyr.seek(lyrT[i],XS_BEG);
			flyr.read(pay0,H2_SIZE);
			pa0 = lt[i];
			for(x = bx0,jj = 0;x != bx1;x = XCYCL(x + 1),jj++){
				pa = pa0 + x;
				pay = pay0 + x;
				h = GET_UP_ALT(pa + H_SIZE,*pa,pa0,x);
				hy = GET_UP_ALT(pay + H_SIZE,*pay,pay0,x);
				d = h - hy;
				if(d){
					if(jj > rx) rx = jj;
					if(jj < lx) lx = jj;
					if(ii > dy) dy = ii;
					if(ii < uy) uy = ii;
					}
				}
			}
	if(lx == H_SIZE) return;

	lx = XCYCL(bx0 + lx);
	rx = XCYCL(bx0 + rx);
	uy = YCYCL(by0 + uy);
	dy = YCYCL(by0 + dy);
	
	sx = getDeltaX(rx,lx) + 1;
	sy = getDelta(dy,uy) + 1;
	x0 = lx; y0 = uy;
	
	delta = new uchar[sx*sy];
	terrain = new uchar[sx*sy];
	uchar* pd = delta;
	uchar* pt = terrain;
	int ss = sx*sy/32 + 1;
	signBits = new unsigned[ss];
	memset(signBits,0,ss*4);
	unsigned* sb = signBits;
	unsigned mask = 1;

	for(j = 0,y = uy;j < sy;j++,y = YCYCL(y + 1)){
		if(lyrT[y] == -1) continue;
		flyr.seek(lyrT[y],XS_BEG);
		flyr.read(pay0,H2_SIZE);
		pa0 = lt[y];
		for(i = 0,x = lx;i < sx;i++,x = XCYCL(x + 1),pd++,pt++){
			h = GET_UP_ALT(pa0 + x + H_SIZE,*(pa0 + x),pa0,x);
			hy = GET_UP_ALT(pay0 + x + H_SIZE,*(pay0 + x),pay0,x);
			d = h - hy;
			*pd = abs(d);
			*pt = *(pa0 + x + H_SIZE) & TERRAIN_MASK;
			if(d < 0) *sb |= mask;
			if(!(mask <<= 1)){ sb++; mask = 1; }
			}
		}

	delete pay0;
}

void MLreset(void)
{
	if(!MLprocess) return;
	for(int i = 0;i < MLTableSize;i++) MLTable[i] -> reset();
}

MobileLocation* mlobj;
static int MLindex;

void MLbegin(void)
{
	vMap -> squeeze();
	MLreset();
	vMap -> flush();
	
	MLstatus = 1;
	BeginSecondLayerEditing();
	vMap -> sssReserve();	
	mlobj = new MobileLocation;
}

void MLedit(int n)
{
//	curGMap -> gotoxy(mlobj -> table[0].x0 + mlobj -> table[0].sx/2,mlobj -> table[0].y0 + mlobj -> table[0].sy/2);
	vMap -> squeeze();
	MLreset();
	vMap -> flush();
	
	MLstatus = 2;
	BeginSecondLayerEditing();
	vMap -> sssReserve();	

	MobileLocation* p = new MobileLocation;
	p -> makeCopy(mlobj);
	mlobj = p;
	mlobj -> frozen = 0;
	MLindex = n;
}

void MLrestoreframe(void)
{
	vMap -> refresh();
}

void MLgo(int n)
{
	MLrestoreframe();
	mlobj -> setPhase(n);
	vMap -> sssReserve();
}

void MLnextframe(void)
{
	int f = mlobj -> cFrame;
	int fn = (f >=  mlobj -> maxFrame - 1) ? 0 : f + 1;
	MLrestoreframe();
	mlobj -> setPhase(fn);
	vMap -> sssReserve();
}

void MLprevframe(void)
{
	int f = mlobj -> cFrame;
	int fp = f ? f - 1 : mlobj -> maxFrame - 1;
	MLrestoreframe();
	mlobj -> setPhase(fp);
	vMap -> sssReserve();
}

void MLacceptframe(void)
{
	ClipboardOperation(10,1);
	MLrestoreframe();

	int f = mlobj -> cFrame;
	int fn = (f >=  mlobj -> maxFrame - 1) ? 0 : f + 1;
	mlobj -> setPhase(fn);
	ClipboardOperation(11,1);

	int fp = f ? f - 1 : mlobj -> maxFrame - 1;
	mlobj -> setPhase(fp);
	vMap -> sssReserve();
	
	ClipboardOperation(10,0,0);

	int sx,sy,x0,y0;
	uchar* delta = NULL;
	uchar* terrain = NULL;
	unsigned* signBits = NULL;
	MLanalyze(delta,terrain,signBits,sx,sy,x0,y0,0);
	mlobj -> table[fp].accept(delta,terrain,signBits,sx,sy,x0,y0,0);
	vMap -> sssReserve();

	ClipboardOperation(11,0,0);
	delta = NULL;
	terrain = NULL;
	signBits = NULL;
	MLanalyze(delta,terrain,signBits,sx,sy,x0,y0,0);
	mlobj -> table[f].accept(delta,terrain,signBits,sx,sy,x0,y0,0);
	ClipboardOperation(10,0);
	mlobj -> cFrame = f;
	memset(mlobj -> steps,0,mlobj -> maxFrame*4);
	mlobj -> accept();
}

void MLdeleteframe(void)
{
	if(mlobj -> maxFrame <= 2) return;
	MLrestoreframe();

	int f = mlobj -> cFrame;
	int fp = f ? f - 1 : mlobj -> maxFrame - 1;
	int fn = (f >=  mlobj -> maxFrame - 1) ? 0 : f + 1;
	
	mlobj -> setPhase(fp);
	vMap -> sssReserve();
	mlobj -> setPhase(fn);

	int sx,sy,x0,y0;
	uchar* delta = NULL;
	uchar* terrain = NULL;
	unsigned* signBits = NULL;
	MLanalyze(delta,terrain,signBits,sx,sy,x0,y0,0);
	mlobj -> table[fp].accept(delta,terrain,signBits,sx,sy,x0,y0,0);
	vMap -> sssReserve();

	mlobj -> table[f].release();
	for(int i = f + 1;i < mlobj -> maxFrame;i++)
		mlobj -> table[i - 1] = mlobj -> table[i];
	mlobj -> maxFrame--;
	f = (f == mlobj -> maxFrame) ? 0 : f;
	mlobj -> cFrame = f;
	mlobj -> accept();
}

void MLinsertframe(void)
{
	MLrestoreframe();

	int f = mlobj -> cFrame;
	int fn = (f >=  mlobj -> maxFrame - 1) ? 0 : f + 1;
	
	mlobj -> setPhase(fn);
	ClipboardOperation(10,1);

	LayerStatus = UP_LAYER;
	vMap -> refresh();
	LayerStatus = DOWN_LAYER;

	mlobj -> maxFrame++;
	delete mlobj -> steps;
	memset(mlobj -> steps = new int[mlobj -> maxFrame],0,mlobj -> maxFrame*4);

	MLFrame* nt = new MLFrame[mlobj -> maxFrame];
	register int i;
	for(i = 0;i <= f;i++)
		nt[i] = mlobj -> table[i];
	for(i = f + 2;i < mlobj -> maxFrame;i++)
		nt[i] = mlobj -> table[i - 1];
	nt[f + 1].clear();
	delete mlobj -> table;
	mlobj -> table = nt;

	int sx = 0,sy = 0;
	int x0 = mlobj -> table[f].x0;
	int y0 = mlobj -> table[f].y0;
	uchar* delta = NULL;
	uchar* terrain = NULL;
	unsigned* signBits = NULL;
	MLanalyze(delta,terrain,signBits,sx,sy,x0,y0,1);
	mlobj -> table[f].accept(delta,terrain,signBits,sx,sy,x0,y0,0,1);
	vMap -> sssReserve();

	ClipboardOperation(10,0,0);
	delta = NULL;
	terrain = NULL;
	signBits = NULL;
	MLanalyze(delta,terrain,signBits,sx,sy,x0,y0,1);
	mlobj -> table[f + 1].accept(delta,terrain,signBits,sx,sy,x0,y0,0,1);
	mlobj -> cFrame = f + 1;

	MLrestoreframe();
	mlobj -> accept();
}

void MLadd(void)
{
	int sx,sy,x0,y0;
	uchar* delta = NULL;
	uchar* terrain = NULL;
	unsigned* signBits = NULL;
	MLanalyze(delta,terrain,signBits,sx,sy,x0,y0,0);
	if(!delta) return;
	mlobj -> add(delta,terrain,signBits,sx,sy,x0,y0);
	vMap -> sssReserve();	
}

void MLaccept(void)
{
	LayerStatus = UP_LAYER;
	vMap -> refresh();
	LayerStatus = DOWN_LAYER;
	int sx,sy,x0,y0;
	uchar* delta = NULL;
	uchar* terrain = NULL;
	unsigned* signBits = NULL;
	MLanalyze(delta,terrain,signBits,sx,sy,x0,y0,1);
	LayerStatus = UP_LAYER;
	if(delta) mlobj -> add(delta,terrain,signBits,sx,sy,x0,y0);
	
	if(!mlobj -> maxFrame) return;

	MLTableSize++;
	if(!MLTable) MLTable = new MobileLocation*;
	else MLTable = (MobileLocation**)realloc(MLTable,sizeof(MobileLocation*)*MLTableSize);
	MLTable[MLTableSize - 1] = mlobj;
	mlobj -> accept();
	mlobj -> save();
	MLstatus = 0;
}

void MLcancel(void)
{
	MLrestoreframe();
	MLstatus = 0;
	CancelSecondLayerEditing();
	mlobj -> reset();
	delete mlobj;
	mlobj = NULL;
}

void MLacceptediting(void)
{
	MLrestoreframe();
	delete MLTable[MLindex];
	MLTable[MLindex] = mlobj;
	mlobj -> reset();
	mlobj -> save();
	MLstatus = 0;
	CancelSecondLayerEditing();
}

void MobileLocation::add(uchar* delta,uchar* terrain,unsigned* signBits,int sx,int sy,int x0,int y0)
{
	maxFrame++;
	if(!table){
		table = new MLFrame;
		steps = new int[maxFrame];
		}
	else{
		table = (MLFrame*)realloc(table,sizeof(MLFrame)*maxFrame);
		table[maxFrame - 1].clear();
		steps = (int*)realloc(steps,sizeof(int)*maxFrame);
		}
	table[maxFrame - 1].accept(delta,terrain,signBits,sx,sy,x0,y0);
	steps[maxFrame - 1] = 0;
}

void MobileLocation::accept(void)
{
	altSx = altSy = 0;
	for(int i = 0;i < maxFrame;i++){
		if(table[i].sx > altSx) altSx = table[i].sx;
		if(table[i].sy > altSy) altSy = table[i].sy;
		}
	if(alt) delete alt;
	alt = new MLAtype[altSx*altSy];
	calcBounds();
}

void MobileLocation::makeCopy(MobileLocation* p)
{
	*this = *p;
	table = new MLFrame[maxFrame];
	register int i;
	MLFrame* f,*ff;
	for(i = 0;i < maxFrame;i++){
		table[i] = p -> table[i];
		f = table + i;
		ff = p -> table + i;
		memcpy(f -> delta = new uchar[f -> sz],ff -> delta,f -> sz);
		if(f -> terrain)
			memcpy(f -> terrain = new uchar[f -> sz],ff -> terrain,f -> sz);
		memcpy(f -> signBits = new unsigned[f -> ss],ff -> signBits,f -> ss*4);
		}
	
	memcpy(steps = new int[maxFrame],p -> steps,maxFrame*sizeof(int));
	alt = new MLAtype[altSx*altSy];
}

void MLFrame::accept(uchar* _delta,uchar* _terrain,unsigned* _signBits,int _sx,int _sy,int _x0,int _y0,int reallynew,int nonull)
{ 
	release();
	delta = _delta;
	terrain = _terrain;
	signBits = _signBits;
	sx = _sx; sy = _sy; 
	x0 = _x0; y0 = _y0;
	sz = sx*sy;
	ss = sz/32 + 1;
	if(reallynew){
		period = 1;
		surfType = 99;
		}
	if(nonull && !delta){
		sx = sy = 1;
		delta = new uchar;*delta = 0;
		terrain = new uchar;*terrain = 0xFF;
		signBits = new unsigned; *signBits = 0;
		sz = 1; ss = 1;
		period = 1;
		surfType = 99;
		}
}

void MLFrame::release(void)
{
	if(delta) delete delta;
	if(terrain) delete terrain;
	if(signBits) delete signBits;
}

void MLFrame::cut(int cx0,int cy0,int csx,int csy)
{
	int dx0 = cx0 - x0;
	int dy0 = cy0 - y0;
	int dx1 = x0 + sx - 1 - (cx0 + csx - 1);
	int dy1 = y0 + sy - 1 - (cy0 + csy - 1);
	
	int nx0 = dx0 < 0 ? x0 : cx0;
	int ny0 = dy0 < 0 ? y0 : cy0;

	dx0 = dx0 < 0 ? 0 : dx0;
	dx1 = dx1 < 0 ? 0 : dx1;
	dy0 = dy0 < 0 ? 0 : dy0;
	dy1 = dy1 < 0 ? 0 : dy1;
	if(!dx0 && !dx1 && !dy0 && !dy1) return;

	int nsx = sx - dx0 - dx1;
	int nsy = sy - dy0 - dy1;

	uchar* np0 = new uchar[nsx*nsy];
	uchar* np = np0;
	uchar* p = delta;
	p += dy0*sx + dx0;
	int i,j;
	for(j = 0;j < nsy;j++){
		for(i = 0;i < nsx;i++) *np++ = *p++;
		p += dx0 + dx1;
		}
	delete delta;
	delta = np0;

	if(terrain){
		np0 = new uchar[nsx*nsy];
		np = np0;
		p = terrain;
		p += dy0*sx + dx0;
		for(j = 0;j < nsy;j++){
			for(i = 0;i < nsx;i++) *np++ = *p++;
			p += dx0 + dx1;
			}
		delete terrain;
		terrain = np0;
		}

	unsigned* ns0 = new unsigned[nsx*nsy/32 + 1];
	memset(ns0,0,4*(nsx*nsy/32 + 1));
	unsigned* ns = ns0;
	int x,y = dy0;
	unsigned mask = 1;
	for(j = 0;j < nsy;j++,y++){
		x = dx0;
		for(i = 0;i < nsx;i++,x++){
			if(signBits[(y*sx + x)/32] & (1 << ((y*sx + x)%32))) *ns |= mask;
			if(!(mask <<= 1)){ ns++; mask = 1; }
			}
		}
	delete signBits;
	signBits = ns0;

	x0 = nx0;
	y0 = ny0;
	sx = nsx;
	sy = nsy;
	sz = sx*sy;
	ss = sz/32 + 1;
}

void MLshow(int n)
{
	curGMap -> gotoxy(MLTable[n] -> table[0].x0 + MLTable[n] -> table[0].sx/2,MLTable[n] -> table[0].y0 + MLTable[n] -> table[0].sy/2);
}

void MLdelete(int n)
{
	MobileLocation** p = NULL;
	if(MLTableSize - 1){
		p = new MobileLocation*[MLTableSize - 1];
		register int i,j;
		for(i = 0,j = 0;i < MLTableSize;i++)
			if(i != n){ p[j] = MLTable[i]; j++; }
		}
	MLTable[n] -> reset();
	MLTable[n] -> erase();
	delete MLTable[n];
	delete MLTable;
	MLTable = p;
	MLTableSize--;
}

static const char* vlNames[] = { "TNT","ML Clone","Sensor","Danger" };
const char* getVLname(void){ return vlNames[VLmode]; }
BaseValoc* getVLTail(void)
{
	switch(VLmode){
		case 0: return TntValoc::tail;
		case 1: return MLCloneValoc::tail;
		case 2: return SensorValoc::tail;
		case 3: return DangerValoc::tail;
		}
	return NULL;
}

void VLstart(int n)
{
	VLstatus = 1;
	VLmode = n;
	curVL = getVLTail();
}

void VLcancel(void)
{
	VLstatus = 0;
}

void VLsave(void)
{
	VLstatus = 0;

	XBuffer buf;

	buf < "DATA.VOT\\" < VLfilenames[VLmode] < ".VLC";
	XStream ff(GetTargetName(buf.GetBuf()),XS_OUT);
	ff.write(VLCsign[VLmode],strlen(VLCsign[VLmode]));

	BaseValoc* p = getVLTail();
	int max = 0;
	while(p){ max++; p = p -> next; }
	ff < max;
	p = getVLTail();
	while(p){
		p -> save(ff);
		p = p -> next;
		}
	
	ff.close();
}

BaseValoc* vlSeek(int x,int y)
{
	const int E = 5;

	BaseValoc* p = getVLTail();
	while(p){
		if(abs(getDistX(x,p -> x)) <= E && abs(getDistY(y,p -> y)) <= E) return p;
		p = p -> next;
		}
	return NULL;
}

void BaseValoc::link(void)
{
	BaseValoc** tail = getTail();
	if(*tail){
		prev = (*tail) -> prev;
		(*tail) -> prev -> next = this;
		(*tail) -> prev = this;
		}
	else {
		prev = this;
		*tail = this;
		}
	next = NULL;
}

void BaseValoc::remove(void)
{
	curVL = next;
	BaseValoc** tail = getTail();
	if(*tail == this){
		if(next) next -> prev = (*tail) -> prev;
		*tail = next;
		}
	else {
		prev -> next = next;
		if(next) next -> prev = prev;
		else (*tail) -> prev = prev;
		}
}

void VLadd(int x,int y)
{
	BaseValoc* p;
	switch(VLmode){
		case 0:
			p = new TntValoc;
			*((TntValoc*)p) = TntVLpattern;
			if(TntZall == -1)
				p -> z = GET_UP_ALT(vMap -> lineT[y] + x + H_SIZE,*(vMap -> lineT[y] + x),vMap -> lineT[y],x);
			else
				p -> z = TntZall;
			break;
		case 1:
			p = new MLCloneValoc; 
			*((MLCloneValoc*)p) = MLCVLpattern;
			break;
		case 2:
			p = new SensorValoc;
			*((SensorValoc*)p) = SnsVLpattern;
			if(SnsZall == -1)
				p -> z = GET_UP_ALT(vMap -> lineT[y] + x + H_SIZE,*(vMap -> lineT[y] + x),vMap -> lineT[y],x);
			else
				p -> z = SnsZall;
			break;
		case 3:
			p = new DangerValoc;
			*((DangerValoc*)p) = DngVLpattern;
			if(DngZall == -1)
				p -> z = GET_UP_ALT(vMap -> lineT[y] + x + H_SIZE,*(vMap -> lineT[y] + x),vMap -> lineT[y],x);
			else
				p -> z = DngZall;
			break;
		}
	p -> link();
	p -> x = x;
	p -> y = y;

	curVL = p;
}

void VLshow(void)
{
	BaseValoc* p = getVLTail();
	while(p){
		p -> show();
		p = p -> next;
		}
}

void BaseValoc::show(void)
{
	const int D = 3;

	int xd = getDistX(x,ViewX);
	int yd = getDistY(y,ViewY);
	if(abs(xd) < curGMap -> xside - D && abs(yd) < curGMap -> yside - D)
		XGR_Rectangle(curGMap -> xc + xd - D,curGMap -> yc + yd - D,2*D + 1,2*D + 1,COL1,COL1,XGR_FILLED);
}

void TntValoc::edit(void)
{
	VLediting = 1;
	TntVLobj = this;
	sqE -> put(E_VLTNTFORM,E_COMMON,XGR_MAXX/2,XGR_MAXY/2);
}

void MLCloneValoc::edit(void)
{
	VLediting = 1;
	MLCVLobj = this;
	sqE -> put(E_VLMLCFORM,E_COMMON,XGR_MAXX/2,XGR_MAXY/2);
}

void SensorValoc::edit(void)
{
	VLediting = 1;
	SnsVLobj = this;
	sqE -> put(E_VLSNSFORM,E_COMMON,XGR_MAXX/2,XGR_MAXY/2);
}

void DangerValoc::edit(void)
{
	VLediting = 1;
	DngVLobj = this;
	sqE -> put(E_VLDNGFORM,E_COMMON,XGR_MAXX/2,XGR_MAXY/2);
}

void VLnext(void)
{
	if(!curVL) curVL = getVLTail();
	else curVL = curVL -> next;
	if(curVL) curGMap -> gotoxy(curVL -> x,curVL -> y);
}

void TntValoc::accept(int _z,int _value,int _radius,int _type)
{ 
	z = _z; value = _value; radius = _radius; type = _type;
	TntVLpattern = *this;
}

void MLCloneValoc::accept(int _z,int _value)
{ 
	z = _z; value = _value; 
	MLCVLpattern = *this;
}

void SensorValoc::accept(int _z,int _id,int _radius,const char* _name,int _z0,int _data0,int _data1,int _data2,int _data3,int _data4,int _data5,int _data6)
{ 
	z = _z; id = _id; radius = _radius;
	name = strdup(_name);
	nameLen = strlen(name);
	SnsVLpattern = *this;
	z0 = _z0;
	data0 = _data0;
	data1 = _data1;
	data2 = _data2;
	data3 = _data3;
	data4 = _data4;
	data5 = _data5;
	data6 = _data6;
}

void DangerValoc::accept(int _z,int _type,int _radius)
{ 
	z = _z; type = _type; radius = _radius;
	DngVLpattern = *this;
}

void TntValoc::save(XStream& ff)
{
	ff < x < y < z;
	ff < value < radius < type;
}

void MLCloneValoc::save(XStream& ff)
{
	ff < x < y < z;
	ff < value;
}

void SensorValoc::save(XStream& ff)
{
	ff < x < y < z;
	ff < id < radius < nameLen;
	if(nameLen) ff.write(name,nameLen);

	ff < z0;
	ff < data0;
	ff < data1;
	ff < data2;
	ff < data3;
	ff < data4;
	ff < data5;
	ff < data6;

//	for(int i = 0;i < 8;i++) ff < (int)0;
}

void DangerValoc::save(XStream& ff)
{
	ff < x < y < z;
	ff < type < radius;
	for(int i = 0;i < 4;i++) ff < (int)0;
}

void VLlbpress(int mX,int mY)
{
	if(VLediting) return;
	BaseValoc* p = vlSeek(mX,mY);
	if(p) p -> edit();
	else VLadd(mX,mY);
}

int VLrbpress(int mX,int mY)
{
	if(VLediting) return 0;
	BaseValoc* p = vlSeek(mX,mY);
	if(p){
		p -> remove();
		return 1;
		}
	return 0;
}

const int CNUM = 10;
static int ClipboardStatus[CNUM + 2];
static int ClipboardCX[CNUM + 2];
static int ClipboardCY[CNUM + 2];
//@caiiiycuk: memory leak at the end (not worth to fix)
static char* ClipFname = strdup("clpbrd#.___");

void ClipboardInit(void)
{
	ClipFname[6] = '$';
	XStream ff(0);
	if(ff.open(GetTargetName(ClipFname),XS_IN)){
		for(int i = 0;i < CNUM;i++){
			ff >= ClipboardStatus[i];
			ff >= ClipboardCX[i] >= ClipboardCY[i];
			}
		ff.close();
		}
	else
		for(int i = 0;i < CNUM;i++) ClipboardStatus[i] = 0;
}

void ClipboardOperation(int slot,int save,int render)
{
	uint y;
	uchar** lt = vMap -> lineT;
	int xside = curGMap -> xside;
	int yside = curGMap -> yside;

	ClipFname[6] = '0' + slot;
	if(save){
		ClipboardCX[slot] = ViewX;
		ClipboardCY[slot] = ViewY;
		int xl = XCYCL(ViewX - xside);
		int xr = XCYCL(ViewX + xside);
		XStream ff(GetTargetName(ClipFname),XS_OUT);
		if(xr > xl)
			for(y = YCYCL(ViewY - yside);y != YCYCL(ViewY + yside);y = YCYCL(y + 1)){
				ff.write(lt[y] + xl,xr - xl + 1);
				ff.write(lt[y] + xl + H_SIZE,xr - xl + 1);
				}
		else
			for(y = YCYCL(ViewY - yside);y != YCYCL(ViewY + yside);y = YCYCL(y + 1)){
				ff.write(lt[y] + xl,H_SIZE - xl + 1);
				ff.write(lt[y] + xl + H_SIZE,H_SIZE - xl + 1);
				ff.write(lt[y],xr + 1);
				ff.write(lt[y] + H_SIZE,xr + 1);
				}
		ff.close();
		ClipboardStatus[slot] = 1;

		ClipFname[6] = '$';
		ff.open(GetTargetName(ClipFname),XS_OUT);
		for(int i = 0;i < CNUM;i++){
			ff <= ClipboardStatus[i] < " ";
			ff <= ClipboardCX[i] < " " <= ClipboardCY[i] < "\r\n";
			}
		ff.close();
		}
	else {
		if(!ClipboardStatus[slot]) return;
		curGMap -> gotoxy(ClipboardCX[slot],ClipboardCY[slot]);
		curGMap -> draw();
		int xl = XCYCL(ViewX - xside);
		int xr = XCYCL(ViewX + xside);
		XStream ff(GetTargetName(ClipFname),XS_IN);
		if(xr > xl)
			for(y = YCYCL(ViewY - yside);y != YCYCL(ViewY + yside);y = YCYCL(y + 1)){
				ff.read(lt[y] + xl,xr - xl + 1);
				ff.read(lt[y] + xl + H_SIZE,xr - xl + 1);
				if(render) LINE_render(y);
				vMap -> changedT[y] = 1;
				}
		else
			for(y = YCYCL(ViewY - yside);y != YCYCL(ViewY + yside);y = YCYCL(y + 1)){
				ff.read(lt[y] + xl,H_SIZE - xl + 1);
				ff.read(lt[y] + xl + H_SIZE,H_SIZE - xl + 1);
				ff.read(lt[y],xr + 1);
				ff.read(lt[y] + H_SIZE,xr + 1);
				if(render){
					LINE_render(y);
					vMap -> changedT[y] = 1;
					}
				}
		ff.close();
		}
}

void MakeAbsML(void)
{
	if(!mlobj) return;
	MobileLocation* m = new MobileLocation;
	m -> makeCopy(mlobj);
	
	int len = strlen(m -> name);
	len = MIN(len,MLNAMELEN - 4);
	strcpy(m -> name + len,"_ABS");
	m -> mode = MLM_REL2ABS;
	m -> inUse = 1;
	mlobj -> inUse = 0;
	memset(m -> isConverted = new int[m -> maxFrame],0,m -> maxFrame*sizeof(int));

	MLTableSize++;
	MLTable = (MobileLocation**)realloc(MLTable,sizeof(MobileLocation*)*MLTableSize);
	MLTable[MLTableSize - 1] = m;
	mlobj = m;
}
