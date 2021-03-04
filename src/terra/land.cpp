#include "../global.h"


#include "../3d/3d_math.h"
#include "../3d/3dgraph.h"

#include "vmap.h"
#include "world.h"
#include "render.h"
#ifdef _SURMAP_
#include "surmap/tools.h"
#endif
const int _COL1 = 224 + 15;
const int _COL2 = 224 + 10;

#ifdef _SURMAP_
#define DEBUG_SHOW
#endif

/* ----------------------------- EXTERN SECTION ---------------------------- */
extern int RenderingLayer;
extern int ExclusiveLayerMode;
#ifdef _SURMAP_
extern int MosaicToggle;
#endif
/* --------------------------- PROTOTYPE SECTION --------------------------- */
void GeoPoint(int x,int y,int level,int delta,int mode);
void _regRender(int icLowX,int icLowY,int icHiX,int icHiY,int changed);
/* --------------------------- DEFINITION SECTION -------------------------- */
int MESH		 = 200;
int TunnelProof = 16;

static int* xRad[MAX_RADIUS + 1];
static int* yRad[MAX_RADIUS + 1];
static int maxRad[MAX_RADIUS + 1];

uchar BEGCOLOR[TERRAIN_MAX];
uchar ENDCOLOR[TERRAIN_MAX];

uchar POWER;
uchar QUANT;
uint WPART_POWER;
uint part_map_size_y;
uint part_map_size;
int MINSQUARE;

#ifndef TERRAIN16
const int MATERIAL_MAX = 2;
const int TERRAIN_MATERIAL[TERRAIN_MAX] = { 1,0,0,0,0,0,0,0 };
const double TERRAIN_DXKOEF[MATERIAL_MAX] = { 1.0, 5.0 };
const double TERRAIN_SDKOEF[MATERIAL_MAX] = { 1.0, 1.25 };
const double TERRAIN_JJKOEF[MATERIAL_MAX] = { 1.0, 0.5 };
#else
const int MATERIAL_MAX = 1;
const int TERRAIN_MATERIAL[TERRAIN_MAX] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
const double TERRAIN_DXKOEF[MATERIAL_MAX] = { 1.0 };
const double TERRAIN_SDKOEF[MATERIAL_MAX] = { 1.0 };
const double TERRAIN_JJKOEF[MATERIAL_MAX] = { 1.0 };
#endif

uchar lightCLRmaterial[MATERIAL_MAX][CLR_MAX];
uchar* lightCLR[TERRAIN_MAX];
uchar palCLR[TERRAIN_MAX][2*256];

uchar FloodLEVEL;

double zMod_cycle = 0;
double zMod_flood_level_delta = 0;

uchar* shadowParent;
#ifndef TERRAIN16
static uchar* waterBuf[3];
#endif

int LandBounded = 1;
int VisiRegR = 0;

void RenderPrepare(void)
{
	int i;
	int j,ind,v;
	if(!shadowParent){
		memset(shadowParent = new uchar[4*map_size_x],0,4*map_size_x);
#ifndef TERRAIN16
		uchar* p = new uchar[3*map_size_x];
		memset(p,0,3*map_size_x);
		for(i = 0;i < 3;i++,p += map_size_x) waterBuf[i] = p;
#endif
		}

	FloodLEVEL = FloodLvl[0];

	const double dx = 8.0;
	const double dx2 = dx*dx;
	const double sd = 256.0/(double)SHADOWDEEP;
	double jj,DX,SD;
	int colnum;
	for(ind = 0;ind < MATERIAL_MAX;ind++){
		DX = TERRAIN_DXKOEF[ind]*dx;
		SD = TERRAIN_SDKOEF[ind]*sd;
		for(j = -CLR_MAX_SIDE;j <= CLR_MAX_SIDE;j++){
			jj = TERRAIN_JJKOEF[ind]*(double)j;
			v = round(255.0*(DX*SD - jj)/sqrt((1.0 + SD*SD)*(DX*DX + jj*jj)));
			if(v < 0) v = 0;
			lightCLRmaterial[ind][CLR_MAX_SIDE + j] = v;
			}
		}
	for(ind = 0;ind < TERRAIN_MAX;ind++){
		lightCLR[ind] = lightCLRmaterial[TERRAIN_MATERIAL[ind]];
		memset(palCLR[ind],BEGCOLOR[ind],256);
		colnum = ENDCOLOR[ind] - BEGCOLOR[ind];
#ifndef TERRAIN16
		if(!ind){
			int d = (255 - FloodLEVEL) >> H_CORRECTION;
			memset(palCLR[ind] + 2*256 - d,ENDCOLOR[ind],d);
			for(j = 0;j < 256 - d;j++){
				v = round((double)j*1.25*(double)colnum/(255.0 - (double)d) - 0.25*(double)colnum);
				if(v < 0) v = 0;
				palCLR[ind][256 + j] = BEGCOLOR[ind] + v;
				}
			}
		else 
#endif
			for(j = 0;j < 256;j++)
				palCLR[ind][256 + j] = BEGCOLOR[ind] + round((double)j*(double)colnum/255.0);
		}
}

#ifdef _SURMAP_
void landPrepare(void)
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
#endif

void regRenderSimple(int x0,int y0,int x1,int y1)
{
	regRender(x0,y0,x1,y1);
}

#ifdef _SURMAP_
void deltaZone(int x,int y,int rad,int smth,int dh,int smode,int eql)
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
				pixSet((x + xx[j]) & clip_mask_x,(y + yy[j]) & clip_mask_y,dh);
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
							pixSet((x + xx[j]) & clip_mask_x,(y + yy[j]) & clip_mask_y,h);
							s -= 1.0;
							}
					break;
				case 1:
					v = (int)(dd*1000000.0);
					for(j = 0;j < max;j++)
						if((int)realRND(1000000) < v) pixSet((x + xx[j]) & clip_mask_x,(y + yy[j]) & clip_mask_y,h);
					break;
				case 2:
					v = (int)(dd*max);
					for(k = 0,j = locp%max;k < v;j = (j + 1 == max) ? 0 : j + 1,k++)
						pixSet((x + xx[j]) & clip_mask_x,(y + yy[j]) & clip_mask_y,h);
					locp += max;
					break;
				}
			}
		locp++;
		}
	else {
		if(RenderingLayer == UP_LAYER){
			uchar** lt = vMap -> lineT;
			uchar* pa,*pa0,*pa_,*pa0_;
			int cx,h,cy,cx_;
			if(eql){
				mean = k = 0;
				for(i = 0;i <= r;i++){
					max = maxRad[i];
					xx = xRad[i];
					yy = yRad[i];
					for(j = 0;j < max;j++){
						pa0 = pa = lt[YCYCL(y + yy[j])];
						if(pa){
							pa += (cx = XCYCL(x + xx[j]));
							mean += GET_UP_ALT(pa + H_SIZE,*pa,pa0,cx);
							}
						}
					k += max;
					}
				mean /= k;
				for(i = 0;i <= r;i++){
					max = maxRad[i];
					xx = xRad[i];
					yy = yRad[i];
					for(j = 0;j < max;j++){
						pa0 = pa = lt[cy = YCYCL(y + yy[j])];
						if(pa){
							pa += (cx = XCYCL(x + xx[j]));
							h = GET_UP_ALT(pa + H_SIZE,*pa,pa0,cx);
							if(abs(h - mean) < eql)
								if(h > mean) pixSet(cx,cy,-1);
								else if(h < mean) pixSet(cx,cy,1);
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
							pa0 = pa = lt[cy = YCYCL(y + yy[j])];
							if(pa){
								pa += (cx = XCYCL(x + xx[j]));
								h = GET_UP_ALT(pa + H_SIZE,*pa,pa0,cx);
								if(abs(h - mean) < eql)
									if(h > mean) pixSet(cx,cy,-1);
									else if(h < mean) pixSet(cx,cy,1);
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
						pa0 = pa = lt[cy = YCYCL(y + yy[j])];
						if(pa){
							pa += (cx = XCYCL(x + xx[j]));
							h = GET_UP_ALT(pa + H_SIZE,*pa,pa0,cx);
							v = 0;
							switch(smode){
								case 0:
									for(dy = -1;dy <= 1;dy++)
										for(dx = -1;dx <= 1;dx++){
											pa0_ = pa_ = lt[YCYCL(cy + dy)];
											if(pa_){
												pa_ += (cx_ = XCYCL(cx + dx));
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
											pa0_ = pa_ = lt[YCYCL(cy + dy)];
											if(pa_){
												pa_ += (cx_ = XCYCL(cx + dx));
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
					h = (int)(dd*dh);
					if(!h) h = dh > 0 ? 1 : -1;

					v = (int)(dd*max);
					ds = (double)v/(double)max;
					for(s = ds,k = 0,j = locp%max;k < max;j = (j + 1 == max) ? 0 : j + 1,k++,s += ds)
						if(s >= 1.0){
							pa0 = pa = lt[cy = YCYCL(y + yy[j])];
							if(pa){
								pa += (cx = XCYCL(x + xx[j]));
								h = GET_UP_ALT(pa + H_SIZE,*pa,pa0,cx);
								v = 0;
								switch(smode){
									case 0:
										for(dy = -1;dy <= 1;dy++)
											for(dx = -1;dx <= 1;dx++){
												pa0_ = pa_ = lt[YCYCL(cy + dy)];
												if(pa_){
													pa_ += (cx_ = XCYCL(cx + dx));
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
												pa0_ = pa_ = lt[YCYCL(cy + dy)];
												if(pa_){
													pa_ += (cx_ = XCYCL(cx + dx));
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
		else {
			uchar** lt = vMap -> lineT;
			uchar* pa,*pa0,*pa_,*pa0_;
			int cx,h,cy,cx_;
			if(eql){
				mean = k = 0;
				for(i = 0;i <= r;i++){
					max = maxRad[i];
					xx = xRad[i];
					yy = yRad[i];
					for(j = 0;j < max;j++){
						pa0 = pa = lt[YCYCL(y + yy[j])];
						if(pa){
							pa += (cx = XCYCL(x + xx[j]));
							mean += GET_DOWN_ALT(pa + H_SIZE,*pa,pa0,cx);
							}
						}
					k += max;
					}
				mean /= k;
				for(i = 0;i <= r;i++){
					max = maxRad[i];
					xx = xRad[i];
					yy = yRad[i];
					for(j = 0;j < max;j++){
						pa0 = pa = lt[cy = YCYCL(y + yy[j])];
						if(pa){
							pa += (cx = XCYCL(x + xx[j]));
							h = GET_DOWN_ALT(pa + H_SIZE,*pa,pa0,cx);
							if(abs(h - mean) < eql)
								if(h > mean) pixSet(cx,cy,-1);
								else if(h < mean) pixSet(cx,cy,1);
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
							pa0 = pa = lt[cy = YCYCL(y + yy[j])];
							if(pa){
								pa += (cx = XCYCL(x + xx[j]));
								h = GET_DOWN_ALT(pa + H_SIZE,*pa,pa0,cx);
								if(abs(h - mean) < eql)
									if(h > mean) pixSet(cx,cy,-1);
									else if(h < mean) pixSet(cx,cy,1);
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
						pa0 = pa = lt[cy = YCYCL(y + yy[j])];
						if(pa){
							pa += (cx = XCYCL(x + xx[j]));
							h = GET_DOWN_ALT(pa + H_SIZE,*pa,pa0,cx);
							v = 0;
							switch(smode){
								case 0:
									for(dy = -1;dy <= 1;dy++)
										for(dx = -1;dx <= 1;dx++){
											pa0_ = pa_ = lt[YCYCL(cy + dy)];
											if(pa_){
												pa_ += (cx_ = XCYCL(cx + dx));
												v += GET_DOWN_ALT(pa_ + H_SIZE,*pa_,pa0_,cx_);
												}
											}
									v -= h;
									v >>= 3;
									break;
								case 1:
								case 2:
									for(dy = -1;dy <= 1;dy++)
										for(dx = -1;dx <= 1;dx++){
											pa0_ = pa_ = lt[YCYCL(cy + dy)];
											if(pa_){
												pa_ += (cx_ = XCYCL(cx + dx));
												if(abs(dx) + abs(dy) == 2)
													v += GET_DOWN_ALT(pa_ + H_SIZE,*pa_,pa0_,cx_);
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
					h = (int)(dd*dh);
					if(!h) h = dh > 0 ? 1 : -1;

					v = (int)(dd*max);
					ds = (double)v/(double)max;
					for(s = ds,k = 0,j = locp%max;k < max;j = (j + 1 == max) ? 0 : j + 1,k++,s += ds)
						if(s >= 1.0){
							pa0 = pa = lt[cy = YCYCL(y + yy[j])];
							if(pa){
								pa += (cx = XCYCL(x + xx[j]));
								h = GET_DOWN_ALT(pa + H_SIZE,*pa,pa0,cx);
								v = 0;
								switch(smode){
									case 0:
										for(dy = -1;dy <= 1;dy++)
											for(dx = -1;dx <= 1;dx++){
												pa0_ = pa_ = lt[YCYCL(cy + dy)];
												if(pa_){
													pa_ += (cx_ = XCYCL(cx + dx));
													v += GET_DOWN_ALT(pa_ + H_SIZE,*pa_,pa0_,cx_);
													}
												}
										v -= h;
										v >>= 3;
										break;
									case 1:
									case 2:
										for(dy = -1;dy <= 1;dy++)
											for(dx = -1;dx <= 1;dx++){
												pa0_ = pa_ = lt[YCYCL(cy + dy)];
												if(pa_){
													pa_ += (cx_ = XCYCL(cx + dx));
													if(abs(dx) + abs(dy) == 2)
														v += GET_DOWN_ALT(pa_ + H_SIZE,*pa_,pa0_,cx_);
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
		}
	regRender(x - rad,y - rad,x + rad,y + rad);
}

void GeoSetZone(int x,int y,int rad,int level,int delta)
{
	int i,j;
	int max;
	int* xx,*yy;

	GeoPoint(0,0,0,0,1);

	for(i = 0;i <= rad;i++){
		max = maxRad[i];
		xx = xRad[i];
		yy = yRad[i];
		for(j = 0;j < max;j++)
			GeoPoint((x + xx[j]) & clip_mask_x,(y + yy[j]) & clip_mask_y,level,delta,0);
		}

	GeoPoint(0,0,0,0,2);
}
#endif

void pixSet(int x,int y,int delta,int surf)
{
	if(!delta) return;
#ifdef _SURMAP_
	if(RenderingLayer == DOWN_LAYER){
		pixDownSet(x,y,delta,surf);
		return;
		}
#endif

	uchar** lt = vMap -> lineT;
	uchar* pa = lt[y];
	if(!pa){
#ifdef _SURMAP_
		vMap -> increase(y,y);
		pa = lt[y];
#else
		return;
#endif
		}
	
	uchar* pa0 = pa;
	pa += x;
	uchar* pf = pa + H_SIZE;

	int h = GET_UP_ALT(pf,*pa,pa0,x),xx;
#ifdef _SURMAP_
	if(surf) if(!ActiveTerrain[GET_REAL_TERRAIN(pf,x)]) return;
	if(h < MiniLevel || h > MaxiLevel) return;
	if(MosaicToggle && mosaicBMP.mode <= 1) delta = mosaicBMP.getDelta(x,y,delta);
#endif
	
	if(IS_DOUBLE(*pf))
		if(x & 1){
			h += delta;
			if((int)*(pa - 1) + (((GET_DELTA(*(pf - 1)) << 2) + GET_DELTA(*pf) + 1) << DELTA_SHIFT) >= h){
				*pf &= ~DOUBLE_LEVEL;
				*(pf - 1) &= ~DOUBLE_LEVEL;
#ifdef _SURMAP_
				if(surf && CurrentTerrain != -1){
					SET_TERRAIN(*pf,CurrentTerrain << TERRAIN_OFFSET);
					SET_TERRAIN(*(pf - 1),CurrentTerrain << TERRAIN_OFFSET);
					}
				else
#endif
					SET_TERRAIN(*pf,GET_TERRAIN_TYPE(*(pf - 1)));
				xx = XCYCL(x + 1);
				h = *pa = (*(pa - 1) + GET_DOWN_ALT(pa0 + xx + H_SIZE,*(pa0 + xx),pa0,xx)) >> 1;
				SET_DELTA(*pf,0);
				SET_DELTA(*(pf - 1),0);
//				*(vMap -> lineTcolor[y] + x - 1) = COL1;
//				*(vMap -> lineTcolor[y] + x) = COL1;
				}
			}
		else
			return;
	else
#ifdef _SURMAP_
		if(ExclusiveLayerMode) return; else
#endif
		h += delta;

#ifdef _SURMAP_
	if(LandBounded){
#endif
		if(h < 0) h = 0;
		else if(h > 255) h = 255;
#ifdef _SURMAP_
		}
	else {
		if(h < 0) h += 256;
		else if(h > 255) h -= 256;
		}
	if(h < MiniLevel) h = MiniLevel;
	if(h > MaxiLevel) h = MaxiLevel;
#endif
//	SET_UP_ALT(pf,h,pa0,x);
	*pa = h;
//	*(vMap -> lineTcolor[y] + x) = COL1;

#ifdef _SURMAP_
	if(MosaicToggle && mosaicBMP.mode == 2){
		int t = mosaicBMP.getType(x,y);
		if(t >= 0) SET_REAL_TERRAIN(pf,(t%TERRAIN_MAX) << TERRAIN_OFFSET,x);
		}
	else if(surf) if(CurrentTerrain != -1) SET_REAL_TERRAIN(pf,CurrentTerrain << TERRAIN_OFFSET,x);
#endif

#ifndef TERRAIN16
	if(h < FloodLvl[y >> WPART_POWER]){
		uchar* pfN = pa0 + H_SIZE;
		int xx = XCYCL(x - 1);
		if(GET_REAL_TERRAIN_TYPE(pfN + xx,xx) == WATER_TERRAIN){ SET_REAL_TERRAIN(pf,WATER_TERRAIN,x); return; }
		xx = XCYCL(x + 1);
		if(GET_REAL_TERRAIN_TYPE(pfN + xx,xx) == WATER_TERRAIN){ SET_REAL_TERRAIN(pf,WATER_TERRAIN,x); return; }
		int yy = YCYCL(y - 1);
		pfN = lt[yy];
		if(pfN){
			pfN += H_SIZE + x;
			if(GET_REAL_TERRAIN_TYPE(pfN,x) == WATER_TERRAIN){ SET_REAL_TERRAIN(pf,WATER_TERRAIN,x); return; }
			}
		yy = YCYCL(y + 1);
		pfN = lt[yy];
		if(pfN){
			pfN += H_SIZE + x;
			if(GET_REAL_TERRAIN_TYPE(pfN,x) == WATER_TERRAIN){ SET_REAL_TERRAIN(pf,WATER_TERRAIN,x); return; }
			}
		}
	else {
		if(GET_REAL_TERRAIN_TYPE(pf,x) == WATER_TERRAIN)
			SET_REAL_TERRAIN(pf,MAIN_TERRAIN,x);
		}
#endif
}

//Отрисовка следа от колёс!!!
void pixSetR(int x,int y,int delta,int surf)
{
//std::cout<<"pixSetR"<<std::endl;
	if(!delta) return;

	uchar** lt = vMap -> lineT;
	uchar* pa = lt[y];
// Render
	uchar* pc = vMap -> lineTcolor[y];
	uchar* pc0=pc;
	pc += x;
//
	if(!pa){
		return;
		}
	
	uchar* pa0 = pa;
	pa += x;
	uchar* pf = pa + H_SIZE;

	int h = GET_UP_ALT(pf,*pa,pa0,x),xx;
	
	if(IS_DOUBLE(*pf))
		if(x & 1){
			h += delta;
			if((int)*(pa - 1) + (((GET_DELTA(*(pf - 1)) << 2) + GET_DELTA(*pf) + 1) << DELTA_SHIFT) >= h){
				*pf &= ~DOUBLE_LEVEL;
				*(pf - 1) &= ~DOUBLE_LEVEL;
					SET_TERRAIN(*pf,GET_TERRAIN_TYPE(*(pf - 1)));
				xx = XCYCL(x + 1);
				h = *pa = (*(pa - 1) + GET_DOWN_ALT(pa0 + xx + H_SIZE,*(pa0 + xx),pa0,xx)) >> 1;
				SET_DELTA(*pf,0);
				SET_DELTA(*(pf - 1),0);
//				*(vMap -> lineTcolor[y] + x - 1) = COL1;
//				*(vMap -> lineTcolor[y] + x) = COL1;
				}
			}
		else
			return;
	else
		h += delta;

		if(h < 0) h = 0;
		else if(h > 255) h = 255;
//	SET_UP_ALT(pf,h,pa0,x);
	*pa = h;
//	*(vMap -> lineTcolor[y] + x) = COL1;


#ifndef TERRAIN16
	if(h < FloodLvl[y >> WPART_POWER]){
		uchar* pfN = pa0 + H_SIZE;
		int xx = XCYCL(x - 1);
		if(GET_REAL_TERRAIN_TYPE(pfN + xx,xx) == WATER_TERRAIN){ SET_REAL_TERRAIN(pf,WATER_TERRAIN,x); return; }
		xx = XCYCL(x + 1);
		if(GET_REAL_TERRAIN_TYPE(pfN + xx,xx) == WATER_TERRAIN){ SET_REAL_TERRAIN(pf,WATER_TERRAIN,x); return; }
		int yy = YCYCL(y - 1);
		pfN = lt[yy];
		if(pfN){
			pfN += H_SIZE + x;
			if(GET_REAL_TERRAIN_TYPE(pfN,x) == WATER_TERRAIN){ SET_REAL_TERRAIN(pf,WATER_TERRAIN,x); return; }
			}
		yy = YCYCL(y + 1);
		pfN = lt[yy];
		if(pfN){
			pfN += H_SIZE + x;
			if(GET_REAL_TERRAIN_TYPE(pfN,x) == WATER_TERRAIN){ SET_REAL_TERRAIN(pf,WATER_TERRAIN,x); return; }
			}
		}
	else {
		if(GET_REAL_TERRAIN_TYPE(pf,x) == WATER_TERRAIN)
			SET_REAL_TERRAIN(pf,MAIN_TERRAIN,x);
		}
#endif
/// Render !!!
	uchar types=(*pf&TERRAIN_MASK)>>TERRAIN_OFFSET;
	if(IS_DOUBLE(*pf)){
		if (x&1){
		uchar lx_h =*(pa0+XCYCL(x-2));
		uchar c_h =*pa;
		uchar rx_h =*(pa0+XCYCL(x+2));
		if (rx_h-3>=h) *pf|=*(pf-1)|=SHADOW_MASK;
		if (*pf&SHADOW_MASK){
		*pc=*(pc-1)=palCLR[types][256+((lightCLR[types][255-(c_h-rx_h)]-((255-c_h)>>H_CORRECTION))>>1)];
		}
		else *pc=*(pc-1)=palCLR[types][256+(lightCLR[types][255-(c_h-rx_h)]-((255-c_h)>>H_CORRECTION))];
		//2pix
		pf=pa0+XCYCL(x-2)+H_SIZE;
		pc=pc0+XCYCL(x-2);
		if (h-3>=lx_h) *pf|=*(pf-1)|=SHADOW_MASK;
		if (*pf&SHADOW_MASK){
		*pc=*(pc-1)=palCLR[types][256+((lightCLR[types][255-(lx_h-c_h)]-((255-lx_h)>>H_CORRECTION))>>1)];
		}
		else *pc=*(pc-1)=palCLR[types][256+(lightCLR[types][255-(lx_h-c_h)]-((255-lx_h)>>H_CORRECTION))];
		
		}
	} else {
		uchar llx_h=*(pa0+XCYCL(x-2));
		uchar lx_h =*(pa0+XCYCL(x-1));
		uchar c_h =*pa;
		uchar rx_h =*(pa0+XCYCL(x+1));
		if (rx_h-1-(x&1)>=h) {
			*pf|=SHADOW_MASK;
		}
		if (*pf&SHADOW_MASK){
			*pc=palCLR[types][256+((lightCLR[types][255-(lx_h-rx_h)]-((255-c_h)>>H_CORRECTION))>>1)];
		} else {
			*pc=palCLR[types][256+(lightCLR[types][255-(lx_h-rx_h)]-((255-c_h)>>H_CORRECTION))];
		}
		//2pix
		pf=pa0+XCYCL(x-1)+H_SIZE;
		pc=pc0+XCYCL(x-1);
		if (h-1-((x-1)&1)>=llx_h) *pf|=SHADOW_MASK;
		if (*pf&SHADOW_MASK){
		*pc=palCLR[types][256+((lightCLR[types][255-(llx_h-c_h)]-((255-lx_h)>>H_CORRECTION))>>1)];
		}
		else *pc=palCLR[types][256+(lightCLR[types][255-(llx_h-c_h)]-((255-lx_h)>>H_CORRECTION))];
	}


}

void pixDownSet(int x,int y,int delta,int surf)
{
// std::cout<<"pixDownSet"<<std::endl;

	uchar** lt = vMap -> lineT;
	uchar* pa = lt[y];
	if(!pa){
#ifdef _SURMAP_
		vMap -> increase(y,y);
		pa = lt[y];
#else
		return;
#endif
		}
	uchar* pa0 = pa;
	pa += x;
	uchar* pf = pa + H_SIZE;

	int h = GET_DOWN_ALT(pf,*pa,pa0,x),t;
#ifdef _SURMAP_
	if(surf) if(!ActiveTerrain[GET_REAL_DOWNTERRAIN(pf,x)]) return;
	if(h < MiniLevel || h > MaxiLevel) return;
	if(MosaicToggle && mosaicBMP.mode <= 1) delta = mosaicBMP.getDelta(x,y,delta);
#endif

	if(*pf & DOUBLE_LEVEL)
		if(!(x & 1)){
			h += delta;
#ifdef _SURMAP_
			if(h < MiniLevel) h = MiniLevel;
			if(h > MaxiLevel) h = MaxiLevel;
#endif
//			t = ((GET_DELTA(*pf) << 2) + GET_DELTA(*(pf + 1)) + 1) << DELTA_SHIFT;
			t = *(pa + 1) - TunnelProof - h;
			if(t < 0) t = 0;
			t &= ~(MIN_RDELTA - 1);
			if(t <= MIN_RDELTA){
				*pf &= ~DOUBLE_LEVEL;
				*(pf + 1) &= ~DOUBLE_LEVEL;
				SET_TERRAIN(*pf,GET_TERRAIN_TYPE(*(pf + 1)));
				h = *pa = *(pa + 1);
				SET_DELTA(*pf,0);
				SET_DELTA(*(pf + 1),0);
				}
			else {
				if(t > MAX_RDELTA) t = MAX_RDELTA;
				t = (t >> DELTA_SHIFT) - 1;
				SET_DELTA(*pf,(t & 12) >> 2);
				SET_DELTA(*(pf + 1),(t & 3));
				}
			}
		else
			return;
	else
#ifdef _SURMAP_
		if(ExclusiveLayerMode) return; else
#endif
		h += delta;

#ifdef _SURMAP_
	if(LandBounded){
#endif
		if(h < 0) h = 0;
		else if(h > 255) h = 255;
#ifdef _SURMAP_
		}
	else {
		if(h < 0) h += 256;
		else if(h > 255) h -= 256;
		}
	if(h < MiniLevel) h = MiniLevel;
	if(h > MaxiLevel) h = MaxiLevel;
#endif
//	SET_DOWN_ALT(pf,h,pa0,x);
	*pa = h;

#ifdef _SURMAP_
	if(MosaicToggle && mosaicBMP.mode == 2){
		int t = mosaicBMP.getType(x,y);
		if(t >= 0) SET_REAL_DOWNTERRAIN(pf,(t%TERRAIN_MAX) << TERRAIN_OFFSET,x);
		}
	else if(surf) if(CurrentTerrain != -1) SET_REAL_DOWNTERRAIN(pf,CurrentTerrain << TERRAIN_OFFSET,x);
#endif
#ifndef TERRAIN16
	if(h < FloodLvl[y >> WPART_POWER]){
		uchar* pfN = pa0 + H_SIZE;
		int xx = XCYCL(x - 1);
		if(GET_REAL_DOWNTERRAIN_TYPE(pfN + xx,xx) == WATER_TERRAIN){ SET_REAL_DOWNTERRAIN(pf,WATER_TERRAIN,x); return; }
		xx = XCYCL(x + 1);
		if(GET_REAL_DOWNTERRAIN_TYPE(pfN + xx,xx) == WATER_TERRAIN){ SET_REAL_DOWNTERRAIN(pf,WATER_TERRAIN,x); return; }
		int yy = YCYCL(y - 1);
		pfN = lt[yy];
		if(pfN){
			pfN += H_SIZE + x;
			if(GET_REAL_DOWNTERRAIN_TYPE(pfN,x) == WATER_TERRAIN){ SET_REAL_DOWNTERRAIN(pf,WATER_TERRAIN,x); return; }
			}
		yy = YCYCL(y + 1);
		pfN = lt[yy];
		if(pfN){
			pfN += H_SIZE + x;
			if(GET_REAL_DOWNTERRAIN_TYPE(pfN,x) == WATER_TERRAIN){ SET_REAL_DOWNTERRAIN(pf,WATER_TERRAIN,x); return; }
			}
		}
	else {
		if(GET_REAL_DOWNTERRAIN_TYPE(pf,x) == WATER_TERRAIN)
			SET_REAL_DOWNTERRAIN(pf,MAIN_TERRAIN,x);
		}
#endif
}

#define IS_WATER(a)	(!((a) & TERRAIN_MASK))

void regSet(int x0,int y0,int x1,int y1,int dry,int surf)
{
//std::cout<<"regSet"<<std::endl;

	int i;
	uchar** lt = vMap -> lineT;
	uchar* pa0,*pa,*pf,*pf0;
	
	x0 = XCYCL(x0);
 	x1 = XCYCL(x1 + 1);
	y0 = YCYCL(y0);
	y1 = YCYCL(y1);
	
	int sx = getDeltaX(x1,x0) + 3;
	int x0l = XCYCL(x0 - 1);
	int x0r = XCYCL(x1 + 1);

	int x,y;
#ifndef TERRAIN16
	uchar *pfd,*pfd0;
	uchar* uw = waterBuf[0],*w = waterBuf[1],*dw = waterBuf[2];
	memset(dw,0,sx);
	pa0 = lt[YCYCL(y0 - 1)];
	if(pa0){
		pf = (pf0 = pa0 + H_SIZE) + x0l;
		pfd = (pfd0 = lt[y0] + H_SIZE) + x0l;
		for(i = 0,x = x0l;i < sx;i++,x = XCYCL(x + 1),pf++,pfd++){
			if(!x){ pf = pf0; pfd = pfd0; }
			*uw++ = IS_WATER(*pf);
			*w++ = IS_WATER(*pfd);
			}
		}
	uw = waterBuf[0] + 1;
	w = waterBuf[1] + 1;
	int fld = FloodLEVEL,ind = 0;
#endif
	for(y = y0;y != y1;y = YCYCL(y + 1)){
		pa = (pa0 = lt[y]) + x0;
		if(!pa0) return;
		pf0 = pa0 + H_SIZE;
		pf = pa + H_SIZE;
#ifndef TERRAIN16
		pfd0 = lt[YCYCL(y + 1)];
		if(!pfd0) pfd0 = pf0;
		else pfd0 += H_SIZE;
		pfd = pfd0 + x0;
		dw[sx - 1] = IS_WATER(*(pfd0 + x0r));
		*dw++ = IS_WATER(*(pfd0 + x0l));
#endif
		for(x = x0;x != x1;x = XCYCL(x + 1),pa++,pf++){
			if(!x){
				pa = pa0; pf = pf0; 
#ifndef TERRAIN16
				pfd = pfd0; 
#endif
				}
			if((*pf & DOUBLE_LEVEL) && (x & 1)){
				if((int)*(pa - 1) + (((GET_DELTA(*(pf - 1)) << 2) + GET_DELTA(*pf) + 1) << DELTA_SHIFT) >= *pa){
					*pf &= ~DOUBLE_LEVEL;
					*(pf - 1) &= ~DOUBLE_LEVEL;
					SET_TERRAIN(*pf,GET_TERRAIN_TYPE(*(pf - 1)));
					*pa = (*(pa - 1) + (int)*(pa0 + XCYCL(x + 1))) >> 1;
					SET_DELTA(*pf,0);
					SET_DELTA(*(pf - 1),0);
					}				
				}
#ifdef _SURMAP_
			if(surf) if(CurrentTerrain != -1) SET_REAL_TERRAIN(pf,CurrentTerrain << TERRAIN_OFFSET,x);
#endif
#ifndef TERRAIN16
			*dw = IS_WATER(*pfd);
			if(*pa >= fld){
				if(*w){ SET_REAL_TERRAIN(pf,dry,x); *w = 0; }
				}
			else {
				if(*uw){ SET_REAL_TERRAIN(pf,WATER_TERRAIN,x); *w = 1; }
				else if(*dw){ SET_REAL_TERRAIN(pf,WATER_TERRAIN,x); *w = 1; }
				else if(*(w - 1)){ SET_REAL_TERRAIN(pf,WATER_TERRAIN,x); *w = 1; }
				else if(*(w + 1)){ SET_REAL_TERRAIN(pf,WATER_TERRAIN,x); *w = 1; }
				}
			uw++;w++;dw++;pfd++;
#endif
			}
#ifndef TERRAIN16
		if(++ind == 3) ind = 0;
		uw = waterBuf[ind] + 1;
		if(++ind == 3) ind = 0;
		w = waterBuf[ind] + 1;
		if(++ind == 3) ind = 0;
		dw = waterBuf[ind];
		if(++ind == 3) ind = 0;
#endif
		}
}

//Рендринг, в основную картинку поверхности.
void LINE_render(int y)
{
//std::cout<<" LINE_render "<<y<<std::endl;
	uchar* pa,*pc,*pf,*pa0,*pc0,*pf0;
	uchar type,lxVal,rxVal;

	pa0 = pa = vMap -> lineT[y];
	pf0 = pf = pa0 + H_SIZE;
	pc0 = pc = vMap -> lineTcolor[y];
	
	for(int x = 0;x < map_size_x;x++,pa++,pc++,pf++){
		if(*pf & DOUBLE_LEVEL){
			x++; pc++; pf++; pa++;
			type = (*pf & TERRAIN_MASK) >> TERRAIN_OFFSET;
			lxVal = *pa;
			rxVal = *(pa0 + XCYCL(x + 2));
			if(*pf & SHADOW_MASK)
				*(pc - 1) = *pc = palCLR[type][256 + ((lightCLR[type][255 - (lxVal - rxVal)] - ((255 - *pa) >> H_CORRECTION)) >> 1)];
			else
				*(pc - 1) = *pc = palCLR[type][256 + lightCLR[type][255 - (lxVal - rxVal)] - ((255 - *pa) >> H_CORRECTION)];
			//std::cout<<(int)*(pa)<<" ";
			}
		else {
			type = (*pf & TERRAIN_MASK) >> TERRAIN_OFFSET;
			lxVal = *(pa0 + XCYCL(x - 1));
			rxVal = *(pa + 1);
			if(*pf & SHADOW_MASK)
				*pc = palCLR[type][256 + ((lightCLR[type][255 - (lxVal - rxVal)] - ((255 - *pa) >> H_CORRECTION)) >> 1)];
			else
				*pc = palCLR[type][256 + lightCLR[type][255 - (lxVal - rxVal)] - ((255 - *pa) >> H_CORRECTION)];
			
			//std::cout<<(int)*pc<<" ";
			lxVal = *pa;
			x++; pa++; pc++; pf++;

			type = (*pf & TERRAIN_MASK) >> TERRAIN_OFFSET;
			rxVal = *(pa0 + XCYCL(x + 1));
			if(*pf & SHADOW_MASK)
				*pc = palCLR[type][256 + ((lightCLR[type][255 - (lxVal - rxVal)] - ((255 - *pa) >> H_CORRECTION)) >> 1)];
			else
				*pc = palCLR[type][256 + lightCLR[type][255 - (lxVal - rxVal)] - ((255 - *pa) >> H_CORRECTION)];
			}
		
		//std::cout<<std::endl;
		}
}
