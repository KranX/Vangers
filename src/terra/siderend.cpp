//ASM DEFINE
#define _SHADOWHEIGHT  32

#define _MAP_POWER_X  11
#define _H_CORRECTION  1
#define _H_POWER  _MAP_POWER_X
#define  _H_SIZE  (1 << _H_POWER)
#define _MIN_ALT  0
#define _MAX_ALT  255
#define _map_size_x  (1 << _MAP_POWER_X)
#define _clip_mask_x  (_map_size_x - 1)

#define _SS_WIDTH  16

#define _SHADOW_MASK  (1 << 7)
#define _DELTA_MASK  (1 | (1 << 1))
#define _DOUBLE_LEVEL  (1 << 6)
#define _OBJSHADOW  (1 << 2)

#ifdef TERRAIN16
#define _TERRAIN_MAX  16
#define _TERRAIN_OFFSET  2
#define _TERRAIN_MASK  ((1 << _TERRAIN_OFFSET) | (1 << (_TERRAIN_OFFSET + 1)) | (1 << (_TERRAIN_OFFSET + 2)) | (1 << (_TERRAIN_OFFSET + 3)))
#else
#define _TERRAIN_MAX  8
#define _TERRAIN_OFFSET  3
#define _TERRAIN_MASK  ((1 << _TERRAIN_OFFSET) | (1 << (_TERRAIN_OFFSET + 1)) | (1 << (_TERRAIN_OFFSET + 2)))
#endif

#include "../global.h"

#include "../3d/3d_math.h"
#include "vmap.h"
#include "world.h"
#include "render.h"


#define MEMSET(a,b,c) memset(a,b,c)

typedef unsigned char BYTE;

const int POSPOWER = 8; //8
const int POSVAL = 1 << POSPOWER;
const int SHADOWHEIGHT = 32; //32


extern BYTE* shadowParent;
extern BYTE FloodLEVEL;

//Surmap
inline void PreStage(int& LastStep,int HiX,BYTE* pa0,int& hC,BYTE* pmask)
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
			MEMSET(pmask + h - SS_WIDTH,1,SS_WIDTH + 1);
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

inline void PreStage2(int& LastStep,int HiX,BYTE* pa0,int& hC,BYTE* pmask)
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
			MEMSET(pmask + h - SS_WIDTH,1,SS_WIDTH + 1);
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

//Surmap
inline void MainStage(BYTE* pa0,int& hC,BYTE* pc0,int SizeX,int& x,BYTE*& grid,int& MaxAlt)
{
	BYTE* pa = pa0 + x;
	BYTE* pf = pa + H_SIZE;
	BYTE* pc = pc0 + x;

	BYTE* palCLRlast;
	BYTE* lightCLRlast;
	BYTE typeC = 0xFF;
	int i;
	for(i = 0;i < SizeX;i += 2){
		if(*pf & DOUBLE_LEVEL){
			const BYTE lxVal = *pa;
			const BYTE rxVal = *(pa0 + XCYCL(x + 2));
			const BYTE h = *pa;
			const BYTE dh = *(pa - 1);
			const BYTE type = (*pf & TERRAIN_MASK) >> TERRAIN_OFFSET;
#ifndef TERRAIN16
			const BYTE level = type ? h : FloodLEVEL;
#else
			const BYTE level = h;
#endif
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
				*(pc - 1) = *pc = palCLRlast[256 + ((lightCLRlast[255 - (lxVal - rxVal)] - ((255 - h) >> H_CORRECTION)) >> 1)];
				}
			else {
				*pf &= ~SHADOW_MASK;
				*(pf - 1) &= ~SHADOW_MASK;
				*(pc - 1) = *pc = palCLRlast[256 + lightCLRlast[255 - (lxVal - rxVal)] - ((255 - h) >> H_CORRECTION)];
				}
#ifndef TERRAIN16
			if(*(grid + SHADOWHEIGHT + level)){
				*pf |= OBJSHADOW;
				*(pf - 1) |= OBJSHADOW;
				}
			else {
				*pf &= ~OBJSHADOW;
				*(pf - 1) &= ~OBJSHADOW;
				}
#endif
			pa--; pf--; pc--;
			if(dh > hC){
				MEMSET(grid + hC,1,dh - hC);
				hC = dh;
				}
			MEMSET(grid + h - SS_WIDTH,1,SS_WIDTH + 1);
			if(h > MaxAlt) MaxAlt = h;
			}
		else {
			BYTE lxVal = *(pa - 1);
			BYTE rxVal = *(pa0 + XCYCL(x + 1));
			BYTE h = *pa;
			BYTE type = (*pf & TERRAIN_MASK) >> TERRAIN_OFFSET;
#ifndef TERRAIN16
			BYTE level = type ? h : FloodLEVEL;
#else
			BYTE level = h;
#endif
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
				*pc = palCLRlast[256 + ((lightCLRlast[255 - (lxVal - rxVal)] - ((255 - h) >> H_CORRECTION)) >> 1)];
				}
			else {
				*pf &= ~SHADOW_MASK;
				*pc = palCLRlast[256 + lightCLRlast[255 - (lxVal - rxVal)] - ((255 - h) >> H_CORRECTION)];
				}
#ifndef TERRAIN16
			if(*(grid + SHADOWHEIGHT + level))
				*pf |= OBJSHADOW;
			else
				*pf &= ~OBJSHADOW;
#endif
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
#ifndef TERRAIN16
			level = type ? h : FloodLEVEL;
#else
			level = h;
#endif			
			if(type != typeC){
				typeC = type;
				palCLRlast = palCLR[type];
				lightCLRlast = lightCLR[type];
				}

			if(*(grid + level)){
				*pf |= SHADOW_MASK;
				*pc = palCLRlast[256 + ((lightCLRlast[255 - (lxVal - rxVal)] - ((255 - h) >> H_CORRECTION)) >> 1)];
				}
			else {
				*pf &= ~SHADOW_MASK;
				*pc = palCLRlast[256 + lightCLRlast[255 - (lxVal - rxVal)] - ((255 - h) >> H_CORRECTION)];
				}
#ifndef TERRAIN16
			if(*(grid + SHADOWHEIGHT + level))
				*pf |= OBJSHADOW;
			else
				*pf &= ~OBJSHADOW;
#endif
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

inline void MainStage2(BYTE* pa0,int& hC,BYTE* pc0,int SizeX,int& x,BYTE*& grid,int& MaxAlt)
{
	BYTE* pa = pa0 + x;
	BYTE* pf = pa + H_SIZE;
	BYTE* pc = pc0 + x;

	BYTE* palCLRlast;
	BYTE* lightCLRlast;
	BYTE typeC = 0xFF;
	int i;
	for(i = 0;i < SizeX;i += 2){
		if(*pf & DOUBLE_LEVEL){
			const BYTE lxVal = *pa;
			const BYTE rxVal = *(pa0 + XCYCL(x + 2));
			const BYTE h = *pa;
			const BYTE dh = *(pa - 1);
			const BYTE type = (*pf & TERRAIN_MASK) >> TERRAIN_OFFSET;
			const BYTE level = h;
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
				*(pc - 1) = *pc = palCLRlast[256 + (lightCLRlast[255 - (lxVal - rxVal)] - ((255 - h) >> H_CORRECTION))];
				}
			else {
				*pf &= ~SHADOW_MASK;
				*(pf - 1) &= ~SHADOW_MASK;
				*(pc - 1) = *pc = palCLRlast[256 + lightCLRlast[255 - (lxVal - rxVal)] - ((255 - h) >> H_CORRECTION)];
				}
			pa--; pf--; pc--;
			if(dh > hC){
				MEMSET(grid + hC,1,dh - hC);
				hC = dh;
				}
			MEMSET(grid + h - SS_WIDTH,1,SS_WIDTH + 1);
			if(h > MaxAlt) MaxAlt = h;
			}
		else {
			BYTE lxVal = *(pa - 1);
			BYTE rxVal = *(pa0 + XCYCL(x + 1));
			BYTE h = *pa;
			BYTE type = (*pf & TERRAIN_MASK) >> TERRAIN_OFFSET;
			BYTE level = h;
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
				*pc = palCLRlast[256 + (lightCLRlast[255 - (lxVal - rxVal)] - ((255 - h) >> H_CORRECTION) )];
				}
			else {
				*pf &= ~SHADOW_MASK;
				*pc = palCLRlast[256 + lightCLRlast[255 - (lxVal - rxVal)] - ((255 - h) >> H_CORRECTION)];
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
			level = h;
			if(type != typeC){
				typeC = type;
				palCLRlast = palCLR[type];
				lightCLRlast = lightCLR[type];
				}

			if(*(grid + level)){
				*pf |= SHADOW_MASK;
				*pc = palCLRlast[256 + (lightCLRlast[255 - (lxVal - rxVal)] - ((255 - h) >> H_CORRECTION) )];
				}
			else {
				*pf &= ~SHADOW_MASK;
				*pc = palCLRlast[256 + lightCLRlast[255 - (lxVal - rxVal)] - ((255 - h) >> H_CORRECTION)];
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

//Surmap render
void regRender(int LowX,int LowY,int HiX,int HiY,int changed)
{
#ifdef _SURMAP_
	if(RenderingLayer == DOWN_LAYER){
		regDownRender(LowX,LowY,HiX,HiY,changed);
		return;
		}
#endif

	LowX = XCYCL(LowX);
	HiX = XCYCL(HiX);
	LowY = YCYCL(LowY);
	HiY = YCYCL(HiY);

	LowX &= ~1;
	HiX |= 1;

	int SizeY = (LowY == HiY) ? V_SIZE : YCYCL(HiY - LowY);
	int SizeX = (0 == XCYCL(HiX - LowX)) ? H_SIZE : XCYCL(HiX - LowX);

	int BackScanLen = 0;
	int j;
	for(j = 0;j < SizeY;j++){
		const int y = YCYCL(j + LowY);
		BYTE* pa0 = vMap -> lineT[y];
		BYTE* pc0 = vMap -> lineTcolor[y];
		if(!pa0) return;
		if(changed) vMap -> changedT[y] = 1;
		MEMSET(shadowParent,0,(((SizeX*SHADOWDEEP) >> POSPOWER) + 4*MAX_ALT));
//		MEMSET(shadowParent,0,map_size_x);
		int hC = MAX_ALT;
		int LastStep = (H_SIZE - 1 - HiX) * SHADOWDEEP;
		LastStep -= ((LastStep >> POSPOWER) - MAX_ALT) << POSPOWER;

		//препроход
		PreStage(LastStep,HiX,pa0,hC,shadowParent + MAX_ALT);
		//основной проход
		int x = HiX;
		BYTE* grid = shadowParent + MAX_ALT + MAX_ALT;
		int MaxAlt = 0;
		MainStage(pa0,hC,pc0,SizeX,x,grid,MaxAlt);
		
		//пост проход
		{
		x |= 1;
		BYTE* pa = pa0 + x;
		BYTE* pf = pa + H_SIZE;
		BYTE* pc = pc0 + x;
		BYTE* palCLRlast;
		BYTE* lightCLRlast;
		BYTE typeC = 0xFF;
		int MaxPossibleAlt = MAX_ALT;
		unsigned bNeedScan = 1;
		while(bNeedScan && MaxPossibleAlt >= 0){
			bNeedScan = 0;
			if(*pf & DOUBLE_LEVEL){
				const BYTE lxVal = *pa;
				const BYTE rxVal = *(pa0 + XCYCL(x + 2));
				const BYTE h = *pa;
				const BYTE dh = *(pa - 1);
				const BYTE type = (*pf & TERRAIN_MASK) >> TERRAIN_OFFSET;
#ifndef TERRAIN16
				const BYTE level = type ? h : FloodLEVEL;
#else
				const BYTE level = h;
#endif
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
					*(pc - 1) = *pc = palCLRlast[256 + ((lightCLRlast[255 - (lxVal - rxVal)] - ((255 - h) >> H_CORRECTION)) >> 1)];
					}
				else {
					*pf &= ~SHADOW_MASK;
					*(pf - 1) &= ~SHADOW_MASK;
					*(pc - 1) = *pc = palCLRlast[256 + lightCLRlast[255 - (lxVal - rxVal)] - ((255 - h) >> H_CORRECTION)];
					}
#ifndef TERRAIN16
				if(*(grid + SHADOWHEIGHT + level)){
					*pf |= OBJSHADOW;
					*(pf - 1) |= OBJSHADOW;
					}
				else {
					*pf &= ~OBJSHADOW;
					*(pf - 1) &= ~OBJSHADOW;
					}
#endif
				pa--; pf--; pc--;
				if(*pf & SHADOW_MASK) bNeedScan = 1;
				if(dh > hC){
					MEMSET(grid + hC,1,dh - hC);
					hC = dh;
					}
				MEMSET(grid + h - SS_WIDTH,1,SS_WIDTH + 1);
				}
			else {
				BYTE lxVal = *(pa - 1);
				BYTE rxVal = *(pa0 + XCYCL(x + 1));
				BYTE h = *pa;
				BYTE type = (*pf & TERRAIN_MASK) >> TERRAIN_OFFSET;
#ifndef TERRAIN16
				BYTE level = type ? h : FloodLEVEL;
#else
				BYTE level = h;
#endif
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
					*pc = palCLRlast[256 + ((lightCLRlast[255 - (lxVal - rxVal)] - ((255 - h) >> H_CORRECTION)) >> 1)];
					}
				else {
					*pf &= ~SHADOW_MASK;
					*pc = palCLRlast[256 + lightCLRlast[255 - (lxVal - rxVal)] - ((255 - h) >> H_CORRECTION)];
					}
#ifndef TERRAIN16
				if(*(grid + SHADOWHEIGHT + level))
					*pf |= OBJSHADOW;
				else
					*pf &= ~OBJSHADOW;
#endif
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
#ifndef TERRAIN16
				level = type ? h : FloodLEVEL;
#else
				level = h;
#endif
				if(type != typeC){
					typeC = type;
					palCLRlast = palCLR[type];
					lightCLRlast = lightCLR[type];
					}

				if(h <= MaxAlt || (*pf & SHADOW_MASK)) bNeedScan = 1;
				if(*(grid + level)){
					*pf |= SHADOW_MASK;
					*pc = palCLRlast[256 + ((lightCLRlast[255 - (lxVal - rxVal)] - ((255 - h) >> H_CORRECTION)) >> 1)];
					}
				else {
					*pf &= ~SHADOW_MASK;
					*pc = palCLRlast[256 + lightCLRlast[255 - (lxVal - rxVal)] - ((255 - h) >> H_CORRECTION)];
					}
#ifndef TERRAIN16
				if(*(grid + SHADOWHEIGHT + level))
					*pf |= OBJSHADOW;
				else
					*pf &= ~OBJSHADOW;
#endif
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

//znfo render
void regRender2(int LowX,int LowY,int HiX,int HiY,int changed)
{
	//ErrH.Abort("regRender");
	//std::cout<<"BB"<<std::endl;
	LowX = XCYCL(LowX);
	HiX = XCYCL(HiX);
	LowY = YCYCL(LowY);
	HiY = YCYCL(HiY);

	LowX &= ~1;
	HiX |= 1;

	int SizeY = (LowY == HiY) ? V_SIZE : YCYCL(HiY - LowY);
	int SizeX = (0 == XCYCL(HiX - LowX)) ? _H_SIZE : XCYCL(HiX - LowX);

//	LowX = XCYCL(LowX-172-2);
	LowX = XCYCL(LowX-2);

	int BackScanLen = 0;
	//register
	int j;
	for(j = 0;j <SizeY;j++){//SizeY
		const int y = YCYCL(j + LowY);
		BYTE* pLineAddr = vMap -> lineT[y];
		BYTE* pColorLine = vMap -> lineTcolor[y];
		if(!pLineAddr) return;
		if(changed) vMap -> changedT[y] = 1;
		int hC = _MAX_ALT;
		int LastStep = (H_SIZE - 1 - HiX) * SHADOWDEEP;
		LastStep -= ((LastStep >> POSPOWER) - MAX_ALT) << POSPOWER;
		//препроход
		PreStage(LastStep,HiX,pLineAddr,hC,shadowParent + MAX_ALT);
		//основной проход
		BYTE* grid =(BYTE*)((uintptr_t)(shadowParent+_SS_WIDTH+_MAX_ALT+1)|0x00ff)+1;
		
		int MaxAlt = 0;
		int x = HiX;
		MainStage(pLineAddr,hC,pColorLine,SizeX,x,grid,MaxAlt);

		//пост проход
		{
		x |= 1;
		BYTE* pa = pLineAddr + x;
		BYTE* pf = pa + H_SIZE;
		BYTE* pc = pColorLine + x;
		BYTE* palCLRlast;
		BYTE* lightCLRlast;
		BYTE typeC = 0xFF;
		int MaxPossibleAlt = MAX_ALT;
		unsigned bNeedScan = 1;
		while(bNeedScan && MaxPossibleAlt >= 0){
			bNeedScan = 0;
			if(*pf & DOUBLE_LEVEL){
				const BYTE lxVal = *pa;
				const BYTE rxVal = *(pLineAddr + XCYCL(x + 2));
				const BYTE h = *pa;
				const BYTE dh = *(pa - 1);
				const BYTE type = (*pf & TERRAIN_MASK) >> TERRAIN_OFFSET;
				const BYTE level = h;
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
					*(pc - 1) = *pc = palCLRlast[256 + (lightCLRlast[255 - (lxVal - rxVal)] - ((255 - h) >> H_CORRECTION) )];
					}
				else {
					*pf &= ~SHADOW_MASK;
					*(pf - 1) &= ~SHADOW_MASK;
					*(pc - 1) = *pc = palCLRlast[256 + lightCLRlast[255 - (lxVal - rxVal)] - ((255 - h) >> H_CORRECTION)];
					}
				pa--; pf--; pc--;
				if(*pf & SHADOW_MASK) bNeedScan = 1;
				if(dh > hC){
					MEMSET(grid + hC,1,dh - hC);
					hC = dh;
					}
				MEMSET(grid + h - SS_WIDTH,1,SS_WIDTH + 1);
				}
			else {
				BYTE lxVal = *(pa - 1);
				BYTE rxVal = *(pLineAddr + XCYCL(x + 1));
				BYTE h = *pa;
				BYTE type = (*pf & TERRAIN_MASK) >> TERRAIN_OFFSET;
				BYTE level = h;
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
					*pc = palCLRlast[256 + (lightCLRlast[255 - (lxVal - rxVal)] - ((255 - h) >> H_CORRECTION))];
					}
				else {
					*pf &= ~SHADOW_MASK;
					*pc = palCLRlast[256 + lightCLRlast[255 - (lxVal - rxVal)] - ((255 - h) >> H_CORRECTION)];
					}
				if(h > hC){
					MEMSET(grid + hC,1,h - hC);
					hC = h;
					}

				pa--; pf--; pc--;
				rxVal = h;
				h = lxVal;
				lxVal = *(pLineAddr + XCYCL(x - 2));
				grid += 2;
				hC -= 2;
				MaxAlt -= 2;
				MaxPossibleAlt -= 2;
				type = (*pf & TERRAIN_MASK) >> TERRAIN_OFFSET;
				level = h;
				if(type != typeC){
					typeC = type;
					palCLRlast = palCLR[type];
					lightCLRlast = lightCLR[type];
					}

				if(h <= MaxAlt || (*pf & SHADOW_MASK)) bNeedScan = 1;
				if(*(grid + level)){
					*pf |= SHADOW_MASK;
					*pc = palCLRlast[256 + (lightCLRlast[255 - (lxVal - rxVal)] - ((255 - h) >> H_CORRECTION) )];
					}
				else {
					*pf &= ~SHADOW_MASK;
					*pc = palCLRlast[256 + lightCLRlast[255 - (lxVal - rxVal)] - ((255 - h) >> H_CORRECTION)];
					}
				if(h > hC){
					MEMSET(grid + hC,1,h - hC);
					hC = h;
					}
				}
			if(x == 1){
				x = H_SIZE - 1;
				pa = pLineAddr + x;
				pf = pa + H_SIZE;
				pc = pColorLine + x;
				}
			else {
				x = XCYCL(x - 2);
				pa--; pf--; pc--;
				}
			}
		const int CurrScanLenght = XCYCL(LowX - x);
		if(CurrScanLenght > BackScanLen) BackScanLen = CurrScanLenght;
		}
		

/*WORK		_asm {
		pushad
;PreStage
			xor eax,eax
			mov ebx,hC	;
			mov edi,hC
			mov ecx,HiX
			inc ecx
			and ecx,7FFh	;_clip_mask_x
p_b_linex:	mov esi,pLineAddr
			add esi,ecx
p_b_while:	sub edi,ebx
			cmp edi,255d
			jnbe p_e_while
			add edi,ebx

			test byte ptr [esi+_H_SIZE],40h	;cmp na DL
/// W R E M E N N O !!!
////			jmp found_dl_pr
			jne found_dl_pr

			bswap ecx
			mov ax,[esi]	;if eax ??? !!!
			xchg ch,ah
			add eax,ebx
			sub ebx,2
			js p_e_while
			cmp edi,eax
			jnb pl_1
			mov edi,eax
pl_1:
			xor eax,eax
			mov al,ch
			add eax,ebx
			sub ebx,1
			js p_e_while
			cmp edi,eax
			jnb pl_2
			mov edi,eax
pl_2:
			bswap ecx
			add ecx,2
			add esi,2
			and ecx,7FFh	;_clip_mask_x
			je p_b_linex
			jmp p_b_while
p_e_while:
			add edi,ebx
			sub edi,255d
			jns pr_not_overflov
			xor edi,edi
pr_not_overflov:
			mov edx,edi
;MainStage
			mov esi,pLineAddr
			mov ecx,HiX
			dec ecx
			mov edi,pColorLine
			add esi,ecx
			add edi,ecx
			cmp ecx,0
			je ms_beg_map_detectet
			cmp ecx,_H_SIZE-2
			je m_xoverflov
beg_mstage:
			push ecx
			xor ecx,ecx
			mov cx,[esi+_H_SIZE]	;if eax ??? !!!
			test cl,40h		;cmp na DL
			jne found_dl_ms

			mov eax,[esi-1]
ml_continue:
			mov ebx,0ffffh	;255 & 255
			sub bh,al	;lx 2
			sub bl,ah	;lx 1
			bswap eax
			;add bh,ah	;rx 2
			;add bl,al	;rx 1
			bswap eax
			bswap ebx	;save delta 1pix & 2pix
			shr eax,8	;al  h 2pix; ah  h 1pix 
			or ebx,0ffffh	;255 & 255
			sub bl,ah
			sub bh,al
			shr bl,_H_CORRECTION
			shr bh,_H_CORRECTION

			and ecx,_TERRAIN_MASK + (_TERRAIN_MASK<<8)
			shr ecx,_TERRAIN_OFFSET	
			bswap edx
			mov dx,cx	; free ecx !!!

			push esi
			bswap ebx       ;restore delta 1p&2p ;save delta2 1p&2p
;lightCLR 1pix
			movzx esi,dh
			shl esi,2
//			add esi,*lightCLR
			lea ecx,lightCLR
			mov esi,[esi+ecx]	;in esi - ^lightCLR
			movzx ecx,bl
			bswap eax
			add cl,ah
			adc ch,0
			mov bl,[esi+ecx]
			bswap eax
;lightCLR 2pix			
			movzx esi,dl
			shl esi,2
//			add esi,lightCLR
			lea ecx,lightCLR
			mov esi,[esi+ecx]	;in esi - ^lightCLR
			movzx ecx,bh
			add cl,ah
			adc ch,0
			mov bh,[esi+ecx]

			cmp dh,0	;cmp na FloodLevel 1pix
			jne ml_3
			mov ah,FloodLEVEL
ml_3:		cmp dl,0	;cmp na FloodLevel 2pix
			jne ml_4
			mov al,FloodLEVEL
ml_4:
;calculate 1pix
			movzx esi,dh
			shl esi,9
			lea ecx,palCLR
			add esi,ecx

			mov ecx,256
			mov cl,bl
			bswap ebx	;restore delt2 ;save delta
			sub cl,bl
			sbb ch,0
			bswap edx	;restore hC ;save terrain
			mov bl,0	;not SHADOW_MASK;not OBJSHADOW

			sub dl,1
			jc ml_5		;if dl <0 
			cmp dl,ah	;compire na h>hc	; cmp hC & h 1pix
			jbe ml_5	
			shr ecx,1
			add ecx,128d
			mov bl,_SHADOW_MASK
			add ah,_SHADOWHEIGHT
			jc ml_5$
			cmp dl,ah
			jb ml_5$
			or bl,_OBJSHADOW
			jmp ml_5$
ml_5:
			mov dl,ah	;save hC 1pix
ml_5$:		mov ah,[esi+ecx]	
			
			bswap edx	;restore terrain;save hC
			bswap ebx	;restore delta;save delta2
;calculate 2pix
			movzx esi,dl
			shl esi,9
			lea ecx,palCLR
			add esi,ecx

			mov ecx,256
			mov cl,bh
			bswap ebx	;restore delt2 ;save delta
			sub cl,bh
			sbb ch,0
			bswap edx	;restore hC ;save terrain
			mov bh,0	;not SHADOW_MASK;not OBJSHADOW
			
			sub dl,2
			jc ml_6
			cmp dl,al	; cmp hC & h 2pix ; cmp hC & h 1pix
			jbe ml_6		;
			shr ecx,1
			add ecx,128d
			mov bh,_SHADOW_MASK
			add al,_SHADOWHEIGHT
			jc ml_6$
			cmp dl,al
			jb ml_6$
			or bh,_OBJSHADOW
			jmp ml_6$
ml_6:		mov dl,al	;save hC 2pix
ml_6$:		mov al,[esi+ecx]
		
//			xchg bl,bh
			bswap ebx
			pop esi
//			mov [edi],al
//			mov [edi+1],ah
			mov [edi],ax
*//*			or [esi+_H_SIZE],bx
			not bx
			and [esi+_H_SIZE],bx
*/
/*WORK			shr ebx,10h
			mov eax,[esi+_H_SIZE]
			and eax,0FFFF7B7Bh
			or eax,ebx
			mov [esi+_H_SIZE],eax

			pop ecx
			sub edi,2
			sub esi,2
			sub ecx,2
			and ecx,7FFh	;_clip_mask_x
			je ms_beg_map_detectet
			cmp ecx,LowX
			je end_mstage
			cmp ecx,_H_SIZE-2
			je m_xoverflov
			jmp beg_mstage
ms_beg_map_detectet:
			cmp ecx,LowX
			je end_mstage
			push ecx
			xor ecx,ecx
			mov cx,[esi+_H_SIZE]	;if eax ??? !!!
			mov eax,[esi]
			shl eax,8
			mov esi,pLineAddr
			add esi,_H_SIZE-1
			mov al,[esi]
			mov esi,pLineAddr
			test cl,40h		;cmp na DL
			jne found_dl_ms
			jmp ml_continue
m_xoverflov:
			push ecx
			mov esi,pLineAddr
			add esi,_H_SIZE-2
			xor ecx,ecx
			mov cx,[esi+_H_SIZE]	;if eax ??? !!!
			mov eax,[esi-1]
			rol eax,8
			mov esi,pLineAddr
			mov al,[esi]
			ror eax,8
			add esi,_H_SIZE-2
			mov edi,pColorLine
			add edi,_H_SIZE-2
			test cl,40h		;cmp na DL
			jne found_dl_ms
			jmp ml_continue

end_mstage:

;PostStage
			cmp ecx,0
			je ps_beg_map_detectet
			cmp ecx,_H_SIZE-2
			je p_xoverflov
beg_pstage:
			push ecx
			xor ecx,ecx
			mov cx,[esi+_H_SIZE]	;if eax ??? !!!
			test cl,40h		;cmp na DL
			jne found_dl_ps

			mov eax,[esi-1]
pl_continue:
			test ecx,_SHADOW_MASK*256+_SHADOW_MASK
			jne pl_continue2
			cmp psflag,0
			jne end_pstage
pl_continue2:
			mov ebx,0ffffh	;255 & 255
			sub bh,al	;lx 2
			sub bl,ah	;lx 1
			bswap ebx	;save delta 1pix & 2pix
			shr eax,8	;al  h 2pix; ah  h 1pix 
			sub MAX_H_RENDER,1
			cmp MAX_H_RENDER,ah
			jbe end_pstage
			or ebx,0ffffh	;255 & 255
			sub bl,ah
			sub bh,al
			shr bl,_H_CORRECTION
			shr bh,_H_CORRECTION

			and ecx,_TERRAIN_MASK + (_TERRAIN_MASK<<8)
			shr ecx,_TERRAIN_OFFSET	
			bswap edx
			mov dx,cx	; free ecx !!!

			push esi
			bswap ebx       ;restore delta 1p&2p ;save delta2 1p&2p
;lightCLR 1pix
			movzx esi,dh
			shl esi,2
			lea ecx,lightCLR
			mov esi,[esi+ecx]	;in esi - ^lightCLR
			movzx ecx,bl
			bswap eax
			add cl,ah
			adc ch,0
			mov bl,[esi+ecx]
			bswap eax
;lightCLR 2pix			
			movzx esi,dl
			shl esi,2
			lea ecx,lightCLR
			mov esi,[esi+ecx]	;in esi - ^lightCLR
			movzx ecx,bh
			add cl,ah
			adc ch,0
			mov bh,[esi+ecx]

			cmp dh,0	;cmp na FloodLevel 1pix
			jne pl_3
			mov ah,FloodLEVEL
pl_3:		cmp dl,0	;cmp na FloodLevel 2pix
			jne pl_4
			mov al,FloodLEVEL
pl_4:
;calculate 1pix
			movzx esi,dh
			shl esi,9
			lea ecx,palCLR
			add esi,ecx

			mov ecx,256
			mov cl,bl
			bswap ebx	;restore delt2 ;save delta
			sub cl,bl
			sbb ch,0
			bswap edx	;restore hC ;save terrain
			mov bl,0	;not SHADOW_MASK;not OBJSHADOW

			sub dl,1
			jc pl_5		;if dl <0 
			cmp dl,ah	;compire na h>hc	; cmp hC & h 1pix
			jbe pl_5	
			shr ecx,1
			add ecx,128d
			mov bl,_SHADOW_MASK
			add ah,_SHADOWHEIGHT
			jc pl_5$
			cmp dl,ah
			jb pl_5$
			or bl,_OBJSHADOW
			jmp pl_5$
pl_5:
			mov psflag,1h
			mov dl,ah	;save hC 1pix
pl_5$:		mov ah,[esi+ecx]	
			
			bswap edx	;restore terrain;save hC
			bswap ebx	;restore delta;save delta2
;calculate 2pix
			sub MAX_H_RENDER,2
			cmp MAX_H_RENDER,al
			jbe end_pstage$$

			movzx esi,dl
			shl esi,9
			lea ecx,palCLR
			add esi,ecx

			mov ecx,256
			mov cl,bh
			bswap ebx	;restore delt2 ;save delta
			sub cl,bh
			sbb ch,0
			bswap edx	;restore hC ;save terrain
			mov bh,0	;not SHADOW_MASK;not OBJSHADOW
			
			sub dl,2
			jc pl_6
			cmp dl,al	; cmp hC & h 2pix ; cmp hC & h 1pix
			jbe pl_6		;
			shr ecx,1
			add ecx,128d
			mov bh,_SHADOW_MASK
			add al,_SHADOWHEIGHT
			jc pl_6$
			cmp dl,al
			jb pl_6$
			or bh,_OBJSHADOW
			jmp pl_6$
pl_6:
			mov psflag,1h
			mov dl,al	;save hC 2pix
pl_6$:		mov al,[esi+ecx]
		
			bswap ebx
			pop esi
			mov [edi],ax
			shr ebx,10h
			mov eax,[esi+_H_SIZE]
			and eax,0FFFF7B7Bh
			or eax,ebx
			mov [esi+_H_SIZE],eax

			pop ecx
			sub edi,2
			sub esi,2
			sub ecx,2
			and ecx,7FFh	;_clip_mask_x
			je ps_beg_map_detectet
			cmp ecx,_H_SIZE-2
			je p_xoverflov
			jmp beg_pstage
ps_beg_map_detectet:
			push ecx
			xor ecx,ecx
			mov cx,[esi+_H_SIZE]	;if eax ??? !!!
			mov eax,[esi]
			shl eax,8
			mov esi,pLineAddr
			add esi,_H_SIZE-1
			mov al,[esi]
			mov esi,pLineAddr
			test cl,40h		;cmp na DL
			jne found_dl_ps
			jmp pl_continue
p_xoverflov:
			push ecx
			mov esi,pLineAddr
			add esi,_H_SIZE-2
			xor ecx,ecx
			mov cx,[esi+_H_SIZE]	;if eax ??? !!!
			mov eax,[esi-1]
			rol eax,8
			mov esi,pLineAddr
			mov al,[esi]
			ror eax,8
			add esi,_H_SIZE-2
			mov edi,pColorLine
			add edi,_H_SIZE-2
			test cl,40h		;cmp na DL
			jne found_dl_ps
			jmp pl_continue




;;;;;;;;;;;;;; F O U N D    D O U B L E    L E V E L    ! ! ! ;;;;;;;;;;;;;;;;
found_dl_pr:
			push ecx
			push edi
			mov edi,grid
			xor eax,eax
			mov ecx,64d
			rep stosd
			pop edi
			pop ecx
			mov edx,grid	;O P T M I Z A T I O N !
			jmp pr2_begin
found_dl_ms:
			pop ecx
			push ecx
			push edi
			mov edi,grid
			xor eax,eax
			mov ecx,64d
			rep stosd
			pop edi
			pop ecx
			jmp m2_begin
found_dl_ps:
			pop ecx
			push ecx
			push edi
			mov edi,grid
			xor eax,eax
			mov ecx,64d
			rep stosd
			pop edi
			pop ecx
			jmp p2_begin

;PreStage 2
pr2_b_linex:
			mov esi,pLineAddr
			add esi,ecx
pr2_b_while:
			sub edi,ebx
			cmp edi,255d
			jnbe pr2_e_while
			add edi,ebx
pr2_begin:
			bswap ecx
			mov ax,[esi]	;if eax ??? !!!
			xchg ch,ah
			add eax,ebx
			sub ebx,2
			js pr2_e_while
			cmp edi,eax
			jnb pr2l_1
			mov edi,eax
pr2l_1:
			test byte ptr [esi+_H_SIZE],40h	;cmp na DL
			jne found_dl_pr2

			xor eax,eax
			mov al,ch
			add eax,ebx
			sub ebx,1
			js pr2_e_while
			cmp edi,eax
			jnb pr2l_2
			mov edi,eax
pr2l_2:
			bswap ecx
			add ecx,2
			add esi,2
			and ecx,7FFh	;_clip_mask_x
			je pr2_b_linex
			jmp pr2_b_while

found_dl_pr2:
			xor eax,eax
			mov edx,grid
			mov al,ch
			add eax,ebx
			sub  ebx,1
			js pr2_e_while
			cmp eax,_SS_WIDTH+_MAX_ALT+1
			jbe pr2l_not_shadow
			sub eax,_SS_WIDTH+_MAX_ALT
			add edx,eax
			mov eax,01010101h
			mov [edx],eax
			;add edx,4		;Change to DL !
			mov [edx+4],eax
			;add edx,4
			mov [edx+8],eax
			;add edx,4
			mov [edx+0Ch],eax
			;add edx,4
			mov [edx+10h],al
pr2l_not_shadow:
			xor eax,eax

			bswap ecx
			add ecx,2
			add esi,2
			and ecx,7FFh	;_clip_mask_x
			je pr2_b_linex
			jmp pr2_b_while
pr2_e_while:
			add edi,ebx
			sub edi,255d
			jns pr2_not_overflov
			xor edi,edi
pr2_not_overflov:
			mov edx,edi
///////////////////////////////////////////////////////////////////////
;MainStage2
			mov esi,pLineAddr
			mov ecx,HiX
			dec ecx
			mov edi,pColorLine
			add esi,ecx
			add edi,ecx
			cmp ecx,0
			je ms2_beg_map_detectet
			cmp ecx,_H_SIZE-2
			je m2_xoverflov

m2_begin:
beg_mstage2:
			push ecx
			xor ecx,ecx
			mov cx,[esi+_H_SIZE]	;if eax ??? !!!
			test cl,40h		;cmp na DL
			jne found_dl_m2
			mov eax,[esi-1]
m2l_continue:
			mov ebx,0ffffh	;255 & 255
			sub bh,al	;lx 2
			sub bl,ah	;lx 1
			;bswap eax
			;add bh,ah	;rx 2
			;add bl,al	;rx 1
			;bswap eax
			bswap ebx	;save delta 1pix & 2pix
			shr eax,8	;al  h 2pix; ah  h 1pix 
			or ebx,0ffffh	;255 & 255
			sub bl,ah
			sub bh,al
			shr bl,_H_CORRECTION
			shr bh,_H_CORRECTION

			and ecx,_TERRAIN_MASK + (_TERRAIN_MASK<<8)
			shr ecx,_TERRAIN_OFFSET	
			bswap edx
			mov dx,cx	; free ecx !!!

			push esi
			push edi
			bswap ebx       ;restore delta 1p&2p ;save delta2 1p&2p
;lightCLR 1pix
			movzx esi,dh
			shl esi,2
//			add esi,*lightCLR
			lea ecx,lightCLR
			mov esi,[esi+ecx]	;in esi - ^lightCLR
			movzx ecx,bl
			bswap eax
			add cl,ah
			adc ch,0
			mov bl,[esi+ecx]
			bswap eax
;lightCLR 2pix			
			movzx esi,dl
			shl esi,2
//			add esi,lightCLR
			lea ecx,lightCLR
			mov esi,[esi+ecx]	;in esi - ^lightCLR
			movzx ecx,bh
			add cl,ah
			adc ch,0
			mov bh,[esi+ecx]

			cmp dh,0	;cmp na FloodLevel 1pix
			jne m2l_3
			mov ah,FloodLEVEL
m2l_3:		cmp dl,0	;cmp na FloodLevel 2pix
			jne m2l_4
			mov al,FloodLEVEL
m2l_4:
;calculate 1pix
			movzx esi,dh
			shl esi,9
			lea ecx,palCLR
			add esi,ecx

			mov ecx,256
			mov cl,bl
			bswap ebx	;restore delt2 ;save delta
			sub cl,bl
			sbb ch,0
			bswap edx	;restore hC ;save terrain
			mov bl,0	;not SHADOW_MASK;not OBJSHADOW

			MOV EDI,ECX			;DL
			MOV ECX,grid		;DL
			MOV byte ptr [ecx],0;DL
			INC CL				;DL
			MOV grid,ecx		;DL
			ADD CL,AH
			sub dl,1
			jc m2l_5		;if dl <0 
			cmp dl,ah	;compire na h>hc	; cmp hC & h 1pix
			jbe m2l_5	
M2L_SHADOW_2L:
			shr eDI,1
			add eDI,128d
			mov bl,_SHADOW_MASK
			add ah,_SHADOWHEIGHT
			jc m2l_5$
			cmp dl,ah
			jbe m2l_5$$
M2L_SHADOW_2L_OBJ:
			or bl,_OBJSHADOW
			jmp m2l_5$
m2l_5:		mov dl,ah	;save hC 1pix
			CMP BYTE PTR [ECX],0
			JNE M2L_SHADOW_2L
m2l_5$$:	ADD CL,_SHADOWHEIGHT
			CMP BYTE PTR [ECX],0
			JNE M2L_SHADOW_2L_OBJ
m2l_5$:		
			mov ah,[esi+eDI]	
			
			bswap edx	;restore terrain;save hC
			bswap ebx	;restore delta;save delta2
;calculate 2pix
			movzx esi,dl
			shl esi,9
			lea ecx,palCLR
			add esi,ecx

			mov ecx,256
			mov cl,bh
			bswap ebx	;restore delt2 ;save delta
			sub cl,bh
			sbb ch,0
			bswap edx	;restore hC ;save terrain
			mov bh,0	;not SHADOW_MASK;not OBJSHADOW
			
			MOV EDI,ECX			;DL
			MOV ECX,grid		;DL
			MOV byte ptr [ecx],0;DL
			INC CL				;DL
			MOV byte ptr [ecx],0;DL
			INC CL				;DL
			MOV grid,ecx		;DL
			ADD CL,AL
			sub dl,2
			jc m2l_6
			cmp dl,al	; cmp hC & h 2pix ; cmp hC & h 1pix
			jbe m2l_6		;
M2L_SHADOW_2L_2P:
			shr eDI,1
			add eDI,128d
			mov bh,_SHADOW_MASK
			add al,_SHADOWHEIGHT
			jc m2l_6$
			cmp dl,al
			jbe m2l_6$$
M2L_SHADOW_2L_OBJ_2P:
			or bh,_OBJSHADOW
			jmp m2l_6$
m2l_6:		mov dl,al	;save hC 2pix
			CMP BYTE PTR [ECX],0
			JNE M2L_SHADOW_2L_2P
m2l_6$$:	ADD CL,_SHADOWHEIGHT
			CMP BYTE PTR [ECX],0
			JNE M2L_SHADOW_2L_OBJ_2P
m2l_6$:		mov al,[esi+eDI]
		
//			xchg bl,bh
			bswap ebx
			pop edi
			pop esi
//			mov [edi],al
//			mov [edi+1],ah
			mov [edi],ax
*//*			or [esi+_H_SIZE],bx
			not bx
			and [esi+_H_SIZE],bx
*/
/* WORK			shr ebx,10h
			mov eax,[esi+_H_SIZE]
			and eax,0FFFF7B7Bh
			or eax,ebx
			mov [esi+_H_SIZE],eax

			pop ecx
			sub edi,2
			sub esi,2
			sub ecx,2
			and ecx,7FFh	;_clip_mask_x
			je ms2_beg_map_detectet
			cmp ecx,LowX
			je end_mstage2
			cmp ecx,_H_SIZE-2
			je m2_xoverflov
			jmp beg_mstage2
ms2_beg_map_detectet:
			cmp ecx,LowX
			je end_mstage2
			push ecx
			xor ecx,ecx
			mov cx,[esi+_H_SIZE]	;if eax ??? !!!
			test cl,40h		;cmp na DL
			jne found_dl_m2
			mov eax,[esi]
			shl eax,8
			mov esi,pLineAddr
			add esi,_H_SIZE-1
			mov al,[esi]
			mov esi,pLineAddr
			jmp m2l_continue
m2_xoverflov:
			push ecx
			mov esi,pLineAddr
			add esi,_H_SIZE-2
			xor ecx,ecx
			mov cx,[esi+_H_SIZE]	;if eax ??? !!!
			test cl,40h		;cmp na DL
			jne found_dl_m2_xoverflov
			mov eax,[esi-1]
			rol eax,8
			mov esi,pLineAddr
			mov al,[esi]
			ror eax,8
			add esi,_H_SIZE-2
			mov edi,pColorLine
			add edi,_H_SIZE-2
			jmp m2l_continue

found_dl_m2_xoverflov:
   			mov ax,[esi]
			shl eax,16d
			mov esi,pLineAddr
			mov ax,[esi]
			ror eax,16d
			add esi,_H_SIZE-2
			mov edi,pColorLine
			add edi,_H_SIZE-2
			jmp m2l_continue_2pix
;C A L C U L A T E D    2 LEVEL 
found_dl_m2:
			mov eax,[esi]
m2l_continue_2pix:

			shr ecx,8h
			and cl,_TERRAIN_MASK
			shr cl,_TERRAIN_OFFSET
			mov dh,cl	;free ecx

			push esi
			push edi
;lightCLR DOUBLE LEVEL
			mov ebx,0ffh
			sub bl,ah
			movzx esi,dh
			shl esi,2
			lea ecx,lightCLR
			mov esi,[esi+ecx]
			bswap eax
			add bl,al
			adc bh,0
			mov bl,[esi+ebx]
			bswap eax
;calc pix
			mov bh,0ffh
			sub bh,ah
			shr bh,_H_CORRECTION
			cmp dh,0
			jne m2dl_1
			mov ah,FloodLEVEL
m2dl_1:
			movzx esi,dh
			shl esi,9
			lea ecx,palCLR
			add esi,ecx

			mov ecx,256d
			mov cl,bl
			sub cl,bh
			sbb ch,0

			;mov ebx,0	;not SHADOW_MASK;not OBJSHADOW
			xor ebx,ebx

			mov dh,ah	;Save h DoubleLevel

			MOV EDI,ECX			;DL
			MOV ECX,grid		;DL
			MOV byte ptr [ecx],0;DL
			INC CL				;DL
			MOV byte ptr [ecx],0;DL
			INC CL				;DL
			MOV byte ptr [ecx],0;DL
			INC CL				;DL
			MOV grid,ecx		;DL
			ADD CL,AH
			sub dl,3
			jc m2l_5_DL		;if dl <0 
			cmp dl,ah	;compire na h>hc	; cmp hC & h 1pix
			jbe m2l_5_DL1
M2L_SHADOW_2L_DL:
			shr eDI,1
			add eDI,128d
			mov bl,_SHADOW_MASK
			add ah,_SHADOWHEIGHT
			jc m2l_5$_DL
			cmp dl,ah
			jb m2l_5$$_DL
M2L_SHADOW_2L_OBJ_DL:
////			or bl,_OBJSHADOW
			jmp m2l_5$_DL
m2l_5_DL:	mov dl,al	
m2l_5_DL1:
			CMP BYTE PTR [ECX],0
			JNE M2L_SHADOW_2L_DL
m2l_5$$_DL:	ADD CL,_SHADOWHEIGHT
			CMP BYTE PTR [ECX],0
			JNE M2L_SHADOW_2L_OBJ_DL
m2l_5$_DL:
			cmp dl,al
			jb m2l_5$$$_DL
			mov dl,al
m2l_5$$$_DL:
			mov ah,[esi+eDI]
			and ebx,0ffh	;ZERO EBX
			mov al,ah
			mov bh,bl

			pop edi
			pop esi
			mov [edi],ax
			mov eax,[esi+_H_SIZE]
			and eax,0FFFF7B7Bh
			or eax,ebx
			mov [esi+_H_SIZE],eax

// SHADOW DOUBLE LEVEL			
			mov ecx,grid
			sub dh,_SS_WIDTH+1
			jb m2_cut_shadow
			add cl,dh
			mov al,_SS_WIDTH+1
			add al,cl
			cmp al,cl
			jb m2_overflov_shadow
			mov eax,01010101h
			mov [ecx],eax
			mov [ecx+4],eax
			mov [ecx+8],eax
			mov [ecx+0Ch],eax
			mov [ecx+10h],al
m2_ret_cut_over_shadow:
			pop ecx
			sub edi,2
			sub esi,2
			sub ecx,2
			and ecx,7FFh	;_clip_mask_x
;OPTIMIZATION !!! V
			je ms2_beg_map_detectet
			cmp ecx,LowX
			je end_mstage2
			cmp ecx,_H_SIZE-2
			je m2_xoverflov
			jmp beg_mstage2
// SHADOW DOUBLE LEVEL			
m2_cut_shadow:
			mov al,_SS_WIDTH+1
			add al,dh
			je m2_ret_cut_over_shadow
m2_cut_shadow_loop:
			mov byte ptr [ecx],1
			inc cl
			dec al
			je m2_ret_cut_over_shadow
			jmp m2_cut_shadow_loop
m2_overflov_shadow:
			mov al,_SS_WIDTH+1
			jmp m2_cut_shadow_loop


end_mstage2:

//////////////////////////////////////////////////////
;PostStage2
			cmp ecx,0
			je ps2_beg_map_detectet
			cmp ecx,_H_SIZE-2
			je p2_xoverflov

p2_begin:
beg_pstage2:
			push ecx
			xor ecx,ecx
			mov cx,[esi+_H_SIZE]	;if eax ??? !!!
			test cl,40h		;cmp na DL
			jne found_dl_p2
			mov eax,[esi-1]
p2l_continue:
			test ecx,_SHADOW_MASK*256+_SHADOW_MASK
			jne p2l_continue2
			cmp psflag,0
			jne end_pstage
p2l_continue2:
			mov ebx,0ffffh	;255 & 255
			sub bh,al	;lx 2
			sub bl,ah	;lx 1
			bswap ebx	;save delta 1pix & 2pix
			shr eax,8	;al  h 2pix; ah  h 1pix 
			sub MAX_H_RENDER,1
			cmp MAX_H_RENDER,ah
			jbe end_pstage
			or ebx,0ffffh	;255 & 255
			sub bl,ah
			sub bh,al
			shr bl,_H_CORRECTION
			shr bh,_H_CORRECTION

			and ecx,_TERRAIN_MASK + (_TERRAIN_MASK<<8)
			shr ecx,_TERRAIN_OFFSET	
			bswap edx
			mov dx,cx	; free ecx !!!

			push esi
			push edi
			bswap ebx       ;restore delta 1p&2p ;save delta2 1p&2p
;lightCLR 1pix
			movzx esi,dh
			shl esi,2
			lea ecx,lightCLR
			mov esi,[esi+ecx]	;in esi - ^lightCLR
			movzx ecx,bl
			bswap eax
			add cl,ah
			adc ch,0
			mov bl,[esi+ecx]
			bswap eax
;lightCLR 2pix			
			movzx esi,dl
			shl esi,2
			lea ecx,lightCLR
			mov esi,[esi+ecx]	;in esi - ^lightCLR
			movzx ecx,bh
			add cl,ah
			adc ch,0
			mov bh,[esi+ecx]

			cmp dh,0	;cmp na FloodLevel 1pix
			jne p2l_3
			mov ah,FloodLEVEL
p2l_3:		cmp dl,0	;cmp na FloodLevel 2pix
			jne p2l_4
			mov al,FloodLEVEL
p2l_4:
;calculate 1pix
			movzx esi,dh
			shl esi,9
			lea ecx,palCLR
			add esi,ecx

			mov ecx,256
			mov cl,bl
			bswap ebx	;restore delt2 ;save delta
			sub cl,bl
			sbb ch,0
			bswap edx	;restore hC ;save terrain
			mov bl,0	;not SHADOW_MASK;not OBJSHADOW

			MOV EDI,ECX			;DL
			MOV ECX,grid		;DL
			MOV byte ptr [ecx],0;DL
			INC CL				;DL
			MOV grid,ecx		;DL
			ADD CL,AH
			sub dl,1
			jc p2l_5		;if dl <0 
			cmp dl,ah	;compire na h>hc	; cmp hC & h 1pix
			jbe p2l_5	
p2L_SHADOW_2L:
			shr eDI,1
			add eDI,128d
			mov bl,_SHADOW_MASK
			add ah,_SHADOWHEIGHT
			jc p2l_5$
			cmp dl,ah
			jb p2l_5$$
p2L_SHADOW_2L_OBJ:
			or bl,_OBJSHADOW
			jmp p2l_5$
p2l_5:
			mov psflag,1h
			mov dl,ah	;save hC 1pix
			CMP BYTE PTR [ECX],0
			JNE p2L_SHADOW_2L
p2l_5$$:	ADD CL,_SHADOWHEIGHT
			CMP BYTE PTR [ECX],0
			JNE p2L_SHADOW_2L_OBJ
p2l_5$:		
			mov ah,[esi+eDI]	
			
			bswap edx	;restore terrain;save hC
			bswap ebx	;restore delta;save delta2
;calculate 2pix
			sub MAX_H_RENDER,2
			cmp MAX_H_RENDER,ah
			jbe end_pstage$
			movzx esi,dl
			shl esi,9
			lea ecx,palCLR
			add esi,ecx

			mov ecx,256
			mov cl,bh
			bswap ebx	;restore delt2 ;save delta
			sub cl,bh
			sbb ch,0
			bswap edx	;restore hC ;save terrain
			mov bh,0	;not SHADOW_MASK;not OBJSHADOW
			
			MOV EDI,ECX			;DL
			MOV ECX,grid		;DL
			MOV byte ptr [ecx],0;DL
			INC CL				;DL
			MOV byte ptr [ecx],0;DL
			INC CL				;DL
			MOV grid,ecx		;DL
			ADD CL,AL
			sub dl,2
			jc p2l_6
			cmp dl,al	; cmp hC & h 2pix ; cmp hC & h 1pix
			jbe p2l_6		;
p2L_SHADOW_2L_2P:
			shr eDI,1
			add eDI,128d
			mov bh,_SHADOW_MASK
			add al,_SHADOWHEIGHT
			jc p2l_6$
			cmp dl,al
			jb p2l_6$$
p2L_SHADOW_2L_OBJ_2P:
			or bh,_OBJSHADOW
			jmp p2l_6$
p2l_6:
			mov psflag,1h
			mov dl,al	;save hC 2pix
			CMP BYTE PTR [ECX],0
			JNE p2L_SHADOW_2L_2P
p2l_6$$:	ADD CL,_SHADOWHEIGHT
			CMP BYTE PTR [ECX],0
			JNE p2L_SHADOW_2L_OBJ_2P
p2l_6$:		mov al,[esi+eDI]
		
			bswap ebx
			pop edi
			pop esi
			mov [edi],ax
			shr ebx,10h
			mov eax,[esi+_H_SIZE]
			and eax,0FFFF7B7Bh
			or eax,ebx
			mov [esi+_H_SIZE],eax

			pop ecx
			sub edi,2
			sub esi,2
			sub ecx,2
			and ecx,7FFh	;_clip_mask_x
			je ps2_beg_map_detectet
			cmp ecx,_H_SIZE-2
			je p2_xoverflov
			jmp beg_pstage2
ps2_beg_map_detectet:
			push ecx
			xor ecx,ecx
			mov cx,[esi+_H_SIZE]	;if eax ??? !!!
			test cl,40h		;cmp na DL
			jne found_dl_p2
			mov eax,[esi]
			shl eax,8
			mov esi,pLineAddr
			add esi,_H_SIZE-1
			mov al,[esi]
			mov esi,pLineAddr
			jmp p2l_continue
p2_xoverflov:
			push ecx
			mov esi,pLineAddr
			add esi,_H_SIZE-2
			xor ecx,ecx
			mov cx,[esi+_H_SIZE]	;if eax ??? !!!
			test cl,40h		;cmp na DL
			jne found_dl_p2_xoverflov
			mov eax,[esi-1]
			rol eax,8
			mov esi,pLineAddr
			mov al,[esi]
			ror eax,8
			add esi,_H_SIZE-2
			mov edi,pColorLine
			add edi,_H_SIZE-2
			jmp p2l_continue

found_dl_p2_xoverflov:
   			mov ax,[esi]
			shl eax,16d
			mov esi,pLineAddr
			mov ax,[esi]
			ror eax,16d
			add esi,_H_SIZE-2
			mov edi,pColorLine
			add edi,_H_SIZE-2
			jmp p2l_continue_2pix
;C A L C U L A T E D    2 LEVEL 
found_dl_p2:
			mov eax,[esi]
p2l_continue_2pix:
			test ecx,_SHADOW_MASK*256+_SHADOW_MASK
			jne p2l_continue3
			cmp psflag,0
			jne end_pstage
p2l_continue3:
			sub MAX_H_RENDER,3
			cmp MAX_H_RENDER,al
			jbe end_pstage

			shr ecx,8h
			and cl,_TERRAIN_MASK
			shr cl,_TERRAIN_OFFSET
			mov dh,cl	;free ecx

			push esi
			push edi
;lightCLR DOUBLE LEVEL
			mov ebx,0ffh
			sub bl,ah
			movzx esi,dh
			shl esi,2
			lea ecx,lightCLR
			mov esi,[esi+ecx]
			bswap eax
			add bl,al
			adc bh,0
			mov bl,[esi+ebx]
			bswap eax
;calc pix
			mov bh,0ffh
			sub bh,ah
			shr bh,_H_CORRECTION
			cmp dh,0
			jne p2dl_1
			mov ah,FloodLEVEL
p2dl_1:
			movzx esi,dh
			shl esi,9
			lea ecx,palCLR
			add esi,ecx

			mov ecx,256d
			mov cl,bl
			sub cl,bh
			sbb ch,0

			;mov ebx,0	;not SHADOW_MASK;not OBJSHADOW
			xor ebx,ebx

			mov dh,ah	;Save h DoubleLevel

			MOV EDI,ECX			;DL
			MOV ECX,grid		;DL
			MOV byte ptr [ecx],0;DL
			INC CL				;DL
			MOV byte ptr [ecx],0;DL
			INC CL				;DL
			MOV byte ptr [ecx],0;DL
			INC CL				;DL
			MOV grid,ecx		;DL
			ADD CL,AH
			sub dl,3
			jc p2l_5_DL		;if dl <0 
			cmp dl,ah	;compire na h>hc	; cmp hC & h 1pix
			jbe p2l_5_DL1
p2L_SHADOW_2L_DL:
			shr eDI,1
			add eDI,128d
			mov bl,_SHADOW_MASK
			add ah,_SHADOWHEIGHT
			jc p2l_5$_DL
			cmp dl,ah
			jb p2l_5$$_DL
p2L_SHADOW_2L_OBJ_DL:
////			or bl,_OBJSHADOW
			jmp p2l_5$_DL
p2l_5_DL:	mov dl,al	
p2l_5_DL1:
			mov psflag,1h
			CMP BYTE PTR [ECX],0
			JNE p2L_SHADOW_2L_DL
p2l_5$$_DL:	ADD CL,_SHADOWHEIGHT
			CMP BYTE PTR [ECX],0
			JNE p2L_SHADOW_2L_OBJ_DL
p2l_5$_DL:
			cmp dl,al
			jb p2l_5$$$_DL
			mov dl,al
p2l_5$$$_DL:
			mov ah,[esi+eDI]
			and ebx,0ffh	;ZERO EBX
			mov al,ah
			mov bh,bl

			pop edi
			pop esi
			mov [edi],ax
			mov eax,[esi+_H_SIZE]
			and eax,0FFFF7B7Bh
			or eax,ebx
			mov [esi+_H_SIZE],eax

// SHADOW DOUBLE LEVEL			
			mov ecx,grid
			sub dh,_SS_WIDTH+1
			jb ps2_cut_shadow
			add cl,dh
			mov al,_SS_WIDTH+1
			add al,cl
			cmp al,cl
			jb ps2_overflov_shadow
			mov eax,01010101h
			mov [ecx],eax
			mov [ecx+4],eax
			mov [ecx+8],eax
			mov [ecx+0Ch],eax
			mov [ecx+10h],al
ps2_ret_cut_over_shadow:

			pop ecx
			sub edi,2
			sub esi,2
			sub ecx,2
			and ecx,7FFh	;_clip_mask_x
;OPTIMIZATION !!! V
			je ps2_beg_map_detectet
			cmp ecx,_H_SIZE-2
			je p2_xoverflov
			jmp beg_pstage2
// SHADOW DOUBLE LEVEL			
ps2_cut_shadow:
			mov al,_SS_WIDTH+1
			add al,dh
			je ps2_ret_cut_over_shadow
ps2_cut_shadow_loop:
			mov byte ptr [ecx],1
			inc cl
			dec al
			je ps2_ret_cut_over_shadow
			jmp ps2_cut_shadow_loop
ps2_overflov_shadow:
			mov al,_SS_WIDTH+1
			jmp ps2_cut_shadow_loop

end_pstage$:
			pop edi
end_pstage$$:
			pop	esi
end_pstage:
			pop ecx



	popad
		}*/
	}
}
