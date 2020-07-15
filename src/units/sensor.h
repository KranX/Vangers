#define NEW_TNT

const int EXPLOSION_BARELL_RADIUS = 40;
const int TRAP_SENSOR_RADIUS = 3;
const int MAX_SENSOR_RADIUS = 200;

const int TNT_BULLET = 0;
const int TNT_BULLET_EXPLOSION = 1;

const int TNT_POWER_RADIUS = 100;
const int TNT_POWER_DAMAGE = 15 << 16;
const int TNT_POWER_IMPULSE = 50;


struct StaticObjectType
{
	enum{
		TNT = 0,
		SENSOR,
		DANGER
	};
};

struct StaticObject : GeneralObject
{
	int StaticType;
	int radius;
	char Enable;
};

struct SensorTypeList
{
	enum{
		NONE = 0,
		SENSOR = 1,
		IMPULSE = 2,
		SPOT = 3,
		ESCAVE = 4,
		PASSAGE = 5,
		TRAIN = 6,
		TRAP = 7,
		ARMOR_UPDATE = 8,
		FLY_UPDATE = 9,
		KEY_UPDATE = 10,
		FIRE_UPDATE = 11,
		RANDOMIZE_UPDATE = 12,
		TRANSPORTER = 13,
		OXIGEN_UPDATE = 14,
		EARTH_PASSAGE = 83,
		PLAYER_BIRTH = 15
	};
};

struct LocationEngine;

struct SensorDataType : StaticObject
{
	int SensorType,Mode;
	LocationEngine* Owner;
	char* Name;
	int Index;
	int TableIndex;
	Vector vData;
	int z0,Power,z1,data5,data6;	

	void CreateSensor(XStream& in,int ind = 0);
	void Close(void);
	void Touch(GeneralObject* obj);	

	void SaveSensor(XStream& out);
};

struct EngineModeList
{
	enum{
		ACCEPT_PROCESS = 0,
		ACCEPT_DELAY,
		ACCEPT_PROCESS_END,
		SEND_PROCESS,
		SEND_DELAY,
		SEND_PROCESS_END,
		WAIT,
		OPEN
	};
};

struct EngineTypeList
{
	enum{
		DOOR = 0,
		ESCAVE = 1,
		ELEVATOR = 2,
		PASSAGE = 3,
		CYCLIC = 4,
		TRAIN = 5,
		IMPULSE_ESCAVE = 6,
		TIRISTOR = 7,
		IMPULSE_SPOT = 8,
		CHECK_POINT = 9,
		LAND_SLIDE = 10,
		SIGN_PLAY = 11,
		ITEM_GENERATOR = 12
	};
};

const int LOCATION_SOUND_NONE = 0;
const int LOCATION_SOUND_ESCAVE = 1;
const int LOCATION_SOUND_BRIDGE_THREALL = 2;
const int LOCATION_SOUND_BRIDGE_FOSTRAL = 3;
const int LOCATION_SOUND_DOOR_BOOZEENA = 4;
const int LOCATION_SOUND_STONE_GLORX = 5;
const int LOCATION_SOUND_CRYPT = 6;
const int LOCATION_SOUND_IMPULSE = 7;

struct LocationEngine
{
	int NetID;
	int Type,Mode;
	int NetMode;
	MobileLocation* MLLink;
	int ActivePhase,DeactivePhase;
	int Enable;
	int ActiveTime,DeactiveTime;
	int* CheckIndex;
	int SoundID;

	int HideFlag;
	int UpdateFlag;

	Vector R_curr;
	int radius;

	int TabuUse;

	virtual void Open(Parser& in);
	virtual void Close(void);

	virtual void Quant(void);
	virtual void Link(void);
	virtual void Touch(GeneralObject* obj,SensorDataType* p);
	void NetEvent(void);
	void SoundEvent(void);
};

// ------------------------- Main Engines --------------------------------

struct CyclicEngine : LocationEngine
{
	SensorDataType** ActionLink;
	int Time,ActionMode;
	int NumAction;

	void Open(Parser& in);
	void Quant(void);
};

const int DOOR_OPEN_LOCK = 1;
const int DOOR_CLOSE_LOCK = 2;

struct DoorEngine : LocationEngine
{
	int NumSensor;
	SensorDataType** SensorLink;
	int TouchFlag,ProcessFlag;
	int Time,LockFlag;
	int Level;
	int NumTouchObject;
	int Luck;

	void Open(Parser& in);
	void Close(void);

	void Touch(GeneralObject* obj,SensorDataType* p);
	void Quant(void);
	void OpenDoor(void);
	void CloseDoor(void);
	void OpenDoor(int t);
	void CloseDoor(int t);
};

struct TiristorEngine : LocationEngine
{
	int NumSensor;
	SensorDataType** SensorLink;
	int TouchFlag,ProcessFlag;
	int LockFlag;
	int NumTouchObject;
	int Luck;

	void Open(Parser& in);
	void Close(void);
	void Touch(GeneralObject* obj,SensorDataType* p);
	void Quant(void);
	void OpenDoor(void);
	void CloseDoor(void);
};

struct TrainEngine : LocationEngine
{
	int LockFlag;
	SensorDataType* TrainLink[2];
	int EffectID;
	DangerDataType* DangerLink;

	void Open(Parser& in);
	void CreateTrain(SensorDataType* p1,SensorDataType* p2,int time);
	void Link(void);
};

struct SignPlayEngine : LocationEngine
{	
	int NumSensor;
	SensorDataType** SensorLink;
	int Time,LockFlag;
	int ReplayCount,NumReplay;

	void Open(Parser& in);
	void Quant(void);
	void Touch(GeneralObject* obj,SensorDataType* p);
	void Close(void);
};

struct EnterEngine : LocationEngine
{
	SensorDataType* ActionLink;
	EnterCenter* Owner;

	void Open(Parser& in);
	void Close(void);
	virtual void Active(void);
};

struct EnterCenter
{
	int MaxEnter,NumEnter;
	EnterEngine** Data;
	int* Lock;
	uvsTarget* Owner;
	EnterEngine** ListData;

	void Open(Parser& in);
	void Close(void);
	SensorDataType* GetCenter(void);
	void PutCenter(EnterEngine* p);
	SensorDataType* GetNearCenter(Vector& v);
	EnterEngine* GetEnterCenter(Vector& v);
	void LockedCenter(EnterEngine* p);
	void ActiveCenter(void){ Data[NumEnter]->Active(); };
	void Resort(void);
};

struct PassageEngine : LocationEngine
{
	uvsPassage* uvsPort;
	SensorDataType* ActionLink;
	char* PassageName;

	void Open(Parser& in);
	void Close(void);
	void Quant(void);
	void Link(void);
};

/*struct SpotEngine : EnterEngine
{
	int UseFlag;
	void Open(Parser& in);
	void Close(void);
	void Quant(void);
	void Active(void);
	void Touch(GeneralObject* obj,SensorDataType* p);
};*/

struct ElevatorEngine : EnterEngine
{
	DoorEngine* DoorLink;
	char* DoorName;
	int UseFlag,ActionFlag;

	void Open(Parser& in);
	void Close(void);
	void Quant(void);
	void Active(void);
	void Link(void);
	void Touch(GeneralObject* obj,SensorDataType* p);
};

struct EscaveEngine : ElevatorEngine
{
	int LastOpen;
	int StartAngle;
	void Open(Parser& in);
	void Quant(void);
	void Active(void);
};

struct ImpulseEscave : EscaveEngine
{
	SensorDataType* ImpulseLink;

	void Open(Parser& in);
	void Quant(void);
	void Active(void);
};

struct ImpulseSpot : ImpulseEscave
{
	void Open(Parser& in);
	void Quant(void);
	void Active(void);
};

struct CheckPointEngine : LocationEngine
{
	int WrongCheckMode;
	int NumActive;
	int NumCheckSensor;
	SensorDataType** CheckSensor;
	char* MainLocationName;
	char** CheckLocationName;
	LocationEngine** CheckLocation;
	LocationEngine* MainLocation;

	void Open(Parser& in);
	void Close(void);
	void Link(void);
	void Touch(GeneralObject* obj,SensorDataType* p);
};

struct LandSlideEngine : LocationEngine
{
	int LifeTime;
	int cX[4],cY[4];
	SensorDataType* SignalSensor;

	void Open(Parser& in);
	void Touch(GeneralObject* obj,SensorDataType* p);
//	void Quant(void);
};

struct ItemGenerator : LocationEngine
{
	int Time;
	char* DoorName;
	int DeviceType,DeviceP1,DeviceP2;
	LocationEngine* pLink;
	SensorDataType* SensorLink;

	void Open(Parser& in);
	void Quant(void);
	void Link(void);
	void Close(void);
};

MobileLocation* FindMobileLocation(char* Name);
SensorDataType* FindSensor(const char* Name);	
LocationEngine* FindLocation(char* Name);

int CheckKeySensor(SensorDataType* p);

//---------------------------------------------------------------------

const int TNT_NUM_LINK = 7;

extern char* TntCloneName;

#ifdef NEW_TNT

const int MAX_TNT_EXPLOSION = 7;

struct WorldBulletTemplate : BulletControlType
{
	int ID;
	int TapeSize;
	int WaitTime;
	int LifeTime;
	int BulletRadius;
	void Init(Parser& in);
};

extern WorldBulletTemplate* GameBulletData;

//zNfo константы типа WD_BULLET_BIG_FIREBALL

const int WD_BULLET_FIRE_GARDEN = 0;
const int WD_BULLET_BIG_FIREBALL = 1;
const int WD_BULLET_SMALL_FIREBALL = 2;
const int WD_BULLET_EYE_TERROR = 3;
const int WD_BULLET_XPLORATOR = 4;
const int WD_BULLET_SPORE = 5;
const int WD_BULLET_FSEED = 6;
const int WD_BULLET_GSEED = 7;
const int WD_BULLET_NSEED = 8;
const int WD_BULLET_RUFFA = 9;
const int WD_BULLET_OTHER = 10;

const int WD_BULLET_LMACHOTINE = 11;
const int WD_BULLET_HMACHOTINE = 12;
const int WD_BULLET_WORMER = 13;
const int WD_BULLET_HORDER = 14;
const int WD_BULLET_LGHORB = 15;
const int WD_BULLET_HGHORB = 16;
const int WD_BULLET_LSPEETLE = 17;
const int WD_BULLET_HSPEETLE = 18;
const int WD_BULLET_BEEBBANOZA = 19;
const int WD_BULLET_CRUSTEST = 20;
const int WD_BULLET_HYPNOTISER = 21;

//extern WorldBulletTemplate* WorldBulletData;

struct TntCreature : StaticObject
{
	int HideFlag;
	int MaxHeight;
	int CurrentHeight;
	int StartHeight;
	int DelayHeight;
	int nSource;

	int DestroyProcess;
	int DestroyDelay;
	int Time;
	int TouchTime;

	MobileLocation* TntClone;

	int TntNumLink;
	int TntLinkDelay;
	TntCreature* TntLinkData[MAX_TNT_EXPLOSION];

	unsigned int NetTime;

	void Load(XStream& in);
	void Save(XStream& in);

	void DrawQuant(void);
	void Quant(void);
	void Touch(GeneralObject* p);	
	void Destroy(void);
	void NetEvent(void);
	void NetUpdate(void);
	void ClearUpdate(void);	
	void NetQuant(void);
	void NetDestroy(int fl = 1);	

	void NetHideEvent(void){
		if(HideFlag != 1)
			HideFlag = 1;
	};

	void HideEvent(void){
		if(HideFlag != 1){
			HideFlag = 1;
			TntClone->cStage = CurrentHeight = StartHeight;
			Time = 0;
			TouchTime = 0;
		};
	};
};

//extern int TntSpore;
#else

struct TntStaticObject : StaticObject
{
	int DestroyType;
	TntStaticObject* Link[TNT_NUM_LINK];
	int NumLink;
	int DestroyTime;
	int DestroyFlag;
	int DestroyRestore;
	MobileLocation* TntClone;

	void  Touch(GeneralObject* obj);
	void Load(XStream& in);
	void Quant(uchar** lt);
};
#endif

const int  MAX_DANGER_TYPE = 2;

struct DangerTypeList
{
	enum{
		FASTSAND = 0,	//0 пески(икспло)	
		WHIRLPOOL,		//1 водоворот
		SWAMP,			//2 топь на некроссе (аналог песков)
		FIRE,			//3 выползки(икспло,арк),выстрелы(трилл)
		HOLE,			//4 раскрывающиеся дырки на дорогах некросса
		TRAIN			//5 поезд
	};
};

struct DangerDataType : StaticObject
{
	int Type;
	int Delay,Time;
	int rActive,dActive;
	int TabuUse;

	void CreateDanger(XStream& in);
	void CreateDanger(Vector v,int r,int tp);

	void Quant(void);
	void FireWork(int rFactor,int Angle);
};

void StaticOpen(void);
void StaticClose(void);
void StaticQuant(void);

void ValocTableFree(void);

EnterCenter* FindDoor(DoorEngine* p,int& type);

void StaticSort(int num,StaticObject** data,StaticObject** list);
int FindFirstStatic(int y0,int y1,StaticObject** list,int num);
int FastFindFirstStatic(int y0,int y1,int radius,StaticObject** list,int num);

#ifdef NEW_TNT
int TestLink2Link(TntCreature* p,TntCreature* n);
#else
int TestLink2Link(TntStaticObject* p,TntStaticObject* n);
void RestoreBarell(void);
void RestoreFlagBarell(void);
#endif

#ifdef NEW_TNT
extern TntCreature** TntSortedData;
extern TntCreature** TntObjectData;
#else
extern TntStaticObject** TntSortedData;
extern TntStaticObject** TntObjectData;
#endif

extern SensorDataType** SensorObjectData;
extern SensorDataType** SensorSortedData;

extern int NumKeySensor;
extern SensorDataType** KeySensorData;
extern SensorDataType** KeySensorSorted;

extern int NumLocation;
extern LocationEngine** LocationData;

extern int DngTableSize;
extern DangerDataType** DangerObjectData;
extern DangerDataType** DangerSortedData;

void StaticClearNet(void);
void ResortEnter(void);
SensorDataType* GetNearSensor(int y,int type);


const int MAX_TIME_SECRET = 2;
extern int TimeSecretEnable[2][MAX_TIME_SECRET];
extern const char* TimeSecretName[2][MAX_TIME_SECRET];
extern int TimeSecretData0[2][MAX_TIME_SECRET];
extern int TimeSecretData1[2][MAX_TIME_SECRET];
extern int TimeSecretType[2][MAX_TIME_SECRET];

extern int ThreallMessageProcess;
