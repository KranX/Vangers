#ifndef __UNITS__ITEMS_H
#define __UNITS__ITEMS_H

#include "../3d/3dgraph.h"

const int ITEM_GUN_DEVICE = 0;
const int ITEM_CHANGER = 1;
const int ITEM_OTHER = 2;

const int GUN_FIRE = 0;
const int GUN_READY = 1;
const int GUN_WAIT = 2;

const int STUFF_CREATE_NONE = 0;
const int STUFF_CREATE_TRACK = 1;
const int STUFF_CREATE_RELAX = 2;
const int STUFF_CREATE_EXRELAX = 3;
const int STUFF_CREATE_IMPULSE = 4;
const int STUFF_CREATE_DEVICE_OUT  = 5;

const int ITEM_DEFAULT_Z = 2048;
const int NETWORK_INIT_SERVER = 1;
const int NETWORK_INIT_OBJECT = 0;

struct ActionUnit;
struct GunDevice;

const int NET_OUT_OBJECT = 11083;

const int MAX_SEED_QUANT = 10;

struct DebrisObject : Object
{
	int ModelID;
	int ModelIndex;
	int Time;

	void Init(void);
	void Quant(void);
	void DrawQuant(void);
	void CreateDebris(int id,int ind);	
	void vExplosion(Object* parent,int debris_index);
};

const int ITEM_LUCK_MAX = 5;
const int ITEM_LUCK_DESTROY = 30;

struct StuffObject : Object
{
	int StuffType;
	int ModelID;
	int uvsDeviceType;
	actintItemData ActIntBuffer;
	VangerUnit* Owner;
	int CreateMode;
	int DataID;
	int SizeID;

	StuffObject* NextDeviceList;
	StuffObject* PrevDeviceList;

	LightPoint* LightData;
	int CycleTime;
	VangerUnit* PrevOwner;

	int NetDeviceID;
	int xImpulse;
	int yImpulse;
	int OutFlag;
	int NetOwner;
	int Hour,Minutes;
	int Time;
	int FindDolly;
	

	void Init(StorageType* s);
	void Quant(void);
	void DrawQuant(void);
	void Touch(GeneralObject* p);
	void CreateStuff(const Vector& _v,StuffObject* p,int cMode);
	void CreateDevice(Vector v,VangerUnit* own,StuffObject* p);
	virtual void DeviceIn(void);
	virtual void DeviceOut(Vector v1,int flag = 1,Vector v2 = Vector(-1,-1,-1));
	virtual void Active(void);
	virtual void Deactive(void);
	virtual void DeviceQuant(void);	
	virtual void DeviceLoad(Parser& in);
	virtual void GetDevice(StuffObject* p);
	void NetEvent(int type,int id,int creator,int x,int y);
	void NetDeviceEvent(int type,int id,int creator,int x,int y);
	void NetOwnerQuant(int impulse = 0);
	int GetTouchSensor(const char* name);

	void ImpulseAction(Vector v1,int flag,Vector v2);

	int CheckItemOver(void){
		if(ActIntBuffer.type == ACI_PROTRACTOR || 
		   ActIntBuffer.type == ACI_MECHANIC_MESSIAH ||
		   ActIntBuffer.type == ACI_FUNCTION83 || 
		   ActIntBuffer.type == ACI_SPUMMY ||
		   ActIntBuffer.type == ACI_BOOT_SECTOR ||
		   ActIntBuffer.type == ACI_PEELOT ||
		   ActIntBuffer.type == ACI_LEEPURINGA || 
		   ActIntBuffer.type == ACI_SANDOLL_PART1 || 
		   ActIntBuffer.type == ACI_SANDOLL_PART2 || 
		   ActIntBuffer.type == ACI_LAST_MOGGY_PART1 || 
		   ActIntBuffer.type == ACI_LAST_MOGGY_PART2 ||
		   ActIntBuffer.type == ACI_QUEEN_FROG_PART1 ||
		   ActIntBuffer.type == ACI_QUEEN_FROG_PART2 || 
		   ActIntBuffer.type == ACI_LAWN_MOWER_PART1 || 
		   ActIntBuffer.type == ACI_LAWN_MOWER_PART2 ||
		   ActIntBuffer.type == ACI_WORMASTER_PART1 ||
		   ActIntBuffer.type == ACI_WORMASTER_PART2 || 
		   ActIntBuffer.type == ACI_TABUTASK_SUCCESSFUL ||
		   ActIntBuffer.type == ACI_TABUTASK)
		   return 0;
		else
			return 1;
	};

	void SetStuffColor(void);
};

const int MAX_DEVICE_TYPE = 3;
const int DEVICE_ID_GUN = 0;
const int DEVICE_ID_CHANGER = 1;
const int DEVICE_ID_OTHER = 2;

struct BULLET_TARGET_MODE
{
	enum {
		NONE = 0,
		CONTROL,
		LEFT_ROUND,
		RIGHT_ROUND,
		RANDOM,
		WALL
	};
};

struct BULLET_CONTROL_MODE
{
	enum{
		NONE = 0,
		FLY = 1,
		TARGET = 2,
		AIM = 4,
		SPEED = 8,
		IMPULSE = 16,
		MAP_DESTRUCTION = 32,
		UNTOUCH = 64,
		PHYSIC = 128
	};
};

struct BULLET_EVENT_ID
{
	enum{
		MAP_DESTROY = 0,
		LIFE_TIME_DESTROY,
		TOUCH,
		HIDE_LIFE_TIME_DESTROY,
		HIDE_MAP_DESTROY
	};
};

struct BULLET_SHOW_TYPE_ID
{
	enum{
		PARTICLE = 0,
		FIREBALL,
		DEFORM,
		DUST,
		CRATER,
		JUMPBALL,
		LASER
	};
};

struct BULLET_TYPE_ID
{
	enum{
		CHAIN_GUN = 0,
		ROCKET,
		FIREBALL,
		TERMINATOR,
		JUMPBALL,
		LASER,
		HORDE,
		HYPNOTISE
	};
};

const int BMAX_TARGET_VECTOR = 128;
const int BMAX_TARGET_VECTOR2 = BMAX_TARGET_VECTOR << 1;

const int BULLET_OWNER_TOUCH = 1;
const int BULLET_OWNER_CHECK = 2;

const int TOUCH_SPHERE_SCALE = 4;

struct BulletControlType
{
	int BulletID,ShowID;
	int TargetMode,BulletMode;
	int ShowType,ExtShowType;
	int Power,DeltaPower,Speed;
	int Precision;
	int Time;
	int CraterType;	
	int AltOffset;
	int BulletScale;
};

struct GunSlot;

struct BulletObject : BaseObject , BulletControlType
{
	Vector vDelta,vTarget;
	GeneralObject* TargetObject;
	int FrameCount;
	GeneralObject* Owner;	
	Vector vTail;		
	LightPoint* LightData;
	Vector vWallTarget;
	int DataID;

	void Init(void);
	void Quant(void);
	void DrawQuant(void);
	void Touch(GeneralObject* p);
	void Event(int type);
	void CreateBullet(GunSlot* p,WorldBulletTemplate* n);
	void CreateBullet(Vector fv,Vector tv,GeneralObject* target,WorldBulletTemplate* p,GeneralObject* _Owner = NULL,int _speed = 0);
	void TimeOutQuant(void);
};

const int HORDE_RESTORE_MODE = 3;
const int HORDE_ATTACK_MODE = 4;
const int HORDE_WAIT_MODE = 5;

struct HordeSource : Object
{
//	int NumHorde;
	int zHorde,HordeRadius;
	int Time,DataID;

	void Quant(void);
	void DrawQuant(void);
	void CreateSource(Vector v,int nModel,int r,int z);
	void Touch(GeneralObject* p);
	int test_objects_collision();
};

struct HordeObject : BaseObject
{
	int Mode,Speed,Precision;
	VangerUnit* TargetObject;
	int NumParticle;
	SimpleParticleType* Data;
	int Power;
	Vector vDelta;
	int zAttackRadius,AttackRadius;
	GeneralObject* Owner;
	int zCruiser;
	Vector vZone;
	int Time;

	void Init(void);
	void Quant(void);
	void DrawQuant(void);
	void CreateHorde(Vector v,int r,int z,int cZ,VangerUnit* own = NULL);
	void Touch(GeneralObject* p);
	void Free(void);
};

struct JumpBallObject : Object
{
	int DataID;
	int Mode;
	int Power,DeltaPower;
	int CraterType;
	VangerUnit* Owner;

	void Init(void);
	void Quant(void);
	void DrawQuant(void);
	void CreateBullet(GunSlot* p,WorldBulletTemplate* n);
	void Touch(GeneralObject* p);
};

struct ClefObject : Object
{
	void Init(void);
	void Quant(void);
	void DrawQuant(void);
	void CreateClef(void);
	void Touch(GeneralObject *obj);
	int test_objects_collision();
};

struct SkyFarmerObject : Object
{
	int xSpeed,ySpeed;
	int CornType,Corn,Timer;
	int DropCount;
	int dTarget;
	int SeedCount;
	SensorDataType* TargetObject;

	void Init(void);
	void CreateSkyFarmer(int x_pos,int y_pos,int x_speed,int y_speed,int corn_type,int corn,int time);
	void Quant(void);
	void DrawQuant(void);
	SensorDataType* FindFarmer(int ID);
};

const int DEFAULT_MAX_AMMO = 8;

struct GunDevice : StuffObject
{	
	WorldBulletTemplate* pData;

	void CreateGun(void);
	int CheckTarget(ActionUnit* p);
	void DeviceLoad(Parser& in);
	void GetDevice(StuffObject* p);
	void DeviceIn(void);
	void DeviceOut(Vector v1,int flag = 1,Vector v2 = Vector(-1,-1,-1));
	void Quant(void);	
	void DeviceQuant(void);
};

const int FISH_WARRIOR_NONE = 0;
const int FISH_WARRIOR_PATROL = 1;
const int FISH_WARRIOR_ATTACK = 2;

const int FISH_WARRIOR_ATTACK_TIME = 100;

struct FishWarrior : Object
{
	int Mode,Speed,MaxSpeed;
	Vector vDelta;
	int Precision,Power;
	int AttackTime;
	int Time;
	int zStatus;
	DBM RotMat;
	VangerUnit* TargetObject;

	void Init(void);
	void CreateFish(Vector v,int _Speed,int Angle,int _Precision,int _Time,int _Mode,int _Model,VangerUnit* p,int Power);
	void Quant(void);
	void DrawQuant(void);
	void Touch(GeneralObject *obj);
	int test_objects_collision();
};

struct ChangerDevice : StuffObject
{
	VangerUnit* PrevOwner;

	void DeviceQuant(void);	
	void CreateChanger(void){ PrevOwner = NULL; };	
	void DeviceOut(Vector v1,int flag = 1,Vector v2 = Vector(-1,-1,-1));
};

struct GloryPlace final : GeneralObject
{
	int World;
	int Enable;
	int Index;
	LightPoint* LightData;

	void Init(int ind);
	void Quant(void);	
	void CloseWorld(void);
};

const int GLORY_PLACE_RADIUS = 40;

extern int GloryPlaceNum;
extern GloryPlace* GloryPlaceData;
extern int UsedCheckNum;

struct ItemsDispatcher : UnitList 
{
	int NumDeviceType;
	StuffObject** DeviceTypeData;
	StuffObject** StuffItemLink;
	int NumVisibleItem;

	int UnitEnergy,UnitArmor,UnitDeltaEnergy;

	void Init(Parser& in);
	void Free(void);
	void Open(Parser& in);
	void Close(void);
	void Quant(void);
	void DrawQuant(void);

	void DeleteItem(StuffObject* p);
	void NetEvent(int type,int id);
	void NetDevice(int type,int id);

	int CreateEnableCrypt(void);
	void CryptQuant(void);

	void LarvaHilator(void);
};

inline int GetColorDevice(int id)
{
	switch(id){
		case ACI_EMPTY_COPTE_RIG:
		case ACI_EMPTY_CUTTE_RIG:
		case ACI_EMPTY_CIRTAINER:
		case ACI_EMPTY_CONLARVER:
		case ACI_EMPTY_CROT_RIG:
			return COLORS_IDS::BODY_GRAY;
		case ACI_DEAD_ELEECH:
		case ACI_ROTTEN_KERNOBOO:
		case ACI_ROTTEN_PIPETKA:
		case ACI_ROTTEN_WEEZYK:
			return COLORS_IDS::ROTTEN_ITEM;
		case ACI_COPTE_RIG:
		case ACI_CUTTE_RIG:
		case ACI_CIRTAINER:
		case ACI_CONLARVER:
		case ACI_CROT_RIG:
			return COLORS_IDS::YELLOW_CHARGED;
		case ACI_TABUTASK_FAILED:
			return COLORS_IDS::BODY_RED;
		case ACI_TABUTASK_SUCCESSFUL:
			return COLORS_IDS::BODY_GRAY;
	};
	return -1;
};

void CreateDestroyEffect(Vector& v,int crat_id,int def_id,int def_flag,int expl_id,int fire_id);
int Name2Int(char* name,const char* key[],int max);

StuffObject* addDevice(int x,int y,int z,int device_type,int param1,int param2,VangerUnit* Owner,int cMode = STUFF_CREATE_NONE);

extern ItemsDispatcher ItemD;

const int PHANTOM_PARTICLE_NORMAL = 0;
const int PHANTOM_PARTICLE_MODE = 1;

const int CHECK_DEVICE_IN  = 1;
const int CHECK_DEVICE_ADD = 2;
const int CHECK_DEVICE_OUT = 3;

extern int WeaponWaitTime;

struct BulletList : UnitBaseListType
{
	BulletObject* CreateBullet(void);
	void FreeUnit(GeneralObject* p);
};

struct ClefList : UnitBaseListType
{
	ClefObject* Data;
	void Init(void);
	void Free(void);
	void FreeUnit(GeneralObject* p);
};

struct SkyFarmerList : UnitBaseListType
{
	void Init(void);
	void Free(void);
	SkyFarmerObject* CreateSkyFarmer(void);
};

struct HordeList : UnitBaseListType
{	
	void Init(void);
	HordeObject* CreateHorde(void);	
};

struct HordeSourceList : UnitBaseListType
{
	void Init(void);
	void Quant(void);
	HordeSource* CreateSource(void);
};

struct FishWarriorList : UnitBaseListType
{
	FishWarrior* Data;
	void Init(void);
	void FreeUnit(GeneralObject* p);
	void Free(void);
};

struct DebrisList : UnitBaseListType
{
	DebrisObject* CreateDebris(void);
};

struct JumpBallList : UnitBaseListType
{
	JumpBallObject* CreateBall(void);
};

extern FishWarriorList FishD;
extern HordeSourceList HordeSourceD;
extern HordeList HordeD;
extern SkyFarmerList FarmerD;
extern ClefList ClefD;
extern BulletList BulletD;
extern DebrisList DebrisD;
extern JumpBallList JumpD;

extern aiRndType GloryRnd;

void Item2ShopAction(int type);

#endif