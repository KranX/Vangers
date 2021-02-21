#include "../src/global.h"

#include "sqint.h"
#include "../src/common.h"

#include "../src/terra/vmap.h"
#include "../src/terra/world.h"
#include "../src/terra/render.h"

#include "missed.h"

struct PartParameters {
	int minAlt;
	int maxAlt;
	int noise;
	int minsquare;
	};

/* ----------------------------- EXTERN SECTION ---------------------------- */
extern int RestoreLog,CGenLog,BorderLog,MobilityLog,InitLog,ROLog,ShotLog,GlassLog,WHLog;
extern int WaterPrm;
extern int DriftPower;
extern int DriftNoise;
extern int RenderingLayer;
extern int ShotPrm,GlassPrm;
extern int StartshotPrm;
extern uchar* shadowParent;
extern uchar FloodLevel;
/* --------------------------- PROTOTYPE SECTION --------------------------- */
void LINE_render(int y);
void RenderPrepare(void);
void ExpandBuffer(unsigned char* InBuffer,unsigned char* OutBuffer);
void InitSplay(XStream&);
/* --------------------------- DEFINITION SECTION -------------------------- */
const int MAX_POWER_Y = 15;
const int MAX_POWER = 10;
const int WPART_POWER_MAX = 7;

const int FLOOD_INIT = 80;

const int CLUSTER_SIZE = 10;
const int CLUSTER_MAXDELTA = 10;
const int CLUSTER_HOLE = 1;

int NOISE_AMPL;
int DEFAULT_TERRAIN = MAIN_TERRAIN;

static uint r_preRNDVAL[MAX_POWER];
static uint m_preRNDVAL[MAX_POWER];
static uint r_cycleRNDVAL[1 << MAX_POWER_Y - WPART_POWER_MAX][MAX_POWER];
static uint m_cycleRNDVAL[1 << MAX_POWER_Y - WPART_POWER_MAX][MAX_POWER];

static uchar* alt_map;
static uchar* color_map;
static uchar* temp_map;
static uchar* surf_map;
static uchar* proto_map;
static uchar* current_map;

static uchar lastlineA[map_size_x];
static uchar lastlineC[map_size_x];
static uchar lastlineF[map_size_x];
static uchar* r_net_map;
static uchar* m_net_map;
static uchar* proto_m_net_map;
static uchar* new_m_net_map;
static uint Stage;
static PartParameters PartPrm[1 << MAX_POWER_Y - WPART_POWER_MAX];
static int geoUsed[1 << MAX_POWER_Y - WPART_POWER_MAX],geoFirst;

static int min_alt,max_alt;
static int Cmin_alt = MIN_ALT;
static int Cmax_alt = MAX_ALT;
static int noiselevel;
static int minsquare;

static int* xx[2];
static int* yy[2];

int Verbose = 1;

unsigned storeRNDVAL,storerealRNDVAL;

static void r_net_init(void)
{
	if(Verbose) XCon < "\nRoughNetInit...";
	register uint x,y;

	for(x = 0;x < POWER;x++) r_preRNDVAL[x] = realRND(0xFFFFFFFF);
	for(y = 0;y < PART_MAX;y++)
		for(x = 0;x < POWER;x++)
			r_cycleRNDVAL[y][x] = realRND(0xFFFFFFFF);

	PartParameters* p;
	for(y = 0;y < map_size_y/QUANT;y++){
		p = &PartPrm[y*QUANT/part_map_size_y];
		for(x = 0;x < map_size_x/QUANT;x++)
			switch(WORLD_TYPE){
				case 0:
					if(!realRND(10))
						R_NET_MAP(y,x) = (max_alt - 1);
					else
						R_NET_MAP(y,x) = (uchar)((max_alt - 1)*(x + y)/(map_size_x + map_size_y));
					break;
				case 1:
//					  R_NET_MAP(y,x) = (uchar)((max_alt - 1)*(x + y)/(map_size_x + map_size_y));
					R_NET_MAP(y,x) = (uchar)realRND(max_alt - 1);
					break;
				case 2:
//					  R_NET_MAP(y,x) = (uchar)((max_alt - 1)*((x + y)*QUANT)/abs(x*QUANT - map_size_x/2 - 1));
					R_NET_MAP(y,x) = (uchar)realRND(max_alt - 1);
					break;
				case 3:
//					  R_NET_MAP(y,x) = (uchar)((max_alt - 1)*((x + y)*QUANT)/abs(x*QUANT - map_size_x/2 - 1));
					R_NET_MAP(y,x) = p -> minAlt + (uchar)realRND(p -> maxAlt - p -> minAlt);
					break;
				}
		}
}

static void m_net_init(void)
{
	if(Verbose) XCon < "\nMapNetInit...";
	register uint x,y;

	for(x = 0;x < POWER;x++) m_preRNDVAL[x] = realRND(0xFFFFFFFF);
	for(y = 0;y < PART_MAX;y++)
		for(x = 0;x < POWER;x++)
			m_cycleRNDVAL[y][x] = realRND(0xFFFFFFFF);

	PartParameters* p;
	for(y = 0;y < map_size_y/QUANT;y++){
		p = &PartPrm[y*QUANT/part_map_size_y];
		for(x = 0;x < map_size_x/QUANT;x++)
			M_NET_MAP(y,x) = p -> minAlt + (uchar)realRND(p -> maxAlt - p -> minAlt);
		}
}

static void rough_init(void)
{
	if(Verbose) XCon < "\nRoughInit...";
	register uint x,y,i,j;

	for(y = 0,j = Stage*(part_map_size_y/QUANT);y <= part_map_size_y + QUANT;y += QUANT,j++){
		if(j == PART_MAX*(part_map_size_y/QUANT)) j = 0;
		for(x = 0,i = 0;x < map_size_x;x += QUANT,i++)
			COLOR_MAP(y,x) = R_NET_MAP(j,i);
		}
}

static void generate_roughness_map(void)
{
	if(MINSQUARE > 1){
		RNDVAL = r_cycleRNDVAL[Stage][0];
		register uint i;
		uchar* pc = color_map;
		uchar* pa = alt_map;
		for(i = 0;i < part_map_size + map_size_x;i++){
			*pc++ = random(noiselevel);
			*pa++ = random(noiselevel);
			}
		}

	rough_init();

	if(Verbose) XCon < "\nGenerateRoughnessMap...";

	int which = 0;
	int square_size,x1,y1,x2,y2;
	int p1,p2,p3,p4;
	int random_center,random_range,ind;
	int i1,i2,i3,i4;
	int i1_9,i2_9,i3_9,i4_9;
	int i1_3,i2_3,i3_3,i4_3;

	for(ind = 0,square_size = QUANT;square_size > MINSQUARE;square_size >>= 1,ind++){
		RNDVAL = r_preRNDVAL[ind];
		for(y1 = 0;y1 <= (int)part_map_size_y;y1 += square_size){
			if(y1 == square_size) RNDVAL = r_cycleRNDVAL[Stage][ind];
			else if(y1 == (int)part_map_size_y) RNDVAL = r_preRNDVAL[ind];
			for(x1 = 0;x1 < map_size_x;x1 += square_size){
				x2 = (x1 + square_size) & clip_mask_x;
				y2 = y1 + square_size;
				if(!which){
					i1 = COLOR_MAP(y1,x1);
					i2 = COLOR_MAP(y1,x2);
					i3 = COLOR_MAP(y2,x1);
					i4 = COLOR_MAP(y2,x2);
					}
				else {
					i1 = ALT_MAP(y1,x1);
					i2 = ALT_MAP(y1,x2);
					i3 = ALT_MAP(y2,x1);
					i4 = ALT_MAP(y2,x2);
					}
				i1_3 = i1*3; i1_9 = i1_3*3;
				i2_3 = i2*3; i2_9 = i2_3*3;
				i3_3 = i3*3; i3_9 = i3_3*3;
				i4_3 = i4*3; i4_9 = i4_3*3;
				p1 = i1_9 + i2_3 + i3_3 + i4 >> 4;
				p2 = i1_3 + i2_9 + i3 + i4_3 >> 4;
				p3 = i1_3 + i2 + i3_9 + i4_3 >> 4;
				p4 = i1 + i2_3 + i3_3 + i4_9 >> 4;

				random_center = square_size;
				random_range = random_center << 1;
				if(BIZARRE_ROUGHNESS_MAP){
					p1 += random(random_center) + random_range - (Cmax_alt - p1)/6;
					p2 += random(random_center) + random_range - (Cmax_alt - p2)/6;
					p3 += random(random_center) + random_range - (Cmax_alt - p3)/6;
					p4 += random(random_center) + random_range - (Cmax_alt - p4)/6;

					if(p1 < Cmin_alt) p1 += Cmin_alt + 1; else if(p1 > Cmax_alt) p1 %= Cmax_alt + 1;
					if(p2 < Cmin_alt) p2 += Cmin_alt + 1; else if(p2 > Cmax_alt) p2 %= Cmax_alt + 1;
					if(p3 < Cmin_alt) p3 += Cmin_alt + 1; else if(p3 > Cmax_alt) p3 %= Cmax_alt + 1;
					if(p4 < Cmin_alt) p4 += Cmin_alt + 1; else if(p4 > Cmax_alt) p4 %= Cmax_alt + 1;
					}
				else {
					p1 += random(random_range) - random_center;
					p2 += random(random_range) - random_center;
					p3 += random(random_range) - random_center;
					p4 += random(random_range) - random_center;

					if(p1 < Cmin_alt) p1 = Cmin_alt; else if(p1 > Cmax_alt) p1 = Cmax_alt;
					if(p2 < Cmin_alt) p2 = Cmin_alt; else if(p2 > Cmax_alt) p2 = Cmax_alt;
					if(p3 < Cmin_alt) p3 = Cmin_alt; else if(p3 > Cmax_alt) p3 = Cmax_alt;
					if(p4 < Cmin_alt) p4 = Cmin_alt; else if(p4 > Cmax_alt) p4 = Cmax_alt;
					}
				x2 = (x1 + (square_size >> 1)) & clip_mask_x;
				y2 = y1 + (square_size >> 1);

				if(!which){
					ALT_MAP(y1,x1) = (uchar)p1;
					ALT_MAP(y1,x2) = (uchar)p2;
					ALT_MAP(y2,x1) = (uchar)p3;
					ALT_MAP(y2,x2) = (uchar)p4;
					}
				else {
					COLOR_MAP(y1,x1) = (uchar)p1;
					COLOR_MAP(y1,x2) = (uchar)p2;
					COLOR_MAP(y2,x1) = (uchar)p3;
					COLOR_MAP(y2,x2) = (uchar)p4;
					}
				}
			}
		which = 1 - which;
		}

	if(!which)
		memcpy(temp_map,color_map,part_map_size + map_size_x);
	else
		memcpy(temp_map,alt_map,part_map_size + map_size_x);
}

static void map_init(void)
{
	if(Verbose) XCon < "\nMapInit...";
	register uint i,j,x,y;

	for(y = 0,j = Stage*(part_map_size_y/QUANT);y <= part_map_size_y + QUANT;y += QUANT,j++){
		if(j == PART_MAX*(part_map_size_y/QUANT)) j = 0;
		for(x = 0,i = 0;x < map_size_x;x += QUANT,i++)
			COLOR_MAP(y,x) = M_NET_MAP(j,i);
		}
}

static void generate_alt_map(void)
{
	int which = 0;
	int square_size,x1,y1;
	int x2,y2;
	int p1,p2,p3,p4;
	int random_center,random_range,ind;
	int i1,i2,i3,i4;
	int i1_9,i2_9,i3_9,i4_9;
	int i1_3,i2_3,i3_3,i4_3;

	generate_roughness_map();
	map_init();

	if(Verbose) XCon < "\nGenerateAltMap...";

	for(ind = 0,square_size = QUANT;square_size > 1;square_size >>= 1,ind++){
		RNDVAL = m_preRNDVAL[ind];
		for(y1 = 0;y1 <= (int)part_map_size_y;y1 += square_size){
			if(y1 == square_size) RNDVAL = m_cycleRNDVAL[Stage][ind];
			else if(y1 == (int)part_map_size_y) RNDVAL = m_preRNDVAL[ind];
			for(x1 = 0;x1 < map_size_x;x1 += square_size){
				x2 = (x1 + square_size) & clip_mask_x;
				y2 = y1 + square_size;
				if(!which){
					i1 = COLOR_MAP(y1,x1);
					i2 = COLOR_MAP(y1,x2);
					i3 = COLOR_MAP(y2,x1);
					i4 = COLOR_MAP(y2,x2);
					}
				else {
					i1 = ALT_MAP(y1,x1);
					i2 = ALT_MAP(y1,x2);
					i3 = ALT_MAP(y2,x1);
					i4 = ALT_MAP(y2,x2);
					}
				i1_3 = i1*3; i1_9 = i1_3*3;
				i2_3 = i2*3; i2_9 = i2_3*3;
				i3_3 = i3*3; i3_9 = i3_3*3;
				i4_3 = i4*3; i4_9 = i4_3*3;
				p1 = i1_9 + i2_3 + i3_3 + i4 >> 4;
				p2 = i1_3 + i2_9 + i3 + i4_3 >> 4;
				p3 = i1_3 + i2 + i3_9 + i4_3 >> 4;
				p4 = i1 + i2_3 + i3_3 + i4_9 >> 4;
				random_center = square_size*TEMP_MAP(y1,x1)/MESH;
				random_range = random_center << 1;
				if(BIZARRE_ALT_MAP){
					p1 += (random(random_range) - random_center + p1/4);
					p2 += (random(random_range) - random_center + p2/4);
					p3 += (random(random_range) - random_center + p3/4);
					p4 += (random(random_range) - random_center + p4/4);

					if(p1 < Cmin_alt) p1 = Cmin_alt; else if(p1 > Cmax_alt) p1 -= Cmax_alt + 1;
					if(p2 < Cmin_alt) p2 = Cmin_alt; else if(p2 > Cmax_alt) p2 -= Cmax_alt + 1;
					if(p3 < Cmin_alt) p3 = Cmin_alt; else if(p3 > Cmax_alt) p3 -= Cmax_alt + 1;
					if(p4 < Cmin_alt) p4 = Cmin_alt; else if(p4 > Cmax_alt) p4 -= Cmax_alt + 1;
					}
				else {
					p1 += (random(random_range) - random_center);
					p2 += (random(random_range) - random_center);
					p3 += (random(random_range) - random_center);
					p4 += (random(random_range) - random_center);

					if(p1 < Cmin_alt) p1 = Cmin_alt; else if(p1 > Cmax_alt) p1 = Cmax_alt;
					if(p2 < Cmin_alt) p2 = Cmin_alt; else if(p2 > Cmax_alt) p2 = Cmax_alt;
					if(p3 < Cmin_alt) p3 = Cmin_alt; else if(p3 > Cmax_alt) p3 = Cmax_alt;
					if(p4 < Cmin_alt) p4 = Cmin_alt; else if(p4 > Cmax_alt) p4 = Cmax_alt;
					}
				x2 = (x1 + (square_size >> 1)) & clip_mask_x;
				y2 = y1 + (square_size >> 1);

				if(!which){
					ALT_MAP(y1,x1) = (uchar)p1;
					ALT_MAP(y1,x2) = (uchar)p2;
					ALT_MAP(y2,x1) = (uchar)p3;
					ALT_MAP(y2,x2) = (uchar)p4;
					}
				else {
					COLOR_MAP(y1,x1) = (uchar)p1;
					COLOR_MAP(y1,x2) = (uchar)p2;
					COLOR_MAP(y2,x1) = (uchar)p3;
					COLOR_MAP(y2,x2) = (uchar)p4;
					}
				}
			}
		which = 1 - which;
		}

	if(!which) memcpy(alt_map,color_map,part_map_size);
}

static void calc_Flood(int x0,int y0,int level,int ttype,int ti = 0)
{
	const int N = 1024 << 6;

	if(!xx[0]){
		xx[0] = new int[N];
		xx[1] = new int[N];
		yy[0] = new int[N];
		yy[1] = new int[N];
		}

	int index = 0;
	int num[2];
	xx[0][0] = x0;
	yy[0][0] = y0;
	num[0] = 1;

	uchar** lt = vMap -> lineT;
	uchar* pa,*pf,*pa0;
	int na,h,t;

	int log = 1,i,j,x,y;
	while(log){
		log = 0;
		for(j = 0,i = 0;i < num[index];i++){
			pa0 = pa = lt[y = yy[index][i]];
			if(pa){
				pa += (x = xx[index][i]);
				pf = pa + H_SIZE;
				na = level;
				h = GET_UP_ALT(pf,*pa,pa0,x);
				if(ti) na += (level - h)*DriftPower/16 + realRND(DriftNoise);
				t = GET_REAL_TERRAIN_TYPE(pf,x);
				if(h < level && t != ttype && j < N - 4 && na >= MiniLevel && na <= MaxiLevel && ActiveTerrain[t >> TERRAIN_OFFSET] && h >= MiniLevel && h <= MaxiLevel){
					if(SET_REAL_TERRAIN(pf,ttype,x)) log = 1;

					if(ti) *pa = (uchar)na;

					xx[1 - index][j] = (x - 1) & clip_mask_x;
					yy[1 - index][j] = y;
					j++;
					xx[1 - index][j] = (x + 1) & clip_mask_x;
					yy[1 - index][j] = y;
					j++;
					xx[1 - index][j] = x;
					yy[1 - index][j] = (y - 1) & clip_mask_y;
					j++;
					xx[1 - index][j] = x;
					yy[1 - index][j] = (y + 1) & clip_mask_y;
					j++;
					}
				}
			}
		index = 1 - index;
		num[index] = j;
		}
}

void calc_DownFlood(int x0,int y0,uchar level,int ttype,int ti = 0)
{
	const int N = 1024 << 6;

	if(!xx[0]){
		xx[0] = new int[N];
		xx[1] = new int[N];
		yy[0] = new int[N];
		yy[1] = new int[N];
		}

	int index = 0;
	int num[2];
	xx[0][0] = x0;
	yy[0][0] = y0;
	num[0] = 1;

	uchar** lt = vMap -> lineT;
	uchar* pa,*pf,*pa0;
	int na,h,t;

	int log = 1,i,j,x,y;
	while(log){
		log = 0;
		for(j = 0,i = 0;i < num[index];i++){
			pa0 = pa = lt[y = yy[index][i]];
			if(pa){
				pa += (x = xx[index][i]);
				pf = pa + H_SIZE;
				na = level;
				h = GET_DOWN_ALT(pf,*pa,pa0,x);
				if(ti) na += (level - h)*DriftPower/16 + realRND(DriftNoise);
				t = GET_REAL_DOWNTERRAIN_TYPE(pf,x);
				if(h < level && t != ttype && j < N - 4 && na >= MiniLevel && na <= MaxiLevel && ActiveTerrain[t >> TERRAIN_OFFSET] && h >= MiniLevel && h <= MaxiLevel){
					if(SET_REAL_DOWNTERRAIN(pf,ttype,x)) log = 1;

					if(ti) *pa = (uchar)na;

					xx[1 - index][j] = (x - 1) & clip_mask_x;
					yy[1 - index][j] = y;
					j++;
					xx[1 - index][j] = (x + 1) & clip_mask_x;
					yy[1 - index][j] = y;
					j++;
					xx[1 - index][j] = x;
					yy[1 - index][j] = (y - 1) & clip_mask_y;
					j++;
					xx[1 - index][j] = x;
					yy[1 - index][j] = (y + 1) & clip_mask_y;
					j++;
					}
				}
			}
		index = 1 - index;
		num[index] = j;
		}
}

void WORLD_Flood(int x0,int y0,int ttype)
{
	uchar level = FloodLvl[y0 >> WPART_POWER];
	if(RenderingLayer == UP_LAYER){
		calc_Flood(x0,y0,level,(TERRAIN_MAX - 1) << TERRAIN_OFFSET);
		calc_Flood(x0,y0,level,ttype);
		}
	else {
		calc_DownFlood(x0,y0,level,(TERRAIN_MAX - 1) << TERRAIN_OFFSET);
		calc_DownFlood(x0,y0,level,ttype);
		}
}

void WORLD_Drift(int x0,int y0,uchar level,int ttype)
{
	if(RenderingLayer == UP_LAYER){
		calc_Flood(x0,y0,level,(TERRAIN_MAX - 1) << TERRAIN_OFFSET);
		calc_Flood(x0,y0,level,ttype,1);
		}
	else {
		calc_DownFlood(x0,y0,level,(TERRAIN_MAX - 1) << TERRAIN_OFFSET);
		calc_DownFlood(x0,y0,level,ttype,1);
		}
}

static void calc_color_map(void)
{
	if(Verbose) XCon < "\nCalcColorMap...";
	WORLD_colcalc(Stage << WPART_POWER,YCYCL(((Stage + 1) << WPART_POWER)),0);
}

void WORLD_colcalc(int y0,int y1,int isColor)
{
	int dnD = RenderingLayer == DOWN_LAYER ? 1 : 0;
	int x,y;
	uchar** lt = vMap -> lineT;
	uchar** ltc = vMap -> lineTcolor;
	uchar h,type,hd,lxVal,rxVal,hM,level;
	int hdC;
	uchar* pa,*pc,*pf,*pa0,*grid,*pc0,*pf0,*pff;
	for(y = y0;y != y1;y = YCYCL(y + 1)){
		pa0 = pa = lt[y]; pa += H_SIZE - 1;
		pf0 = pa0 + H_SIZE;
		pf = pa + H_SIZE;
		pc0 = ltc[y];
		pc = pc0 + H_SIZE - 1;
		memset(grid = shadowParent + H_SIZE,0,2*H_SIZE);
		hM = 0;
		hdC = 0;
		for(x = map_size_x - 1;x >= 0;x--,pa--,pc--,pf--){
			if(!(*pf & 3) && !(*(pf - 1) & 3)){
				pff = pf0 + XCYCL(x - 2);
				if((*pff & DOUBLE_LEVEL) && ((*pff & 3) || (*(pff - 1) & 3))){
					*pf |= DOUBLE_LEVEL;
					*(pf - 1) |= DOUBLE_LEVEL;
					}
				else {
					*pf &= ~DOUBLE_LEVEL;
					*(pf - 1) &= ~DOUBLE_LEVEL;
					}
				}
			if(*pf & DOUBLE_LEVEL){
				type = (*(pf - dnD) & TERRAIN_MASK) >> TERRAIN_OFFSET;
				lxVal = *(pa - dnD);
				rxVal = *(pa0 + XCYCL(x + 2 - dnD));
/*
//					исправление 0-полоски
					if(x == map_size_x - 1 && (*(pf - 2) & DOUBLE_LEVEL) && (*(pf0 + 1) & DOUBLE_LEVEL)){
					*pa = (uchar)((int)*(pa - 2) + (int)rxVal >> 1);
					type = *(pf - 2) & TERRAIN_MASK;
					SET_TERRAIN(*pf,type);
					type >>= TERRAIN_OFFSET;
					}
*/
				h = *(pa - dnD);
				hd = *(pa - 1);

				grid += 3;
				hdC -= 3;
#ifndef TERRAIN16
				if(grid[h + 32]){
					*pf |= OBJSHADOW;
					*(pf - 1) |= OBJSHADOW;
					}
				else {
					*pf &= ~OBJSHADOW;
					*(pf - 1) &= ~OBJSHADOW;
					}
#endif
				if(grid[h]){
					if(isColor) *(pc - 1) = *pc = palCLR[type][256 + (lightCLR[type][255 - (lxVal - rxVal)] - (255 - h >> H_CORRECTION) >> 1)];
					*pf |= SHADOW_MASK;
					*(pf - 1) |= SHADOW_MASK;
					}
				else {
					if(isColor) *(pc - 1) = *pc = palCLR[type][256 + lightCLR[type][255 - (lxVal - rxVal)] - (255 - h >> H_CORRECTION)]; 
					*pf &= ~SHADOW_MASK;
					*(pf - 1) &= ~SHADOW_MASK;
					}
				if(hd > hdC){
					if(hdC < 0) hdC = 0;
					memset(grid + hdC,1,hd - hdC);
					hdC = hd;
					}
				if(!dnD) memset(grid + h - SS_WIDTH,2,SS_WIDTH + 1);
				if(h > hM) hM = h;
				else hM -= 3;

				x--,pa--,pc--,pf--;
				}
			else {
				type = (*pf & TERRAIN_MASK) >> TERRAIN_OFFSET;
				lxVal = *(pa - 1);
				rxVal = *(pa0 + XCYCL(x + 1));
				h = *pa;

				grid += 2;
				hdC -= 2;
#ifndef TERRAIN16
				level = type ? h : FloodLEVEL;
				if(grid[level + 32]) *pf |= OBJSHADOW;
				else *pf &= ~OBJSHADOW;
#else
				level = h;
#endif
				if(grid[level]){
					if(isColor) *pc = palCLR[type][256 + (lightCLR[type][255 - (lxVal - rxVal)] - (255 - h >> H_CORRECTION) >> 1)];
					*pf |= SHADOW_MASK;
					*pf &= ~3;
//					if(grid[level] == 2) *pf |= 1;
					}
				else {
					if(isColor) *pc = palCLR[type][256 + lightCLR[type][255 - (lxVal - rxVal)] - (255 - h >> H_CORRECTION)]; 
					*pf &= ~SHADOW_MASK;
					*pf &= ~3;
					}
				if(h > hdC){
					if(hdC < 0) hdC = 0;
					memset(grid + hdC,1,h - hdC);
					hdC = h;
					}
				if(h > hM) hM = h;
				else hM -= 2;

				rxVal = h;
				x--,pa--,pc--,pf--;

				type = (*pf & TERRAIN_MASK) >> TERRAIN_OFFSET;
				lxVal = *(pa0 + XCYCL(x - 1));
				h = *pa;

				grid += 1;
				hdC -= 1;
#ifndef TERRAIN16
				level = type ? h : FloodLEVEL;
				if(grid[level + 32]) *pf |= OBJSHADOW;
				else *pf &= ~OBJSHADOW;
#else
				level = h;
#endif
				if(grid[level]){
					if(isColor) *pc = palCLR[type][256 + (lightCLR[type][255 - (lxVal - rxVal)] - (255 - h >> H_CORRECTION) >> 1)];
					*pf |= SHADOW_MASK;
					*pf &= ~3;
//					if(grid[level] == 2) *pf |= 1;
					}
				else {
					if(isColor) *pc = palCLR[type][256 + lightCLR[type][255 - (lxVal - rxVal)] - (255 - h >> H_CORRECTION)]; 
					*pf &= ~SHADOW_MASK;
					*pf &= ~3;
					}
				if(h > hdC){
					if(hdC < 0) hdC = 0;
					memset(grid + hdC,1,h - hdC);
					hdC = h;
					}
				if(h > hM) hM = h;
				else hM -= 1;
				}
			}

		pa = pa0 + H_SIZE - 1;
		pf0 = pa0 + H_SIZE;
		pf = pa + H_SIZE;
		pc = pc0 + H_SIZE - 1;
		for(x = map_size_x - 1;;x--,pa--,pc--,pf--){
			if(*pf & DOUBLE_LEVEL){
				type = (*(pf - dnD) & TERRAIN_MASK) >> TERRAIN_OFFSET;
				lxVal = *(pa - dnD);
				rxVal = *(pa0 + XCYCL(x + 2 - dnD));
				h = *(pa - dnD);
				hd = *(pa - 1);

				grid += 3;
				hM -= 3;
				if(h > hM) break;
#ifndef TERRAIN16
				if(grid[h + 32]){
					*pf |= OBJSHADOW;
					*(pf - 1) |= OBJSHADOW;
					}
#endif
				if(grid[h] && !(*pf & SHADOW_MASK)){
					if(isColor) *(pc - 1) = *pc = palCLR[type][256 + (lightCLR[type][255 - (lxVal - rxVal)] - (255 - h >> H_CORRECTION) >> 1)];
					*pf |= SHADOW_MASK;
					*(pf - 1) |= SHADOW_MASK;
					}

				x--;pa--;pc--;pf--;
				}
			else {
				type = (*pf & TERRAIN_MASK) >> TERRAIN_OFFSET;
				lxVal = *(pa - 1);
				rxVal = *(pa0 + XCYCL(x + 1));
				h = *pa;

				grid += 2;
				hM -= 2;
				if(h > hM) break;
#ifndef TERRAIN16
				level = type ? h : FloodLEVEL;
				if(grid[level + 32]) *pf |= OBJSHADOW;
#else
				level = h;
#endif
				if(grid[level] && !(*pf & SHADOW_MASK)){
					if(isColor) *pc = palCLR[type][256 + (lightCLR[type][255 - (lxVal - rxVal)] - (255 - h >> H_CORRECTION) >> 1)];
					*pf |= SHADOW_MASK;
//					if(grid[level] == 2) *pf |= 1;
					*pf &= ~3;
					}

				rxVal = h;
				x--;pa--;pc--;pf--;

				type = (*pf & TERRAIN_MASK) >> TERRAIN_OFFSET;
				lxVal = *(pa0 + XCYCL(x - 1));
				h = *pa;

				grid += 1;
				hM -= 1;
				if(h > hM) break;
#ifndef TERRAIN16
				level = type ? h : FloodLEVEL;
				if(grid[level + 32]) *pf |= OBJSHADOW;
#else
				level = h;
#endif
				if(grid[level] && !(*pf & SHADOW_MASK)){
					if(isColor) *pc = palCLR[type][256 + (lightCLR[type][255 - (lxVal - rxVal)] - (255 - h >> H_CORRECTION) >> 1)];
					*pf |= SHADOW_MASK;
//					if(grid[level] == 2) *pf |= 1;
					*pf &= ~3;
					}
				}
			}
/*
		int inshadow = 1;
		int counter = 100;
		pa = pa0 + H_SIZE - 1;
		pf = pa + H_SIZE;
		for(x = map_size_x - 1;x >= 0;x--,pa--,pf--){
			if(!(*pf & DOUBLE_LEVEL))
				if(*pf & SHADOW_MASK){
					if(inshadow) counter++;
					else{
						counter = 0;
						inshadow = 1;
						}
					if(*pf & 1){
						*pf &= ~3;
						if((*pf & TERRAIN_MASK) && counter < 6)
							*pf += 4 - (counter/2 + 1);
						}
					}
				else 
					inshadow = 0;
			}
		pa = pa0;
		pf = pa + H_SIZE;
		inshadow = 1;
		counter = 100;
		for(x = 0;x < map_size_x;x++,pa++,pf++){
			if(!(*pf & DOUBLE_LEVEL))
				if(*pf & SHADOW_MASK){
					if(inshadow) counter++;
					else{
						counter = 0;
						inshadow = 1;
						}
					if((*pf & TERRAIN_MASK) && !(*pf & 3) && counter < 6)
						*pf += 4 - (counter/2 + 1);
					}
				else 
					inshadow = 0;
			}
*/
		}
}

void worldPrepare(void)
{
	if(r_net_map) return;

	XCon < vMap -> wTable[vMap -> cWorld].name < " -> ";
	XCon < "X-Power: " <= MAP_POWER_X < " Y-Power: " <= MAP_POWER_Y < "\n";
	int c = CLOCK(); while(CLOCK() - c < 24);

	r_net_map = new uchar[net_size];
	m_net_map = new uchar[net_size];
	proto_m_net_map = new uchar[net_size];
	new_m_net_map = new uchar[net_size];
}

void worldFree(void)
{
	delete[] r_net_map;
	delete[] m_net_map;
	delete[] proto_m_net_map;
	delete[] new_m_net_map;
	r_net_map = NULL;
}

void worldClean(void)
{
	if(Verbose) XCon < "\nMemory cleaning...";
	memset(alt_map,0,part_map_size + (QUANT + 1)*map_size_x);
	memset(color_map,0,part_map_size + (QUANT + 1)*map_size_x);
	memset(temp_map,0,part_map_size + (QUANT + 1)*map_size_x);
	memset(surf_map,0,part_map_size + map_size_x);
	memset(proto_map,0,part_map_size);
	memset(current_map,0,part_map_size);

	memset(lastlineA,0,map_size_x);
	memset(lastlineC,0,map_size_x);
	memset(lastlineF,DEFAULT_TERRAIN,map_size_x);
}

void worldInit(void)
{
	if(Verbose) XCon < "\nMemory allocation...";

	alt_map = new uchar[part_map_size + (QUANT + 1)*map_size_x];
	color_map = new uchar[part_map_size + (QUANT + 1)*map_size_x];
	temp_map = new uchar[part_map_size + (QUANT + 1)*map_size_x];
	surf_map = new uchar[part_map_size + map_size_x];
	proto_map = new uchar[part_map_size];
	current_map = new uchar[part_map_size];

	worldClean();
}

void worldRelease(void)
{
	delete alt_map;
	delete color_map;
	delete temp_map;
	delete surf_map;
	delete proto_map;
	delete current_map;
	alt_map = color_map = temp_map = NULL;
}
/*
int GetMOBILITY(uchar* p0,uchar* p1,uchar* p2,int x)
{
	int d = 0,dd;
	int h = *(p1 + x);

	int xl = (x - 1) & clip_mask_x;
	int xr = (x + 1) & clip_mask_x;

	dd = (int)(*(p0 + xl)) - h; d += dd*dd;
	dd = (int)(*(p0 + x)) - h; d += dd*dd;
	dd = (int)(*(p0 + xr)) - h; d += dd*dd;
	dd = (int)(*(p1 + xl)) - h; d += dd*dd;
	dd = (int)(*(p1 + xr)) - h; d += dd*dd;
	dd = (int)(*(p2 + xl)) - h; d += dd*dd;
	dd = (int)(*(p2 + x)) - h; d += dd*dd;
	dd = (int)(*(p2 + xr)) - h; d += dd*dd;
	d >>= 3;
	d = (int)sqrt((double)d);
	if(d > 7) d = 7;
	return d;
}
*/
void WORLD_mobility(void)
{
/*
	if(Verbose) XCon < "\nMobilityCalc...";

	int x,y;
	uchar* pa,*pf,*pp;

	pa = (uchar*)alt_map;
	pf = (uchar*)surf_map;
	pp = pa + map_size_x;

	uchar* pl = lastlineA;
	for(x = 0;x < map_size_x;x++,pf++)
		SET_MOBILITY(*pf,GetMOBILITY(pa,pl,pp,x));
	pa += map_size_x;

	for(y = 1;y < part_map_size_y;y++,pa += map_size_x)
		for(x = 0;x < map_size_x;x++,pf++)
			SET_MOBILITY(*pf,GetMOBILITY(pa,pa - map_size_x,pa + map_size_x,x));
*/
}

void WORLD_shot(void)
{
	uchar* buf = new uchar[map_size_x];
	int max = YCYCL(ShotPrm + 1),i,y;
	XStream ff("HARMONY.BMP",XS_OUT);
	XStream ffh("HIGHTS.BMP",XS_OUT);
	XStream ffv("HARMONY.VMP",XS_OUT);
	if(ShotPrm){
		i = 0; y = StartshotPrm;
		do { y = YCYCL(y + 1); i++; } while(y != max);
//		ff < (short)map_size_x < (short)i;
//		ffh < (short)map_size_x < (short)i;
		ff < (short)640 < (short)i;
		ffh < (short)640 < (short)i;
		}
	else {
		ff < (short)map_size_x < (short)map_size_y;
		ffh < (short)map_size_x < (short)map_size_y;
		}
	y = StartshotPrm;

	if(vMap -> isCompressed){
		vMap -> fmap.open(vMap -> fname,XS_IN);
		memset(vMap -> lineT = new uchar*[V_SIZE],0,V_SIZE*sizeof(uchar*));
		memset(vMap -> lineTcolor = new uchar*[V_SIZE],0,V_SIZE*sizeof(uchar*));
		vMap -> st_table = new int[V_SIZE];
		vMap -> sz_table = new short[V_SIZE];
		for(i = 0;i < V_SIZE;i++){
			vMap -> fmap > vMap -> st_table[i];
			vMap -> fmap > vMap -> sz_table[i];
			}
		InitSplay(vMap -> fmap);
		uchar* p0 = new uchar[2*map_size_x];
		uchar* p1 = new uchar[2*map_size_x];
		do {
			vMap -> fmap.seek(vMap -> st_table[y],XS_BEG);
			vMap -> fmap.read(p0,vMap -> sz_table[y]);
			ExpandBuffer(p0,p1);
			vMap -> lineT[y] = p1;
			vMap -> lineTcolor[y] = buf;
			LINE_render(y);
			ff.write(buf,map_size_x);
			ffh.write(p1,map_size_x);
			ffv.write(p1,2*map_size_x);
			y = YCYCL(y + 1);
			} while(y != max);
		delete[] p0;
		delete[] p1;
		}
	else
		do {
			vMap -> lineTcolor[y] = buf;
			LINE_render(y);
//			ff.write(buf,map_size_x);
//			ffh.write(vMap -> lineT[y],map_size_x);
			ff.write(buf + 1000,640);
			ffh.write(vMap -> lineT[y] + 1000,640);
			y = YCYCL(y + 1);
			} while(y != max);

	ff.close();
	ffh.close();
	ffv.close();
	delete[] buf;

	ffh.open("HIGHTS.PAL",XS_OUT);
	for(i = 0;i < 256;i++){
		ffh < (uchar)(i*64/256);
		ffh < (uchar)(i*64/256);
		ffh < (uchar)(i*64/256);
		}
	ffh.close();
}

void WORLD_handle(void)
{
	if(Verbose) XCon < "\n\n";
	
	uint x,y;
	uchar** lt = vMap -> lineT;
	uchar* pa0,*pa,*pf;
	int hd,hu,h;
	int jC = 0;
	for(y = 0;y < V_SIZE;y++){
		if(Verbose) XCon < "\rWorldHandling: " <= y;
		pa = pa0 = lt[y];
		pf = pa + H_SIZE;
		for(x = 0;x < H_SIZE;x += 2,pa += 2,pf += 2){
			if(IS_DOUBLE(*pf)){
				hd = *pa;
				hu = *(pa + 1);
				if((h = hu - hd) < 32){
					*pa = hu;
					SET_TERRAIN(*pf,GET_TERRAIN_TYPE(*(pf + 1)));
					*pf &= ~DOUBLE_LEVEL;
					*(pf + 1) &= ~DOUBLE_LEVEL;
					SET_DELTA(*pf,0);
					SET_DELTA(*(pf + 1),0);
					jC++;
					}
				else {
					h -= 16;
					if(h > MAX_RDELTA) h = MAX_RDELTA;
					h = (h >> DELTA_SHIFT) - 1;
					SET_DELTA(*pf,(h & 12) >> 2);
					SET_DELTA(*(pf + 1),(h & 3));
					}
				}
			else {
				SET_DELTA(*pf,0);
				SET_DELTA(*(pf + 1),0);
				}
			}
		if(Verbose) XCon < ", j: " <= jC < "           ";
		}	
}

void WORLD_glass(void)
{
/*
	if(Verbose) XCon < "\nGlassHandling...";

	int x,y;
	uchar* pa,*pf;

	pa = (uchar*)alt_map;
	pf = (uchar*)surf_map;
	for(y = 0;y < (int)part_map_size_y;y++)
		for(x = 0;x < map_size_x;x += 2,pf += 2,pa += 2)
			if(GET_TERRAIN_TYPE(*(pf + 1)) == GLASS_TERRAIN){
				SET_TERRAIN(*(pf + 1),GET_TERRAIN_TYPE(*pf));
				*(pa + 1) = *pa;
				*pf &= ~DOUBLE_LEVEL;
				*(pf + 1) &= ~DOUBLE_LEVEL;
				SET_DELTA(*pf,0);
				SET_DELTA(*(pf + 1),0);
				}
	if(GlassPrm) return;

	int state,cnt,v,h,dl1,dl2,offset,i;
	for(y = 0;y < (int)part_map_size_y;y++){
		state = cnt = 0;		
		for(x = 0,i = 0;i < map_size_x || state == 2;x = XCYCL(x + 2),pa += 2,pf += 2,i += 2){
			if(!x){
				pa = (uchar*)alt_map + y*map_size_x;
				pf = (uchar*)surf_map + y*map_size_x;
				}
			if(state == 1 && !(*pf & DOUBLE_LEVEL)){
				state = 2;
				cnt = 0;
				}
			if(!state && (*pf & DOUBLE_LEVEL)){
				state = 1;
				cnt = 0;
				}
			if(state == 2 && !(*pf & DOUBLE_LEVEL))
				cnt++;
			if(state == 2 && (*pf & DOUBLE_LEVEL)){
				state = 1;
				if(cnt <= CLUSTER_SIZE){
					v = (int)*(pa + 1) - (h = (int)*((uchar*)alt_map + (offset = y*map_size_x + XCYCL(x - 2*cnt - 1))));
					if(abs(v) > CLUSTER_MAXDELTA) continue;
					x = XCYCL(x - 2*cnt);
					pa = (uchar*)alt_map + y*map_size_x + x;
					pf = (uchar*)surf_map + y*map_size_x + x;
					h += v/2;
					h -= CLUSTER_HOLE;
					dl1 = GET_DELTA(*((uchar*)surf_map + offset - 1));
					dl2 = GET_DELTA(*((uchar*)surf_map + offset));
					while(cnt){
						SET_TERRAIN(*(pf + 1),GLASS_TERRAIN);
						if(h < 0) h = 0; else if(h > 255) h = 255;
						*(pa + 1) = h;
						*pf |= DOUBLE_LEVEL;
						*(pf + 1) |= DOUBLE_LEVEL;
						SET_DELTA(*pf,dl1);
						SET_DELTA(*(pf + 1),dl2);
						
						cnt--; x += 2; pa += 2; pf += 2;
						}
					}
				}
			}
		}

	static uchar* stateP = new uchar[map_size_x/2];
	static ushort* cntP = new ushort[map_size_x/2];

	memset(stateP,0,map_size_x/2);
	memset(cntP,0,map_size_x*sizeof(ushort)/2);
	pa = (uchar*)alt_map;
	pf = (uchar*)surf_map;
	for(y = 0;y < (int)part_map_size_y;y++)
		for(x = 0,i = 0;x < map_size_x;x += 2,pa += 2,pf += 2,i++){
			if(stateP[i] == 1 && !(*pf & DOUBLE_LEVEL)){
				stateP[i] = 2;
				cntP[i] = 0;
				}
			if(!stateP[i] && (*pf & DOUBLE_LEVEL)){
				stateP[i] = 1;
				cntP[i] = 0;
				}
			if(stateP[i] == 2 && !(*pf & DOUBLE_LEVEL))
				cntP[i]++;
			if(stateP[i] == 2 && (*pf & DOUBLE_LEVEL)){
				stateP[i] = 1;
				if(cntP[i] <= CLUSTER_SIZE){
					v = (int)*(pa + 1) - (h = (int)*((uchar*)alt_map + (offset = (y - cntP[i] - 1)*map_size_x + XCYCL(x + 1))));
					if(abs(v) > CLUSTER_MAXDELTA) continue;
					y -= cntP[i];
					pa = (uchar*)alt_map + y*map_size_x + x;
					pf = (uchar*)surf_map + y*map_size_x + x;
					h += v/2;
					h -= CLUSTER_HOLE;
					dl1 = GET_DELTA(*((uchar*)surf_map + offset - 1));
					dl2 = GET_DELTA(*((uchar*)surf_map + offset));
					while(cntP[i]){
						SET_TERRAIN(*(pf + 1),GLASS_TERRAIN);
						if(h < 0) h = 0; else if(h > 255) h = 255;
						*(pa + 1) = h;
						*pf |= DOUBLE_LEVEL;
						*(pf + 1) |= DOUBLE_LEVEL;
						SET_DELTA(*pf,dl1);
						SET_DELTA(*(pf + 1),dl2);
						
						cntP[i]--; y++; pa += map_size_x; pf += map_size_x;
						}
					}
				}
			}
*/
}

void generate_noise(void)
{
	uchar* pa = alt_map;
	register int i,j;
	for(i = 0;i < (int)part_map_size_y;i++)
		for(j = 0;j < map_size_x;j++,pa++)
			*pa += realRND(NOISE_AMPL);
}

void partWrite(XStream& ff,int mode)
{
	if(ShotLog) return;
	
	register int i;
	uchar* pa = alt_map;
	uchar* pf = surf_map;
	if(!mode)
		for(i = 0;i < (int)part_map_size_y;i++,pa += map_size_x){
			ff.write(pa,map_size_x);
			ff.write(lastlineF,map_size_x);
			}
	else
		for(i = 0;i < (int)part_map_size_y;i++,pa += map_size_x,pf += map_size_x){
			if(GlassLog)
				ff.write(pa,map_size_x);
			else
				ff.seek(map_size_x,XS_CUR);
			ff.write(pf,map_size_x);
			}
}

void LoadPP(void)
{
	static const char* errMsg = "Wrong VSC data";

	for(uint i = 0;i < PART_MAX;i++){
		PartPrm[i].minAlt = MIN_ALT;
		PartPrm[i].maxAlt = MAX_ALT;
		PartPrm[i].noise = NOISELEVEL;
		PartPrm[i].minsquare = MINSQUARE;
		}

	char* s = strdup(GetTargetName(vMap -> scenarioName));
	XStream ff(0);
	if(ff.open(s,XS_IN)){
		uint a = 0,b,c,d,e;
		while(!ff.eof()){
			ff >= b >= c >= d >= e;
			if(b == 0 && c == 0 && d == 0) break;
			if(b < 0 || b > 255 || c < 0 || c > 255 || d < 0 || d > 255) ErrH.Abort(errMsg);
			if(a < PART_MAX){
				PartPrm[a].minAlt = b;
				PartPrm[a].maxAlt = c;
				PartPrm[a].noise = d;
//				  PartPrm[a].minsquare = e;
				}
			else break;
			a++;
			}
		ff.close();
		}
	free(s);
}

void SetPP(int n)
{
	min_alt = PartPrm[n].minAlt;
	max_alt = PartPrm[n].maxAlt;
	noiselevel = PartPrm[n].noise;
	minsquare = PartPrm[n].minsquare;
}

#ifdef _SURMAP_
//@caiiiycuk: not sure that this is right fix
extern unsigned realRNDVAL;
void LoadVPR(int ind)
{
	XStream ff(vMap -> pname[ind],XS_IN);

	if(ff.size() != 2*4 + (1 + 4 + 4)*4 + 2*(int)net_size + 2*POWER*4 + 2*(int)PART_MAX*POWER*4 + (int)PART_MAX*4) ErrH.Abort("Incorrect VPR size");

	ff > RNDVAL;
	ff > realRNDVAL;

	ff > MESH;
	ff.seek(2*4*4,XS_CUR);

	ff.read(r_net_map,net_size);
	ff.read(m_net_map,net_size);
	ff.read(r_preRNDVAL,POWER*sizeof(int));
	ff.read(m_preRNDVAL,POWER*sizeof(int));
	ff.read(r_cycleRNDVAL,PART_MAX*POWER*sizeof(int));
	ff.read(m_cycleRNDVAL,PART_MAX*POWER*sizeof(int));
	if(WaterPrm == -1)
		ff.read(FloodLvl,PART_MAX*sizeof(int));
	else {
		for(uint i = 0;i < PART_MAX;i++) FloodLvl[i] = WaterPrm;
		FloodLevel = WaterPrm;
		}

	ff.close();
}
#endif

void SaveVPR(int ind)
{
	if(!ROLog){
		XStream ff(vMap -> pname[ind],XS_OUT);

		ff < RNDVAL;
		ff < realRNDVAL;

		ff < MESH;
		
		for(int i = 0;i < 2*4;i++) ff < (int)0;

		ff.write(&r_net_map[0],net_size);
		ff.write(&m_net_map[0],net_size);
		ff.write(r_preRNDVAL,POWER*sizeof(int));
		ff.write(m_preRNDVAL,POWER*sizeof(int));
		ff.write(r_cycleRNDVAL,PART_MAX*POWER*sizeof(int));
		ff.write(m_cycleRNDVAL,PART_MAX*POWER*sizeof(int));
		ff.write(FloodLvl,PART_MAX*sizeof(unsigned));

		ff.close();
		}
}

void buildWorld(void)
{
	uint first = 0,i;

	if(RestoreLog || CGenLog || MobilityLog || ShotLog || GlassLog || WHLog){
		XStream ff(vMap -> fname,XS_IN);
		LoadVPR();
		ff.close();
		}

	if(!alt_map){
	       worldInit();
		first = 1;
		}

	storeRNDVAL = RNDVAL;
	storerealRNDVAL = realRNDVAL;

	XStream ff;
	if(InitLog || (!CGenLog && !MobilityLog && !ShotLog && !GlassLog && !WHLog)){
		for(i = 0;i < PART_MAX;i++) FloodLvl[i] = WaterPrm == -1 ? FLOOD_INIT : WaterPrm;
		r_net_init();
		m_net_init();

		ff.open(vMap -> fname,XS_OUT);
		SaveVPR();
		int off = ff.tell();
		for(Stage = 0;Stage < PART_MAX;Stage++){
			if(Verbose) XCon < "\n\n------------- Part: " <= Stage < " -------------\n";
			SetPP(Stage);
			generate_alt_map();
			generate_noise();
			partWrite(ff,0);
			}
		ff.close();

		XCon < "\n\n\n\nRecoloring in progress...";
		CGenLog = 1;
		InitLog = 0;
		buildWorld();
		return;
		}
	else {
		SaveVPR();
		RenderPrepare();
		if(!vMap -> isCompressed) vMap -> openMirror();
		if(ShotLog)
			WORLD_shot();
		else
			if(WHLog)
				WORLD_handle();
			else {
				for(Stage = 0;Stage < PART_MAX;Stage++){
					if(Verbose) XCon < "\n\n------------- Part: " <= Stage < " -------------\n";
					if(CGenLog) calc_color_map();
					if(MobilityLog) WORLD_mobility();
					if(GlassLog) WORLD_glass();
					}
				}
		vMap -> closeMirror();
		}
	worldRelease();
}

void GeoRecalc(int n)
{
	int y0 = n << WPART_POWER;
	int y1 = (((n + 1) << WPART_POWER) - 1) & clip_mask_y;

	Stage = n;
	SetPP(n);

	vMap -> request(y0,y1,0,0);

	register int i,j;
	uchar* pc = current_map;
	uchar** lt = vMap -> lineT;
	int y1m = (y1 + 1) & clip_mask_y;
	for(i = y0;i != y1m;i = (i + 1) & clip_mask_y,pc += map_size_x)
		memcpy(pc,lt[i],map_size_x);

	memcpy(m_net_map,proto_m_net_map,net_size);
	generate_alt_map();
	memcpy(proto_map,alt_map,part_map_size);

	memcpy(m_net_map,new_m_net_map,net_size);
	generate_alt_map();

	uchar* ch = vMap -> changedT;
	uchar* pa = alt_map;
	uchar* pp = proto_map;
	uchar* p;
	pc = current_map;
	for(i = y0;i != y1m;i = (i + 1) & clip_mask_y){
		for(p = lt[i],j = 0;j < map_size_x;j++,pa++,pc++,pp++,p++){
/*
			v = (int)*pc + ((int)*pa - (int)*pp);
			if(v < 0) v = 0;
			else if(v > 255) v = 255;
			*p = (uchar)v;
*/
			pixSet(j,i,(int)*pa - (int)*pp);
			}
		ch[i] = 1;
		}
}

void GeoPoint(int x,int y,int level,int delta,int mode)
{
	int v,vm;
	switch(mode){
		case 0:
			if(!(x%QUANT) && !(y%QUANT)){
				v = ((y - 3*QUANT) & clip_mask_y) >> WPART_POWER;
				vm = (1 + (((y + 3*QUANT) & clip_mask_y) >> WPART_POWER)) & (PART_MAX - 1);
				for(;v != vm;v = (v + 1) & (PART_MAX - 1)) geoUsed[v] = 1;
				x >>= POWER;
				y >>= POWER;
				if(!delta)
					v = level;
				else {
					v = M_NET_MAP(y,x);
					v += delta;
					if(delta > 0){ if(v > 255) v = 255; }
					else if(v < 0) v = 0;
					}
				M_NET_MAP(y,x) = (uchar)v;
				}
			break;
		case 1:
			worldInit();
			for(v = 0;v < (int)PART_MAX;v++) geoUsed[v] = 0;
			geoFirst = 0;
			memcpy(proto_m_net_map,m_net_map,net_size);
			break;
		case 2:
			memcpy(new_m_net_map,m_net_map,net_size);
			v = geoFirst;
			do {
				if(geoUsed[v]) GeoRecalc(v);
				v = (v + 1) & (PART_MAX - 1);
				} while(v != geoFirst);
			worldRelease();
			break;
		}
}

void CUTVMP(void)
{
	const int LEN = 13;
	const int X0 = 1600;
	const int Y0 = 700;

	XCon < "\nCUTVMP running...";

	char* name = strdup(vMap -> fname);
	memcpy(name + strlen(name) - 10,"cutvmp",6);
	XStream ff(name,XS_IN | XS_OUT);
	uchar* buf = new uchar[512];
	ff.read(buf,512);
	
	int h_size = 1 << *(buf + LEN);
	int v_size = 1 << *(buf + LEN + 1);

	delete[] buf;
	buf = new uchar[2*h_size];

	vMap -> openMirror();
	int y = YCYCL(Y0 - v_size/2);

	register int i,j,x;
	uchar* pa,*pf,*p;
	uchar** lt = vMap -> lineT;
	for(j = 0;j < v_size;j++,y = YCYCL(y + 1)){
		x = XCYCL(X0 - h_size/2);
		pa = lt[y] + x; pf = pa + H_SIZE;
		p = buf;
		for(i = 0;i < h_size;i++,x = XCYCL(x + 1),p++,pa++,pf++){
			if(!x){ pa = lt[y]; pf = pa + H_SIZE; }
			*p = *pa; *(p + h_size) = *pf;
			}
		ff.write(buf,2*h_size);
		}

	vMap -> closeMirror();
}
