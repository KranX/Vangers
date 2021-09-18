#include "../src/global.h"

#include "../src/common.h"
#include "../src/3d/3d_math.h"
#include "../src/terra/vmap.h"
#include "../src/terra/world.h"
#include "../src/terra/render.h"

#define MEMSET(a,b,c) memset(a,b,c)

typedef unsigned char BYTE;

const int POSPOWER = 8;
const int POSVAL = 1 << POSPOWER;
const int SHADOWHEIGHT = 32;

extern BYTE* shadowParent;
extern BYTE FloodLEVEL;

inline void downPreStage(int& LastStep,int HiX,BYTE* pa0,int& hC,BYTE* pmask)
{
	int MaskShift = LastStep >> POSPOWER;
	int x = XCYCL(HiX + 1);
	BYTE* pa = pa0 + x;
	BYTE* pf = pa + H_SIZE;

	while(hC - MaskShift < MAX_ALT){
		if(!x){
			pa = pa0;
			pf = pa + H_SIZE;
			}
		if(*pf & DOUBLE_LEVEL){
			const int dh = *pa + MaskShift;

			LastStep -= SHADOWDEEP;
			MaskShift = LastStep >> POSPOWER;

			pa++;
			const int h = *pa + MaskShift;
			if(hC < dh){
				MEMSET(pmask + hC,1,dh - hC);
				hC = dh;
				}
//			MEMSET(pmask + h - SS_WIDTH,1,SS_WIDTH + 1);
			pa++;
			}
		else {
			int h = *pa + MaskShift;
			if(hC < h){
				MEMSET(pmask + hC,1,h - hC);
				hC = h;
				}
			LastStep -= SHADOWDEEP;
			MaskShift = LastStep >> POSPOWER;

			pa++;
			h = *pa + MaskShift;
			if(hC < h){
				MEMSET(pmask + hC,1,h - hC);
				hC = h;
				}
			pa++;
			}
          	LastStep -= SHADOWDEEP;
		MaskShift = LastStep >> POSPOWER;

		x = XCYCL(x + 2);
		pf += 2;
		}
	hC -= MAX_ALT;
}

inline void downMainStage(BYTE* pa0,int& hC,BYTE* pc0,int SizeX,int& x,BYTE*& grid,int& MaxAlt)
{
	BYTE* pa = pa0 + x;
	BYTE* pf = pa + H_SIZE;
	BYTE* pc = pc0 + x;

	BYTE* palCLRlast = nullptr;
	BYTE* lightCLRlast = nullptr;
	BYTE typeC = 0xFF;
	register int i;
	for(i = 0;i < SizeX;i += 2){
		if(*pf & DOUBLE_LEVEL){
			pa--; pf--; pc--;
			const BYTE lxVal = *pa;
			const BYTE rxVal = *(pa0 + XCYCL(x + 1));
			const BYTE h = *pa;
			const BYTE dh = *pa;
			const BYTE type = (*pf & TERRAIN_MASK) >> TERRAIN_OFFSET;
			const BYTE level = type ? h : FloodLEVEL;
			if(type != typeC){
				typeC = type;
				palCLRlast = palCLR[type];
				lightCLRlast = lightCLR[type];
				}

			grid += 3;
			hC -= 3;
			MaxAlt -= 3;

			if(*(grid + level)){
				*pf |= SHADOW_MASK;
				*(pf - 1) |= SHADOW_MASK;
				*(pc - 1) = *pc = palCLRlast[256 + (lightCLRlast[255 - (lxVal - rxVal)] - (255 - h >> H_CORRECTION) >> 1)];
				}
			else {
				*pf &= ~SHADOW_MASK;
				*(pf - 1) &= ~SHADOW_MASK;
				*(pc - 1) = *pc = palCLRlast[256 + lightCLRlast[255 - (lxVal - rxVal)] - (255 - h >> H_CORRECTION)];
				}
			if(dh > hC){
				MEMSET(grid + hC,1,dh - hC);
				hC = dh;
				}
			if(h > MaxAlt) MaxAlt = h;
			}
		else {
			BYTE lxVal = *(pa - 1);
			BYTE rxVal = *(pa0 + XCYCL(x + 1));
			BYTE h = *pa;
			BYTE type = (*pf & TERRAIN_MASK) >> TERRAIN_OFFSET;
			BYTE level = type ? h : FloodLEVEL;
			grid += 1;
			hC -= 1;
			MaxAlt -= 1;
			if(type != typeC){
				typeC = type;
				palCLRlast = palCLR[type];
				lightCLRlast = lightCLR[type];
				}

			if(*(grid + level)){
				*pf |= SHADOW_MASK;
				*pc = palCLRlast[256 + (lightCLRlast[255 - (lxVal - rxVal)] - (255 - h >> H_CORRECTION) >> 1)];
				}
			else {
				*pf &= ~SHADOW_MASK;
				*pc = palCLRlast[256 + lightCLRlast[255 - (lxVal - rxVal)] - (255 - h >> H_CORRECTION)];
				}
			if(h > hC){
				MEMSET(grid + hC,1,h - hC);
				hC = h;
				}
			if(h > MaxAlt) MaxAlt = h;
			
			pa--; pf--; pc--;
			rxVal = h;
			h = lxVal;
			lxVal = *(pa0 + XCYCL(x - 2));
			grid += 2;
			hC -= 2;
			MaxAlt -= 2;
			type = (*pf & TERRAIN_MASK) >> TERRAIN_OFFSET;
			level = type ? h : FloodLEVEL;
			if(type != typeC){
				typeC = type;
				palCLRlast = palCLR[type];
				lightCLRlast = lightCLR[type];
				}

			if(*(grid + level)){
				*pf |= SHADOW_MASK;
				*pc = palCLRlast[256 + (lightCLRlast[255 - (lxVal - rxVal)] - (255 - h >> H_CORRECTION) >> 1)];
				}
			else {
				*pf &= ~SHADOW_MASK;
				*pc = palCLRlast[256 + lightCLRlast[255 - (lxVal - rxVal)] - (255 - h >> H_CORRECTION)];
				}
			if(h > hC){
				MEMSET(grid + hC,1,h - hC);
				hC = h;
				}
			if(h > MaxAlt)
				MaxAlt = h;
			}
		if(x == 1){
			x = H_SIZE - 1;
			pa = pa0 + x;
			pf = pa + H_SIZE;
			pc = pc0 + x;
			}
		else {
			x -= 2;
			pa--; pf--; pc--;
			}
		}
}

void regDownRender(int LowX,int LowY,int HiX,int HiY,int changed)
{
	LowX = XCYCL(LowX);
	HiX = XCYCL(HiX);
	LowY = YCYCL(LowY);
	HiY = YCYCL(HiY);

	LowX &= ~1;
	HiX |= 1;

	int SizeY = (LowY == HiY) ? V_SIZE : YCYCL(HiY - LowY);
	int SizeX = (0 == XCYCL(HiX - LowX)) ? H_SIZE : XCYCL(HiX - LowX);

	int BackScanLen = 0;
	register int j;
	for(j = 0;j < SizeY;j++){
		const int y = YCYCL(j + LowY);
		BYTE* pa0 = vMap -> lineT[y];
		BYTE* pc0 = vMap -> lineTcolor[y];
		if(!pa0) return;
		if(changed) vMap -> changedT[y] = 1;
		MEMSET(shadowParent + MAX_ALT,0,(((SizeX*SHADOWDEEP) >> POSPOWER) + 2*MAX_ALT));
		int hC = MAX_ALT;
		int LastStep = (H_SIZE - 1 - HiX) * SHADOWDEEP;
		LastStep -= ((LastStep >> POSPOWER) - MAX_ALT) << POSPOWER;

		//препроход
		downPreStage(LastStep,HiX,pa0,hC,shadowParent);
		//основной проход
		int x = HiX;
		BYTE* grid = shadowParent + MAX_ALT;
		int MaxAlt = 0;
		downMainStage(pa0,hC,pc0,SizeX,x,grid,MaxAlt);
		
		//пост проход
		{
		x |= 1;
		BYTE* pa = pa0 + x;
		BYTE* pf = pa + H_SIZE;
		BYTE* pc = pc0 + x;
		BYTE* palCLRlast = nullptr;
		BYTE* lightCLRlast = nullptr;
		BYTE typeC = 0xFF;
		int MaxPossibleAlt = MAX_ALT;
		unsigned bNeedScan = 1;
		while(bNeedScan && MaxPossibleAlt >= 0){
			bNeedScan = 0;
			if(*pf & DOUBLE_LEVEL){
				pa--; pf--; pc--;
				const BYTE lxVal = *pa;
				const BYTE rxVal = *(pa0 + XCYCL(x + 1));
				const BYTE h = *pa;
				const BYTE dh = *pa;
				const BYTE type = (*pf & TERRAIN_MASK) >> TERRAIN_OFFSET;
				const BYTE level = type ? h : FloodLEVEL;
				if(type != typeC){
					typeC = type;
					palCLRlast = palCLR[type];
					lightCLRlast = lightCLR[type];
					}

				grid += 3;
				hC -= 3;
				MaxAlt -= 3;
				MaxPossibleAlt -= 3;
				if(dh < MaxAlt || (*pf & SHADOW_MASK)) bNeedScan = 1;
				if(*(grid + level)){
					*pf |= SHADOW_MASK;
					*(pf - 1) |= SHADOW_MASK;
					*(pc - 1) = *pc = palCLRlast[256 + (lightCLRlast[255 - (lxVal - rxVal)] - (255 - h >> H_CORRECTION) >> 1)];
					}
				else {
					*pf &= ~SHADOW_MASK;
					*(pf - 1) &= ~SHADOW_MASK;
					*(pc - 1) = *pc = palCLRlast[256 + lightCLRlast[255 - (lxVal - rxVal)] - (255 - h >> H_CORRECTION)];
					}
				if(*pf & SHADOW_MASK) bNeedScan = 1;
				if(dh > hC){
					MEMSET(grid + hC,1,dh - hC);
					hC = dh;
					}
				}
			else {
				BYTE lxVal = *(pa - 1);
				BYTE rxVal = *(pa0 + XCYCL(x + 1));
				BYTE h = *pa;
				BYTE type = (*pf & TERRAIN_MASK) >> TERRAIN_OFFSET;
				BYTE level = type ? h : FloodLEVEL;
				grid += 1;
				hC -= 1;
				MaxAlt -= 1;
				MaxPossibleAlt -= 1;
				if(type != typeC){
					typeC = type;
					palCLRlast = palCLR[type];
					lightCLRlast = lightCLR[type];
					}

				if(h <= MaxAlt || (*pf & SHADOW_MASK)) bNeedScan = 1;
				if(*(grid + level)){
					*pf |= SHADOW_MASK;
					*pc = palCLRlast[256 + (lightCLRlast[255 - (lxVal - rxVal)] - (255 - h >> H_CORRECTION) >> 1)];
					}
				else {
					*pf &= ~SHADOW_MASK;
					*pc = palCLRlast[256 + lightCLRlast[255 - (lxVal - rxVal)] - (255 - h >> H_CORRECTION)];
					}
				if(h > hC){
					MEMSET(grid + hC,1,h - hC);
					hC = h;
					}
				
				pa--; pf--; pc--;
				rxVal = h;
				h = lxVal;
				lxVal = *(pa0 + XCYCL(x - 2));
				grid += 2;
				hC -= 2;
				MaxAlt -= 2;
				MaxPossibleAlt -= 2;
				type = (*pf & TERRAIN_MASK) >> TERRAIN_OFFSET;
				level = type ? h : FloodLEVEL;
				if(type != typeC){
					typeC = type;
					palCLRlast = palCLR[type];
					lightCLRlast = lightCLR[type];
					}

				if(h <= MaxAlt || (*pf & SHADOW_MASK)) bNeedScan = 1;
				if(*(grid + level)){
					*pf |= SHADOW_MASK;
					*pc = palCLRlast[256 + (lightCLRlast[255 - (lxVal - rxVal)] - (255 - h >> H_CORRECTION) >> 1)];
					}
				else {
					*pf &= (~SHADOW_MASK);
					*pc = palCLRlast[256 + lightCLRlast[255 - (lxVal - rxVal)] - (255 - h >> H_CORRECTION)];
					}
				if(h > hC){
					MEMSET(grid + hC,1,h - hC);
					hC = h;
					}
				}
			if(x == 1){
				x = H_SIZE - 1;
				pa = pa0 + x;
				pf = pa + H_SIZE;
				pc = pc0 + x;
				}
			else {
				x = XCYCL(x - 2);
				pa--; pf--; pc--;
				}
			}
		const int CurrScanLenght = XCYCL(LowX - x);
		if(CurrScanLenght > BackScanLen) BackScanLen = CurrScanLenght;
		}
		}
}
