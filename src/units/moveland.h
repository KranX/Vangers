#ifndef __UNITS__MOVELAND_H
#define __UNITS__MOVELAND_H

#ifdef _ROAD_

const int MAP_POINT_NONE = -1;

const int MAP_POINT_CRATER01 = 0;
const int MAP_POINT_CRATER02 = 1;
const int MAP_POINT_CRATER03 = 2;
const int MAP_POINT_CRATER04 = 3;
const int MAP_POINT_CRATER05 = 4;
const int MAP_POINT_CRATER06 = 5;
const int MAP_POINT_CRATER07 = 6;
const int MAP_POINT_CRATER08 = 7;
const int MAP_POINT_CRATER09 = 8;
const int MAP_POINT_CRATER10 = 9;
const int MAP_POINT_CRATER11 = 10;
const int MAP_POINT_CRATER12 = 11;
const int MAP_POINT_SHOW_HEAD = 12;
const int MAP_POINT_HIDE_HEAD = 13;

/*const int MAP_POINT_DUST01 = 0;
const int MAP_POINT_DUST02 = 1;
const int MAP_POINT_DUST03 = 2;
const int MAP_POINT_DUST04 = 3;
const int MAP_POINT_DUST05 = 4;
const int MAP_POINT_DUST06 = 5;
const int MAP_POINT_DUST07 = 6;
const int MAP_POINT_DUST08 = 7;
const int MAP_POINT_DUST09 = 8;*/

const int MAP_DUST_PROCESS = 0;
const int MAP_SMOKE_PROCESS = 1;

const int MAX_DESTROY_RADIUS = 64;

const int MAP_LAVA_SPOT = 0;
const int MAP_LIGHT_POINT = 1;
const int MAP_LANDSLIDE_TYPE = 2;
const int MAP_LANDHOLE_TYPE = 3;
const int MAP_ACID_TYPE = 4;

struct MapLandHole : GeneralObject
{
	int maxRadius,cRadius;
	int Time,Mode;
	int LifeTime;
	
	void CreateLandHole(Vector v,int rMax,int l1,int l2,int l3);
	void Quant(void);
};

struct MapAcidSpot : GeneralObject
{
	int Radius;
	int dRadius;
	int Delta;
	int dDelta;
	int Time;

	void CreateAcid(Vector v,int fRad,int lRad,int fDelta,int lDelta,int lTime);
	void Quant(void);
};

struct MapLavaSpot : GeneralObject
{
	uchar Terrain;
	int Radius,Delta;
	int LastRadius,LastDelta;
	int dDelta1,dRadius1;
	int dDelta2,dRadius2;
	int Phase,MaxPhase1,MaxPhase2;
	uchar Delay,mDelay;
	uchar rFactor;
	uchar RenderFlag;
	uchar ClipTerrain;

	void CreateSpot(Vector& v,int fRadius,int fDelta,int radius1,int delta1,int radius2,int delta2,int phase1,int phase2,uchar terrain,uchar md,uchar rf,uchar render,uchar clip_t);
	void Quant(void);
};

struct LandSlideType : GeneralObject
{
	int x,y,z;
	int Radius,Time;
	int sZ,dZ;
	int cX[4],cY[4];

	void makeLittelNoise(void);
	void makeFastlNoise(void);
	void CreateLandSlide(int* _xx,int* _yy,int _Time);
	void Quant(void);
};

struct MapPointType : GeneralObject
{
	int Angle;
	int Type;
	void Create(const Vector& v,int type = 0,int angle = 0){ R_curr = v;Type = type;Angle = angle; };
};

struct TrackUnit;

struct MapTraceType : GeneralObject
{
	char UpFlag;
	int x0,y0,x1,y1;
	int track_nx,track_ny;

	void Create(int _x0,int _y0,int _x1,int _y1,int nx,int ny,char f){ x0 = _x0;y0 = _y0;x1 = _x1;y1 = _y1; track_nx = nx; track_ny = ny; UpFlag = f;};
};

struct LocalMapProcess : UnitList
{
	int NumMapPoint;
	StorageType MapPointStorage;
	MapPointType* MapPointData;

	int NumTrace;
	StorageType TraceStorage;
	MapTraceType* TraceData;

	int NumDustType;
	StorageType* DustStorage;
	MapPointType** DustData;
	ParticleProcess* Dust;
//	int DustID[TERRAIN_MAX];

	void Init(Parser& in);
	void Open(Parser& in);
	void Close(void);
	void Free(void);
	void Quant(void);

	void CreateDust(const Vector& v,int type);
//	void CreateTerrainDust(Vector& v,uchar terrain);
	void CreateCrater(Vector& v,int type = 0,int angle = 0);

	void CreateTrace(int _x0,int _y0,int _x1,int _y1,int track_nx,int track_ny,char f);

	void CreateLavaSpot(Vector& v,int fRadius,int fDelta,int radius1,int delta1,int radius2,int delta2,int phase1,int phase2,uchar terrain,uchar md,uchar rf,uchar render,uchar clip_t);
	LightPoint* CreateLight(int x,int y,int z,int r,int e,int mode);
	void CreateLandSlide(int* x,int* y,int time);
	void CreateMapHole(Vector v,int rMax,int l1,int l2,int l3);
	void CreateAcidSpot(Vector v,int fR,int lR,int fD,int lD,int lT);
	void TestDust(void);
};

extern LocalMapProcess MapD;
#endif

const char MOBILE_LOCATION_ACTIVE = 1;
typedef int MLAtype;
const int MLPREC = 16;
//typedef short MLAtype;
//const int MLPREC = 7;

struct MLFrame {
	int x0,y0;
	int sx,sy,sz,ss;
	int csd, cst;
	uchar* delta;
	uchar* terrain;
	uchar* c_delta;
	uchar* c_terrain;

	unsigned* signBits;
	int period;
	int surfType;

		MLFrame(void){ clear(); }

	void clear(void){ period = 1; delta = terrain = c_delta = c_terrain = NULL; signBits = NULL; surfType = 99; 
					csd = cst = 0; sz = ss = sx = sy = 0;
					}
	void load(XStream& ff,int mode);
	void save(XStream& ff);
	int quant(int& step,int dx,int dy,MLAtype* alt,int dry,int render = 1,int fastMode = 0);
	int quantAbs(int& step,int dx,int dy,MLAtype* alt,int dry,int render = 1,int fastMode = 0);
	void start(int dx,int dy,MLAtype* alt);
	int check(int dy);
	void setPeriod(int _period){ if(_period < 1) period = 1; else period = _period; }
	void setSurftype(int _surf){ if(_surf < -1) _surf = -1; surfType = _surf; }
	void put(int x,int y,MLAtype* alt);
	void Render(int dx,int dy){ regRender(x0 + dx,y0 + dy,x0 + dx + sx,y0 + dy + sy,0); }
#ifdef _SURMAP_
	void accept(uchar* _delta,uchar* _terrain,unsigned* _signBits,int _sx,int _sy,int _x0,int _y0,int reallynew = 1,int nonull = 0);
	void cut(int cx0,int cy0,int csx,int csy);
	void release(void);
#else
	int DestroyQuant(int dx,int dy,MLAtype* alt,int dry);
#endif
	};

const int MAX_KEYPHASE = 4;

#define MLM_RELATIVE	0
#define MLM_ABSOLUTE	1
#define MLM_REL2ABS	2

struct MobileLocation {
	int maxFrame,cFrame;
	MLFrame* table;
	MLAtype* alt;
	int* steps;
	int altSx,altSy;
	int x0,y0,x1,y1,dx,dy;
	int goPh;
	int fastMode,isClone,isAlt,frozen;
	int mode;

	int DryTerrain;
	int Impulse;
	int KeyPhase[MAX_KEYPHASE];
	
	int inUse;
	char name[MLNAMELEN + 1];
	int cStage;

#ifdef _ROAD_
	int NetGoPhase,NetSetPhase;
	int maxStage;
	int NetID;
	char GetVisible(void);
#endif
#ifdef _SURMAP_
	char* fileName;
	int* isConverted;
#endif
	
		MobileLocation(void){ clear(); }
		~MobileLocation(void);
	
	void clear(void){
		maxFrame = cFrame = 0;
		table = NULL; alt = NULL; steps = NULL;
		altSx = altSy = 0; dx = dy = 0;
		frozen = 1; fastMode = 0; isClone = 0; isAlt = 0;
		goPh = -1;
		DryTerrain = 1; Impulse = 0;
		KeyPhase[0] = 0; KeyPhase[1] = KeyPhase[2] = KeyPhase[3] = -1;
		memset(name,'\0',MLNAMELEN + 1); 
		mode = MLM_RELATIVE;
#ifdef _SURMAP_
		inUse = 0;
		cStage = 0;
		isAlt = 1;
		fileName = NULL;
#endif
		}
	void load(char* fname,int direct = 0);
	int quant(int render = 1,int skipVZ = 0,int skipCheck = 0);	// skipVZ - пропускать проверку на видимость в Surmap (1 в setPhase)
	MobileLocation* cloning(int _dx,int _dy, const char* _name);

	void calcBounds(void);
	void setDry(int dry){ if(dry < 0) dry = 0; else if(dry >= TERRAIN_MAX) dry = TERRAIN_MAX - 1; DryTerrain = dry; }
	void setKeyPhase(int n,int val){ KeyPhase[n] = val; }	
	void put(int x,int y);

#ifdef _ROAD_
	void reset(void){ frozen = 1; cStage = -1; steps[cFrame] = 0; cFrame = 0; /*goPh = StaticPhase;*/ }
	void checkQuant(void);
//	void setStaticPhase(int ph);
	int QuickCheck(void);
	int NetQuickCheck(int x83,int y83,int r83);
#else
	void reset(void){ setPhase(0); }
	void add(uchar* delta,uchar* terrain,unsigned* signBits,int sx,int sy,int x0,int y0);
	void accept(void);
	void save(int reserve = 0);
	void removeFile(void);
	void erase(void);
	void makeCopy(MobileLocation* p);
#endif

	// сразу устанавливает указанную фазу (прокручивает без рендера нужные периоды и рендерит фазу nPhase)
	// если nPhase не существует ничего не происходит
	void setPhase(int nPhase,int noChange = 0);
	// выставляет конечную фазу, на которой ML останавливается и ждет следующих указаний
	// если nPhase == -1, то ML входит в безостановочный режим, если фазы nPhase не существует (слишком большая), то она становится последней возможной
	void goPhase(int nPhase);

	void setKeyPhase(int n){ if(n < 0) setPhase(-1); else setPhase(KeyPhase[n]); }
	void goKeyPhase(int n){ if(n < 0) goPhase(-1); else goPhase(KeyPhase[n]); }
	int isGoFinish(void){ return getCurPhase() == goPh; };	

#ifdef _ROAD_
	int getCurPhase(void){ if(cStage == maxStage - 1) return 0; else return cStage + 1; }
//	int getCurPhase(void){ return cStage; }
#else
	int getCurPhase(void){ if(cFrame == maxFrame - 1) return 0; else return cFrame + 1; }
	int getStage(void){ return cStage++; }
#endif
	};


#ifdef _SURMAP_

struct BaseValoc {
	int x,y,z;
	BaseValoc* next;
	BaseValoc* prev;

	virtual ~BaseValoc() = default;
	
	void link(void);

	virtual BaseValoc** getTail(void){ return NULL; }
	virtual void load(XStream& ff){}

	virtual void edit(void){}
	virtual void save(XStream& ff){}
	void remove(void);
	void show(void);
	};

struct TntValoc : BaseValoc {
	static BaseValoc* tail;
	int value;
	int radius;
	int type;

	virtual BaseValoc** getTail(void){ return &tail; }
	virtual void load(XStream& ff);
		
		TntValoc(void){ value = 0; radius = 0; type = 0; }

	virtual void edit(void);
	virtual void save(XStream& ff);
	void accept(int _z,int _value,int _radius,int _type = 0);
	};
	
struct MLCloneValoc : BaseValoc {
	static BaseValoc* tail;
	int value;

	virtual BaseValoc** getTail(void){ return &tail; }
	virtual void load(XStream& ff);
		
		MLCloneValoc(void){ value = 0; }

	virtual void edit(void);
	virtual void save(XStream& ff);
	void accept(int _z,int _value);
};

struct SensorValoc : BaseValoc {
	static BaseValoc* tail;
	int id;
	int nameLen;
	char* name;
	int radius;
	int z0,data0,data1,data2,data3,data4,data5,data6;

	virtual BaseValoc** getTail(void){ return &tail; }
	virtual void load(XStream& ff);
		
		SensorValoc(){ id = 0; nameLen = 0; name = strdup(""); radius = 0; }
		~SensorValoc() { free(name); }

	virtual void edit(void);
	virtual void save(XStream& ff);
	void accept(int _z,int _id,int _radius,const char* _name,int _z0,int _data0,int _data1,int _data2,int _data3,int _data4,int _data5,int _data6);
	};

struct DangerValoc : BaseValoc {
	static BaseValoc* tail;
	int type;
	int radius;

	virtual BaseValoc** getTail(void){ return &tail; }
	virtual void load(XStream& ff);
		
		DangerValoc(void){ type = 0; radius = 0; }

	virtual void edit(void);
	virtual void save(XStream& ff);
	void accept(int _z,int _type,int _radius);
	};
	
#endif

extern MobileLocation** MLTable;
extern int MLTableSize;
extern int TntTableSize;
extern int SnsTableSize;
extern int DngTableSize;

void MLload(void);
void MLsave(void);
void MLreload(void);
int MLquant(void);
void MLfree(void);
#ifdef _ROAD_
void MLCheckQuant(void);
#endif

void VLload(void);
void MLfree(void);

#ifndef _SURMAP_


void DestroyBarellSmooth(int x0,int y0,int SmoothRadius,int SpotRadius,int Delta,uchar terrain,int Num,int z0,uchar rfactor);
void DestroySmooth(int x0,int y0,int SmoothRadius,int SpotRadius,int Delta,int cDelta,uchar terrain,int Num,uchar rfactor);
void DestroySpot(int x0,int y0,int Radius,uchar terrain,int delta,uchar rfactor);
void DestroyBarellSpot(int x0,int y0,int Radius,uchar terrain,int delta,uchar rfactor);

int DestroyBarellSmoothWithCheck(int x0,int y0,int SmoothRadius,int SpotRadius,int Delta,uchar terrain,int Num,int z0,uchar rfactor);

int DestroyBarellSmoothWithCheck(int x0,int y0,int SmoothRadius,int SpotRadius,int Delta,uchar terrain,int Num,int z0,uchar rfactor);

void DestroySmoothCheck(int x0,int y0, int Radius);
void DestroySmoothPut(int x0,int y0,int Radius);
void DestroySmoothInit(void);
void DestroySmoothFree(void);

void xDestroySpot(int x0,int y0,int Radius,uchar terrain,int delta,uchar rfactor);
void xRestoreDestroySpot(int x0,int y0,int Radius,uchar terrain,int delta,uchar rfactor);

void RadialRender(int x0,int y0,int radius);

//int LandSlide(int x0,int y0,int Radius,int z0);
//int LandSlideMove(int x0,int y0,int Radius,int z0,int delta);
int GetLandAlt(int x0,int y0,int Radius);

int LandSlideProcess(int* fx,int* fy,int pow,int z0,int delta);
int LandSlideLine(int x0,int y0,int z0,int x1,int delta);

void MakeSecondLevel(int x0,int y0,uchar z0,int Radius);
void MapCircleProcess(int x0,int y0,int z0,int r0,int mode,uchar terrain);

void ClearBarell(int x0,int y0,int radius,uchar terrain,int z0);

inline int GetSmoothUpAlt(uchar* type,uchar* pa0,int x)
{
	if(IS_DOUBLE(*type)){
		if(x & 1) return *(pa0 + x);
		else return *(pa0 + x + 1);
	};
	return *(pa0 + x);
};

void MLReset(void);

extern uchar DestroyMechosTable[TERRAIN_MAX];
#define GET_DESTROY_MECHOS(n) DestroyMechosTable[n]

extern uchar DestroyMoleTable[TERRAIN_MAX];
#define GET_DESTROY_MOLE(n) DestroyMechosMole[n]


#endif

#endif
