#include "global.h"

#include "terra/world.h"
#include "palette.h"
#ifdef _ROAD_
#include "sound/hsound.h"
#endif

#define CLOCK()		(SDL_GetTicks()*18/1000)

extern int DIBLog;
extern uchar* palbuf,*palbufA,*palbufC,*palbufOrg,*palbufInt,*palbufBlack;
extern int MuteLog;
extern int BackgroundSound;

#ifdef _ROAD_
PaletteTransform* palTr;

void PalEvidence(char* tpal,char* pal)
{
	const int SPEED = 4;
	int log = 1,i;
	int t;
	while(log){
		t = CLOCK();
		while(CLOCK() < t + 1);
		log = 0;
		for(i = 0;i < 256;i++){
			tpal[3*i + 0] += SPEED;
			tpal[3*i + 1] += SPEED;
			tpal[3*i + 2] += SPEED;
			if(tpal[3*i + 0] >= pal[3*i + 0]) tpal[3*i + 0] = pal[3*i + 0]; else log = 1;
			if(tpal[3*i + 1] >= pal[3*i + 1]) tpal[3*i + 1] = pal[3*i + 1]; else log = 1;
			if(tpal[3*i + 2] >= pal[3*i + 2]) tpal[3*i + 2] = pal[3*i + 2]; else log = 1;
			}
		XGR_SetPal(tpal,0,255);
		XGR_Flush(0,0,XGR_MAXX,XGR_MAXY);
		xtClearMessageQueue();
		}
}

void PalSlake(char* pal)
{
	const int SPEED = 4;

	int log = 1,i;
	int t;
	while(log){
		t = CLOCK();
		while(CLOCK() < t + 1);
		log = 0;
		for(i = 0;i < 256;i++){
			if(pal[3*i + 0] <= SPEED) pal[3*i + 0] = 0; else pal[3*i + 0] -= SPEED, log = 1;
			if(pal[3*i + 1] <= SPEED) pal[3*i + 1] = 0; else pal[3*i + 1] -= SPEED, log = 1;
			if(pal[3*i + 2] <= SPEED) pal[3*i + 2] = 0; else pal[3*i + 2] -= SPEED, log = 1;
			}
		XGR_SetPal(pal,0,255);
		XGR_Flush(0,0,XGR_MAXX,XGR_MAXY);
		xtClearMessageQueue();
		}
}

void PaletteTransform::set(uchar* _from,uchar* _to,int _n0,int _sz,int* _event)
{
	n0 = _n0;
	sz = _sz;
	event = _event;
	if(!_from)
		memset(from + 3*n0,0,3*sz);
	else
		memcpy(from + 3*n0,_from + 3*n0,3*sz);
	if(!_to)
		memset(to + 3*n0,0,3*sz);
	else
		memcpy(to + 3*n0,_to + 3*n0,3*sz);
	memset(delta + 3*n0,0,3*sz);
	int i,j;
	for(i = 0,j = 3*n0;i < 3*sz;i++,j++){
		if(to[j] > from[j]) delta[j] = 2 + ((int)to[j] - (int)from[j])/32;
		else if(to[j] < from[j]) delta[j] = -2 - ((int)from[j] - (int)to[j])/32;
		}
	inProgress = 1;
}

int PaletteTransform::quant(void)
{
	if(!inProgress) return 0;

	uchar* pf = from + 3*n0;
	uchar* pt = to + 3*n0;
	char* pd = delta + 3*n0;
	int max = 3*sz;
	int log = 0;
	while(max--){
		if(*pd){
			log = 1;
			if(abs((int)*pf - (int)*pt) <= abs(*pd))
				*pf = *pt, *pd = 0;
			else
				*pf += *pd;
			}
		pf++; pt++; pd++;
		}
	if(!log){ inProgress = 0; if(event) *event = 0; }
	XGR_SetPal(from,n0,sz);

	if(DIBLog) XGR_Flush(0,0,XGR_MAXX,XGR_MAXY);
	return 1;
}
#endif

void pal_iter0(void)
{
	if(PAL_WAVE_TERRAIN == -1 || PAL_WAVE_TERRAIN >= TERRAIN_MAX) return;
	const int DSIZE = 8;
	const int PRD = 64;
	const int DATA[DSIZE] = { 1,3,5,6,5,3,2,1 };
	const int BEG = BEGCOLOR[PAL_WAVE_TERRAIN];
	const int SZ = ENDCOLOR[PAL_WAVE_TERRAIN] - BEG;

	static int off = 0;
	static int cnt = PRD;
	static int add = 1;

	register int i;
	uchar* p = palbuf + 3*(BEG + 1);
	uchar* po = palbufOrg + 3*(BEG + 1);
	memcpy(p,po,3*SZ);
	if(off > 0) p += 3*off, po += 3*off;
	uchar* p0 = p;

	for(i = 0;i < DSIZE;i++)
		if(off + i >= 0 && off + i < SZ){
			*p += DATA[i];
			if(*p > 63) *p = 63;
			p++;
			*p += DATA[i];
			if(*p > 63) *p = 63;
			p++;
			*p += DATA[i];
			if(*p > 63) *p = 63;
			p++;
			}
	if(--cnt) off = off + add;
	else {
		cnt = PRD;
		add = realRND(2) ? 1 : -1;
		if(add > 0) off = -DSIZE;
		else off = SZ - 1;
		}

#ifdef _ROAD_
	if(MuteLog) return;
	static int timer = 10 + realRND(100);
	if(!--timer){
		SOUND_GLOBAL();
		timer = 20 + realRND(100);
		}
	if(BackgroundSound) StartEFFECT(EFF_GLOBAL,1);
#endif
}

void pal_iter1(void)
{
	if(PAL_WAVE_TERRAIN == -1 || PAL_WAVE_TERRAIN >= TERRAIN_MAX) return;
	const int DSIZE = 10;
	const int PRD = 150;
	const int DATA[DSIZE] = { 1,3,5,7,10,8,6,4,2,1 };
	const int BEG = BEGCOLOR[PAL_WAVE_TERRAIN];
	const int SZ = ENDCOLOR[PAL_WAVE_TERRAIN] - BEG;

	static int off = 0;
	static int cnt = PRD;
	static int add = 1;

	register int i;
	uchar* p = palbuf + 3*(BEGCOLOR[PAL_WAVE_TERRAIN] + 1);
	uchar* po = palbufOrg + 3*(BEGCOLOR[PAL_WAVE_TERRAIN] + 1);
	memcpy(p,po,3*SZ);
	if(off > 0) p += 3*off, po += 3*off;
	uchar* p0 = p;

	for(i = 0;i < DSIZE;i++)
		if(off + i >= 0 && off + i < SZ){
			*p += DATA[i];
			if(*p > 63) *p = 63;
			p++;
			*p += DATA[i];
			if(*p > 63) *p = 63;
			p++;
			*p += DATA[i];
			if(*p > 63) *p = 63;
			p++;
			}
	if(--cnt) off = off + add;
	else {
		cnt = PRD;
		add = realRND(2) ? 1 : -1;
		if(add > 0) off = -DSIZE;
		else off = SZ - 1;
		}
}

int PAL_MAX;
int PAL_WAVE_TERRAIN;
int PAL_TERRAIN[TERRAIN_MAX];
int PAL_SPEED[TERRAIN_MAX];
int PAL_AMPL[TERRAIN_MAX];
int PAL_RED[TERRAIN_MAX];
int PAL_GREEN[TERRAIN_MAX];
int PAL_BLUE[TERRAIN_MAX];

static int offset[TERRAIN_MAX];

void pal_iter2(void)
{
	int add,i,j;
	uchar *p,*po;
	for(int ind = 0;ind < PAL_MAX;ind++){
		offset[ind] = rPI(offset[ind] + PAL_SPEED[ind]);
		add = PAL_AMPL[ind]*SI[offset[ind]]/UNIT;
		p = palbuf + 3*BEGCOLOR[PAL_TERRAIN[ind]];
		po = palbufOrg + 3*BEGCOLOR[PAL_TERRAIN[ind]];
		for(i = BEGCOLOR[PAL_TERRAIN[ind]];i <= ENDCOLOR[PAL_TERRAIN[ind]];i++){
			if(PAL_RED[ind]){
				j = *po + add; if(j > 63) j = 63; else if(j < 0) j = 0;
				*p++ = j; po++;
				}
			else *p++ = *po++;
			if(PAL_GREEN[ind]){
				j = *po + add; if(j > 63) j = 63; else if(j < 0) j = 0;
				*p++ = j; po++;
				}
			else *p++ = *po++;
			if(PAL_BLUE[ind]){
				j = *po + add; if(j > 63) j = 63; else if(j < 0) j = 0;
				*p++ = j; po++;
				}
			else *p++ = *po++;
			}
		}
}

void pal_iter_init(void)
{
	PAL_MAX = 0;
	PAL_WAVE_TERRAIN = -1;
	for(int i = 0;i < TERRAIN_MAX;i++) offset[i] = 0;
}

void pal_iter(void)
{
	pal_iter0();
	pal_iter1();
	pal_iter2();
#ifdef TERRAIN16
	XGR_SetPal(palbuf,0,256);
#else
	XGR_SetPal(palbuf,0,128);
#endif
}
