#ifndef __UNITS__EFFECT_H
#define __UNITS__EFFECT_H

const int EXPLOSION_OFFSET = 50;

const char DT_DEFORM01 = 0;
const char DT_DEFORM02 = 1;
const char DT_FIRE_BALL01 = 0;
const char DT_FIRE_BALL02 = 1;
const char DT_FIRE_BALL04 = 3;
const char DT_FIRE_BALL05 = 4;

const char EFF_EXPLOSION01 = 0;
const char EFF_EXPLOSION02 = 1;
const char EFF_EXPLOSION03 = 2;
const char EFF_EXPLOSION04 = 3;
const char EFF_EXPLOSION05 = 4;
const char EFF_DEFORM01 = 5;
const char EFF_PARTICLE01 = 6;
const char EFF_PARTICLE02 = 7;
const char EFF_PARTICLE03 = 8;
const char EFF_FIREBALL = 9;
const char EFF_PARTICLE04 = 10;
const char EFF_PARTICLE05 = 11;
const char EFF_PARTICLE06 = 12;

const char EFF_PARTICLE_GENERATOR = 13;
const char EFF_PARTICLE07 = 14;

const int PARTICLE_SIZE01 = 30;
const int PARTICLE_SIZE02 = 5000;
const int PARTICLE_SIZE03 = 30;
const int PARTICLE_SIZE04 = 300;
const int PARTICLE_SIZE05 = 30;
const int PARTICLE_SIZE06 = 500;
const int PARTICLE_SIZE07 = 8;

const int PARTICLE_DIRECT = 0;
const int PARTICLE_REVERS = 1;

struct ExplosionObject : BaseObject
{
	int Scale,dScale;
	int FirstRadius;
	BaseObject* Owner;
	ParticleMapProcess MainMapProcess;

	void InitProcess(char _type);
	void Init(StorageType* s);
	void Free(void);
	void Quant(void);
	void DrawQuant(void);
	void CreateExplosion(const Vector& v,BaseObject* _owner,int _scale,int _dscale);
};

struct FireBallObject : BaseObject
{
	int Scale,dScale;
	int FirstRadius;
	BaseObject* Owner;
	FireBallProcess* FBP;
	int frame;

	void Init(StorageType* s);
	void Quant(void);
	void DrawQuant(void);
	void CreateFireBall(const Vector& v,BaseObject* _owner,int _scale,int _dscale,char _type);
};

struct DeformObject : BaseObject
{
	char FullFlag;
	int Offset;
	WaveProcess* wProcess;

	void Init(StorageType* s);
	void Quant(void);
	void DrawQuant(void);
	void CreateDeform(const Vector& v,char _fl,WaveProcess* p);
};

struct SimpleParticleType
{
	Vector vR;
	Vector vD;
	int Color,dColor;
	void Quant(void);
	void QuantT(int x,int y,int s);
	void QuantP(Vector _c, Vector _n, int s,int c);
	void QuantRingOfLord(Vector v,int s,int c);
};

struct ParticleInitDataType
{
	int LifeTime;																		   
	int Velocity;
	int FirstRadius,EndRadius;
	int FirstColor,EndColor;
	int FirstAlpha,StepAlpha;
};

struct ParticleObject : BaseObject
{
	int NumParticle;
	int LifeTime;
	int Time;
	int Mode,Phase,dPhase;

	SimpleParticleType* Data;
	
	void InitParicle(int num);
	void Init(StorageType* s);
	void Free(void);
	void Quant(void);
	void DrawQuant(void);
	void CreateParticle(int _LifeTime,int _Velocity,int _FirstRadius,int _EndRadius,int _FirstColor,int _EndColor,const Vector& v1,const Vector& v2);
	void CreateParticle(int _LifeTime,int _Velocity,int _FirstRadius,int _EndRadius,int _FirstColor,int _EndColor,int _StepAlpha,const Vector& v1,const Vector& v2,const DBM& n);
	void CreateParticle(ParticleInitDataType* n,const Vector& v1,const Vector& v2);
	void CreateParticle(ParticleInitDataType* n,const Vector& v1,const Vector& v2,const DBM& m);
	void CreateParticle(ParticleInitDataType* n,const Vector& v);
	void CreateDirectParticle(ParticleInitDataType* n,const Vector& v);
	void CreateRingOfLord(const Vector v1,int rad,int ltime,int fcol,int lcol,int vel);
};

struct WaterParticleObject : BaseObject
{
	int NumParticle;
	int LifeTime,SetLifeTime;
	int DeltaColor;
	int Time;

	int TargetType;
	Vector vCenter;
	int Velocity;

	SimpleParticleType* Data;
	
	void InitParicle(int num);
	void Init(StorageType* s);
	void Free(void);
	void Quant(void);
	void DrawQuant(void);
	void CreateParticle(int _LifeTime,int _SetLifeTime,int _Velocity,int _Radius,int _FirstColor,int _SetColor,int _EndColor,const Vector& v1,int _TargetType = 0);
};

struct TargetParticleType
{
	char type;
	int Color;
	Vector vR,vT,vD;
	int s;
	short pDist, LifeTime;
	void aQuant(void);
	void aQuant2(void);

	void sQuant(void);
	void sQuant2(void);
};

struct TargetParticleObject : BaseObject
{
	char TargetType;
	int NumParticle,CurrParticle;
	int LifeTime,Time;
	TargetParticleType* Data;
	Vector vTarget;
	int FadeTime,FadeNum;

	void InitParicle(int num);
	void Init(StorageType* s);
	void Free(void);
	void Quant(void);
	void DrawQuant(void);
	void AddVertex(const Vector& _vR,int _Color,int _Speed1,int _Speed2);
	void AddVertex2(const Vector& _vR,const Vector& _vT,int _Color, int _type = PARTICLE_DIRECT);
	void CreateParticle(const Vector& _vTarget,int _LifeTime,char type);
};

const int PG_TARGET = 1;
const int PG_TARGET_RANDOM = 2;
const int PG_TARGET_LEFT = 4;
const int PG_TARGET_RIGHT = 8;
const int PG_WALL = 16;
const int PG_RADIUS = 32;

const int PG_MODE_TRUE_MASS = 1;
const int PG_MODE_MOVE_TARGET = 2;

const int PG_STYLE_INCAR = 0;
const int PG_STYLE_ENTER = 1;
const int PG_STYLE_REINCAR = 2;

struct ParticleGenerator : BaseObject
{
	Vector vTarget,vDelta;
	int Speed,Precision;
	int MoveMode,TargetMode;
	int ParticleStorage,ParticleType;
	int Time;	
	int FlyRadius;

	void Init(StorageType* s);
	void Quant(void);
	void DrawQuant(void);
	void CreateGenerator(Vector vC,Vector vT,Vector vD,int mode = PG_STYLE_INCAR);
};

struct EffectDispatcher : UnitList
{
	int NumDeformProcessType;
	WaveProcess* DeformData;

	int NumFireBallProcessType;
	FireBallProcess* FireBallData;

	int NumParticleInitType;
	ParticleInitDataType* ParticleInitData;

	void Init(Parser& in);
	void Free(void);
	void Open(Parser& in);
	void Close(void);
	void Quant(void);

	void CalcWave(void);

	void CreateExplosion(const Vector& v,unsigned char _type,BaseObject* _owner = NULL,int _scale = 1 << 15,int _dscale = 0);
	void CreateFireBall(const Vector& v,unsigned char _type,BaseObject* _owner = NULL,int _scale = 1 << 8,int _dscale = 0);
	void CreateDeform(const Vector& v,char _fl,unsigned char _type);
	void CreateParticle(char _init,const Vector& v1,const Vector& v2,unsigned char _type);
	void CreateParticle(char _init,const Vector& v1,const Vector& v2,const DBM& m,unsigned char Type);
	void CreateParticle(char _init,const Vector& v,unsigned char _type);
	void CreateDirectParticle(char _init,const Vector& v,unsigned char _type);
	void CreateRingOfLord(int type,const Vector v1,int rad,int ltime,int fcol,int lcol,int vel);
	void DeleteEffect(BaseObject* p);
	void CreateParticleTime(unsigned char _init,const Vector& v1,const Vector& v2,unsigned char _type,int LifeTime);
	void CreateParticleGenerator(Vector vC,Vector vT,Vector vD,int mode = PG_STYLE_INCAR);
};

int GetAltLevel(int x,int y,int z);
//char GetAltLevel(int x,int y,int z);
int WaterAltLevel(int x,int y,int z);
//char WaterAltLevel(int x,int y,int z);

extern EffectDispatcher EffD;
int SPGetDistX(int s0,int s1);

void MakeColorTable(int fc,int lc,uchar* d,uchar* pal);

extern int SPViewX,SPViewY,SPScaleMap,SPTorXSize,SPTorYSize;
inline int SPGetDistX(int s0,int s1)
{
	int d0 = s0 - s1;
	int ad0 = abs(d0);
	int d1 = SPTorXSize - ad0;
	if(ad0 < d1) return d0;
	return s0 > s1 ? -(SPTorXSize - s0 + s1) : SPTorXSize - s1 + s0;
};

inline int SPGetDistY(int s0,int s1)
{
	int d0 = s0 - s1;
	int ad0 = abs(d0);
	int d1 = SPTorYSize - ad0;
	if(ad0 < d1) return d0;
	return s0 > s1 ? -(SPTorYSize - s0 + s1) : SPTorYSize - s1 + s0;
};

#endif