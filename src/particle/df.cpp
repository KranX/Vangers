#include "../global.h"

#include "../3d/3d_math.h"

#include "../sqexp.h"

#include "../terra/vmap.h"
#include "../terra/world.h"

#include "../3d/parser.h"

#include "df.h"

extern int UcutLeft,UcutRight,VcutUp,VcutDown;

uchar* WaterColorTable;
uchar* FireColorTable;
extern uchar* TerrainAlphaTable[TERRAIN_MAX];

void smart_putspr(unsigned char* data,int Xcenter,int Ycenter,int XsizeB,int YsizeB,int ScaleXsize,int height,unsigned char* color_table);

//void bitmap2screenFire(char* vb,unsigned char* data,int Xcenter,int Ycenter,int XsizeB,int YsizeB,int ScaleXsize);

void WaveProcess::Init(char* filename)
{
	XStream ff(filename,XS_IN);
	ff > NumFrame;
	ff > sx;
	ff > sy;
	ff > bx;
	ff > by;
	size = sx * sy;
	g_size = size * NumFrame;
	buff = new uchar[size];
	memset(buff, 0, size);
	offset = new WAVE_TYPE[g_size];
//	x_offset = new char[g_size];
//	y_offset = new char[g_size];

	ff.read(offset,sizeof(WAVE_TYPE)*g_size);
	ff.close();

//	Make();
};

void WaveProcess::Free(void)
{
//	delete y_offset;
//	delete x_offset;
	delete[] offset;
	delete[] buff;
};

void WaveProcess::Make(void)
{
	WAVE_TYPE* buf = offset;
//	char* x_buf = x_offset;
//	char* y_buf = y_offset;
	char x_off,y_off;
	int off,tmp;
	int i,j;
	int _x_, _y_;

	for(i = 0;i < NumFrame;i++){
		off = 0;
		x_off = y_off = 0;
		for(j = 0;j < size;j++){
			off += *buf;
			tmp = off / bx;
			_x_ = off - tmp*bx - x_off;
			_y_ = tmp - y_off;
			x_off = off - tmp * bx;
			y_off = tmp;
			*(buf) = (WAVE_TYPE)(_y_) * XGR_MAXX + (WAVE_TYPE)(_x_);

			buf++;
//			x_buf++;
//			y_buf++;
		};
	};

/*	x_max = sx - 1;
	x_min = 0;
	y_max = sy - 1;
	y_min = 0;
	x_buf = x_offset;
	y_buf = y_offset;
	for(j = 0;j < NumFrame;j++){
		x_off = y_off = 0;
		for(i = 0;i < size;i++){
			x_off += *x_buf;
			y_off += *y_buf;
			if(x_off < x_min) x_min = x_off;
			if(x_off > x_max) x_max = x_off;
			if(y_off < y_min) y_min = y_off;
			if(y_off > y_max) y_max = y_off;
			x_buf++;
			y_buf++;
		};
	};*/
};

int WaveProcess::CheckOffset(int off)
{
	if(off < g_size - size) return 0;
	return 1;
};

void WaveProcess::Deform(int x,int y,int& off,char fl)
{
	int i;
	WAVE_TYPE* mask;
	uchar* buf;
	uchar* vb;
//	char* x_off;
//	char* y_off;
	short cx,cy;

	x -= sx >> 1;
	y -= sy >> 1;

	vb = XGR_VIDEOBUF + y * XGR_MAXX + x;
	buf = buff;		    
	mask = offset + off;

//	if((x + x_min) > UcutLeft && (x + x_max) < UcutRight && (y + y_min) > VcutUp && (y + y_max) < VcutDown){
	if( x > UcutLeft && (x + sx) < UcutRight && y > VcutUp && (y + sy) < VcutDown){
		for(i = 0;i < size;i++){
			vb += *mask;
			*buf = *vb;
			mask++;
			buf++;
		};
	}else{
		cx = x;
		cy = y;

//		x_off = x_offset + off;
//		y_off = y_offset + off;
		int o = 0;

	/*		for(i = 0;i < size;i++){
			cx += *x_off;
			cy += *y_off;
			vb += *mask;
			if(cx > UcutLeft && cx < UcutRight && cy > VcutUp && cy < VcutDown) *buf = *vb;
			mask++;
			buf++;
			x_off++;
			y_off++;
		};*/

		float _l_ = 1.0/XGR_MAXX;

		for(i = 0;i < size;i++){
			o += *mask;
			int _tmp_ = o*_l_;
			cx = o - _tmp_*XGR_MAXX + x;
			cy = _tmp_  + y;
			vb += *mask;
			if(cx > UcutLeft && cx < UcutRight && cy > VcutUp && cy < VcutDown) *buf = *vb;
			mask++;
			buf++;
		};
	};

	int cclx,ccly,ccrx,ccry;
	if(fl == DEFORM_WATER_ONLY) Show(x,y);
	else{
/*		cclx = VS(_video)->_clip_left;
		ccly = VS(_video)->_clip_top;
		ccrx = VS(_video)->_clip_right;
		ccry = VS(_video)->_clip_bottom;

		VS(_video)->_clip_left = UcutLeft;      
		VS(_video)->_clip_top = VcutUp;      
		VS(_video)->_clip_right = UcutRight;    
		VS(_video)->_clip_bottom = VcutDown;

		c_putspr(x,y,sx,sy,buff,HIDDEN_FON);
		VS(_video)->_clip_left = cclx;
		VS(_video)->_clip_top = ccly;
		VS(_video)->_clip_right = ccrx;
		VS(_video)->_clip_bottom = ccry;*/

		cclx = XGR_Obj.clipLeft;
		ccly = XGR_Obj.clipTop;
		ccrx = XGR_Obj.clipRight;
		ccry = XGR_Obj.clipBottom;

		XGR_SetClip(UcutLeft,VcutUp,UcutRight,VcutDown);

/*		XGR_Obj.clipLeft = UcutLeft; 
		XGR_Obj.clipTop = VcutUp;
		XGR_Obj.clipRight = UcutRight;
		XGR_Obj.clipBottom = VcutDown;*/

		XGR_PutSpr(x,y,sx,sy,buff,XGR_HIDDEN_FON | XGR_CLIPPED);

		XGR_SetClip(cclx,ccly,ccrx,ccry);
/*		XGR_Obj.clipLeft = cclx;
		XGR_Obj.clipTop = ccly;
		XGR_Obj.clipRight = ccrx;
		XGR_Obj.clipBottom = ccry;*/
	};
	off += size;
};

void WaveProcess::Show(int x,int y)
{
	int dx,dy;
	int nx,ny;
	int nxs,nys;
	int i,j;
	int vadd,dadd;
	uchar* vbuf;
	uchar* dbuf;

	if(y >= VcutDown) return;
	if(x >= UcutRight) return;
	dx = x + sx - 1;
	dy = y + sy - 1;
	nxs = sx;
	nys = sy;
	nx = x;
	ny = y;

	if(ny < VcutUp){
		if(dy <= VcutUp) return;
		else{
			ny = VcutUp;
			nys = dy - VcutUp + 1;
		};
	};
	if(nx < UcutLeft){
		if(dx <= UcutLeft) return;
		else{
			nx = UcutLeft;
			nxs = dx - UcutLeft + 1;
		};
	};

	if(dy >= VcutDown) nys = VcutDown - ny;
	if(dx >= UcutRight) nxs = UcutRight - nx;

	vadd = XGR_MAXX - nxs;
	dadd = sx - nxs;

	vbuf = XGR_VIDEOBUF + ny * XGR_MAXX + nx;
	dbuf = buff + (ny - y)*sx + (nx - x);

	for(i = 0;i < nys;i++){
		for(j = 0;j < nxs;j++){
			if(*vbuf <= ENDCOLOR[0]&&dbuf<buff+size) {
				*vbuf = WaterColorTable[*dbuf];
			}
			vbuf++;
			dbuf++;
		};
		vbuf += vadd;
		dbuf += dadd;
	};
};

//#define FIRE_TEST
void FireBallProcess::Load(char* filename)
{
//	int i,sz,cmax;
#ifdef FIRE_TEST
	int i,j;
	NumFrames = 256;
	cTable = 7;
	x_size = 32;
	y_size = 32;
	FrameSize = x_size * y_size;
	DataSize = NumFrames * FrameSize;
	Data = new uchar[DataSize];
	for(i = 0;i < NumFrames;i++)
		for(j = 0;j < FrameSize;j++) Data[i * FrameSize + j] = i;
#else
	XStream in;
	in.open(filename,XS_IN);
	in > NumFrames;
	in > cTable;
	in > x_size;
	in > y_size;
	FrameSize = x_size * y_size;
	DataSize = NumFrames * FrameSize;
	Data = new uchar[DataSize];
	in.read(Data,DataSize);
	in.close();
#endif

/*	if(cTable){
		cmax = Data[0];
		for(i = 1;i < DataSize;i++)
			if(Data[i] > cmax) cmax = Data[i];

		for(i = 0;i < DataSize;i++)
			Data[i] = (uchar)((int)(Data[i])*255/cmax);

		sz = ENDCOLOR[cTable] - BEGCOLOR[cTable];
		for(i = 0;i < DataSize;i++)
			if(Data[i] > 0) Data[i] = (uchar)(BEGCOLOR[cTable] + (((int)(Data[i]) * sz) >> 8));
	};*/
};

void FireBallProcess::Convert(char* filename)
{
	XStream in;
	in.open(filename,XS_OUT);
	in < NumFrames;
	in < (int)(0);
	in < x_size;
	in < y_size;
	in.write(Data,DataSize);
	in.close();
};

void FireBallProcess::Show(int x,int y,int z,int scale,int& frame)
{
/*	int dx,dy;
	int nx,ny;
	int nxs,nys;
	int i,j;
	int vadd,dadd;
	uchar* vbuf;
	uchar* dbuf;
	uchar c;

	if(y > VcutDown) return;
	if(x > UcutRight) return;

	dx = x + x_size;
	dy = y + y_size;
	nxs = x_size;
	nys = y_size;
	nx = x;
	ny = y;

	if(ny < VcutUp){
		if(dy <= VcutUp) return;
		else{
			ny = VcutUp;
			nys = dy - VcutUp;
		};
	};
	if(nx < UcutLeft){
		if(dx <= UcutLeft) return;
		else{
			nx = UcutLeft;
			nxs = dx - UcutLeft;
		};
	};

	if(dy >= VcutDown) nys = VcutDown - ny;
	if(dx >= UcutRight) nxs = UcutRight - nx;

	vadd = XGR_MAXX - nxs;
	dadd = x_size - nxs;

	vbuf = XGR_VIDEOBUF + ny * XGR_MAXX + nx;
	dbuf = Data + frame + (ny - y)*x_size + (nx - x);

	for(i = 0;i < nys;i++){
		for(j = 0;j < nxs;j++){
			c = (*dbuf);
		       if(c) *vbuf =FireColorTable[(*vbuf) + (c << 8)];
			vbuf++;
			dbuf++;
		};
		vbuf += vadd;
		dbuf += dadd;
	};*/

	if(cTable) 
		smart_putspr(Data + frame,x,y,x_size,y_size,x_size * scale >> 16,z,TerrainAlphaTable[cTable]);
	else 
		smart_putspr(Data + frame,x,y,x_size,y_size,x_size * scale >> 16,z,FireColorTable);
//	bitmap2screenFire((char*)(VS(_video)->_video),,x,y,x_size,y_size,x_size * scale >> 15);
};

void FireBallProcess::Show2(int x,int y,int& frame)
{
	int dx,dy;
	int nx,ny;
	int nxs,nys;
	int i,j;
	int vadd,dadd;
	uchar c;
	uchar* vbuf;
	uchar* dbuf;

	if(y > VcutDown) return;
	if(x > UcutRight) return;

	dx = x + x_size;
	dy = y + y_size;
	nxs = x_size;
	nys = y_size;
	nx = x;
	ny = y;

	if(ny < VcutUp){
		if(dy <= VcutUp) return;
		else{
			ny = VcutUp;
			nys = dy - VcutUp;
		};
	};

	if(nx < UcutLeft){
		if(dx <= UcutLeft) return;
		else{
			nx = UcutLeft;
			nxs = dx - UcutLeft;
		};
	};

	if(dy >= VcutDown) nys = VcutDown - ny;
	if(dx >= UcutRight) nxs = UcutRight - nx;

	vadd = XGR_MAXX - nxs;
	dadd = x_size - nxs;

	vbuf = XGR_VIDEOBUF + ny * XGR_MAXX + nx;
	dbuf = Data + frame + (ny - y)*x_size + (nx - x);

	for(i = 0;i < nys;i++){
		for(j = 0;j < nxs;j++){	
		       c = (*dbuf);
		       if(c) *vbuf =FireColorTable[(*vbuf) + (c << 8)];

//			if((*dbuf)) (*vbuf) = (*dbuf);
			vbuf++;
			dbuf++;
		};
		vbuf += vadd;
		dbuf += dadd;
	};
};

char FireBallProcess::CheckOut(int& f)
{
	f += FrameSize;
	if(f >= DataSize){
		f = 0;
		return 1;
	};
	return 0;
};

void FireBallProcess::Save(char* filename)
{
	uchar c;
	int n;
	int i,j;
	short xs,ys;
	XStream in,out;
	Parser list(filename);
	list.search_name("BmlName");
	out.open(list.get_name(),XS_OUT);
	list.search_name("NumBmp");
	NumFrames = list.get_int();
	out < NumFrames;

	list.search_name("ColorSpace");
	n = list.get_int();
	out < (int)(n);

	for(i = 0;i < NumFrames;i++){
		list.search_name("NameBmp");
		in.open(list.get_name(),XS_IN);
		in > xs;
		in > ys;
		if(i == 0){
			x_size = xs;
			y_size = ys;
			out < x_size;
			out < y_size;
		};

		if(xs != x_size || ys != y_size) ErrH.Abort("Bad Bitmap Size");

		for(j = 0;j < x_size*y_size;j++){
			in > c;
//			if(sz && c > 0) c = (uchar)(c1 + (((int)(c) * sz) >> 8));
/*			if(c != 0){ 
				c += 16;
				if(c >= 255) c = 255;
			};*/
			out < c;
		};
		in.close();
	};
	out.close();
};

void DrawHFLine(int x0,int y0,int x1,uchar* c,int h)
{
	uchar* vf;
	int i,j;
	int sz,d;

	if(y0 < VcutUp || y0 > VcutDown) return;
	if(x0 < UcutLeft){
		if(x1 < UcutLeft) return;
		else x0 = UcutLeft;
	};
	if(x1 > UcutRight){
		if(x0 > UcutRight) return;
		else x1 = UcutRight;
	};
	sz = x1 - x0;
	d = XGR_MAXX - sz;

	vf = XGR_VIDEOBUF + (y0 * XGR_MAXX + x0);
	for(j = 0;j < h;j++){
		for(i = 0;i < sz;i++){
			*(vf) = c[*(vf)];
			vf++;
		};
		vf += d;
	};
};

void DrawHLine(int x0,int y0,int x1,uchar* c)
{
	uchar* vf;
	int i;
	int sz;

	if(y0 < VcutUp || y0 > VcutDown - 1) return;
	if(x0 < UcutLeft){
		if(x1 < UcutLeft) return;
		else x0 = UcutLeft;
	};
	if(x1 > UcutRight - 1){
		if(x0 > UcutRight - 1) return;
		else x1 = UcutRight - 1;
	};
	sz = x1 - x0;

	vf = XGR_VIDEOBUF + (y0 * XGR_MAXX + x0);
	for(i = 0;i < sz;i++){
		*(vf) = c[*(vf)];
		vf++;
	};
};

void DrawVLine(int x0,int y0,int y1,uchar* c)
{
	uchar* vf;
	int i;
	int sz;

	if(x0 < UcutLeft || x0 > UcutRight - 1) return;
	if(y0 < VcutUp){
		if(y1 < VcutUp) return;
		else y0 = VcutUp;
	};
	if(y1 > VcutDown - 1){
		if(y0 > VcutDown - 1) return;
		else y1 = VcutDown - 1;
	};

	sz = y1 - y0;

	vf = XGR_VIDEOBUF + (y0 * XGR_MAXX + x0);
	for(i = 0;i < sz;i++){
		*(vf) = c[*(vf)];
		vf += XGR_MAXX;
	};
};

/*const int MAX_LOCATOR_RADIUS = 100;
int maxLocatorRad[MAX_LOCATOR_RADIUS + 1];
int* dLocatorRad[MAX_LOCATOR_RADIUS + 1];

const int TP_PROCESS_TIME_MAX = 10;
uchar* TpProcessFrame[TP_PROCESS_TIME_MAX];

int TeleportShowPhase = 0;
uchar EnableTeleportShow = 0;

void TeleportShowPrepare(void)
{
	const int SIDE = 2*MAX_LOCATOR_RADIUS + 1;

	short* rad = new short[SIDE*SIDE];
	int max = 0;
	int i,j,r,ind;
	short* p = rad;

	for(j = -MAX_LOCATOR_RADIUS;j <= MAX_LOCATOR_RADIUS;j++){
		for(i = -MAX_LOCATOR_RADIUS;i <= MAX_LOCATOR_RADIUS;i++,p++){
			r = (int)sqrt(i*(double)i + j*(double)j);
			if(r > MAX_LOCATOR_RADIUS) *p = -1;
			else{
				*p = r;
				maxLocatorRad[r]++;
				max++;
			};
		};
	};

	int* xheap = new int[max];
	int* yheap = new int[max];
	int* xLocatorRad[MAX_LOCATOR_RADIUS + 1];
	int* yLocatorRad[MAX_LOCATOR_RADIUS + 1];

	for(ind = 0;ind <= MAX_LOCATOR_RADIUS;ind++){
		xLocatorRad[ind] = xheap;
		yLocatorRad[ind] = yheap;
		for(p = rad,r = 0,j = -MAX_LOCATOR_RADIUS;j <= MAX_LOCATOR_RADIUS;j++)
			for(i = -MAX_LOCATOR_RADIUS;i <= MAX_LOCATOR_RADIUS;i++,p++)
				if(*p == ind){
					xheap[r] = i;
					yheap[r] = j;
					r++;
				};
		xheap += maxLocatorRad[ind];
		yheap += maxLocatorRad[ind];
	};

	int* xx,*yy,*delta;
	int px,py,dx,dy;
	for(i = 0;i < MAX_LOCATOR_RADIUS;i++){
		dLocatorRad[i] = new int[maxLocatorRad[i]];

		xx = xLocatorRad[i];
		yy = yLocatorRad[i];
		delta =  dLocatorRad[i];

		px = xx[0];
		py = yy[0];
		delta[0] = py * XGR_MAXX + px;
		for(j = 1;j < maxLocatorRad[i];j++){
			dx = xx[j] - px;
			dy = yy[j] - py;
			delta[j] = dy * XGR_MAXX + dx;
			px = xx[j];
			py = yy[j];
			xx[j] = dx;
			yy[j] = dy;
		};
	};

	delete xLocatorRad[0];
	delete yLocatorRad[0];
	delete rad;

	XStream in;
	short sx,sy;
	in.open("tp_process.bmp",XS_IN);
	in > sx > sy;

	max = sx * sy;
	if(sx != MAX_LOCATOR_RADIUS || sy != TP_PROCESS_TIME_MAX) ErrH.Abort("Bad TeleportProcess Map");
	for(i = 0;i < TP_PROCESS_TIME_MAX;i++){
		TpProcessFrame[i] = new uchar[MAX_LOCATOR_RADIUS];
		for(j = 0;j < MAX_LOCATOR_RADIUS;j++) in > TpProcessFrame[i][j];
	};
	in.close();
};

void TeleportShowFree(void)
{
	int i;
	for(i = 0;i < TP_PROCESS_TIME_MAX;i++) delete TpProcessFrame[i];
	for(i = 0;i < MAX_LOCATOR_RADIUS;i++) delete dLocatorRad[i];
};

void TeleportShowProcess(int x,int y,uchar* p)
{
	int i,j;
	int max;
	uchar* vb;
	uchar* vf;
	int* delta;
	uchar* f;

	vf = XGR_VIDEOBUF + (y * XGR_MAXX + x);
	for(i = 0;i < MAX_LOCATOR_RADIUS;i++){
		if(*p){
			max = maxLocatorRad[i];
			delta = dLocatorRad[i];
			vb = vf;
			f = FireColorTable + ((*p) << 8);
			for(j = 0;j < max;j++){
				vb += *delta;
				(*vb) = f[(*vb)];
				delta++;
			};
		};
		p++;
	};
};

extern int ScreenCX;
extern int ScreenCY;

void TeleportShowQuant(void)
{
	if(EnableTeleportShow){
		TeleportShowProcess(ScreenCX,ScreenCY,TpProcessFrame[TeleportShowPhase]);
		TeleportShowPhase++;
		if(TeleportShowPhase >= TP_PROCESS_TIME_MAX) EnableTeleportShow = 0;
	};
};

void TeleportShowStart(void)
{
	EnableTeleportShow = 1;
	TeleportShowPhase = 0;
};
*/

void ShowSigmaBmp(int x,int y,int sx,int sy,uchar* b,uchar* t)
{
	int dx,dy;
	int nx,ny;
	int nxs,nys;
	int i,j;
	int vadd,dadd;
	uchar* vbuf;
	uchar* dbuf;

	if(y > VcutDown) return;
	if(x > UcutRight) return;

	dx = x + sx;
	dy = y + sy;
	nxs = sx;
	nys = sy;
	nx = x;
	ny = y;

	if(ny < VcutUp){
		if(dy <= VcutUp) return;
		else{
			ny = VcutUp;
			nys = dy - VcutUp;
		};
	};
	if(nx < UcutLeft){
		if(dx <= UcutLeft) return;
		else{
			nx = UcutLeft;
			nxs = dx - UcutLeft;
		};
	};

	if(dy >= VcutDown) nys = VcutDown - ny;
	if(dx >= UcutRight) nxs = UcutRight - nx;

	vadd = XGR_MAXX - nxs;
	dadd = sx - nxs;

	vbuf = XGR_VIDEOBUF + ny * XGR_MAXX + nx;
	dbuf = b + (ny - y)*sx + (nx - x);

	for(i = 0;i < nys;i++){
		for(j = 0;j < nxs;j++){
			if((*dbuf)) *vbuf = t[*vbuf];
			vbuf++;
			dbuf++;
		};
		vbuf += vadd;
		dbuf += dadd;
	};
};

