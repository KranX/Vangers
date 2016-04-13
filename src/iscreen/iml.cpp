#include "../global.h"

#include "iworld.h"
#include "ivmap.h"
#include "iml.h"
#include "../rle.h"
#include "../actint/mlconsts.h"

#define FLUSH_DELTA	48

/* ----------------------------- EXTERN SECTION ---------------------------- */
extern int idOS;
extern int aciML_ToolzerRadius;
extern int uvsCurrentWorldUnable;
/* --------------------------- PROTOTYPE SECTION --------------------------- */
void MLverify(XStream& ff);

int aciGetCurCycle(void);
int getWID(void);
/* --------------------------- DEFINITION SECTION -------------------------- */
const int IML_MAXTABLE = 32;

const int ACTIVATED = 1;

static iMobileLocation* imlTable[IML_MAXTABLE];
static int imlChannel[IML_MAXTABLE];
static int imlMode[IML_MAXTABLE];
static int imlQueue[IML_MAXTABLE];
static int imlPauseMode[IML_MAXTABLE];
static int imlLocked[IML_MAXTABLE];

static int ML_FRAME_SIZE = 0;
static uchar *ML_FRAME_DELTA = NULL;
static uchar *ML_FRAME_TERRAIN = NULL;

static inline uchar GET_UP_ALT(uchar* type,int h,uchar* pa0,int x)
{
	if(IS_DOUBLE(*type)){
		if(x & 1)
			h = *(pa0 + x);
		else
			h = *(pa0 + x + 1);
		}
	return h;
}

static inline uchar GET_DOWN_ALT(uchar* type,int h,uchar* pa0,int x)
{
	if(IS_DOUBLE(*type)) if(x & 1) h = *(pa0 + x - 1);
	return h;
}

static inline uchar GET_REAL_TERRAIN(uchar* type,int x)
{
	if(x & 1) return GET_TERRAIN(*type);
	else if(*type & DOUBLE_LEVEL) return GET_TERRAIN(*(type + 1));
	return GET_TERRAIN(*type);
}

static inline int SET_REAL_TERRAIN(uchar* type,uchar newtype,int x)
{
	if(*type & DOUBLE_LEVEL){
		if(x & 1){ SET_TERRAIN(*type,newtype); return 1; }
		}
	else {
		SET_TERRAIN(*type,newtype);
		return 1;
		}
	return 0;
}

static inline void SET_UP_ALT(uchar* type,uchar h,uchar* pa0,int x)
{
	if(*type & DOUBLE_LEVEL){ if(x & 1) *(pa0 + x) = h; }
	else *(pa0 + x) = h;
}

iMobileLocation::~iMobileLocation(void)
{
	for(int i = 0;i < maxFrame;i++){
		if(table[i].signBits) {
			delete[] table[i].signBits;
		}
		if(table[i].delta) {
			delete table[i].delta;
		}
		if(table[i].terrain) {
			delete table[i].terrain;
		}
		if(table[i].c_delta) {
			delete[] table[i].c_delta;
		}
		if(table[i].c_terrain) {
			delete[] table[i].c_terrain;
		}
	}
	if(alt) {
		delete[] alt;
	}
	if(table) {
		delete[] table;
	}
	if(steps) {
		delete[] steps;
	}
}

void iMobileLocation::load(char* fname)
{
	XStream ff(fname,XS_IN);

	MLverify(ff);
	ff.read(name,MLNAMELEN + 1);
	int i;
	ff > maxFrame > DryTerrain > Impulse;
	ff.seek(1,XS_CUR);
	uchar t; ff > t; mode = t;
	ff.seek(2,XS_CUR);
	for(i = 1;i < MAX_KEYPHASE;i++) ff > KeyPhase[i];
	ff.seek(4,XS_CUR);

	table = new iMLFrame[maxFrame];
	memset(steps = new int[maxFrame],0,maxFrame*sizeof(int));
	int isalt = 0;
	for(i = 0;i < maxFrame;i++){
		table[i].load(ff,mode);
		if(table[i].sx > altSx) altSx = table[i].sx;
		if(table[i].sy > altSy) altSy = table[i].sy;
		if(table[i].period > 1) isalt = 1;
		}
	ff.close();
	if(isalt) alt = new MLAtype[altSx*altSy];
	calcBounds();

	goPh = -1;
	firstUse = 1;
}

void iMobileLocation::calcBounds(void)
{
	x0 = table[0].x0;
	y0 = table[0].y0;
	x1 = x0 + table[0].sx - 1;
	y1 = y0 + table[0].sy - 1;
	for(int i = 1;i < maxFrame;i++){
		if(x0 > table[i].x0) x0 = table[i].x0;
		if(y0 > table[i].y0) y0 = table[i].y0;
		if(table[i].x0 + table[i].sx - 1 > x1) x1 = table[i].x0 + table[i].sx - 1;
		if(table[i].y0 + table[i].sy - 1 > y1) y1 = table[i].y0 + table[i].sy - 1;
		}
}

void iMLFrame::load(XStream& ff,int mode)
{
	ff > x0 > y0 > sx > sy > period > surfType;
	ff > csd > cst;
	ff.seek(2*4,XS_CUR);
	sz = sx*sy;

	if(sz > ML_FRAME_SIZE)	ML_FRAME_SIZE = sz + 2;

	c_terrain = c_delta = NULL;

	if (!csd){
		delta = new uchar[sz + 2];
		ff.read(delta,sz);
#ifdef ML_COMPRESS_ON
		RLE_ANALISE(delta,sz,c_delta);
		delete delta; delta = NULL;
#endif
		}
	else {
		delta = NULL;
		c_delta = new uchar[csd];
		ff.read(c_delta,csd);
		}

	if(surfType >= TERRAIN_MAX){
		if (!cst){
			terrain = new uchar[sz + 2];
			ff.read(terrain,sz);
#ifdef ML_COMPRESS_ON
			RLE_ANALISE(terrain,sz,c_terrain);
			delete terrain; terrain = NULL;
#endif
			}
		else {
			terrain = NULL;
			c_terrain = new uchar[cst];
			ff.read(c_terrain,cst);
			}
		}
	if(mode == MLM_RELATIVE){
		ss = sz/32 + 1;
		signBits = new unsigned[ss];
		ff.read(signBits,ss*4);
		}
}

int iMobileLocation::quant(int render)
{
	if(getCurPhase() == goPh && !goSkip) return 0;

/*	  if(render){
		if(skipFrameFlag){
			skipFrameFlag = 0;
			return 1;
		}
		else
			skipFrameFlag = 1;
	}*/

	if(mode == MLM_RELATIVE){
		if(table[cFrame].quant(steps[cFrame],alt,DryTerrain,render,fastMode)){
			goSkip = 0;
			if(++cFrame == maxFrame)
				steps[cFrame = 0] = 0;
			if(!render) return 1;
			table[cFrame].quant(steps[cFrame],alt,DryTerrain,render,fastMode);
			}
		}
	else
		if(table[cFrame].quantAbs(steps[cFrame],alt,DryTerrain,render,fastMode)){
			goSkip = 0;
			if(++cFrame == maxFrame)
				steps[cFrame = 0] = 0;
			if(!render) return 1;
			table[cFrame].quantAbs(steps[cFrame],alt,DryTerrain,render,fastMode);
			}
	return 1;
}

int iMLFrame::quant(int& step,MLAtype* alt,int dry,int render,int fastMode)
{
	if(!step && period > 1 && !fastMode) start(alt);
	step++;
	if(step > period || (step > 1 && fastMode)){
		step = 0;
		return 1;
		}
	if(fastMode == 2) return 0;

	uchar** lt = ivMap -> lineT;
	uchar* pa0,*pf0;
#ifdef ML_COMPRESS_ON
	uchar* pd = ML_FRAME_DELTA;
#else
	uchar* pd = delta;
#endif
	unsigned* sb = signBits;
	unsigned mask = 1;
	MLAtype* pt = alt;
	int dl;

#ifdef ML_COMPRESS_ON
	RLE_UNCODE(pd, sz, c_delta);
#else
	if (csd){
		pd = ML_FRAME_DELTA;
		RLE_UNCODE(pd, sz, c_delta);
		}
#endif

	int i,j,x,y,v;
	uchar* pr,t;
	if(step == period || fastMode){
#ifdef ML_COMPRESS_ON
		if(c_terrain){
			pr = ML_FRAME_TERRAIN;
			RLE_UNCODE(pr,sz,c_terrain);
			}
		else pr = NULL;
#else
		if(cst){
			pr = ML_FRAME_TERRAIN;
			RLE_UNCODE(pr, sz, c_terrain);
			}
		pr = terrain;
#endif
		}
	else pr =  NULL;

	int sft = (step == period || fastMode) ? surfType : -1;
	if(period > 1 && !fastMode)
		if(!pr)
			if(sft == -1){
				for(j = 0,y = iYCYCL(y0);j < sy;j++,y = iYCYCL(y + 1)){
					pa0 = lt[y];
					pf0 = pa0 + H_SIZE;
					for(i = 0,x = iXCYCL(x0);i < sx;i++,x = iXCYCL(x + 1),pd++,pt++){
						dl = *pd;
						if(dl){
							v = (int)*(pa0 + x) - (*pt >> MLPREC);
							if(*sb & mask) dl = -dl;
							v += ((*pt += ((dl << MLPREC)/period)) >> MLPREC);
							if(v < 1){ v = 0; *pt = 0; }
							SET_UP_ALT(pf0 + x,v,pa0,x);
							}
						if(!(mask <<= 1)){ sb++; mask = 1; }
						}
					}
				}
			else {
				int type = sft << TERRAIN_OFFSET;
				for(j = 0,y = iYCYCL(y0);j < sy;j++,y = iYCYCL(y + 1)){
					pa0 = lt[y];
					pf0 = pa0 + H_SIZE;
					for(i = 0,x = iXCYCL(x0);i < sx;i++,x = iXCYCL(x + 1),pd++,pt++){
						dl = *pd;
						if(dl){
							v = (int)*(pa0 + x) - (*pt >> MLPREC);
							if(*sb & mask) dl = -dl;
							v += ((*pt += ((dl << MLPREC)/period)) >> MLPREC);
							if(v < 1){ v = 0; *pt = 0; }
							SET_UP_ALT(pf0 + x,v,pa0,x);
							t = *(pf0 + x) & TERRAIN_MASK;
							if(t != CHTYPE0 && t != CHTYPE1) SET_REAL_TERRAIN(pf0 + x,type,x);
							}
						if(!(mask <<= 1)){ sb++; mask = 1; }
						}
					}
				}
		else
			for(j = 0,y = iYCYCL(y0);j < sy;j++,y = iYCYCL(y + 1)){
				pa0 = lt[y];
				pf0 = pa0 + H_SIZE;
				for(i = 0,x = iXCYCL(x0);i < sx;i++,x = iXCYCL(x + 1),pd++,pt++,pr++){
					dl = *pd;
					if(dl){
						v = (int)*(pa0 + x) - (*pt >> MLPREC);
						if(*sb & mask) dl = -dl;
						v += ((*pt += ((dl << MLPREC)/period)) >> MLPREC);
						if(v < 1){ v = 0; *pt = 0; }
						SET_UP_ALT(pf0 + x,v,pa0,x);
						t = *(pf0 + x) & TERRAIN_MASK;
						if(t != CHTYPE0 && t != CHTYPE1) SET_REAL_TERRAIN(pf0 + x,*pr,x);
						}
					if(!(mask <<= 1)){ sb++; mask = 1; }
					}
				}
	else
		if(!pr)
			if(sft == -1){
				for(j = 0,y = iYCYCL(y0);j < sy;j++,y = iYCYCL(y + 1)){
					pa0 = lt[y];
					pf0 = pa0 + H_SIZE;
					for(i = 0,x = iXCYCL(x0);i < sx;i++,x = iXCYCL(x + 1),pd++){
						dl = *pd;
						if(dl){
							if(*sb & mask) dl = -dl;
							v = (int)*(pa0 + x) + dl;
							if(v < 0) v = 0;
							SET_UP_ALT(pf0 + x,v,pa0,x);
							}
						if(!(mask <<= 1)){ sb++; mask = 1; }
						}
					}
				}
			else {
				int type = sft << TERRAIN_OFFSET;
				for(j = 0,y = iYCYCL(y0);j < sy;j++,y = iYCYCL(y + 1)){
					pa0 = lt[y];
					pf0 = pa0 + H_SIZE;
					for(i = 0,x = iXCYCL(x0);i < sx;i++,x = iXCYCL(x + 1),pd++){
						dl = *pd;
						if(dl){
							if(*sb & mask) dl = -dl;
							v = (int)*(pa0 + x) + dl;
							if(v < 0) v = 0;
							SET_UP_ALT(pf0 + x,v,pa0,x);
							t = *(pf0 + x) & TERRAIN_MASK;
							if(t != CHTYPE0 && t != CHTYPE1) SET_REAL_TERRAIN(pf0 + x,type,x);
							}
						if(!(mask <<= 1)){ sb++; mask = 1; }
						}
					}
				}
		else
			for(j = 0,y = iYCYCL(y0);j < sy;j++,y = iYCYCL(y + 1)){
				pa0 = lt[y];
				pf0 = pa0 + H_SIZE;
				for(i = 0,x = iXCYCL(x0);i < sx;i++,x = iXCYCL(x + 1),pd++,pr++){
					dl = *pd;
					if(dl){
						if(*sb & mask) dl = -dl;
						v = (int)*(pa0 + x) + dl;
						if(v < 0) v = 0;
						SET_UP_ALT(pf0 + x,v,pa0,x);
						t = *(pf0 + x) & TERRAIN_MASK;
						if(t != CHTYPE0 && t != CHTYPE1) SET_REAL_TERRAIN(pf0 + x,*pr,x);
						}
					if(!(mask <<= 1)){ sb++; mask = 1; }
					}
				}

	if(render){
//		iregSet(x0 + dx,y0 + dy,x0 + dx + sx - 1,y0 + dy + sy - 1,dry << TERRAIN_OFFSET,0);
		Render();
		SetFlushIML(x0 - FLUSH_DELTA,y0,sx + FLUSH_DELTA,sy);
		}

	return 0;
}

int iMLFrame::quantAbs(int& step,MLAtype* alt,int dry,int render,int fastMode)
{
	step++;
	if(step > 1){
		step = 0;
		return 1;
		}
	if(fastMode == 2) return 0;

	uchar** lt = ivMap -> lineT;
	uchar* pa0,*pf0,t;
#ifdef ML_COMPRESS_ON
	uchar* pd = ML_FRAME_DELTA;
#else
	uchar* pd = delta;
#endif

#ifdef ML_COMPRESS_ON
	RLE_UNCODE(pd, sz, c_delta);
#else
	if (csd){
		pd = ML_FRAME_DELTA;
		RLE_UNCODE(pd, sz, c_delta);
		}
#endif

	int i,j,x,y;
	uchar* pr;
	if(step == 1 || fastMode) {
#ifdef ML_COMPRESS_ON
		if ( c_terrain ){
			pr = ML_FRAME_TERRAIN;
			RLE_UNCODE(pr, sz, c_terrain);
			}
		else pr = NULL;
#else
		if(cst){
			pr = ML_FRAME_TERRAIN;
			RLE_UNCODE(pr, sz, c_terrain);
			}
		pr = terrain;
#endif
		}
	else pr =  NULL;

	int sft = (step == 1 || fastMode) ? surfType : -1;
	if(!pr)
		if(sft == -1){
			for(j = 0,y = iYCYCL(y0);j < sy;j++,y = iYCYCL(y + 1)){
				pa0 = lt[y];
				pf0 = pa0 + H_SIZE;
				for(i = 0,x = iXCYCL(x0);i < sx;i++,x = iXCYCL(x + 1),pd++){
					if(*pd) *(pa0 + x) = *pd;
					}
				}
			}
		else {
			int type = sft << TERRAIN_OFFSET;
			for(j = 0,y = iYCYCL(y0);j < sy;j++,y = iYCYCL(y + 1)){
				pa0 = lt[y];
				pf0 = pa0 + H_SIZE;
				for(i = 0,x = iXCYCL(x0);i < sx;i++,x = iXCYCL(x + 1),pd++){
					if(*pd){
						*(pa0 + x) = *pd;
						t = *(pf0 + x) & TERRAIN_MASK;
						if(t != CHTYPE0 && t != CHTYPE1) SET_TERRAIN(*(pf0 + x),type);
						}
					}
				}
			}
	else
		for(j = 0,y = iYCYCL(y0);j < sy;j++,y = iYCYCL(y + 1)){
			pa0 = lt[y];
			pf0 = pa0 + H_SIZE;
			for(i = 0,x = iXCYCL(x0);i < sx;i++,x = iXCYCL(x + 1),pd++,pr++){
				if(*pd){
					*(pa0 + x) = *pd;
					t = *(pf0 + x) & TERRAIN_MASK;
					if(t != CHTYPE0 && t != CHTYPE1) SET_TERRAIN(*(pf0 + x),*pr);
					}
				}
			}

	if(render){
//		iregSet(x0,y0,x0 + sx - 1,y0 + sy - 1,dry << TERRAIN_OFFSET,0);
		Render();
		SetFlushIML(x0 - FLUSH_DELTA,y0,sx + FLUSH_DELTA,sy);
		}

	return 0;
}

void iMLFrame::start(MLAtype* alt)
{
	uchar** lt = ivMap -> lineT;
	uchar* pa0;
	MLAtype* pt = alt;

	int i,j,x,y;
	for(j = 0,y = y0;j < sy;j++,y = iYCYCL(y + 1)){
		pa0 = lt[y];
		for(i = 0,x = x0;i < sx;i++,x = iXCYCL(x + 1),pt++)
			*pt = (((MLAtype)*(pa0 + x)) << MLPREC) + (1 << (MLPREC - 1));
		}
}

void iMobileLocation::setNull(void)
{
	cFrame = 0;
	goSkip = 0;
	goPh = -1;
	for(int i = 0;i < maxFrame;i++) steps[i] = 0;
}

void iMobileLocation::setPhase(int nPhase)
{
	if(nPhase < 0 || nPhase > maxFrame - 1) return;
//	if(cFrame == nPhase && !table[cFrame].step) return;
	int val = goPh;
	goPh = -1;
	int log = 1;
	int f = cFrame;
	while(log && steps[f]) log = quant(0);
	if(f != maxFrame - 1){
		fastMode = 1;
		while(log && (cFrame != nPhase || steps[cFrame])) log = quant(0);
		fastMode = 0;
		}
	if(log){
		iregRender(x0,y0,x1,y1);
		SetFlushIML(x0 - FLUSH_DELTA,y0,x1 - x0 + 1 + FLUSH_DELTA,y1 - y0 + 1);
		}
	goPh = val;
}

void RegisterIML(char* fname,int id,int channel)
{
	if(id < 0 || id >= IML_MAXTABLE) ErrH.Abort("Wrong IML object ID");
	(imlTable[id] = new iMobileLocation) -> load(fname);
	imlChannel[id] = channel;
	imlMode[id] = 0;
	imlQueue[id] = -1;
	imlLocked[id] = 0;
	imlPauseMode[id] = 0;
}

void ReleaseIML(void)
{
	for(int i = 0;i < IML_MAXTABLE;i++) if(imlTable[i]){ delete imlTable[i]; imlTable[i] = NULL; }
	delete[] ML_FRAME_DELTA;
	ML_FRAME_DELTA = NULL;
	delete[] ML_FRAME_TERRAIN;
	ML_FRAME_TERRAIN = NULL;
}

void HandlerIML(int code,int id,int val)
{
	int i;

	if(!imlLocked[id]){
		switch(code){
			case AML_GO_INFINITE:
				imlMode[id] = ACTIVATED;
				imlTable[id] -> goPhase(-1);
				break;
			case AML_GO_PHASE:
				imlMode[id] = ACTIVATED;
				imlTable[id] -> goPhase(val);
				break;
			case AML_PUT_IN_QUEUE:
				imlQueue[id] = val;
				imlTable[id] -> goPhase(0);
				break;
			case AML_STOP:
				imlMode[id] = 0;
				break;
			case AML_SET_PHASE:
				imlTable[id] -> setPhase(val);
				break;
			case AML_REWIND:
				imlTable[id] -> setNull();
				break;
			}
		}

	switch(code){
		case AML_PUT_IN_CH_QUEUE:
			for(i = 0;i < IML_MAXTABLE;i++)
				if(imlChannel[i] == id)
					if(imlTable[i] && imlMode[i] == ACTIVATED && !imlLocked[i]){
						imlQueue[i] = val;
						imlTable[i] -> goPhase(0);
						return;
						}
			if(imlTable[val] && !imlLocked[val]){
				imlMode[val] = ACTIVATED;
				imlTable[val] -> goPhase(0);
				}
			break;
		case AML_STOP_CHANNEL:
			for(i = 0;i < IML_MAXTABLE;i++)
				if(imlTable[i] && imlMode[i] == ACTIVATED && imlChannel[i] == id && !imlLocked[i])
					imlMode[i] = 0;
			break;
		}
}

int QuantIML(void)
{
	int ret = 0,log;
	for(int i = 0;i < IML_MAXTABLE;i++)
		if(imlTable[i])
			if(imlMode[i] == ACTIVATED){
				ret += (log = imlTable[i] -> quant(1));
				if(!log){
					imlMode[i] = 0;
					if(imlQueue[i] != -1){
						imlMode[imlQueue[i]] = ACTIVATED;
						imlTable[imlQueue[i]] -> goPhase(-1);
						imlQueue[i] = -1;
						}
					}
				}
	return ret;
}

int isInQueueIML(int ch_id,int ml_id)
{
	for(int i = 0;i < IML_MAXTABLE;i++)
		if(imlChannel[i] == ch_id && imlQueue[i] == ml_id) return 1;
	return 0;
}

void CleanQueueIML(int ch_id)
{
	for(int i = 0;i < IML_MAXTABLE;i++)
		if(imlChannel[i] == ch_id) imlQueue[i] = -1;
}

int CheckIML(int id,int x,int y,int frame)
{
	iMobileLocation* m = imlTable[id];
	if(!frame){
		if(x > m -> x0 && x < m -> x1 && y > m -> y0 && y < m -> y1) return 1;
		}
	else {
		iMLFrame* f = m -> table + m -> cFrame;
		if(x > f -> x0 && x < f -> x0 + f -> sx && y > f -> y0 && y < f -> y0 + f -> sy) return 1;
		}
	return 0;
}

void PauseIMLall(void)
{
	for(int i = 0;i < IML_MAXTABLE;i++){
		imlPauseMode[i] = imlMode[i];
		imlMode[i] = 0;
		}
}

void ResumeIMLall(void)
{
	for(int i = 0;i < IML_MAXTABLE;i++)
		imlMode[i] = imlPauseMode[i];
}

int GetPhaseIML(int id){ return imlTable[id] -> getCurPhase(); }
int isActiveIML(int id){ return imlMode[id]; }
void LockIML(int id){ imlLocked[id] = 1; imlMode[id] = 0; }
void UnlockIML(int id){ imlLocked[id] = 0; }
int isLockedIML(int id){ return imlLocked[id]; }

static int* xRad[MAX_RADIUS + 1];
static int* yRad[MAX_RADIUS + 1];
static int maxRad[MAX_RADIUS + 1];

int ActiveTerrain[TERRAIN_MAX] = { 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,0 };
static int MiniLevel,MaxiLevel = 255;
static int CurrentTerrain;

void ilandPrepare(void)
{
	const int SIDE = 2*MAX_RADIUS + 1;

	short* rad = new short[SIDE*SIDE];
	int max = 0;

	int calc = 1;
	int i,j,r,ind;
	short* p = rad;
	if(calc){
		for(j = -MAX_RADIUS;j <= MAX_RADIUS;j++)
			for(i = -MAX_RADIUS;i <= MAX_RADIUS;i++,p++){
				r = (int)sqrt(i*(double)i + j*(double)j);
				if(r > MAX_RADIUS) *p = -1;
				else {
					*p = r;
					maxRad[r]++;
					max++;
					}
				}
		}

	int* xheap = new int[max];
	int* yheap = new int[max];
	for(ind = 0;ind <= MAX_RADIUS;ind++){
		xRad[ind] = xheap;
		yRad[ind] = yheap;
		for(p = rad,r = 0,j = -MAX_RADIUS;j <= MAX_RADIUS;j++)
			for(i = -MAX_RADIUS;i <= MAX_RADIUS;i++,p++)
				if(*p == ind){
					xheap[r] = i;
					yheap[r] = j;
					r++;
					}
		xheap += maxRad[ind];
		yheap += maxRad[ind];
		}
	delete[] rad;
}

static void pixSet(int x,int y,int delta)
{
	if(!delta) return;

	uchar** lt = ivMap -> lineT;
	uchar* pa = lt[y];
	if(!pa || x >= XGR_MAXX) return;

	uchar* pa0 = pa;
	pa += x;
	uchar* pf = pa + H_SIZE;

	int h = GET_UP_ALT(pf,*pa,pa0,x),xx;
	if(!ActiveTerrain[GET_REAL_TERRAIN(pf,x)]) return;
	if(h < MiniLevel || h > MaxiLevel) return;

	if(IS_DOUBLE(*pf))
		if(x & 1){
			h += delta;
			if((int)*(pa - 1) + (((GET_DELTA(*(pf - 1)) << 2) + GET_DELTA(*pf) + 1) << DELTA_SHIFT) >= h){
				*pf &= ~DOUBLE_LEVEL;
				*(pf - 1) &= ~DOUBLE_LEVEL;
				if(CurrentTerrain != -1){
					SET_TERRAIN(*pf,CurrentTerrain << TERRAIN_OFFSET);
					SET_TERRAIN(*(pf - 1),CurrentTerrain << TERRAIN_OFFSET);
					}
				else
					SET_TERRAIN(*pf,GET_TERRAIN_TYPE(*(pf - 1)));
				xx = iXCYCL(x + 1);
				h = *pa = (*(pa - 1) + GET_DOWN_ALT(pa0 + xx + H_SIZE,*(pa0 + xx),pa0,xx)) >> 1;
				SET_DELTA(*pf,0);
				SET_DELTA(*(pf - 1),0);
				}
			}
		else
			return;
	else
		h += delta;

	if(h < 0) h = 0;
	else if(h > 255) h = 255;
	if(h < MiniLevel) h = MiniLevel;
	if(h > MaxiLevel) h = MaxiLevel;

	*pa = h;

	if(CurrentTerrain != -1) SET_REAL_TERRAIN(pf,CurrentTerrain << TERRAIN_OFFSET,x);
}

static void deltaZone(int x,int y,int rad,int smth,int dh,int smode,int eql)
{
	static int locp;

	int i,j;
	int max;
	int* xx,*yy;

	int r = rad - rad*smth/10;
	double d = 1.0/(rad - r + 1),dd,ds,s;
	int v,h,k,mean;

	if(dh){
		for(i = 0;i <= r;i++){
			max = maxRad[i];
			xx = xRad[i];
			yy = yRad[i];
			for(j = 0;j < max;j++)
				pixSet(iXCYCL(x + xx[j]),iYCYCL(y + yy[j]),dh);
			}

		for(i = r + 1,dd = 1.0 - d;i <= rad;i++,dd -= d){
			max = maxRad[i];
			xx = xRad[i];
			yy = yRad[i];
			h = (int)(dd*dh);
			if(!h) h = dh > 0 ? 1 : -1;

			switch(smode){
				case 0:
					v = (int)(dd*max);
					ds = (double)v/(double)max;
					for(s = ds,k = 0,j = locp%max;k < max;j = (j + 1 == max) ? 0 : j + 1,k++,s += ds)
						if(s >= 1.0){
							pixSet(iXCYCL(x + xx[j]),iYCYCL(y + yy[j]),h);
							s -= 1.0;
							}
					break;
				case 1:
					v = (int)(dd*1000000.0);
					for(j = 0;j < max;j++)
						if((int)realRND(1000000) < v) pixSet(iXCYCL(x + xx[j]),iYCYCL(y + yy[j]),h);
					break;
				case 2:
					v = (int)(dd*max);
					for(k = 0,j = locp%max;k < v;j = (j + 1 == max) ? 0 : j + 1,k++)
						pixSet(iXCYCL(x + xx[j]),iYCYCL(y + yy[j]),h);
					locp += max;
					break;
				}
			}
		}
	else {
		uchar** lt = ivMap -> lineT;
		uchar* pa,*pa0,*pa_,*pa0_;
		int cx,h,cy,cx_;
		if(eql){
			mean = k = 0;
			for(i = 0;i <= r;i++){
				max = maxRad[i];
				xx = xRad[i];
				yy = yRad[i];
				for(j = 0;j < max;j++){
					pa0 = pa = lt[iYCYCL(y + yy[j])];
					if(pa){
						pa += (cx = iXCYCL(x + xx[j]));
						mean += GET_UP_ALT(pa + H_SIZE,*pa,pa0,cx);
						k++;
						}
					}
				}
			if(k) mean /= k;
			for(i = 0;i <= r;i++){
				max = maxRad[i];
				xx = xRad[i];
				yy = yRad[i];
				for(j = 0;j < max;j++){
					pa0 = pa = lt[cy = iYCYCL(y + yy[j])];
					if(pa){
						pa += (cx = iXCYCL(x + xx[j]));
						h = GET_UP_ALT(pa + H_SIZE,*pa,pa0,cx);
						if(abs(h - mean) < eql) {
							if(h > mean) {
								pixSet(cx, cy, -1);
							} else if(h < mean) {
								pixSet(cx, cy, 1);
							}
						}
					}
				}
			}
			for(i = r + 1,dd = 1.0 - d;i <= rad;i++,dd -= d){
				max = maxRad[i];
				xx = xRad[i];
				yy = yRad[i];
				h = (int)(dd*dh);
				if(!h) h = dh > 0 ? 1 : -1;

				v = (int)(dd*max);
				ds = (double)v/(double)max;
				for(s = ds,k = 0,j = locp%max;k < max;j = (j + 1 == max) ? 0 : j + 1,k++,s += ds)
					if(s >= 1.0){
						pa0 = pa = lt[cy = iYCYCL(y + yy[j])];
						if(pa){
							pa += (cx = iXCYCL(x + xx[j]));
							h = GET_UP_ALT(pa + H_SIZE,*pa,pa0,cx);
							if(abs(h - mean) < eql) {
								if(h > mean) {
									pixSet(cx, cy, -1);
								} else if(h < mean) {
									pixSet(cx, cy, 1);
								}
							}
						}
						s -= 1.0;
					}
				}
			}
		else {
			int dx,dy;
			for(i = 0;i <= r;i++){
				max = maxRad[i];
				xx = xRad[i];
				yy = yRad[i];
				for(j = 0;j < max;j++){
					pa0 = pa = lt[cy = iYCYCL(y + yy[j])];
					if(pa){
						pa += (cx = iXCYCL(x + xx[j]));
						h = GET_UP_ALT(pa + H_SIZE,*pa,pa0,cx);
						v = 0;
						switch(smode){
							case 0:
								for(dy = -1;dy <= 1;dy++)
									for(dx = -1;dx <= 1;dx++){
										pa0_ = pa_ = lt[iYCYCL(cy + dy)];
										if(pa_){
											pa_ += (cx_ = iXCYCL(cx + dx));
											v += GET_UP_ALT(pa_ + H_SIZE,*pa_,pa0_,cx_);
											}
										}
								v -= h;
								v >>= 3;
								break;
							case 1:
							case 2:
								for(dy = -1;dy <= 1;dy++)
									for(dx = -1;dx <= 1;dx++){
										pa0_ = pa_ = lt[iYCYCL(cy + dy)];
										if(pa_){
											pa_ += (cx_ = iXCYCL(cx + dx));
											if(abs(dx) + abs(dy) == 2)
												v += GET_UP_ALT(pa_ + H_SIZE,*pa_,pa0_,cx_);
											}
										}
								v >>= 2;
								break;
							}
						pixSet(cx,cy,v - h);
						}
					}
				}
			for(i = r + 1,dd = 1.0 - d;i <= rad;i++,dd -= d){
				max = maxRad[i];
				xx = xRad[i];
				yy = yRad[i];
				v = (int)(dd*max);
				ds = (double)v/(double)max;
				for(s = ds,k = 0,j = realRND(max);k < max;j = (j + 1)%max,k++,s += ds)
					if(s >= 1.0){
						pa0 = pa = lt[cy = iYCYCL(y + yy[j])];
						if(pa){
							pa += (cx = iXCYCL(x + xx[j]));
							h = GET_UP_ALT(pa + H_SIZE,*pa,pa0,cx);
							v = 0;
							switch(smode){
								case 0:
									for(dy = -1;dy <= 1;dy++)
										for(dx = -2;dx <= 2;dx+=2){
											pa0_ = pa_ = lt[iYCYCL(cy + dy)];
											if(pa_){
												pa_ += (cx_ = iXCYCL(cx + dx));
												v += GET_UP_ALT(pa_ + H_SIZE,*pa_,pa0_,cx_);
												}
											}
									v -= h;
									v >>= 3;
									break;
								case 1:
								case 2:
									for(dy = -1;dy <= 1;dy++)
										for(dx = -1;dx <= 1;dx++){
											pa0_ = pa_ = lt[iYCYCL(cy + dy)];
											if(pa_){
												pa_ += (cx_ = iXCYCL(cx + dx));
												if(abs(dx) + abs(dy) == 2)
													v += GET_UP_ALT(pa_ + H_SIZE,*pa_,pa0_,cx_);
												}
											}
									v >>= 2;
									break;
								}
							pixSet(cx,cy,v - h);
							}
						s -= 1.0;
						}
				}
			}
		}
	locp++;
	iregRender(x - rad,y - rad,x + rad,y + rad);
	int sx = 2*rad + 1;
	int sy = sx;
	x -= rad; y -= rad;
	if(y < 0){ sy += y; y = 0; }
	else if(y + sy >= XGR_MAXY) sy -= y + sy - XGR_MAXY + 1;
	SetFlushIML(x - FLUSH_DELTA,y,sx + FLUSH_DELTA,sy);
}

void ToolzerQuant(int x,int y)
{
	y += 16;
	int period = !uvsCurrentWorldUnable ? aciGetCurCycle() : 3;
	if(getWID() == 3) period = 3;
	switch(period){
		case 0:
			ActiveTerrain[CHTYPEINDEX0] = 0;
			ActiveTerrain[CHTYPEINDEX1] = 1;
			ActiveTerrain[7] = 0;
			CurrentTerrain = CHTYPEINDEX0;
			deltaZone(x,y,32,10,1 - realRND(3),realRND(3),realRND(8) ? 0 : realRND(10));
			aciML_ToolzerRadius = 32;
			break;
		case 1:
			ActiveTerrain[CHTYPEINDEX0] = 1;
			ActiveTerrain[CHTYPEINDEX1] = 1;
			ActiveTerrain[7] = 0;
			CurrentTerrain = -1;
			deltaZone(x,y,40,10,0,0,0);
			aciML_ToolzerRadius = 40;
			break;
		case 2:
			ActiveTerrain[CHTYPEINDEX0] = 1;
			ActiveTerrain[CHTYPEINDEX1] = 0;
			ActiveTerrain[7] = 0;
			CurrentTerrain = CHTYPEINDEX1;
			deltaZone(x,y,32,10,1 - realRND(3),realRND(3),realRND(8) ? 0 : realRND(10));
			aciML_ToolzerRadius = 32;
			break;
		case 3:
			ActiveTerrain[CHTYPEINDEX0] = 1;
			ActiveTerrain[CHTYPEINDEX1] = 1;
			ActiveTerrain[7] = 1;
			CurrentTerrain = -1;
			deltaZone(x,y,45,10,1 + realRND(4),0,0);
			aciML_ToolzerRadius = 45;
			break;
		}
}

void StartupIML(void)
{
	for(int i = 0;i < IML_MAXTABLE;i++){
		imlChannel[i] = 0;
		imlMode[i] = 0;
		imlQueue[i] = -1;
		imlPauseMode[i] = 0;
		imlLocked[i] = 0;
		}
}

void InitIML(void)
{
	ML_FRAME_DELTA = new uchar[ML_FRAME_SIZE];
	ML_FRAME_TERRAIN = new uchar[ML_FRAME_SIZE];
}
