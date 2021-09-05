#ifndef __UNITS__MECHOS_H
#define __UNITS__MECHOS_H

#include "../3d/3dobject.h"
#include "../particle/light.h"
#include "../dast/poly3d.h"
#include "hobj.h"
#include "track.h"
#include "items.h"
//#define TEST_TRACK

/*
const int WORLD_FOSTRAL = 0;
const int WORLD_GLORX = 1;
const int WORLD_NECROSS = 2;
const int WORLD_XPLO = 3;
const int WORLD_KHOX = 4;
const int WORLD_BOOZEENA = 5;
const int WORLD_WEEXOW = 6;
const int WORLD_HMOK = 7;
const int WORLD_THREALL = 8;
const int WORLD_ARKONOY = 9;
*/

const char TRK_BRANCH_MASK = 1;
const char TRK_NODE_MASK = 2;
const char TRK_IN_MASK = 4;
const char TRK_OUT_MASK = 8;
const char TRK_END_MASK = 16;
const char TRK_CHANGE_MASK = 32;

const char TRK_IN_BRANCH = TRK_BRANCH_MASK + TRK_IN_MASK;
const char TRK_CHANGE_LINK = TRK_BRANCH_MASK + TRK_CHANGE_MASK;
const char TRK_OUT_BRANCH = TRK_BRANCH_MASK + TRK_OUT_MASK;
const char TRK_END_BRANCH = TRK_BRANCH_MASK + TRK_END_MASK;
const char TRK_IN_NODE = TRK_NODE_MASK + TRK_IN_MASK;
const char TRK_OUT_NODE = TRK_NODE_MASK + TRK_OUT_MASK;

const char TRK_WAIT_NODE = 75;
const char TRK_WAIT_BRANCH = 76;
const char TRK_TARGET_NODE = 77;
const char TRK_TARGET_LINK = 78;

const char TOTAL_REACTION_SHIFT = 8;
const int TOTAL_REACTION = 1 << TOTAL_REACTION_SHIFT;

const char MECHOS_TARGET_MOVE = 1;  //0 - DIRECT_MOVE
const char MECHOS_CALC_WAY = 2;
const char MECHOS_WAIT = 4;
const char MECHOS_RECALC_FRONT = 8;

const int MECHOS_ROT_DELTA = PI / 4; //!!!!!!!!!!!!

const int CHECK_BRANCH_DELTA = 30;

const int MAX_TOUCH_VECTOR = 256;
const int MAX_TOUCH_VECTOR2 = MAX_TOUCH_VECTOR * MAX_TOUCH_VECTOR;

const int MAX_TOUCH_BORDER = 100;
const int MAX_DELTA_VECTOR = (MAX_TOUCH_VECTOR << 15) / MAX_TOUCH_BORDER;

const int MAX_TOUCH_TARGET = 200;
const int MAX_DELTA_TARGET = (MAX_TOUCH_VECTOR << 15) / MAX_TOUCH_TARGET;

const int MECHOS_VIEW_RADIUS = 500;

const char ACTION_VANGER = 0;
const char ACTION_RAFFA = 1;
const char ACTION_INSECT = 2;

const char VANGER_MOBILITY_TYPE = 0;

const int EXTERNAL_MODE_NORMAL = 1;
const int EXTERNAL_MODE_WAIT = 2;
const int EXTERNAL_MODE_MOVE = 3;
const int EXTERNAL_MODE_PASS_OUT = 4;
const int EXTERNAL_MODE_ESCAVE_OUT = 5;
const int EXTERNAL_MODE_SPOT_OUT = 6;
const int EXTERNAL_MODE_SPOT_IN = 7;
const int EXTERNAL_MODE_ESCAVE_IN = 8;
const int EXTERNAL_MODE_PASS_IN = 9;
const int EXTERNAL_MODE_FREE_IN = 10;
const int EXTERNAL_MODE_TRANSPORTER = 11;
const int EXTERNAL_MODE_SIGN_IN = 12;
const int EXTERNAL_MODE_EARTH_IN = 83;
const int EXTERNAL_MODE_EARTH_PREPARE = 69;
const int EXTERNAL_MODE_IN_VANGER = 20;
const int EXTERNAL_MODE_OUT_VANGER = 21;
const int EXTERNAL_MODE_LIGHT = 22;
const int EXTERNAL_MODE_DARK = 23;
const int EXTERNAL_MODE_PASS_IMPULSE = 24;

#ifdef TEST_TRACK
const int EXTERNAL_MODE_TEST = 110;
const int EXTERNAL_MODE_TEST_MOVE = 111;
#endif

struct WaveProcess;

const char LOCATOR_ECHO_TARGET = 1;
const char LOCATOR_ECHO_VIEW = 2;

const int UNIT_MAX_TARGET_TYPE = 4;
const int UNIT_TARGET_MECHOS = 0;
const int UNIT_TARGET_ITEM = 1;
const int UNIT_TARGET_BULLET = 2;
const int UNIT_TARGET_BARELL = 3;

const int UNIT_MAX_TARGET_MODE = 7;

const int PASSING_WAVE_PROCESS = 1;
const int PASSING_WAVE_RADIUS = 32;
const int PASSING_WAVE_RADIUS2 = PASSING_WAVE_RADIUS << 1;

const int KHOX_ARMOR_SHIFT = 10;

const int MAX_KHOX_CHECKPOINT = 10;

const int CHANGE_VANGER_TIME = 100;

const int VANGER_POWER_NONE = 0;
const int VANGER_POWER_RUFFA_GUN = 1;
const int VANGER_POWER_FLY = 2;
const int VANGER_POWER_SWIM_ALWAYS = 4;

const int RUFFA_GUN_WAIT = 51;

const int DEVICE_MASK_COPTE = 1;
const int DEVICE_MASK_MOLE = 2;
const int DEVICE_MASK_SWIM = 4;

const int VANGER_RACE_NONE = 0;
const int VANGER_RACE_ELR = 1;
const int VANGER_RACE_PPS = 2;
const int VANGER_RACE_KRW = 3;
const int VANGER_RACE_ZLP = 4;

const int EXTERNAL_IMPULSE_TIME = 50;

const int MAX_PASSAGE_DELAY = 50;

struct UnitItemMatrix
{
	int ID;
	int NumID;
	UnitItemMatrix** DataID;
	int NumSlot;
	int* SlotSize;
	int* nSlot; 
	int FullNum;

	void Open(Parser& in);
	void Close(void);	
	int GetFullNum(void);
	int CheckSize(int sz,int*& p);
	int GetSize(int sz,int*& p);
	int CheckFree(int*& p);
};

const int UNIT_SUB_MATRIX_NUM = 9;

extern int NumUnitMatrix;
extern UnitItemMatrix* UnitMatrixData;

struct uvsUnitType : uvsKernel
{
	int NumDevice;
	StuffObject* DeviceData;

	UnitItemMatrix* ItemMatrix;
	int* ItemMatrixData;

	int MaxVolume,Volume;
	int MaxEnergy,MaxArmor;
	int Armor,Energy;
	int dEnergy,DropEnergy;
	int DelayDrop,DropTime;
	int uvsMaxSpeed;	
	int MaxOxigenResource,OxigenResource;
	int MaxPassageCount,PassageCount;

	int DestroyClass;

	int PowerFlag,aiPowerFlag;
	int ImpulsePower;

	void Init(void);
	void Free(void);

	void CreateUnitType(uvsVanger* p);

	void AddDevice(StuffObject* p);
	void DelDevice(StuffObject* p);

	virtual void ItemQuant(void);	
	virtual void Go2World(void);
	virtual void Go2Universe(void);
	virtual char CheckInDevice(StuffObject* p);
	virtual void CheckOutDevice(StuffObject* p);

	void AddOxigenResource(void);
	void UseOxigenResource(void);	
};

struct ActionUnit : Object
{
	Vector vDirect,vEnvir;
	int EnvirAngle,EnvirLen;
	int NumCalcUnit,NumViewUnit;
	int MaxVelocity;

	int ViewRadius;

	char SpeedDir;
	int MinSpeed,MaxSpeed,CurrSpeed;
	int MaxHideSpeed;
	int MoveAngle,DeltaSpeed,DeltaTraction;
	char OtherFlag;

	Vector vUp,vDown;
	int Speed,Angle;
	DBM RotMat,MovMat;
	int PrevVisibility;

	int Count;

	int WavePhase;
	WaveProcess* wProcess;

	int nModel;

	void Init(StorageType* s);
	void Free(void);
	void Open(void);
	void Close(void);

	void DrawQuant(void);

	void CreateActionUnit(int nmodel/*Object& _model*/,int _status,const Vector& _v,int _angle,int put_mode);

	virtual void Quant(void);
	virtual void InitEnvironment(void);
	virtual void keyhandler(int key){std::cout<<"virtual ActionUnit::keyhandler key:"<<key<<std::endl;};

	virtual void Hide2Show(void);
	virtual void DeleteHandler(void);

	void CalcTrackVector(Vector& v,int& len,int& angle1,int& angle2);
	void Action(void);
	void HideAction(void);

	void MixVector(void);

	void Touch(GeneralObject* obj);	
};

const int AI_NAVIGATION_NORMAL = 1;
const int AI_NAVIGATION_FAR = 2;

struct TrackUnit : ActionUnit , TrackLinkType
{
	int aiMoveMode,aiMoveFunction;
	TrackLinkType TestPoint,FrontPoint;	
	int* WayDist;
	int* WayDistData;
	int* CalcWayDist;
	int* TrackDist;

	int NoWayDirect;

	TrackLinkType TargetPoint;

	Vector vTarget;
	int TargetAngle,TargetLen;
	int EnvirReaction,TargetReaction;

	int NullTime;

	char MoveDir,FrontDir;
	BranchType* PrevBranch;

	int CheckPosition,MaxCheckPosition;
	int CheckWayCount,MaxWayCount;

	int DestroyPhase;

	int PrevWheelX[6];
	int PrevWheelY[6];
	int PrevWheelFlag[6];
//	int nLeftWheel,nRightWheel;

	void Init(StorageType* s);
	void Free(void);
	void Open(void);
	void Close(void);

	void Quant(void);

	void CreateTrackUnit(void);

	int TraceWay(void);
	void WayInit(void);

	void GetNode(TrackLinkType* tp,char& dir);
	void GetInside(void);
	char GetDirect(TrackLinkType* tp);

	char Check2Position(int fx,int fy,int num);
	char TestAnalysis(TrackLinkType& tp);

	void GetBranch(void);
	void HideGetBranch(void);
	char ActiveGetBranch(void);

	void MixVector(void);
	void GetTrackVector(void);

	void DirectQuant(void);
	void TrackQuant(void);

	void LastFire(void);

	void DrawMechosParticle(int x,int y,int speed,int level,int n);

	void Hide2Show(void);
	void MakeTrackDist(void);

	int gRnd;
	int GeneralRND(unsigned int  m){
		gRnd ^= gRnd >> 3;
		gRnd ^= gRnd << 28;
		gRnd &= 0x7FFFFFFF;
		if(!m) return 0;
		return gRnd % m;
	};
};

struct  CharacterDataType
{
	int Aggressia,Hunting;
	float KillerInstinct,FetishMaina;
	float Aggressia2Hunting,RobberFactor;

	void GetStuffOrder(int d,int a,int item,StuffObject* s);
	void GetVangerOrder(int d,int a,VangerUnit* p,VangerUnit* n);
	void GetInternalStuffOrder(int d,int a,int item,StuffObject* s,VangerUnit* p,VangerUnit* n);
};

extern int vSetVangerFlag;

const int AI_STOP = 0;
const int AI_SLAVE = 1;

const int MAX_AMMO_USE = 2;

struct GunSlot
{
	int NetID;
	int StuffNetID;
	int nSlot;
	GunDevice* ItemData;
	WorldBulletTemplate* pData;
	int RealSpeed;
	int Time;
	int GunStatus;
	Vector vPlace,vFire;
	DBM mPlace,mFire;
	VangerUnit* Owner;
	int ControlFlag;
	GeneralObject* TargetObject;
	GeneralObject* aiTargetObject;
	int FireCount,NetFireCount;
	int TableIndex;

	void OpenSlot(int slot,VangerUnit* own,int ind);
	void CloseSlot(void);
	void OpenGun(GunDevice* p);
	void CloseGun(void);
	void Fire(void);
	void Quant(void);
	int CheckTarget(ActionUnit* p);
	
	void RemoteFire(void);

	void NetEvent(unsigned int time);
	void NetStuffQuant(void);
	void NetUpdate(void);
};

//Global Events
const int AI_EVENT_DROP = 0;
const int AI_EVENT_CAPTURE = 1;
const int AI_EVENT_COLLISION = 2;
const int AI_EVENT_MAX = 3;

//Local Events
const int AI_EVENT_VANGER = 4;
const int AI_EVENT_ITEM = 5;

//Check Envirinment
const int AI_STATUS_NONE = 0;
const int AI_STATUS_NOISE = 1;
const int AI_STATUS_SPEED_FACTOR = 2;
const int AI_STATUS_WHEEL = 4;
const int AI_STATUS_FLY = 8;
const int AI_STATUS_SWIM = 16;
const int AI_STATUS_IMPULSE = 32;
const int AI_STATUS_SIDE = 64;
const int AI_STATUS_TNT = 128;
const int AI_STATUS_TARGET = 256;
const int AI_STATUS_WALL = 512;

const int AI_MODIFIER_ELEEPOD = 0;
const int AI_MODIFIER_BEEBOORAT = 1;
const int AI_MODIFIER_ZEEX = 2; 

struct aiListElement
{
	aiListElement* Next;	
	aiListElement* Prev;
};

struct aiListType
{
	aiListElement* Tail;
	int Num;
	void Init(void);
	void Free(void);
	void Connect(aiListElement* p);
	void Disconnect(aiListElement* p);
};

struct aiUnitEvent : aiListElement
{
	int ID;
	int Refresh;
	int Time;	
	VangerUnit* Subj;	
	GeneralObject* Obj;
};

extern int TotalVangerSpeed;

extern int aiReadEvent,aiWriteEvent;
const int AI_GLOBAL_EVENT_MAX = 500;
extern int aiNumGlobalEvent[2][AI_EVENT_MAX];
extern aiUnitEvent aiGlobalEventData[2][AI_EVENT_MAX][AI_GLOBAL_EVENT_MAX];

#define PUT_GLOBAL_EVENT(type,id,obj,subj)  { if(aiNumGlobalEvent[aiWriteEvent][type] < AI_GLOBAL_EVENT_MAX){\
										    aiGlobalEventData[aiWriteEvent][type][aiNumGlobalEvent[aiWriteEvent][type]].Obj = obj;\
										    aiGlobalEventData[aiWriteEvent][type][aiNumGlobalEvent[aiWriteEvent][type]].Subj = subj;\
										    aiGlobalEventData[aiWriteEvent][type][aiNumGlobalEvent[aiWriteEvent][type]].ID = id;\
										    aiGlobalEventData[aiWriteEvent][type][aiNumGlobalEvent[aiWriteEvent][type]].Refresh = 1;\
										    aiNumGlobalEvent[aiWriteEvent][type]++;}\
										}\

const int AI_RESOLVE_ATTACK = 0;
const int AI_RESOLVE_FIND = 1;
const int AI_RESOLVE_NOWAY = 2;
const int AI_RESOLVE_FORCE = 3;

struct aiUnitResolve : aiListElement
{
	int Type;
	UnitOrderType Obj;
	int rStatus;
	int Time;
	int Level;
};

const int AI_MOVE_TRACK = 0;
const int AI_MOVE_POINT = 1;
const int AI_MOVE_ROLL = 2;

const int AI_MOVE_FUNCTION_WHEEL = 0;
const int AI_MOVE_FUNCTION_FLY = 1;
const int AI_MOVE_FUNCTION_IMPULSE = 2;
const int AI_MOVE_FUNCTION_SWIM = 3;
const int AI_MOVE_FUNCTION_MOLE = 4;
const int AI_MOVE_FUNCTION_SIDE = 5;

const int AI_RESOLVE_STATUS_NONE = 0;
const int AI_RESOLVE_STATUS_DISCONNECT = 1;
const int AI_RESOLVE_STATUS_DEACTIVE = 2;
const int AI_RESOLVE_STATUS_VAR = 4;
const int AI_RESOLVE_STATUS_TARGET = 8;
const int AI_RESOLVE_STATUS_VIEW = 16;

const int MAX_AI_ACTION_TYPE = 12;

const int AI_ACTION_BRAKE = 0;
const int AI_ACTION_AUTO = 1;
const int AI_ACTION_RUFFA = 2;
const int AI_ACTION_SLAVE = 3;
const int AI_ACTION_RACER = 4;
const int AI_ACTION_FARMER = 5;
const int AI_ACTION_RANGER = 6;
const int AI_ACTION_THIEF = 7;

const int MAX_CHECK_AROUND = 5;

struct aiResolveList : aiListType
{
	void ClearResolve(void);	
	void ClearResolve(int type,UnitOrderType obj);
	void ClearResolveForce(UnitOrderType obj);

	aiUnitResolve* FindResolve(int type,UnitOrderType obj);
	aiUnitResolve* AddResolve(int type,UnitOrderType obj,int level = 0,int status = AI_RESOLVE_STATUS_NONE);
	aiUnitResolve* FastAddResolve(int type,UnitOrderType obj,int level = 0,int status = AI_RESOLVE_STATUS_NONE);
};

const int NET_OPEN_DOOR = 1;

const int MAX_TRACTION_CHECK = 30;
const int MAX_WALL_TIME = 10;
const int MAX_TRACTION_CHECK_DELTA = 10;

const int FRONT_VANGER_COLLISION = 30;
const int BACK_VANGER_COLLISION = 10;
const int SIDE_VANGER_COLLISION = 20;

const int MAX_TRACTION_CHECK_TIME = 40;

const int AI_NO_WAY_NONE = 0;
const int AI_NO_WAY_CALC = 1;
const int AI_NO_WAY_PROCESS = 2;

const int AI_MAX_ALARM_TIME = 50;

const int MAX_AI_FACTOR = 4;
const int AI_FACTOR_IGNORE = 0;
const int AI_FACTOR_NOWAY = 1;
const int AI_FACTOR_ATTACK = 2;
const int AI_FACTOR_RUN = 3;

const int MAX_CHECK_VISIBLE_FACTOR = 4;
const int MAX_CHECK_UNVISIBLE_FACTOR = 8;

const int AI_ADD_FRONT_COLLISION = 0;
const int AI_ADD_BACK_COLLISION = 1;
const int AI_ADD_SIDE_COLLISION = 2;
const int AI_ADD_ATTACK = 3;

struct aiFactorType
{
//MainSection
	int aiSection;

	int aiAttackUp,aiAttackDown;
	int aiRunUp,aiRunDown;
	int aiIgnoreUp,aiIgnoreDown;
	int aiNoWayUp,aiNoWayDown;

	float aiFactorAttack,aiFactorRun,aiFactorIgnore,aiFactorNoWay;
	float aiRacerAttack;

	int aiMainLevel,aiDeltaLevel;

	int aiFindLevel;

	int aiAddAttack,aiMaxAttack;
	int aiAddView,aiMaxView;
	int aiAddFrontCollision,aiMaxFrontCollision;
	int aiAddBackCollision,aiMaxBackCollision;
	int aiAddSideCollision,aiMaxSideCollision;
	int aiAddAlarm,aiMaxAlarm;

	float aiFuryLevel;
//AlarmSection

	float aiAlarmRun,aiAlarmAttack;
	float aiAmmoRun,aiAmmoAttack;
	float aiArmorRun,aiArmorAttack;	
	
//TargetSection

	float aiMultyAttack,aiTargetAttack,aiTargetFind;

//OtherSection
	float aiDollyAttack,aiDollyRun;
	float aiDominanceAttack,aiDominanceRun;

	float aiFrontOverAttack,aiFrontOverRun,aiFrontOverIgnore,aiFrontOverNoWay;
	float aiBackOverAttack,aiBackOverRun,aiBackOverIgnore,aiBackOverNoWay;
	float aiFrontUnderAttack,aiFrontUnderRun,aiFrontUnderIgnore,aiFrontUnderNoWay;
	float aiBackUnderAttack,aiBackUnderRun,aiBackUnderIgnore,aiBackUnderNoWay;

	float aiVisibleRun,aiVisibleAttack;
	float aiInvisibleRun,aiInvisibleAttack;
	float aiAttackTime;	
	
	void FactorOpen(aiFactorType* in);
	void FactorLoad(Parser& in);	
};

const int AI_FACTOR_NUM = 13;

const int AI_FACTOR_ALARM_FREEZER = 0;
const int AI_FACTOR_ALARM_NORMAL = 1;
const int AI_FACTOR_ALARM_FUNK = 2;

const int AI_FACTOR_TARGET_MONO = 3;
const int AI_FACTOR_TARGET_FREEZER = 4;

const int AI_FACTOR_OTHER = 5;

const int AI_FACTOR_RACER = 6;
const int AI_FACTOR_RANGER = 7;
const int AI_FACTOR_THIEF = 8;
const int AI_FACTOR_KILLER = 9;
const int AI_FACTOR_TRANSPORT = 10;

const int AI_FACTOR_OTHER_GAMER = 11;
const int AI_FACTOR_ALARM_GAMER = 12;


const float AI_MAX_DOLLY_FACTOR = 10000;

const int MAX_ADD_ATTACK_TIME = 40;

extern aiFactorType aiFactorData[AI_FACTOR_NUM];

const int TABUTASK_COUNT_NONE = 0;
const int TABUTASK_COUNT_DAMAGE = 1;
const int TABUTASK_COUNT_SPEETLE = 2;
const int TABUTSAK_COUNT_RITUAL = 4;
const int TABUTSAK_COUNT_FLY = 8;
const int TABUTASK_COUNT_OTHER = 16;

const int SAFE_STUFF_MAX = 3;

struct VangerUnit : TrackUnit , uvsUnitType , aiFactorType
{
	Vector vMove;

	int ShellNetID;
	int ShellUpdateFlag;

	int aiActionID;
	int aiStatus;

	aiResolveList aiResolveFind;
	aiResolveList aiResolveAttack;
	aiListType aiEvent;

	int NoWayEnable;

	Vector aiNearTrack;
	int aiResolveWayEnable;
	int aiModifier;	
	aiUnitResolve* aiLocalTarget;

	int aiScanDist;
	int aiReactionTime,aiReactionTimeMax;
	int aiVangerRadius;
	int aiFirstItem,aiSecondItem;
	int aiItemRadius;

	int MixVectorEnable;

	BranchType* aiRunBranch;
	NodeType* aiRunNode;
	Vector vRunStartPosition;
	int RunStartDist;
	int RunTimeProcess;

	void TargetAnalysis(void);
	void ResolveGenerator(void);
	
	void NoWayHandler(void);

	void ResolveHandler(aiUnitResolve* p);
	int CheckReturn2Track(void);	
	void GetForceWay(int d2,Vector& v);
	void WeaponGenerator(void);
	int CheckNearTrack(void);	

	aiUnitEvent* AddEvent(int id,GeneralObject* obj,VangerUnit* subj);
	void ClearSubjEvent(VangerUnit* subj);
	void ClearObjEvent(GeneralObject* obj);
	void ClearEvent(GeneralObject* obj);
	
	void MainOrderInit(void);
	int GetAllert(VangerUnit* p);
	void StuffDestroyHandler(GeneralObject* p);
	void VangerDestroyHandler(GeneralObject* p);		
	void InitAI(void);
	void CheckFind(aiUnitResolve* p,Vector v);

	void ResolveHandlerForce(aiUnitResolve* p);
	void ResolveHandlerFind(aiUnitResolve* p);
	void ResolveHandlerAttack(aiUnitResolve* p);

	aiUnitResolve* AddFindResolve(int type,UnitOrderType obj,int level = 0,int status = AI_RESOLVE_STATUS_NONE);
	aiUnitEvent* FindEvent(VangerUnit* subj,GeneralObject* obj);
	void NoWayInit(void){if(NoWayEnable == AI_NO_WAY_NONE) NoWayEnable = AI_NO_WAY_CALC;};
	void CalcImpulse(void);
	void CalcForce(void);

	void CalcWeaponDelta(void);
	void AddEventTime(VangerUnit* p,int delta,int mode);

	void ResolveFindDestroy(aiUnitResolve* p);
	void GamerOrderInit(void);

	int aiMaxJumpRadius;
	int WeaponDelta;

	int SeedNum;
	int MaxSeed;

	int LowAmmo;
	int LowArmor,ArmorAlarm;

	int aiAlarmTime;
	int aiDamage;
	int SpeedFactorTime;
	VangerUnit* aiDamageData;

	float aiResolveFactor[MAX_AI_FACTOR];

	void CalcAiFactor(VangerUnit* p,int ind);

	int aiRelaxTime,aiRealxRadius,aiRelaxTimeMax;
	Vector aiRelaxData;

	int WallCollisionTime;
	int DeltaTractionTime,TractionValue,TractionStat;

	StuffObject* CoptePoint;
	StuffObject* MolePoint;
	StuffObject* SwimPoint;

	int AttackRadius;
	int aiReactionCheckTime,aiReactionMode,aiReactionCheckTimeMax;

	int AddAttackTime;

	int BeebonationFlag,SensorEnable;

	void DestroyEnvironment(void);
	void GetWeaponDelta(void);

	int UseDeviceMask;
	int VangerRaceStatus;
//----------------------------------------------------------
	int DoorFlag,nDoorFlag;
	int NetCreateID;
	int NetExternalSensor;
	unsigned char NetDestroyID;		

	int LastMole;
	dastPoly3D* MoleTool;
	dastPoly3D* MolePoint1;
	dastPoly3D* MolePoint2;
	int Molerizator;
	int VangerCloneID;

	VangerUnit* VangerChanger;
	int MechosChangerType;
	int VangerChangerCount;
	Vector vChangerPosition;
	int VangerChangerAngle;
	int VangerChangerArmor,VangerChangerEnergy;
	int VangerChangerColor;

	unsigned char NetChanger;
	unsigned char NetFunction;
	unsigned int NetRuffaGunTime;
	unsigned int NetProtractorFunctionTime,NetMessiahFunctionTime;
	unsigned int NetFunction83Time,PrevNetFunction83Time;

	GunSlot GunSlotData[MAX_ACTIVE_SLOT];

	int RuffaGunTime;

	SensorDataType* TouchKeyObject;
	int TouchKeyObjectFlag;

	int ExternalMode,ExternalTime,ExternalLock,ExternalDraw,ExternalAngle;
	int ExternalTime2;
	SensorDataType* ExternalObject;
	SensorDataType* ExternalLastSensor;
	SensorDataType* ExternalSensor;
	int RandomUpdate;

	PlayerData* pNetPlayer;	
	
	int NetworkArmor,NetworkEnergy;	
	int TabuUse;

//For Secret WORLD
	int CheckPointCount;
	int MaxKhoxPoison,KhoxPoison;

	int PlayerDestroyFlag;		

	int PrevImpuseFrame;

	void Init(StorageType* s);
	void Free(void);

//	void CreateAI(void);

	void TouchSensor(SensorDataType* p);
	void AutomaticTouchSensor(SensorDataType* p);
	void StopTouchSensor(SensorDataType* p);

	void Touch(GeneralObject* p);
	void SensorQuant(void);

	void Action(void);
	void HideAction(void);

	void Quant(void);
	void DrawQuant(void);

	void Destroy(void);
	void keyhandler(int key);
	void NewKeyHandler(void);

	void InitEnvironment(void);
	void CreateVangerUnit(void);

	void CreateParticleRotor(const Vector& v,int r);
	void CreateParticleMechos(const Vector& v,int r, int type = 0);	

	void AddFree(void);
	void AddPassage(SensorDataType* p);
	void AddEscave(SensorDataType* p);
	void AddSpot(SensorDataType* p);

	void ItemQuant(void);	
	void Go2World(void); //znfo !!!
	void Go2Universe(void); //znfo !!!
	char CheckInDevice(StuffObject* p); //znfo !!!
	void CheckOutDevice(StuffObject* p); //znfo !!!

	void Hide2Show(void);

	void DeleteHandler(void);

	void BulletCollision(int pow,GeneralObject* p);
	void DestroyCollision(int l,Object* p);
	int test_objects_collision();

	void LocalScan(void);
	void SwitchTarget(void);

	void TerrainQuant(void);

	void MoleProcessQuant(void);
	void SetMechos(int n); //znfo !!!

#ifdef TEST_TRACK
	void DestroyAroundBarrel(void);
#endif
	int CheckStartJump(void);

	void DrawMechosParticle(int x,int y,int speed,int level,int n);

	void SendCameraData(void);
	void ChangeVangerProcess(void);
	void InCarNator(void); //znfo !!!
	void OutCarNator(void); //znfo !!!

	void StartMoleProcess(void);
	void MapQuant(void);
	void CheckAmmo(void);

	int UsingCopterig(int decr_8);
	int UsingCrotrig(int decr_8);
	int UsingCutterig(int decr_8);

	int CheckSpeetle(void);

	void NetCreateVanger(uvsPassage* pp,uvsEscave* pe,uvsSpot* ps);
	void NetEvent(int type,int id,int creator,int time,int x,int y,int radius_);
	void ShellNetEvent(int type,int id,int creator,int time,int x,int y,int radius_);
	void NetCreateSlave(void);	
	void InitPlayerPoint(PlayerData* p);	

	int CheckInMatrix(int sz);
	int CheckOutMatrix(int sz);

	void ShellUpdate(void);

	void DischargeItem(StuffObject* p);
	void DestroyItem(StuffObject* p);

	void ClearItemList(void);
};

struct InsectUnit : ActionUnit
{
	Vector Target;
	int BeebType;

	void Init(void);

	void CreateInsect(void);

	void Quant(void);
	void InitEnvironment(void);
	void Touch(GeneralObject* p);
	int test_objects_collision();
	void HideAction(void);
};

struct ModelDispatcher
{
	int MaxModel;
	Object* Data;
	char** NameData;

	void Init(Parser& in);
	void Free(void);

	Object& ActiveModel(char id);
	int FindModel(const char* name);
};

struct CompasTargetType
{
	int ID;
	UnitOrderType Data;
	char* Name;
	char* aciName;
	CompasTargetType* Next;
	CompasTargetType* Prev;
	int dFlag;
};

struct CompasObject
{
	int x,y;
	int FrameCount;
	Vector vMove;
	CompasTargetType* TargetData;
	CompasTargetType* CurrentTarget;

	void Init(void);
	void Open(void);
	void Close(void);
	void Free(void);
	void Quant(void);
	void AddTarget(int id,UnitOrderType d,char* n1,const char* n2);
	void DeleteTarget(CompasTargetType* p);
};

//const int SPEETLE_AMMO = 0;
//const int CRUSTEST_AMMO = 1;

const int RES_DRAW_LEFT = 150;
const int RES_DRAW_DOWN = 80;
const int RES_DRAW_STEP_Y = 10;
const int RES_DRAW_MAX_SIZE = 300;

const int FUNCTION_OFF = 0;
const int PROTRACTOR_OPEN_SPODS = 1;
const int PROTRACTOR_SCALE_UP = 2;
const int PROTRACTOR_SCALE_DOWN = 3;
const int PROTRACTOR_PALLADIUM = 4;
const int PROTRACTOR_BEEBOS_DANCE = 5;
const int PROTRACTOR_JESTEROID = 6;
const int PROTRACTOR_MOLERIZATOR = 7;
const int PROTRACTOR_PREPASSAGE  = 8;
const int MECHANIC_UNVISIBLE = 9;
const int MECHANIC_GAME_OVER = 10;
const int MECHANIC_ITEM_FALL = 11;
const int MECHANIC_FIRE_GARDEN = 12;
const int MECHANIC_BEEB_NATION = 13;

const int PALLADIUM_RADIUS = 80;
const int MOLERIZATOR_RADIUS = 40;
const int MOLERIZATOR_NUM = 100;
const int FIRE_GARDEN_RADIUS = 40;
const int FIRE_GARDEN_START = 50;
const int FIRE_GARDEN_NUM = 8;

struct VangerFunctionType
{
	int ID;
	Vector vR;
	int Time,LifeTime;
	VangerFunctionType* Next;
	VangerFunctionType* Prev;

	void Init(int _ID,Vector _vR,int _Time,int _External = 0);
	void Quant(void);
	void SoundQuant(void);
};

const int LOCATOR_DATA_SIZE = 100;
const int LOCATOR_DATA_RADIUS = 300;

const int GAME_OVER_EVENT_TIME = 20*15;

struct ActionDispatcher : UnitList 
{
	int PassageTouchEnable;

	int CameraModifier;
	int SpobsEntrance;
	VangerUnit* Active;

	dastPoly3D* SignEngine;
	int hMokKeyEnable,XploKeyEnable;

	int LocatorNum,LocatorOffset;
	Object* LocatorData[LOCATOR_DATA_SIZE];
	Object* LocatorPoint;

	LightPoint LightData;
	LightPoint* ProtractorLight;
	int NumVisibleVanger;

	int NumResolve;

	int PromptPodishFreeVisit,PromptIncubatorFreeVisit;
	int PromptChangeCycleCount;
	int PromptCurrentWay;
	int PromptPodishCount,PromptIncubatorCount;
	int PromptPrevY,PromptPrevTime;

	StuffObject* BigZek;
	int WorldSeedNum;

	int SpobsDestroy,ThreallDestroy;
	int SpobsDestroyActive,ThreallDestroyActive;
	int FunctionSpobsDestroyActive,FunctionThreallDestroyActive;
	int SpummyRunner;
	int LuckyFunction;

	unsigned char NetFunctionProtractor,NetFunctionMessiah;
	int NetEnterOffset;
//	int FishWarriorNum;

	void Init(Parser& in);
	void Free(void);
	void Open(Parser& in);
	void Close(void);
	void Quant(void);
	void keyhandler(int key);

	VangerUnit* GetNextVanger(ActionUnit* p);
	void DeleteUnit(ActionUnit* p);
	void CameraQuant(void);

	void ActiveAllTerminator(void);
	void ActiveTerminatorSlot(int n);

//-----------------------------------------------------------------------------	

	int DrawResourceMaxValue;
	int DrawResourceValue;
	int DrawResourceTime;	

//	int HotBug;	
	int DoorEnable;

	StuffObject* Slot[MAX_ACTIVE_SLOT];
	
	void CreateActive(VangerUnit* p);

	void CheckDevice(StuffObject* p);
	void SlotIn(int n,StuffObject* p);
	void SlotOut(StuffObject* p);

	void ActiveAllSlots(void);
	void DeactiveAllSlots(void);
	void ActiveSlot(int n);

	void AddWaterResource(void);
	void AddFireResource(void);
	void AddFlyResource(void);
	
	void UseFlyResource(void);
	
	void DrawResource(void);
	void UseMole(void);

	int FlyTaskFirstY,FlyTaskLastY,FlyTaskDirect;
	int FlyTaskEnable;
//----------------------------------------------------------------

	VangerUnit* pfActive;
	VangerUnit* mfActive;


	VangerFunctionType* fTail;
	void AddFunction(VangerFunctionType* p);
	void DeleteFunction(VangerFunctionType* p);
	int NewFunction(int id,int tp);
	void FunctionQuant(void);
	void ClearMessiah(void);
	void ClearProtractor(void);
	void AddCoolFunction(int _ID,Vector _vR,int _Time,int _External = 0);

	void ChangeLocator(void);
	void NetEvent(int type,int id);
	void ShellNetEvent(int type,int id);

	void PromptInit(int ind);
	void PromptChangeCycle(void);
	void PromptQuant(void);
};


//zNfo beeb price

const int MAX_INSECT_TYPE = 3;
const int INSECT_PRICE_DATA[MAX_INSECT_TYPE] = {1,10,100};

struct InsectList : UnitBaseListType
{
	int NumInsect[MAX_INSECT_TYPE];

	InsectUnit* Data;
	void Init(void);
	void FreeUnit(GeneralObject* p);
	void Free(void);
	void Quant(void);
};

extern InsectList InsectD;

extern ActionDispatcher ActD;
extern ModelDispatcher ModelD;

char CheckAddLink(LinkType* l,BranchType* b,char s);
char CheckDecLink(LinkType* l,BranchType* b,char s);

int GetAngle(int x0,int y0,int x1,int y1,int x2,int y2);
char CheckInLink(Vector& v,LinkType* pl,LinkType* nl);
char CheckInNode(Vector& v,NodeType* n);
char CheckInBranch(Vector& v,LinkType* pl,LinkType* nl,BranchType* b);

int LinkDist(int x,int y,int x1,int y1,int x2,int y2);
void LinkDist(int x,int y,int x1,int y1,int x2,int y2,int radius,Vector& v);
void PointDist(int dx,int dy,int dz,int r,int radius,char dir,Vector& vd,Vector& v);
void LenDist(int x,int y,int z,int x1,int y1,int z1,int dist,int radius,Vector& v);
void AxisLen(Vector vA0,Vector vA1,Vector vC,Vector& vR);

char GetNodeDist(TrackLinkType* p1,TrackLinkType* p2);

void section_prepare(int xg0,int yg0,int xg1,int yg1,int xg2,int yg2,int xg3,int yg3,int z_mechous,
		       double& A,double& B,double& Z_AVR,DBV& normal);

StuffObject* GetStuffObject(uvsUnitType* g,int n);

void ClearTabutaskTarget(void);

extern CompasObject CompasObj;

extern Vector vInsectTarget;
extern int UnitGlobalTime;

int ChargeWeapon(VangerUnit* p,int ind,int sign);
int ChargeDevice(VangerUnit* p,int ind,int sign);
void CreatePhantomTarget(void);
void ObjectDestroy(GeneralObject* p,int mode = 1);
void CreateTabutaskTarget(void);


void CheckPlayerList(void);
void NetEvent4Uvs(PlayerData* p);

void NetSlotEvent(int type,int id);

inline int isTerminator(int i)
{
	if(i == ACI_TERMINATOR || i == ACI_TERMINATOR2 || i == ACI_EMPTY_AMPUTATOR || i == ACI_EMPTY_DEGRADATOR || i == ACI_EMPTY_MECHOSCOPE)
		return 1;
	else 
		return 0;
};

extern XBuffer RaceTxtBuff;

const int NUM_CHECK_BSIGN = 13;
const int NUM_CONTROL_BSIGN = 7;
extern int CHECK_BSIGN_INDEX[NUM_CHECK_BSIGN];
extern char* CHECK_BSIGN_DATA;

void camera_reset();
void start_vibration();
void ClearPhantomTarget(int id);
void AddPhantomTarget(PlayerData* n);
void DeleteArtefactTarget(StuffObject*n);
void CreateArtefactTarget(StuffObject* n);

//Fly Podish->Incubator
//Fly Lampasso->Ogorod
//Fly Zeepa ->B-Zone

extern int aiHotBugData00;
extern int aiHotBugData01;
extern int aiHotBugData02;
extern int aiHotBugData03;

extern int aiHotBugAdd00;
extern int aiHotBugAdd01;
extern int aiHotBugAdd02;
extern int aiHotBugAdd03;


inline int aiGetHotBug(void)
{	
	return(((aiHotBugData00 ^ aiHotBugAdd03) & 0xf000000f) | 
		((aiHotBugData01 ^ aiHotBugAdd02) & 0x0f0000f0) | 
		((aiHotBugData02 ^ aiHotBugAdd01) & 0x00f00f00) | 
		((aiHotBugData03 ^ aiHotBugAdd00) & 0x000ff000));	
};

inline void aiPutHotBug(int d)
{	
	aiHotBugData00 = d & 0xf000000f;
	aiHotBugData01 = d & 0x0f0000f0;
	aiHotBugData02 = d & 0x00f00f00;
	aiHotBugData03 = d & 0x000ff000;

	aiHotBugData00 ^= aiHotBugAdd03;
	aiHotBugData01 ^= aiHotBugAdd02;
	aiHotBugData02 ^= aiHotBugAdd01;
	aiHotBugData03 ^= aiHotBugAdd00;
};


#endif