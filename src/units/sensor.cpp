#include "../global.h"

//#include "..\win32f.h"

#include "../3d/3d_math.h"
#include "../3d/3dgraph.h"
#include "../3d/3dobject.h"
#include "../3d/parser.h"

#include "../common.h"
#include "../sqexp.h"
#include "../backg.h"

#include "../sound/hsound.h"

#include "../terra/vmap.h"
#include "../terra/world.h"
#include "../terra/render.h"

#include "../actint/item_api.h"
#include "uvsapi.h"
#include "../network.h"

#include "../particle/particle.h"
#include "../particle/partmap.h"
#include "../particle/df.h"
#include "../particle/light.h"

#include "../uvs/univang.h"

#include "../dast/poly3d.h"

#include "track.h"
#include "hobj.h"
#include "moveland.h"
#include "items.h"
#include "sensor.h"
#include "effect.h"
#include "mechos.h"

//#define SAVE_TNT_DATA

const int  TNT_WAIT_TIME = 2;

extern listElem* EscaveTail;
extern listElem* SpotTail;
extern listElem* PassageTail;

const int TNT_CRATER_RADIUS = 45;

#ifdef NEW_TNT
TntCreature** TntObjectData;
TntCreature** TntSortedData;
#else
TntStaticObject** TntObjectData;
TntStaticObject** TntSortedData;
#endif

SensorDataType** SensorObjectData;
SensorDataType** SensorSortedData;

int NumKeySensor;
SensorDataType** KeySensorData;
SensorDataType** KeySensorSorted;

DangerDataType** DangerObjectData;
DangerDataType** DangerSortedData;

int NumLocation;
LocationEngine** LocationData;

int NumEnterCenter;
EnterCenter* EnterCenterData;

const int TNT_DETONATION_RADIUS = 50;

char* TntCloneName;

#ifdef _DEBUG
XStream fDanger;
#endif

//char* TntSporeName = "Bomb";

extern int NumSkipLocation[WORLD_MAX];
extern int RealNumLocation[WORLD_MAX];
extern int* NumLocationData[WORLD_MAX];
extern int* NumLocationID[WORLD_MAX];

void dgInThreall(void);
void dgOutThreall(void);
char* dgGetThreallPhrase(void);

int TimeSecretEnable[2][MAX_TIME_SECRET];
const char* TimeSecretName[2][MAX_TIME_SECRET] = {{"YLI1","YLI2"},{"YLI1","YLI2"}};
int TimeSecretData0[2][MAX_TIME_SECRET] = {{0,0},{0,0}};
int TimeSecretData1[2][MAX_TIME_SECRET] = {{0,0},{0,0}};
int TimeSecretType[2][MAX_TIME_SECRET] = {{UVS_ITEM_TYPE::PEELOT,UVS_ITEM_TYPE::POPONKA},{UVS_ITEM_TYPE::WORMASTER_PART1,UVS_ITEM_TYPE::QUEEN_FROG_PART1}};

int ThreallMessageProcess;
extern uvsVanger *Gamer;
void StaticOpen(void)
{
	int i,j;
	Vector vCheck;
	int n,l,q,t;

#ifdef NEW_TNT
	if(TntTableSize){
		TntSortedData = new TntCreature*[TntTableSize];
		StaticSort(TntTableSize,(StaticObject**)TntObjectData,(StaticObject**)TntSortedData);
	};
#else
	if(TntTableSize){
		TntSortedData = new TntStaticObject*[TntTableSize];
		StaticSort(TntTableSize,(StaticObject**)TntObjectData,(StaticObject**)TntSortedData);
	};
#endif

	if(SnsTableSize){
		SensorSortedData = new SensorDataType*[SnsTableSize];
		StaticSort(SnsTableSize,(StaticObject**)SensorObjectData,(StaticObject**)SensorSortedData);
	};

//	TntSpore = ModelD.FindModel(TntSporeName);

	for(i = 0;i < TntTableSize;i++){
//		TntObjectData[i]->Enable = 1;

#ifdef NEW_TNT
		
		TntObjectData[i]->CurrentHeight = TntObjectData[i]->StartHeight = TntObjectData[i]->MaxHeight;
		TntObjectData[i]->TntNumLink = 0;

		for(j = 0;j < TntTableSize;j++){
			if(i != j){
				if(TntObjectData[i]->TntNumLink >= MAX_TNT_EXPLOSION) break;
				vCheck.x = getDistX(TntObjectData[i]->R_curr.x,TntObjectData[j]->R_curr.x);
				vCheck.y = getDistY(TntObjectData[i]->R_curr.y,TntObjectData[j]->R_curr.y);
				vCheck.z = TntObjectData[i]->R_curr.z - TntObjectData[j]->R_curr.z;
				if(vCheck.vabs() < TNT_DETONATION_RADIUS && !TestLink2Link(TntObjectData[i],TntObjectData[j])){
					TntObjectData[i]->TntLinkData[TntObjectData[i]->TntNumLink] = TntObjectData[j];
					TntObjectData[i]->TntNumLink++;
				};
			};
	 	};
#else
		TntObjectData[i]->DestroyRestore = 0;
		TntObjectData[i]->DestroyFlag = 0;
		TntObjectData[i]->NumLink = 0;
		for(j = 0;j < TntTableSize;j++){
			if(i != j){
				if(TntObjectData[i]->NumLink >= TNT_NUM_LINK) break;

				vCheck.x = getDistX(TntObjectData[i]->R_curr.x,TntObjectData[j]->R_curr.x);
				vCheck.y = getDistY(TntObjectData[i]->R_curr.y,TntObjectData[j]->R_curr.y);
				vCheck.z = TntObjectData[i]->R_curr.z - TntObjectData[j]->R_curr.z;
				if(vCheck.vabs() < TNT_DETONATION_RADIUS && !TestLink2Link(TntObjectData[i],TntObjectData[j])){
					TntObjectData[i]->Link[TntObjectData[i]->NumLink] = TntObjectData[j];
					TntObjectData[i]->NumLink++;
				};
			};
	 	};
#endif
	};

#ifdef GLOBAL_TIMER
	Parser in(GetTargetName("tlocation.lst"));
#else
	Parser in(GetTargetName("location.lst"));
#endif

	in.search_name("NumEngine");
	NumLocation = in.get_int();

	NumLocation -= NumSkipLocation[CurrentWorld] - RealNumLocation[CurrentWorld];
	
	i = 0;
	LocationData = new LocationEngine*[NumLocation];

	for(l = 0;l < NumLocation + NumSkipLocation[CurrentWorld] - RealNumLocation[CurrentWorld];l++){
		in.search_name("Part");
		q = in.get_int();
		if(l != q) ErrH.Abort("Bad Number of Part");
		t = 1;

		for(j = 0;j < NumSkipLocation[CurrentWorld];j++){
			if(NumLocationID[CurrentWorld][NumLocationData[CurrentWorld][j]] == q){
				if(j >= RealNumLocation[CurrentWorld])
					t = 0;
				break;
			};
		};
		
		if(t){
			in.search_name("EngineType");
			n = in.get_int();
			switch(n){
				case EngineTypeList::DOOR:
					LocationData[i] = new DoorEngine;
					break;
				case EngineTypeList::ELEVATOR:
					LocationData[i] = new ElevatorEngine;
					break;
				case EngineTypeList::ESCAVE:
					LocationData[i] = new EscaveEngine;
					break;
				case EngineTypeList::PASSAGE:
					LocationData[i] = new PassageEngine;
					break;
				case EngineTypeList::CYCLIC:
					LocationData[i] = new CyclicEngine;
					break;
				case EngineTypeList::TRAIN:
					LocationData[i] = new TrainEngine;
					break;
				case EngineTypeList::IMPULSE_ESCAVE:
					LocationData[i] = new ImpulseEscave;
					break;
				case EngineTypeList::TIRISTOR:
					LocationData[i] = new TiristorEngine;
					break;
				case EngineTypeList::IMPULSE_SPOT:
					LocationData[i] = new ImpulseSpot;
					break;
				case EngineTypeList::CHECK_POINT:
					LocationData[i] = new CheckPointEngine;
					break;
				case EngineTypeList::LAND_SLIDE:
					LocationData[i] = new LandSlideEngine;
					break;
				case EngineTypeList::SIGN_PLAY:
					LocationData[i] = new SignPlayEngine;
					break;
				case EngineTypeList::ITEM_GENERATOR:
					LocationData[i] = new ItemGenerator;
					break;
				default:
					ErrH.Abort("Bad Location");
					break;
			};
			LocationData[i]->Open(in);
			i++;
		};
	};

	if(DngTableSize){
		DangerSortedData = new DangerDataType*[DngTableSize];
		StaticSort(DngTableSize,(StaticObject**)DangerObjectData,(StaticObject**)DangerSortedData);
	};

	NumKeySensor = 0;
	for(i = 0;i < SnsTableSize;i++){
//		SensorObjectData[i]->Enable = 1;
		if(CheckKeySensor(SensorObjectData[i]))
			NumKeySensor++;
	};
	
	if(NumKeySensor){
		KeySensorData = new SensorDataType*[NumKeySensor];
		KeySensorSorted  = new SensorDataType*[NumKeySensor];
		j = 0;
		for(i = 0;i < SnsTableSize;i++){
			if(CheckKeySensor(SensorObjectData[i])){
				KeySensorData[j] = SensorObjectData[i];
				j++;
			};
		};
		StaticSort(NumKeySensor,(StaticObject**)KeySensorData,(StaticObject**)KeySensorSorted);
	};

	for(i = 0;i < NumLocation;i++) LocationData[i]->Link();

	in.search_name("NumEnterCenter");
	NumEnterCenter = in.get_int();
	EnterCenterData = new EnterCenter[NumEnterCenter];
	for(i = 0;i < NumEnterCenter;i++) EnterCenterData[i].Open(in);

#ifdef _DEBUG
	fDanger.open("danger.log",XS_OUT);
#endif

	for(i = 0;i < DngTableSize;i++)
		DangerObjectData[i]->TabuUse = 0;

	ThreallMessageProcess = 0;

#ifdef SAVE_TNT_DATA
	int xTntPosition;
	int yTntPosition;
	int nTntPosition;
	int iTntPosition;
	XStream outTnt;

	Parser tin(GetTargetName("out.vl"));	
	for(i = 0;i < TntTableSize;i++){
		iTntPosition = tin.get_int();
		xTntPosition = tin.get_int();
		yTntPosition = tin.get_int();
		nTntPosition = tin.get_int();
		TntObjectData[iTntPosition]->R_curr.x = xTntPosition;
		TntObjectData[iTntPosition]->R_curr.y = yTntPosition;
		TntObjectData[iTntPosition]->nSource = nTntPosition;
	};

	outTnt.open(GetTargetName("tnttable.vlc"),XS_OUT);
	outTnt.write("VLT1",4);
	outTnt < TntTableSize;
	for(i = 0;i < TntTableSize;i++)
		TntObjectData[i]->Save(outTnt);
	outTnt.close();
#endif
};

void ValocTableFree(void)
{
	int i;
	if(TntTableSize){
		for(i = 0;i < TntTableSize;i++) delete TntObjectData[i];
		delete[] TntObjectData;
		TntTableSize = 0;
	};

	if(SnsTableSize){
		for(i = 0;i < SnsTableSize;i++){
			SensorObjectData[i]->Close();
			delete SensorObjectData[i];
		};
		delete[] SensorObjectData;
		SnsTableSize = 0;
	};

	if(DngTableSize){
		for(i = 0;i < DngTableSize;i++) delete DangerObjectData[i];
		delete[] DangerObjectData;
		DngTableSize = 0;
	};
};

void StaticClose(void)
{
	int i;

	if(NumKeySensor){
		delete[] KeySensorSorted;
		delete[] KeySensorData;
		NumKeySensor = 0;
	};

	if(NumLocation){
		for(i = 0;i < NumLocation;i++){
			LocationData[i]->Close();
			delete LocationData[i];	
		};
		delete[] LocationData;
		NumLocation = 0;
	};

	if(TntTableSize) delete[] TntSortedData;
	if(SnsTableSize) delete[] SensorSortedData;
	if(DngTableSize) delete[] DangerSortedData;
	
	for(i = 0;i < NumEnterCenter;i++) EnterCenterData[i].Close();
	delete[] EnterCenterData;
#ifdef _DEBUG
	fDanger.close();
#endif
};

void StaticQuant(void)
{
	int y0,y1,i;
	StaticObject* st;
	uchar** lt;
		
	lt = vMap->lineT;

	if(NetworkON){
		for(i = 0;i < TntTableSize;i++)
		{
			if(lt[TntObjectData[i]->R_curr.y])
				TntObjectData[i]->NetQuant();
			else
				TntObjectData[i]->NetHideEvent();
		};
	}else{
		for(i = 0;i < TntTableSize;i++)
		{		
			if(lt[TntObjectData[i]->R_curr.y])
				TntObjectData[i]->Quant();
			else
				TntObjectData[i]->HideEvent();

		};	
	};

	for(i = 0;i < NumLocation;i++)
		if(LocationData[i]->Enable)
			LocationData[i]->Quant();

	int dy0;
	y0 = ViewY -  TurnSideY;
	y1 = ViewY + TurnSideY;

	if(y0 < 0){
		dy0 = -y0;
		y0 = 0;
		i = FindFirstStatic(y0,y1,(StaticObject**)DangerSortedData,DngTableSize);
		if(i > -1){
			do{
				st = DangerSortedData[i];
				if(abs(getDistX(st->R_curr.x,ViewX)) < TurnSideX) ((DangerDataType*)(st))->Quant();
				i++;
			}while(i < DngTableSize && DangerSortedData[i]->R_curr.y < y1);
		};

		y0 = TOR_YSIZE;
		y1 = TOR_YSIZE - dy0;
		i = FindFirstStatic(y0,y1,(StaticObject**)DangerSortedData,DngTableSize);
		if(i > -1){
			do{
				st = DangerSortedData[i];
				if(abs(getDistX(st->R_curr.x,ViewX)) < TurnSideX) ((DangerDataType*)(st))->Quant();
				i++;
			}while(i < DngTableSize && DangerSortedData[i]->R_curr.y < y1);
		};
	}else{
		if(y1 > TOR_YSIZE){
			dy0 = y1 - TOR_YSIZE;
			 y1 = TOR_YSIZE;
			i = FindFirstStatic(y0,y1,(StaticObject**)DangerSortedData,DngTableSize);
			if(i > -1){
				do{
					st = DangerSortedData[i];
					if(abs(getDistX(st->R_curr.x,ViewX)) < TurnSideX) ((DangerDataType*)(st))->Quant();
					i++;
				}while(i < DngTableSize && DangerSortedData[i]->R_curr.y < y1);
			};

			y0 = 0;
			y1 = dy0;
			i = FindFirstStatic(y0,y1,(StaticObject**)DangerSortedData,DngTableSize);

			if(i > -1){
				do{
					st = DangerSortedData[i];
					if(abs(getDistX(st->R_curr.x,ViewX)) < TurnSideX) ((DangerDataType*)(st))->Quant();
					i++;
				}while(i < DngTableSize && DangerSortedData[i]->R_curr.y < y1);
			};
		}else{
			i = FindFirstStatic(y0,y1,(StaticObject**)DangerSortedData,DngTableSize);

			if(i > -1){
				do{
					st = DangerSortedData[i];
					if(abs(getDistX(st->R_curr.x,ViewX)) < TurnSideX) ((DangerDataType*)(st))->Quant();
					i++;
				}while(i < DngTableSize && DangerSortedData[i]->R_curr.y < y1);
			};
		};
	};
};

int CheckKeySensor(SensorDataType* p)
{
	if(p->SensorType == SensorTypeList::ESCAVE ||
	   p->SensorType == SensorTypeList::PASSAGE ||
	   p->SensorType == SensorTypeList::SPOT || 
	   p->SensorType == SensorTypeList::ARMOR_UPDATE ||
	   p->SensorType == SensorTypeList::FIRE_UPDATE ||
	   p->SensorType == SensorTypeList::FLY_UPDATE)
		return 1;
	else 
		return 0;
};

#ifdef NEW_TNT

int TestLink2Link(TntCreature* p,TntCreature* n)
{
	int i;  
	for(i = 0;i < p->TntNumLink;i++)
		if(p->TntLinkData[i] == n) return 1;
	 return 0;
};

#else

int TestLink2Link(TntStaticObject* p,TntStaticObject* n)
{
	int i;  
	for(i = 0;i < p->NumLink;i++)
		if(p->Link[i] == n) return 1;
	 return 0;
};

#endif

extern int TurnSideX;
extern int TurnSideY;

#ifndef NEW_TNT
void RestoreFlagBarell(void)
{
	int i;
	for(i = 0;i < TntTableSize;i++)
		if(TntObjectData[i]->Enable == 0) 
			((TntStaticObject*)(TntObjectData[i]))->DestroyRestore = 1;
};

void RestoreBarell(void)
{
	int i,y0,y1;
	StaticObject* st;

	y0 = vMap->upLine + EXPLOSION_BARELL_RADIUS + 5;
	y1 = vMap->downLine - EXPLOSION_BARELL_RADIUS - 5;

	if(y0 > y1){
		if(ViewY > y1) 
			y1 =  TOR_YSIZE - 1;
		else 
			y0 = 0;
	};

	i = FindFirstStatic(y0,y1,(StaticObject**)TntSortedData,TntTableSize);
	if(i > -1){
		do{
			st = TntSortedData[i];
			if(((TntStaticObject*)(st))->DestroyRestore){
//				DestroyBarellSmooth(st->R_curr.x,st->R_curr.y,TNT_CRATER_RADIUS,TNT_CRATER_RADIUS / 2,32 - RND(64),7,3,st->R_curr.z,3);
//				MapD.CreateCrater(st->R_curr,MAP_POINT_CRATER06,TNT_CRATER_RADIUS);
				if(DestroyBarellSmoothWithCheck(st->R_curr.x,st->R_curr.y,TNT_CRATER_RADIUS,TNT_CRATER_RADIUS / 2,32 - RND(64),7,3,st->R_curr.z,3))
					((TntStaticObject*)(st))->DestroyRestore = 0;
			};
			i++;
		}while(i < TntTableSize && TntSortedData[i]->R_curr.y < y1);
	};
};
#endif

void StaticSort(int num,StaticObject** data,StaticObject** list)
{
	int nmin,min;
	StaticObject* c;
	int i,j;
	for(i = 0;i < num;i++) list[i] = data[i];
	for(i = 0;i < num - 1;i++){
		nmin = i;
		min = list[i]->R_curr.y;
		for(j = i + 1;j < num;j++){
			if(list[j]->R_curr.y < min){
				min = list[j]->R_curr.y;
				nmin = j;
			};
		};
		if(nmin != i){
			c = list[i];
			list[i] = list[nmin];
			list[nmin] = c;
		};
	};
/*	for(i = 1;i < num;i++)
		if(list[i]->R_curr.y < list[i - 1]->R_curr.y)
			ErrH.Abort("Bad Sorted List");*/
};

int FastFindFirstStatic(int y0,int y1,StaticObject** list,int num)
{
	int n0,n1,n;
	int cy;
	int i,j;
	n0 = 0;
	n1= num;
	for(i = 0;i < num;i++){
		n = (n0 + n1) >> 1;
		cy = list[n]->R_curr.y;
		if(cy < y0) n1 = n;
		else{
			if(cy > y1) n0 = n;
			else{
				for(j = n;j >= 0;j++){
					if(list[j]->R_curr.y < y0)
						return (j + 1);
				};
				return 0;
			};
		};
	};
	return -1;
};

int FindFirstStatic(int y0,int y1,StaticObject** list,int num)
{
	int i;
	for(i = 0;i < num;i++){
		if(list[i]->R_curr.y > y0){
			if(list[i]->R_curr.y < y1) return i;
			return -1;
		};
	};
	return -1;
};

//---------------------------------------------------NEW NEW NEW -------------------------------------------------------


MobileLocation* FindMobileLocation(char* Name)
{
	int i;
	for(i = 0;i < MLTableSize;i++)
		{
		if(!strcmp(Name,MLTable[i]->name)) return (MLTable[i]);
		}
	return NULL;
};

SensorDataType* FindSensor(const char* Name)
{
	int i;
	for(i = 0;i < SnsTableSize;i++)
		{
		if(SensorSortedData[i]->Name != NULL && !strcmp(Name,SensorSortedData[i]->Name)) return (SensorSortedData[i]);
		}
	return NULL;
};

LocationEngine* FindLocation(char* Name)
{
	int i;
	for(i = 0;i < NumLocation;i++){
		if(LocationData[i]->MLLink != NULL && !strcmp(Name,LocationData[i]->MLLink->name)) return (LocationData[i]);
	};
	return NULL;
};

void SensorDataType::SaveSensor(XStream& out)
{
	int n;

	out < R_curr.x;
	out < R_curr.y;
	out < R_curr.z;

	out < SensorType;
	out < radius;
	n = strlen(Name);
	out < n;
	out.write(Name,n);

	out < (R_curr.z - radius);
//	out < z0;
	out < vData.x;
	out < vData.y;
	out < vData.z;
	out < Power;
	out < (R_curr.z + radius);
//	out < z1;
	out < data5;
	out < data6;
};

void SensorDataType::CreateSensor(XStream& in,int ind)
{
	int n;

	in > R_curr.x;
	in > R_curr.y;
	in > R_curr.z;

	in > SensorType;
	in > radius > n;
	if(n){
		Name = new char[n + 1];
		in.read(Name,n);
		Name[n] = '\0';
	}else Name = NULL;

	in > z0;
	in > vData.x;
	in > vData.y;
	in > vData.z;
	in > Power;
	in > z1;
	in > data5;
	in > data6;
//	in.seek(8*sizeof(int),XS_CUR);

	Owner = NULL;
	Mode = SensorTypeList::NONE;
	Enable = 1;
	StaticType = StaticObjectType::SENSOR;

	cycleTor(R_curr.x,R_curr.y);
	Status = 0;
	ID = ID_STATIC;
	Index = 0;
	TableIndex = ind;
};

void SensorDataType::Close(void)
{
	if(Name) delete[] Name;
};

void SensorDataType::Touch(GeneralObject* obj)
{
	if(Owner) 
		Owner->Touch(obj,this);
//Send to Server
};
	
#define LOCATION_ENGINE_MESSAGE

void LocationEngine::Open(Parser& in)
{
	char* n;

	in.search_name("MLName");
	n = in.get_name();
	if(strcmp(n,"None")){
		MLLink = FindMobileLocation(n);

#ifdef LOCATION_ENGINE_MESSAGE
		if(!MLLink) 
			ErrH.Abort("Bad Find MLName");
#endif	
		R_curr.x = XCYCL(MLLink->x0 + abs(getDistX(MLLink->x0,MLLink->x1) / 2));
		R_curr.y = YCYCL(MLLink->y0 + abs(getDistY(MLLink->y0,MLLink->y1) / 2));
		radius = abs(getDistX(MLLink->x0,MLLink->x1) / 2) + abs(getDistY(MLLink->y0,MLLink->y1) / 2);
	}else MLLink = NULL;

	in.search_name("ActivePhase");
	ActivePhase = in.get_int();
	in.search_name("DeactivePhase");
	DeactivePhase = in.get_int();

	in.search_name("ActiveTime");

	ActiveTime = in.get_int();
	in.search_name("DeactiveTime");
	DeactiveTime = in.get_int();

	in.search_name("SoundID");
	SoundID =  in.get_int();

	NetMode = Mode = EngineModeList::WAIT;
	Enable = 1;

	HideFlag = 1;
	UpdateFlag = 0;
	if(NetworkON) NetID = CREATE_NET_ID(NID_SENSOR);
	else NetID = 0;
	TabuUse = 0;
};

void LocationEngine::SoundEvent(void)
{
	if(!MLLink || abs(getDistY(YCYCL(MLLink->table[MLLink->cFrame].y0 - MLLink->table[MLLink->cFrame].sy / 2),ViewY)) > (TurnSideY + MLLink->table[MLLink->cFrame].sy) ||
	   abs(getDistX(XCYCL(MLLink->table[MLLink->cFrame].x0 - MLLink->table[MLLink->cFrame].sx / 2),ViewX)) > (TurnSideX + MLLink->table[MLLink->cFrame].sx))
		return;	
	switch(SoundID){
		case LOCATION_SOUND_ESCAVE:
			if(ActD.Active) SOUND_ENTRANCE(getDistX(ActD.Active->R_curr.x,R_curr.x));
			break;
		case LOCATION_SOUND_BRIDGE_THREALL:
			SOUND_CRASH();
			break;
		case LOCATION_SOUND_BRIDGE_FOSTRAL:
			SOUND_BOLT();
			break;
		case LOCATION_SOUND_DOOR_BOOZEENA:
			SOUND_TOGGLE();
			break;
		case LOCATION_SOUND_STONE_GLORX:
			if(ActD.Active) SOUND_TEAR(getDistX(ActD.Active->R_curr.x,R_curr.x));
			break;
		case LOCATION_SOUND_CRYPT:
			if(ActD.Active) SOUND_HIDEOUT(getDistX(ActD.Active->R_curr.x,R_curr.x));
			break;
		case LOCATION_SOUND_IMPULSE:
			SOUND_KIDPUSH();
			break;
	};
};

void LocationEngine::Close(void)
{
};

void LocationEngine::Touch(GeneralObject* obj,SensorDataType* p)
{
};

void LocationEngine::Quant(void)
{
};
   
void LocationEngine::Link(void)
{
};

void CyclicEngine::Open(Parser& in)
{
	char* n;
	int i;
	LocationEngine::Open(in);

	in.search_name("ActionMode");
	ActionMode = in.get_int();

	if(ActionMode > -1){
		in.search_name("NumActionLink");
		NumAction = in.get_int();

		ActionLink = new SensorDataType*[NumAction];
		for(i = 0;i < NumAction;i++){
			in.search_name("ActionName");
			n = in.get_name();
			ActionLink[i] = FindSensor(n);
			if(!ActionLink[i]) 
				ErrH.Abort("Bad Find Sensor");
			ActionLink[i]->Enable = 0;	     
			ActionLink[i]->Owner = this;
		};
	}else ActionLink = NULL;

	Type = EngineTypeList::CYCLIC;

	Time = 0;
	Mode = EngineModeList::SEND_PROCESS;
//	MLLink->setStaticPhase(DeactivePhase);
	MLLink->goKeyPhase(DeactivePhase);
};

void CyclicEngine::Quant(void)
{
	int i;
	if(!MLLink) return;
	if(!MLLink->frozen){
		if(MLLink->isGoFinish()){
			if(abs(getDistY(YCYCL(MLLink->table[MLLink->cFrame].y0 - MLLink->table[MLLink->cFrame].sy / 2),ViewY)) > (TurnSideY + MLLink->table[MLLink->cFrame].sy) ||
			   abs(getDistX(XCYCL(MLLink->table[MLLink->cFrame].x0 - MLLink->table[MLLink->cFrame].sx / 2),ViewX)) > (TurnSideX + MLLink->table[MLLink->cFrame].sx))
				return;
			switch(Mode){
				case EngineModeList::ACCEPT_PROCESS:
					Time++;
					if(Time > DeactiveTime){
						MLLink->goKeyPhase(ActivePhase);
						SoundEvent();
						Time = 0;
						Mode = EngineModeList::ACCEPT_DELAY;
					};
					break;
				case EngineModeList::ACCEPT_DELAY:
					Mode = EngineModeList::ACCEPT_PROCESS_END;
					break;
				case EngineModeList::ACCEPT_PROCESS_END:
					Time++;
					if(Time > ActiveTime){
						MLLink->goKeyPhase(DeactivePhase);
						SoundEvent();
						Time = 0;
						Mode = EngineModeList::SEND_PROCESS;
					};
					break;
				case EngineModeList::SEND_PROCESS:
					Mode = EngineModeList::ACCEPT_PROCESS;
					break;
			};
		};

		if(ActionLink){
			if(Mode == ActionMode){
				for(i = 0;i < NumAction;i++) 
					ActionLink[i]->Enable = 1;
			}else{
				for(i = 0;i < NumAction;i++)
					ActionLink[i]->Enable = 0;
			};
		};
	}else{
//		MLLink->goKeyPhase(DeactivePhase);
		Mode = EngineModeList::SEND_PROCESS;
		Time = 0;
	};
};

void DoorEngine::Open(Parser& in)
{
	char* n;
	int i;
	LocationEngine::Open(in);

	in.search_name("NumSensorLink");
	NumSensor = in.get_int();
	SensorLink = new SensorDataType*[NumSensor];

	for(i = 0;i < NumSensor;i++){
		in.search_name("SensorName");
		n = in.get_name();	
		SensorLink[i] = NULL;
		SensorLink[i] = FindSensor(n);
		if(!SensorLink[i]) 
			ErrH.Abort("Bad Find Sensor");
		SensorLink[i]->Enable = 1;
		SensorLink[i]->Owner = this;
	};

	in.search_name("LockFlag");
	LockFlag = in.get_int();

	in.search_name("Luck");
	Luck = in.get_int();

	if(NetworkON) Luck = 0;

	TouchFlag = 0;
	Type = EngineTypeList::DOOR;
	Mode = EngineModeList::WAIT;
	Time = 0;
	ProcessFlag = 0;
//	MLLink->setStaticPhase(DeactivePhase);
	MLLink->goKeyPhase(DeactivePhase);

	Level = 0;
	NumTouchObject = 0;
};

void DoorEngine::Close(void)
{
	delete[] SensorLink;
};

void DoorEngine::Touch(GeneralObject* obj,SensorDataType* p)
{
	if(!Enable || !MLLink) return;
	if(!MLLink->frozen) 
		TouchFlag++;
};

void DoorEngine::Quant(void)
{
	char* ThreallText;
	if(!MLLink) return;
	if(!MLLink->frozen){
		HideFlag = 0;
		NumTouchObject = TouchFlag;
		if(MLLink->isGoFinish()){
			ProcessFlag = 0;
			if(NetworkON && Mode != NetMode){
				if(UpdateFlag){
					if(NetMode == EngineModeList::OPEN) OpenDoor(0);
					else CloseDoor(0);
					UpdateFlag = 0;
					return;
				}else{
					NetMode = Mode;
					NETWORK_OUT_STREAM.update_object(NetID,R_curr.x,R_curr.y);
					NETWORK_OUT_STREAM < (char)(NetMode);
					NETWORK_OUT_STREAM.end_body();
				};
			};

			if(Time){
				Time--;
				TouchFlag = 0;
			}else{
				if(TouchFlag){
					if(Mode != EngineModeList::OPEN && !(LockFlag & DOOR_OPEN_LOCK)){
						if(CurrentWorld == WORLD_THREALL && !strcmp(MLLink->name,"MMMM")){
//							aciSendEvent2actint(ACI_SHOW_TEXT,NULL);
							dgInThreall();
							ThreallText = dgGetThreallPhrase();
							if(ThreallText) aciPrepareText(ThreallText);
							ThreallMessageProcess = 1;
						};
						Mode = EngineModeList::OPEN;
						MLLink->goKeyPhase(ActivePhase);
						ProcessFlag = 1;
						SoundEvent();
					};
					TouchFlag = 0;
				}else{
					if(Mode != EngineModeList::WAIT && !(LockFlag & DOOR_CLOSE_LOCK)){
						if(CurrentWorld == WORLD_THREALL && !strcmp(MLLink->name,"MMMM")){
							aciSendEvent2actint(ACI_HIDE_TEXT,NULL);
							dgOutThreall();
							ThreallMessageProcess = 0;
						};
						Mode = EngineModeList::WAIT;
						MLLink->goKeyPhase(DeactivePhase);
						ProcessFlag = 1;
						SoundEvent();
					};
				};
			};

		};
	}else{
		if(Mode != EngineModeList::WAIT) 
			MLLink->goKeyPhase(DeactivePhase);
		Mode = EngineModeList::WAIT;
		Time = 0;
		ProcessFlag = 0;
		NumTouchObject = 0;
		HideFlag = 1;
	};
};

void DoorEngine::OpenDoor(void)
{
	if(!Enable || !MLLink) return;
	Time = ActiveTime;
	Mode = EngineModeList::OPEN;
	ProcessFlag = 1;
	if(!MLLink->frozen){
		MLLink->goKeyPhase(ActivePhase);
		SoundEvent();
	};

/*	if(NetworkON){
		NetMode = Mode;
		NETWORK_OUT_STREAM.update_object(NetID,R_curr.x,R_curr.y);
		NETWORK_OUT_STREAM < (char)(NetMode);
		NETWORK_OUT_STREAM.end_body();
	};*/
};

void DoorEngine::CloseDoor(void)
{
	if(!Enable || !MLLink) return;
	Time = DeactiveTime;
	Mode = EngineModeList::WAIT;
	ProcessFlag = 1;
	if(!MLLink->frozen){
		MLLink->goKeyPhase(DeactivePhase);
		SoundEvent();
	};

/*	if(NetworkON){
		NetMode = Mode;
		NETWORK_OUT_STREAM.update_object(NetID,R_curr.x,R_curr.y);
		NETWORK_OUT_STREAM < (char)(NetMode);
		NETWORK_OUT_STREAM.end_body();
	};*/
};

void DoorEngine::OpenDoor(int t)
{
	if(!Enable || !MLLink) return;
	Time = t;
	Mode = EngineModeList::OPEN;
	ProcessFlag = 1;
	if(!MLLink->frozen){
		MLLink->goKeyPhase(ActivePhase);
		SoundEvent();
	};

/*	if(NetworkON){
		NetMode = Mode;
		NETWORK_OUT_STREAM.update_object(NetID,R_curr.x,R_curr.y);
		NETWORK_OUT_STREAM < (char)(NetMode);
		NETWORK_OUT_STREAM.end_body();
	};*/
};

void DoorEngine::CloseDoor(int t)
{
	if(!Enable || !MLLink) return;
	Time = t;
	Mode = EngineModeList::WAIT;
	ProcessFlag = 1;
	if(!MLLink->frozen){
		MLLink->goKeyPhase(DeactivePhase);
		SoundEvent();
	};

/*	if(NetworkON){
		NetMode = Mode;
		NETWORK_OUT_STREAM.update_object(NetID,R_curr.x,R_curr.y);
		NETWORK_OUT_STREAM < (char)(NetMode);
		NETWORK_OUT_STREAM.end_body();
	};*/
};


void TiristorEngine::Open(Parser& in)
{
	char* n;
	int i;
	LocationEngine::Open(in);

	in.search_name("NumSensorLink");
	NumSensor = in.get_int();
	SensorLink = new SensorDataType*[NumSensor];

	for(i = 0;i < NumSensor;i++){
		in.search_name("SensorName");
		n = in.get_name();	
		SensorLink[i] = NULL;
		SensorLink[i] = FindSensor(n);
		if(!SensorLink[i]) 
			ErrH.Abort("Bad Find Sensor");
		SensorLink[i]->Enable = 1;
		SensorLink[i]->Owner = this;
	};

	in.search_name("LockFlag");
	LockFlag = in.get_int();

	in.search_name("Luck");
	Luck = in.get_int();

	if(NetworkON) Luck = 0;

	TouchFlag = 0;
	Type = EngineTypeList::TIRISTOR;
	Mode = EngineModeList::WAIT;
	ProcessFlag = 0;
//	MLLink->setStaticPhase(DeactivePhase);
	MLLink->goKeyPhase(DeactivePhase);
	NumTouchObject = 0;

	if(CurrentWorld == WORLD_FOSTRAL || CurrentWorld == WORLD_GLORX){
		for(i = 0;i < MAX_TIME_SECRET;i++){
			if(!strcmp(TimeSecretName[CurrentWorld][i],MLLink->name) && !TimeSecretEnable[CurrentWorld][i])
				Enable = 0;
		};
	};
};

void TiristorEngine::Close(void)
{
	delete[] SensorLink;
};

void TiristorEngine::Touch(GeneralObject* obj,SensorDataType* p)
{
	if(!Enable || !MLLink) return;
	if(!MLLink->frozen) 
		TouchFlag++;
};

void TiristorEngine::Quant(void)
{
	if(!MLLink) return;
	if(!MLLink->frozen){
		NumTouchObject = TouchFlag;
		if(MLLink->isGoFinish()){

			if(NetworkON && Mode != NetMode){
				if(UpdateFlag){
					if(NetMode == EngineModeList::OPEN) OpenDoor();
					UpdateFlag = 0;
					return;
				}else{
					NetMode = Mode;
					NETWORK_OUT_STREAM.update_object(NetID,R_curr.x,R_curr.y);
					NETWORK_OUT_STREAM < (char)(NetMode);
					NETWORK_OUT_STREAM.end_body();					
				};
			};

			ProcessFlag = 0;
			if(TouchFlag){
				if(Mode != EngineModeList::OPEN && !(LockFlag & DOOR_OPEN_LOCK)){
					Mode = EngineModeList::OPEN;
					MLLink->goKeyPhase(ActivePhase);
					SoundEvent();
					ProcessFlag = 1;
				};
				TouchFlag = 0;
			};
		};
	}else{
		if(Mode != EngineModeList::WAIT) 
			MLLink->goKeyPhase(DeactivePhase);
		Mode = EngineModeList::WAIT;
		ProcessFlag = 0;
		NumTouchObject = 0;
	};
};

void TiristorEngine::OpenDoor(void)
{
	if(!Enable || !MLLink) return;
	Mode = EngineModeList::OPEN;
	ProcessFlag = 1;
	if(!MLLink->frozen){
		MLLink->goKeyPhase(ActivePhase);
		SoundEvent();
	};
};

void TiristorEngine::CloseDoor(void)
{
	if(!Enable || !MLLink) return;
	Mode = EngineModeList::WAIT;
	ProcessFlag = 1;
	if(!MLLink->frozen){
		MLLink->goKeyPhase(DeactivePhase);
		SoundEvent();
	};
};

/*void DoorWaitEngine::Quant(void)
{
	if(!MLLink) return;
	if(!MLLink->frozen){
		NumTouchObject = TouchFlag;
		if(MLLink->isGoFinish()){
			ProcessFlag = 0;
			if(Time){
				Time--;
				TouchFlag = 0;
			}else{
				if(TouchFlag){
					if(Mode != EngineModeList::OPEN && !(LockFlag & DOOR_OPEN_LOCK)){
						Mode = EngineModeList::OPEN;
						MLLink->goKeyPhase(ActivePhase);
						ProcessFlag = 1;
					};
					TouchFlag = 0;
				}else{
					if(Mode != EngineModeList::WAIT && !(LockFlag & DOOR_CLOSE_LOCK)){
						Mode = EngineModeList::WAIT;
						MLLink->goKeyPhase(DeactivePhase);
						ProcessFlag = 1;
					};
				};
			};
		};
	}else{
		Mode = EngineModeList::WAIT;
		MLLink->goKeyPhase(DeactivePhase);
		Time = 0;
		ProcessFlag = 0;
		NumTouchObject = 0;
	};
};*/

void EnterEngine::Open(Parser& in)
{
	char* n;
	LocationEngine::Open(in);

	in.search_name("ActionName");
	n = in.get_name();
	ActionLink = FindSensor(n);
	if(!ActionLink)
		ErrH.Abort("Bad Find Sensor");
	ActionLink->Owner = this;
	ActionLink->Enable = 1;
	ActionLink->Index = 0;
};

void EnterEngine::Close(void)
{
};

void EnterEngine::Active(void)
{
};

const int ENGINE_ACTION_TOUCH = 1;
const int ENGINE_ACTION_USE = 2;

/*void SpotEngine::Open(Parser& in)
{
	EnterEngine::Open(in);
	Type = EngineTypeList::SPOT;
	UseFlag = 0;
};

void SpotEngine::Close(void)
{
	EnterEngine::Close();
};

void SpotEngine::Quant(void)
{
	if(UseFlag){
		Owner->PutCenter(this);
		UseFlag = 0;
	}else  UseFlag--;
};														

void SpotEngine::Active(void)
{
	UseFlag = 1;
};

void SpotEngine::Touch(GeneralObject* obj,SensorDataType* p)
{
	UseFlag = 1;
};
*/

void PassageEngine::Open(Parser& in)
{
	char* n;

	LocationEngine::Open(in);

	Type = EngineTypeList::PASSAGE;

	in.search_name("ActionName");
	n = in.get_name();
	ActionLink = FindSensor(n);
	if(!ActionLink) 
		ErrH.Abort("Bad Find Sensor");
	ActionLink->Owner = this;
	ActionLink->Enable = 1;

	in.search_name("PassageName");
	n = in.get_name();
	PassageName = new char[strlen(n) + 1];
	strcpy(PassageName,n);
};

void PassageEngine::Close(void)
{
	delete[] PassageName;
};

void PassageEngine::Quant(void)
{
};

void PassageEngine::Link(void)
{
	uvsPassage* pe;
	uvsPort = NULL;
	pe = (uvsPassage*)PassageTail;
	while(pe){
		if(!strcmp(pe->name,PassageName)){
			uvsPort = pe;
			break;
		};
		pe = (uvsPassage*)(pe -> next);
	};
	if(!uvsPort) 
		ErrH.Abort("Bad uvsPort");
	uvsPort->unitPtr.PassageT = this;
	uvsPort->locked = 0;
};

//Close Locked

void ElevatorEngine::Open(Parser& in)
{
	char* n;
	EnterEngine::Open(in);
	Type = EngineTypeList::ELEVATOR;
	UseFlag = 0;
	ActionFlag = 0;

	in.search_name("DoorName");
	n = in.get_name();
	if(strcmp(n,"None")){
		DoorName = new char[strlen(n) + 1];
		strcpy(DoorName,n);
		DoorLink = NULL;
	}else DoorName = NULL;
};

void ElevatorEngine::Close(void)
{
	delete[] DoorName;
};

void ElevatorEngine::Quant(void)
{
	if(MLLink && !MLLink->frozen){
		if(UseFlag){
			if(!(DoorLink->NumTouchObject)){
				Owner->PutCenter(this);
				UseFlag = 0;
			}else{
//				if(!(DoorLink->ProcessFlag) && DoorLink->Mode == EngineModeList::OPEN)
				if(ActionFlag && !(DoorLink->ProcessFlag) && DoorLink->Mode == EngineModeList::OPEN)
					DoorLink->CloseDoor(0);
			};
			ActionFlag = 0;
		}else{
			 if(DoorLink->NumTouchObject){
				Owner->LockedCenter(this);
				UseFlag = 1;
			 };
		};		
	}else{
		if(UseFlag){
			Owner->PutCenter(this);
			UseFlag = 0;
			ActionFlag = 0;
		};
	};	
};

void ElevatorEngine::Touch(GeneralObject* obj,SensorDataType* p)
{
	if(UseFlag) ActionFlag = 1;
};

void ElevatorEngine::Link(void)
{
	DoorLink = (DoorEngine*)(FindSensor(DoorName)->Owner);
	if(!DoorLink) ErrH.Abort("Bad Door Object");
};

void ElevatorEngine::Active(void)
{
	DoorLink->OpenDoor(0);
	UseFlag = 1;
};

void EscaveEngine::Open(Parser& in)
{
	ElevatorEngine::Open(in);
	in.search_name("StartAngle");
	StartAngle = in.get_int();
	Type = EngineTypeList::ESCAVE;
	LastOpen = 0;
};
	
void EscaveEngine::Quant(void)
{
	if(MLLink && !MLLink->frozen){
		if(UseFlag){
			if(DoorLink->Mode == EngineModeList::WAIT && !(DoorLink->ProcessFlag) && !LastOpen){
				Owner->PutCenter(this);
				UseFlag = 0;
				ActionLink->Enable = 1;
			};
		}else{
			if(DoorLink->Mode != EngineModeList::WAIT){
				Owner->LockedCenter(this);
				UseFlag = 1;
				ActionLink->Enable = 0;
			 };
		};
	}else{
		if(UseFlag){
			Owner->PutCenter(this);
			UseFlag = 0;
			ActionLink->Enable = 1;
		};
	};
	LastOpen = 0;
};

void EscaveEngine::Active(void)
{
	UseFlag = 1;
	ActionLink->Enable = 0;
	LastOpen = 1;
};

void ImpulseEscave::Open(Parser& in)
{
	char* n;

	ElevatorEngine::Open(in);
	Type = EngineTypeList::IMPULSE_ESCAVE;
	in.search_name("ImpulseName");
	n = in.get_name();
	ImpulseLink = FindSensor(n);
	if(!ImpulseLink)
		ErrH.Abort("Bad Find Impulse Sensor");
//	ImpulseLink->Owner = NULL;
	ImpulseLink->Enable = 0;
	ImpulseLink->Index = 1;
};

void ImpulseEscave::Quant(void)
{
	if(MLLink && !MLLink->frozen){
		switch(UseFlag){
			case 0:
				if(DoorLink->Mode != EngineModeList::WAIT){
					Owner->LockedCenter(this);
					UseFlag = 1;
				};
				break;
			case 1:
				if(DoorLink->Mode == EngineModeList::WAIT && !(DoorLink->ProcessFlag)){
					Owner->PutCenter(this);
					UseFlag = 0;
					ImpulseLink->Enable = 0;
				}else{
					if(ActionFlag && !(DoorLink->ProcessFlag) && DoorLink->Mode == EngineModeList::OPEN)
						DoorLink->CloseDoor(0);
				};
				ActionFlag = 0;
				break;
			case 2:
				if(DoorLink->Mode == EngineModeList::WAIT && !(DoorLink->ProcessFlag)){
					Owner->PutCenter(this);
					UseFlag = 0;
					ImpulseLink->Enable = 0;
				};
				ActionFlag = 0;
				break;
		};

/*		if(UseFlag){
			if(DoorLink->Mode == EngineModeList::WAIT && !(DoorLink->ProcessFlag)){
				Owner->PutCenter(this);
				UseFlag = 0;
				ImpulseLink->Enable = 0;
			}else{
				if(ActionFlag && !(DoorLink->ProcessFlag) && DoorLink->Mode == EngineModeList::OPEN)
					DoorLink->CloseDoor(0);
			};
			ActionFlag = 0;
		}else{
			if(DoorLink->Mode != EngineModeList::WAIT){
				Owner->LockedCenter(this);
				UseFlag = 1;
			 };
		};*/

	}else{
		if(UseFlag){
			Owner->PutCenter(this);
			UseFlag = 0;
			ImpulseLink->Enable = 0;
			ActionFlag = 0;
		};
	};
};

void ImpulseEscave::Active(void)
{
	DoorLink->OpenDoor(0);
//	UseFlag = 1;
	UseFlag = 2;
	ImpulseLink->Enable = 1;
};

void ImpulseSpot::Open(Parser& in)
{
	ImpulseEscave::Open(in);
	Type = EngineTypeList::IMPULSE_SPOT;
	ActionLink->Enable = 1;
};

void ImpulseSpot::Quant(void)
{
	if(MLLink && !MLLink->frozen){
		switch(UseFlag){
			case 0:
				if(DoorLink->Mode != EngineModeList::WAIT){
					Owner->LockedCenter(this);
					UseFlag = 1;					
					ActionLink->Enable = 0;
				};
				break;
			case 1:
				if(DoorLink->Mode == EngineModeList::WAIT && !(DoorLink->ProcessFlag)){
					Owner->PutCenter(this);
					UseFlag = 0;
					ActionLink->Enable = 1;
				};
				break;
			case 2:
				if(DoorLink->Mode == EngineModeList::WAIT && !(DoorLink->ProcessFlag)){
					Owner->PutCenter(this);
					UseFlag = 0;
					ImpulseLink->Enable = 0;
					ActionLink->Enable = 1;
				};
				break;
		};
	}else{
		if(UseFlag){
			Owner->PutCenter(this);
			UseFlag = 0;
			ImpulseLink->Enable = 0;			
			ActionLink->Enable =1;
		};
	};
};

void ImpulseSpot::Active(void)
{
	ImpulseLink->Enable = 1;
	UseFlag = 2;
	ActionLink->Enable = 0;
	DoorLink->OpenDoor(0);
};

void TrainEngine::CreateTrain(SensorDataType* p1,SensorDataType* p2,int time)
{
	Type = EngineTypeList::TRAIN;

	MLLink = NULL;

	ActivePhase = 0;
	DeactivePhase = 0;

	Mode = EngineModeList::WAIT;
	Enable = 1;				    

	TrainLink[0] = p1;
	TrainLink[0]->Owner = this;
	TrainLink[0]->Enable = 1;
	TrainLink[0]->Index = 0;

	TrainLink[1] = p2;
	TrainLink[1]->Owner = this;
	TrainLink[1]->Enable = 1;
	TrainLink[1]->Index = 1;

	ActiveTime = time;
	DeactiveTime = 10;
	LockFlag = DOOR_CLOSE_LOCK;	
};

int NumAddDanger;
void TrainEngine::Open(Parser& in)
{
	char* n;
	LocationEngine::Open(in);

	Type = EngineTypeList::TRAIN;

	in.search_name("FirstStationName");
	n = in.get_name();
	TrainLink[0] = FindSensor(n);
	if(!TrainLink[0])
		ErrH.Abort("Bad Find Sensor");
	TrainLink[0]->Owner = this;
	TrainLink[0]->Enable = 1;
	TrainLink[0]->Index = 0;

	in.search_name("SecondStationName");
	n = in.get_name();
	TrainLink[1] = FindSensor(n);
	if(!TrainLink[1]) 
		ErrH.Abort("Bad Find Sensor");
	TrainLink[1]->Owner = this;
	TrainLink[1]->Enable = 1;
	TrainLink[1]->Index = 1;
	LockFlag = DOOR_CLOSE_LOCK;
	in.search_name("EffectID");
	EffectID = in.get_int();
	DangerLink = NULL;
};

void TrainEngine::Link(void)
{
	if(EffectID && NumAddDanger < DngTableSize){
		DangerLink = DangerObjectData[NumAddDanger];
		NumAddDanger++;
		DangerLink->CreateDanger(Vector(TrainLink[0]->R_curr.x,TrainLink[0]->R_curr.y,FloodLEVEL),TrainLink[0]->radius / 2,DangerTypeList::TRAIN);
	};
};

void DangerDataType::CreateDanger(Vector v,int r,int tp)
{
	R_curr = v;

	Type = tp;
	radius =  r;	

    //zmod fixed
	//if(NetworkON) Enable = 0; //zmod 1.13
	//else Enable = 1;
	Enable = 1;

	StaticType = StaticObjectType::DANGER;

	cycleTor(R_curr.x,R_curr.y);
	Status = 0;
	ID = ID_STATIC;

	switch(Type){
		case DangerTypeList::WHIRLPOOL:
		case DangerTypeList::TRAIN:
			Delay = 3;
			if(CurrentWorld == WORLD_GLORX){
				if(!RND(1000)) Enable = 0;
			}else if(!RND(200)) Enable = 0;
			break;	
		case DangerTypeList::FASTSAND:
		case DangerTypeList::SWAMP:
			dActive = 3;
			break;
	};
	Time = 0;
	rActive = radius / 2;	
};

void DangerDataType::CreateDanger(XStream& in)
{
	in > R_curr.x;
	in > R_curr.y;
	in > R_curr.z;

	in > Type;
	in > radius;
	in.seek(4*sizeof(int),XS_CUR);

	//zmod fixed
	//if(NetworkON) Enable = 1; //zmod danger
	//else Enable = 1;
	Enable = 1;

	StaticType = StaticObjectType::DANGER;

	cycleTor(R_curr.x,R_curr.y);
	Status = 0;
	ID = ID_STATIC;

	switch(Type){
		case DangerTypeList::WHIRLPOOL:
		case DangerTypeList::TRAIN:
			Delay = 3;
			if(CurrentWorld == WORLD_GLORX){
				if(!RND(1000)) Enable = 0;
			}else if(!RND(200)) Enable = 0;
			break;	
		case DangerTypeList::FASTSAND:
		case DangerTypeList::SWAMP:
			dActive = 3;
			break;
//zmod fixed 1.14
		case DangerTypeList::FIRE:
			if (NetworkON) { //!RND(4) means remove 1/4 in random
//zmod 1.21
				if(CurrentWorld==WORLD_ARKONOY && !RND(2) && !RND(2)) Enable=0;
				if(CurrentWorld==WORLD_THREALL && !RND(2)) Enable=0;
				}
			break;	
	};
	Time = 0;
	rActive = radius / 2;
};

void DangerDataType::Quant(void)
{
	Vector vPos;
	int r,n,d1,d2,h;
	WaterParticleObject* w;
	int x,y,z,a;

	//if(NetworkON) return; //zmod fixed

	switch(Type){
		case DangerTypeList::FASTSAND:
//#ifdef _DEBUG
//			fDanger < "\nFastSand : " <= Enable < ";" <= R_curr.x < "," <= R_curr.y < "," <= R_curr.z;
//#endif
			if(Enable){
				if(rActive > radius - abs(dActive) || rActive <= abs(dActive)) dActive = -dActive;
				rActive += dActive;

				d1 = (radius + rActive) / (2*10);
				d2 = (radius + rActive) / (2*2);

	//			for(i = 0;i < (int)RND(5);i++){
					vPos = Vector(rActive - RND(2*rActive),rActive - RND(2*rActive),0);
					vPos += R_curr;
					vPos.x = XCYCL(vPos.x);
					vPos.y = YCYCL(vPos.y);

					r = d1 + RND(d1);
					if(RND(100) < 50) h = -r;
					else h = RND(d2) + r;
					n = 3 + RND(3);
					MapD.CreateLavaSpot(vPos,0,0,r,h,r,0,n,n*2,83,0,0,0,83);
	//			};
				RadialRender(R_curr.x,R_curr.y,radius + d1 * 2);
				if(!RND(100)) Enable = 0;
			}else{
				if(!RND(100)) Enable = 1;				
			};
			break;
		case DangerTypeList::WHIRLPOOL:
//#ifdef _DEBUG
//			fDanger < "\nWhirlpool : " <= Enable < ";" <= R_curr.x < "," <= R_curr.y < "," <= R_curr.z;
//#endif

			if(Enable){
				if(Time <= 0){
					if(!RND(100)) Enable = 0;
					else{
						EffD.CreateDeform(R_curr,DEFORM_WATER_ONLY,0);
						Time = EffD.DeformData[1].NumFrame - 1;
//						Time = 1 + RND(3);
					};
				}else{
					Time--;
					if(!(Time & 3)){
						w = (WaterParticleObject*)(EffD.GetObject(EFF_PARTICLE03));
						if(w){
							w->CreateParticle(30,5,1 << 7,radius,10,31,5,R_curr,1);
							EffD.ConnectObject(w);
						};
					};
				};
			}else{
				if(!RND(100)){
					EffD.CreateDeform(R_curr,DEFORM_WATER_ONLY,0);
					Time = EffD.DeformData[1].NumFrame - 1;
//					Time = 1 + RND(3);
					Enable = 1;
				};
			};
			break;
		case DangerTypeList::SWAMP:

//#ifdef _DEBUG
//			fDanger < "\nSwamp : " <= Enable < ";" <= R_curr.x < "," <= R_curr.y < "," <= R_curr.z;
//#endif
			if(Enable){
				if(rActive > radius - abs(dActive) || rActive <= abs(dActive)) dActive = -dActive;
				rActive += dActive;

				d1 = (radius + rActive) / (2*10);
				d2 = (radius + rActive) / (2*2);

	//			for(i = 0;i < (int)RND(5);i++){
					vPos = Vector(rActive - RND(2*rActive),rActive - RND(2*rActive),0);
					vPos += R_curr;
					vPos.x = XCYCL(vPos.x);
					vPos.y = YCYCL(vPos.y);

					r = d1 + RND(d1);
					if(RND(100) < 50) h = -r;
					else h = RND(d2) + r;
					n = 3 + RND(3);
					MapD.CreateLavaSpot(vPos,0,0,r,h,r,0,n,n*2,83,0,0,0,83);
	//			};
				RadialRender(R_curr.x,R_curr.y,radius + d1 * 2);
				if(!RND(100)) Enable = 0;
			}else{
				if(!RND(100)) Enable = 1;
			};
			break;
		case DangerTypeList::FIRE:
			switch(CurrentWorld){
				case WORLD_BOOZEENA:
					FireWork(200,PI/4);
					break;
				case WORLD_THREALL:
					if(!ActD.ThreallDestroy) FireWork(500,PI/8);
					break;
				case WORLD_ARKONOY:
					FireWork(1000,PI/6);
					break;
				case WORLD_XPLO:
					FireWork(800,PI / 6);
					break;
			};
			break;
		case DangerTypeList::HOLE:

//#ifdef _DEBUG
//			fDanger < "\nHole : " <= Enable < ";" <= R_curr.x < "," <= R_curr.y < "," <= R_curr.z;
//#endif

			if(Enable){
//zmod fixed 1.15
				if (!NetworkON) {
				Delay = 70 + RND(30);
				MapD.CreateMapHole(R_curr,radius,Delay,0,0);
				} else {
					vPos = Vector(realRND(30)-15,realRND(30)-15,0);
					vPos += R_curr;
					vPos.x = XCYCL(vPos.x);
					vPos.y = YCYCL(vPos.y);
					Delay = realRND(100);
					MapD.CreateMapHole(vPos,(radius*0.8+realRND(radius*0.4)),Delay,0,0);
				}
///zmod
				Enable = 0;
				Time = 0;
				Delay += radius*2;
			}else{
				if(Time > Delay)
					Enable = 1;
				Time++;
			};
			break;
		case DangerTypeList::TRAIN:
			if(!RND(3)){
				a = rPI(RND(PI*2));
				r = RND(radius);
				x = XCYCL(R_curr.x + (CO[a] * r >> 16));
				y = YCYCL(R_curr.y + (SI[a] * r >> 16));
				z = FloodLEVEL;
				EffD.CreateParticleGenerator(Vector(x,y,z),Vector(R_curr.x,R_curr.y,z),Vector(5 - RND(10),5 - RND(10),0),PG_STYLE_ENTER);
			};
			break;
	};
};

void DangerDataType::FireWork(int rFactor,int Angle)
{
	BulletObject* p;
	Vector vCheck;
//	FishWarrior* f;

	if(ActD.Active && RND(rFactor) < 5){
		switch(CurrentWorld){
			case WORLD_THREALL:
				p = BulletD.CreateBullet();
				vCheck = Vector(getDistX(ActD.Active->R_curr.x,R_curr.x),getDistY(ActD.Active->R_curr.y, R_curr.y),ActD.Active->R_curr.z - R_curr.z)*DBM((Angle >> 1) - (int)(RND(Angle)),Z_AXIS);

				p->Owner = NULL;
//zmod 1.21 
				if (!NetworkON) {
					if(RND(100) < 50)
						p->CreateBullet(Vector(R_curr.x,R_curr.y,radius + R_curr.z),
							Vector(XCYCL(vCheck.x + R_curr.x),YCYCL(vCheck.y + R_curr.y),R_curr.z + vCheck.z),NULL,&GameBulletData[WD_BULLET_BIG_FIREBALL]);
					else 
						p->CreateBullet(Vector(R_curr.x,R_curr.y,radius + R_curr.z),
							Vector(XCYCL(vCheck.x + R_curr.x),YCYCL(vCheck.y + R_curr.y),R_curr.z + vCheck.z),NULL,&GameBulletData[WD_BULLET_SMALL_FIREBALL]);
				} else {
					if(RND(100) < 50)
						p->CreateBullet(
							Vector(R_curr.x,R_curr.y,radius + R_curr.z),
							Vector(XCYCL(vCheck.x + R_curr.x),YCYCL(vCheck.y + R_curr.y),R_curr.z + vCheck.z),
							NULL,
							&GameBulletData[WD_BULLET_BIG_FIREBALL],
							NULL,
							-2); //decrease speed
					else 
						p->CreateBullet(
							Vector(R_curr.x,R_curr.y,radius + R_curr.z),
							Vector(XCYCL(vCheck.x + R_curr.x),YCYCL(vCheck.y + R_curr.y),R_curr.z + vCheck.z),
							NULL,
							&GameBulletData[WD_BULLET_SMALL_FIREBALL],
							NULL,
							-2); //decrease speed
				}

				if(ActD.Active)
					SOUND_THUNDER(getDistX(ActD.Active->R_curr.x,R_curr.x)) 
				break;
			case WORLD_ARKONOY:
				p = BulletD.CreateBullet();
				vCheck = Vector(getDistX(ActD.Active->R_curr.x,R_curr.x),getDistY(ActD.Active->R_curr.y, R_curr.y),ActD.Active->R_curr.z - R_curr.z)*DBM((Angle >> 1) - (int)(RND(Angle)),Z_AXIS);
				p->Owner = NULL;

//zmod fixed 1.15
				if (!NetworkON) {
				p->CreateBullet(Vector(R_curr.x,R_curr.y,radius + R_curr.z),
					Vector(XCYCL(vCheck.x + R_curr.x),YCYCL(vCheck.y + R_curr.y),R_curr.z + vCheck.z),ActD.Active,&GameBulletData[WD_BULLET_EYE_TERROR]);
				} else {
					p->CreateBullet(
						Vector(R_curr.x,R_curr.y,radius + R_curr.z),
						Vector(XCYCL(vCheck.x + R_curr.x),YCYCL(vCheck.y + R_curr.y),R_curr.z + vCheck.z),
						ActD.Active,
						&GameBulletData[WD_BULLET_EYE_TERROR],
						NULL,
						-3); //real speed=5 so 5-3=2
				}
				break;
			case WORLD_XPLO:
				p = BulletD.CreateBullet();
				vCheck = Vector(getDistX(ActD.Active->R_curr.x,R_curr.x),getDistY(ActD.Active->R_curr.y, R_curr.y),ActD.Active->R_curr.z - R_curr.z)*DBM((Angle >> 1) - (int)(RND(Angle)),Z_AXIS);
				p->Owner = NULL;

				p->CreateBullet(Vector(R_curr.x,R_curr.y,radius + R_curr.z),
					Vector(XCYCL(vCheck.x + R_curr.x),YCYCL(vCheck.y + R_curr.y),R_curr.z + vCheck.z),ActD.Active,&GameBulletData[WD_BULLET_XPLORATOR]);
				break;
			case WORLD_BOOZEENA:
				p = BulletD.CreateBullet();
				vCheck = Vector(getDistX(ActD.Active->R_curr.x,R_curr.x),getDistY(ActD.Active->R_curr.y, R_curr.y),ActD.Active->R_curr.z - R_curr.z)*DBM((Angle >> 1) - (int)(RND(Angle)),Z_AXIS);
				p->Owner = NULL;

				p->CreateBullet(Vector(R_curr.x,R_curr.y,radius + R_curr.z),
					Vector(XCYCL(vCheck.x + R_curr.x),YCYCL(vCheck.y + R_curr.y),R_curr.z + vCheck.z),NULL,&GameBulletData[WD_BULLET_OTHER]);
				if(ActD.Active)
					SOUND_THUNDER(getDistX(ActD.Active->R_curr.x,R_curr.x)) 
				break;
			
		};
	};
};

void EnterCenter::Open(Parser& in)
{
	int i,j,t;
	char* n;
	uvsEscave* pe;
	uvsSpot* ps;

	in.search_name("MaxEnter");
	MaxEnter = in.get_int();
	NumEnter = 0;

	if(MaxEnter){
		Data = new EnterEngine*[MaxEnter];
		ListData = new EnterEngine*[MaxEnter];

		in.search_name("uvsObjectType");
		t = in.get_int();
		
		in.search_name("uvsObjectName");
		n = in.get_name();

		Lock = NULL;
		if(t == 0){
			pe = (uvsEscave*)EscaveTail;
			while(pe){
				if(!strcmp(pe->name,n)){
					Lock = &(pe->locked);
					Owner = pe;
					break;
				};
				pe = (uvsEscave*)(pe -> next);
			};
			if(!Lock) 
				ErrH.Abort("Bad Escave Center Name");
		}else{
			ps = (uvsSpot*)SpotTail;
			while(ps){
				if(!strcmp(ps->name,n)){
					Lock = &(ps->locked);
					Owner = ps;
					break;
				};
				ps = (uvsSpot*)(ps -> next);
			};
			if(!Lock) 
				ErrH.Abort("Bad Spot Center Name");
		};
		Owner->unitPtr.EnterT = this;

		for(i = 0;i < MaxEnter;i++){
			in.search_name("SensorName");
			n = in.get_name();
			for(j = 0;j < SnsTableSize;j++){
				if(!strcmp(SensorObjectData[j]->Name,n)){
	//				if(SensorObjectData[j]->Owner->Type != EngineTypeList::ELEVATOR && SensorObjectData[j]->Owner->Type != EngineTypeList::ESCAVE)
	//					ErrH.Abort("Bad Enter Sensor Name");
					ListData[i] = (EnterEngine*)(SensorObjectData[j]->Owner);
					PutCenter((EnterEngine*)(SensorObjectData[j]->Owner));
					((EnterEngine*)(SensorObjectData[j])->Owner)->Owner = this;
					break;
				};
			};
		};
	};
};

void EnterCenter::Resort(void)
{
	int i,t;
	EnterEngine* b[10];
	t = RND(NumEnter);
	for(i = 0;i < NumEnter;i++) b[i] = ListData[(t + i) % NumEnter];
	for(i = 0;i < NumEnter;i++) Data[i] = b[i];
};

void EnterCenter::Close(void)
{
	if(MaxEnter){
		delete[] Data; 
		delete[] ListData;
	};
};

SensorDataType* EnterCenter::GetCenter(void)
{
	if(NumEnter == 0)
		ErrH.Abort("Bad get Center");
	NumEnter--;
	if(NumEnter == 0) (*Lock) = 1;
//	Data[NumEnter]->Active();
	return Data[NumEnter]->ActionLink;
};

void EnterCenter::PutCenter(EnterEngine* p)
{
	if(NumEnter == 0) (*Lock) = 0;
	else if(NumEnter >= MaxEnter)
		ErrH.Abort("Bad put Center");
	Data[NumEnter] = p;
	NumEnter++;
};

SensorDataType* EnterCenter::GetNearCenter(Vector& v)
{
	EnterEngine* p;
	p = NULL;
	if(NumEnter == 0) p =  Data[RND(NumEnter)];
	else  p = GetEnterCenter(v);
	return p->ActionLink;
};

EnterEngine* EnterCenter::GetEnterCenter(Vector& v)
{
	Vector vCheck;
	int d,dm,i;
	EnterEngine* p;

	vCheck = Vector(getDistX(v.x,ListData[0]->ActionLink->R_curr.x),getDistY(v.y,ListData[0]->ActionLink->R_curr.y),0);
	dm = d = vCheck.x * vCheck.x + vCheck.y * vCheck.y;
	p = ListData[0];

	for(i = 1;i < MaxEnter;i++){
		vCheck = Vector(getDistX(v.x,ListData[i]->ActionLink->R_curr.x),getDistY(v.y,ListData[i]->ActionLink->R_curr.y),0);
		d = vCheck.x * vCheck.x + vCheck.y * vCheck.y;
		if(d < dm){
			dm = d;
			p = ListData[i];
		};
	};
	return p;
};

void EnterCenter::LockedCenter(EnterEngine* p)
{
	int i;

	if(NumEnter == 0)
		ErrH.Abort("Bad Locked Num Enter");

	for(i = 0;i < NumEnter;i++)
		if(Data[i] == p) break;

	if(i == NumEnter)
		ErrH.Abort("Bad LockedCenter");

	NumEnter--;
	if(NumEnter == 0) (*Lock) = 1;
	else Data[i] = Data[NumEnter];
};

EnterCenter* FindDoor(DoorEngine* p,int& type)
{
	int i,j;
	EnterEngine* n;

	for(j = 0;j < NumEnterCenter;j++){
		for(i = 0;i < EnterCenterData[j].MaxEnter;i++){
			n = EnterCenterData[j].ListData[i];
			switch(n->Type){
				case EngineTypeList::IMPULSE_ESCAVE:
					if(((ImpulseEscave*)(n))->DoorLink == p){
						type = n->Type;
						return &(EnterCenterData[j]);
					};
					break;
				case EngineTypeList::IMPULSE_SPOT:
					if(((ImpulseSpot*)(n))->DoorLink == p){
						type = n->Type;
						return &(EnterCenterData[j]);
					};
					break;
				case EngineTypeList::ELEVATOR:
					if(((ElevatorEngine*)(n))->DoorLink == p){
						type = n->Type;
						return &(EnterCenterData[j]);
					};
					break;
				case EngineTypeList::ESCAVE:
					if(((EscaveEngine*)(n))->DoorLink == p){
						type = n->Type;
						return &(EnterCenterData[j]);
					};
					break;
			};
		};
	};
	return NULL;
};

//--------------------------------------------------------------------------------------------------------------------

#ifdef NEW_TNT

void TntCreature::Quant(void)
{
	BulletObject* p;
	Vector vCheck;

	if(TntClone->QuickCheck()){
		if(HideFlag)
			HideFlag = 0;		

		if(TouchTime > 0){
			TouchTime--;
			if(TouchTime <= 0) Destroy();
		}else{
			if(CurrentHeight < MaxHeight){
				if(Time <= 0){
					Time =  DelayHeight;
					CurrentHeight++;
					TntClone->setPhase(CurrentHeight,0);
					if(ActD.Active && abs(getDistY(R_curr.y,ViewY)) - (radius << 1) < TurnSideY && abs(getDistX(R_curr.x,ViewX)) - (radius << 1) < TurnSideX)
						SOUND_GROW(getDistX(ActD.Active->R_curr.x,R_curr.x)) 
				}else Time--;
			}else{
				switch(CurrentWorld){
					case 0:
						if(RND(300) < 5){
							p = BulletD.CreateBullet();
							vCheck = Vector(radius,0,0) * DBM((int)(RND(2*PI)),Z_AXIS);
							p->CreateBullet(R_curr,
								Vector(XCYCL(vCheck.x + R_curr.x),YCYCL(vCheck.y + R_curr.y),R_curr.z),NULL,&GameBulletData[WD_BULLET_SPORE]);
							p->Owner = NULL;
						};
						break;
					case 1:
						if(RND(1000) < 5 && abs(getDistY(R_curr.y,ViewY)) - (radius << 1) < TurnSideY && abs(getDistX(R_curr.x,ViewX)) - (radius << 1) < TurnSideX)
							TouchTime = TntLinkDelay;
						break;
				};
			};
		};
	};
};

void TntCreature::Destroy(void)
{	
	int i;
	Vector vCheck;
	Object* p;
	int d;
	BulletObject* g;

	if(TntClone->QuickCheck()){
		CurrentHeight = 0;
		TntClone->setPhase(CurrentHeight,1);
		MapD.CreateCrater(R_curr,MAP_POINT_CRATER06,radius + 10);
		EffD.CreateFireBall(R_curr + Vector(0,0,20),DT_FIRE_BALL04,NULL,1 << 8,0);
		TouchTime = 0;
		Time = DestroyDelay;
		if(ActD.Active)
			SOUND_BARREL_DESTROY(getDistX(ActD.Active->R_curr.x,R_curr.x));

/*			CurrentHeight = 0;
			TntClone->setPhase(CurrentHeight,0);
			MapD.CreateCrater(R_curr,MAP_POINT_CRATER06,radius);
			EffD.CreateFireBall(R_curr + Vector(0,0,20),DT_FIRE_BALL04,NULL,1 << 8,0);
			TouchTime = 0;
			Time = DestroyDelay;*/

		switch(CurrentWorld){
			case WORLD_GLORX:
/*				if(ActD.Active){
					g = BulletD.CreateBullet();
					vCheck = Vector(getDistX((ActD.Active)->R_curr.x,R_curr.x),getDistY((ActD.Active)->R_curr.y,R_curr.y),(ActD.Active)->R_curr.z - R_curr.z);
					d = vCheck.vabs();
					vCheck *= radius;
					vCheck /= d;

					g->CreateBullet(Vector(XCYCL(vCheck.x + R_curr.x),YCYCL(vCheck.y + R_curr.y),R_curr.z + vCheck.z),
						(ActD.Active)->R_curr,NULL,&GameBulletData[WD_BULLET_GSEED]);
					g->Owner = NULL;
				};*/

				for(i = 0;i < PI*2;i+= PI / 8){
					g = BulletD.CreateBullet();
					vCheck = Vector(radius,0,0) * DBM(i,Z_AXIS);
					g->CreateBullet(Vector(XCYCL(vCheck.x + R_curr.x),YCYCL(vCheck.y + R_curr.y),R_curr.z + radius),
						Vector(XCYCL(vCheck.x*4 + R_curr.x),YCYCL(vCheck.y*4 + R_curr.y),R_curr.z),NULL,&GameBulletData[WD_BULLET_GSEED]);
					g->Owner = NULL;
				};

				p = (Object*)(ActD.Tail);
				while(p){
					if(p->Visibility == VISIBLE){
						vCheck = Vector(getDistX(p->R_curr.x,R_curr.x),getDistY(p->R_curr.y,R_curr.y),p->R_curr.z - R_curr.z);
						d = vCheck.vabs();
						if(d < TNT_POWER_RADIUS && d > 0){
							p->impulse(vCheck,TNT_POWER_IMPULSE*(TNT_POWER_RADIUS - d) / d,0);
							if(p->ID == ID_VANGER) ((VangerUnit*)(p))->BulletCollision(TNT_POWER_DAMAGE,NULL);
						};
					};		
					p = (Object*)(p->NextTypeList);
				};
				break;
			case WORLD_FOSTRAL:
				for(i = 0;i < PI*2;i+= PI / 8){
					g = BulletD.CreateBullet();
					vCheck = Vector(radius,0,0) * DBM(i,Z_AXIS);
					g->CreateBullet(Vector(XCYCL(vCheck.x + R_curr.x),YCYCL(vCheck.y + R_curr.y),R_curr.z + radius),
						Vector(XCYCL(vCheck.x*4 + R_curr.x),YCYCL(vCheck.y*4 + R_curr.y),R_curr.z + radius*2),NULL,&GameBulletData[WD_BULLET_FSEED]);
					g->Owner = NULL;
				};

				p = (Object*)(ActD.Tail);
				while(p){
					if(p->Visibility == VISIBLE){
						vCheck = Vector(getDistX(p->R_curr.x,R_curr.x),getDistY(p->R_curr.y,R_curr.y),p->R_curr.z - R_curr.z);
						d = vCheck.vabs();
						if(d < TNT_POWER_RADIUS && d > 0){
							p->impulse(vCheck,TNT_POWER_IMPULSE*(TNT_POWER_RADIUS - d) / d,0);
							if(p->ID == ID_VANGER) ((VangerUnit*)(p))->BulletCollision(TNT_POWER_DAMAGE,NULL);
						};
					};		
					p = (Object*)(p->NextTypeList);
				};
				break;
			case WORLD_NECROSS:
				for(i = 0;i < PI*2;i+= PI / 8){
					g = BulletD.CreateBullet();
					vCheck = Vector(radius,0,0) * DBM(i,Z_AXIS);
					g->CreateBullet(Vector(XCYCL(vCheck.x + R_curr.x),YCYCL(vCheck.y + R_curr.y),R_curr.z + radius),
						Vector(XCYCL(vCheck.x*4 + R_curr.x),YCYCL(vCheck.y*4 + R_curr.y),R_curr.z + radius*2),NULL,&GameBulletData[WD_BULLET_NSEED]);
					g->Owner = NULL;
				};

				p = (Object*)(ActD.Tail);
				while(p){
					if(p->Visibility == VISIBLE){
						vCheck = Vector(getDistX(p->R_curr.x,R_curr.x),getDistY(p->R_curr.y,R_curr.y),p->R_curr.z - R_curr.z);
						d = vCheck.vabs();
						if(d < TNT_POWER_RADIUS && d > 0){
							p->impulse(vCheck,TNT_POWER_IMPULSE*(TNT_POWER_RADIUS - d) / d,0);
							if(p->ID == ID_VANGER) ((VangerUnit*)(p))->BulletCollision(TNT_POWER_DAMAGE,NULL);
						};
					};		
					p = (Object*)(p->NextTypeList);
				};
				break;
		};
		for(i = 0;i < TntNumLink;i++){
			if(TntLinkData[i]->CurrentHeight > 0 && TouchTime == 0)
				TntLinkData[i]->TouchTime = TntLinkDelay;
		};
	};
};

const int MAX_TNT_FIRE_RADIUS = 300;

void TntCreature::DrawQuant(void)
{
/*	int aaa;	
	if(TntClone->QuickCheck()){
		aaa = 0;
		if(HideFlag) HideFlag = 0;
		while(TntClone->cStage != CurrentHeight){
			TntClone->quant(1);
			if(aaa++ > 10) return;
		};
		if(RND(15000) < 5)
			TouchTime = TntLinkDelay;
	};*/
};

extern MobileLocation* MLTnt[2];

void TntCreature::Save(XStream& in)
{
	in < R_curr.x;
	in < R_curr.y;
	in < R_curr.z;
	in < nSource;
	in < radius;
	in < StartHeight;	
};

void TntCreature::Load(XStream& in)
{
	int i;
	int dx,dy;

	GeneralObject::Init();

	in > R_curr.x;
	in > R_curr.y;
	in > R_curr.z;
	in > nSource;
	in > radius;
	in > StartHeight;
	
	Status = 0;
	ID = ID_STATIC;

	StaticType = StaticObjectType::TNT;
	Enable = 1;	

	CurrentHeight = 0;

	DestroyProcess = MAP_POINT_CRATER06;

//	DestroyDelay = 5;
	if(NetworkON) DestroyDelay = 350;
	else DestroyDelay = 350 + RND(350);
//	DestroyDelay = 10 + RND(20);
	
	if(NetworkON) DelayHeight = 1;
	else DelayHeight = 1 + RND(5);

	Time = 0;
	TouchTime = 0;

	TntLinkDelay = 5;
	TntNumLink = 0;

	TntClone = MLTnt[nSource]->cloning(getDistX(R_curr.x,MLTnt[nSource]->x0),getDistY(R_curr.y,MLTnt[nSource]->y0),"TntLocation");

	dx = abs(getDistX(TntClone->x0,TntClone->x1)) / 2;
	dy = abs(getDistY(TntClone->y0,TntClone->y1)) / 2;
	radius = (int)(sqrt(dx*(double)dx + dy*(double)dy));

	R_curr.x += dx;
	R_curr.y += dy;
	cycleTor(R_curr.x,R_curr.y);

	MaxHeight = TntClone->maxStage - 1;
//	StartHeight = MaxHeight - 1;
	CurrentHeight = StartHeight = MaxHeight;

//	TntClone->cStage = CurrentHeight = StartHeight;

	TntClone->setPhase(StartHeight,1);

	HideFlag = 1;

	for(i = 0;i < MAX_TNT_EXPLOSION;i++)
		TntLinkData[i] = NULL;
	NetTime = 0;

	if(NetworkON) NetID = CREATE_NET_ID(NID_TNT);
	else NetID = 0;
};

void TntCreature::Touch(GeneralObject* obj)
{
	if(CurrentHeight){
		if(NetworkON) NetDestroy();
		else Destroy();
	};
};

void TntCreature::NetEvent(void)
{
	NETWORK_IN_STREAM > NetTime;
	if(!HideFlag && NetTime > NetGlobalTime)
		NetDestroy(0);
};

void TntCreature::NetQuant(void)
{
	BulletObject* p;
	Vector vCheck;

	if(TntClone->NetQuickCheck(R_curr.x,R_curr.y,radius)){
		if(HideFlag){
			if(NetTime > NetGlobalTime){
				TntClone->setPhase(0,1);
				if(CurrentWorld == WORLD_GLORX) ClearBarell(R_curr.x,R_curr.y,radius,83,R_curr.z);
				else ClearBarell(R_curr.x,R_curr.y,radius,7,R_curr.z);
				CurrentHeight = 0;
				TouchTime = 0;
				Time = 0;
			}else{
				TntClone->cStage = CurrentHeight = StartHeight;
				Time = 0;
				TouchTime = 0;
				TntClone->setPhase(0,1);
				if(CurrentWorld == WORLD_GLORX) ClearBarell(R_curr.x,R_curr.y,radius,83,R_curr.z);
				else ClearBarell(R_curr.x,R_curr.y,radius,7,R_curr.z);
				TntClone->setPhase(CurrentHeight,0);					
			};
			HideFlag = 0;
		};
		
		if(TouchTime > 0){
			TouchTime--;
			if(TouchTime <= 0) NetDestroy();
		}else{
			if(NetTime < NetGlobalTime){
				if(CurrentHeight < MaxHeight){
					if(Time <= 0){
						Time =  DelayHeight;
						CurrentHeight++;
						TntClone->setPhase(CurrentHeight,0);
					}else Time--;
				}else{
					switch(CurrentWorld){
						case 0:
							if(RND(300) < 5){
								p = BulletD.CreateBullet();
								vCheck = Vector(radius,0,0) * DBM((int)(RND(2*PI)),Z_AXIS);
								p->CreateBullet(R_curr,
									Vector(XCYCL(vCheck.x + R_curr.x),YCYCL(vCheck.y + R_curr.y),R_curr.z),NULL,&GameBulletData[WD_BULLET_SPORE]);
								p->Owner = NULL;
							};
							break;
						case 1:
							if(RND(1000) < 5)
								TouchTime = TntLinkDelay;
							break;
					};
				};
			};
		};
	}else NetHideEvent();
};

void TntCreature::NetDestroy(int fl)
{	
	int i;
	Vector vCheck;
	Object* p;
	int d;
	BulletObject* g;

	if(!HideFlag && CurrentHeight && TntClone->NetQuickCheck(R_curr.x,R_curr.y,radius)){
		CurrentHeight = 0;
		TntClone->setPhase(CurrentHeight,1);
		MapD.CreateCrater(R_curr,MAP_POINT_CRATER06,radius + 10);
		EffD.CreateFireBall(R_curr + Vector(0,0,20),DT_FIRE_BALL04,NULL,1 << 8,0);

		TouchTime = 0;
		Time = 0;
		NetTime = NetGlobalTime + (DestroyDelay*256 / 15)*1000;
		if(ActD.Active)
			SOUND_BARREL_DESTROY(getDistX(ActD.Active->R_curr.x,R_curr.x));
	
		if(fl){
			NETWORK_OUT_STREAM.update_object(NetID,R_curr.x,R_curr.y);
			NETWORK_OUT_STREAM < NetTime;
			NETWORK_OUT_STREAM.end_body();
		};

		switch(CurrentWorld){ //znfo TNT power scheme
			case WORLD_GLORX:
				for(i = 0;i < PI*2;i+= PI / 8){
					g = BulletD.CreateBullet();
					vCheck = Vector(radius,0,0) * DBM(i,Z_AXIS);
					g->CreateBullet(Vector(XCYCL(vCheck.x + R_curr.x),YCYCL(vCheck.y + R_curr.y),R_curr.z + radius),
						Vector(XCYCL(vCheck.x*4 + R_curr.x),YCYCL(vCheck.y*4 + R_curr.y),R_curr.z),NULL,&GameBulletData[WD_BULLET_GSEED]);
					g->Owner = NULL;
				};

				p = (Object*)(ActD.Tail);
				while(p){
					if(p->Visibility == VISIBLE){
						vCheck = Vector(getDistX(p->R_curr.x,R_curr.x),getDistY(p->R_curr.y,R_curr.y),p->R_curr.z - R_curr.z);
						d = vCheck.vabs();
						if(d < 2*TNT_POWER_RADIUS && d > 0){
							p->impulse(vCheck,TNT_POWER_IMPULSE*(TNT_POWER_RADIUS*2 - d) / d,0);
							if(p->ID == ID_VANGER) ((VangerUnit*)(p))->BulletCollision(TNT_POWER_DAMAGE,NULL);
						};
					};		
					p = (Object*)(p->NextTypeList);
				};
				break;
			case WORLD_FOSTRAL:
				for(i = 0;i < PI*2;i+= PI / 8){
					g = BulletD.CreateBullet();
					vCheck = Vector(radius,0,0) * DBM(i,Z_AXIS);
					g->CreateBullet(Vector(XCYCL(vCheck.x + R_curr.x),YCYCL(vCheck.y + R_curr.y),R_curr.z + radius),
						Vector(XCYCL(vCheck.x*4 + R_curr.x),YCYCL(vCheck.y*4 + R_curr.y),R_curr.z + radius*2),NULL,&GameBulletData[WD_BULLET_FSEED]);
					g->Owner = NULL;
				};

				p = (Object*)(ActD.Tail);
				while(p){
					if(p->Visibility == VISIBLE){
						vCheck = Vector(getDistX(p->R_curr.x,R_curr.x),getDistY(p->R_curr.y,R_curr.y),p->R_curr.z - R_curr.z);
						d = vCheck.vabs();
						if(d < TNT_POWER_RADIUS && d > 0){
							p->impulse(vCheck,TNT_POWER_IMPULSE*(TNT_POWER_RADIUS - d) / d,0);
							if(p->ID == ID_VANGER) ((VangerUnit*)(p))->BulletCollision(TNT_POWER_DAMAGE,NULL);
						};
					};		
					p = (Object*)(p->NextTypeList);
				};
				break;
			case WORLD_NECROSS:
				for(i = 0;i < PI*2;i+= PI / 8){
					g = BulletD.CreateBullet();
					vCheck = Vector(radius,0,0) * DBM(i,Z_AXIS);
					g->CreateBullet(Vector(XCYCL(vCheck.x + R_curr.x),YCYCL(vCheck.y + R_curr.y),R_curr.z + radius),
						Vector(XCYCL(vCheck.x*4 + R_curr.x),YCYCL(vCheck.y*4 + R_curr.y),R_curr.z + radius*2),NULL,&GameBulletData[WD_BULLET_NSEED]);
					g->Owner = NULL;
				};

				p = (Object*)(ActD.Tail);
				while(p){
					if(p->Visibility == VISIBLE){
						vCheck = Vector(getDistX(p->R_curr.x,R_curr.x),getDistY(p->R_curr.y,R_curr.y),p->R_curr.z - R_curr.z);
						d = vCheck.vabs();
						if(d < TNT_POWER_RADIUS && d > 0){
							p->impulse(vCheck,TNT_POWER_IMPULSE*(TNT_POWER_RADIUS - d) / d,0);
							if(p->ID == ID_VANGER) ((VangerUnit*)(p))->BulletCollision(TNT_POWER_DAMAGE,NULL);
						};
					};		
					p = (Object*)(p->NextTypeList);
				};
				break;
		};

		for(i = 0;i < TntNumLink;i++){
			if(TntLinkData[i]->CurrentHeight > 0 && TouchTime == 0)
				TntLinkData[i]->TouchTime = TntLinkDelay;
		};
	};
};

#else

void TntStaticObject::Quant(uchar** lt)
{
	if(Enable){
		if(DestroyFlag){
			if(DestroyTime == 0){
				Touch(this);
				DestroyFlag = 0;
			}else DestroyTime--;
		};
	}else{
		if(DestroyFlag) DestroyFlag = 0;
		if(!lt[R_curr.y]) DestroyRestore = 1;
	};
};

void TntStaticObject::Load(XStream& in)
{
	int Skip;
	in > R_curr.x;
	in > R_curr.y;
	in > R_curr.z;
	in > Skip;
	in > radius;
	in > Skip;

	cycleTor(R_curr.x,R_curr.y);
	Status = 0;
	ID = ID_STATIC;

	StaticType = StaticObjectType::TNT;
	Enable = 1;
	DestroyType = MAP_POINT_CRATER06;
	DestroyFlag = 0;
	DestroyRestore = 0;

//	MobileLocation* mln;
//	mln = FindMobileLocation(TntCloneName);
//	TntClone = mln->cloning(getDistX(R_curr.x,mln->x0),getDistY(R_curr.y,mln->y0),"TntLocation");
};

void TntStaticObject::Touch(GeneralObject* obj)
{
	int i;
	ActionUnit* p;
	int r,dist,k;
	Vector vDir;
	uchar** lt;

	lt = vMap->lineT;
	if(Enable){
		for(i = R_curr.y - TNT_CRATER_RADIUS;i < R_curr.y + TNT_CRATER_RADIUS;i++) if(!lt[i]) return;

//		EffD.CreateExplosion(R_curr,EFF_EXPLOSION01,0,3 << 14,0);
//		EffD.CreateFireBall(R_curr,DT_FIRE_BALL04,NULL,1 << 10,0);
		EffD.CreateFireBall(R_curr,DT_FIRE_BALL04,NULL,1 << 8,0);

		MapD.CreateCrater(R_curr,DestroyType,TNT_CRATER_RADIUS);		
		Enable = 0;
		r = radius*4;
		p = (ActionUnit*)(ActD.Tail);
		while(p){
			if(p->Visibility == VISIBLE){
				vDir.y = getDistY(p->R_curr.y,R_curr.y);
				if(abs(vDir.y) < r){
					vDir.x = getDistX(p->R_curr.x,R_curr.x);
					if(abs(vDir.x) < r){
						vDir.z = p->R_curr.z - R_curr.z;
						if(vDir.z < r){
							dist = vDir.vabs();
							k = dist < 30 ? impulse_of_tuns_explosion : impulse_of_tuns_explosion*30/dist;
							// Tun's explosion impulse
#ifndef TEST_TRACK
							p->impulse(vDir,k,0);
#endif
						};
					};
				};
			};
			p = (ActionUnit*)(p->NextTypeList);
		};
		for(i = 0;i < NumLink;i++){
			if(Link[i]->Enable && !Link[i]->DestroyFlag){
				Link[i]->DestroyFlag = 1;
				Link[i]->DestroyTime = i*2 + 1;
			};
		};
	};
};
#endif

void CheckPointEngine::Open(Parser& in)
{
	char* n;
	int i;
	LocationEngine::Open(in);

	in.search_name("NumCheckSensor");
	NumCheckSensor = in.get_int();
	CheckSensor = new SensorDataType*[NumCheckSensor];

	CheckLocationName = new char*[NumCheckSensor];
	CheckLocation = new LocationEngine*[NumCheckSensor];
	CheckIndex = new int[NumCheckSensor];

	for(i = 0;i < NumCheckSensor;i++){
		in.search_name("SensorName");
		n = in.get_name();
		CheckSensor[i] = NULL;
		CheckSensor[i] = FindSensor(n);
		if(!CheckSensor[i])
			ErrH.Abort("Bad Find Sensor");
		CheckSensor[i]->Enable = 1;
		CheckSensor[i]->Owner = this;
		
		in.search_name("CheckLocationName");
		n = in.get_name();
		if(strcmp(n,"None")){
			CheckLocationName[i] = new char[strlen(n) + 1];
			strcpy(CheckLocationName[i],n);
		}else CheckLocationName[i] = NULL;
		CheckIndex[i] = 0;
	};

	in.search_name("MainLocationName");
	n = in.get_name();
	if(strcmp(n,"None")){
		MainLocationName = new char[strlen(n) + 1];
		strcpy(MainLocationName,n);
	}else MainLocationName = NULL;
	
	Type = EngineTypeList::CHECK_POINT;
	Mode = EngineModeList::WAIT;

	in.search_name("WrongCheckAction");
	WrongCheckMode = in.get_int();
};

void CheckPointEngine::Close(void)
{
	int i;
	for(i = 0;i < NumCheckSensor;i++){
		if(CheckLocationName[i]){
			delete CheckLocationName[i];
//			delete CheckLocation[i];
		};
	};
	delete CheckLocationName;
	delete CheckLocation;
	delete CheckSensor;
	if(MainLocationName) delete MainLocationName;
};

void CheckPointEngine::Link(void)
{
	int i;
	if(MainLocationName){
		MainLocation = FindLocation(MainLocationName);
		MainLocation->Enable = 0;
	};

	for(i = 0;i < NumCheckSensor;i++){
		if(CheckLocationName[i]){
			CheckLocation[i] = FindLocation(CheckLocationName[i]);
			CheckLocation[i]->Enable = 0;
		};
	};
};

const int CHECK_POINT_IMPULSE = 1;
const int CHECK_POINT_RESET = 2;

void CheckPointEngine::Touch(GeneralObject* obj,SensorDataType* p)
{
	
	int i,ind;	
	VangerUnit* v;	

	if(obj->ID != ID_VANGER) return;	
	v = (VangerUnit*)(obj);
	if(v->CheckPointCount >= NUM_CONTROL_BSIGN) return;
	ind = CHECK_BSIGN_INDEX[v->CheckPointCount];
	if(p == CheckSensor[ind]){
		if(CheckLocationName[ind]){
			CheckLocation[ind]->Enable = 1;
			CheckSensor[ind]->Enable = 0;
			v->CheckPointCount++;
		};
		if(v->CheckPointCount == NUM_CONTROL_BSIGN/*NumCheckSensor*/ && MainLocationName)
			MainLocation->Enable = 1;
	}else{
		if(WrongCheckMode & CHECK_POINT_IMPULSE)
			v->impulse(DBV(32 - RND(64),32 - RND(64),32 - RND(64)),RND(5),RND(5));
		if(WrongCheckMode & CHECK_POINT_RESET){
			for(i = 0;i < NumCheckSensor;i++){
				if(CheckLocation[i]->Type == EngineTypeList::TIRISTOR) ((TiristorEngine*)(CheckLocation[i]))->CloseDoor();
				CheckLocation[i]->Enable = 0;
				CheckSensor[i]->Enable = 1;
				v->CheckPointCount = 0;
			};
		};
	};
};

void LandSlideEngine::Open(Parser& in)
{
	SensorDataType* p;
	char* n;
	int i;
	LocationEngine::Open(in);

	for(i = 0;i < 4;i++){
		in.search_name("BorderSensorName");
		n = in.get_name();
		p = NULL;
		p = FindSensor(n);
		if(!p)
			ErrH.Abort("Bad Find Sensor");
		p->Enable = 0;
		p->Owner = NULL;
		cX[i] = p->R_curr.x;
		cY[i] = p->R_curr.y;
	};

	in.search_name("SignalSensorName");
	n = in.get_name();
	SignalSensor = NULL;
	SignalSensor = FindSensor(n);
	if(!SignalSensor)
		ErrH.Abort("Bad Find Sensor");
	SignalSensor->Enable = 1;
	SignalSensor->Owner = this;
	
	in.search_name("LifeTime");
	LifeTime = in.get_int();

	Type = EngineTypeList::LAND_SLIDE;
	Mode = EngineModeList::WAIT;
};

void LandSlideEngine::Touch(GeneralObject* obj,SensorDataType* p)
{
	if(Mode == EngineModeList::WAIT){
		Mode = EngineModeList::OPEN;
		MapD.CreateLandSlide(cX,cY,LifeTime);
	};
};

void SignPlayEngine::Open(Parser& in)
{
	char* n;
	int i;
	LocationEngine::Open(in);

	in.search_name("NumSensorLink");
	NumSensor = in.get_int();
	SensorLink = new SensorDataType*[NumSensor];

	for(i = 0;i < NumSensor;i++){
		in.search_name("SensorName");
		n = in.get_name();	
		SensorLink[i] = NULL;
		SensorLink[i] = FindSensor(n);
		if(!SensorLink[i]) 
			ErrH.Abort("Bad Find Sensor");
		SensorLink[i]->Enable = 1;
		SensorLink[i]->Owner = this;
	};

	in.search_name("NumReplay");
	NumReplay = in.get_int();
	ReplayCount = 0;

	Type = EngineTypeList::SIGN_PLAY;
	Mode = EngineModeList::WAIT;
	Time = 0;
	MLLink->goKeyPhase(DeactivePhase);
};

void SignPlayEngine::Quant(void)
{
	if(!MLLink) return;
	if(!MLLink->frozen){
		if(Mode == EngineModeList::OPEN && MLLink->isGoFinish()){
			if(ReplayCount >= NumReplay)
				Mode = EngineModeList::WAIT;
			else{
				if(Time > 0) Time--;
				else{				
					if(ReplayCount & 1){
						MLLink->goKeyPhase(ActivePhase);
						Time = ActiveTime;					
						SoundEvent();
					}else{
						MLLink->goKeyPhase(DeactivePhase);
						Time = DeactiveTime;
						SoundEvent();
					};
					ReplayCount++;
				};
			};
		};
	}else{
		if(Mode != EngineModeList::WAIT) 
			MLLink->goKeyPhase(DeactivePhase);
		Mode = EngineModeList::WAIT;
		Time = 0;
	};
};

void SignPlayEngine::Close(void)
{
	delete SensorLink;
};

void SignPlayEngine::Touch(GeneralObject* obj,SensorDataType* p)
{
	if(!Enable || !MLLink) return;
	if(!MLLink->frozen && Mode != EngineModeList::OPEN){
		Mode = EngineModeList::OPEN;
		MLLink->goKeyPhase(ActivePhase);
		ReplayCount = 0;
		Time = ActivePhase;
	};
};


void ItemGenerator::Open(Parser& in)
{
	int i;
	char* n;
	LocationEngine::Open(in);

	in.search_name("SensorName");
	n = in.get_name();	
	SensorLink = NULL;
	SensorLink = FindSensor(n);
	if(!SensorLink) 
		ErrH.Abort("Bad Find Sensor");
	SensorLink->Enable = 1;
	SensorLink->Owner = this;

	in.search_name("DoorName");
	n = in.get_name();
	if(strcmp(n,"None")){
		DoorName = new char[strlen(n) + 1];
		strcpy(DoorName,n);
		pLink = NULL;
	}else DoorName = NULL;

	Enable = 1;
	Type = EngineTypeList::ITEM_GENERATOR;
	Mode = EngineModeList::WAIT;

	if(CurrentWorld == WORLD_FOSTRAL || CurrentWorld == WORLD_GLORX){
		for(i = 0;i < MAX_TIME_SECRET;i++){
			if(!strcmp(TimeSecretName[CurrentWorld][i],MLLink->name)){
				DeviceType = TimeSecretType[CurrentWorld][i];
				DeviceP1 = TimeSecretData0[CurrentWorld][i];
				DeviceP2 = TimeSecretData1[CurrentWorld][i];
				if(!TimeSecretEnable[CurrentWorld][i])
					Enable = 0;
			};
		};
	};
	
	Time = 0;
};

void ItemGenerator::Link(void)
{
	pLink = FindSensor(DoorName)->Owner;
	if(!pLink) ErrH.Abort("Bad Door Object");
};

void ItemGenerator::Quant(void)
{
	int i;
	if(Mode == EngineModeList::WAIT){
		if(pLink->Mode != EngineModeList::WAIT){
			Time = ActiveTime;
			Mode = EngineModeList::OPEN;
		};
	}else{
		Time--;
		if(Time == 0){ 
			addDevice(SensorLink->R_curr.x,SensorLink->R_curr.y,SensorLink->R_curr.z,DeviceType,DeviceP1,DeviceP2,NULL);

			if(CurrentWorld == WORLD_FOSTRAL || CurrentWorld == WORLD_GLORX){
				for(i = 0;i < MAX_TIME_SECRET;i++){
					if(!strcmp(TimeSecretName[CurrentWorld][i],MLLink->name) && TimeSecretEnable[CurrentWorld][i]){
						TimeSecretEnable[CurrentWorld][i] = 0;
						break;
					};
				};
			};

		};
	};
};

void ItemGenerator::Close(void)
{
	if(DoorName) delete[] DoorName;
};

void ResortEnter(void)
{
	int i;
	if(!NetworkON){
		for(i = 0;i < NumEnterCenter;i++)
			EnterCenterData[i].Resort();
	};
};


SensorDataType* GetNearSensor(int y,int type)
{
	int i;
	int d,dm;
	SensorDataType * p;
	p = NULL;
	dm = 0;
	for(i = 0;i < SnsTableSize;i++){
		if(SensorSortedData[i]->SensorType == type){
			d = abs(getDistY(SensorSortedData[i]->R_curr.y,y));
			if(!p || d < dm){
				dm = d;
				p = SensorSortedData[i];
			};
		};
	};
	return p;
};

void NetworkSetLocation(int id)
{
	if(!NetworkLoadEnable) LocationData[(id & 0xffff) - 1]->NetID = id;
	LocationData[(id & 0xffff) - 1]->NetEvent();
};

void LocationEngine::NetEvent(void)
{	
	uchar ch;
	NETWORK_IN_STREAM > ch;	
	NetMode = ch;
	if(NetMode != Mode) UpdateFlag = 1;
};
