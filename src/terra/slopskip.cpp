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

uchar** SetSkipLineTable(uchar** lt,int ky,int Ymin,int Ymax);
int ParaMapSkipFactor0 = 100;
int ParaMapSkipFactor = 100;

#ifdef _VTEST_
extern uchar** lineTcolor;
#endif

int ParaScreenSkipFactor0 = 100;
int ParaScreenSkipFactor = 0;

#ifndef TURN_TEST
#define BitMap vrtMap
#endif

inline int pre_calc_fy(int i, int XDstSize, int YDstSize, int Awy, int Awz, int Auy, int Avy, int Avz, int F, int H, int cy) {
	int u = -XDstSize/2;
	int v = -YDstSize/2 + i;
	int z = (v*Avz + F*Awz) >> 16;
	if(!z) z = 1;
	int fy = H*((u*Auy + v*Avy + F*Awy) >> 4)/z << 4;
	int k_xscr_y = H*Auy/z;

	fy += cy;
	return (fy + k_xscr_y*i) >> 16;
}

void SlopTurnSkip(int Turn,int Slop,int H,int F,int cx,int cy,int xc,int yc,int XDstSize,int YDstSize)
{
	Slop = -Slop;
	int sinAlpha,cosAlpha;
	int sinTetta,cosTetta;
#ifdef __ZORTECHC__
	char* vp = _video + (yc - YDstSize)*XGR_MAXX + (xc - XDstSize);
#else
	char* vp = (char*)XGR_VIDEOBUF + (yc - YDstSize)*XGR_MAXX + (xc - XDstSize);
#endif
	XDstSize = 2*XDstSize;
	YDstSize = 2*YDstSize;

	sinAlpha = SI[rPI(Turn)];
	cosAlpha = CO[rPI(Turn)];
	sinTetta = SI[rPI(Slop)];
	cosTetta = CO[rPI(Slop)];

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

//#ifndef TURN_TEST
// #ifndef _VTEST_
// 	vMap -> request(MIN(MIN(MIN(y0,y1),y2),y3) - MAX_RADIUS/2, MAX(MAX(MAX(y0,y1),y2),y3) + MAX_RADIUS/2, 0, 0);
// 	uchar** lt = vMap -> lineTcolor;
// #else
// 	uchar** lt = lineTcolor;
// #endif
// #else
// 	uchar** lt = TextureDataTable;
// 	uchar** ht = HeightDataTable;
// #endif
	int first_line = pre_calc_fy(0, XDstSize, YDstSize, Awy, Awz, Auy, Avy, Avz, F, H, cy);
	int last_line = pre_calc_fy(YDstSize - 1, XDstSize, YDstSize, Awy, Awz, Auy, Avy, Avz, F, H, cy);
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

	int z;
	z = F*Awz >> 16;
	if(!z) z = 1;
	int ky = ((ParaMapSkipFactor0 << 16) + abs(sinAlpha)*ParaMapSkipFactor)/100*CenterDistance/F;
	uchar** slt = SetSkipLineTable(lt, ky, Ymin - MAX_RADIUS/2, Ymax + MAX_RADIUS/2);
#ifdef TURN_TEST
	::MapSkipFactor = ky;
#endif
	int k_xscr_x,k_xscr_y;
	int i,fx,fy;
	i = 0;
	int i_float = 0;
	int ScreenSkipFactor;
	ScreenSkipFactor = (ParaScreenSkipFactor0 << 16)/100;
	ScreenSkipFactor += abs(sinAlpha)*ParaScreenSkipFactor/100;
#ifdef TURN_TEST
	::ScreenSkipFactor = ScreenSkipFactor;
#endif
	int DrawPrev = 0;
	while (i < YDstSize) {
		char* vpp = vp + XGR_MAXX*i;
		int u = -XDstSize/2;
		int v = -YDstSize/2 + i;
		int z = (v*Avz + F*Awz) >> 16;
		if(!z) z = 1;
		fx = H*((u*Aux + v*Avx + F*Awx) >> 4)/z << 4;
		fy = H*((u*Auy + v*Avy + F*Awy) >> 4)/z << 4;

		k_xscr_x = H*Aux/z;
		k_xscr_y = H*Auy/z;

		fx += cx;
		fy += cy;

		if (DrawPrev) {
			k_xscr_x *= 2;
			k_xscr_y *= 2;
			for (int j = 0; j < XDstSize; j += 2) {
				uchar* linePtr = slt[YCYCL(fy >> 16)];
				uchar tmp = linePtr == nullptr ? 0 : *(linePtr + XCYCL(fx >> 16));
				*vpp++ = tmp;
				*vpp++ = tmp;
				fx += k_xscr_x;
				fy += k_xscr_y;
			}
		} else {
			for (int j = 0; j < XDstSize; j++) {
				uchar* linePtr = slt[YCYCL(fy >> 16)];
				*vpp++ = linePtr == nullptr ? 0 : *(linePtr + XCYCL(fx >> 16));
				fx += k_xscr_x;
				fy += k_xscr_y;
			}
		}
		int tmp_i = i_float >> 16;
		i_float += ScreenSkipFactor;
		DrawPrev = (i_float >> 16) > tmp_i + 1;
		i++;
	}
}
