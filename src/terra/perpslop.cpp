#include "../global.h"


#ifndef _VTEST_
#include "../3d/3d_math.h"
#include "vmap.h"
#else
const uint H_POWER = MAP_POWER_X;
extern uint V_POWER;
extern uint V_SIZE;
#endif

#define ROX(x) (x << (16 - H_POWER))
#define ROY(y) (y << (16 - V_POWER))

int* sTables;

extern uchar** SkipLineTable;
#ifdef _VTEST_
extern uchar** lineTcolor;
#endif

int SlopeQualityFactor = 0;		//100

int Nclips = 20;
int SlopModeTriggerAngle = 45;

int PerpMapSkipFactor0 = 100;
int PerpMapSkipFactor = 100;

int PerpScreenSkipFactor0 = 100;
int PerpScreenSkipFactor = 100;
extern int ParaMapSkipFactor;

uchar** SetSkipLineTable(uchar** lt,int ky,int Ymin,int Ymax);
void SlopTurnSkip(int Turn,int Slop,int H,int F,int cx,int cy,int xc,int yc,int XDstSize,int YDstSize);
#ifndef TURN_TEST
#define BitMap vrtMap
#endif
//Наклон изображения
int PerpSlopTurn(int Turn,int Slop,int H,int F,int cx,int cy,int xc,int yc,int XDstSize,int YDstSize)
{
	// std::cout<<"PerpSlopTurn Turn:"<<Turn<<" Slop:"<<Slop<<" H:"<<H<<" cx:"<<cx<<" cy:"<<cy<<" xc:"<<xc<<" yc:"<<yc<<" XDstSize:"<<XDstSize<<" YDstSize:"<<YDstSize<<std::endl;
	ParaMapSkipFactor =PerpScreenSkipFactor = PerpMapSkipFactor = SlopeQualityFactor;

	static int initialisation = 1;
	if(initialisation){
		sTables = new int[1024*4];
		initialisation = 0;
		}
#ifdef TURN_TEST
	Slop = -Slop;
#endif
	int sinAlpha,cosAlpha;
	int sinTetta,cosTetta;

	sinAlpha = SI[rPI(Turn)];
	cosAlpha = CO[rPI(Turn)];
	sinTetta = SI[rPI(Slop)];
	cosTetta = CO[rPI(Slop)];

	if(abs(sinAlpha) < abs(SI[SlopModeTriggerAngle*PIx2/360])){
		SlopTurnSkip(Turn,-Slop,H,F,cx,cy,xc,yc,XDstSize,YDstSize);
		return 0;
		}

#ifdef __ZORTECHC__
	char* vp = _video + (yc - YDstSize)*XGR_MAXX + (xc - XDstSize);
#else
	char* vp = (char*)XGR_VIDEOBUF + (yc - YDstSize)*XGR_MAXX + (xc - XDstSize);
#endif
	XDstSize = 2*XDstSize;
	YDstSize = 2*YDstSize;

	int CenterDistance = H;
	H = H*cosTetta >> 16;

	int Aux,Avx,Awx;
	int Auy,Avy,Awy;
	int Auz,Avz,Awz;

	Aux = cosAlpha;
	Avx = (sinAlpha >> 1)*(cosTetta >> 1) >> 14;
	Awx = (sinAlpha >> 1)*(sinTetta >> 1) >> 14;

	Auy = -sinAlpha;
	Avy = (cosAlpha >> 1)*(cosTetta >> 1) >> 14;
	Awy = (cosAlpha >> 1)*(sinTetta >> 1) >> 14;

	Auz = 0;
	Avz = -sinTetta;
	Awz = cosTetta;

	cx -= Awx*H/Awz;
	cy -= Awy*H/Awz;
	cx = (cx & clip_mask_x) << 16;
	cy = (cy & clip_mask_y) << 16;

	int y0,y1,y2,y3;
#define uv2y(u,v) (((Auy*(u) + Avy*(v) + Awy*F) >> 8)*H/((Auz*(u) + Avz*(v) + Awz*F) >> 8))
	y0 = uv2y(XDstSize/2,YDstSize/2) + (cy >> 16);
	y1 = uv2y(-XDstSize/2,YDstSize/2) + (cy >> 16);
	y2 = uv2y(XDstSize/2,-YDstSize/2) + (cy >> 16);
	y3 = uv2y(-XDstSize/2,-YDstSize/2) + (cy >> 16);

// #ifndef TURN_TEST
// #ifndef _VTEST_
// 	vMap -> request(MIN(MIN(MIN(y0,y1),y2),y3) - MAX_RADIUS/2, MAX(MAX(MAX(y0,y1),y2),y3) + MAX_RADIUS/2,0,0);
// 	uchar** lt = vMap -> lineTcolor;
// #else
// 	uchar** lt = lineTcolor;
// #endif
// #else
// 	uchar** lt = TextureDataTable;
// 	uchar** ht = HeightDataTable;
// #endif

	int z;
	z = F*Awz >> 16;
	if(!z) z = 1;
	int ky = ((PerpMapSkipFactor0 << 16) + abs(cosAlpha)*PerpMapSkipFactor)/100*CenterDistance/F;
#ifdef TURN_TEST
	::MapSkipFactor = ky;
#endif

	int i,j,fx,fy;

	for (j = 0; j < YDstSize; j++) {
		int u = -XDstSize/2;
		int v = -YDstSize/2 + j;
		int z = (v*Avz + F*Awz) >> 16;
		if(!z) z = 1;
		fx = H*((u*Aux + v*Avx + F*Awx) >> 4)/z << 4;
		fy = H*((u*Auy + v*Avy + F*Awy) >> 4)/z << 4;


		fx += cx;
		fy += cy;

#ifdef LOWLEVEL_OUTPUT
		sTables[j*4 + 0] = ROY(fy);
		sTables[j*4 + 1] = ROY(H*Auy/z);
		sTables[j*4 + 2] = ROX(fx);
		sTables[j*4 + 3] = ROX(H*Aux/z);
#else
		sTables[j*4 + 0] = fx;
		sTables[j*4 + 1] = fy;
		sTables[j*4 + 2] = H*Aux/z;
		sTables[j*4 + 3] = H*Auy/z;
#endif
	}

	int first_line = sTables[1] >> 16;
	int last_line = sTables[(YDstSize-1)*4 + 1] >> 16;
	int Ymin = MIN(MIN(MIN(MIN(MIN(y0,y1),y2),y3), first_line), last_line);
	int Ymax = MAX(MAX(MAX(MAX(MAX(y0,y1),y2),y3), first_line), last_line);
#ifndef TURN_TEST
	Ymin -= 30;
	Ymax += 30;
#endif
	vMap->request(
		Ymin - MAX_RADIUS/2,
		Ymax + MAX_RADIUS/2,
		0,
		0
	);
	uchar** lt = vMap -> lineTcolor;
	uchar** slt = SetSkipLineTable(lt, ky, Ymin - MAX_RADIUS/2, Ymax + MAX_RADIUS/2);
	int i_float = 0;
	int ScreenSkipFactor;
	ScreenSkipFactor = (PerpScreenSkipFactor0 << 16)/100;
	ScreenSkipFactor += abs(cosAlpha)*PerpScreenSkipFactor/100;
#ifdef TURN_TEST
	::ScreenSkipFactor = ScreenSkipFactor;
#endif
	int J0,J1;
	J1 = 0;
	int i_clip;
	for (i_clip = 0; i_clip < Nclips; i_clip++){
		J0 = J1;
		J1 = YDstSize*(i_clip + 1) / Nclips;
		char* vppp = vp + J0*XGR_MAXX;
		i = 0;
		int DrawPrev = 0;
		while(i < XDstSize){
			char* vpp = vppp + i - DrawPrev;
			if (DrawPrev) {
				for(j = J0; j < J1; j++){
					fx = sTables[j*4 + 0];
					fy = sTables[j*4 + 1];
					uchar* linePtr = slt[YCYCL(fy >> 16)];
					uchar tmp = linePtr == nullptr ? 0 : *(linePtr + XCYCL(fx >> 16));
					*vpp = tmp;
					vpp[1] = tmp;
					vpp += XGR_MAXX;
					fx += sTables[j*4 + 2] << 1;
					fy += sTables[j*4 + 3] << 1;
					sTables[j*4 + 0] = fx;
					sTables[j*4 + 1] = fy;
					}
			} else {
				for (j = J0; j < J1; j++) {
					fx = sTables[j*4 + 0];
					fy = sTables[j*4 + 1];
					uchar* linePtr = slt[YCYCL(fy >> 16)];
					*vpp = linePtr == nullptr ? 0 : *(linePtr + XCYCL(fx >> 16));
					vpp += XGR_MAXX;
					fx += sTables[j*4 + 2];
					fy += sTables[j*4 + 3];
					sTables[j*4 + 0] = fx;
					sTables[j*4 + 1] = fy;
				}
			}
			int tmp_i = i_float >> 16;
			i_float += ScreenSkipFactor;
			DrawPrev = (i_float >> 16) > tmp_i + 1;
			i++;
			if(DrawPrev)
				i++;
			}
		}
	return 1;
}


uchar** SetSkipLineTable(uchar** lt,int ky,int Ymin,int Ymax)
{
	Ymin = YCYCL(Ymin);
	Ymax = YCYCL(Ymax);
#ifdef TURN_TEST
	Ymin = 0;
	Ymax = V_SIZE;
#endif
	if (ky <= 1 << 16)
		return lt;
	unsigned int tmpy;
	tmpy = Ymin << 16;
	tmpy = tmpy - tmpy%ky;
	unsigned i, y;
	if (Ymin < Ymax) {
		for (i = Ymin; (int)i < Ymax; i = YCYCL(i + 1)) {
			y = YCYCL(tmpy >> 16);
			SkipLineTable[i] = lt[y];
			if (y < i) {
				tmpy += ky;
			}
		}
	} else {
		for (i = Ymin; i < V_SIZE; i++) {
			y = YCYCL(tmpy >> 16);
			SkipLineTable[i] = lt[y];
			if (y - i) {
				tmpy += ky;
			}
		}
		for (i = 0; (int)i <= Ymax; i++) {
			y = YCYCL(tmpy >> 16);
			SkipLineTable[i] = lt[y];
			if (y - i) {
				tmpy += ky;
			}
		}
	}
	return SkipLineTable;
}

