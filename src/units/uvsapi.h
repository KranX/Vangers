#ifndef __UNITS__UVSAPI_H
#define __UNITS__UVSAPI_H

const int BUNCH_CHANGE_CYCLE  = 1;

const int UVS_DEVICE_POWER = 1000;
const int UVS_AMMO_POWER = 30;
const int UVS_TERMINATOR_POWER = 8;

struct FireOfTerror;
struct ActionUnit;
struct uvsVanger;
struct VangerUnit;
struct StuffObject;
struct SensorDataType;
struct PassageEngine;
struct EnterCenter;
struct uvsTarget;
struct uvsPassage;
struct uvsEscave;
struct uvsSpot;	 
struct uvsItem;
struct uvsDolly;
struct uvsFlyFarmer;
struct SimpleParticleType;
struct WorldBulletTemplate;

struct	uvsKernel
{
	uvsVanger* uvsPoint;
};

const int UNIT_ORDER_NONE = 0;
const int UNIT_ORDER_ACTION = 1;
const int UNIT_ORDER_STUFF = 2;
const int UNIT_ORDER_SENSOR = 3;
const int UNIT_ORDER_ENTER = 4;
const int UNIT_ORDER_PASSAGE = 5;
const int UNIT_ORDER_DOLL = 6;
const int UNIT_ORDER_FARMER = 7;
const int UNIT_ORDER_VECTOR = 8;
const int UNIT_ORDER_VANGER = 9;

union UnitOrderType
{
	ActionUnit* ActionT;
	StuffObject* StuffT;
	SensorDataType* SensorT;
	EnterCenter* EnterT;
	uvsDolly* DollT;
	PassageEngine* PassageT;
	uvsTarget* TargetT;
	struct{
		int x,y,z;
	}vT;
	int ID;
	VangerUnit* VangerT;

	UnitOrderType(void){ ActionT = NULL;};
	UnitOrderType(ActionUnit* p) { ActionT = p; };
	UnitOrderType(StuffObject* p) { StuffT = p; };
	UnitOrderType(SensorDataType* p) { SensorT = p; };
	UnitOrderType(EnterCenter* p) { EnterT = p; };
	UnitOrderType(uvsDolly* p) { DollT = p; };
	UnitOrderType(PassageEngine* p) { PassageT = p; };
	UnitOrderType(uvsTarget* p) { TargetT = p; };
	UnitOrderType(int x,int y,int z) { vT.x = x; vT.y = y; vT.z = z; };
	UnitOrderType(int id) { ID = id; };
	UnitOrderType(VangerUnit* p){ VangerT = p; };
};

VangerUnit* addVanger(uvsVanger* p,int x,int y,int Human = 0);
VangerUnit* addVanger(uvsVanger* p,uvsPassage* origin,int Human = 0);
VangerUnit* addVanger(uvsVanger* p,uvsEscave* origin,int Human = 0);
VangerUnit* addVanger(uvsVanger* p,uvsSpot* origin,int Human = 0);

//void addDevice(int x,int y,int z,uvsItem* uvsD,ActionUnit* Owner,int net_mode,int nid);
void BunchEvent(int type);
void ChangeWorld(int world,int flag = 1);

int uvsapiDestroyItem( int ind,int ind2);
int uvsSetItemType(int type,int param1,int param2);

void addFarmer(int x_pos,int y_pos,int x_speed,int y_speed,int corn_type,int corn,int time);
void ChangeItemData(int d);
void ChangeTabutaskItem(int type,int status);

void NetStatisticInit(void);
void NetStatisticUpdate(int id);
void NetStatisticGameStart(void);
extern int GeneralSystemSkip;

void CreateBoozeeniadaString(void);
int FuckOFFinWater(void);
char* StringOfBoozeeniada(void);

void addInfernalsKill2Protractor(void);
void addSpectorsKill2Messiah(void);

int getSpobsState(void);
int getThreallState(void);
int isSpummyDeath(void);

void sBunchCreate(int& id,int world,int data,int cycle);
void sBunchSend(int id,int data,int cycle);
void sBunchReceive(int id);

void OpenCrypt(int world,int id);
//id = [0 .. 1]
//world = [0 .. 1]

void NetworkQuant(void);
void CloseBunchPal(void);

int CheckThreallMessiah(void);
void ShowDominanceMessage(int d);
void ShowLuckMessage(int d);
void ShowTaskMessage(int l);
void aiPromptDominanceMessage(int d);
void aiPromptLuckMessage(int d);
void aiPromptTaskMessage(int l);
void NetworkGetStart(char* name,int& x,int& y);

void NetStatisticUpdate(int id);

const int NET_STATISTICS_KILL = 0;
const int NET_STATISTICS_DEATH = 1;
const int NET_STATISTICS_CHECKPOINT = 2;
const int NET_STATISTICS_END_RACE = 3;

#endif
