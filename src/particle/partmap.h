
const int  FIRE_COLOR_BITS = 5;
const int  FIRE_COLOR_FIRST = 192;
const int  PROCESS_TYPE_TOTAL = 5;

const int  FIRE_PROCESS_COLOR_MAX = 1 << FIRE_COLOR_BITS;

#include "deform.h"

const int  WHITE_POINT = 1 << FIRE_COLOR_BITS;
const int  RED_POINT = WHITE_POINT/3;
const int  YELLOW_POINT = RED_POINT*2;
const int  MAX_MASK_TOTAL = 20;

const int  FIRE_CENTER_DEV_AMPL = 10;

//const int  PARTICLE_MAP_GAP = 100;
const int  PARTICLE_MAP_GAP = 2;

struct ParticleMapProcessType{
	char MSSfile[32];
	int ProcedureType;
	int Xmax;
	int Ymax;
	int HotSpotArea;
	int NhotSpots;
	int AttackTime;
	int LifeTime;
	int FadeTime;
	int FadePower;
	int DeviationRadius;
	int DeviationPeriod;
	int HotCentersRadius;
};
const int  GLOBAL_N_HOT_CENTERS = 3;
struct ParticleMapProcess{
	int ProcedureType;
	unsigned char* WorkArea;
	unsigned char* field1;
	unsigned char* field2;
	unsigned char* f1;
	unsigned char* f2;
//	unsigned char* ColorBuf;
	int NhotSpots;
	int CurrNhotSpots;
	int NhotCenters;
	int HotCentersRadius;
	int HotCentersX[GLOBAL_N_HOT_CENTERS];
	int HotCentersY[GLOBAL_N_HOT_CENTERS];
	Mask* Motion;
	int HotSpotArea;
	int AttackTime;
	int LifeTime;
	int FadeTime;
	int FadePower;
	int DeviationRadius;
	int DeviationPeriod;

	int HotSpotRadius;
	int Xmax;
	int Ymax;
	int phase;
	int ProcessTime;
	ParticleMapProcessType* Type;
	int PrevCurrDev;


	void PickUpMask();
	void activate();
	void activate(int type);
	void activate(int n_hot_spots,int hot_spot_area,int attackT,int lifeT,int fadeT,int fadeP,int dR,int dP,int pType,int HCR,int type);
	void init(int type);
	void init(int x_max,int y_max,int type);
	void finit();
	void finitMask();
	void init_flame(unsigned char* f,int fade = 0);
	int process(char* vb,int Xc,int Yc,int XcS,int turn,int noout, int height, int Xreal, int Yreal);
	void express();
};


extern unsigned char *FirePaletteTable;
extern ParticleMapProcessType ParticleMapProcessTypeArray[PROCESS_TYPE_TOTAL];
extern Mask* ParticleMapProcessMaskArray;
extern int ParticleMapProcessMaskTotal;

void FirePaletteInit(unsigned char* palbuf);
void FirePaletteTableInit(unsigned char *pal);

enum {	 EXPLOSION_64x64 = 0,
	EXPLOSION_16x16
	};


