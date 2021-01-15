#ifndef __TERRA__WORLD_H__
#define __TERRA__WORLD_H__

#define SIMPLE_SHADOWS
#define COMPLEX_RENDER

const unsigned SS_WIDTH = 16;

const uchar SHADOW_MASK = 1 << 7;
const uchar DELTA_MASK = 1 | (1 << 1);
const uchar DOUBLE_LEVEL = 1 << 6;
const uchar OBJSHADOW = 1 << 2;

#ifdef TERRAIN16
const int TERRAIN_MAX = 16;
const uchar TERRAIN_OFFSET = 2;
const uchar TERRAIN_MASK = (1 << TERRAIN_OFFSET) | (1 << (TERRAIN_OFFSET + 1)) | (1 << (TERRAIN_OFFSET + 2)) | (1 << (TERRAIN_OFFSET + 3));
#else
const int TERRAIN_MAX = 8;
const uchar TERRAIN_OFFSET = 3;
const uchar TERRAIN_MASK = (1 << TERRAIN_OFFSET) | (1 << (TERRAIN_OFFSET + 1)) | (1 << (TERRAIN_OFFSET + 2));
#endif

#define GET_DELTA(a)	((a) & DELTA_MASK)

#define SET_DELTA(a,b)	{ a &= ~DELTA_MASK, a += b; }
#define SET_TERRAIN(a,b)	{ a &= ~TERRAIN_MASK, a += b; }

extern uchar BEGCOLOR[TERRAIN_MAX];
extern uchar ENDCOLOR[TERRAIN_MAX];

const uchar WATER_TERRAIN_INDEX = 0;
const uchar MAIN_TERRAIN_INDEX	= 1;

const uchar WATER_TERRAIN = WATER_TERRAIN_INDEX << TERRAIN_OFFSET;
const uchar MAIN_TERRAIN = MAIN_TERRAIN_INDEX << TERRAIN_OFFSET;

const uchar MIN_ALT = 0;
const uchar MAX_ALT = 255;

extern uchar POWER;
extern uchar QUANT;
extern uint WPART_POWER;
extern uint part_map_size_y;
extern uint part_map_size;
extern uint net_size;
extern uint PART_MAX;
extern int MINSQUARE;

void buildWorld(void);
void WORLD_colcalc(int y0,int y1,int isColor = 1);
void DOWNWORLD_colcalc(int y0,int y1);
void worldPrepare(void);

void LoadVPR(int ind = 0);
void SaveVPR(int ind = 0);
void LoadPP(void);

extern int CurrentTerrain, ActiveTerrain[TERRAIN_MAX];
extern int MiniLevel,MaxiLevel;

#define COLOR_MAP(y,x)	  (*(color_map + ((y) << MAP_POWER_X) + (x)))
#define ALT_MAP(y,x)	  (*(alt_map   + ((y) << MAP_POWER_X) + (x)))
#define PTR_ALT_MAP(y,x)  (alt_map   + ((y) << MAP_POWER_X) + (x))
#define TEMP_MAP(y,x)	  (*(temp_map  + ((y) << MAP_POWER_X) + (x)))
#define R_NET_MAP(y,x)	  r_net_map[((y) << (MAP_POWER_X - POWER)) + (x)]
#define M_NET_MAP(y,x)	  m_net_map[((y) << (MAP_POWER_X - POWER)) + (x)]

// LAND prototypes
void landPrepare(void);
void WORLD_qrender(int x0,int y0,int x1,int y1);
void setSmallCrater(int x,int y,int z,int type);

extern int MESH;

const uchar DELTA_SHIFT = 3;
const uchar MAX_RDELTA = 16 << DELTA_SHIFT;
const uchar MIN_RDELTA = 1 << DELTA_SHIFT;

// | SHADOW | D_LEVEL | TERRAIN | TERRAIN | TERRAIN | OBJSHADOW | DELTA | DELTA | 
//   7        6         5         4         3         2           1       0       

// Direct-changed parameters (CSET system)

const int WORLD_TYPE	       =  3;

// 256 = 1 voxel/step
const int SHADOWDEEP	       =  384;		//362;

const int NOISELEVEL	       = 256;

const int BIZARRE_ROUGHNESS_MAP = 0;
const int BIZARRE_ALT_MAP	= 0;

extern uchar DestroyTerrainTable[TERRAIN_MAX];
#define GET_DESTROY_TERRAIN(n) DestroyTerrainTable[n]

#endif