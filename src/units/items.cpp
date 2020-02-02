#include "../global.h"

//#include "..\win32f.h"

#include "../3d/3d_math.h"
#include "../3d/3dgraph.h"
#include "../3d/3dobject.h"
#include "../3d/parser.h"

#include "../common.h"
#include "../sqexp.h"
#include "../backg.h"

#include "../actint/item_api.h"
#include "uvsapi.h"
#include "../network.h"

#include "../terra/vmap.h"
#include "../terra/world.h"
#include "../terra/render.h"

#include "../uvs/univang.h"

#include "../particle/particle.h"
#include "../particle/df.h"
#include "../particle/partmap.h"
#include "../particle/light.h"

#include "../dast/poly3d.h"

#include "track.h"
#include "hobj.h"
#include "moveland.h"
#include "items.h"
#include "sensor.h"
#include "effect.h"
#include "mechos.h"

#include "../sound/hsound.h"
#include "magnum.h"


extern int RAM16;
extern iGameMap* curGMap;
extern uchar* FireColorTable;

const char DEBRIS_LIFE_TIME = 100;

//extern XStream MechosLst;
extern int AdvancedView;

const int MAX_STUFF_SCATTER = 150;

const int MAX_BULLET_ID = 8;
const int MAX_STORAGE_NAME = 2;

const char* BULLET_ID_NAME[MAX_BULLET_ID] = {
	"ChainGun",
	"Rocket",
	"FireBall",
	"Terminator",
	"JumpBall",
	"Laser",
	"Horde",
	"Hypnotise"
};

const int MAX_BULLET_TARGET_MODE_NAME = 6;
const char* BULLET_TARGET_MODE_NAME[MAX_BULLET_TARGET_MODE_NAME] = {
		"None",
		"Control",
		"LeftRound",
		"RightRound",
		"Random",
		"Wall"
};

const int MAX_BULLET_CONTROL_MODE_NAME = 9;
const char* BULLET_CONTROL_MODE_NAME[MAX_BULLET_CONTROL_MODE_NAME] = {
	"None",
	"Fly",
	"Target",
	"Aim",
	"Speed",
	"Impulse",
	"MapDestruction",
	"Untouch",
	"Physic"
};
const int BULLET_CONTROL_MODE_VALUE[MAX_BULLET_CONTROL_MODE_NAME] = {0,1,2,4,8,16,32,64,128};

const int MAX_BULLET_SHOW_ID_NAME = 7;
const char* BULLET_SHOW_ID_NAME[MAX_BULLET_SHOW_ID_NAME] = {
		"Particle",
		"FireBall",
		"Deform",
		"Dust",
		"Crater",
		"JumpBall",
		"Laser"
};

Vector GeneralMousePoint;
int SkyFarmerWayTableSize;
SensorDataType* SkyFarmerWayTable;

int WeaponWaitTime;

extern int ProtoCryptTableSize[WORLD_MAX];
extern SensorDataType* ProtoCryptTable[WORLD_MAX];

void ItemsDispatcher::Init(Parser& in)
{
	int i,j,k;
	char* name;
	Parser dp("device.lst");

	UnitList::Init(in);

	Total = 0;

	MaxUnit = new int[MAX_ITEMS_OBJECT];
	UnitStorage = new StorageType[MAX_ITEMS_OBJECT];

	in.search_name("NumItems");

	if(RAM16){
		for(i = 0;i < MAX_ITEMS_OBJECT;i++){
			MaxUnit[i] = in.get_int();
			MaxUnit[i] /= 2;
			if(MaxUnit[i] == 0) MaxUnit[i] = 1;
			Total += MaxUnit[i];
		};
	}else{
		for(i = 0;i < MAX_ITEMS_OBJECT;i++){
			MaxUnit[i] = in.get_int();
			Total += MaxUnit[i];
		};
	};

	in.search_name("UnitEnergy");
	UnitEnergy = in.get_int();
	in.search_name("UnitArmor");
	UnitArmor = in.get_int();
	in.search_name("UnitDeletaEnergy");
	UnitDeltaEnergy = in.get_int();

	UnitData = new GeneralObject*[Total];
	k = 0;

	for(i = 0;i < MAX_ITEMS_OBJECT;i++){
		UnitStorage[i].Init(MaxUnit[i]);
		for(j = 0;j < MaxUnit[i];j++){
			switch(i){
				case ITEM_GUN_DEVICE:
					UnitData[k] = new GunDevice;
					break;
				case ITEM_CHANGER:
					UnitData[k] = new ChangerDevice;
					break;
				case ITEM_OTHER:
					UnitData[k] = new StuffObject;
					break;
			};
			UnitData[k]->Init(&UnitStorage[i]);
			UnitStorage[i].Deactive(UnitData[k]);
			k++;
		};
	};

	dp.search_name("NumDeviceType");
	NumDeviceType = dp.get_int();
	DeviceTypeData = new StuffObject*[NumDeviceType];

	StuffItemLink = new StuffObject*[MAX_ITEM_TYPE];
	for(j =0;j < MAX_ITEM_TYPE;j++) StuffItemLink[j] = (StuffObject*)0x83838383;

	for(i = 0;i < NumDeviceType;i++){
		dp.search_name("DeviceID");
		name = dp.get_name();
		if(!strcmp("Gun",name)){
			DeviceTypeData[i] = new GunDevice;
			DeviceTypeData[i]->StuffType = DEVICE_ID_GUN;
		}else{
			if(!strcmp("Changer",name)){
				DeviceTypeData[i] = new ChangerDevice;
				DeviceTypeData[i]->StuffType = DEVICE_ID_CHANGER;
			}else{
				if(!strcmp("Other",name)){
					DeviceTypeData[i] = new StuffObject;
					DeviceTypeData[i]->StuffType = DEVICE_ID_OTHER;
				};
			};
		};

		DeviceTypeData[i]->DataID = i;

		dp.search_name("UvsDeviceName");
		name = dp.get_name();
		for(j = 0;j < MAX_ITEM_TYPE;j++){
			if(!strcmp(uvsItemTable[j]->name,name)){
				StuffItemLink[j] = DeviceTypeData[i];
				DeviceTypeData[i]->uvsDeviceType = j;
				DeviceTypeData[i]->ActIntBuffer.type = uvsItemTable[j]->SteelerTypeEmpty;
				break;
			};
		};

		if(j == MAX_ITEM_TYPE){
			ErrH.Abort("Bad Item ID");
			StuffItemLink[0] = DeviceTypeData[i];
			DeviceTypeData[i]->uvsDeviceType = 0;
			DeviceTypeData[i]->ActIntBuffer.type = uvsItemTable[0]->SteelerTypeEmpty;
		};		
		DeviceTypeData[i]->DeviceLoad(dp);
	};
};

void ItemsDispatcher::Free(void)
{
	int i;	
	for(i = 0;i < NumDeviceType;i++)
		delete DeviceTypeData[i];
	delete[] DeviceTypeData;
	delete[] StuffItemLink;

	for(i = 0;i < Total;i++){
		UnitData[i]->Free();
		delete UnitData[i];
	};

	for(i = 0;i < MAX_ITEMS_OBJECT;i++) UnitStorage[i].Free();

	delete[] UnitData;
	delete[] MaxUnit;
	delete[] UnitStorage;
};

const int ITEM_LUCK_RADIUS = 20;
void ItemsDispatcher::Open(Parser& in)
{
	listElem* p;
	uvsItem* n;
	int i,j;
	UnitList::Open(in);

	for(i = 0;i < Total;i++) UnitData[i]->Open();
	GeneralMousePoint = Vector(83,0,0);

	NumVisibleItem = Total;
	if(!NetworkON){
		for(i = 0;i < ProtoCryptTableSize[CurrentWorld];i++){
			if(ProtoCryptTable[CurrentWorld][i].Enable){
				if(ProtoCryptTable[CurrentWorld][i].SensorType == 2 || ProtoCryptTable[CurrentWorld][i].SensorType == 6){
					for(j = 0;j < ITEM_LUCK_MAX;j++){
						if((int)(RND(100)) <= aiCutLuck){
							if(!RND(8))
								uvsCreateNewItem(ProtoCryptTable[CurrentWorld][i].R_curr.x + (ITEM_LUCK_RADIUS*CO[RND(PI*2)] >> 16),ProtoCryptTable[CurrentWorld][i].R_curr.y + (ITEM_LUCK_RADIUS*SI[RND(PI*2)] >> 16),ProtoCryptTable[CurrentWorld][i].R_curr.z,uvsGenerateItemForCrypt(6),CurrentWorld);
							else 
								uvsCreateNewItem(ProtoCryptTable[CurrentWorld][i].R_curr.x + (ITEM_LUCK_RADIUS*CO[RND(PI*2)] >> 16),ProtoCryptTable[CurrentWorld][i].R_curr.y + (ITEM_LUCK_RADIUS*SI[RND(PI*2)] >> 16),ProtoCryptTable[CurrentWorld][i].R_curr.z,uvsGenerateItemForCrypt(2),CurrentWorld);
						};
					};
				}else{
					if(ProtoCryptTable[CurrentWorld][i].SensorType == 1 || ProtoCryptTable[CurrentWorld][i].SensorType == 7)
						uvsCreateNewItem(ProtoCryptTable[CurrentWorld][i].R_curr.x,ProtoCryptTable[CurrentWorld][i].R_curr.y,ProtoCryptTable[CurrentWorld][i].R_curr.z,uvsGenerateItemForCrypt(ProtoCryptTable[CurrentWorld][i].SensorType),CurrentWorld);
					else 
						uvsCreateNewItem(ProtoCryptTable[CurrentWorld][i].R_curr.x,ProtoCryptTable[CurrentWorld][i].R_curr.y,ProtoCryptTable[CurrentWorld][i].R_curr.z,ProtoCryptTable[CurrentWorld][i].z0,CurrentWorld);
				};
			};
		};

		p = WorldTable[GameD.cWorld]->Pitem;
		while(p){
			n = (uvsItem*)(p);			
			addDevice(n->pos_x,n->pos_y,n->pos_z,n->type,n->param1,n->param2,NULL);
			p = p->next;
		};
		FreeList(WorldTable[CurrentWorld]->Pitem);
	};
};

void ItemsDispatcher::LarvaHilator(void)
{
	StuffObject* p;	
	p = (StuffObject*)Tail;
	while(p){		
		if(p->ActIntBuffer.type == ACI_CONLARVER){
			uvsFreeDolly(p->R_curr.x,p->R_curr.y,p->ActIntBuffer.data0);
			p->ActIntBuffer.data1 = p->ActIntBuffer.data0 = 0;					
			p->ActIntBuffer.type = uvsSetItemType(p->uvsDeviceType,p->ActIntBuffer.data0,p->ActIntBuffer.data1);
		};
		p = (StuffObject*)(p->NextTypeList);
	};
};


void ItemsDispatcher::Close(void)
{
	int i;	
	StuffObject* p;
	StuffObject* pp;
	uvsItem* g;

	p = (StuffObject*)Tail;
	FreeList(WorldTable[GameD.cWorld]->Pitem);

	if(NetworkON){
		while(p){
			pp = (StuffObject*)(p->NextTypeList);
			DeleteItem(p);
			p = pp;
		};
	}else{
		for(i = 0;i < ProtoCryptTableSize[CurrentWorld];i++){
			if(ProtoCryptTable[CurrentWorld][i].SensorType == 3 || ProtoCryptTable[CurrentWorld][i].SensorType == 5 || (ProtoCryptTable[CurrentWorld][i].SensorType == 4 && ProtoCryptTable[CurrentWorld][i].z0 == UVS_ITEM_TYPE::SPUMMY)){
				ProtoCryptTable[CurrentWorld][i].Enable = 0;
			};
		};

		while(p){
			pp = (StuffObject*)(p->NextTypeList);
			if(p->ActIntBuffer.type == ACI_CONLARVER){
				uvsFreeDolly(p->R_curr.x,p->R_curr.y,p->ActIntBuffer.data0);
				p->ActIntBuffer.data1 = p->ActIntBuffer.data0 = 0;					
				p->ActIntBuffer.type = uvsSetItemType(p->uvsDeviceType,p->ActIntBuffer.data0,p->ActIntBuffer.data1);

				g = new uvsItem(p->uvsDeviceType);
				g->param1 = p->ActIntBuffer.data0;
				g->param2 = p->ActIntBuffer.data1;
				g->pos_x = p->R_curr.x;
				g->pos_y = p->R_curr.y;
				g->pos_z = ITEM_DEFAULT_Z;
				g -> link(WorldTable[CurrentWorld]->Pitem);
//				uvsKronDeleteItem(p->uvsDeviceType,p->ActIntBuffer.data0,p->ActIntBuffer.data1);
			}else{
				if(p->CreateMode != STUFF_CREATE_RELAX){
					if(((int)(RND(100)) < aiCutLuck || p->Hour == ConTimer.hour || p->ActIntBuffer.type == ACI_TABUTASK_SUCCESSFUL || p->ActIntBuffer.type == ACI_TABUTASK || CurrentWorld >= MAIN_WORLD_MAX) && 
					   (p->ActIntBuffer.type != ACI_ZEEX && p->ActIntBuffer.type != ACI_BEEBOORAT && p->ActIntBuffer.type != ACI_ELEEPOD && p->ActIntBuffer.type != ACI_TABUTASK_FAILED && p->ActIntBuffer.type != ACI_BOORAWCHIK)){
						g = new uvsItem(p->uvsDeviceType);
						g->param1 = p->ActIntBuffer.data0;
						g->param2 = p->ActIntBuffer.data1;
						g->pos_x = p->R_curr.x;
						g->pos_y = p->R_curr.y;
						g->pos_z = ITEM_DEFAULT_Z;
						g -> link(WorldTable[CurrentWorld]->Pitem);
					}else uvsKronDeleteItem(p->uvsDeviceType,p->ActIntBuffer.data0,p->ActIntBuffer.data1);
				}else{
					for(i = 0;i < ProtoCryptTableSize[CurrentWorld];i++){
						if((ProtoCryptTable[CurrentWorld][i].SensorType == 3 || ProtoCryptTable[CurrentWorld][i].SensorType == 5 || (ProtoCryptTable[CurrentWorld][i].SensorType == 4 && ProtoCryptTable[CurrentWorld][i].z0 == UVS_ITEM_TYPE::SPUMMY)) && ProtoCryptTable[CurrentWorld][i].z0 == p->uvsDeviceType){
							if(getDistY(ProtoCryptTable[CurrentWorld][i].R_curr.y,p->R_curr.y) < p->radius && getDistX(ProtoCryptTable[CurrentWorld][i].R_curr.x,p->R_curr.x) < p->radius)
								ProtoCryptTable[CurrentWorld][i].Enable = 1;
						};					
					};				
				};
			};
			
			DeleteItem(p);
			p = pp;
		};

		if(ConTimer.day >= 16){
			if(!RND(3) && !aciWorldLinkExist(WORLD_FOSTRAL,WORLD_WEEXOW,0) && !aciWorldLinkExist(WORLD_GLORX,WORLD_KHOX,0)
			   && !aciWorldLinkExist(WORLD_NECROSS,WORLD_BOOZEENA,0) && !aciWorldLinkExist(WORLD_NECROSS,WORLD_ARKONOY,0))
				Item2ShopAction(5);

			if(!RND(3))
				Item2ShopAction(3);
		};
	};

	for(i = 0;i < Total;i++) UnitData[i]->Close();
	for(i = 0;i < MAX_ITEMS_OBJECT;i++) UnitStorage[i].Check();	
};


void Item2ShopAction(int type)
{
	int i,j;
	for(i = 0;i < WORLD_MAX;i++){
		for(j = 0;j < ProtoCryptTableSize[i];j++){
			if(ProtoCryptTable[i][j].SensorType == type && ProtoCryptTable[i][j].Enable){
				uvsKronDeleteItem(ProtoCryptTable[i][j].z0,0,0);
				ProtoCryptTable[i][j].Enable = 0;
				return;
			};			
		};
	};
};

void ItemsDispatcher::Quant(void)
{
	GeneralObject* n;
	StuffObject* p;
	StuffObject* np;
	StuffObject* pp;
	VangerUnit* v;	

	
	if(NetworkON) CryptQuant();

	NumVisibleItem = 0;
	n = Tail;
	while(n){
		n->Quant();
		n = n->NextTypeList;
	};	

	p = (StuffObject*)Tail;
	while(p){
		np = (StuffObject*)(p->NextTypeList);
		if(p->Status & SOBJ_DISCONNECT)	DeleteItem(p);
		else{
			if(p->Status & SOBJ_LINK){
				p->Status &= ~SOBJ_LINK;
				if(p->Status & SOBJ_WAIT_CONFIRMATION)
					p->Owner = NULL;
				else{
					v = (VangerUnit*)(p->Owner);
					switch(v->CheckInDevice(p)){
						case CHECK_DEVICE_IN:
							p->DeviceIn();
							PUT_GLOBAL_EVENT(AI_EVENT_CAPTURE,p->ID,p,v);
							if(v->Status & SOBJ_ACTIVE){
								p->ActIntBuffer.slot = -1;
								aciSendEvent2actint(ACI_PUT_ITEM,&(p->ActIntBuffer));
								if(ActD.Active && v == ActD.Active)
									SOUND_TAKE_ITEM(getDistX(ActD.Active->R_curr.x,p->R_curr.x))
							};						
							break;
						case CHECK_DEVICE_ADD:
							ObjectDestroy(p);
							pp = v->DeviceData;
							while(pp){
								if(pp->uvsDeviceType == p->uvsDeviceType){
									pp->ActIntBuffer.data1+= p->ActIntBuffer.data1;
									break;
								};
								pp = pp->NextDeviceList;
							};
							DeleteItem(p);
							if(ActD.Active && v == ActD.Active)
								SOUND_TAKE_ITEM(getDistX(ActD.Active->R_curr.x,p->R_curr.x))						
							break;
						case CHECK_DEVICE_OUT:
							p->Owner = NULL;
							break;
					};
				};
			};
		};
		p = np;
	};
};

void ItemsDispatcher::DeleteItem(StuffObject* p)
{
	if(p->LightData){
		p->LightData->Destroy();
		p->LightData = NULL;
	};

	GameD.DisconnectBaseList(p);
	DisconnectTypeList(p);
	if(p->Storage) p->Storage->Deactive(p);
};

void DebrisObject::Init(void)
{
	BaseObject::Init();
	ID = ID_DEBRIS;
	Status = SOBJ_DISCONNECT;
};

void DebrisObject::CreateDebris(int id,int ind)
{
	Status = 0;
	cycleTor(R_curr.x,R_curr.y);
	GetVisible();
	Time = 0;
	set_active(0);

	ModelID = id;
	ModelIndex = ind;
};

void DebrisObject::Quant(void)
{
	int t;
	t = Visibility;
	GetVisible();
	Time++;
	if(Visibility == VISIBLE){
		R_prev = R_curr;
		analysis();
		cycleTor(R_curr.x,R_curr.y);
	}else{
		if(Time > DEBRIS_LIFE_TIME)
			Status |= SOBJ_DISCONNECT;			
	};
};

void DebrisObject::DrawQuant(void)
{	
	draw();
};

void StuffObject::Init(StorageType* s)
{
	BaseObject::Init(s);

	ID = ID_STUFF;
	Status = SOBJ_DISCONNECT;
	Owner = NULL;
};

void aciPrepareWorldsMenu(void);
extern dastPoly3D DollyPoint;
void StuffObject::Quant(void)
{
	int lv;
	int update_log = 0;

	if(NetworkON && (Status & SOBJ_WAIT_CONFIRMATION)) NetOwnerQuant();
	lv = Visibility;
	GetVisible();	

	if(Status & SOBJ_WAIT_CONFIRMATION) return;

	if(ActIntBuffer.type == ACI_CONLARVER && Hour != ConTimer.hour/* && Minutes == ConTimer.min*/){
		uvsFreeDolly(R_curr.x,R_curr.y,ActIntBuffer.data0);
		ActIntBuffer.data1 = ActIntBuffer.data0 = 0;					
		ActIntBuffer.type = uvsSetItemType(uvsDeviceType,ActIntBuffer.data0,ActIntBuffer.data1);
		FindDolly = 0;
		SetStuffColor();

		if(LightData){
			LightData->Destroy();
			LightData = NULL;
		};
	};

	if(Visibility == VISIBLE){
		ItemD.NumVisibleItem++;
		switch(ActIntBuffer.type){
			case ACI_CONLARVER:
				if(LightData) LightData->set_position(R_curr.x,R_curr.y,512);
				else LightData = MapD.CreateLight(R_curr.x,R_curr.y,512,radius*2,63,LIGHT_TYPE::STATIC);
				break;
			case ACI_EMPTY_CONLARVER:
				if(LightData) LightData->set_position(R_curr.x,R_curr.y,512);
				else LightData = MapD.CreateLight(R_curr.x,R_curr.y,512,radius,63,LIGHT_TYPE::DYNAMIC);
				break;
			case ACI_MECHANIC_MESSIAH:
				if(CurrentWorld == WORLD_XPLO && ActD.FunctionSpobsDestroyActive){
					if(!ActD.SpobsDestroy && (dynamic_state & GROUND_COLLISION) && GetTouchSensor("Spot1")){
						ActD.SpobsDestroy = 1;
						PalCD.Set(CPAL_SPOBS_TO,50);
						Status |= SOBJ_DISCONNECT;
						ActD.XploKeyEnable = 0;
						aciOpenWorldLink(WORLD_HMOK,WORLD_HMOK);
						aciPrepareWorldsMenu();
						start_vibration();
						ActD.FunctionSpobsDestroyActive = 0;
						SOUND_SPOPS_ALIES();
					};
				};
				break;
			case ACI_PROTRACTOR:
				if(CurrentWorld == WORLD_THREALL){
					if(ActD.ThreallDestroy){
						if(LightData) LightData->set_position(R_curr.x,R_curr.y,512);
						else LightData = MapD.CreateLight(R_curr.x,R_curr.y,512,80,63,LIGHT_TYPE::DYNAMIC);
						if(R_curr.z < 255) impulse(Vector(0,0,64),RND(15),RND(8));
					}else{
						if((dynamic_state & GROUND_COLLISION) && GetTouchSensor("SIGN") && ActD.FunctionThreallDestroyActive){
							ActD.ThreallDestroy = 1;
							aciOpenWorldLink(WORLD_HMOK,WORLD_HMOK);
							aciPrepareWorldsMenu();
							PalCD.Set(CPAL_THREALL_TO,50);
							start_vibration();
							SOUND_SPOPS_ALIES();
							ActD.FunctionThreallDestroyActive = 0;
						};
					};
				};
				break;
			case ACI_ELEEPOD:
			case ACI_BEEBOORAT:
			case ACI_ZEEX:
			case ACI_BOORAWCHIK:
				if(device_modulation > 0 && (dynamic_state & GROUND_COLLISION)){
					DollyPoint.set(R_curr,15,20);
					EffD.CreateDeform(R_curr,DEFORM_ALL,PASSING_WAVE_PROCESS);
					device_modulation = 0;
				};
				break;				 
			default:
				if((ActIntBuffer.type == ACI_ROTTEN_KERNOBOO || ActIntBuffer.type == ACI_ROTTEN_PIPETKA || ActIntBuffer.type == ACI_ROTTEN_WEEZYK || ActIntBuffer.type == ACI_DEAD_ELEECH) && (dynamic_state & GROUND_COLLISION))
					Status |= SOBJ_DISCONNECT;
				else{
					if(LightData){
						LightData->Destroy();
						LightData = NULL;
					};
				};
				break;
		};

		if(lv == UNVISIBLE){
			if(CreateMode == STUFF_CREATE_RELAX)
				set_3D(SET_3D_DIRECT_PLACE,R_curr.x,R_curr.y,R_curr.z,0,0,0);
			else{
				if(uvsDeviceType == UVS_ITEM_TYPE::KERNOBOO || uvsDeviceType == UVS_ITEM_TYPE::PIPETKA)
					set_3D(SET_3D_TO_THE_UPPER_LEVEL,R_curr.x,R_curr.y,0,0,0,0);
				else
					set_3D(SET_3D_CHOOSE_LEVEL,R_curr.x,R_curr.y,R_curr.z,0,0,0);
			};
		};

		analysis();
		cycleTor(R_curr.x,R_curr.y);
	}else{
		switch(ActIntBuffer.type){
			case ACI_ELEEPOD:
			case ACI_BEEBOORAT:
			case ACI_ZEEX:
			case ACI_BOORAWCHIK:
				uvsKronDeleteItem(uvsDeviceType,ActIntBuffer.data0,ActIntBuffer.data1);
				Status |= SOBJ_DISCONNECT;
				break;
			case ACI_ROTTEN_KERNOBOO:
			case ACI_ROTTEN_PIPETKA:
			case ACI_ROTTEN_WEEZYK:
			case ACI_DEAD_ELEECH:
				Status |= SOBJ_DISCONNECT;
				break;
		};

		if(LightData){
			LightData->Destroy();
			LightData = NULL;
		};
	};
};

int StuffObject::GetTouchSensor(const char* name)
{
	int dx,dy,l;
	StaticObject* st;
	Vector vCheck;

	st = FindSensor(name);
	if(st){
		if(R_curr.z > ((SensorDataType*)(st))->z0 - radius && R_curr.z  < ((SensorDataType*)(st))->z1 + radius){
			dx = getDistX(R_curr.x,st->R_curr.x);			
			dy = getDistY(R_curr.y,st->R_curr.y);
			l = radius + st->radius;
			if((dx*dx + dy*dy) < l*l)
				return 1;
		};
	};
	return 0;
};

void StuffObject::DrawQuant(void)
{
	if(Status & SOBJ_WAIT_CONFIRMATION) return;
	if(ActIntBuffer.type != ACI_CONLARVER && ActIntBuffer.type != ACI_EMPTY_CONLARVER){
		CycleTime = rPI(CycleTime + PI / 12);
		scale_size = original_scale_size + original_scale_size*Sin(CycleTime) / 8.;
	};
	draw();
};

void StuffObject::CreateStuff(const Vector& _v,StuffObject* p,int cMode)
{
	GetDevice(p);
	ActIntBuffer.type = uvsSetItemType(uvsDeviceType,ActIntBuffer.data0,ActIntBuffer.data1);
	Object::operator = (ModelD.ActiveModel(ModelID));
	Owner = NULL;
	ItemD.ConnectTypeList(this);
	GameD.ConnectBaseList(this);
	R_curr = _v;
	Status = 0;
	cycleTor(R_curr.x,R_curr.y);
	set_active(0);	
	GetVisible();
	switch_analysis(0);
	CreateMode = cMode;
	CycleTime = 0;

	LightData = NULL;
	Hour = ConTimer.hour;
	Minutes = ConTimer.min;
	Time = 0;
	FindDolly = 0;

	switch(CreateMode){
		case STUFF_CREATE_TRACK:
			if(Visibility == VISIBLE){
				set_3D(SET_3D_DIRECT_PLACE,R_curr.x,R_curr.y,R_curr.z,0,0,0);
				xImpulse = XCYCL(R_curr.x + MAX_STUFF_SCATTER - RND(2*MAX_STUFF_SCATTER));
				yImpulse = YCYCL(R_curr.y + MAX_STUFF_SCATTER - RND(2*MAX_STUFF_SCATTER));
				precise_impulse(R_curr,xImpulse,yImpulse);
			}else{
				R_curr.x = XCYCL(R_curr.x + MAX_STUFF_SCATTER - RND(2*MAX_STUFF_SCATTER));
				R_curr.y = YCYCL(R_curr.y + MAX_STUFF_SCATTER - RND(2*MAX_STUFF_SCATTER));
				xImpulse = R_curr.x;
				yImpulse = R_curr.y;
			};
			break;
		case STUFF_CREATE_RELAX:
			switch_analysis(1);
			set_3D(SET_3D_DIRECT_PLACE,R_curr.x,R_curr.y,R_curr.z,0,0,0);
			xImpulse = R_curr.x;
			yImpulse = R_curr.y;
			break;
		case STUFF_CREATE_NONE:
			set_3D(SET_3D_TO_THE_LOWER_LEVEL,R_curr.x,R_curr.y,0,radius/2,0,0);
			xImpulse = R_curr.x;
			yImpulse = R_curr.y;
			break;
		default:
			xImpulse = R_curr.x;
			yImpulse = R_curr.y;
			break;
	};	

	SetStuffColor();

	switch(ActIntBuffer.type){
		case  ACI_KERNOBOO:
		case ACI_PIPETKA:			
			archimedean = 255;
			break;
		case ACI_ELEEPOD:
		case ACI_BEEBOORAT:
		case ACI_ZEEX:
		case ACI_BOORAWCHIK:
			device_modulation = 1;
			break;
		case ACI_PROTRACTOR:
		case ACI_MECHANIC_MESSIAH:
		case ACI_FUNCTION83:
		case ACI_SPUMMY:			
			CreateArtefactTarget(this);
			break;
		default:
			archimedean = 0;
			break;
	};

	OutFlag = 0;
	NetOwner = 0;
	if(NetworkON){
		NetID = CREATE_STUFF_NET_ID(NID_STUFF);
		NetDeviceID = CREATE_STUFF_NET_ID(NID_DEVICE);
		NETWORK_OUT_STREAM.create_permanent_object(NetID,xImpulse,yImpulse,radius);
		NETWORK_OUT_STREAM < (uchar)(DataID);
		NETWORK_OUT_STREAM < (int)(NetDeviceID);
		NETWORK_OUT_STREAM < (short)(R_curr.z);
		NETWORK_OUT_STREAM < (uchar)(ActIntBuffer.type);
		NETWORK_OUT_STREAM < (int)(ActIntBuffer.data0);
		NETWORK_OUT_STREAM < (int)(ActIntBuffer.data1);
		NETWORK_OUT_STREAM < (uchar)(CreateMode | (OutFlag));
		NETWORK_OUT_STREAM < (short)(Time);
		NETWORK_OUT_STREAM < (short)(R_curr.x);
		NETWORK_OUT_STREAM < (short)(R_curr.y);
		NETWORK_OUT_STREAM < (int)(NetOwner);
		NETWORK_OUT_STREAM.end_body();		
	};
};

void StuffObject::CreateDevice(Vector v,VangerUnit* own,StuffObject* p)
{
	GetDevice(p);
	ActIntBuffer.type = uvsSetItemType(uvsDeviceType,ActIntBuffer.data0,ActIntBuffer.data1);
	own->AddDevice(this);
	Object::operator = (ModelD.ActiveModel(ModelID));
	Owner = own;
	Status = 0;
	R_curr = v;
//	set_3D(R_curr.x,R_curr.y,0,0);
	set_active(0);
	LightData = NULL;

	OutFlag = 0;
	xImpulse = R_curr.x;
	yImpulse = R_curr.y;
	NetOwner = Owner->NetID;

	Time = 0;
	FindDolly = 0;

	if(NetworkON && (Owner->Status & SOBJ_ACTIVE)){
		NetID = CREATE_STUFF_NET_ID(NID_STUFF);
		NetDeviceID = CREATE_STUFF_NET_ID(NID_DEVICE);
		NETWORK_OUT_STREAM.create_permanent_object(NetDeviceID,xImpulse,yImpulse,radius);
		NETWORK_OUT_STREAM < (uchar)(DataID);
		NETWORK_OUT_STREAM < (int)(NetID);
		NETWORK_OUT_STREAM < (short)(R_curr.z);
		NETWORK_OUT_STREAM < (uchar)(ActIntBuffer.type);
		NETWORK_OUT_STREAM < (int)(ActIntBuffer.data0);
		NETWORK_OUT_STREAM < (int)(ActIntBuffer.data1);
		NETWORK_OUT_STREAM < (uchar)(CreateMode | (OutFlag));
		NETWORK_OUT_STREAM < (short)(Time);
		NETWORK_OUT_STREAM < (short)(R_curr.x);
		NETWORK_OUT_STREAM < (short)(R_curr.y);
		NETWORK_OUT_STREAM < (int)(NetOwner);
		NETWORK_OUT_STREAM.end_body();		
	};
};

void StuffObject::DeviceIn(void)
{
	GameD.DisconnectBaseList(this);
	ItemD.DisconnectTypeList(this);
	Owner->AddDevice(this);
	Status = 0;

	if(LightData){
		LightData->Destroy();
		LightData = NULL;
	};

	FindDolly = 0;

	NetOwner = Owner->NetID;

	switch(ActIntBuffer.type){
		case ACI_PROTRACTOR:
		case ACI_MECHANIC_MESSIAH:
		case ACI_FUNCTION83:
		case ACI_SPUMMY:
			DeleteArtefactTarget(this);
			break;
//		case ACI_MECHOSCOPE:			
//			break;
	};

	if(NetworkON && (Owner->Status & SOBJ_ACTIVE)){
		NETWORK_OUT_STREAM.delete_object(NetID);
		NETWORK_OUT_STREAM < (uchar)(1);
		NETWORK_OUT_STREAM.end_body();

		NETWORK_OUT_STREAM.create_permanent_object(NetDeviceID,xImpulse,yImpulse,radius);
		NETWORK_OUT_STREAM < (uchar)(DataID);
		NETWORK_OUT_STREAM < (int)(NetID);
		NETWORK_OUT_STREAM < (short)(R_curr.z);
		NETWORK_OUT_STREAM < (uchar)(ActIntBuffer.type);
		NETWORK_OUT_STREAM < (int)(ActIntBuffer.data0);
		NETWORK_OUT_STREAM < (int)(ActIntBuffer.data1);
		NETWORK_OUT_STREAM < (uchar)(CreateMode | (OutFlag));
		NETWORK_OUT_STREAM < (short)(Time);
		NETWORK_OUT_STREAM < (short)(R_curr.x);
		NETWORK_OUT_STREAM < (short)(R_curr.y);
		NETWORK_OUT_STREAM < (int)(NetOwner);
		NETWORK_OUT_STREAM.end_body();
	};
};

void GunDevice::DeviceIn(void)
{
	StuffObject::DeviceIn();
	if(pData->BulletID == BULLET_TYPE_ID::HYPNOTISE){
		switch(ActIntBuffer.type){
			case ACI_AMPUTATOR:
				EffD.CreateDeform(Owner->R_curr,DEFORM_ALL,PASSING_WAVE_PROCESS);
				if(ActD.Active)
					SOUND_AMPUTATOR_SHOT(getDistX(ActD.Active->R_curr.x,Owner->R_curr.x));
				break;
			case ACI_DEGRADATOR:
				EffD.CreateDeform(Owner->R_curr,DEFORM_ALL,PASSING_WAVE_PROCESS);
				if(ActD.Active)
					SOUND_DEGRADATOR_SHOT(getDistX(ActD.Active->R_curr.x,Owner->R_curr.x));
				break;
			case ACI_MECHOSCOPE:
				if(NetworkON){
					if(PrevOwner && PrevOwner != Owner && (Owner->Status & SOBJ_ACTIVE) && PrevOwner->Visibility == VISIBLE && !(PrevOwner->Status & SOBJ_WAIT_CONFIRMATION)){
						if(!(Owner->VangerChanger) && !(PrevOwner->VangerChanger)){
							Owner->NetChanger = GET_STATION(PrevOwner->NetID);
							Owner->ShellUpdateFlag = 1;

							Owner->VangerChanger = PrevOwner;
							Owner->MechosChangerType = PrevOwner->uvsPoint->Pmechos->type;
							Owner->VangerChangerCount = 3;
							Owner->vChangerPosition = PrevOwner->R_curr;
							Owner->VangerChangerAngle = PrevOwner->Angle;
							Owner->VangerChangerArmor = PrevOwner->Armor;
							Owner->VangerChangerEnergy = PrevOwner->Energy;
						};
					};
				}else{
					if(PrevOwner && PrevOwner != Owner && PrevOwner->Visibility == VISIBLE && Owner->Visibility == VISIBLE && ((PrevOwner->Status & SOBJ_ACTIVE) || (Owner->Status & SOBJ_ACTIVE))){
						if(ActD.Active)
							SOUND_INCARNATOR_SHOT(getDistX(ActD.Active->R_curr.x,Owner->R_curr.x));
						Owner->VangerChanger = PrevOwner;
						Owner->MechosChangerType = PrevOwner->uvsPoint->Pmechos->type;
						Owner->VangerChangerCount = 3;
						Owner->vChangerPosition = PrevOwner->R_curr;
						Owner->VangerChangerAngle = PrevOwner->Angle;
						Owner->VangerChangerArmor = PrevOwner->Armor;
						Owner->VangerChangerEnergy = PrevOwner->Energy;
						Owner->VangerChangerColor = PrevOwner->uvsPoint->Pmechos->color;

						PrevOwner->VangerChanger = Owner;
						PrevOwner->MechosChangerType = Owner->uvsPoint->Pmechos->type;
						PrevOwner->VangerChangerCount = 3;
						PrevOwner->vChangerPosition = Owner->R_curr;
						PrevOwner->VangerChangerAngle = Owner->Angle;
						PrevOwner->VangerChangerArmor = Owner->Armor;
						PrevOwner->VangerChangerEnergy = Owner->Energy;
						PrevOwner->VangerChangerColor = Owner->uvsPoint->Pmechos->color;
					};
				};

				ActIntBuffer.data1 = 0;
				ActIntBuffer.type = uvsSetItemType(uvsDeviceType,ActIntBuffer.data0,ActIntBuffer.data1);
				set_body_color(COLORS_IDS::BODY_GRAY);
				break;
		};
		PrevOwner = NULL;
	};
};

void GunDevice::Quant(void)
{
	if(Time > 0){
		Time--;
		if(Time == 0 && (ActIntBuffer.type == ACI_AMPUTATOR || ActIntBuffer.type == ACI_DEGRADATOR || ActIntBuffer.type == ACI_MECHOSCOPE)){
			ActIntBuffer.data1 = 0;
			ActIntBuffer.type = uvsSetItemType(uvsDeviceType,ActIntBuffer.data0,ActIntBuffer.data1);
			set_body_color(COLORS_IDS::BODY_GRAY);
		}else{
			if(ActIntBuffer.type == ACI_EMPTY_AMPUTATOR || ActIntBuffer.type == ACI_EMPTY_DEGRADATOR || ActIntBuffer.type == ACI_EMPTY_MECHOSCOPE){
				ActIntBuffer.data1 = 1;
				ActIntBuffer.type = uvsSetItemType(uvsDeviceType,ActIntBuffer.data0,ActIntBuffer.data1);
				set_body_color(COLORS_IDS::YELLOW_CHARGED);
			};
		};
	};
	StuffObject::Quant();	
};	

void GunDevice::DeviceQuant(void)
{
	if(pData->BulletID == BULLET_TYPE_ID::HYPNOTISE){
		switch(ActIntBuffer.type){
			case ACI_AMPUTATOR:
//				Owner->disable_control = 1;
				Owner->traction = 0;
				Time--;
				if(Time <= 0){
					EffD.CreateDeform(Owner->R_curr,DEFORM_ALL,PASSING_WAVE_PROCESS);
					Owner->disable_control = 0;
					ActIntBuffer.data1 = 0;
					ActIntBuffer.type = uvsSetItemType(uvsDeviceType,ActIntBuffer.data0,ActIntBuffer.data1);
					set_body_color(COLORS_IDS::BODY_GRAY);
					if(Owner->Status & SOBJ_ACTIVE)
						aciChangeItem(&ActIntBuffer);
				};
				break;
			case ACI_DEGRADATOR:
				Owner->scale_size = Owner->original_scale_size / 2;
				Time--;
				if(Time <= 0){
					EffD.CreateDeform(Owner->R_curr,DEFORM_ALL,PASSING_WAVE_PROCESS);
					Owner->scale_size = Owner->original_scale_size;
					ActIntBuffer.data1 = 0;
					ActIntBuffer.type = uvsSetItemType(uvsDeviceType,ActIntBuffer.data0,ActIntBuffer.data1);
					set_body_color(COLORS_IDS::BODY_GRAY);
					if(Owner->Status & SOBJ_ACTIVE)
						aciChangeItem(&ActIntBuffer);
				};
				break;
		};
	};
};

void GunDevice::DeviceOut(Vector v1,int flag,Vector v2)
{
	if(Time){
		switch(ActIntBuffer.type){
			case ACI_AMPUTATOR:
//				Owner->disable_control = 0;
				EffD.CreateDeform(Owner->R_curr,DEFORM_ALL,PASSING_WAVE_PROCESS);
				break;
			case ACI_DEGRADATOR:
				Owner->scale_size = Owner->original_scale_size;				
				EffD.CreateDeform(Owner->R_curr,DEFORM_ALL,PASSING_WAVE_PROCESS);
				break;
		};
	};
	PrevOwner = Owner;
	StuffObject::DeviceOut(v1,flag,v2);
};

const int DEVICE_OUT_NORMAL = 70;

void StuffObject::ImpulseAction(Vector v1,int flag,Vector v2)
{	
	R_curr = v1;
	cycleTor(R_curr.x,R_curr.y);
	GetVisible();

	CycleTime = 0;
	Hour = ConTimer.hour;
	Minutes = ConTimer.min;

	if(CreateMode == STUFF_CREATE_RELAX) CreateMode = STUFF_CREATE_EXRELAX;
	else CreateMode = STUFF_CREATE_NONE;

	switch_analysis(0);
	SetStuffColor();

	switch(ActIntBuffer.type){
		case  ACI_KERNOBOO:
		case ACI_PIPETKA:			
			archimedean = 255;
			break;
		case ACI_ELEEPOD:
		case ACI_BEEBOORAT:
		case ACI_ZEEX:
		case ACI_BOORAWCHIK:
			device_modulation = 1;
			break;
		default:
			archimedean = 0;
			break;
	};

	if(flag){
		if(ActIntBuffer.type == ACI_EMPTY_CONLARVER){
			FindDolly = 1;
			DollyPoint.set(R_curr,15,8);
		};
		set_3D(SET_3D_DIRECT_PLACE,R_curr.x,R_curr.y,R_curr.z,0,0,0);
		precise_impulse(R_curr,v2.x,v2.y);		
	}else
		set_3D(SET_3D_DIRECT_PLACE,R_curr.x,R_curr.y,R_curr.z,0,0,0);
};

void StuffObject::DeviceOut(Vector v1,int flag,Vector v2)
{
	int d;
	
	if(LightData){
		LightData->Destroy();
		LightData = NULL;
	};	
	
	R_curr = v1;
	Status = 0;
	cycleTor(R_curr.x,R_curr.y);
	GetVisible();

	CycleTime = 0;
	Hour = ConTimer.hour;
	Minutes = ConTimer.min;

	if(CreateMode == STUFF_CREATE_RELAX) CreateMode = STUFF_CREATE_EXRELAX;
	else CreateMode = STUFF_CREATE_NONE;

	switch_analysis(0);
	SetStuffColor();

	switch(ActIntBuffer.type){
		case  ACI_KERNOBOO:
		case ACI_PIPETKA:			
			archimedean = 255;
			break;
		case ACI_ELEEPOD:
		case ACI_BEEBOORAT:
		case ACI_ZEEX:
		case ACI_BOORAWCHIK:
			device_modulation = 1;
			break;
		case ACI_PROTRACTOR:
		case ACI_MECHANIC_MESSIAH:
		case ACI_FUNCTION83:
		case ACI_SPUMMY:			
			CreateArtefactTarget(this);
			break;
		default:
			archimedean = 0;
			break;
	};	

	if(!NetworkON || (Owner->Status & SOBJ_ACTIVE)){
		if(flag){
			if(ActIntBuffer.type == ACI_EMPTY_CONLARVER){
				FindDolly = 1;
				R_curr = Vector(XCYCL(R_curr.x + GeneralMousePoint.x),YCYCL(R_curr.y + GeneralMousePoint.y),0);
				set_3D(SET_3D_TO_THE_LOWER_LEVEL,R_curr.x,R_curr.y,0,0,0,0);
				DollyPoint.set(R_curr,15,8);
			}else{
				set_3D(SET_3D_DIRECT_PLACE,R_curr.x,R_curr.y,R_curr.z,0,0,0);
				if(Owner->Armor > 0 && Owner->Energy >= Owner->DropEnergy){
					if(Owner->Status & SOBJ_ACTIVE){
						d = GeneralMousePoint.vabs();
						if(d > 512) d = 512;
						if(v2.y == -1){
							v2.x = XCYCL(R_curr.x + GeneralMousePoint.x);
							v2.y = YCYCL(GeneralMousePoint.y + R_curr.y);
						};					
					}else{
						if(v2.y == -1){
							v2.x = XCYCL(R_curr.x + MAX_STUFF_SCATTER - RND(2*MAX_STUFF_SCATTER));
							v2.y = YCYCL(R_curr.y + MAX_STUFF_SCATTER - RND(2*MAX_STUFF_SCATTER));
						};
					};
				}else{
					if(v2.y == -1){
						v2.x = XCYCL(R_curr.x + MAX_STUFF_SCATTER - RND(2*MAX_STUFF_SCATTER));
						v2.y = YCYCL(R_curr.y + MAX_STUFF_SCATTER - RND(2*MAX_STUFF_SCATTER));
					};
				};
				precise_impulse(R_curr,v2.x,v2.y);
			};
		}else
			set_3D(SET_3D_DIRECT_PLACE,R_curr.x,R_curr.y,R_curr.z,0,0,0);
	}else{
		if(flag){
			if(ActIntBuffer.type == ACI_EMPTY_CONLARVER){
				FindDolly = 1;
				DollyPoint.set(R_curr,15,8);
			};
			set_3D(SET_3D_DIRECT_PLACE,R_curr.x,R_curr.y,R_curr.z,0,0,0);
			precise_impulse(R_curr,v2.x,v2.y);		
		}else
			set_3D(SET_3D_DIRECT_PLACE,R_curr.x,R_curr.y,R_curr.z,0,0,0);
	};

	Owner->DelDevice(this);
	ItemD.ConnectTypeList(this);
	GameD.ConnectBaseList(this);	

	PUT_GLOBAL_EVENT(AI_EVENT_DROP,ID,this,Owner);

	NetOwner = 0;
	xImpulse = v2.x;
	yImpulse = v2.y;
	if(Visibility == UNVISIBLE){
		R_curr.x = v2.x;
		R_curr.y = v2.y;
	};
	OutFlag = flag;

	if(NetworkON && (Owner->Status & SOBJ_ACTIVE)){
		NETWORK_OUT_STREAM.delete_object(NetDeviceID);
		NETWORK_OUT_STREAM < (uchar)(1);
		NETWORK_OUT_STREAM.end_body();

		NETWORK_OUT_STREAM.create_permanent_object(NetID,xImpulse,yImpulse,radius);
		NETWORK_OUT_STREAM < (uchar)(DataID);
		NETWORK_OUT_STREAM < (int)(NetDeviceID);
		NETWORK_OUT_STREAM < (short)(R_curr.z);
		NETWORK_OUT_STREAM < (uchar)(ActIntBuffer.type);
		NETWORK_OUT_STREAM < (int)(ActIntBuffer.data0);
		NETWORK_OUT_STREAM < (int)(ActIntBuffer.data1);
		NETWORK_OUT_STREAM < (uchar)(CreateMode | (OutFlag << 7));
		NETWORK_OUT_STREAM < (short)(Time);
		NETWORK_OUT_STREAM < (short)(R_curr.x);
		NETWORK_OUT_STREAM < (short)(R_curr.y);
		NETWORK_OUT_STREAM < (int)(NetOwner);
		NETWORK_OUT_STREAM.end_body();
	};	
	Owner = NULL;
};

void StuffObject::Touch(GeneralObject* p)
{
	if(Status & SOBJ_WAIT_CONFIRMATION) return;
	switch(p->ID){
		case ID_VANGER:
			if(!Owner){
				Status |= SOBJ_LINK;
				Owner = (VangerUnit*)p;
			};
/*			if(!Owner){
				if(((VangerUnit*)(p))->CheckInDevice(this)){
					if(!(((VangerUnit*)(p))->Status & SOBJ_AUTOMAT)){
						ActIntBuffer.slot = -1;
						aciSendEvent2actint(ACI_PUT_ITEM,&ActIntBuffer);
						SOUND_TAKE_ITEM()
					};
					Owner = (VangerUnit*)p;
					Status |= SOBJ_LINK;
				};
			}else{
				if(!(Status & SOBJ_LINK)) ErrH.Abort("Bad Collision Stuff");
			};*/
			break;
	};
};

void StuffObject::Active(void){ };
void StuffObject::Deactive(void){ };
void StuffObject::DeviceQuant(void){ };

void StuffObject::DeviceLoad(Parser& in)
{
	in.search_name("ModelID");
	ModelID = ModelD.FindModel(in.get_name());

	in.search_name("SizeID");
	SizeID = in.get_int();

//	in.search_name("ActiveInterfaceID");
//	ActIntBuffer.type = in.get_int();
};

void StuffObject::GetDevice(StuffObject* p)
{
	DataID = p->DataID;
	StuffType = p->StuffType;
	ModelID = p->ModelID;
	uvsDeviceType = p->uvsDeviceType;
	ActIntBuffer.type = p->ActIntBuffer.type;
	ActIntBuffer.stuffOwner = (void*)(this);
	SizeID = p->SizeID;
};

void BulletObject::Init(void)
{
	BaseObject::Init();

	ID = ID_BULLET;
	Status = SOBJ_DISCONNECT;
	radius = 1;
};

void BulletObject::CreateBullet(Vector fv,Vector tv,GeneralObject* target,WorldBulletTemplate* p,GeneralObject* _Owner,int _speed)
{
	int d;
	R_prev = R_curr = fv;
	Status = 0;
	GetVisible();

	Owner = _Owner;

	R_curr.z += p->AltOffset;

	Time = p->LifeTime;
	Power = p->Power;
	DeltaPower = p->DeltaPower;

	radius = 1;
	TargetObject = target;

	BulletID = p->BulletID;
	ShowID = p->ShowID;
	ShowType = p->ShowType;
	ExtShowType = p->ExtShowType;

	TargetMode = p->TargetMode;
	BulletMode = p->BulletMode;
	Precision = p->Precision;

	CraterType = p->CraterType;

	vTail = R_curr;
	
	vDelta = vTarget = Vector(0,0,0);
	FrameCount = 0;
//	OwnerTouchFlag = BULLET_OWNER_TOUCH | BULLET_OWNER_CHECK;

	Speed = p->Speed + _speed;
	BulletScale = p->BulletScale;
	AltOffset = p->AltOffset;
	vWallTarget = tv;


	LightData = NULL;

//	if(BulletMode & BULLET_CONTROL_MODE::AIM){
		vDelta = Vector(getDistX(tv.x,R_curr.x),getDistY(tv.y,R_curr.y),tv.z - R_curr.z);
		d = vDelta.vabs();
		vDelta = vDelta * Speed / vDelta.vabs();
//	};

	DataID = p->ID;
 };


void BulletObject::CreateBullet(GunSlot* p,WorldBulletTemplate* n)
{
	int d;
	R_prev = R_curr = p->vFire;
	Status = 0;
	GetVisible();

	Time = n->LifeTime;
	Power = n->Power;
	DeltaPower = n->DeltaPower;

	radius = n->BulletRadius;
	TargetObject = p->TargetObject;
	BulletID = n->BulletID;
	ShowID = n->ShowID;
	ShowType = n->ShowType;
	ExtShowType = n->ExtShowType;

	TargetMode = n->TargetMode;
	BulletMode = n->BulletMode;
	Precision = n->Precision;

	CraterType = n->CraterType;
	BulletScale = n->BulletScale;

	vTail = R_curr;

	vDelta = vTarget = Vector(0,0,0);
	FrameCount = 0;
//	OwnerTouchFlag = BULLET_OWNER_TOUCH | BULLET_OWNER_CHECK;

	Speed = n->Speed;
	if(BulletMode & BULLET_CONTROL_MODE::SPEED) Speed += p->RealSpeed;

	if(BulletID == BULLET_TYPE_ID::LASER)
		vDelta = Vector(Speed,3 - RND(6),0)*p->mFire;
	else 
		vDelta = Vector(Speed,0,0)*p->mFire;

	if(TargetObject && (BulletMode & BULLET_CONTROL_MODE::AIM)){
		vDelta = Vector(getDistX(TargetObject->R_curr.x,R_curr.x),getDistY(TargetObject->R_curr.y,R_curr.y),TargetObject->R_curr.z - R_curr.z);
		d = vDelta.vabs();
		vDelta = vDelta * Speed / vDelta.vabs();
	};

	if(CurrentWorld == WORLD_KHOX)
		BulletScale >>= 1;

	AltOffset = n->AltOffset;
	vWallTarget = R_curr;
	DataID = n->ID;

	LightData = NULL;
};

void BulletObject::Event(int type)
{
	int i,y0,y1;
	Vector vCheck;
	StaticObject* st;
	int l;

	WaterParticleObject* wp;
	uchar* tp;

	int TntEnable;

	Status |= SOBJ_DISCONNECT;

	switch(type){
		case BULLET_EVENT_ID::MAP_DESTROY:
			TntEnable = 0;
			if(CurrentWorld != 1){
				y0 = R_curr.y - EXPLOSION_BARELL_RADIUS;
				y1 = R_curr.y + EXPLOSION_BARELL_RADIUS;
				i = FindFirstStatic(y0,y1,(StaticObject**)TntSortedData,TntTableSize);				
				
				if(i > -1){
					do{
						st = TntSortedData[i];
						if(st->Enable){
							vCheck.x = getDistX(st->R_curr.x,R_curr.x);
	//						l = radius + st->radius;
							l = radius + EXPLOSION_BARELL_RADIUS;
							if(abs(vCheck.x) < l){
								vCheck.y = getDistY(st->R_curr.y,R_curr.y);
								vCheck.z = st->R_curr.z - R_curr.z;
								if(vCheck.abs2() < l*l){
									st->Touch(this);
									TntEnable = 1;
									if(Owner && Owner == ActD.Active)
										uvsCheckKronIventTabuTask(UVS_KRON_EVENT::GRIB,1);
								};
							};
						};
						i++;
					}while(i < TntTableSize && TntSortedData[i]->R_curr.y < y1);
				};
			};

			if(!TntEnable){
				switch(ShowID){
					case BULLET_SHOW_TYPE_ID::PARTICLE:
						tp = vMap->lineT[R_curr.y];
						if(tp){
							tp += R_curr.x + H_SIZE;
							if(GET_TERRAIN_TYPE(*tp) == WATER_TERRAIN && FloodLvl[R_curr.y >> WPART_POWER] >= R_curr.z){
								if(!RND(2) || CurrentWorld != WORLD_GLORX) EffD.CreateDeform(R_curr,DEFORM_WATER_ONLY,DT_DEFORM02);
								wp = (WaterParticleObject*)(EffD.GetObject(EFF_PARTICLE04));
								if(wp){
									wp->CreateParticle(30,5,1 << 8,5,5,28,5,R_curr);
									EffD.ConnectObject(wp);
								};
							}else{
								for(i = 0;i < 6;i++){
									vCheck = Vector(20,0,30)*DBM((int)RND(PI*2),Z_AXIS);
									vCheck += R_curr;
									vCheck.x = XCYCL(vCheck.x);
									vCheck.y = YCYCL(vCheck.y);
									EffD.CreateParticle(ExtShowType,R_curr,vCheck,ShowType);
//									EffD.CreateParticleTime(ExtShowType,R_curr,vCheck,ShowType,10);
								};							
							};
							if(CraterType >= 0) MapD.CreateCrater(R_curr,CraterType);
						};
						break;
					case BULLET_SHOW_TYPE_ID::FIREBALL:
						CreateDestroyEffect(R_curr,CraterType,DT_DEFORM02,DEFORM_WATER_ONLY,-1,DT_FIRE_BALL02);
						break;
					case BULLET_SHOW_TYPE_ID::LASER:
						if(CraterType >= 0) MapD.CreateCrater(R_curr,CraterType);
						break;
				};
			};
			break;
		case BULLET_EVENT_ID::LIFE_TIME_DESTROY:
			if(BulletMode & BULLET_CONTROL_MODE::MAP_DESTRUCTION){
				if(ShowID == BULLET_SHOW_TYPE_ID::PARTICLE){
					tp = vMap->lineT[R_curr.y];
					if(tp){
						tp += R_curr.x + H_SIZE;
						if(GET_TERRAIN_TYPE(*tp) == WATER_TERRAIN && FloodLvl[R_curr.y >> WPART_POWER] >= R_curr.z){
							if(!RND(2) || CurrentWorld != WORLD_GLORX) EffD.CreateDeform(R_curr,DEFORM_WATER_ONLY,DT_DEFORM02);
							wp = (WaterParticleObject*)(EffD.GetObject(EFF_PARTICLE04));
							if(wp){
								wp->CreateParticle(30,5,1 << 8,5,5,28,5,R_curr);
								EffD.ConnectObject(wp);
							};
						}else{
							for(i = 0;i < 6;i++){
								vCheck = Vector(20,0,30)*DBM((int)RND(PI*2),Z_AXIS);
								vCheck += R_curr;
								vCheck.x = XCYCL(vCheck.x);
								vCheck.y = YCYCL(vCheck.y);
	//							EffD.CreateParticle(ExtShowType,R_curr,vCheck,ShowType);
								EffD.CreateParticleTime(ExtShowType,R_curr,vCheck,ShowType,15);
							};							
						};						
					};					
				};
				if(CraterType >= 0) MapD.CreateCrater(R_curr,CraterType);
			}else{
				if(ShowID == BULLET_SHOW_TYPE_ID::FIREBALL)
					EffD.CreateFireBall(R_curr + Vector(0,0,20),DT_FIRE_BALL02,NULL,1 << 8,0);
			};
			break;
		case BULLET_EVENT_ID::TOUCH:
			switch(ShowID){
				case BULLET_SHOW_TYPE_ID::PARTICLE:
					R_prev = R_curr - vDelta;
					R_prev.x = XCYCL(R_prev.x);
					R_prev.y = YCYCL(R_prev.y);
					for(i = 0;i < 6;i++){
						vCheck = Vector(25 - RND(50),25 - RND(50),25 - RND(50));
						vCheck += R_prev;
						vCheck.x = XCYCL(vCheck.x);
						vCheck.y = YCYCL(vCheck.y);
						EffD.CreateParticle(ExtShowType,R_curr,vCheck,ShowType);
					};
					break;
				case BULLET_SHOW_TYPE_ID::FIREBALL:
					EffD.CreateFireBall(R_curr + Vector(0,0,20),DT_FIRE_BALL02);
					break;
				case BULLET_SHOW_TYPE_ID::LASER:
					EffD.CreateExplosion(R_curr + Vector(0,0,20),EFF_EXPLOSION05);
					break;
				case BULLET_SHOW_TYPE_ID::CRATER:
//					EffD.CreateDeform(R_curr + Vector(16 - RND(32),16 - RND(32),0),DEFORM_ALL,PASSING_WAVE_PROCESS);
//					EffD.CreateDeform(R_curr + Vector(16 - RND(32),16 - RND(32),0),DEFORM_ALL,PASSING_WAVE_PROCESS);
//					EffD.CreateDeform(R_curr + Vector(16 - RND(32),16 - RND(32),0),DEFORM_ALL,PASSING_WAVE_PROCESS);
//					EffD.CreateDeform(R_curr + Vector(16 - RND(32),16 - RND(32),0),DEFORM_ALL,PASSING_WAVE_PROCESS);
					if(CraterType >= 0) MapD.CreateCrater(R_curr,CraterType);
					break;
			};
			break;
	};
};

void BulletObject::TimeOutQuant(void)
{
	int d;
	uchar alt;
	Vector v;
	VangerFunctionType* g;

	if(TargetObject){
		v = Vector(getDistX(TargetObject->R_curr.x,R_curr.x),getDistY(TargetObject->R_curr.y,R_curr.y),TargetObject->R_curr.z - R_curr.z);
		if((TargetObject->Status & SOBJ_DISCONNECT) || (TargetObject->ID == ID_VANGER && (!((VangerUnit*)(TargetObject))->ExternalDraw || ((VangerUnit*)(TargetObject))->draw_mode != NORMAL_DRAW_MODE || ((VangerUnit*)(TargetObject))->BeebonationFlag)))
			TargetObject = NULL;
	};

	if(BulletMode & BULLET_CONTROL_MODE::TARGET){
		switch(TargetMode){
			case BULLET_TARGET_MODE::CONTROL:
				if(TargetObject) vTarget = v;
				else{
					vTarget = Vector(0,0,0);
					BulletMode &= ~BULLET_CONTROL_MODE::TARGET;
				};
				break;
			case BULLET_TARGET_MODE::LEFT_ROUND:
				vTarget = Vector(vDelta.y,-vDelta.x,0);
				break;
			case BULLET_TARGET_MODE::RIGHT_ROUND:
				vTarget = Vector(-vDelta.y,vDelta.x,0);
				break;
			case BULLET_TARGET_MODE::RANDOM:
				if(RND(100) < 10) vTarget = Vector(BMAX_TARGET_VECTOR - RND(BMAX_TARGET_VECTOR2),BMAX_TARGET_VECTOR - RND(BMAX_TARGET_VECTOR2),BMAX_TARGET_VECTOR - RND(BMAX_TARGET_VECTOR2));
				break;
			case BULLET_TARGET_MODE::WALL:				
				v.x = getDistX(R_curr.x,vWallTarget.x);
				v.y = getDistY(R_curr.y,vWallTarget.y);
				vTarget.x = v.x - v.y;
				vTarget.y = v.y + v.x;
				vTarget.z = 0;
				break;
		};

		d = vTarget.vabs();
		if(d){
			vDelta += vTarget * Precision / d;
			d = vDelta.vabs();
			if(d > Speed) vDelta = vDelta * Speed / d;
		};
	};

	vTail = R_curr;
	R_curr += vDelta;
	cycleTor(R_curr.x,R_curr.y);	

	Time--;
	Power -= DeltaPower;

	if(Visibility == VISIBLE){
		g = ActD.fTail;
		while(g){
			if(g->ID == PROTRACTOR_PALLADIUM){
				v = Vector(getDistX(R_curr.x,g->vR.x),getDistY(R_curr.y,g->vR.y),0);
				d = v.vabs();
				if(d < PALLADIUM_RADIUS){
					vDelta = Vector(Speed,0,0)*DBM(PI/2 + v.psi(),Z_AXIS);
					R_curr = vTail;
					R_curr += vDelta;
					cycleTor(R_curr.x,R_curr.y);
				};
				break;
			};
			g = g->Next;
		};

		if(Time <= 0)
			Event(BULLET_EVENT_ID::LIFE_TIME_DESTROY);

		if(BulletMode & BULLET_CONTROL_MODE::FLY){
			v = vTail;
			if(MapLineTrace(v,R_curr)){
				R_curr = v;
				Event(BULLET_EVENT_ID::MAP_DESTROY);
			};
		}else{
			if(ShowID == BULLET_SHOW_TYPE_ID::CRATER && ExtShowType){
				if(BigGetAlt(R_curr.x,R_curr.y,R_curr.z,alt,ExtShowType)) MapLevel = 1;
				else{
					MapLevel = 0;
					alt = 0;
					if(LightData){
						LightData->Destroy();
						LightData = NULL;
					};
				};
			}else MapLevel = GetAlt(R_curr.x,R_curr.y,R_curr.z,alt);			
			R_curr.z = alt + AltOffset;
		};
	}else{
		if(Time <= 0)
			Event(BULLET_EVENT_ID::HIDE_LIFE_TIME_DESTROY);
	};

//	if(ShowID == BULLET_SHOW_TYPE_ID::PARTICLE){ 
//		EffD.CreateParticle(ExtShowType,vTail,R_curr,ShowType);
//		if(BulletScale) 
//			EffD.CreateParticle(ExtShowType,vTail,R_curr,ShowType);
//	};
};
		
void BulletObject::Quant(void)
{	
	if(Status & SOBJ_DISCONNECT) return;
	
	GetVisible();
	TimeOutQuant();
	if(Visibility == UNVISIBLE){
		if(LightData){
			LightData->Destroy();
			LightData = NULL;
		};
		R_prev = vTail;
	};
};

const int BULLET_IMPULSE_POWER = 9 << 14;
const int BULLET_IMPULSE_ARM = 5 << 16;

void camera_impulse(int amplitude_8);

void BulletObject::Touch(GeneralObject* p)
{
	if(p == Owner) return;
	if(p->ID == ID_VANGER){
		if(((VangerUnit*)(p))->BeebonationFlag) return;
		if(Owner) PUT_GLOBAL_EVENT(AI_EVENT_COLLISION,ID_BULLET,Owner,(VangerUnit*)(p));

		if(BulletMode & BULLET_CONTROL_MODE::IMPULSE){
			if(/*(BulletMode & BULLET_CONTROL_MODE::FLY) && */(ShowID != BULLET_SHOW_TYPE_ID::CRATER))
				((VangerUnit*)(p))->impulse(vDelta,Power / BULLET_IMPULSE_POWER,Power / BULLET_IMPULSE_ARM);
			else{
				((VangerUnit*)(p))->impulse(Vector(0,0,64),4*Power / BULLET_IMPULSE_POWER,4*Power / BULLET_IMPULSE_ARM);
			};
		};

		if((ShowID == BULLET_SHOW_TYPE_ID::CRATER) && (p->Status & SOBJ_ACTIVE))
			camera_impulse(256);

		((VangerUnit*)(p))->BulletCollision(Power,Owner);

		if(!(BulletMode & BULLET_CONTROL_MODE::UNTOUCH))				
			Event(BULLET_EVENT_ID::TOUCH);
	};
	R_prev = vTail;
};

void BulletObject::DrawQuant(void)
{
	int tx,ty,s;
	Vector vCheck;	

	switch(ShowID){
		case BULLET_SHOW_TYPE_ID::PARTICLE:			
			EffD.CreateParticle(ExtShowType,R_prev,R_curr,ShowType);
			if(BulletScale) 
				EffD.CreateParticle(ExtShowType,R_prev,R_curr,ShowType);
			break;
		case BULLET_SHOW_TYPE_ID::FIREBALL:
			if(AdvancedView) s = G2LF(R_curr.x,R_curr.y,R_curr.z,tx,ty);
			else s = G2LS(R_curr.x,R_curr.y,R_curr.z,tx,ty);
//!!!!!!!!!
			s *= BulletScale;
			EffD.FireBallData[ShowType].Show(tx,ty,R_curr.z,s,FrameCount);
			EffD.FireBallData[ShowType].CheckOut(FrameCount);
			if(LightData) 
				LightData->set_position(XCYCL(R_curr.x + vDelta.x),YCYCL(R_curr.y + vDelta.y),R_curr.z);
			else 
				LightData = MapD.CreateLight(R_curr.x,R_curr.y,R_curr.z,40,32,LIGHT_TYPE::DYNAMIC);
			break;
		case BULLET_SHOW_TYPE_ID::DEFORM:
			if(AdvancedView) s = G2LF(R_curr.x,R_curr.y,R_curr.z,tx,ty);
			else s = G2LS(R_curr.x,R_curr.y,R_curr.z,tx,ty);
			if(EffD.DeformData[ShowType].CheckOffset(FrameCount)) FrameCount = 0;
			EffD.DeformData[ShowType].Deform(tx,ty,FrameCount,1);
			break;
		case BULLET_SHOW_TYPE_ID::DUST:
			MapD.CreateDust(Vector(R_curr.x,R_curr.y,MapLevel),ShowType);
			break;
		case BULLET_SHOW_TYPE_ID::CRATER:
			if(MapLevel && (vDelta.x != 0 || vDelta.y != 0 || vDelta.z != 0)){
				if(ExtShowType){
					if(BulletScale){
						if(LightData) 
							LightData->set_position(R_prev.x,R_prev.y,R_prev.z + 63);
						else 
							LightData = MapD.CreateLight(R_curr.x,R_curr.y,R_curr.z,20,32,LIGHT_TYPE::STATIC);

						MapD.CreateLavaSpot(R_curr,5,5,20,10,0,0,1,8,83,0,4,1,ExtShowType);
					}else MapD.CreateLavaSpot(R_curr,5,2,10,7,0,0,2,7,83,0,4,1,ExtShowType);
				}else MapD.CreateLavaSpot(R_curr,5,2,10,7,0,0,2,7,83,0,4,1,83);
			};				
			break;
		case BULLET_SHOW_TYPE_ID::LASER:
			vCheck = Vector(getDistX(R_prev.x,R_curr.x),getDistY(R_prev.y,R_curr.y),R_prev.z - R_curr.z);

			vCheck.x = 2*vCheck.x / 3;
			vCheck.y = 2*vCheck.y / 3;
			vCheck.z = 2*vCheck.z / 3;

			vCheck += R_curr;
			cycleTor(vCheck.x,vCheck.y);
			ScreenLineTrace(R_curr,vCheck,FireColorTable,0);
			break;
	};
	R_prev = vTail;
};


void GunDevice::DeviceLoad(Parser& in)
{
//	char* name;

	StuffObject::DeviceLoad(in);
	in.search_name("GunID");

	pData = &GameBulletData[in.get_int()];

};

void GunDevice::GetDevice(StuffObject* p)
{
	GunDevice* n;
	n = (GunDevice*)p;

	StuffObject::GetDevice(p);
	pData = n->pData;
};

int DevicetStorageID[MAX_DEVICE_TYPE] = {ITEM_GUN_DEVICE,ITEM_CHANGER,ITEM_OTHER};

/*void addDevice(int x,int y,int z,uvsItem* uvsD,ActionUnit* Owner,int net_mode,int nid)
{
	StuffObject* n = (StuffObject*)0x83838383;
	StuffObject* p = (StuffObject*)0x69696969;	
	int l;

	l = uvsD->type;
	n = ItemD.StuffItemLink[l];
	p = (StuffObject*)(ItemD.GetObject(DevicetStorageID[n->StuffType]));
	if(p){
		if(Owner) p->CreateDevice(Owner,n);
		else p->CreateStuff(Vector(x,y,z),n);
		
		p->ActIntBuffer.data0 = uvsD->param1;
		p->ActIntBuffer.data1 = uvsD->param2;

		p->ActIntBuffer.type = uvsSetItemType(p->uvsDeviceType,uvsD->param1,uvsD->param2);

		switch(n->StuffType){
			case DEVICE_ID_GUN:
				((GunDevice*)(p))->CreateGun();
				break;
			case DEVICE_ID_LOCATOR:
				((LocatorDevice*)(p))->CreateLocator();
				break;
		};
	};
};*/

StuffObject* addDevice(int x,int y,int z,int device_type,int param1,int param2,VangerUnit* Owner,int cMode)
{
	StuffObject* n = NULL;
	StuffObject* p = NULL;
	GeneralObject* g;	
	GeneralObject* gg;
	int t;

	n = ItemD.StuffItemLink[device_type];
	p = (StuffObject*)(ItemD.GetObject(DevicetStorageID[n->StuffType]));

	if(!p && Owner){
		g = ItemD.Tail;
		t = n->StuffType;
		while(g){
			gg = g->NextTypeList;
			if(((StuffObject*)(g))->StuffType == t && ((StuffObject*)(g))->CheckItemOver()){
				ItemD.DeleteItem((StuffObject*)(g));
				break;
			};
			g = gg;
		};
		p = (StuffObject*)(ItemD.GetObject(DevicetStorageID[t]));
	};

	if(p){
		p->ActIntBuffer.data0 = param1;
		p->ActIntBuffer.data1 = param2;

		if(Owner) p->CreateDevice(Vector(x,y,z),Owner,n);
		else{
			if(cMode == STUFF_CREATE_NONE){
				if(z == ITEM_DEFAULT_Z) p->CreateStuff(Vector(x,y,z),n,STUFF_CREATE_NONE);
				else 
					p->CreateStuff(Vector(x,y,z),n,STUFF_CREATE_RELAX);
			}else p->CreateStuff(Vector(x,y,z),n,cMode);
		};

		if(p->StuffType == DEVICE_ID_GUN)
			((GunDevice*)(p))->CreateGun();
	};
	
	return p;
};

void JumpBallObject::Init(void)
{
	BaseObject::Init();

	ID = ID_JUMPBALL;
	Status = SOBJ_DISCONNECT;
};

void JumpBallObject::CreateBullet(GunSlot* p,WorldBulletTemplate* n)
{
	Vector vCheck;
	R_prev = R_curr = p->vFire;
	Status = 0;

	DataID = n->ID;

	Object::operator = (ModelD.ActiveModel(n->ShowType));
	cycleTor(R_curr.x,R_curr.y);
	set_active(0);

	set_3D(SET_3D_DIRECT_PLACE,R_curr.x,R_curr.y,R_curr.z,0,0,0);

	GetVisible();

	Power = n->Power;
	DeltaPower = n->DeltaPower;
	Mode = n->TargetMode;
	CraterType = n->CraterType;

	Owner = p->Owner;

	archimedean = 0;

	if(Mode == BULLET_TARGET_MODE::CONTROL){
		vCheck = Vector(-(n->Speed),0,0)*DBM((int)(PI/4 - RND(PI/2)),Z_AXIS);
		vCheck *= p->mFire;
		vCheck *= n->LifeTime + Owner->Speed;
		vCheck /= n->Speed;
		precise_impulse(R_curr,XCYCL(vCheck.x + R_curr.x),YCYCL(vCheck.y + R_curr.y));
		set_body_color(COLORS_IDS::MATERIAL_5);
	}else{
		vCheck = Vector(n->Speed,0,0)*p->mFire;
		vCheck *= n->LifeTime + Owner->Speed;
		vCheck /= n->Speed;
		precise_impulse(R_curr,XCYCL(R_curr.x + vCheck.x),YCYCL(R_curr.y + vCheck.y));
	};
};

void JumpBallObject::Quant(void)
{
	char t;
	int y0,y1,i,l;
	Vector vCheck;
	StaticObject* st;
	VangerFunctionType* g;

	t = Visibility;
	GetVisible();

	if(Power > DeltaPower)
		Power -= DeltaPower;

	if(Visibility == VISIBLE){

		g = ActD.fTail;
		while(g){
			if(g->ID == PROTRACTOR_PALLADIUM){
				vCheck = Vector(getDistX(R_curr.x,g->vR.x),getDistY(R_curr.y,g->vR.y),R_curr.z - g->vR.z);
				l = vCheck.vabs();
				if(l < PALLADIUM_RADIUS)
					impulse(vCheck,30 * (PALLADIUM_RADIUS - l) / PALLADIUM_RADIUS,0);
				break;
			};
			g = g->Next;
		};

		analysis();
		cycleTor(R_curr.x,R_curr.y);
		if(dynamic_state & GROUND_COLLISION){
			y0 = R_curr.y - EXPLOSION_BARELL_RADIUS;
			y1 = R_curr.y + EXPLOSION_BARELL_RADIUS;
			i = FindFirstStatic(y0,y1,(StaticObject**)TntSortedData,TntTableSize);
			if(i > -1){
				do{
					st = TntSortedData[i];
					if(st->Enable){
						vCheck.x = getDistX(st->R_curr.x,R_curr.x);
//						l = radius + st->radius;
						l = radius + EXPLOSION_BARELL_RADIUS;
						if(abs(vCheck.x) < l){
							vCheck.y = getDistY(st->R_curr.y,R_curr.y);
							vCheck.z = st->R_curr.z - R_curr.z;
							if(vCheck.abs2() < l*l){
								st->Touch(this);
								Touch(st);
								if(Owner && Owner == ActD.Active)
									uvsCheckKronIventTabuTask(UVS_KRON_EVENT::GRIB,1);
							};
						};
					};
					i++;
				}while(i < TntTableSize && TntSortedData[i]->R_curr.y < y1);
			};
			Status |= SOBJ_DISCONNECT;
			if(Mode == BULLET_TARGET_MODE::CONTROL)	CreateDestroyEffect(R_curr,MAP_POINT_CRATER03,DT_DEFORM02,DEFORM_WATER_ONLY,-1,DT_FIRE_BALL02);
			else CreateDestroyEffect(R_curr,MAP_POINT_CRATER10,DT_DEFORM02,DEFORM_WATER_ONLY,EFF_EXPLOSION03,-1);			
			if(ActD.Active)
				SOUND_EXPLOSION_CRUSTER(getDistX(ActD.Active->R_curr.x,R_curr.x));
		};
	}else Status |= SOBJ_DISCONNECT;	
};

void JumpBallObject::DrawQuant(void)
{
	draw();
};

void JumpBallObject::Touch(GeneralObject* p)
{
	if(p->ID == ID_VANGER){
		if(((VangerUnit*)(p))->BeebonationFlag) return;
		((VangerUnit*)(p))->BulletCollision(Power,Owner);
		Status |= SOBJ_DISCONNECT;
		if(ActD.Active)
			SOUND_BARREL_DESTROY(getDistX(ActD.Active->R_curr.x,R_curr.x))
		if(Mode == BULLET_TARGET_MODE::CONTROL){
			EffD.CreateExplosion(R_curr + Vector(0,0,20),EFF_EXPLOSION02);
			((VangerUnit*)(p))->impulse(Vector(32 - RND(64),32 - RND(64),RND(64)),8 + RND(15),20);
			if(ActD.Active)
				SOUND_BARREL_DESTROY(getDistX(ActD.Active->R_curr.x,R_curr.x))
		}else{
			EffD.CreateExplosion(R_curr + Vector(0,0,20),EFF_EXPLOSION03);						
			if(ActD.Active)
				SOUND_EXPLOSION_CRUSTER(getDistX(ActD.Active->R_curr.x,R_curr.x));
		};
	};
};

void CreateDestroyEffect(Vector& v,int crat_id,int def_id,int def_flag,int expl_id,int fire_id)
{
	WaterParticleObject* wp;

	uchar* tp = vMap->lineT[v.y];
	if(tp)	tp += v.x + H_SIZE;
	else return;

	if(GET_TERRAIN_TYPE(*tp) == WATER_TERRAIN && FloodLvl[v.y >> WPART_POWER] >= v.z){
		if(def_id >= 0) EffD.CreateDeform(v,def_flag,def_id);
		wp = (WaterParticleObject*)(EffD.GetObject(EFF_PARTICLE04));
		if(wp){
			wp->CreateParticle(30,5,1 << 8,5,5,28,5,v);
			EffD.ConnectObject(wp);
		};
	}else{
		if(expl_id >= 0) EffD.CreateExplosion(v,expl_id);
		else if(fire_id >= 0) EffD.CreateFireBall(v,fire_id);
	};
	if(crat_id >= 0) MapD.CreateCrater(v,crat_id);
};

int Name2Int(char* name,const char* key[],int max)
{
	int i,n;
	n = -1;
	for(i = 0;i < max;i++)
		if(!strcmp(name,key[i])){
			n = i;
			break;
		};
	if(n == -1)
		ErrH.Abort("Bad Name 2 Int");
	return n;
};

void ChangerDevice::DeviceQuant(void)
{
};

void ChangerDevice::DeviceOut(Vector v,int flag,Vector v2)
{
	PrevOwner = (VangerUnit*)(Owner);
	StuffObject::DeviceOut(v,flag,v2);
};

const int ACI_CHECK_RADIUS = 1;

int aciGetScreenItem(int x,int y)
{
	BaseObject* p;
	BaseObject* mp;

   	int tx,ty,dx,dy,l;
	int rz;

	mp = NULL;
	rz = -1;
	S2G(x,y,tx,ty);

	GeneralMousePoint = Vector(getDistX(tx,ViewX),getDistY(ty,ViewY),0);

	p = (BaseObject*)(ItemD.Tail);
	while(p){
//		if(p->ID != ID_STUFF)
//			ErrH.Abort("Bad STUFF List");
		if(p->Visibility == VISIBLE && p->R_curr.z > rz){
			dx = getDistX(tx,p->R_curr.x);
			dy = getDistY(ty,p->R_curr.y);
			l = p->radius + ACI_CHECK_RADIUS;
			if((dx*dx + dy*dy) < l*l){
				mp = p;
				rz = p->R_curr.z;
			};
		};
		p = (BaseObject*)(p->NextTypeList);
	};
	if(mp) return ((StuffObject*)(mp))->ActIntBuffer.type;
	return -1;	
};

void SkyFarmerObject::Init(void)
{
	BaseObject::Init();

	ID = ID_SKYFARMER;
	Status = SOBJ_DISCONNECT;
};

const int MAX_DROP_SEED = 1;
const int MAX_DROP_POINT = MAX_DROP_SEED*10;

void SkyFarmerObject::CreateSkyFarmer(int x_pos,int y_pos,int x_speed,int y_speed,int corn_type,int corn,int time)
{
	Vector vCheck;
	int nTarget;

	nTarget = 1 + RND(SkyFarmerWayTableSize - 2);
	TargetObject = FindFarmer(nTarget);

	R_prev = R_curr = Vector(TargetObject->R_curr.x,TargetObject->R_curr.y,0);
	Status = 0;
	cycleTor(R_curr.x,R_curr.y);
	GetVisible();
	set_active(0);
//	switch_analysis(1);
	
	Object::operator = (ModelD.ActiveModel(ModelD.FindModel("SkyFarmer")));

	skyfarmer_start(R_curr.x,R_curr.y,random(Pi*2));

	xSpeed = x_speed;
	ySpeed = y_speed;
	CornType = corn_type;
	Corn = corn;
	Timer  = time;

	switch(CornType){
		case UVS_ITEM_TYPE::KERNOBOO:
			set_body_color(COLORS_IDS::SKYFARMER_KERNOBOO);
			break;
		case UVS_ITEM_TYPE::PIPETKA:
			set_body_color(COLORS_IDS::SKYFARMER_PIPETKA);
			break;
	};

/*	nTarget = 0;
	vCheck = Vector(getDistX(SkyFarmerWayTable[0].R_curr.x,R_curr.x),getDistY(SkyFarmerWayTable[0].R_curr.y,R_curr.y),0);
	mDist = vCheck.abs2();
	for(i = 1;i < SkyFarmerWayTableSize;i++){
		vCheck = Vector(getDistX(SkyFarmerWayTable[i].R_curr.x,R_curr.x),getDistY(SkyFarmerWayTable[i].R_curr.y,R_curr.y),0);
		d = vCheck.abs2();
		if(d < mDist){
			mDist = d;
			nTarget = i;
		};
	};
	nTarget = SkyFarmerWayTable[nTarget].SensorType;
	if((nTarget + MAX_DROP_POINT) >= (SkyFarmerWayTableSize - 1)) dTarget = -1;
	else dTarget = 1;
	nTarget += dTarget;
*/

	if(!RND(2)) dTarget = -1;
	else dTarget = 1;

	DropCount = MAX_DROP_POINT;
	SeedCount = 0;
};

SensorDataType* SkyFarmerObject::FindFarmer(int n)
{
	int i;
	for(i = 0;i < SkyFarmerWayTableSize;i++){
		if(SkyFarmerWayTable[i].SensorType == n)
			return(&SkyFarmerWayTable[i]);
	};
	return NULL;
};

void SkyFarmerObject::Quant(void)
{
	Vector vTrack;
	int d;

	GetVisible();
	R_prev = R_curr;
	cycleTor(R_curr.x,R_curr.y);
	Timer++;
	
	vTrack = Vector(getDistX(TargetObject->R_curr.x,R_curr.x),getDistY(TargetObject->R_curr.y,R_curr.y),0);
	d = vTrack.vabs();

	if(d < radius*4){
		if(TargetObject->SensorType + dTarget <= 0) dTarget = 1;
		else if(TargetObject->SensorType + dTarget >= SkyFarmerWayTableSize - 1) dTarget = -1;

		TargetObject = FindFarmer(TargetObject->SensorType + dTarget);
		SeedCount = MAX_DROP_SEED;
	}else{
		if(d < radius*7) skyfarmer_set_direction(vTrack);
		else{
			if(!RND(3)) skyfarmer_set_direction(vTrack);
		};
	};

	if(skyfarmer_fly_direction == 0){
		if(SeedCount > 0){
			addDevice(R_curr.x,R_curr.y,R_curr.z,CornType,0,1,NULL,STUFF_CREATE_TRACK);
			DropCount--;
			SeedCount--;
		};
		if(SeedCount <= 0) scale_size = 2 * original_scale_size / 5 + 3*DropCount * original_scale_size / (MAX_DROP_POINT * 5);
		else scale_size = 2 * original_scale_size / 5 + 2*DropCount*original_scale_size / (5 * MAX_DROP_POINT) + DropCount * SeedCount*original_scale_size / (MAX_DROP_SEED * 5 * MAX_DROP_POINT);
		if(DropCount == 0) skyfarmer_end();
	};
	analysis();
	if(R_curr.z < -radius) Status |= SOBJ_DISCONNECT;
};

void SkyFarmerObject::DrawQuant(void)
{
	draw();
};

void addFarmer(int x_pos,int y_pos,int x_speed,int y_speed,int corn_type,int corn,int time)
{
	(FarmerD.CreateSkyFarmer())->CreateSkyFarmer(x_pos,y_pos,x_speed,y_speed,corn_type,corn,time);
};

//ind2 == -1 
int uvsapiDestroyItem(int ind,int ind2)
{
	VangerUnit* n;
	GeneralObject* p;
	StuffObject* g;
	int GamerOwnerLog,keep_log;
	GeneralObject* tp;
	StuffObject* tg;

	GamerOwnerLog = 0;	
	p = ItemD.Tail;
	while(p){
//		if(p->ID != ID_STUFF)
//			ErrH.Abort("Bad Stuff List");
		tp = p->NextTypeList;
		if(((StuffObject*)(p))->uvsDeviceType == ind)
			ItemD.DeleteItem((StuffObject*)(p));
		p = tp;
	};

	n = (VangerUnit*)(ActD.Tail);
	if(ind2 == -1){
		while(n){
//			if(n->ID != ID_VANGER)
//				ErrH.Abort("Bad Vanger List");
			g = n->DeviceData;
			if(n->Status & SOBJ_ACTIVE){
				while(g){
					tg = g->NextDeviceList;
					if(g->uvsDeviceType == ind){
						n->CheckOutDevice(g);
						ActD.CheckDevice(g);
						aciRemoveItem(&(g->ActIntBuffer));
						GamerOwnerLog = 1;
						n->DelDevice(g);
						g->Storage->Deactive(g);
 					};
					g = tg;
				};
			}else{
				while(g){
					tg = g->NextDeviceList;
					if(g->uvsDeviceType == ind){
						n->CheckOutDevice(g);
						n->DelDevice(g);
						g->Storage->Deactive(g);
 					};
					g = tg;
				};
			};
			n = (VangerUnit*)(n->NextTypeList);
		};
	}else{
		while(n){
			keep_log = 0;
			if(ind == UVS_ITEM_TYPE::ELEECH){
				g = n->DeviceData;
				while(g){
					if(g->ActIntBuffer.type == ACI_TOXICK){
						keep_log++;
						if(keep_log == SAFE_STUFF_MAX)
							break;
					};
					g = g->NextDeviceList;
				};
//				if(GetStuffObject(n,ACI_TOXICK)) keep_log = 1;
			}else{
				if(ind == UVS_ITEM_TYPE::WEEZYK){
//					if(GetStuffObject(n,ACI_PHLEGMA)) keep_log = 1;
					g = n->DeviceData;
					while(g){
						if(g->ActIntBuffer.type == ACI_PHLEGMA){
							keep_log++;
							if(keep_log == SAFE_STUFF_MAX)
								break;
						};
						g = g->NextDeviceList;
					};
				};
			};

			if(keep_log < SAFE_STUFF_MAX){
				g = n->DeviceData;
				while(g){
					tg = g->NextDeviceList;
					if(g->uvsDeviceType == ind){	
						g->uvsDeviceType = ind2;
						g->ActIntBuffer.type = uvsSetItemType(g->uvsDeviceType,g->ActIntBuffer.data0,g->ActIntBuffer.data1);
						if(n->Status & SOBJ_ACTIVE){
							switch(ind){
 								case UVS_ITEM_TYPE::ELEECH:
									aiMessageQueue.Send(AI_MESSAGE_DOWN_ELLECH,0,0xff,0);
									break;
								case UVS_ITEM_TYPE::WEEZYK:
									aiMessageQueue.Send(AI_MESSAGE_DOWN_WEEZYK,0,0xff,0);
									break;
								case UVS_ITEM_TYPE::KERNOBOO:
									aiMessageQueue.Send(AI_MESSAGE_DOWN_KERN,0,0xff,0);
									break;
								case UVS_ITEM_TYPE::PIPETKA:
									aiMessageQueue.Send(AI_MESSAGE_DOWN_PIP,0,0xff,0);
									break;
							};
							aciChangeItem(&(g->ActIntBuffer));
							GamerOwnerLog = 1;
						};
					};
					g = tg;
				};
			};
			n = (VangerUnit*)(n->NextTypeList);
		};
	};
	return GamerOwnerLog;
};


void WorldBulletTemplate::Init(Parser& in)
{
	char* name;

	in.search_name("BulletID");
	name = in.get_name();
	BulletID = Name2Int(name,BULLET_ID_NAME,MAX_BULLET_ID);

	in.search_name("LifeTime");
	LifeTime = in.get_int();

	in.search_name("FirstPower");
	Power = (in.get_int() << 16) / 100;

	in.search_name("LastPower");
	DeltaPower = (((in.get_int() << 16) / 100) - Power) / LifeTime;

	in.search_name("CraterType");
	CraterType = in.get_int();

	in.search_name("BulletMode");
	name = in.get_name();
	if(!strcmp(name,"{")){
		BulletMode = 0;
		while(1){
			name = in.get_name();
			if(!strcmp(name,"}")) break;
			BulletMode |= BULLET_CONTROL_MODE_VALUE[Name2Int(name,BULLET_CONTROL_MODE_NAME,MAX_BULLET_CONTROL_MODE_NAME)];
		};
	}else BulletMode = BULLET_CONTROL_MODE_VALUE[Name2Int(name,BULLET_CONTROL_MODE_NAME,MAX_BULLET_CONTROL_MODE_NAME)];

	if(BulletID != BULLET_TYPE_ID::CHAIN_GUN){
		in.search_name("Speed");
		Speed = in.get_int();
		in.search_name("ShowID");
		name = in.get_name();
		ShowID = Name2Int(name,BULLET_SHOW_ID_NAME,MAX_BULLET_SHOW_ID_NAME);
		in.search_name("ShowType");
		ShowType = in.get_int();
		in.search_name("ExtentionShowType");
		ExtShowType = in.get_int();
		in.search_name("Precision");
		Precision = in.get_int();
		in.search_name("TargetMode");
		name = in.get_name();
		TargetMode = Name2Int(name,BULLET_TARGET_MODE_NAME,MAX_BULLET_TARGET_MODE_NAME);
		in.search_name("BulletScale");
		BulletScale = in.get_int();
		in.search_name("AltOffset");
		AltOffset = in.get_int();
		in.search_name("BulletRadius");
		BulletRadius = in.get_int();
		in.search_name("TapeSize");
		TapeSize = in.get_int();
		in.search_name("WaitTime");
		WaitTime = WeaponWaitTime * in.get_int() >> 8;
	};
	Time = 0;
};

void ClefObject::Init(void)
{
	BaseObject::Init();
	ID = ID_DEBRIS;
	Status = 0;
};

void ClefObject::CreateClef(void)
{
	Object::operator = (ModelD.ActiveModel(ModelD.FindModel("WorldLocker")));
	switch(RND(24) / 8){
		case 0:
			R_curr = Vector(808,1630,0);
			break;
		case 1:
			R_curr = Vector(699,1785,0);
			break;
		default:
			R_curr = Vector(1188,1843,0);
			break;
	};
	cycleTor(R_curr.x,R_curr.y);
	GetVisible();
	set_active(0);	
	switch_analysis(0);
	ID = ID_DEBRIS;
	Status = 0;
	set_3D(SET_3D_TO_THE_LOWER_LEVEL,R_curr.x,R_curr.y,0,0,0,0);
	set_body_color(COLORS_IDS::MATERIAL_5);
};

void ClefObject::Quant(void)
{
	int lv;
	lv = Visibility;
	GetVisible();
	if(Visibility == VISIBLE){
		if(lv == UNVISIBLE) 
			set_3D(SET_3D_TO_THE_LOWER_LEVEL,R_curr.x,R_curr.y,0,0,0,0);
		R_prev = R_curr;
		analysis();
		cycleTor(R_curr.x,R_curr.y);
	};
};

void ClefObject::DrawQuant(void)
{
	draw();
};

void ClefObject::Touch(GeneralObject *obj)
{
	if(obj->ID == ID_BULLET){
		Status |= SOBJ_DISCONNECT;
		ActD.hMokKeyEnable = 1;
		EffD.CreateExplosion(R_curr,EFF_EXPLOSION01,this,1 << 15,1 << 11);
		if(ActD.Active)
			SOUND_EXPLOSION_CRUSTER(getDistX(ActD.Active->R_curr.x,R_curr.x));
	};
};

int ClefObject::test_objects_collision()
{
	BaseObject* p;

	p = (BaseObject*)(BulletD.Tail);
	while(p){
		if(p->Visibility == VISIBLE)
			test_object_to_baseobject(p);
		p = (BaseObject*)(p->NextTypeList);
	};

	p = (BaseObject*)(JumpD.Tail);
	while(p){
		if(p->Visibility == VISIBLE)
			test_object_to_baseobject(p);
		p = (BaseObject*)(p->NextTypeList);
	};
	return 0;
};

void FishWarrior::Init(void)
{
	BaseObject::Init();
	ID = ID_FISH;
	zStatus = 0;
};

void FishWarrior::Quant(void)
{
	Vector vTarget;
	int d,a,s,angle;
	VangerFunctionType* g;
	Vector v;

	GetVisible();

//zmod 1.21
	if (NetworkON && zStatus==1)
		Visibility = UNVISIBLE;

	if(Visibility == VISIBLE){
		switch(Mode){
			case FISH_WARRIOR_PATROL:
				if(TargetObject && TargetObject->ExternalDraw && TargetObject->draw_mode == NORMAL_DRAW_MODE && !(TargetObject->BeebonationFlag)){
					if(AttackTime > 0){
						vTarget = Vector(getDistX(TargetObject->R_curr.x,R_curr.x),getDistY(TargetObject->R_curr.y,R_curr.y),TargetObject->R_curr.z - R_curr.z);
						AttackTime--;
					}else{
						if(!RND(15)){
							if(!RND(4)){
								vTarget = Vector(BMAX_TARGET_VECTOR - RND(BMAX_TARGET_VECTOR2),BMAX_TARGET_VECTOR - RND(BMAX_TARGET_VECTOR2),BMAX_TARGET_VECTOR - RND(BMAX_TARGET_VECTOR2));
								Speed = RND(MaxSpeed >> 2);
							};
						}else{
							AttackTime = FISH_WARRIOR_ATTACK_TIME;
							Speed = MaxSpeed;
						};
					};
				}else{
					if(!RND(50)){
						vTarget = Vector(BMAX_TARGET_VECTOR - RND(BMAX_TARGET_VECTOR2),BMAX_TARGET_VECTOR - RND(BMAX_TARGET_VECTOR2),BMAX_TARGET_VECTOR - RND(BMAX_TARGET_VECTOR2));
						Speed = RND(MaxSpeed);
					};
					TargetObject = ActD.Active;
				};
				break;
			case FISH_WARRIOR_ATTACK:
				if(TargetObject && TargetObject->ExternalDraw && TargetObject->draw_mode == NORMAL_DRAW_MODE && !(TargetObject->BeebonationFlag)) vTarget = Vector(getDistX(TargetObject->R_curr.x,R_curr.x),getDistY(TargetObject->R_curr.y,R_curr.y),TargetObject->R_curr.z - R_curr.z);
				else{
					TargetObject = ActD.Active;
					if(!RND(50)){
						vTarget = Vector(BMAX_TARGET_VECTOR - RND(BMAX_TARGET_VECTOR2),BMAX_TARGET_VECTOR - RND(BMAX_TARGET_VECTOR2),BMAX_TARGET_VECTOR - RND(BMAX_TARGET_VECTOR2));
						Speed = RND(MaxSpeed);
					};
				};
				break;
			case FISH_WARRIOR_NONE:
				if(!RND(50)){
					vTarget = Vector(BMAX_TARGET_VECTOR - RND(BMAX_TARGET_VECTOR2),BMAX_TARGET_VECTOR - RND(BMAX_TARGET_VECTOR2),BMAX_TARGET_VECTOR - RND(BMAX_TARGET_VECTOR2));
					Speed = RND(MaxSpeed);
				};
				break;
		};

		d = vTarget.vabs();
		if(d){
			vDelta += vTarget * Precision / d;
			d = vDelta.vabs();
			if(d > Speed) vDelta = vDelta * Speed / d;
		};

		g = ActD.fTail;
		while(g){
			if(g->ID == PROTRACTOR_PALLADIUM){
				v = Vector(getDistX(R_curr.x,g->vR.x),getDistY(R_curr.y,g->vR.y),R_curr.z - g->vR.z);
				d = v.vabs();
				if(d < PALLADIUM_RADIUS)
					impulse(v,30 * (PALLADIUM_RADIUS - d) / PALLADIUM_RADIUS,0);
				break;
			};
			g = g->Next;
		};

		analysis();

		RotMat = A_l2g*DBM(PI/2,Z_AXIS);
		angle = rPI((int)RTOG(atan2(RotMat.a[1],RotMat.a[0])));
		s = (int)V.y;

		if(Speed == s){
			if(Speed != MaxSpeed)
				Speed = MaxSpeed;
		};

		a = rPI(vDelta.psi() - angle);
		if(a > PI) a -= 2*PI;
		d = Speed - s;

		if(a > 0){
			if(a > MECHOS_ROT_DELTA) a = MECHOS_ROT_DELTA;
		}else{
			if(a < -MECHOS_ROT_DELTA) a = -MECHOS_ROT_DELTA;
		};

		if(d > 0) controls(CONTROLS::TRACTION_INCREASE,83);
		else if(d < 0) controls(CONTROLS::TRACTION_DECREASE,83);

		controls(CONTROLS::STEER_BY_ANGLE,-a);
	};

//zmod 1.21
	if (NetworkON) {
		if(Time-- <= 0) {
			switch (zStatus) {
				case 0:
					CreateDestroyEffect(R_curr,MAP_POINT_CRATER09,DT_DEFORM02,DEFORM_WATER_ONLY,EFF_EXPLOSION01,-1);
					Time = 30;
					zStatus = 1;
					break;
				case 1:
					R_curr = Vector(RND(map_size_x),RND(map_size_y),10);
					Time = 900 + RND(900);
					zStatus = 0;
					break;
				}
		}
	}

};

void FishWarrior::DrawQuant(void)
{
	draw();
};

void FishWarrior::CreateFish(Vector v,int _Speed,int Angle,int _Precision,int _Time,int _Mode,int _Model,VangerUnit* p,int _Power)
{
	MaxSpeed = Speed = _Speed;
	Precision = _Precision;
	Time = _Time;
	Mode = _Mode;
	vDelta = Vector(Speed,0,0)*DBM(Angle,Z_AXIS);

	Object::operator = (ModelD.ActiveModel(_Model));
	R_curr = v;
	cycleTor(R_curr.x,R_curr.y);
	GetVisible();
	set_active(0);
	switch_analysis(0);
	Status = 0;
	set_body_color(COLORS_IDS::BODY_RED);
	set_3D(SET_3D_DIRECT_PLACE,R_curr.x,R_curr.y,R_curr.z,0,0,0);
	TargetObject = p;
	AttackTime = 0;
	Power = _Power;
//	ActD.FishWarriorNum++;
};

void FishWarrior::Touch(GeneralObject *p)
{
/* //zmod. as it was
	if(p->ID == ID_VANGER) {
		if(((VangerUnit*)(p))->BeebonationFlag) return;
		((VangerUnit*)(p))->BulletCollision(Power,NULL);
		CreateDestroyEffect(R_curr,MAP_POINT_CRATER09,DT_DEFORM02,DEFORM_WATER_ONLY,EFF_EXPLOSION01,-1);
		Status |= SOBJ_DISCONNECT;
	}
*/	

	if(p->ID != ID_VANGER) return;
	if(((VangerUnit*)(p))->BeebonationFlag) return;
	((VangerUnit*)(p))->BulletCollision(Power,NULL);
	CreateDestroyEffect(R_curr,MAP_POINT_CRATER09,DT_DEFORM02,DEFORM_WATER_ONLY,EFF_EXPLOSION01,-1);

	if (!NetworkON) {
		Status |= SOBJ_DISCONNECT;
	} else {
		Time = 30;
		zStatus = 1;
	}
};

int FishWarrior::test_objects_collision()
{
	return 0;
};

const int HORDE_PARTICLE_NUM = 200;
const int HORDE_RADIUS_DELTA = 30;

extern int AdvancedView;

void HordeSource::Quant(void)
{
	if(Status & SOBJ_DISCONNECT) return;
	GetVisible();
	if(Visibility == VISIBLE) analysis();
	Time = rPI(Time + PI / 8);
};

void HordeSource::DrawQuant(void)
{
	if(Status & SOBJ_DISCONNECT) return;
//	draw();	
	scale_size = original_scale_size + original_scale_size*Sin(Time) / 8.;
	pixel_draw();	
};

void HordeSource::Touch(GeneralObject* p)
{
	if(Status & SOBJ_DISCONNECT) return;
	switch(p->ID){
		case ID_VANGER:
		case ID_BULLET:
		case ID_JUMPBALL:
//			EffD.CreateExplosion(R_curr + Vector(0,0,radius),EFF_EXPLOSION01,this,1 << 15,0);
			if(p->Status & SOBJ_ACTIVE)
				uvsCheckKronIventTabuTask(UVS_KRON_EVENT::HIVE,1);
			Status |= SOBJ_DISCONNECT;
			break;
	};
};

int HordeSource::test_objects_collision()
{
	BaseObject* p;

	p = (BaseObject*)(BulletD.Tail);
	while(p){
//		if(p->ID != ID_BULLET)
//			ErrH.Abort("Bad Bullet List");

		if(p->Visibility == VISIBLE)
			test_object_to_baseobject(p);
		p = (BaseObject*)(p->NextTypeList);
	};

	p = (BaseObject*)(JumpD.Tail);
	while(p){
//		if(p->ID != ID_JUMPBALL)
//			ErrH.Abort("Bad JumpBall List");

		if(p->Visibility == VISIBLE)
			test_object_to_baseobject(p);
		p = (BaseObject*)(p->NextTypeList);
	};
	return 0;
};

void HordeSource::CreateSource(Vector v,int nModel,int r,int z)
{
	ID = ID_HORDE_SOURCE;
	Object::operator = (ModelD.ActiveModel(nModel));
	R_curr = v;
	cycleTor(R_curr.x,R_curr.y);
	GetVisible();
	set_active(0);
	switch_analysis(1);
	Status = 0;
	set_body_color(COLORS_IDS::BODY_RED);
	set_3D(SET_3D_DIRECT_PLACE,R_curr.x,R_curr.y,R_curr.z,0,0,0);
	HordeRadius =  r;
	zHorde = z;
	Time = 0;
	
	DataID = nModel;
};


void HordeObject::Init(void)
{
	BaseObject::Init();
	Data = new SimpleParticleType[HORDE_PARTICLE_NUM];
};

void HordeObject::Free(void)
{
	delete[] Data;
};

void HordeObject::Quant(void)
{
	int lv;
	int i;
	int phi;
	int radius8,r;
	VangerUnit* n;

	SimpleParticleType* p;
	Vector vTarget;
	int d,md,l;
	int update_log = 0;

	VangerFunctionType* g;
	Vector v;

	if(Status & SOBJ_DISCONNECT) return;

	lv = Visibility;
	GetVisible();

	if(Visibility == VISIBLE){
		if(lv == UNVISIBLE && Mode == HORDE_ATTACK_MODE)
			Mode = HORDE_RESTORE_MODE;

		if(Owner){
			Time--;
			if(Time <= 0) Status |= SOBJ_DISCONNECT;
		};

		switch(Mode){
			case HORDE_RESTORE_MODE:
				vDelta = Vector(0,0,0);
				radius8 = radius << 8;
				vTarget = R_curr << 8;
				for(i = 0,p = Data;i < NumParticle;i++,p++){
					r = RND(radius8);
					phi = rPI(RND(2*PI));
					p->vR = vTarget + Vector(round(Cos(phi)*r),round(Sin(phi)*r),0);
					p->vD = Vector(0,0,0);
					p->Color = 1;
				};
				Mode = HORDE_ATTACK_MODE;
				break;
			case HORDE_ATTACK_MODE:
				if(TargetObject){
					if(TargetObject->Status & SOBJ_DISCONNECT){
						TargetObject = NULL;
						update_log = 1;
					}else{
						g = ActD.fTail;
						while(g){
							if(g->ID == PROTRACTOR_PALLADIUM){
								v = Vector(getDistX(R_curr.x,g->vR.x),getDistY(R_curr.y,g->vR.y),R_curr.z - g->vR.z);
								d = v.vabs();
								if(d < PALLADIUM_RADIUS){
									vDelta = v * Speed;
									vDelta /= d;
								};
								break;
							};
							g = g->Next;
						};

						if(!g){
							if(!(TargetObject->dynamic_state & (GROUND_COLLISION | WHEELS_TOUCH | TOUCH_OF_WATER)) && TargetObject && TargetObject->ExternalDraw && TargetObject->draw_mode == NORMAL_DRAW_MODE && !(TargetObject->BeebonationFlag)){
								vTarget = Vector(getDistX(vZone.x,R_curr.x),getDistY(vZone.y,R_curr.y),vZone.z - R_curr.z);
								d = vTarget.vabs();
								if(d > AttackRadius){
									TargetObject = NULL;
									update_log = 1;
								}else
									vTarget = Vector(getDistX(TargetObject->R_curr.x,R_curr.x),getDistY(TargetObject->R_curr.y,R_curr.y),TargetObject->R_curr.z - R_curr.z);
							}else{
								vTarget = Vector(getDistX(vZone.x,R_curr.x),getDistY(vZone.y,R_curr.y),vZone.z - R_curr.z);
								TargetObject = NULL;
								update_log = 1;
							};
							
							d = vTarget.vabs();
							if(d){
								vDelta += vTarget*Precision / d;
								d = vDelta.vabs();
								if(d > Speed) vDelta = vDelta * Speed / d;
							};						
						};

						R_curr += vDelta;
						cycleTor(R_curr.x,R_curr.y);
					};
				}else{
					g = ActD.fTail;
					while(g){
						if(g->ID == PROTRACTOR_PALLADIUM){
							v = Vector(getDistX(R_curr.x,g->vR.x),getDistY(R_curr.y,g->vR.y),R_curr.z - g->vR.z);
							d = v.vabs();
							if(d < PALLADIUM_RADIUS){
								vDelta = v * Speed;
								vDelta /= d;
								R_curr += vDelta;
								cycleTor(R_curr.x,R_curr.y);
							};								
							break;
						};
						g = g->Next;
					};

					if(!g){
						vTarget = Vector(getDistX(vZone.x,R_curr.x),getDistY(vZone.y,R_curr.y),vZone.z - R_curr.z);

						d = vTarget.vabs();
						if(d){
							vDelta += vTarget*Precision / d;
							d = vDelta.vabs();
							if(d > Speed) vDelta = vDelta * Speed / d;
						};

						R_curr += vDelta;
						cycleTor(R_curr.x,R_curr.y);

						md = AttackRadius - radius;
						n = (VangerUnit*)(ActD.Tail);
						while(n){
							if(((VangerUnit*)(n))->Visibility == VISIBLE){
								d = getDistY(R_curr.y,n->R_curr.y);
								if(d < AttackRadius){
									if(!(n->dynamic_state & (GROUND_COLLISION | WHEELS_TOUCH | TOUCH_OF_WATER)) && n->ExternalDraw && n->draw_mode == NORMAL_DRAW_MODE && !(n->BeebonationFlag)){
										l  = getDistX(R_curr.x,n->R_curr.x);
										if(l < AttackRadius){
											l = round(sqrt(l*(double)l + d*(double)d));
											if(l < md){
												md = l;
												TargetObject = n;
												update_log = 1;
											};
										};
									};
								};
							};
							n = (VangerUnit*)(n->NextTypeList);
						};					
					};
				};
				break;
		};
	}else{
		if(Mode == HORDE_ATTACK_MODE){
			if(TargetObject){
				if(TargetObject->Status & SOBJ_DISCONNECT){
					TargetObject = NULL;
					update_log = 1;
				}else{						
					if(!(TargetObject->dynamic_state & (GROUND_COLLISION | WHEELS_TOUCH | TOUCH_OF_WATER)) && TargetObject && TargetObject->ExternalDraw && TargetObject->draw_mode == NORMAL_DRAW_MODE && !(TargetObject->BeebonationFlag)){
						vTarget = Vector(getDistX(vZone.x,R_curr.x),getDistY(vZone.y,R_curr.y),vZone.z - R_curr.z);
						d = vTarget.vabs();
						if(d > AttackRadius){
							TargetObject = NULL;
							update_log = 1;
						}else
							vTarget = Vector(getDistX(TargetObject->R_curr.x,R_curr.x),getDistY(TargetObject->R_curr.y,R_curr.y),TargetObject->R_curr.z - R_curr.z);
					}else{
						vTarget = Vector(getDistX(vZone.x,R_curr.x),getDistY(vZone.y,R_curr.y),vZone.z - R_curr.z);
						TargetObject = NULL;
						update_log = 1;
					};

					d = vTarget.vabs();
					if(d){
						vDelta += vTarget*Precision / d;
						d = vDelta.vabs();
						if(d > Speed) vDelta = vDelta * Speed / d;
					}else vDelta = Vector(0,0,0);

					R_curr += vDelta;
					cycleTor(R_curr.x,R_curr.y);					
				};
			}else{
				vTarget = Vector(getDistX(vZone.x,R_curr.x),getDistY(vZone.y,R_curr.y),vZone.z - R_curr.z);

				d = vTarget.vabs();
				if(d){
					vDelta += vTarget*Precision / d;
					d = vDelta.vabs();
					if(d > Speed) vDelta = vDelta * Speed / d;
				}else vDelta = Vector(0,0,0);

				R_curr += vDelta;
				cycleTor(R_curr.x,R_curr.y);
			};
		};
	};
};

void HordeObject::DrawQuant(void)
{
	int i;
	SimpleParticleType* p;
	Vector vPos;
	int tx,ty;

	//std::cout<<"HordeObject::DrawQuant "<<ActD.Active<<std::endl;
	if(Status & SOBJ_DISCONNECT) return;
	
	if(ActD.Active)
		SOUND_HORDE(getDistX(ActD.Active->R_curr.x,R_curr.x));

	if(AdvancedView){
		for(i = 0,p = Data;i < NumParticle;i++,p++){
			p->QuantP(R_curr << 8, vDelta << 8,3 << 8,5);
			vPos = p->vR;
			vPos >>= 8;
			G2LQ(vPos.x,vPos.y,vPos.z,tx,ty);
			if(tx > UcutLeft && tx < UcutRight && ty > VcutUp && ty < VcutDown) XGR_SetPixelFast(tx,ty,p->Color >> 8);
		};
	}else{
		if(CurrentWorld < MAIN_WORLD_MAX - 1){
			for(i = 0,p = Data;i < NumParticle;i++,p++){
				p->QuantP(R_curr << 8, vDelta << 8,3 << 8,5);
				tx = ((int)round(SPGetDistX(p->vR.x,SPViewX) * ScaleMapInvFlt) >> 8) + ScreenCX;
				ty = ((int)round((p->vR.y - SPViewY) * ScaleMapInvFlt) >> 8)+ ScreenCY;
				if(tx > UcutLeft && tx < UcutRight && ty > VcutUp && ty < VcutDown) XGR_SetPixelFast(tx,ty,p->Color >> 8);
			};
		}else{
			for(i = 0,p = Data;i < NumParticle;i++,p++){
				p->QuantP(R_curr << 8, vDelta << 8,3 << 8,5);
				tx = ((int)round(SPGetDistX(p->vR.x,SPViewX) * ScaleMapInvFlt) >> 8) + ScreenCX;
				ty = ((int)round(SPGetDistY(p->vR.y,SPViewY) * ScaleMapInvFlt) >> 8) + ScreenCY;
				if(tx > UcutLeft && tx < UcutRight && ty > VcutUp && ty < VcutDown) XGR_SetPixelFast(tx,ty,p->Color >> 8);
			};
		};
	};
};

void HordeObject::CreateHorde(Vector v,int r,int z,int cZ,VangerUnit* own)
{
	ID = ID_HORDE;
	Speed = 10;
	Precision = 7;
	Power = (50 << 16) / UnitGlobalTime;
	Mode = HORDE_RESTORE_MODE;
	NumParticle = HORDE_PARTICLE_NUM;
	vDelta = Vector(0,0,0);
	R_curr = v;
	cycleTor(R_curr.x,R_curr.y);
	GetVisible();
	Status = 0;
	TargetObject = NULL;
	radius =  r;
	AttackRadius = 200;
	zAttackRadius = z;
	Owner = own;
	zCruiser = cZ + radius;
	vZone = R_curr;
	Time = 150;
};

void HordeObject::Touch(GeneralObject* p)
{
	if(p->ID == ID_VANGER){
		if(((VangerUnit*)(p))->BeebonationFlag) return;
		((VangerUnit*)(p))->BulletCollision(Power,NULL);
	};
};
   
BulletObject* BulletList::CreateBullet(void)
{
	BulletObject* p;
	p = new BulletObject;
	p->Init();
	ConnectTypeList(p);
	return p;
};

void BulletList::FreeUnit(GeneralObject* p)
{
	if(((BulletObject*)(p))->LightData){
		((BulletObject*)(p))->LightData->Destroy();
		((BulletObject*)(p))->LightData = NULL;
	};
	DisconnectTypeList(p);
	delete p;
};

void ClefList::Init(void)
{
	int i;
	if(CurrentWorld == WORLD_HMOK){
		Data = new ClefObject[MAX_CLEF_OBJECT];
		for(i = 0;i < MAX_CLEF_OBJECT;i++){
			Data[i].Init();
			Data[i].CreateClef();
			ConnectTypeList(&Data[i]);
		};
	}else Data = NULL;
};

void ClefList::FreeUnit(GeneralObject* p)
{
	DisconnectTypeList(p);
};

void ClefList::Free(void)
{
	UnitBaseListType::Free();
	if(Data) delete[] Data;
};

void SkyFarmerList::Init(void)
{
	int i;
	uvsElement* l;
	char sign[8];	
	int FlyCount,CheckFlyCount;
	XStream ff;

	ff.open("resource/crypts/skyfarmer.vlc",XS_IN);
	ff.read(sign,strlen("VLS1"));
	if(memcmp(sign,"VLS1",strlen("VLS1"))) ErrH.Abort("Bad SkyFarmer Way Point Format");
	ff > SkyFarmerWayTableSize;
	SkyFarmerWayTable = new SensorDataType[SkyFarmerWayTableSize];
	FlyCount = 0;
	CheckFlyCount = 0;
	for(i = 0;i < SkyFarmerWayTableSize;i++){
		SkyFarmerWayTable[i].CreateSensor(ff);
		if(SkyFarmerWayTable[i].SensorType < 0 && SkyFarmerWayTable[i].SensorType >= SkyFarmerWayTableSize)
			ErrH.Abort("Bad Sky Farmer Object");
		FlyCount += SkyFarmerWayTable[i].SensorType;
		CheckFlyCount += i;
//		if(FlyCount != CheckFlyCount)
//			ErrH.Abort("GASDHgJGDS");
	};

	UnitBaseListType::Init();

	l = WorldTable[GameD.cWorld]->Panymal;
	while(l){
		addFarmer(((uvsFlyFarmer*)(l))->pos_x,((uvsFlyFarmer*)(l))->pos_y,((uvsFlyFarmer*)(l))->x_speed,((uvsFlyFarmer*)(l))->y_speed,((uvsFlyFarmer*)(l))->corn_type,((uvsFlyFarmer*)(l))->corn,((uvsFlyFarmer*)(l))->timer);
		l = l->enext;
	};
	FreeFarmer(WorldTable[CurrentWorld]->Panymal);
};

void SkyFarmerList::Free(void)
{
	int i;
	uvsFlyFarmer* l;
	GeneralObject* p;
	GeneralObject* pp;

//	FreeFarmer(WorldTable[GameD.cWorld]->Panymal);	
	p = Tail;
	while(p){
		pp = p->NextTypeList;		
		l = new uvsFlyFarmer(((SkyFarmerObject*)(p))->CornType,WorldTable[CurrentWorld]);
//		l->timer = ((SkyFarmerObject*)(p))->Timer;
		l->timer = 0;
//		l->corn = ((SkyFarmerObject*)(p))->Corn;
		l->corn = 25;
		l->x_speed = ((SkyFarmerObject*)(p))->xSpeed;
		l->y_speed = ((SkyFarmerObject*)(p))->ySpeed;
		l->pos_x = ((SkyFarmerObject*)(p))->R_curr.x;
		l->pos_y = ((SkyFarmerObject*)(p))->R_curr.y;
		l -> elink(WorldTable[CurrentWorld]->Panymal);
		FreeUnit(p);
		p = pp;
	};

	for(i = 0;i < SkyFarmerWayTableSize;i++)
		SkyFarmerWayTable[i].Close();
	delete[] SkyFarmerWayTable;
};

SkyFarmerObject* SkyFarmerList::CreateSkyFarmer(void)
{
	SkyFarmerObject* p;
	p = new SkyFarmerObject;
	p->Init();
	ConnectTypeList(p);
	return p;
};

void HordeList::Init(void)
{
	int i,max;
//zmod 1.21
	if(NetworkON){
		double kmax = (1.-zMod_flood_level_delta)/2;
		switch(CurrentWorld){
			case WORLD_FOSTRAL:
			case WORLD_GLORX:
			case WORLD_NECROSS:
				max = MAX_HORDE_OBJECT;
				break;
			default:
				max = MAX_HORDE_SOURCE_OBJECT / 2;
		};
		max = (int)((double)max * kmax);
		for(i = 0;i < max;i++)
			(CreateHorde())->CreateHorde(Vector(NetRnd.Get(map_size_x),NetRnd.Get(map_size_y),270),20,240,400,NULL);
	}else{
		switch(CurrentWorld){
			case WORLD_WEEXOW:
			case WORLD_ARKONOY:		
				for(i = 0;i < MAX_HORDE_OBJECT;i++)
					(CreateHorde())->CreateHorde(Vector(RND(map_size_x),RND(map_size_y),270),20,240,400,NULL);
				break;
		};
	};
};

HordeObject* HordeList::CreateHorde(void)
{
	HordeObject* p;
	p = new HordeObject;
	p->Init();
	ConnectTypeList(p);
	return p;
};

void HordeSourceList::Init(void)
{
	int i,max;	
	UnitBaseListType::Init();
//zmod 1.21 
	if(NetworkON){
		double kmax = (1.+zMod_flood_level_delta)/2;
		switch(CurrentWorld){
			case WORLD_FOSTRAL:
			case WORLD_GLORX:
			case WORLD_NECROSS:
				max = MAX_HORDE_OBJECT;
				break;
			default:
				max = MAX_HORDE_SOURCE_OBJECT / 2;
		};
		max = (int)((double)max * kmax);
		for(i = 0;i < max;i++)
			(CreateSource())->CreateSource(Vector(NetRnd.Get(map_size_x),NetRnd.Get(map_size_y),270),ModelD.FindModel("Horde"),20,60);
	}else{
		switch(CurrentWorld){
			case WORLD_FOSTRAL:
			case WORLD_GLORX:
			case WORLD_NECROSS:
			case WORLD_XPLO:
				for(i = 0;i < MAX_HORDE_SOURCE_OBJECT;i++)
					(CreateSource())->CreateSource(Vector(RND(map_size_x),RND(map_size_y),270),ModelD.FindModel("Horde"),20,60);
				break;
		};
	};
};

HordeSource* HordeSourceList::CreateSource(void)
{
	HordeSource* p;
	p = new HordeSource;
	p->Init();
	ConnectTypeList(p);
	return p;
};

void HordeSourceList::Quant(void)
{
	GeneralObject* p;
	GeneralObject* pp;
	p = Tail;
	while(p){
		p->Quant();
		p = p->NextTypeList;
	};

	p = Tail;
	while(p){
		pp = p->NextTypeList;
		if(p->Status & SOBJ_DISCONNECT){
			(HordeD.CreateHorde())->CreateHorde(p->R_curr,((HordeSource*)(p))->HordeRadius,((HordeSource*)(p))->zHorde,400,NULL);
			FreeUnit(p);
		};
		p = pp;
	};
};

void FishWarriorList::Init(void)
{
/* 
//zmod 1.17 as it was before
	int i;
	if(CurrentWorld == WORLD_WEEXOW && !NetworkON){
		Data = new FishWarrior[MAX_FISH_WARRIOR];
		for(i = 0;i < MAX_FISH_WARRIOR;i++){
			Data[i].Init();
			Data[i].CreateFish(Vector(RND(map_size_x),RND(map_size_y),10),20,RND(2*PI),5,2000,FISH_WARRIOR_PATROL,ModelD.FindModel("FishWarrior"),NULL,100 << 16);
			ConnectTypeList(&Data[i]);
		};
	}else Data = NULL;
*/
	int i;
	if(!NetworkON){
		if(CurrentWorld == WORLD_WEEXOW){
			Data = new FishWarrior[MAX_FISH_WARRIOR];
			for(i = 0;i < MAX_FISH_WARRIOR;i++){
				Data[i].Init();
				Data[i].CreateFish(Vector(RND(map_size_x),RND(map_size_y),10),20,RND(2*PI),5,2000,FISH_WARRIOR_PATROL,ModelD.FindModel("FishWarrior"),NULL,100 << 16);
				ConnectTypeList(&Data[i]);
			};
		} else Data = NULL;
	} else {
		int max = MAX_FISH_WARRIOR;
//zmod 1.21
		max /= 2;
		switch(CurrentWorld){
			case WORLD_GLORX:
				if (z_my_server_data.mod_id == Z_MODS_FORMULAV_ID) 
					max = 512;
			case WORLD_WEEXOW:
				if (z_my_server_data.mod_id == Z_MODS_NEPTUN_ID) //zmod 1.20 neptun fix
					max = 2;
				Data = new FishWarrior[max];
				for(i = 0;i < max;i++){
					Data[i].Init();
					Data[i].CreateFish(
						Vector(NetRnd.Get(map_size_x),NetRnd.Get(map_size_y),10),
						20,
						NetRnd.Get(2*PI),
						5,
						1200 + RND(1200),
						FISH_WARRIOR_PATROL,
						ModelD.FindModel("FishWarrior"),
						NULL,
						100 << 16
					); //create fish
					ConnectTypeList(&Data[i]);
				}; //for
				break;
			default:
				Data = NULL;
				break;
		};//switch
	};//networkon
};

void FishWarriorList::FreeUnit(GeneralObject* p)
{
	DisconnectTypeList(p);
};

void FishWarriorList::Free(void)
{
	UnitBaseListType::Free();
	if(Data) delete[] Data;
};

DebrisObject* DebrisList::CreateDebris(void)
{
	DebrisObject* p;
	p = new DebrisObject;
	p->Init();
	ConnectTypeList(p);
	return p;
};

JumpBallObject* JumpBallList::CreateBall(void)
{
	JumpBallObject* p;
	p = new JumpBallObject;
	p->Init();
	ConnectTypeList(p);
	return p;
};

//XStream NetStuffLog("netstuff.log",XS_OUT);

void StuffObject::NetOwnerQuant(int impulse)
{
	StuffObject* p;
	VangerUnit* n;

	if(NetOwner){
		if(Owner){
			if(Owner->NetID != NetOwner){
				n = (VangerUnit*)(ActD.GetNetObject(NetOwner));
				if(n){
					Owner->CheckOutDevice(this);
					if(Owner->Status & SOBJ_ACTIVE){
						ActD.CheckDevice(this);
						aciRemoveItem(&ActIntBuffer);
					};
					Owner->DelDevice(this);
					Owner = n;
					DeviceIn();
					if(Owner->Status & SOBJ_ACTIVE){
						ActIntBuffer.slot = -1;
						aciSendEvent2actint(ACI_PUT_ITEM,&ActIntBuffer);
					};
					Status &= ~SOBJ_WAIT_CONFIRMATION;
				};
			}else Status &= ~SOBJ_WAIT_CONFIRMATION;
		}else{
			Owner = (VangerUnit*)(ActD.GetNetObject(NetOwner));
			if(Owner){
				DeviceIn();
				if(Owner->Status & SOBJ_ACTIVE){
					ActIntBuffer.slot = -1;
					aciSendEvent2actint(ACI_PUT_ITEM,&ActIntBuffer);
				};
				Status &= ~SOBJ_WAIT_CONFIRMATION;
			};
		};
	}else{
		if(Owner){
			p = (StuffObject*)(ItemD.Tail);
			while(p){
				if(p == this) break;
				p = (StuffObject*)(p->NextTypeList);
			};
			if(!p){
				Owner->CheckOutDevice(this);
				if(Owner->Status & SOBJ_ACTIVE){
					ActD.CheckDevice(this);
					aciRemoveItem(&ActIntBuffer);
				};
				DeviceOut(R_curr,OutFlag,Vector(xImpulse,yImpulse,0));
			}else{
				p = (StuffObject*)(Owner->DeviceData);
				while(p){
					if(p == this){
						Owner->CheckOutDevice(this);
						if(Owner->Status & SOBJ_ACTIVE){
							ActD.CheckDevice(this);
							aciRemoveItem(&ActIntBuffer);
						};
						Owner->DelDevice(this);
						break;
					};
					p = p->NextDeviceList;
				};
				Owner = NULL;
			};
			Status &= ~SOBJ_WAIT_CONFIRMATION;
		}else{
			if(impulse) ImpulseAction(R_curr,OutFlag,Vector(xImpulse,yImpulse,0));
			Status &= ~SOBJ_WAIT_CONFIRMATION;
		};
	};
};

void StuffObject::NetEvent(int type,int id,int creator,int x,int y)
{
	char ch;
	short st;
	int t;

	switch(type){
		case UPDATE_OBJECT:
			NETWORK_IN_STREAM > ch;
			NETWORK_IN_STREAM > t;
			if(GET_NETWORK_ID(id) == NID_DEVICE){
				NetDeviceID = id;
				NetID = t;
			}else{
				NetID = id;
				NetDeviceID = t;
				GetVisible();
			};

			NETWORK_IN_STREAM > st;
			R_curr.z = st;

			NETWORK_IN_STREAM > ch;
			ActIntBuffer.type = ch;
			NETWORK_IN_STREAM > ActIntBuffer.data0;
			NETWORK_IN_STREAM > ActIntBuffer.data1;

			NETWORK_IN_STREAM > ch;
			CreateMode = ch & 127;
			OutFlag = ch & 128;

			NETWORK_IN_STREAM > st;
			Time = st;

			SetStuffColor();

/*			NETWORK_IN_STREAM > st;
			xImpulse = st;
			NETWORK_IN_STREAM > st;
			yImpulse = st;

			R_curr.x = x;
			R_curr.y = y;*/

			NETWORK_IN_STREAM > st;
			R_curr.x = st;
			NETWORK_IN_STREAM > st;
			R_curr.y = st;

			xImpulse = x;
			yImpulse = y;

			cycleTor(R_curr.x,R_curr.y);

			NETWORK_IN_STREAM > NetOwner;
			if(!Owner && !NetOwner && Visibility == UNVISIBLE){
				R_curr.x = xImpulse;
				R_curr.y = yImpulse;
			};
//			if(Owner && !NetOwner) NetStuffLog < "\nBad Update Owner";
			Status |= SOBJ_WAIT_CONFIRMATION;
			NetOwnerQuant(1);
			break;
		case CREATE_OBJECT:
			NETWORK_IN_STREAM > t;
			if(GET_NETWORK_ID(id) == NID_DEVICE){
				NetDeviceID = id;
				NetID = t;
			}else{
				NetID = id;
				NetDeviceID = t;
			};			

			ID = ID_STUFF;
			GetDevice(ItemD.DeviceTypeData[DataID]);
			Object::operator = (ModelD.ActiveModel(ModelID));
			Status = 0;
			set_active(0);

			CycleTime = 0;
			switch_analysis(0);
			LightData = NULL;

			NETWORK_IN_STREAM > st;
			R_curr.z = st;

			NETWORK_IN_STREAM > ch;
			ActIntBuffer.type = ch;
			NETWORK_IN_STREAM > ActIntBuffer.data0;
			NETWORK_IN_STREAM > ActIntBuffer.data1;

			NETWORK_IN_STREAM > ch;
			CreateMode = ch & 127;
			OutFlag = ch & 128;

			NETWORK_IN_STREAM > st;
			Time = st;

/*			NETWORK_IN_STREAM > st;
			xImpulse = st;
			NETWORK_IN_STREAM > st;
			yImpulse = st;

			R_curr.x = x;
			R_curr.y = y;
			cycleTor(R_curr.x,R_curr.y);*/

			NETWORK_IN_STREAM > st;
			R_curr.x = st;
			NETWORK_IN_STREAM > st;
			R_curr.y = st;

			R_curr.x = xImpulse = x;
			R_curr.y = yImpulse = y;

			NETWORK_IN_STREAM > NetOwner;

			SetStuffColor();

			Time = 0;
			FindDolly = 1;

			switch(ActIntBuffer.type){
				case  ACI_KERNOBOO:
				case ACI_PIPETKA:			
					archimedean = 255;
					break;
				default:
					archimedean = 0;
					break;
			};

			Owner = NULL;
			Status |= SOBJ_WAIT_CONFIRMATION;
			if(NetOwner){
				Owner = (VangerUnit*)(ActD.GetNetObject(NetOwner));
				if(Owner) Owner->AddDevice(this);
				else{
					ItemD.ConnectTypeList(this);
					GameD.ConnectBaseList(this);
				};
			}else{				
				ItemD.ConnectTypeList(this);
				GameD.ConnectBaseList(this);

				switch(CreateMode){
					case STUFF_CREATE_TRACK:
						set_3D(SET_3D_DIRECT_PLACE,R_curr.x,R_curr.y,R_curr.z,0,0,0);
						precise_impulse(R_curr,XCYCL(R_curr.x + MAX_STUFF_SCATTER - RND(2*MAX_STUFF_SCATTER)),YCYCL(R_curr.y + MAX_STUFF_SCATTER - RND(2*MAX_STUFF_SCATTER)));
						break;
					case STUFF_CREATE_RELAX:
						switch_analysis(1);
						set_3D(SET_3D_DIRECT_PLACE,R_curr.x,R_curr.y,R_curr.z,0,0,0);
						break;
					case STUFF_CREATE_NONE:
						set_3D(SET_3D_TO_THE_LOWER_LEVEL,R_curr.x,R_curr.y,0,radius/2,0,0);
						break;
					default:
						set_3D(SET_3D_DIRECT_PLACE,R_curr.x,R_curr.y,R_curr.z,0,0,0);
						break;
				};
				GetVisible();
				
//				R_curr.x = xImpulse;
//				R_curr.y = yImpulse;				
			};
//			if(Owner && !NetOwner) NetStuffLog < "\nBad Create Owner";
			break;
		case DELETE_OBJECT:
			if(NETWORK_IN_STREAM.current_body_size())
				NETWORK_IN_STREAM > ch;
			else
				ch = 0;
			if(!ch){
				if(Owner){
					Owner->CheckOutDevice(this);
					if(Owner->Status & SOBJ_ACTIVE){
						ActD.CheckDevice(this);
						aciRemoveItem(&ActIntBuffer);
					};
					Owner->DelDevice(this);
					Storage->Deactive(this);
				}else 
					Status |= SOBJ_DISCONNECT;
			}else Status |= SOBJ_WAIT_CONFIRMATION;
			break;		
	};
};

void ItemsDispatcher::NetDevice(int type,int id)
{
	StuffObject* p;
	VangerUnit* v;
	uchar t;

	p = NULL;
	v = (VangerUnit*)(ActD.Tail);
	while(v){
		p = v->DeviceData;
		while(p){
			if(p->NetDeviceID == id) break;
			p = p->NextDeviceList;
		};
		if(p) break;
		v = (VangerUnit*)(v->NextTypeList);
	};

	if(p){
		if(p->Status & SOBJ_DISCONNECT)	NETWORK_IN_STREAM.ignore_event();
		else p->NetEvent(type,id,NETWORK_IN_STREAM.current_creator(),NETWORK_IN_STREAM.current_x(),NETWORK_IN_STREAM.current_y());
	}else{
		p = (StuffObject*)(ItemD.Tail);
		while(p){
			if(p->NetDeviceID == id) break;
			p = (StuffObject*)(p->NextTypeList);
		};
		
		if(p){
			if(p->Status & SOBJ_DISCONNECT)	NETWORK_IN_STREAM.ignore_event();
			else p->NetEvent(type,id,NETWORK_IN_STREAM.current_creator(),NETWORK_IN_STREAM.current_x(),NETWORK_IN_STREAM.current_y());
		}else{
			if(type == UPDATE_OBJECT){
				NETWORK_IN_STREAM > t;
				p = (StuffObject*)(ItemD.GetObject(DevicetStorageID[ItemD.DeviceTypeData[t]->StuffType]));
				if(p){
					p->DataID = t;
					p->NetEvent(CREATE_OBJECT,id,NETWORK_IN_STREAM.current_creator(),NETWORK_IN_STREAM.current_x(),NETWORK_IN_STREAM.current_y());
				}else NETWORK_IN_STREAM.ignore_event();
			}else NETWORK_IN_STREAM.ignore_event();
		};
	};
};

void ItemsDispatcher::NetEvent(int type,int id)
{
	StuffObject* p;
	uchar t;
	VangerUnit* v;

	p = (StuffObject*)(GetNetObject(id));
	if(p){
		if(p->Status & SOBJ_DISCONNECT)	NETWORK_IN_STREAM.ignore_event();
		else p->NetEvent(type,id,NETWORK_IN_STREAM.current_creator(),NETWORK_IN_STREAM.current_x(),NETWORK_IN_STREAM.current_y());		
	}else{
		v = (VangerUnit*)(ActD.Tail);
		while(v){
			p = v->DeviceData;
			while(p){
				if(p->NetID == id) break;
				p = p->NextDeviceList;
			};
			if(p) break;
			v = (VangerUnit*)(v->NextTypeList);
		};

		if(p){
			if(p->Status & SOBJ_DISCONNECT)	NETWORK_IN_STREAM.ignore_event();
			else p->NetEvent(type,id,NETWORK_IN_STREAM.current_creator(),NETWORK_IN_STREAM.current_x(),NETWORK_IN_STREAM.current_y());		
		}else{
			if(type == UPDATE_OBJECT){
				NETWORK_IN_STREAM > t;
				p = (StuffObject*)(ItemD.GetObject(DevicetStorageID[ItemD.DeviceTypeData[t]->StuffType]));
				if(p){
					p->DataID = t;
					p->NetEvent(CREATE_OBJECT,id,NETWORK_IN_STREAM.current_creator(),NETWORK_IN_STREAM.current_x(),NETWORK_IN_STREAM.current_y());
				}else NETWORK_IN_STREAM.ignore_event();
			}else NETWORK_IN_STREAM.ignore_event();
		};
	};
};

void DebrisObject::vExplosion(Object* parent,int debris_index)
{
	Object::operator = (*parent);
	n_models = 1;
	models = model = &(parent -> debris[debris_index]);
	
	m = density*model -> volume*pow(scale_size,3);
	J_inv = (model -> J*(pow(scale_size,2)/model -> volume)).inverse();
	body_color_shift++;
	k_archimedean = 0;

	xmax = model -> xmax;
	ymax = model -> ymax;
	zmax = model -> zmax;
	radius = rmax = model -> rmax;
};


int GloryPlaceNum;
GloryPlace* GloryPlaceData;
int UsedCheckNum;

aiRndType GloryRnd;

void GloryPlace::Init(int ind)
{
	ID = ID_GLORY_PLACE;
	Enable = 1;
	R_curr.z = 255;
	Status = 0;
	LightData = NULL;
	Index = ind;

	//zMod fixed

	//zNfo - GloryPlace
	
	//Formula-V
	if (z_my_server_data.mod_id == Z_MODS_FORMULAV_ID) {
		World = WORLD_GLORX;
		switch (ind % 4) {
		case 0:	R_curr.x = 1341;	R_curr.y = 5971;	break;
		case 1:	R_curr.x =  486;	R_curr.y = 5648;	break;
		case 2:	R_curr.x = 1607;	R_curr.y = 6315;	break;
		case 3:	R_curr.x =   80;	R_curr.y = 7281;	break;
		}
		return;
	} 
	
	//Trak-Trial
	if (z_my_server_data.mod_id == Z_MODS_TRAKTRIAL_ID) {
		World = WORLD_NECROSS;
		switch (ind % 2) {
		case 0:	R_curr.x =  620;	R_curr.y = 14771;	break;
		case 1:	R_curr.x = 1756;	R_curr.y = 2447;	break;
		}
		return;
	}
	
	//khoxrun
	if (z_my_server_data.mod_id == Z_MODS_KHOXRUN_ID) {
		World = WORLD_KHOX;
		switch (ind) {
		case  0:	R_curr.x = 1358;	R_curr.y = 7036;	World = WORLD_GLORX;	break; // НЕ МЕНЯТЬ !!! Связано с багом вылета клиента при смерти в пассе. если чек не на трех мирах.
		case  1:	R_curr.x =  390;	R_curr.y = 1266;	break;
		case  2:	R_curr.x = 1858;	R_curr.y = 1849;	break;
		case  3:	R_curr.x = 1869;	R_curr.y =  232;	break;
		case  4:	R_curr.x =  696;	R_curr.y =   86;	break;
		case  5:	R_curr.x = 1145;	R_curr.y = 1210;	break;
		case  6:	R_curr.x = 1987;	R_curr.y = 1043;	break;
		case  7:	R_curr.x = 2021;	R_curr.y =    8;	break;
		case  8:	R_curr.x =  150;	R_curr.y =  378;	break;
		case  9:	R_curr.x =  910;	R_curr.y = 2014;	break;
		case 10:	R_curr.x = 1232;	R_curr.y =  717;	break;
		case 11:	R_curr.x =  307;	R_curr.y =  385;	break;
		}
		return;
	}

	//classic
	if(ind == 0) {
		World = GloryRnd.aiRnd(3); // НЕ МЕНЯТЬ !!! Связано с багом вылета клиента при смерти в пассе. если чек не на трех мирах.
	} else {
		World = GloryRnd.aiRnd(WORLD_MAX);

		while (World == WORLD_HMOK)
			World = GloryRnd.aiRnd(WORLD_MAX);

		if(z_my_server_data.mod_id == Z_MODS_RAFARUN_ID ) //tarakan'i bega/ excludes hmok && threall
			while (World==WORLD_HMOK || World==WORLD_THREALL)
		World = GloryRnd.aiRnd(WORLD_MAX);
	};
	R_curr.x = GloryRnd.aiRnd(WorldTable[World]->x_size);
	if(World < MAIN_WORLD_MAX - 1)
		R_curr.y = 300 + GloryRnd.aiRnd(WorldTable[World]->y_size - 600);
	else	
		R_curr.y = GloryRnd.aiRnd(WorldTable[World]->y_size);
};

void GloryPlace::CloseWorld(void)
{
	if(LightData){
		LightData->Destroy();
		LightData = NULL;
	};
};

void GloryPlace::Quant(void)
{
	int i;
	int dx,dy;
	uchar** lt;
	if(World == CurrentWorld && Enable){
		lt = vMap->lineT;
		for(i = -GLORY_PLACE_RADIUS;i < GLORY_PLACE_RADIUS;i++){
			if(!lt[YCYCL(R_curr.y + i)]){
				if(LightData){
					LightData->Destroy();
					LightData = NULL;
				};
				return;
			};
		};
		
		if(ActD.Active){
			if(!LightData)
				LightData = MapD.CreateLight(R_curr.x,R_curr.y,255,GLORY_PLACE_RADIUS,32,LIGHT_TYPE::DYNAMIC | LIGHT_TYPE::TOR);

			dx = getDistX(ActD.Active->R_curr.x,R_curr.x);
			dy = getDistY(ActD.Active->R_curr.y,R_curr.y);
			if((dx*dx + dy*dy) < GLORY_PLACE_RADIUS*GLORY_PLACE_RADIUS && 
				((ActD.Active->dynamic_state & GROUND_COLLISION) || (ActD.Active->dynamic_state & TRACTION_WHEEL_TOUCH) || (ActD.Active->dynamic_state & STEER_WHEEL_TOUCH))){
					if(LightData){
						LightData->Destroy();
						LightData = NULL;
					};
					MapD.CreateLight(R_curr.x,R_curr.y,R_curr.z,GLORY_PLACE_RADIUS,32,LIGHT_TYPE::STATIONARY);
					for(i = 0;i < 5;i++)
						EffD.CreateDeform(Vector(XCYCL(R_curr.x + GLORY_PLACE_RADIUS - RND(2*GLORY_PLACE_RADIUS)),YCYCL(R_curr.y + GLORY_PLACE_RADIUS - RND(2*GLORY_PLACE_RADIUS)),255),DEFORM_ALL,PASSING_WAVE_PROCESS);
					Enable = 0;					
					UsedCheckNum++;
					NetStatisticUpdate(NET_STATISTICS_CHECKPOINT);
					if(UsedCheckNum >= GloryPlaceNum)
						NetStatisticUpdate(NET_STATISTICS_END_RACE);
					send_player_body(my_player_body);
					SOUND_SUCCESS();
			};		
		};
	};
};

extern aciPromptData aiMessageBuffer;
extern uvsTabuTaskType **TabuTable;
extern int iRussian;

void aiPromptTaskMessage(int l)
{
//	if(!uvsKronActive) return;
	aiMessageBuffer.align_type = 0;
	if(l >= 0){
		if(iRussian) aiMessageBuffer.add_str(0,(unsigned char*)rSuccessTaskMessageData);
		else aiMessageBuffer.add_str(0,(unsigned char*)SuccessTaskMessageData);
	}else{
		if(iRussian) aiMessageBuffer.add_str(0,(unsigned char*)rFailedTaskMessageData);
		else aiMessageBuffer.add_str(0,(unsigned char*)FailedTaskMessageData);
	};

//	aiMessageBuffer.add_str(0,(uchar*)(s));
	aiMessageBuffer.TimeBuf[0] = 200;
	aiMessageBuffer.ColBuf[0] = 143;

	RaceTxtBuff.init();
	if(iRussian) RaceTxtBuff < rTaskMessageLuck;
	else RaceTxtBuff < TaskMessageLuck;
	if(l > 0) RaceTxtBuff < " +";
	else RaceTxtBuff < " -";

	RaceTxtBuff <= abs(l);

	aiMessageBuffer.add_str(1,(uchar*)(RaceTxtBuff.GetBuf()));
	aiMessageBuffer.TimeBuf[1] = 200;
	aiMessageBuffer.ColBuf[1] = 143;

	aiMessageBuffer.NumStr = 2;
	aciSendPrompt(&aiMessageBuffer);
	ClearTabutaskTarget();
};

void aiPromptDominanceMessage(int d)
{
//	if(!uvsKronActive) return;
	RaceTxtBuff.init();
	if(iRussian) RaceTxtBuff < rDominanceMessageData;
	else RaceTxtBuff < DominanceMessageData;
	if(d > 0) RaceTxtBuff < " +";
	else RaceTxtBuff < " -";
	RaceTxtBuff <= abs(d);

	aiMessageBuffer.align_type = 0;
	aiMessageBuffer.TimeBuf[0] = 200;
	aiMessageBuffer.ColBuf[0] = 143;
	aiMessageBuffer.NumStr = 1;
	aiMessageBuffer.add_str(0,(uchar*)(RaceTxtBuff.GetBuf()));
	aciSendPrompt(&aiMessageBuffer);
};

void aiPromptLuckMessage(int d)
{
//	if(!uvsKronActive) return;
	RaceTxtBuff.init();

	if(iRussian) RaceTxtBuff < rTaskMessageLuck;
	else RaceTxtBuff < TaskMessageLuck;

	if(d > 0) RaceTxtBuff < " +";
	else RaceTxtBuff < " -";
	RaceTxtBuff <= abs(d);

	aiMessageBuffer.align_type = 0;
	aiMessageBuffer.TimeBuf[0] = 200;
	aiMessageBuffer.ColBuf[0] = 143;
	aiMessageBuffer.NumStr = 1;
	aiMessageBuffer.add_str(0,(uchar*)(RaceTxtBuff.GetBuf()));
	aciSendPrompt(&aiMessageBuffer);
};

void ShowTaskMessage(int l)
{
	if(NetworkON) return;
	aiMessageQueue.SendTabuTask(l);
};

void ShowDominanceMessage(int d) //znfo
{
//#ifndef ZMOD_BETA
	if(NetworkON) return; 
//#endif
	aiMessageQueue.SendDominance(d);
};

void ShowLuckMessage(int l) //znfo
{
//#ifndef ZMOD_BETA
	if(NetworkON) return; 
//#endif
	aiMessageQueue.SendLuck(l);
};

void StuffObject::SetStuffColor(void)
{
	int t;
	t = GetColorDevice(ActIntBuffer.type);
	if(t >= 0) set_body_color(t);
};

void ChangeItemData(int d)
{
	StuffObject* p;
	VangerUnit* n;

	p = (StuffObject*)(ItemD.Tail);
	while(p){
//		uvsChangeGoodsParam(p->uvsDeviceType,p->ActIntBuffer.data0,p->ActIntBuffer.data1,d);
		if(uvsChangeGoodsParam(p->uvsDeviceType,p->ActIntBuffer.data0,p->ActIntBuffer.data1,d)){
			p->ActIntBuffer.type = uvsSetItemType(p->uvsDeviceType,p->ActIntBuffer.data0,p->ActIntBuffer.data1);
			switch(p->ActIntBuffer.type){
				case ACI_TABUTASK_FAILED:
					SOUND_FAILED();
//					ShowTaskMessage(-TabuTable[p->ActIntBuffer.data1 & 0xffff]->luck);
					p->SetStuffColor();
					break;
				case ACI_TABUTASK_SUCCESSFUL:
					SOUND_SUCCESS();
//					ShowTaskMessage(TabuTable[p->ActIntBuffer.data1 & 0xffff]->luck);
					p->SetStuffColor();
					break;
			};
		};
		p = (StuffObject*)(p->NextTypeList);
	};

	n = (VangerUnit*)(ActD.Tail);
	while(n){
		p = n->DeviceData;
		while(p){
//			uvsChangeGoodsParam(p->uvsDeviceType,p->ActIntBuffer.data0,p->ActIntBuffer.data1,d);
			if(uvsChangeGoodsParam(p->uvsDeviceType,p->ActIntBuffer.data0,p->ActIntBuffer.data1,d)){
				p->ActIntBuffer.type = uvsSetItemType(p->uvsDeviceType,p->ActIntBuffer.data0,p->ActIntBuffer.data1);
				if(p->Owner->Status & SOBJ_ACTIVE) aciChangeItem(&(p->ActIntBuffer));
				switch(p->ActIntBuffer.type){
					case ACI_TABUTASK_FAILED:
						SOUND_FAILED();
//						ShowTaskMessage(-TabuTable[p->ActIntBuffer.data1 & 0xffff]->luck);						
						break;
					case ACI_TABUTASK_SUCCESSFUL:
						SOUND_SUCCESS();
//						ShowTaskMessage(TabuTable[p->ActIntBuffer.data1 & 0xffff]->luck);						
						break;
				};
			};
			p = p->NextDeviceList;
		};
		n = (VangerUnit*)(n->NextTypeList);
	};
};

void ChangeTabutaskItem(int type,int status)
{
	StuffObject* p;
	VangerUnit* n;
	
	p = (StuffObject*)(ItemD.Tail);
	while(p){
		if(p->uvsDeviceType == UVS_ITEM_TYPE::TABUTASK && uvsChangeTabuTask(p->ActIntBuffer.data0,p->ActIntBuffer.data1,type,status)){
			p->ActIntBuffer.type = uvsSetItemType(p->uvsDeviceType,p->ActIntBuffer.data0,p->ActIntBuffer.data1);
			switch(p->ActIntBuffer.type){
				case ACI_TABUTASK_FAILED:
					SOUND_FAILED();
//					ShowTaskMessage(-TabuTable[p->ActIntBuffer.data1 & 0xffff]->luck);
					p->SetStuffColor();
					break;
				case ACI_TABUTASK_SUCCESSFUL:
					SOUND_SUCCESS();
//					ShowTaskMessage(TabuTable[p->ActIntBuffer.data1 & 0xffff]->luck);
					p->SetStuffColor();
					break;
			};
		};
		p = (StuffObject*)(p->NextTypeList);
	};

	n = (VangerUnit*)(ActD.Tail);
	while(n){
		p = n->DeviceData;
		while(p){
			if(p->uvsDeviceType == UVS_ITEM_TYPE::TABUTASK && uvsChangeTabuTask(p->ActIntBuffer.data0,p->ActIntBuffer.data1,type,status)){
				p->ActIntBuffer.type = uvsSetItemType(p->uvsDeviceType,p->ActIntBuffer.data0,p->ActIntBuffer.data1);
				switch(p->ActIntBuffer.type){
					case ACI_TABUTASK_FAILED:
						SOUND_FAILED();
//						ShowTaskMessage(-TabuTable[p->ActIntBuffer.data1 & 0xffff]->luck);
						if(p->Owner->Status & SOBJ_ACTIVE) aciChangeItem(&(p->ActIntBuffer));
						break;
					case ACI_TABUTASK_SUCCESSFUL:
						SOUND_SUCCESS();
//						ShowTaskMessage(TabuTable[p->ActIntBuffer.data1 & 0xffff]->luck);
						if(p->Owner->Status & SOBJ_ACTIVE) aciChangeItem(&(p->ActIntBuffer));
						break;
				};
			};
			p = p->NextDeviceList;
		};
		n = (VangerUnit*)(n->NextTypeList);
	};
};

int aciGetScreenMechos(int x,int y)
{
//	BaseObject* p;
//	BaseObject* mp;

   	int tx,ty,dx,dy,l;
//	Vector vCheck;

//	mp = NULL;
	S2G(x,y,tx,ty);

//	vCheck = Vector(getDistX(tx,ViewX),getDistY(ty,ViewY),0);

/*	p = (BaseObject*)(ActD.Tail);
	while(p){
		if(p->Visibility == VISIBLE){
			dx = getDistX(tx,p->R_curr.x);
			dy = getDistY(ty,p->R_curr.y);
			l = p->radius + ACI_CHECK_RADIUS;
			if((dx*dx + dy*dy) < l*l){
				mp = p;
				return 1;
			};
		};
		p = (BaseObject*)(p->NextTypeList);
	};*/

	if(ActD.Active){
		dx = getDistX(tx,ActD.Active->R_curr.x);
		dy = getDistY(ty,ActD.Active->R_curr.y);
		l = ActD.Active->radius + ACI_CHECK_RADIUS;
		if((dx*dx + dy*dy) < l*l) return 1;
	};
	return 0;	
};

void GunDevice::CreateGun(void)
{
	PrevOwner = NULL;
	if(uvsDeviceType == UVS_ITEM_TYPE::AMPUTATOR || uvsDeviceType == UVS_ITEM_TYPE::DEGRADATOR || uvsDeviceType == UVS_ITEM_TYPE::MECHOSCOPE)
		set_body_color(COLORS_IDS::BODY_GRAY);
//	ActIntBuffer.data1 = 0;
//	ActIntBuffer.type = uvsSetItemType(uvsDeviceType,ActIntBuffer.data0,ActIntBuffer.data1);	
};

int ItemsDispatcher::CreateEnableCrypt(void)
{
	int i,sz;
	sz = 0;
	int rx,ry;
	ry = TurnSideY - ActD.Active->radius - ActD.Active->Speed;
	rx = TurnSideX - ActD.Active->radius - ActD.Active->Speed;
	for(i = 0;i < ProtoCryptTableSize[CurrentWorld];i++){
		if((ProtoCryptTable[CurrentWorld][i].SensorType == 2 || ProtoCryptTable[CurrentWorld][i].SensorType == 6 ||  ProtoCryptTable[CurrentWorld][i].SensorType == 1 || ProtoCryptTable[CurrentWorld][i].SensorType == 7)
		   && abs(getDistY(ProtoCryptTable[CurrentWorld][i].R_curr.y,ViewY)) < ry
		   && abs(getDistX(ProtoCryptTable[CurrentWorld][i].R_curr.x,ViewX)) < rx){
			sz++;
			ProtoCryptTable[CurrentWorld][i].Enable = 1;
		}else 
			ProtoCryptTable[CurrentWorld][i].Enable = 0;
	};
	return sz;
};

void ItemsDispatcher::CryptQuant(void)
{
	int i,j;
	listElem* p;
	uvsItem* n;

	if(ActD.Active && Num < Total / 4 && !RND(200*ActD.NumVisibleVanger)){
		if(CreateEnableCrypt() > NumVisibleItem){
			for(i = 0;i < ProtoCryptTableSize[CurrentWorld];i++){
				if(ProtoCryptTable[CurrentWorld][i].Enable){
					if(ProtoCryptTable[CurrentWorld][i].SensorType == 2 || ProtoCryptTable[CurrentWorld][i].SensorType == 6){
						for(j = 0;j < (int)(RND(ITEM_LUCK_MAX));j++){
							if(!RND(10))
								uvsCreateNewItem(ProtoCryptTable[CurrentWorld][i].R_curr.x + (ITEM_LUCK_RADIUS*CO[RND(PI*2)] >> 16),ProtoCryptTable[CurrentWorld][i].R_curr.y + (ITEM_LUCK_RADIUS*SI[RND(PI*2)] >> 16),ProtoCryptTable[CurrentWorld][i].R_curr.z,uvsGenerateItemForCrypt(6),CurrentWorld);
							else 
								uvsCreateNewItem(ProtoCryptTable[CurrentWorld][i].R_curr.x + (ITEM_LUCK_RADIUS*CO[RND(PI*2)] >> 16),ProtoCryptTable[CurrentWorld][i].R_curr.y + (ITEM_LUCK_RADIUS*SI[RND(PI*2)] >> 16),ProtoCryptTable[CurrentWorld][i].R_curr.z,uvsGenerateItemForCrypt(2),CurrentWorld);
						};
					}else{
						if(ProtoCryptTable[CurrentWorld][i].SensorType == 1 || ProtoCryptTable[CurrentWorld][i].SensorType == 7)
							uvsCreateNewItem(ProtoCryptTable[CurrentWorld][i].R_curr.x,ProtoCryptTable[CurrentWorld][i].R_curr.y,ProtoCryptTable[CurrentWorld][i].R_curr.z,uvsGenerateItemForCrypt(ProtoCryptTable[CurrentWorld][i].SensorType),CurrentWorld);
					};
				};
			};
		};

		p = WorldTable[GameD.cWorld]->Pitem;
		while(p){
			n = (uvsItem*)(p);
			addDevice(n->pos_x,n->pos_y,n->pos_z,n->type,n->param1,n->param2,NULL);
			p = p->next;
		};
		FreeList(WorldTable[CurrentWorld]->Pitem);
	};
};
