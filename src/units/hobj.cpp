#include "../global.h"

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

#include "compas.h"

#include "../sound/hsound.h"
#include "../palette.h"
#include "magnum.h"

const int TOUCH_SHIFT = 11;

int TurnSideX;
int TurnSideY;

extern iGameMap* curGMap;

uchar* ShadowColorTable;

extern uchar* palbufOrg;
extern uchar* FirePaletteTable;
extern uchar* ParticlePaletteTableSmog;
extern uchar* ParticlePaletteTableDust;
extern uchar* WaterColorTable;
extern uchar* palbufC;
extern uchar* palbufA;
extern uchar* FireColorTable;
uchar* 	palbufSrc;
uchar* TerrainAlphaTable[TERRAIN_MAX];

extern uchar* palbufBlack;

extern int DepthShow;
extern int frame;
extern int AdvancedView;

int GameOverID;
char* GameOverText;

VangerUnit* actCurrentViewObject = NULL;

void PalettePrepare(void);

int PTrack_mask_y;

int stuff_ID_offsets;
//XStream MechosLst;

//extern XStream ggg;

int LocalNetEnvironment;
int LocalStationID;
int LocalStationCounter[MAX_NID_OBJECT];

extern int NetworkON;
extern NetRndType NetRnd;


int zGameBirthTime = 0;

int GlobalStationID;

char WorldBorderEnable;

//-------------------------------------------New Source----------------------------------------------------

TrackType HideTrack;
const char* TrackName = "track0.bnl";

GameObjectDispatcher GameD;
ModelDispatcher ModelD;
ActionDispatcher ActD;
BackgroundDispatcher BackD;
ItemsDispatcher ItemD;
EffectDispatcher EffD;
LocalMapProcess MapD;

FishWarriorList FishD;
HordeSourceList HordeSourceD;
HordeList HordeD;
SkyFarmerList FarmerD;
ClefList ClefD;
BulletList BulletD;
DebrisList DebrisD;
JumpBallList JumpD;
InsectList InsectD;

PalConstructor PalCD;

int aiCutLuck;
int aiCutDominance;

/*int WorldBulletNum;
WorldBulletTemplate* WorldBulletData;*/

int GameBulletNum;
WorldBulletTemplate* GameBulletData;

int GeneralLoadReleaseFlag = 0;

int NumUnitMatrix;

extern int network_log;

uvsDolly* actCurrentViewDolly = NULL;
StuffObject* actCurrentViewStuff = NULL;

aciPromptData aiMessageBuffer;

int BulletUpdateFreq = 5;
int JumpBallUpdateFreq = 10;
int StuffUpdateFreq = 10;
int DebrisUpdateFreq = 10;
int HordeUpdateFreq = 10;

#ifdef ITEM_LOG
XStream xLog;
#endif

int ChangeWorldConstraction;
int ChangeWorldSkipQuant;

unsigned int NetGlobalTime;

extern listElem* DollyTail;

aiMessageList aiMessageQueue;

int PassageBmpNum;
int PassageBmpPrev;
PassageImageType* PassageImageData;

void OpenCyclicPal(void)
{
	int i;
	XStream fin;	
	
	if(CurrentWorld < MAIN_WORLD_MAX - 1){
		WorldPalNum = WorldTable[CurrentWorld]->escT[0]->Pbunch->cycleN;
		WorldPalData = new uchar*[WorldPalNum];
		WorldPalCurrent = WorldTable[CurrentWorld]->escT[0]->Pbunch->currentStage;		

		for(i = 0;i < WorldPalNum;i++){
			WorldPalData[i] = new uchar[768];
			fin.open(WorldTable[CurrentWorld]->escT[0]->Pbunch->cycleTable[i].pal_name,XS_IN);
			fin.read(WorldPalData[i],768);
			fin.close();
		};
	};
};

void CloseCyclicPal(void)
{
	int i;
	if(CurrentWorld < MAIN_WORLD_MAX - 1){
		for(i = 0;i < WorldPalNum;i++) {
			delete[] WorldPalData[i];
		}
		delete[] WorldPalData;
	};
	WorldPalData = NULL;
};

const int NUM_UNIQUE_ITEM = 8;
const int UNIQUE_ITEM_ID[NUM_UNIQUE_ITEM] = 
{
	UVS_ITEM_TYPE::SANDOLL_PART1,UVS_ITEM_TYPE::SANDOLL_PART2,
	UVS_ITEM_TYPE::LAST_MOGGY_PART1,UVS_ITEM_TYPE::LAST_MOGGY_PART2,
	UVS_ITEM_TYPE::QUEEN_FROG_PART2,
	UVS_ITEM_TYPE::LAWN_MOWER_PART1,UVS_ITEM_TYPE::LAWN_MOWER_PART2,
	UVS_ITEM_TYPE::WORMASTER_PART2
};

const int NUM_UNIQUE_ARTEFACT = 4;
const int UNIQUE_ARTEFACT_ID[NUM_UNIQUE_ARTEFACT] = 
{
	UVS_ITEM_TYPE::PROTRACTOR,
	UVS_ITEM_TYPE::MECHANIC_MESSIAH,
	UVS_ITEM_TYPE::FUNCTION83,
	UVS_ITEM_TYPE::SECTOR	
};

int ProtoCryptTableSize[WORLD_MAX];
SensorDataType* ProtoCryptTable[WORLD_MAX];

void InitProtoCrypt(void)
{
	int i,j,n,l;
	char sign[8];
	XStream ff;
	XBuffer xn;

//	XStream fout("mechos_p.lst",XS_OUT);

	n = RND(NUM_UNIQUE_ITEM);
	l = RND(NUM_UNIQUE_ARTEFACT);

	for(i = 0;i < WORLD_MAX;i++){
		xn.init();
		xn < "resource/crypts/crypt" <= i  < ".vlc";
		ff.open(xn.GetBuf(),XS_IN);
		ff.read(sign,strlen("VLS1"));
		if(memcmp(sign,"VLS1",strlen("VLS1"))) ErrH.Abort("Bad CryptList Format");
		ff > ProtoCryptTableSize[i];
		ProtoCryptTable[i] = new SensorDataType[ProtoCryptTableSize[i]];
		for(j = 0;j < ProtoCryptTableSize[i];j++){
			ProtoCryptTable[i][j].CreateSensor(ff);
			switch(ProtoCryptTable[i][j].SensorType){
				case 1:
				case 2:
				case 6:
				case 7:
					ProtoCryptTable[i][j].z0 = uvsGenerateItemForCrypt(ProtoCryptTable[i][j].SensorType);
					break;
				case 3:
					ProtoCryptTable[i][j].z0 = UNIQUE_ITEM_ID[n];
					n = (n + 1) % NUM_UNIQUE_ITEM;
//					fout < "World: " <= i < "\tX: " <= ProtoCryptTable[i][j].R_curr.x < "\tY:" <= ProtoCryptTable[i][j].R_curr.y < "\n";
					break;
				case 5:
					ProtoCryptTable[i][j].z0 = UNIQUE_ARTEFACT_ID[l];
					l = (l + 1) % NUM_UNIQUE_ARTEFACT;
					break;
			};
		};
		ff.close();
	};
//	fout.close();
};

void FreeProtoCrypt(void)
{
	int i,j;
	for(i = 0;i < WORLD_MAX;i++){
		for(j = 0;j < ProtoCryptTableSize[i];j++)
			ProtoCryptTable[i][j].Close();
		delete[] ProtoCryptTable[i];
	};
};

void SaveProtoCrypt(XStream& in)
{
	int i,j;
	for(i = 0;i < WORLD_MAX;i++){		
		in < ProtoCryptTableSize[i];
		for(j = 0;j < ProtoCryptTableSize[i];j++){
			in < ProtoCryptTable[i][j].z0;
			in < ProtoCryptTable[i][j].Enable;
		};
	};
};

const int OLD_CRYPT_MAX[WORLD_MAX] = {26,25,19,5,1,1,4,2,4,5};

void LoadProtoCrypt(XStream& in,int v)
{
	int i,j,s,t;
	char ch;
	switch(v){
		case 2:
		case 1:	
			for(i = 0;i < WORLD_MAX;i++){
				in > s;
				for(j = 0;j < s;j++){
					in > t;
					in > ch;
				};
			};
			break;
		case 0:
			for(i = 0;i < WORLD_MAX;i++){
				for(j = 0;j < OLD_CRYPT_MAX[i];j++){
					in > t;
					in > ch;
				};
			};
			break;
		default:
			for(i = 0;i < WORLD_MAX;i++){
				in > s;
				if(s == ProtoCryptTableSize[i]){
					for(j = 0;j < ProtoCryptTableSize[i];j++){
						in > ProtoCryptTable[i][j].z0;
						in > ProtoCryptTable[i][j].Enable;
					};
				}else{
					for(j = 0;j < s;j++){
						in > t;
						in > ch;
					};
				};
			};
			break;
	};
};

/*void PrepareProtoCrypt(void)
{
	int i,j,k,n,l;
	char sign[8];
	XStream ff[WORLD_MAX];
	XBuffer buf;
	int size[WORLD_MAX];

	int ProtoCryptTableSize;
	SensorDataType* ProtoCryptTable;

	ProtoCryptTableSize = 0;
	for(i = 0;i < WORLD_MAX;i++){
		buf.init();
		buf < "RESOURCE\\CRYPTS\\Crypt" <= i  < ".VLC";
		ff[i].open((char*)buf,XS_IN);
		ff[i].read(sign,strlen("VLS1"));
		if(memcmp(sign,"VLS1",strlen("VLS1"))) ErrH.Abort("Bad CryptList Format");
		ff[i] > size[i];
		ProtoCryptTableSize += size[i];
	};

	ProtoCryptTable = new SensorDataType[ProtoCryptTableSize];
	k = 0;
	n = RND(NUM_UNIQUE_ITEM);
	l = RND(NUM_UNIQUE_ARTEFACT);

	for(i = 0;i < WORLD_MAX;i++){
		for(j = 0;j < size[i];j++){
			ProtoCryptTable[k].CreateSensor(ff[i]);
			switch(ProtoCryptTable[k].SensorType){
				case 1:
				case 2:
				case 6:
				case 7:
					uvsCreateNewCrypt(ProtoCryptTable[k].R_curr.x,ProtoCryptTable[k].R_curr.y,ProtoCryptTable[k].R_curr.z,ProtoCryptTable[k].SensorType,i);
					break;
				case 3:
					uvsCreateNewItem(ProtoCryptTable[k].R_curr.x,ProtoCryptTable[k].R_curr.y,ProtoCryptTable[k].R_curr.z,UNIQUE_ITEM_ID[n],i);
					n = (n + 1) % NUM_UNIQUE_ITEM;
					break;
				case 4:
					uvsCreateNewItem(ProtoCryptTable[k].R_curr.x,ProtoCryptTable[k].R_curr.y,ProtoCryptTable[k].R_curr.z,ProtoCryptTable[k].z0,i);
					break;
				case 5:
					uvsCreateNewItem(ProtoCryptTable[k].R_curr.x,ProtoCryptTable[k].R_curr.y,ProtoCryptTable[k].R_curr.z,UNIQUE_ARTEFACT_ID[l],i);
					l = (l + 1) % NUM_UNIQUE_ARTEFACT;
					break;
			};
			ProtoCryptTable[k].Close();
			k++;
		};
	};
	delete ProtoCryptTable;
};*/

int aiNumMessage;
aiMessageType* aiMessageData;

int GeneralSystemSkip = 1;

int MaxOxigenBar = 0;

int NumSkipLocation[WORLD_MAX];
int RealNumLocation[WORLD_MAX];
char** SkipLocationName[WORLD_MAX];
int* NumLocationData[WORLD_MAX];
int* NumLocationID[WORLD_MAX];

extern int ChangeEnergy,ChangeArmor;

void GeneralSystemInit(void)
{
	int i,j;
	char*n;

#ifdef ITEM_LOG
	xLog.open("item.log",XS_OUT);
#endif

	GeneralMapReload = 1;
	GeneralLoadReleaseFlag = 1;

	ChangeArmor = -1;
	ChangeEnergy = -1;

	if(GeneralSystemSkip){
		InitProtoCrypt();
		CreateBoozeeniadaString();

		Parser aiData("jungle.lst");
		for(i = 0;i < AI_FACTOR_NUM;i++)
			aiFactorData[i].FactorLoad(aiData);

		for(i = 0;i < MAX_TIME_SECRET;i++){
			TimeSecretEnable[WORLD_FOSTRAL][i] = 0;
			TimeSecretEnable[WORLD_GLORX][i] = 0;
		};

		BackD.init(200);		

	#ifdef GLOBAL_TIMER
		Parser in("tgame.lst");
	#else
		Parser in("game.lst");
	#endif
		
		in.search_name("NumImage");
		PassageBmpNum = in.get_int();
		PassageImageData = new PassageImageType[PassageBmpNum];		
		for(i = 0;i < PassageBmpNum;i++){
			in.search_name("NameImage");
			PassageImageData[i].Open(in.get_name());
		};
		PassageBmpPrev = -1;

		for(i = 0;i < WORLD_MAX;i++){
			in.search_name("NumSkipML");
			NumSkipLocation[i] = in.get_int();
			in.search_name("RealNumLocation");
			RealNumLocation[i] = in.get_int();

			if(NetworkON) RealNumLocation[i] = 0;

			if(NumSkipLocation[i] > 0){
				SkipLocationName[i] = new char*[NumSkipLocation[i]];
				NumLocationData[i] = new int[NumSkipLocation[i]];
				NumLocationID[i] = new int[NumSkipLocation[i]];

				for(j =0;j < NumSkipLocation[i];j++){
					in.search_name("SkipLocationID");
					NumLocationID[i][j] = in.get_int();

					in.search_name("SkipLocationName");
					n = in.get_name();
					SkipLocationName[i][j] = new char[strlen(n) + 1];
					strcpy(SkipLocationName[i][j],n);
				};
			};
		};

		aiMessageBuffer.alloc_mem(AI_MESSAGE_MAX_STRING);

		in.search_name("NumMessage");
		aiNumMessage = in.get_int();
		aiMessageData = new aiMessageType[aiNumMessage];
		for(i = 0;i < aiNumMessage;i++)
			aiMessageData[i].Load(in);

		aiMessageQueue.Open();

		in.search_name("NumUnitMatrix");
		NumUnitMatrix = in.get_int();

		UnitMatrixData = new UnitItemMatrix[NumUnitMatrix];
		for(i = 0;i < NumUnitMatrix;i++)
			UnitMatrixData[i].Open(in);

		for(i = 0;i < NumUnitMatrix;i++)
			UnitMatrixData[i].FullNum = UnitMatrixData[i].GetFullNum();

		in.search_name("UnitTimeConst");
		UnitGlobalTime = in.get_int();

		in.search_name("WeaponWaitTime");
		WeaponWaitTime = in.get_int();

		in.search_name("MaxOxigenBar");
		MaxOxigenBar = in.get_int();

		in.search_name("TotalVangerSpeed");
		TotalVangerSpeed = in.get_int();

		in.search_name("GameBulletNumType");
		GameBulletNum = in.get_int();

		GameBulletData = new WorldBulletTemplate[GameBulletNum];
		for(i = 0;i < GameBulletNum;i++){
			GameBulletData[i].ID = i;
			GameBulletData[i].Init(in);
		};

		ModelD.Init(in);
		GameD.Init();
		MapD.Init(in);
		EffD.Init(in);
		ItemD.Init(in);
		ActD.Init(in);
	//	MechosLst.open("report.lst",XS_OUT);
		OpenCyclicPal();

		CompasObj.Init();		

		if(NetworkON){
			LocalNetEnvironment  =  LocalStationID | (CurrentWorld << 22);
		};
	};
};

uchar** WorldPalData;
int WorldPalNum;
int WorldPalCurrent;

extern int iRussian;
void aciPrepareMenus(void);

void GeneralSystemOpen(void)
{
//	DBGCHECK;
	aiCutLuck = GamerResult.luck + GamerResult.add_luck;
	aiCutDominance = GamerResult.dominance + GamerResult.add_dominance;
	if(aiCutLuck > 100) aiCutLuck = 100;
	else if(aiCutLuck < 0) aiCutLuck = 0; 		
	if(aiCutDominance > 100) aiCutDominance = 100;
	else if(aiCutDominance < -100) aiCutDominance = -100;

	if(GeneralSystemSkip){
		ChangeWorldConstraction = -1;
		ChangeWorldSkipQuant = 0;			
		aciPrepareMenus();

		PalCD.Init();

		if(CurrentWorld < MAIN_WORLD_MAX - 1) WorldBorderEnable = 1;
		else WorldBorderEnable = 0;
		
		PTrack_mask_y = (map_size_y << 8) - 1;

	#ifdef GLOBAL_TIMER
		Parser in(GetTargetName("tlevel.lst"));
	#else
		Parser in(GetTargetName("level.lst"));
	#endif

		StaticOpen();

//zNfo many game inits
		
		in.search_name("TrackName");
		HideTrack.Open(in.get_name());
		
		GameD.Open();
		MapD.Open(in);

		ItemD.Open(in);

		FishD.Init();			//zNfo инициализация акул
		HordeSourceD.Init();	//zNfo инициализацыя пассывных мухов
		HordeD.Init();			//zNfo инициализацыя актывных мухов

		FarmerD.Init();
		ClefD.Init();
		BulletD.Init();
		DebrisD.Init();
		JumpD.Init();
		InsectD.Init();

		EffD.Open(in);
		ActD.Open(in);

		ResortEnter();

		if(iRussian){
			switch(CurrentWorld){
				case WORLD_FOSTRAL:
//zNfo - add target 2 compass
					AddTarget2Compas(CMP_TARGET_SPOT,(void*)("Incubator"),rCmpIncubator);
					AddTarget2Compas(CMP_TARGET_ESCAVE,(void*)("Podish"),rCmpPodish);
					AddTarget2Compas(CMP_TARGET_SENSOR,(void*)("ArmorUpdate1"),rCmpRepair1);
					AddTarget2Compas(CMP_TARGET_SENSOR,(void*)("ArmorUpdate2"),rCmpRepair2);
					AddTarget2Compas(CMP_TARGET_SENSOR,(void*)("FlyUpdate1"),rCmpRigs);
					AddTarget2Compas(CMP_TARGET_SENSOR,(void*)("FireUpdate1"),rCmpGhOrb);
					AddTarget2Compas(CMP_TARGET_SENSOR,(void*)("RandomUpdate1"),rCmpBroken1);
					AddTarget2Compas(CMP_TARGET_SENSOR,(void*)("RandomUpdate2"),rCmpBroken2);
					AddTarget2Compas(CMP_TARGET_SENSOR,(void*)("KeyUpdate1"),rCmpSpiral);
					AddTarget2Compas(CMP_TARGET_PASSAGE,(void*)("F2G"),rCmpPassGlorx);
					AddTarget2Compas(CMP_TARGET_PASSAGE,(void*)("F2W"),rCmpPassWeexow);
					if(GamerResult.earth_unable)
						AddTarget2Compas(CMP_TARGET_SENSOR,(void*)("PASS83"),rCmpPassEarth);
					break;
				case WORLD_GLORX:
					AddTarget2Compas(CMP_TARGET_SPOT,(void*)("Ogorod"),rCmpOgorod);
					AddTarget2Compas(CMP_TARGET_SPOT,(void*)("Lampasso"),rCmpLampasso);
					AddTarget2Compas(CMP_TARGET_ESCAVE,(void*)("VigBoo"),rCmpVigBoo);
					AddTarget2Compas(CMP_TARGET_PASSAGE,(void*)("G2F"),rCmpPassFostral);
					AddTarget2Compas(CMP_TARGET_PASSAGE,(void*)("G2N"),rCmpPassNecross);
					AddTarget2Compas(CMP_TARGET_PASSAGE,(void*)("G2X"),rCmpPassXplo);
					AddTarget2Compas(CMP_TARGET_PASSAGE,(void*)("G2K"),rCmpPassKhox);
					AddTarget2Compas(CMP_TARGET_SENSOR,(void*)("FlyUpdate01"),rCmpRigs1);
					AddTarget2Compas(CMP_TARGET_SENSOR,(void*)("FlyUpdate02"),rCmpRigs2);
					AddTarget2Compas(CMP_TARGET_SENSOR,(void*)("ArmorUpdate01"),rCmpRepair1);
					AddTarget2Compas(CMP_TARGET_SENSOR,(void*)("ArmorUpdate02"),rCmpRepair2);
					AddTarget2Compas(CMP_TARGET_SENSOR,(void*)("FireUpdate01"),rCmpGhOrb);
					AddTarget2Compas(CMP_TARGET_SENSOR,(void*)("KeyUpdate01"),rCmpSpiral);
					break;
				case WORLD_NECROSS:
					AddTarget2Compas(CMP_TARGET_SPOT,(void*)("B-Zone"),rCmpBZone);
					AddTarget2Compas(CMP_TARGET_ESCAVE,(void*)("ZeePa"),rCmpZeePa);
					AddTarget2Compas(CMP_TARGET_PASSAGE,(void*)("N2G"),rCmpPassGlorx);
					AddTarget2Compas(CMP_TARGET_PASSAGE,(void*)("N2B"),rCmpPassBoozeena);
					AddTarget2Compas(CMP_TARGET_PASSAGE,(void*)("N2A"),rCmpPassArk);
					AddTarget2Compas(CMP_TARGET_SENSOR,(void*)("FlyUpdate01"),rCmpRigs);
					AddTarget2Compas(CMP_TARGET_SENSOR,(void*)("ArmorUpdate01"),rCmpRepair);
					AddTarget2Compas(CMP_TARGET_SENSOR,(void*)("KeyUpdate01"),rCmpSpiral);
					AddTarget2Compas(CMP_TARGET_SENSOR,(void*)("RandomUpdate01"),rCmpBroken);
					break;
				case WORLD_XPLO:
					AddTarget2Compas(CMP_TARGET_SPOT,(void*)("Spobs"),rCmpSpobs);
					AddTarget2Compas(CMP_TARGET_PASSAGE,(void*)("X2G"),rCmpPassGlorx);
					AddTarget2Compas(CMP_TARGET_PASSAGE,(void*)("X2T"),rCmpPassThreall);
					break;
				case WORLD_KHOX:
					AddTarget2Compas(CMP_TARGET_PASSAGE,(void*)("K2G"),rCmpPassGlorx);
					break;
				case WORLD_BOOZEENA:
					AddTarget2Compas(CMP_TARGET_PASSAGE,(void*)("B2N"),rCmpPassNecross);
					break;
				case WORLD_WEEXOW:
					AddTarget2Compas(CMP_TARGET_PASSAGE,(void*)("W2F"),rCmpPassFostral);
					break;
				case WORLD_THREALL:
					AddTarget2Compas(CMP_TARGET_PASSAGE,(void*)("T2X"),rCmpPassXplo);
					break;
				case WORLD_ARKONOY:
					AddTarget2Compas(CMP_TARGET_PASSAGE,(void*)("A2N"),rCmpPassNecross);
					break;
			};
		}else{
			switch(CurrentWorld){
				case WORLD_FOSTRAL:
					AddTarget2Compas(CMP_TARGET_SPOT,(void*)("Incubator"),eCmpIncubator);
					AddTarget2Compas(CMP_TARGET_ESCAVE,(void*)("Podish"),eCmpPodish);
					AddTarget2Compas(CMP_TARGET_SENSOR,(void*)("ArmorUpdate1"),eCmpRepair1);
					AddTarget2Compas(CMP_TARGET_SENSOR,(void*)("ArmorUpdate2"),eCmpRepair2);
					AddTarget2Compas(CMP_TARGET_SENSOR,(void*)("FlyUpdate1"),eCmpRigs);
					AddTarget2Compas(CMP_TARGET_SENSOR,(void*)("FireUpdate1"),eCmpGhOrb);
					AddTarget2Compas(CMP_TARGET_SENSOR,(void*)("RandomUpdate1"),eCmpBroken1);
					AddTarget2Compas(CMP_TARGET_SENSOR,(void*)("RandomUpdate2"),eCmpBroken2);
					AddTarget2Compas(CMP_TARGET_SENSOR,(void*)("KeyUpdate1"),eCmpSpiral);
					AddTarget2Compas(CMP_TARGET_PASSAGE,(void*)("F2G"),eCmpPassGlorx);
					AddTarget2Compas(CMP_TARGET_PASSAGE,(void*)("F2W"),eCmpPassWeexow);
					if(GamerResult.earth_unable)
						AddTarget2Compas(CMP_TARGET_SENSOR,(void*)("PASS83"),eCmpPassEarth);
					break;
				case WORLD_GLORX:
					AddTarget2Compas(CMP_TARGET_SPOT,(void*)("Ogorod"),eCmpOgorod);
					AddTarget2Compas(CMP_TARGET_SPOT,(void*)("Lampasso"),eCmpLampasso);
					AddTarget2Compas(CMP_TARGET_ESCAVE,(void*)("VigBoo"),eCmpVigBoo);
					AddTarget2Compas(CMP_TARGET_PASSAGE,(void*)("G2F"),eCmpPassFostral);
					AddTarget2Compas(CMP_TARGET_PASSAGE,(void*)("G2N"),eCmpPassNecross);
					AddTarget2Compas(CMP_TARGET_PASSAGE,(void*)("G2X"),eCmpPassXplo);
					AddTarget2Compas(CMP_TARGET_PASSAGE,(void*)("G2K"),eCmpPassKhox);
					AddTarget2Compas(CMP_TARGET_SENSOR,(void*)("FlyUpdate01"),eCmpRigs1);
					AddTarget2Compas(CMP_TARGET_SENSOR,(void*)("FlyUpdate02"),eCmpRigs2);
					AddTarget2Compas(CMP_TARGET_SENSOR,(void*)("ArmorUpdate01"),eCmpRepair1);
					AddTarget2Compas(CMP_TARGET_SENSOR,(void*)("ArmorUpdate02"),eCmpRepair2);
					AddTarget2Compas(CMP_TARGET_SENSOR,(void*)("FireUpdate01"),eCmpGhOrb);
					AddTarget2Compas(CMP_TARGET_SENSOR,(void*)("KeyUpdate01"),eCmpSpiral);
					break;
				case WORLD_NECROSS:
					AddTarget2Compas(CMP_TARGET_SPOT,(void*)("B-Zone"),eCmpBZone);
					AddTarget2Compas(CMP_TARGET_ESCAVE,(void*)("ZeePa"),eCmpZeePa);
					AddTarget2Compas(CMP_TARGET_PASSAGE,(void*)("N2G"),eCmpPassGlorx);
					AddTarget2Compas(CMP_TARGET_PASSAGE,(void*)("N2B"),eCmpPassBoozeena);
					AddTarget2Compas(CMP_TARGET_PASSAGE,(void*)("N2A"),eCmpPassArk);
					AddTarget2Compas(CMP_TARGET_SENSOR,(void*)("FlyUpdate01"),eCmpRigs);
					AddTarget2Compas(CMP_TARGET_SENSOR,(void*)("ArmorUpdate01"),eCmpRepair);
					AddTarget2Compas(CMP_TARGET_SENSOR,(void*)("KeyUpdate01"),eCmpSpiral);
					AddTarget2Compas(CMP_TARGET_SENSOR,(void*)("RandomUpdate01"),eCmpBroken);
					break;
				case WORLD_XPLO:
					AddTarget2Compas(CMP_TARGET_SPOT,(void*)("Spobs"),eCmpSpobs);
					AddTarget2Compas(CMP_TARGET_PASSAGE,(void*)("X2G"),eCmpPassGlorx);
					AddTarget2Compas(CMP_TARGET_PASSAGE,(void*)("X2T"),eCmpPassThreall);
					break;
				case WORLD_KHOX:
					AddTarget2Compas(CMP_TARGET_PASSAGE,(void*)("K2G"),eCmpPassGlorx);
					break;
				case WORLD_BOOZEENA:
					AddTarget2Compas(CMP_TARGET_PASSAGE,(void*)("B2N"),eCmpPassNecross);
					break;
				case WORLD_WEEXOW:
					AddTarget2Compas(CMP_TARGET_PASSAGE,(void*)("W2F"),eCmpPassFostral);
					break;
				case WORLD_THREALL:
					AddTarget2Compas(CMP_TARGET_PASSAGE,(void*)("T2X"),eCmpPassXplo);
					break;
				case WORLD_ARKONOY:
					AddTarget2Compas(CMP_TARGET_PASSAGE,(void*)("A2N"),eCmpPassNecross	);
					break;
			};
/*			switch(CurrentWorld){
				case WORLD_FOSTRAL:
					AddTarget2Compas(CMP_TARGET_SPOT,(void*)("Incubator"),"Incubator");
					AddTarget2Compas(CMP_TARGET_ESCAVE,(void*)("Podish"),"Podish");
					AddTarget2Compas(CMP_TARGET_SENSOR,(void*)("ArmorUpdate1"),"Repair Station I");
					AddTarget2Compas(CMP_TARGET_SENSOR,(void*)("ArmorUpdate2"),"Repair Station II");
					AddTarget2Compas(CMP_TARGET_SENSOR,(void*)("FlyUpdate1"),"Rigs Station");
					AddTarget2Compas(CMP_TARGET_SENSOR,(void*)("FireUpdate1"),"ghOrb Station");
					AddTarget2Compas(CMP_TARGET_SENSOR,(void*)("RandomUpdate1"),"Broken Station I");
					AddTarget2Compas(CMP_TARGET_SENSOR,(void*)("RandomUpdate2"),"Broken Station II");
					AddTarget2Compas(CMP_TARGET_SENSOR,(void*)("KeyUpdate1"),"Spiral Station");
					AddTarget2Compas(CMP_TARGET_PASSAGE,(void*)("F2G"),"Passage to Glorx");
					AddTarget2Compas(CMP_TARGET_PASSAGE,(void*)("F2W"),"Passage to  Weexow");
					break;
				case WORLD_GLORX:
					AddTarget2Compas(CMP_TARGET_SPOT,(void*)("Ogorod"),"Ogorod");
					AddTarget2Compas(CMP_TARGET_SPOT,(void*)("Lampasso"),"Lampasso");
					AddTarget2Compas(CMP_TARGET_ESCAVE,(void*)("VigBoo"),"VigBoo");
					AddTarget2Compas(CMP_TARGET_PASSAGE,(void*)("G2F"),"Passage to  Fostral");
					AddTarget2Compas(CMP_TARGET_PASSAGE,(void*)("G2N"),"Passage to  Necross");
					AddTarget2Compas(CMP_TARGET_PASSAGE,(void*)("G2X"),"Passage to  Xplo");
					AddTarget2Compas(CMP_TARGET_PASSAGE,(void*)("G2K"),"Passage to  Khox");
					AddTarget2Compas(CMP_TARGET_SENSOR,(void*)("FlyUpdate01"),"Rigs Station I");
					AddTarget2Compas(CMP_TARGET_SENSOR,(void*)("FlyUpdate02"),"Rigs Station II");
					AddTarget2Compas(CMP_TARGET_SENSOR,(void*)("ArmorUpdate01"),"Repair Station I");
					AddTarget2Compas(CMP_TARGET_SENSOR,(void*)("ArmorUpdate02"),"Repair Station II");
					AddTarget2Compas(CMP_TARGET_SENSOR,(void*)("FireUpdate01"),"ghOrb Station");
					AddTarget2Compas(CMP_TARGET_SENSOR,(void*)("KeyUpdate01"),"Spiral Station");
					break;
				case WORLD_NECROSS:
					AddTarget2Compas(CMP_TARGET_SPOT,(void*)("B-Zone"),"B-Zone");
					AddTarget2Compas(CMP_TARGET_ESCAVE,(void*)("ZeePa"),"ZeePa");
					AddTarget2Compas(CMP_TARGET_PASSAGE,(void*)("N2G"),"Passage to  Glorx");
					AddTarget2Compas(CMP_TARGET_PASSAGE,(void*)("N2B"),"Passage to  Boozeena");
					AddTarget2Compas(CMP_TARGET_PASSAGE,(void*)("N2A"),"Passage to  Ark'a'Znoy");
					AddTarget2Compas(CMP_TARGET_SENSOR,(void*)("FlyUpdate01"),"Rigs Station");
					AddTarget2Compas(CMP_TARGET_SENSOR,(void*)("ArmorUpdate01"),"Repair Station");
					AddTarget2Compas(CMP_TARGET_SENSOR,(void*)("KeyUpdate01"),"Spiral Station");
					AddTarget2Compas(CMP_TARGET_SENSOR,(void*)("RandomUpdate01"),"Broken Station");
					break;
				case WORLD_XPLO:
					AddTarget2Compas(CMP_TARGET_SPOT,(void*)("Spobs"),"Spobs");
					AddTarget2Compas(CMP_TARGET_PASSAGE,(void*)("X2G"),"Passage to Glorx");
					AddTarget2Compas(CMP_TARGET_PASSAGE,(void*)("X2T"),"Passage to Threall");
					break;
				case WORLD_KHOX:
					AddTarget2Compas(CMP_TARGET_PASSAGE,(void*)("K2G"),"Passage to Glorx");
					break;
				case WORLD_BOOZEENA:
					AddTarget2Compas(CMP_TARGET_PASSAGE,(void*)("B2N"),"Passage to Necross");
					break;
				case WORLD_WEEXOW:
					AddTarget2Compas(CMP_TARGET_PASSAGE,(void*)("W2F"),"Passage to Fostral");
					break;
				case WORLD_THREALL:
					AddTarget2Compas(CMP_TARGET_PASSAGE,(void*)("T2X"),"Passage to  Xplo");
					break;
				case WORLD_ARKONOY:
					AddTarget2Compas(CMP_TARGET_PASSAGE,(void*)("A2N"),"Passage to  Necross");
					break;
			};*/
		};

		SelectCompasTarget(NULL);
		CompasObj.Open();

		uvsKronActive = 1;
		actCurrentViewObject = NULL;
		actCurrentViewDolly = NULL;
		actCurrentViewStuff = NULL;

		if(NetworkON) NetworkWorldOpen();
	}	
};

extern double scale_general;

void GeneralSystemFree(void)
{
	int i,j;
	if(GeneralSystemSkip){
		if(CurrentWorld == WORLD_KHOX)
			scale_general *= 2.;

		CompasObj.Free();

		CloseCyclicPal();
		ActD.Free();
		EffD.Free();
		ItemD.Free();
		MapD.Free();
		GameD.Free();
		ModelD.Free();
		BackD.free();
		
		delete[] GameBulletData;
		
		aiMessageQueue.Close();
		delete[] aiMessageData;
		aiMessageBuffer.free_mem();
		
		for(i = 0;i < NumUnitMatrix;i++)
			UnitMatrixData[i].Close();
		delete[] UnitMatrixData;

		FreeProtoCrypt();


		for(i = 0;i < WORLD_MAX;i++){
			if(NumSkipLocation[i]){
				for(j =0;j < NumSkipLocation[i];j++)
					delete[] SkipLocationName[i][j];
				delete[] SkipLocationName[i];
				delete[] NumLocationData[i];
				delete[] NumLocationID[i];
			};
		};
		delete[] CHECK_BSIGN_DATA; 
	};

	for(i = 0;i < PassageBmpNum;i++)
		PassageImageData[i].Close();
	delete[] PassageImageData;

	

	if(NetworkON){
		if(my_server_data.GameType == PASSEMBLOSS){
			if(GloryPlaceData){
				delete GloryPlaceData;
				GloryPlaceData = NULL;
			};
		};
		
		total_players_data_list_query();
		disconnect_from_server();
		NetworkON = 0;
	};
//	for(i = 0;i < WORLD_MAX;i++)
//		delete SKIP_LOCATION_NAME[i];

#ifdef ITEM_LOG
	xLog.close();
#endif
	
//	DBGCHECK;
};

void GeneralSystemClose(void)
{
	int i;
	if(GeneralSystemSkip){
		aciClearTeleportMenu();

		uvsKronActive = 0;
		
		if(NetworkON && my_server_data.GameType == PASSEMBLOSS){
			for(i = 0;i < GloryPlaceNum;i++)
				GloryPlaceData[i].CloseWorld();
		};

//!!!!!!!!!!!
//		StorageLog < "\nStartClose";
//!!!!!!!!!!!

		CompasObj.Close();

//!!!!!!!!!!!
//		StorageLog < "\nActiveClose";
//!!!!!!!!!!!

		ActD.Close();

//!!!!!!!!!!!
//		StorageLog < "\nEffectClose";
//!!!!!!!!!!!

		EffD.Close();

//!!!!!!!!!!!
//		StorageLog < "\nItemClose";
//!!!!!!!!!!!

		ItemD.Close();

//!!!!!!!!!!!
//		StorageLog < "\nOtherListClose";
//!!!!!!!!!!!

		InsectD.Free();
		FishD.Free();
		HordeSourceD.Free();
		HordeD.Free();
		FarmerD.Free();
		ClefD.Free();
		BulletD.Free();
		DebrisD.Free();
		JumpD.Free();

//!!!!!!!!!!!
//		StorageLog < "\nMapClose";
//!!!!!!!!!!!

		MapD.Close();

//!!!!!!!!!!!
//		StorageLog < "\nGameClose";
//!!!!!!!!!!!

		GameD.Close();

//!!!!!!!!!!!
//		StorageLog < "\nTrackClose";
//!!!!!!!!!!!

		HideTrack.Close();
		BackD.close();

//!!!!!!!!!!!
//		StorageLog < "\nStaticClose";
//!!!!!!!!!!!

		StaticClose();

		PalCD.Free();

//!!!!!!!!!!!
//		StorageLog < "\nNetworkClose";
//!!!!!!!!!!!

		if(NetworkON) NetworkWorldClose();
//!!!!!!!!!!!
//		StorageLog < "\nEndClose";
//!!!!!!!!!!!

	};
};

void GeneralObject::Init(StorageType* s)
{
	Storage = s;
	Cluster = NULL;
	ObjectHeap = NULL;
	NetID = 0;
};

void GeneralObject::Init(void)
{
	Storage = NULL;
	Cluster = NULL;
	ObjectHeap = NULL;
	NetID = 0;	
};

void GeneralObject::Init(MemoryStorageType* s)
{
	ObjectHeap = s;
//	Cluster = NULL;
	Storage = NULL;
	NetID = 0;
};

void GeneralObject::Free(void){ };
void GeneralObject::Open(void){ };
void GeneralObject::Close(void){ };
void GeneralObject::Quant(void){ };
void GeneralObject::Touch(GeneralObject* p){ };

void GeneralObject::Send(void)
{
};

void BaseObject::DrawQuant(void){ };
void Object::DestroyCollision(int l,Object* p){ };

GeneralObject* StorageType::Active(void)
{
/*	int i;
	for(i = 0;i < ObjectPointer - 1;i++)
		if(Data[i] == Data[ObjectPointer - 1])
			ErrH.Abort("Bad Object Storage");*/


	if(ObjectPointer > 0) return Data[--ObjectPointer];
	else
#ifdef STACK_EMPTY_IGNORE
		return NULL;
#else
		ErrH.Abort("Object stack empty");
#endif

};

void StorageType::Deactive(GeneralObject* p)
{
/*	int i;
	for(i = 0;i < ObjectPointer;i++)
		if(Data[i] == p){
			ErrH.Abort("Bad Object Storage");
		};*/
	if(ObjectPointer < Max) Data[ObjectPointer++] = p;
	else ErrH.Abort("Object stack overflow");
};

void StorageType::Init(int _max)
{
	Data = new GeneralObject*[Max = _max];
	ObjectPointer = 0;
};

GeneralObject* StorageType::GetAll(void)
{
	if(ObjectPointer > 0) return Data[--ObjectPointer];
	return NULL;
};

void StorageType::Free(void)
{
//	Check();
	delete[] Data;
};


//!!!!!!!!!!!!
//XStream StorageLog("storage.log",XS_OUT);
//!!!!!!!!!!!

void StorageType::Check(void)
{
//!!!!!!!!!!!
//		StorageLog < "\nMax : " <= Max < "\nPoint : " <= ObjectPointer;
//!!!!!!!!!!!

//	if(ObjectPointer != Max)
//		ErrH.Abort("Bad Storage Free");
};

void UnitList::Init(Parser& in)
{
	Num = 0;
	Tail = NULL;
};

void UnitList::Open(Parser& in)
{
	Num = 0;
	Tail = NULL;
};

/*void UnitList::ConnectTypeList(GeneralObject* p)
{
	if(!Tail){
		p -> NextTypeList = p -> PrevTypeList = NULL;
		Tail = p;
	}else{
		Tail -> PrevTypeList = p;
		p -> NextTypeList = Tail;
		p -> PrevTypeList = NULL;
		Tail = p;
	};
	Num++;
};*/

void UnitList::DisconnectTypeList(GeneralObject* p)
{
	if((!p->PrevTypeList) && (!p->NextTypeList)) Tail = NULL;
	else{
		if(p->PrevTypeList) p->PrevTypeList->NextTypeList = p->NextTypeList;
		else Tail = p->NextTypeList;
		if(p->NextTypeList) p->NextTypeList->PrevTypeList = p->PrevTypeList;
	};
	Num--;
};

void UnitList::ConnectObject(BaseObject* p)
{
	ConnectTypeList(p);
	GameD.ConnectBaseList(p);
};

void GameObjectDispatcher::Init(void)
{
	Num = 0;
	Tail = NULL;
	ViewTail = NULL;
};

void GameObjectDispatcher::Free(void)
{
//	if(Num != 0 || Tail) ErrH.Abort("Bad Game Dispatcher Free");
};

void GameObjectDispatcher::Open(void)
{
	ViewTail = NULL;
	Num = 0;
	Tail = NULL;
	FirstQuant = 1;
	cWorld = CurrentWorld;

//	if(Num != 0  || Tail) ErrH.Abort("Bad Game Dispatcher Open");
};

void GeneralObject::TimeOutQuant(void)
{
};

void GameObjectDispatcher::Close(void)
{
//	if(Num != 0  || Tail) ErrH.Abort("Bad Game Dispatcher Close");
};

void GameObjectDispatcher::ConnectBaseList(BaseObject* p)
{
	if(!Tail){
		p->NextBaseList = p->PrevBaseList = NULL;
		Tail = p;
	}else{
		Tail -> PrevBaseList = p;
		p -> NextBaseList = Tail;
		p -> PrevBaseList = NULL;
		Tail = p;
	};
	Num++;
};

void GameObjectDispatcher::DisconnectBaseList(BaseObject* p)
{
	if((!p->PrevBaseList) && (!p->NextBaseList)) Tail = NULL;
	else{
		if(p->PrevBaseList) p->PrevBaseList->NextBaseList = p->NextBaseList;
		else Tail = p->NextBaseList;
		if(p->NextBaseList) p->NextBaseList->PrevBaseList = p->PrevBaseList;
	};
	Num--;
};

/*void GameObjectDispatcher::Quant(void)
{
//	ggg < "\nCurrentWorld\t" <= CurrentWorld;

	if(TurnAngle == 0 && DepthShow == 0) AdvancedView = 0;
	else AdvancedView = 1;

	StaticQuant();

	MapD.Quant();
	ActD.Quant();
	ItemD.Quant();
	EffD.Quant();

	ViewTail = NULL;
	Sort();
};*/


int uvsMechosType_to_AciInt(int);

void GameObjectDispatcher::Quant(void)
{
//	ggg < "\nCurrentWorld\t" <= CurrentWorld;
	VangerUnit* p;
	int i;

	int time;
	if(TurnAngle == 0 && DepthShow == 0) AdvancedView = 0;
	else AdvancedView = 1;

#ifdef _DEBUG
//	DBGCHECK;
#endif	

	NetGlobalTime = GLOBAL_CLOCK();	

	if(NetworkON)
		CheckPlayerList();			

	if(FirstQuant){
		i = TELEPORT_ESCAVE_ID + 1;
		p = (VangerUnit*)(ActD.Tail);
		while(p){
			if(p->uvsPoint->shape == UVS_VANGER_SHAPE::GAMER_SLAVE){
				aciAddTeleportMenuItem(uvsMechosType_to_AciInt(p->uvsPoint->Pmechos->type),i);
				p->VangerCloneID = i;
				i++;
			};
			p = (VangerUnit*)(p->NextTypeList);
		};

		if(CurrentWorld  < MAIN_WORLD_MAX - 1 && (i - TELEPORT_ESCAVE_ID - 1) <= 5) aciAddTeleportMenuItem(-1,TELEPORT_ESCAVE_ID);

		GlobalTime = GLOBAL_CLOCK();
		FirstQuant = 0;

		if(uvsKronActive){
			for(i = 0;i < MAX_ACTIVE_SLOT;i++)
				if(ActD.Slot[i] && ActD.Slot[i]->StuffType == DEVICE_ID_GUN){
					uvsCheckKronIventTabuTask(UVS_KRON_EVENT::WEAPON_ON,1);	
					break;
				};	
		};
	}else{
		time = GLOBAL_CLOCK();
	};	

#ifdef _DEBUG
//	DBGCHECK;
#endif

	if(ActD.Active)
		ActD.Active->NewKeyHandler();

	p = (VangerUnit*)(ActD.Tail);
	while(p){
		p->MapQuant();
		p = (VangerUnit*)(p->NextTypeList);
	};

	if(NetworkON && my_server_data.GameType == PASSEMBLOSS && UsedCheckNum < GloryPlaceNum)
		GloryPlaceData[UsedCheckNum].Quant();

	StaticQuant();

	MapD.Quant();

#ifdef _DEBUG
//	DBGCHECK;
#endif
	ActD.Quant(); 	

#ifdef _DEBUG
//	DBGCHECK;
#endif
	ItemD.Quant();

#ifdef _DEBUG
//	DBGCHECK;
#endif
	InsectD.Quant();
	FishD.Quant();
	HordeSourceD.Quant();
	HordeD.Quant();
	FarmerD.Quant();
	ClefD.Quant();
	BulletD.Quant();
	DebrisD.Quant();
	JumpD.Quant();
	EffD.Quant();

//znfo ai quant
	aiMessageQueue.Quant();

#ifdef _DEBUG
//	DBGCHECK;
#endif

	ViewTail = NULL;
	Sort();	

#ifdef _DEBUG
//	DBGCHECK;
#endif
};

extern int XSIDE;
extern int YSIDE;

extern int TotalDrawFlag;

void GameObjectDispatcher::DrawQuant(void) {
#ifdef _DEBUG
	if(!TotalDrawFlag) return;
#endif

	BaseObject* p;
	p = ViewTail;
	while(p){
		p->DrawQuant();
		p = p->NextViewList;
	};

//	ActD.CameraQuant();
	ActD.DrawResource();

	//znfo network
	if(NetworkON) {
		CreatePhantomTarget();
	}
	CreateTabutaskTarget();
	CompasObj.Quant();	
};

void setMapPixel(int px,int py,int col)
{
	int xx,yy;
	G2L(px,py,xx,yy);
	if(xx < UcutLeft + 1 || xx > UcutRight - 1 || yy < VcutUp + 1 || yy > VcutDown - 1) return;
	uchar c = *(vMap->lineTcolor[py] + px) + col;
	if(c > 255) c = 255;
	*(XGR_GetVideoLine(0) + yy*XGR_MAXX + xx) = c;
};

void putMapPixel(int px,int py,int col)
{
	int xx,yy;
	G2L(px,py,xx,yy);
	if(xx < UcutLeft + 1 || xx > UcutRight - 1 || yy < VcutUp + 1 || yy > VcutDown - 1) return;
	*(XGR_GetVideoLine(0) + yy*XGR_MAXX + xx) = col;
};

/*void line_trace(const Vector& c1,const Vector& c2)
{
	Vector dc;
	Vector k;
	dc.x = getDistX(c2.x,c1.x);
	dc.y = getDistY(c2.y,c1.y);
	dc.z = c2.z - c1.z;

	if(abs(dc.z) > abs(dc.y) && abs(dc.z) > abs(dc.x)){
		c1.x <<= FIXED_SHIFT;
		c1.y <<= FIXED_SHIFT;

		if(dc.z < 0){
			dc.z = -dc.z;
			k.z = -1;
		}else
			k.z = 1;

		if(dc.z){
			k.x = (dc.x << FIXED_SHIFT) / dc.z;
			k.y = (dc.y << FIXED_SHIFT) / dc.z;
		}else{
			k.x = dc.x << FIXED_SHIFT;
			k.y = dc.y << FIXED_SHIFT;
		};

		while(dc.z > 0){
			c1.x &= PTrack_mask_x;
			c1.y &= PTrack_mask_y;
			putMapPixel(c1.x >> FIXED_SHIFT,c1.y >> FIXED_SHIFT,c1.z);
			c1.x += k.x;
			c1.y += k.y;
			c1.z += k.z;
			dc.z--;
		};
	}else{
		c1.z <<= FIXED_SHIFT;
		if(abs(dc.x) > abs(dc.y)){
			if(dc.x < 0){
				dc.x = -dc.x;
				k.x = -1;
			}else
				k.x = 1;

			if(dc.x){
				k.z = (dc.z << FIXED_SHIFT) / dc.x;
				k.y = (dc.y << FIXED_SHIFT) / dc.x;
			}else{
				k.z = dc.z << FIXED_SHIFT;
				k.y = dc.y << FIXED_SHIFT;
			};

			c1.y <<= FIXED_SHIFT;

			while(dc.x > 0){
				c1.x &= clip_mask_x;
				c1.y &= PTrack_mask_y;
				putMapPixel(c1.x,c1.y >> FIXED_SHIFT,c1.z >> FIXED_SHIFT);
				c1.x += k.x;
				c1.y += k.y;
				c1.z += k.z;
				dc.x--;
			};
		}else{
			if(dc.y < 0){
				dc.y = -dc.y;
				k.y = -1;
			}else
				k.y = 1;

			if(dc.y){
				k.x = (dc.x << FIXED_SHIFT) / dc.y;
				k.z = (dc.z  << FIXED_SHIFT) / dc.y;
			}else{
				k.x = dc.x << FIXED_SHIFT;
				k.z = dc.z << FIXED_SHIFT;
			};

			c1.x <<= FIXED_SHIFT;

			while(dc.y > 0){
				c1.x &= PTrack_mask_x;
				c1.y &= clip_mask_y;
				putMapPixel(c1.x >> FIXED_SHIFT,c1.y,c1.z >> FIXED_SHIFT);
				c1.x += k.x;
				c1.y += k.y;
				c1.z += k.z;
				dc.y--;
			};
		};
	};
};*/

int MapLineTrace(Vector& c1,Vector& c2)
{
	Vector dc;
	Vector k;
	dc.x = getDistX(c2.x,c1.x);
	dc.y = getDistY(c2.y,c1.y);
	dc.z = c2.z - c1.z;

	if(dc.x == 0 && dc.y == 0 && dc.z == 0) return 0;

	if(abs(dc.z) > abs(dc.y) && abs(dc.z) > abs(dc.x)){
		c1.x <<= FIXED_SHIFT;
		c1.y <<= FIXED_SHIFT;

		if(dc.z < 0){
			dc.z = -dc.z;
			k.z = -1;
		}else
			k.z = 1;

		if(dc.z){
			k.x = (dc.x << FIXED_SHIFT) / dc.z;
			k.y = (dc.y << FIXED_SHIFT) / dc.z;
		}else{
			k.x = dc.x << FIXED_SHIFT;
			k.y = dc.y << FIXED_SHIFT;
		};
		if(dc.x < 0) dc.x = -dc.x;
		if(dc.y < 0) dc.y = -dc.y;

		while(dc.z > 0){
			c1.x &= PTrack_mask_x;
			c1.y &= PTrack_mask_y;
//			if(c1.z <= getAlt(c1.x >> 16,c1.y >> 16)){
			if(GetCollisionMap(c1.x >> FIXED_SHIFT,c1.y >> FIXED_SHIFT,c1.z)){
				 c1.x >>= FIXED_SHIFT;
				 c1.y >>= FIXED_SHIFT;
				 return 1;
			};
			c1.x += k.x;
			c1.y += k.y;
			c1.z += k.z;
			dc.z--;
		};
	}else{
		c1.z <<= FIXED_SHIFT;
		if(abs(dc.x) > abs(dc.y)){
			c1.y <<= FIXED_SHIFT;
			if(dc.x < 0){
				dc.x = -dc.x;
				k.x = -1;
			}else
				k.x = 1;

			if(dc.x){
				k.y = (dc.y << FIXED_SHIFT) / dc.x;
				k.z = (dc.z << FIXED_SHIFT) / dc.x;
			}else{
				k.y = dc.y << FIXED_SHIFT;
				k.z = dc.z << FIXED_SHIFT;
			};
			if(dc.z < 0) dc.z = -dc.z;
			if(dc.y < 0) dc.y = -dc.y;

			while(dc.x > 0){
				c1.x &= clip_mask_x;
				c1.y &= PTrack_mask_y;
//				if((c1.z >> 16) <= getAlt(c1.x,c1.y >> 16)){
				if(GetCollisionMap(c1.x,c1.y >> FIXED_SHIFT,c1.z >> FIXED_SHIFT)){
					c1.y >>= FIXED_SHIFT;
					c1.z >>= FIXED_SHIFT;
					return 1;
				};
				c1.x += k.x;
				c1.y += k.y;
				c1.z += k.z;
				dc.x--;
			};
		}else{
			c1.x <<= FIXED_SHIFT;

			if(dc.y < 0){
				dc.y = -dc.y;
				k.y = -1;
			}else
				k.y = 1;

			if(dc.y){
				k.x = (dc.x << FIXED_SHIFT) / dc.y;
				k.z = (dc.z << FIXED_SHIFT) / dc.y;
			}else{
				k.x = dc.x << FIXED_SHIFT;
				k.z = dc.z << FIXED_SHIFT;
			};
			if(dc.z < 0) dc.z = - dc.z;
			if(dc.x < 0) dc.x = - dc.x;

			while(dc.y > 0){
				c1.x &= PTrack_mask_x;
				c1.y &= clip_mask_y;
//				if((c1.z >> 16) <= getAlt(c1.x >> 16,c1.y)){
				if(GetCollisionMap(c1.x >> FIXED_SHIFT,c1.y,c1.z >> FIXED_SHIFT)){
					c1.x >>= FIXED_SHIFT;
					c1.z >>= FIXED_SHIFT;
					return 1;
				};
				c1.x += k.x;
				c1.y += k.y;
				c1.z += k.z;
				dc.y--;
			};
		};
	};
	c1.x >>= FIXED_SHIFT;
	c1.y >>= FIXED_SHIFT;
	c1.z >>= FIXED_SHIFT;
	return 0;
};

char GetCollisionMap(int x,int y,int z)
{
	uchar* p = vMap->lineT[y];
	uchar* t;
	uchar d;
	if(p){
		p += x;
		t = p + H_SIZE;
		if((*t) & DOUBLE_LEVEL){
			if(x & 1){
				if((*p) < z) return 0;
				d = *(p -1);
				if(d < z && (d + (((GET_DELTA(*(t - 1)) << 2) + GET_DELTA(*t) + 1) << DELTA_SHIFT)) > z) return 0;
			}else{
				if(*(p + 1) < z) return 0;
				d = *p;
				if(d < z && (d + (((GET_DELTA(*t) << 2) + GET_DELTA(*(t + 1)) + 1) << DELTA_SHIFT)) > z) return 0;
			};
		}else if((*p) < z) return 0;
	};
	return 1;
};

char GetMapLevel(Vector& v)
{
	uchar* p = vMap->lineT[v.y];
	uchar* t;
	uchar d;
	if(p){
		p += v.x;
		t = p + H_SIZE;
		if((*t) & DOUBLE_LEVEL){
			if(v.x & 1){
				if((*p) <= v.z) return 1;
				else{
					d = *(p -1);
					d += ((GET_DELTA(*(t - 1)) << 2) + GET_DELTA(*t) + 1) << DELTA_SHIFT;
					if((d + (((*p) - d) >> 1)) <= v.z) return 1;
				};
			}else{
				if(*(p + 1) <= v.z) return 1;
				else{
					d = *p;
					d += ((GET_DELTA(*t) << 2) + GET_DELTA(*(t + 1)) + 1) << DELTA_SHIFT;
					if((d + (((*(p + 1)) - d) >> 1)) <= v.z) return 1;
				};
			};
		}else return 1;
	};
	return 0;
};


char GetMapLevelType(Vector& v,uchar*& type)
{
	uchar* p = vMap->lineT[v.y];
	uchar* t;
	uchar d;
	if(p){
		p += v.x;
		t = p + H_SIZE;
		type = t;
		if((*t) & DOUBLE_LEVEL){
			if(v.x & 1){
				if((*p) < v.z) return 1;
				else{
					d = *(p -1);
					d += ((GET_DELTA(*(t - 1)) << 2) + GET_DELTA(*t) + 1) << DELTA_SHIFT;
					if((d + (((*p) - d) >> 1)) < v.z) return 1;
				};
			}else{
				if(*(p + 1) <= v.z) return 1;
				else{
					d = *p;
					d += ((GET_DELTA(*t) << 2) + GET_DELTA(*(t + 1)) + 1) << DELTA_SHIFT;
					if((d + (((*(p + 1)) - d) >> 1)) < v.z) return 1;
				};
			};
		}else return 1;
	};
	return -1;
};

uchar GetAlt(int x,int y,int z,uchar& alt)
{
	uchar* p = vMap->lineT[y];
	uchar* t;
	uchar d;
	if(p){
		p += x;
		t = p + H_SIZE;
		if((*t) & DOUBLE_LEVEL){
			if(x & 1){
				if((*p) < z){
					alt = *p;
					return 1;
				};

				d = *(p -1);
				if(d < z){
					if((d + (((GET_DELTA(*(t - 1)) << 2) + GET_DELTA(*t) + 1) << DELTA_SHIFT)) > z){
						alt = d;
						return 0;
					}else{
						alt = *p;
						return 1;
					};
				}else{
					alt = d;
					return 0;
				};
			}else{
				if(*(p + 1) < z){
					alt = *(p + 1);
					return 1;
				};
				d = *p;
				if(d < z){
					if((d + (((GET_DELTA(*t) << 2) + GET_DELTA(*(t + 1)) + 1) << DELTA_SHIFT)) > z){
						alt = d;
						return 0;
					}else{
						alt = *(p + 1);
						return 1;
					};
				}else{
					alt = d;
					return 0;
				};
			};
		}else alt = *p;
	}else alt = 0;
	return 1;
};


int BigGetAlt(int x,int y,int z,uchar& alt,uchar terrain)
{
	uchar* p = vMap->lineT[y];
	if(p){
		p += x;
		alt = *p;
		if(GET_TERRAIN(*(p + H_SIZE)) == terrain)
			return 1;
		else 
			return 0;
	}else alt = 0;
	return 0;
};

int TouchSphere(Vector& r0,Vector& r1,Vector& c,int rad,int& r)
{
	Vector m,m2,p;
	int t,d;
	m.x = getDistX(r1.x,r0.x);
	m.y = getDistY(r1.y,r0.y);
	m.z = r1.z - r0.z;
	m2.x = m.x * m.x;
	m2.y = m.y * m.y;
	m2.z = m.z * m.z;
	d = m2.x + m2.y + m2.z;
	r = d;
	if(d == 0) return 0;
	t = ((m.x*(c.x - r0.x) + m.y*(c.y - r0.y) + m.z*(c.z - r0.z)) << FIXED_SHIFT) / d;
	if(t < 0 || t > (1 << FIXED_SHIFT)) return 0;

	p.x = (r0.x + ((t * m.x) >> FIXED_SHIFT)) & clip_mask_x;
	p.y = (r0.y + ((t * m.y) >> FIXED_SHIFT)) & clip_mask_y;
	p.z = r0.z + ((t * m.z) >> FIXED_SHIFT);

	m.x = getDistX(p.x,c.x);
	m.y = getDistY(p.y,c.y);
	m.z = p.z - c.z;
	t = m.x*m.x + m.y*m.y + m.z*m.z;
	if(t <= rad*rad) return 1;
	return 0;
};

int TouchSphereZ(Vector& r0,Vector& r1,Vector& c,int rad,int& r)
{
	Vector m,m2,p;
	int t,d;
	m.x = getDistX(r1.x,r0.x);
	m.y = getDistY(r1.y,r0.y);
	m.z = r1.z - r0.z;
	m2.x = m.x * m.x;
	m2.y = m.y * m.y;
	m2.z = m.z * m.z;
	d = m2.x + m2.y + m2.z;
	if(d == 0) return 0;
	t = ((m.x*(c.x - r0.x) + m.y*(c.y - r0.y) + m.z*(c.z - r0.z)) << FIXED_SHIFT) / d;
	if(t < 0 || t > (1 << FIXED_SHIFT)) return 0;

	p.x = (r0.x + ((t * m.x) >> FIXED_SHIFT)) & clip_mask_x;
	p.y = (r0.y + ((t * m.y) >> FIXED_SHIFT)) & clip_mask_y;
	p.z = r0.z + ((t * m.z) >> FIXED_SHIFT);

	m.x = getDistX(p.x,c.x);
	m.y = getDistY(p.y,c.y);
	m.z = p.z - c.z;
	t = m.x*m.x + m.y*m.y + (m.z*m.z >> TOUCH_SPHERE_SCALE);
	if(t <= rad*rad){
		m = Vector(getDistX(p.x,r0.x),getDistY(p.y,r0.y),p.z - r0.z);
		r = m.abs2();
		return 1;
	};
	return 0;
};
 
void Object::DrawMechosParticle(int x,int y,int speed,int level,int n)
{
};

void VangerUnit::DrawMechosParticle(int x,int y,int speed,int level,int n)
{
	if(ExternalMode != EXTERNAL_MODE_NORMAL || !ExternalDraw)
		return;
	if(Armor < MaxArmor && (int)(RND(MaxArmor)) > Armor && (int)(RND(MaxArmor)) > Armor)
		MapD.CreateDust(Vector(x,y,level), (int)MAP_SMOKE_PROCESS);
	TrackUnit::DrawMechosParticle(x,y,speed,level,n);
};

void TrackUnit::DrawMechosParticle(int x,int y,int speed,int level,int n)
{
	uchar* TypeMap;
	int rLog = 1;
	uchar trn;
//	int pf;

	double CosTetta = Cos(tetta)*(1 << FIXED_SHIFT)*8/3;
	int track_nx = -round(Cos(psi)*CosTetta);
	int track_ny = round(Sin(psi)*CosTetta);

	if(CurrentWorld == WORLD_KHOX) return;	

	if(speed != 0){
//		if(n == nLeftWheel) pf = 0;
//		else if(n == nRightWheel) pf = 1;
//			else return;
		//rLog = (256 - abs(speed)) >> 4;
		//if(rLog <= 2) rLog = 2;

		cycleTor(x,y);
		TypeMap = vMap->lineT[y];
		if(!TypeMap) return;
		TypeMap += H_SIZE + x;

		if((*TypeMap) & DOUBLE_LEVEL){			
			if(level){
				trn = GET_REAL_TERRAIN(TypeMap,x);
				if(trn == 1){
					if(PrevWheelY[n] != 0)
						MapD.CreateTrace(x,y,PrevWheelX[n],PrevWheelY[n],track_nx,track_ny,1);
					PrevWheelX[n] = x;
					PrevWheelY[n] = y;
					PrevWheelFlag[n] = 1;
				};
			}else{
				trn = GET_REAL_DOWNTERRAIN(TypeMap,x);
				if(trn == 1){
					if(PrevWheelY[n] != 0)
						MapD.CreateTrace(x,y,PrevWheelX[n],PrevWheelY[n],track_nx,track_ny,0);
					PrevWheelX[n] = x;
					PrevWheelY[n] = y;
					PrevWheelFlag[n] = 1;
				};
			};
		}else{
			trn = GET_TERRAIN(*TypeMap);
			if(trn == 1){
				if(PrevWheelY[n] != 0)
					MapD.CreateTrace(x,y,PrevWheelX[n],PrevWheelY[n],track_nx,track_ny,1);
				PrevWheelX[n] = x;
				PrevWheelY[n] = y;
				PrevWheelFlag[n] = 1;
			};
		};

		if((int)(RND(10)) > speed) return;

		if(trn == 1) 
			MapD.CreateDust(Vector(x,y,level), MAP_DUST_PROCESS);
		if(Status & SOBJ_ACTIVE) NullTime = 0;
	}else{
		if(Status & SOBJ_ACTIVE){
			cycleTor(x,y);
			TypeMap = vMap->lineT[y];
			if(!TypeMap) return;
			TypeMap += H_SIZE + x;
			if(level && ((*TypeMap) & DOUBLE_LEVEL) && GET_REAL_TERRAIN(TypeMap,x) == 1) NullTime++;
			else NullTime = 0;
		};
	};
};

void DrawMechosBody(int x,int y,int speed,int level)
{
};

void DrawMechosWheelUp(int cx1,int cy1,int cx2,int cy2,int lh,int h,int delta,int nx,int ny,char step)
{
	int fx,fy,i;

	int dx,dy,kx,ky,tx,ty;
	int ax,ay,bx,by;
	char mask;

	cycleTor(cx1,cy1);
	cycleTor(cx2,cy2);
	dx = getDistX(cx2,cx1);
	dy = getDistY(cy2,cy1);

	tx = cx1;
	ty = cy1;

	mask = step;

	if(abs(dx) > abs(dy)){
		if(dx < 0){
			dx = -dx;
			kx = -1;
		}else kx = 1;

		ty <<= FIXED_SHIFT;
		if(dy < 0){
			dy = -dy;
			if(dx) ky = -(dy << FIXED_SHIFT) / dx;
			else ky = -(dy << FIXED_SHIFT);
		}else{
			if(dx) ky = (dy << FIXED_SHIFT) / dx;
			else ky = dy << FIXED_SHIFT;
		};

		while(dx > 0){
			fx = tx << FIXED_SHIFT;
			fy = ty;
			if(!mask){
				mask = step;
				for(i = 0;i < h;i++){
					pixSetR(XCYCL(fx >> FIXED_SHIFT),YCYCL(fy >> FIXED_SHIFT),-delta);
					fx += nx;
					fy += ny;
				};
			}else{
				for(i = 0;i < h;i++){
					pixSetR(XCYCL(fx >> FIXED_SHIFT),YCYCL(fy >> FIXED_SHIFT),delta);
					fx += nx;
					fy += ny;
				};
			};
			mask--;
			tx += kx;
			ty += ky;
			dx--;
		};
	}else{
		if(dy < 0){
			dy = -dy;
			ky = -1;
		}else ky = 1;

		tx <<= FIXED_SHIFT;
		if(dx < 0){
			dx = -dx;
			if(dy) kx = -(dx << FIXED_SHIFT) / dy;
			else kx = -(dx << FIXED_SHIFT);
		}else{
		       if(dy) kx = (dx << FIXED_SHIFT) / dy;
		       else kx = dx << FIXED_SHIFT;
		};

		while(dy > 0){
			fx = tx;
			fy = ty << FIXED_SHIFT;
			if(!mask){
				mask = step;
				for(i = 0;i < h;i++){
					pixSetR(XCYCL(fx >> FIXED_SHIFT),YCYCL(fy >> FIXED_SHIFT),-delta);
					fx += nx;
					fy += ny;
				};
			}else{
				for(i = 0;i < h;i++){
					pixSetR(XCYCL(fx >> FIXED_SHIFT),YCYCL(fy >> FIXED_SHIFT),delta);
					fx += nx;
					fy += ny;
				};
			};

			mask--;
			tx += kx;
			ty += ky;
			dy--;
		};
	};

	if(cy1 < cy2){
		ay = YCYCL(cy1 - lh);
		by = YCYCL(cy2 + lh);
	}else{
		ay = YCYCL(cy2 - lh);
		by = YCYCL(cy1 + lh);
	};
/*
	if(cx1 < cx2){
		ax = XCYCL(cx1 - lh);
		bx = XCYCL(cx2 + lh);
	}else{
		ax = XCYCL(cx2 - lh);
		bx = XCYCL(cx1 + lh);
	};

	regRenderSimple(ax,ay,bx,by);
*/
//	regRenderSimple(XCYCL(cx1 - lh),YCYCL(cx1 - lh),XCYCL(cx2 + lh),YCYCL(cy2 + lh));

/*	if(cy1 < cy2){
		ay = cy1 - lh;
		by = cy2 + lh;
	}else{
		ay = cy2 - lh;
		by = cy1 + lh;
	};

	if(cx1 < cx2){
		ax = cx2 - cx1;
		bx = TOR_XSIZE - ax;
		if(ax > bx) regRenderSimple(cx2 - lh,ay,cx1 + lh,by);
		else regRenderSimple(cx1 - lh,ay,cx2 + lh,by);
	}else{
		ax = cx1 - cx2;
		bx = TOR_XSIZE - ax;
		if(ax > bx) regRenderSimple(cx1 - lh,ay,cx2 + lh,by);
		else regRenderSimple(cx2 - lh,ay,cx1 + lh,by);
	};*/
};


void DrawMechosWheelDown(int cx1,int cy1,int cx2,int cy2,int lh,int h,int delta,int nx,int ny,char step)
{
	int fx,fy,i;

	int dx,dy,kx,ky,tx,ty;
	int ax,ay,bx,by;
	char mask;

	cycleTor(cx1,cy1);
	cycleTor(cx2,cy2);
	dx = getDistX(cx2,cx1);
	dy = getDistY(cy2,cy1);

	tx = cx1;
	ty = cy1;

	mask = step;

	if(abs(dx) > abs(dy)){
		if(dx < 0){
			dx = -dx;
			kx = -1;
		}else kx = 1;

		ty <<= FIXED_SHIFT;
		if(dy < 0){
			dy = -dy;
			if(dx) ky = -(dy << FIXED_SHIFT) / dx;
			else ky = -(dy << FIXED_SHIFT);
		}else{
			if(dx) ky = (dy << FIXED_SHIFT) / dx;
			else ky = dy << FIXED_SHIFT;
		};

		while(dx > 0){
			fx = tx << FIXED_SHIFT;
			fy = ty;
			if(!mask){
				mask = step;
				for(i = 0;i < h;i++){
					pixDownSet(XCYCL(fx >> FIXED_SHIFT),YCYCL(fy >> FIXED_SHIFT),-delta);
					fx += nx;
					fy += ny;
				};
			}else{
				for(i = 0;i < h;i++){
					pixDownSet(XCYCL(fx >> FIXED_SHIFT),YCYCL(fy >> FIXED_SHIFT),delta);
					fx += nx;
					fy += ny;
				};
			};
			mask--;
			tx += kx;
			ty += ky;
			dx--;
		};
	}else{
		if(dy < 0){
			dy = -dy;
			ky = -1;
		}else ky = 1;

		tx <<= FIXED_SHIFT;
		if(dx < 0){
			dx = -dx;
			if(dy) kx = -(dx << FIXED_SHIFT) / dy;
			else kx = -(dx << FIXED_SHIFT);
		}else{
		       if(dy) kx = (dx << FIXED_SHIFT) / dy;
		       else kx = dx << FIXED_SHIFT;
		};

		while(dy > 0){
			fx = tx;
			fy = ty << FIXED_SHIFT;
			if(!mask){
				mask = step;
				for(i = 0;i < h;i++){
					pixDownSet(XCYCL(fx >> FIXED_SHIFT),YCYCL(fy >> FIXED_SHIFT),-delta);
					fx += nx;
					fy += ny;
				};
			}else{
				for(i = 0;i < h;i++){
					pixDownSet(XCYCL(fx >> FIXED_SHIFT),YCYCL(fy >> FIXED_SHIFT),delta);
					fx += nx;
					fy += ny;
				};
			};
			mask--;
			tx += kx;
			ty += ky;
			dy--;
		};
	};

	if(cy1 < cy2){
		ay = YCYCL(cy1 - lh);
		by = YCYCL(cy2 + lh);
	}else{
		ay = YCYCL(cy2 - lh);
		by = YCYCL(cy1 + lh);
	};

	if(cx1 < cx2){
		ax = XCYCL(cx1 - lh);
		bx = XCYCL(cx2 + lh);
	}else{
		ax = XCYCL(cx2 - lh);
		bx = XCYCL(cx1 + lh);
	};

	regRenderSimple(ax,ay,bx,by);


/*	if(cy1 < cy2){
		ay = cy1 - lh;
		by = cy2 + lh;
	}else{
		ay = cy2 - lh;
		by = cy1 + lh;
	};

	if(cx1 < cx2){
		ax = cx2 - cx1;
		bx = TOR_XSIZE - ax;
		if(ax > bx) regRenderSimple(cx2 - lh,ay,cx1 + lh,by);
		else regRenderSimple(cx1 - lh,ay,cx2 + lh,by);
	}else{
		ax = cx1 - cx2;
		bx = TOR_XSIZE - ax;
		if(ax > bx) regRenderSimple(cx1 - lh,ay,cx2 + lh,by);
		else regRenderSimple(cx2 - lh,ay,cx1 + lh,by);
	};*/
//	regRenderSimple(XCYCL(cx1 - lh),YCYCL(cx1 - lh),XCYCL(cx2 + lh),YCYCL(cy2 + lh));
};

inline int compare(BaseObject* p,BaseObject* n)
{
	// Returns 1 if (p) farther than (n)
//	  if(abs(p->z - n->z) > (p -> max_z + n -> max_z)/2)
//	  if(p -> max_z || n -> max_z)
//		  return (p ->R_curr.z) < (n -> R_curr.z);
	return p->ez > n->ez;
};

void GameObjectDispatcher::Sort(void)
{
	BaseObject* p;
	BaseObject* n;
	BaseObject* pp;
	int dx,dy,dz;

	int view_y,view_z;
	if(DepthShow){
		view_y = ViewY - round(Sin(SlopeAngle)*ViewZ);
		view_z = round(Cos(SlopeAngle)*ViewZ );
		}
	else{
		view_y = ViewY;
		view_z = ViewZ;
		}

	p = Tail;
	while(p){
		if(p->Visibility == VISIBLE){
			dx = getDistX(ViewX, p->R_curr.x);
			dy = getDistY(view_y,p->R_curr.y);
			dz = view_z - p->R_curr.z;
			p->ez = dx*dx + dy*dy + dz*abs(dz);

			if(!ViewTail){
				p->NextViewList = p->PrevViewList = NULL;
				ViewTail = p;
			}else{
				pp = n = ViewTail;
				while(n){
					pp = n;
					if(p->ez > n->ez) break;
//					if(compare(p,n)) break;
					n = n->NextViewList;
				};
				if(n) ConnectNextView(p,pp);
				else  ConnectPrevView(p,pp);
			};
		};
		p = p ->NextBaseList;
	};

/*	  int aaa = 0;
	int bbb = 0;
	p = Tail;
	while(p){
		if(p->Visibility == VISIBLE) bbb++;
		p = p->NextBaseList;
	};

	p = ViewTail;
	while(p){
		if(aaa > 1000) ErrH.Abort("Bad chain 1");
		if(p->PrevViewList && p->PrevViewList->NextViewList != p) ErrH.Abort("Bad chain 2");
		if(p->NextViewList){
			if(p->NextViewList->PrevViewList != p) ErrH.Abort("Bad chain 3");
			if(p->ez < p->NextViewList->ez) ErrH.Abort("Bad chain 4");
		};
		p = p->NextViewList;
		aaa++;
	};
	if(bbb != aaa) ErrH.Abort("Bad chain 5");*/
};

void GameObjectDispatcher::ConnectViewList(BaseObject* p)
{
	if(!ViewTail){
		p->NextViewList = p->PrevViewList = NULL;
		ViewTail = p;
	}else{
		ViewTail -> PrevViewList = p;
		p -> NextViewList = ViewTail;
		p -> PrevViewList = NULL;
		ViewTail = p;
	};
};

void GameObjectDispatcher::ConnectNextView(BaseObject* p,BaseObject* link)
{
//       if(!link) ErrH.Abort("AAAAAAA");
	p->NextViewList = link;
	p->PrevViewList = link->PrevViewList;
	if(link->PrevViewList) link->PrevViewList->NextViewList = p;
	else ViewTail = p;
	link->PrevViewList = p;
};

void GameObjectDispatcher::ConnectPrevView(BaseObject* p,BaseObject* link)
{
//       if(!link) ErrH.Abort("AAAAAAA");
	p->PrevViewList = link;
	p->NextViewList = link->NextViewList;
	if(link->NextViewList) link->NextViewList->PrevViewList = p;
	link->NextViewList = p;
};

void UnitList::Quant(void){ };


void GeneralTableInit(void)
{
	int i;
	if(GeneralSystemSkip){
		palbufC = new unsigned char[768];
		palbufA = new unsigned char[768];
		palbufOrg = new unsigned char[768];

		palbufSrc = new unsigned char[768];

		ShadowColorTable = new uchar[256];
		WaterColorTable = new uchar[256];
		FirePaletteTable = new unsigned char[256 << 8];
		ParticlePaletteTableDust = new unsigned char[256 << 8];
		ParticlePaletteTableSmog = new unsigned char[256 << 8];
		FireColorTable = new uchar[256 << 8];

		palbufBlack = new uchar[768];
		for(i = 0;i < 768;i++) palbufBlack[i] = 0;


		for(i = 0;i < TERRAIN_MAX;i++){
			if(i == 1 || i == 7) 
				TerrainAlphaTable[i] = new uchar[256 << 8];
			else 
				TerrainAlphaTable[i] = NULL;
		};
	};
};

void GeneralTableFree(void)
{
	int i;
	
	if(GeneralSystemSkip){
		for(i = 0;i < TERRAIN_MAX;i++){
			if(i == 1 || i == 7) 
				delete[] TerrainAlphaTable[i];
		};

		delete[] palbufBlack;

		delete[] FireColorTable;
		delete[] ParticlePaletteTableDust;
		delete[] ParticlePaletteTableSmog;
		delete[] FirePaletteTable;
		delete[] WaterColorTable;
		delete[] ShadowColorTable;
		delete[] palbufOrg;
		delete[] palbufA;
		delete[] palbufC;
		delete[] palbufSrc;
	};
};

const int NUM_COLOR_PLACE = 8 + 8;

int FirstColorPlace[WORLD_MAX][NUM_COLOR_PLACE] = {
	{83,83,83,83,83,83,83,83,
		128,144,160,176,184,192,224,240},

	{83,83,83,83,83,83,83,83,
		128,144,160,176,184,192,224,240},

	{83,83,83,83,83,83,83,83,
		128,144,160,176,184,192,224,240},

	{83,83,83,83,83,83,83,83,
		128,144,160,176,184,192,224,240},

	{83,83,83,83,83,83,83,83,
		128,144,160,176,184,192,224,240},

	{83,83,83,83,83,83,83,83,
		128,144,160,176,184,192,224,240},

	{83,83,83,83,83,83,83,83,
		128,144,160,176,184,192,224,240},

	{83,83,83,83,83,83,83,83,
		128,144,160,176,184,192,224,240},

	{83,83,83,83,83,83,83,83,
		128,144,160,176,184,192,224,240},

	{83,83,83,83,83,83,83,83,
		128,144,160,176,184,192,224,240}
};

int LastColorPlace[WORLD_MAX][NUM_COLOR_PLACE] = {
	{83,83,83,83,83,83,83,83,
		143,159,175,183,191,223,239,254},

	{83,83,83,83,83,83,83,83,
		143,159,175,183,191,223,239,254},

	{83,83,83,83,83,83,83,83,
		143,159,175,183,191,223,239,254},

	{83,83,83,83,83,83,83,83,
		143,159,175,183,191,223,239,254},

	{83,83,83,83,83,83,83,83,
		143,159,175,183,191,223,239,254},

	{83,83,83,83,83,83,83,83,
		143,159,175,183,191,223,239,254},

	{83,83,83,83,83,83,83,83,
		143,159,175,183,191,223,239,254},

	{83,83,83,83,83,83,83,83,
		143,159,175,183,191,223,239,254},

	{83,83,83,83,83,83,83,83,
		143,159,175,183,191,223,239,254},

	{83,83,83,83,83,83,83,83,
		143,159,175,183,191,223,239,254}
};

const int DecColorPlace[WORLD_MAX][NUM_COLOR_PLACE] = {
	{4,15,5,5,7,3,7,7,
		17,15,20,4,32,32,32,32},

	{4,15,5,5,7,3,7,7,
		17,15,20,4,32,32,32,32},

	{4,15,5,5,7,3,7,7,
		17,15,20,4,32,32,32,32},

	{4,15,5,5,7,3,7,7,
		17,15,20,4,32,32,32,32},

	{4,15,5,5,7,3,7,7,
		17,15,20,4,32,32,32,32},

	{4,15,5,5,7,3,7,7,
		17,15,20,4,32,32,32,32},

	{4,15,5,5,7,3,7,7,
		17,15,20,4,32,32,32,32},

	{4,15,5,5,7,3,7,7,
		17,15,20,4,32,32,32,32},

	{4,15,5,5,7,3,7,7,
		17,15,20,4,32,32,32,32},

	{4,15,5,5,7,3,7,7,
		17,15,20,4,32,32,32,32}
};

const int GrayColorCycle[WORLD_MAX][NUM_COLOR_PLACE] = {
	{63,63,63,0,0,0,0,63,
		0,0,0,0,0,0,0,0},

	{63,63,63,0,0,0,0,63,
		0,0,0,0,0,0,0,0},

	{63,63,63,0,0,0,0,63,
		0,0,0,0,0,0,0,0},

	{63,63,63,0,0,0,0,63,
		0,0,0,0,0,0,0,0},

	{63,63,63,0,0,0,0,63,
		0,0,0,0,0,0,0,0},

	{63,63,63,0,0,0,0,63,
		0,0,0,0,0,0,0,0},

	{63,63,63,0,0,0,0,63,
		0,0,0,0,0,0,0,0},

	{63,63,63,0,0,0,0,63,
		0,0,0,0,0,0,0,0},

	{63,63,63,0,0,0,0,63,
		0,0,0,0,0,0,0,0},

	{63,63,63,0,0,0,0,63,
		0,0,0,0,0,0,0,0}
};


void CloseBunchPal(void)
{
	int k,j,i;
	Vector vColor,vCheck;
	int MaxVector,dc;

	if(uvsCurrentWorldUnable && CurrentWorld < MAIN_WORLD_MAX - 1){
		vColor = Vector(63,63,63);
		MaxVector = vColor.vabs();		

		for(k = 0;k < WorldPalNum;k++){
			for(i = 0;i < NUM_COLOR_PLACE;i++){
				for(j = FirstColorPlace[CurrentWorld][i];j <= LastColorPlace[CurrentWorld][i];j++){
					vCheck = Vector(WorldPalData[k][j*3],WorldPalData[k][j*3 + 1],WorldPalData[k][j*3 + 2]);
					dc = 63 * vCheck.vabs() / MaxVector;
					WorldPalData[k][j*3] = WorldPalData[k][j*3] + GrayColorCycle[CurrentWorld][i]*(dc - WorldPalData[k][j*3]) / 63;
					WorldPalData[k][j*3 + 1] = WorldPalData[k][j*3 + 1] + GrayColorCycle[CurrentWorld][i]*(dc - WorldPalData[k][j*3 + 1]) / 63;
					WorldPalData[k][j*3 + 2] = WorldPalData[k][j*3 + 2] + GrayColorCycle[CurrentWorld][i]*(dc - WorldPalData[k][j*3 + 2]) / 63;
				};
			};			
		};
	};
};

void GeneralTableOpen(void)
{
	Vector vColor,vCheck;

	int i,j,k;
	int d1,d2,dc;
	int nmin,cmin;
	int ti,dec;
	int n;
	int MaxVector;

	int cmax,cval,cdelta,lmin;

	if(GeneralSystemSkip){

		for(i = 0;i < TERRAIN_MAX;i++){
			LastColorPlace[CurrentWorld][i] = ENDCOLOR[i];
			FirstColorPlace[CurrentWorld][i] = BEGCOLOR[i]; 
		};

		if(uvsCurrentWorldUnable && CurrentWorld < MAIN_WORLD_MAX - 1){
			vColor = Vector(63,63,63);
			MaxVector = vColor.vabs();		

			for(k = 0;k < WorldPalNum;k++){
				for(i = 0;i < NUM_COLOR_PLACE;i++){
					for(j = FirstColorPlace[CurrentWorld][i];j <= LastColorPlace[CurrentWorld][i];j++){
						vCheck = Vector(WorldPalData[k][j*3],WorldPalData[k][j*3 + 1],WorldPalData[k][j*3 + 2]);
						dc = 63 * vCheck.vabs() / MaxVector;
						WorldPalData[k][j*3] = WorldPalData[k][j*3] + GrayColorCycle[CurrentWorld][i]*(dc - WorldPalData[k][j*3]) / 63;
						WorldPalData[k][j*3 + 1] = WorldPalData[k][j*3 + 1] + GrayColorCycle[CurrentWorld][i]*(dc - WorldPalData[k][j*3 + 1]) / 63;
						WorldPalData[k][j*3 + 2] = WorldPalData[k][j*3 + 2] + GrayColorCycle[CurrentWorld][i]*(dc - WorldPalData[k][j*3 + 2]) / 63;
					};
				};			
			};
		};

		PalettePrepare();

		for(i = 0;i < 256;i++){
			if(i > ENDCOLOR[0]){
				vColor = Vector(palbufOrg[i * 3],palbufOrg[i * 3 + 1],palbufOrg[i * 3 + 2]);
				WaterColorTable[i] = vColor.vabs() >> 3;
			}else WaterColorTable[i] = i;
		};

		if(CurrentWorld < MAIN_WORLD_MAX - 1) {
			for(k = 0;k < WorldPalNum;k++) FirePaletteInit(WorldPalData[k]);
		};

		FirePaletteInit(palbufOrg);
		FirePaletteInit(palbuf);

		ParticlePaletteTableInit(palbufOrg);
		FirePaletteTableInit(palbufOrg);
		
		vColor = Vector(63,63,63);
		MaxVector = vColor.vabs();

		for(k = 0;k < 256;k++){
			for(i = 0;i < 256;i++){
				vColor = Vector(palbuf[i*3],palbuf[i*3 + 1],palbuf[i*3 + 2]);

				d1 = vColor.vabs() + (MaxVector - vColor.vabs()) * k / 256;

				ti = FIRE_COLOR_FIRST;
				vColor = Vector(palbuf[FIRE_COLOR_FIRST*3],palbuf[FIRE_COLOR_FIRST*3 + 1],palbuf[FIRE_COLOR_FIRST*3 + 2]);
				dec = abs(vColor.vabs() - d1);

				for(j = FIRE_COLOR_FIRST + 1;j < FIRE_COLOR_FIRST + FIRE_PROCESS_COLOR_MAX;j++){
					vColor = Vector(palbuf[j*3],palbuf[j*3 + 1],palbuf[j*3 + 2]);
					d2 = vColor.vabs();
					dc = abs(d2 - d1);
					if(dc < dec){
						dec = dc;
						ti = j;
					};
				};
				FireColorTable[i + (k << 8)] = ti;
			};
		};

		for(i  = 0;i < 256;i++)
			ShadowColorTable[i] = 0;

		for(k = 0;k < NUM_COLOR_PLACE;k++){
			dec = DecColorPlace[CurrentWorld][k];
			for(i = FirstColorPlace[CurrentWorld][k];i <= LastColorPlace[CurrentWorld][k];i++){
				vColor = Vector(palbufOrg[i * 3],palbufOrg[i * 3 + 1],palbufOrg[i * 3 + 2]);
				dc = vColor.vabs();
				d1 = dc - dec;
				if(d1 < 1) d1 = 1;
				if(dc < 1) dc = 1;

				vColor = vColor * d1 / dc;
				vCheck = Vector(palbufOrg[FirstColorPlace[CurrentWorld][k]],palbufOrg[FirstColorPlace[CurrentWorld][k] + 1],palbufOrg[FirstColorPlace[CurrentWorld][k] + 2]) - vColor;
				nmin = FirstColorPlace[CurrentWorld][k];
				cmin = vCheck.vabs();

				for(j = FirstColorPlace[CurrentWorld][k] + 1;j <= LastColorPlace[CurrentWorld][k];j++){
					vCheck = Vector(palbufOrg[j * 3],palbufOrg[j * 3 + 1],palbufOrg[j * 3 + 2]) - vColor;
					d2 = vCheck.vabs();
					if(d2 <= cmin){
						nmin = j;
						cmin = d2;
					};
				};
				if(nmin > i) ShadowColorTable[i] = i;
				else ShadowColorTable[i] = nmin;
			};
		};
		
		for(n = 0;n < TERRAIN_MAX;n++){
			if(n == 1 || n == 7){
				cmax = 0;
				lmin = 255;
				for(i = BEGCOLOR[n];i < ENDCOLOR[n];i++){
					vColor = Vector(palbufOrg[i*3],palbufOrg[i*3 + 1],palbufOrg[i*3 + 2]);
					cval = vColor.vabs();
					if(cval > cmax) cmax = cval;
					if(cval < lmin) lmin = cval;
				};

				vColor = Vector(63,63,63);
				MaxVector = vColor.vabs();
				cdelta = cmax - lmin;

				for(k = 0;k < 256;k++){
					for(i = 0;i < 256;i++){
						vColor = Vector(palbufOrg[i*3],palbufOrg[i*3 + 1],palbufOrg[i*3 + 2]);
						d1 = vColor.vabs() + (MaxVector - vColor.vabs()) * k / 256;
						d1 = lmin + d1 * cdelta / MaxVector;

						ti = BEGCOLOR[n];
						vColor = Vector(palbufOrg[BEGCOLOR[n]*3],palbufOrg[BEGCOLOR[n]*3 + 1],palbufOrg[BEGCOLOR[n]*3 + 2]);
						dec = abs(vColor.vabs() - d1);

						for(j = BEGCOLOR[n] + 1;j < ENDCOLOR[n];j++){
							vColor = Vector(palbufOrg[j*3],palbufOrg[j*3 + 1],palbufOrg[j*3 + 2]);
							d2 = vColor.vabs();
							dc = abs(d2 - d1);
							if(dc < dec){
								dec = dc;
								ti = j;
							};
						};
						TerrainAlphaTable[n][i + (k << 8)] = ti;
					};
				};
			};
		};

		for(i = 0;i < 768;i++)
			palbufSrc[i] = palbufOrg[i];

		for(i = 0;i < 8;i++)
			register_color(COLORS_IDS::MATERIAL_0 + i,FirstColorPlace[CurrentWorld][i],LastColorPlace[CurrentWorld][i]);
	};
};

int WorldLightParam[WORLD_MAX][3] = {{205,256,160},{205,256,160},{205,256,160},{160,0,0},{160,0,0},{160,0,0},{160,0,0},{160,0,0},{160,0,0},{160,0,0}};
int CurrentWorldLightParam;
int DeltaWorldLightParam;

extern int Redraw;

int NewWorldX;
int NewWorldY;

extern int preViewY;

void ChangeVanger(void) //zNfo
{
	GeneralSystemClose();
	MLReset();
	vMap->release();
	GeneralMapReload = 1;
	GeneralSystemOpen();

	if(WorldTable[CurrentWorld] -> escTmax){
		WorldPalCurrent = uvsCurrentCycle;
		light_modulation  = WorldLightParam[CurrentWorld][WorldPalCurrent];
	};
};

extern int camera_X_prev;
extern int camera_Y_prev;

void ChangeWorld(int world,int flag)
{
	int t,i;
	if(CurrentWorld == WORLD_KHOX)
		scale_general *= 2.;

	GeneralSystemClose();
	CloseCyclicPal();

	PassageBmpPrev = -1;
	t = RND(PassageBmpNum);
	if(PassageImageData[t].xSize != XGR_MAXX){
		for(i = t;i < PassageBmpNum;i++){
			if(PassageImageData[i].xSize == XGR_MAXX){
				PassageBmpPrev = i;
				break;
			};
		};

		if(PassageBmpPrev == -1){
			for(i = 0;i < PassageBmpNum;i++){
				if(PassageImageData[i].xSize == XGR_MAXX){
					PassageBmpPrev = i;
					break;
				};
			};
		};
	}else PassageBmpPrev = t;

	XGR_MouseHide();

	if(PassageBmpPrev >= 0) PassageImageData[PassageBmpPrev].Show();

	CurrentWorld = world;

	camera_X_prev = ViewX = NewWorldX;
	camera_Y_prev = preViewY = ViewY = NewWorldY;

//	for(i = 0;i < MAX_NID_OBJECT;i++) LocalStationCounter[i] = 0;
	LocalStationCounter[GET_COUNTER_INDEX(NID_TNT)] = 0;
	LocalStationCounter[GET_COUNTER_INDEX(NID_SENSOR)] = 0;
	LocalNetEnvironment  =  LocalStationID | (CurrentWorld << 22);

	vMap->release();
	vMap -> reload(CurrentWorld);
	LoadResourceSOUND(GetTargetName("sound"));

	MLreload();
	Redraw = 1;	

	OpenCyclicPal();
	GeneralTableOpen();
//	OpenCyclicPal();
	if(flag)
		GeneralSystemOpen();
	else
		ChangeWorldSkipQuant = 1;

	if(world == WORLD_KHOX) //znfo - items sizing
		scale_general /= 2.; 

	if(PassageBmpPrev >= 0) PassageImageData[PassageBmpPrev].Hide();
	XGR_MouseShow();
	memset(palbuf,0,768);
//	XGR_SetPal(palbufOrg,0,255);

#ifdef ACTINT
	void load_map_bmp(int n);
	load_map_bmp(CurrentWorld);
#endif
};

/*int AG2L(int x,int y,int z,int& sx,int& sy)
{
	int xx = getDistX(x,ViewX);
	int yy = getDistY(y,ViewY);

//	if(DepthShow){
//		  int offset = zmax >> 1;
		int x1 = round_sin(A_g2s.a[0]*xx + A_g2s.a[1]*yy - A_g2s.a[2]*z);
		int y1 = round_sin(A_g2s.a[3]*xx + A_g2s.a[4]*yy - A_g2s.a[5]*z);
		int z1 = round_sin(A_g2s.a[6]*xx + A_g2s.a[7]*yy) + ViewZ - z;
		if(z1 <= 0) z1 = 1;
//		  z1 += offset;
		sx = x1*focus/z1 + ScreenCX;
		sy = y1*focus/z1 + ScreenCY;
		return ((focus << 8)/z1);
//		}
//	else{
//		sx = ((xx*cosTurn - yy*sinTurn)/0x10000) + ScreenCX;
//		sy = ((xx*sinTurn + yy*cosTurn)/0x10000) + ScreenCY;
//		scale = (focus << 8)/(-((int)R.z >> 1) + ViewZ);
//		}
};*/

void G2L(int x,int y,int& xl,int& yl)
{
	x = getDistX(x,ViewX);
	y = getDistY(y,ViewY);

	xl = round((x*cosTurnFlt - y*sinTurnFlt)) + ScreenCX;
	yl = round((x*sinTurnFlt + y*cosTurnFlt)) + ScreenCY;
};

int G2LS(int x,int y,int z,int& sx,int& sy)
{
	int z1;
	sx = round(getDistX(x,ViewX)*ScaleMapInvFlt) + ScreenCX;
	sy = round(getDistY(y,ViewY)*ScaleMapInvFlt) + ScreenCY;
	z1 = ViewZ - (z >> 1);
	if(z1 <= 0) z1 = 1;
//	return((focus << 8)/(-((int)z >> 1) + ViewZ));
	return(focus << 8)/z1;
};

void G2LP(int x,int y,int z,int& sx,int& sy)
{
	sx = getDistX(x,ViewX) + ScreenCX;
	sy = getDistY(y,ViewY) + ScreenCY;
};

void G2LQ(int x,int y,int z,int& sx,int& sy)
{
	z = 0;
	int xx = getDistX(x,ViewX);
	int yy = getDistY(y,ViewY);
	double x1 = A_g2s.a[0]*xx + A_g2s.a[1]*yy - A_g2s.a[2]*z;
	double y1 = A_g2s.a[3]*xx + A_g2s.a[4]*yy - A_g2s.a[5]*z;
	double z1 = ViewZ + (A_g2s.a[6]*xx + A_g2s.a[7]*yy -A_g2s.a[7]*z)*0.5;
	if(z1 <= 0) 
		z1 = 1;
	z1 = focus_flt/z1;
	sx = round(x1*z1) + ScreenCX;
	sy = round(y1*z1) + ScreenCY;
};

int G2LF(int x,int y,int z,int& sx,int& sy)
{
	z = 0;
	int xx = getDistX(x,ViewX);
	int yy = getDistY(y,ViewY);
	double x1 = round(A_g2s.a[0]*xx + A_g2s.a[1]*yy - A_g2s.a[2]*z);
	double y1 = round(A_g2s.a[3]*xx + A_g2s.a[4]*yy - A_g2s.a[5]*z);
	double z1 = ViewZ + round((A_g2s.a[6]*xx + A_g2s.a[7]*yy -A_g2s.a[7]*z)*.5);
	if(z1 <= 0) 
		z1 = 1;
	z1 = focus_flt/z1;
	sx = round(x1*z1) + ScreenCX;
	sy = round(y1*z1) + ScreenCY;
	return round(256.*z1);
};

void S2G(int xs,int ys,int& xg,int& yg)
{
	xs -= ScreenCX;
	ys -= ScreenCY;
	if(DepthShow){
		double al = Ha*xs + Va*ys;
		double bl = Hb*xs + Vb*ys;
		double cl = Oc + Hc*xs + Vc*ys;
		if(cl > 0){
			cl = 1/(cl*(double)(1 << 16));
			xg =  ((int)round(al*cl) + ViewX) & clip_mask_x;
			yg =  ((int)round(bl*cl) + ViewY) & clip_mask_y;
			}
		else{
			xg =  ViewX;
			yg =  ViewY;
			}
		}
	else{
		xg = ((int)round(xs*cosTurnInvFlt + ys*sinTurnInvFlt) + ViewX) & clip_mask_x;
		yg = ((int)round(-xs*sinTurnInvFlt + ys*cosTurnInvFlt) + ViewY) & clip_mask_y;
		}
}

void BaseObject::GetVisible(void)
{
/*	if(DepthShow){
		int xt = getDistX(R_curr.x,ViewX);
		int yy = getDistY(R_curr.y,ViewY);
		int zz = round_sin(A_g2s.a[6]*xt + A_g2s.a[7]*yy) - R_curr.z + ViewZ;
		int xx = round_sin(A_g2s.a[0]*xt + A_g2s.a[1]*yy);
		yy = round_sin(A_g2s.a[3]*xt + A_g2s.a[4]*yy);
		if(zz < -radius || abs(xx) > zz*TurnSideX/ViewZ + radius*2 ||  abs(yy) > zz*TurnSideY/ViewZ + radius*2) Visibility = UNVISIBLE;
		else Visibility = VISIBLE;
	}else{
		if(abs(getDistX(R_curr.x,ViewX)) - (radius << 1) < TurnSideX &&
		   abs(getDistY(R_curr.y,ViewY)) -  (radius << 1) < TurnSideY) Visibility = VISIBLE;
		else Visibility = UNVISIBLE;
	};*/

	int xt = getDistX(R_curr.x,ViewX);
	int yt = getDistY(R_curr.y,ViewY);
	if(DepthShow){
		double xx = A_g2s.a[0]*xt + A_g2s.a[1]*yt;
		double yy = A_g2s.a[3]*xt + A_g2s.a[4]*yt;
		double zz = A_g2s.a[6]*xt + A_g2s.a[7]*yt + ViewZ;
		if(zz <= 0){
			Visibility = UNVISIBLE;
			return;
			}
		zz = focus_flt/zz;
		xt = round(xx*zz);
		yt = round(yy*zz);
		if(abs(xt) - (radius << 1) < curGMap -> xside && abs(yt) - (radius << 1) < curGMap -> yside)
			Visibility = VISIBLE;
		else
			Visibility = UNVISIBLE;
		return;
		}
	else
		if(TurnAngle){
			int xx = round(A_g2s.a[0]*xt + A_g2s.a[1]*yt);
			yt = round(A_g2s.a[3]*xt + A_g2s.a[4]*yt);
			xt = xx;
			}

	if(abs(xt) - (radius << 1) < TurnSideX && abs(yt) - (radius << 1) < TurnSideY)
		Visibility = VISIBLE;
	else
		Visibility = UNVISIBLE;
};

char getObjectPosition(int& x,int& y)
{
	char c;
	
		if(!actCurrentViewObject){
#ifdef _DEBUG
			if(aiCutDominance >= 0){
#else
//#ifdef ZMOD_BETA
//			if (1) {
//#else
			if (!NetworkON && aiCutDominance >= 50) {
				//znfo - dolly visibility
//#endif //ZMOD_BETA
#endif
				if(!actCurrentViewDolly){
#ifdef _DEBUG
					if(!actCurrentViewStuff){
						actCurrentViewStuff = (StuffObject*)(ItemD.Tail);

						actCurrentViewObject = (VangerUnit*)(ActD.Tail);
						if(actCurrentViewObject && (actCurrentViewObject->Status & SOBJ_WAIT_CONFIRMATION)){
							do{
								actCurrentViewObject = (VangerUnit*)(actCurrentViewObject)->NextTypeList;					
							}while(actCurrentViewObject && (actCurrentViewObject->Status & SOBJ_WAIT_CONFIRMATION));
						};

						actCurrentViewDolly = (uvsDolly*)(DollyTail);
						if(actCurrentViewDolly && (actCurrentViewDolly->Pworld->gIndex != CurrentWorld || actCurrentViewDolly->status)){
							do{
								actCurrentViewDolly = (uvsDolly*)(actCurrentViewDolly->next);
							}while(actCurrentViewDolly && (actCurrentViewDolly->Pworld->gIndex != CurrentWorld || actCurrentViewDolly->status));
						};
						return -1;
					};
					x = actCurrentViewStuff->R_curr.x;
					y = actCurrentViewStuff->R_curr.y;
					actCurrentViewStuff = (StuffObject*)(actCurrentViewStuff->NextTypeList);
					return 3;		   
#endif

					actCurrentViewObject = (VangerUnit*)(ActD.Tail);
					if(actCurrentViewObject && (actCurrentViewObject->Status & SOBJ_WAIT_CONFIRMATION)){
						do{
							actCurrentViewObject = (VangerUnit*)(actCurrentViewObject)->NextTypeList;					
						}while(actCurrentViewObject && (actCurrentViewObject->Status & SOBJ_WAIT_CONFIRMATION));
					};

					actCurrentViewDolly = (uvsDolly*)(DollyTail);
					if(actCurrentViewDolly && (actCurrentViewDolly->Pworld->gIndex != CurrentWorld || actCurrentViewDolly->status)){
						do{
							actCurrentViewDolly = (uvsDolly*)(actCurrentViewDolly->next);
						}while(actCurrentViewDolly && (actCurrentViewDolly->Pworld->gIndex != CurrentWorld || actCurrentViewDolly->status));
					};
					return -1;
				};

				x = actCurrentViewDolly->pos_x;
				y = actCurrentViewDolly->pos_y;
				switch(actCurrentViewDolly->biosNindex){
					case 0:
						c = 8;
						break;
					case 1:
						c = 9;
						break;
					case 2:
						c = 10;
						break;
				};

				do{
					actCurrentViewDolly = (uvsDolly*)(actCurrentViewDolly->next);
				}while(actCurrentViewDolly && (actCurrentViewDolly->Pworld->gIndex != CurrentWorld || actCurrentViewDolly->status));

				return c;
			}else{
				actCurrentViewObject = (VangerUnit*)(ActD.Tail);
				if(actCurrentViewObject && (actCurrentViewObject->Status & SOBJ_WAIT_CONFIRMATION)){
					do{
						actCurrentViewObject = (VangerUnit*)(actCurrentViewObject)->NextTypeList;					
					}while(actCurrentViewObject && (actCurrentViewObject->Status & SOBJ_WAIT_CONFIRMATION));
				};
				return -1;
			};
		};

		x = actCurrentViewObject->R_curr.x;
		y = actCurrentViewObject->R_curr.y;
		switch(actCurrentViewObject->ID){
			case ID_VANGER:
				if(actCurrentViewObject->VangerRaceStatus != VANGER_RACE_NONE/* && aciRacingFlag*/){
//					actCurrentViewObject = (VangerUnit*)(actCurrentViewObject->NextTypeList);
					do{
						actCurrentViewObject = (VangerUnit*)(actCurrentViewObject)->NextTypeList;					
					}while(actCurrentViewObject && (actCurrentViewObject->Status & SOBJ_WAIT_CONFIRMATION));
					return 3;
				};				

				switch(actCurrentViewObject->uvsPoint->Pmechos->color){
					case 0:
						c = 4;
						break;
					case 1:
						c = 5;
						break;
					case 2:
						c = 6;
						break;
					default:
						c = 7;
						break;
				};
				
				do{
					actCurrentViewObject = (VangerUnit*)(actCurrentViewObject)->NextTypeList;					
				}while(actCurrentViewObject && (actCurrentViewObject->Status & SOBJ_WAIT_CONFIRMATION));

//				actCurrentViewObject = (VangerUnit*)(actCurrentViewObject)->NextTypeList;
				return c;			
		};	

/*	else{
		if(!actCurrentViewObject){
			actCurrentViewObject = PhantomD.Tail;
			return -1;
		};
		x = actCurrentViewObject->R_curr.x;
		y = actCurrentViewObject->R_curr.y;
		actCurrentViewObject = actCurrentViewObject->NextTypeList;
		return 1;			
	};*/

	return -1;
};

void ScreenLineTrace(Vector& v0,Vector& v1,uchar* ColorTable,uchar flag)
{
	int x0,y0,x1,y1;
	int dx,dy,k,cx,cy;
	int tx,ty;
	Vector vDelta,vC;
	int l,dl;

	v0.x = XCYCL(v0.x);
	v1.x = XCYCL(v1.x);

	if(AdvancedView){
		G2LQ(v0.x,v0.y,v0.z,x0,y0);
		G2LQ(v1.x,v1.y,v1.z,x1,y1);
	}else{
		G2LP(v0.x,v0.y,v0.z,x0,y0);
		G2LP(v1.x,v1.y,v1.z,x1,y1);
	};

	dx = x1 - x0;
	dy = y1 - y0;

	if(dx == 0 && dy == 0) return;
	if(flag){
		l = flag << 8;
		if(abs(dx) > abs(dy)){
			if(dx < 0){
				cx = x1;
				dx = -dx;
				vDelta = Vector(getDistX(v0.x,v1.x),getDistY(v0.y,v1.y),v0.z - v1.z);
				vC = v1;
				dy = -dy;
				cy = y1 << FIXED_SHIFT;
			}else{
				cx = x0;
				vDelta = Vector(getDistX(v1.x,v0.x),getDistY(v1.y,v0.y),v1.z - v0.z);
				vC = v0;
				cy = y0 << FIXED_SHIFT;
			};

			vC <<= FIXED_SHIFT;

			k = (dy << FIXED_SHIFT) / dx;

			vDelta <<= FIXED_SHIFT;
			vDelta /= dx;

			while(dx){
				vC.x &= PTrack_mask_x;
				vC.y &= PTrack_mask_y;

				ty = cy >> FIXED_SHIFT;
				if(cx > UcutLeft && cx < UcutRight && ty > VcutUp && ty < VcutDown){
					if(GetAltLevel(vC.x >> FIXED_SHIFT,vC.y >> FIXED_SHIFT,vC.z >> FIXED_SHIFT))
						XGR_SetPixel(cx,ty,ColorTable[XGR_GetPixel(cx,ty) + l]);
				};
				cx++;
				cy += k;
				vC += vDelta;
				dx--;
			};
		}else{
			if(dy < 0){
				cy = y1;
				dy = -dy;
				vDelta = Vector(getDistX(v0.x,v1.x),getDistY(v0.y,v1.y),v0.z - v1.z);
				vC = v1;
				dx = -dx;
				cx = x1 << FIXED_SHIFT;
			}else{
				cy = y0;
				vDelta = Vector(getDistX(v1.x,v0.x),getDistY(v1.y,v0.y),v1.z - v0.z);
				vC = v0;
				cx = x0 << FIXED_SHIFT;
			};

			vC <<= FIXED_SHIFT;

			k = (dx << FIXED_SHIFT) / dy;

			vDelta <<= FIXED_SHIFT;
			vDelta /= dy;

			while(dy){
				vC.x &= PTrack_mask_x;
				vC.y &= PTrack_mask_y;

				tx = cx >> FIXED_SHIFT;
				if(tx > UcutLeft && tx < UcutRight && cy > VcutUp && cy < VcutDown){
					if(GetAltLevel(vC.x >> FIXED_SHIFT,vC.y >> FIXED_SHIFT,vC.z >> FIXED_SHIFT))
						XGR_SetPixel(tx,cy,ColorTable[XGR_GetPixel(tx,cy) + l]);
				};
				cy++;
				cx += k;
				vC += vDelta;
				dy--;
			};
		};
	}else{
		if(abs(dx) > abs(dy)){
			if(dx < 0){
				cx = x1;
				dx = -dx;
				vDelta = Vector(getDistX(v0.x,v1.x),getDistY(v0.y,v1.y),v0.z - v1.z);
				vC = v1;
				dy = -dy;
				cy = y1 << FIXED_SHIFT;
				l = 0;
				dl =  (150 << 8) / dx;
			}else{
				cx = x0;
				vDelta = Vector(getDistX(v1.x,v0.x),getDistY(v1.y,v0.y),v1.z - v0.z);
				vC = v0;
				cy = y0 << FIXED_SHIFT;
				l = 150 << 8;
				dl = - l / dx;
			};

			vC <<= FIXED_SHIFT;

			k = (dy << FIXED_SHIFT) / dx;

			vDelta <<= FIXED_SHIFT;
			vDelta /= dx;

			while(dx){
				vC.x &= PTrack_mask_x;
				vC.y &= PTrack_mask_y;

				ty = cy >> FIXED_SHIFT;
				if(cx > UcutLeft && cx < UcutRight && ty > VcutUp && ty < VcutDown){
					if(GetAltLevel(vC.x >> FIXED_SHIFT,vC.y >> FIXED_SHIFT,vC.z >> FIXED_SHIFT))
						XGR_SetPixel(cx,ty,ColorTable[XGR_GetPixel(cx,ty) + (l & 0xffffff00)]);
				};

				cx++;
				cy += k;
				vC += vDelta;
				dx--;
				l += dl;
			};
		}else{
			if(dy < 0){
				cy = y1;
				dy = -dy;
				vDelta = Vector(getDistX(v0.x,v1.x),getDistY(v0.y,v1.y),v0.z - v1.z);
				vC = v1;
				dx = -dx;
				cx = x1 << FIXED_SHIFT;

				l = 0;
				dl = (150 << 8) / dy;
			}else{
				cy = y0;
				vDelta = Vector(getDistX(v1.x,v0.x),getDistY(v1.y,v0.y),v1.z - v0.z);
				vC = v0;
				cx = x0 << FIXED_SHIFT;

				l = 150 << 8;
				dl = - l / dy;
			};

			vC <<= FIXED_SHIFT;

			k = (dx << FIXED_SHIFT) / dy;

			vDelta <<= FIXED_SHIFT;
			vDelta /= dy;

			while(dy){
				vC.x &= PTrack_mask_x;
				vC.y &= PTrack_mask_y;

				tx = cx >> FIXED_SHIFT;
				if(tx > UcutLeft && tx < UcutRight && cy > VcutUp && cy < VcutDown){
					if(GetAltLevel(vC.x >> FIXED_SHIFT,vC.y >> FIXED_SHIFT,vC.z >> FIXED_SHIFT))
						XGR_SetPixel(tx,cy,ColorTable[XGR_GetPixel(tx,cy) + (l & 0xffffff00)]);
				};
				cy++;
				cx += k;
				vC += vDelta;
				dy--;
				l += dl;
			};
		};
	};
};

int GlobalFuryLevel;

void GeneralSystemLoad(XStream& in)
{
	int i,ver;
	in > ver;
	in > ViewX;
	in > ViewY;
	in > GlobalFuryLevel;
	preViewY = ViewY;
	GeneralMapReload = 1;
	GeneralLoadReleaseFlag = 1;
	LoadProtoCrypt(in,ver);
	in > ChangeEnergy;
	in > ChangeArmor;

	switch(ver){
		case 0:
			for(i = 0;i < 83;i++)
	 			in >  ver;
			break;
		case 1:
		case 2:
			for(i = 0;i < 83 - WORLD_MAX;i++)
				in > ver;
			break;
		case 3:
			in > ActD.SpobsDestroy;
			in > ActD.ThreallDestroy;	
			in > ActD.SpummyRunner;

			for(i = 0;i < NUM_CHECK_BSIGN;i++)
				in > CHECK_BSIGN_INDEX[i];

			for(i = 0;i < NUM_CHECK_BSIGN;i++)
				in > CHECK_BSIGN_DATA[i];

			for(i = 0;i < 83 - WORLD_MAX - 3*sizeof(int) - 2*NUM_CHECK_BSIGN;i++)
				in > ver;
			break;
		case 4:
			in > ActD.SpobsDestroy;
			in > ActD.ThreallDestroy;	
			in > ActD.SpummyRunner;

			for(i = 0;i < NUM_CHECK_BSIGN;i++)
				in > CHECK_BSIGN_INDEX[i];

			for(i = 0;i < NUM_CHECK_BSIGN;i++)
				in > CHECK_BSIGN_DATA[i];

			in > ActD.SpobsDestroyActive;
			in >ActD.ThreallDestroyActive;
			for(i = 0;i < MAX_TIME_SECRET;i++){
				in > TimeSecretEnable[WORLD_FOSTRAL][i];
				in > TimeSecretEnable[WORLD_GLORX][i];		
			};

			in > i;
			if(i != 3791)
				ErrH.Abort("Bad Save Signature");

			in > i;
			if(i != 11083)
				ErrH.Abort("Bad Save Signature");
			break;
		case 5:
			in > ActD.SpobsDestroy;
			in > ActD.ThreallDestroy;	
			in > ActD.SpummyRunner;

			for(i = 0;i < NUM_CHECK_BSIGN;i++)
				in > CHECK_BSIGN_INDEX[i];

			for(i = 0;i < NUM_CHECK_BSIGN;i++)
				in > CHECK_BSIGN_DATA[i];

			in > ActD.SpobsDestroyActive;
			in > ActD.ThreallDestroyActive;
			for(i = 0;i < MAX_TIME_SECRET;i++){
				in > TimeSecretEnable[WORLD_FOSTRAL][i];
				in > TimeSecretEnable[WORLD_GLORX][i];		
			};

			in > ActD.PromptPodishFreeVisit;
			in > ActD.PromptIncubatorFreeVisit;
			in > ActD.PromptChangeCycleCount;

			in > i;
			if(i != 3791)
				ErrH.Abort("Bad Save Signature");

			in > i;
			if(i != 11083)
				ErrH.Abort("Bad Save Signature");
			break;
		case 6:
			in > ActD.SpobsDestroy;
			in > ActD.ThreallDestroy;	
			in > ActD.SpummyRunner;

			for(i = 0;i < NUM_CHECK_BSIGN;i++)
				in > CHECK_BSIGN_INDEX[i];
			
			for(i = 0;i < NUM_CHECK_BSIGN;i++)
				in > CHECK_BSIGN_DATA[i];

			in > ActD.SpobsDestroyActive;
			in > ActD.ThreallDestroyActive;
			for(i = 0;i < MAX_TIME_SECRET;i++){
				in > TimeSecretEnable[WORLD_FOSTRAL][i];
				in > TimeSecretEnable[WORLD_GLORX][i];

				in > TimeSecretType[WORLD_FOSTRAL][i];
				in > TimeSecretType[WORLD_GLORX][i];

				in > TimeSecretData0[WORLD_FOSTRAL][i];
				in > TimeSecretData0[WORLD_GLORX][i];

				in > TimeSecretData1[WORLD_FOSTRAL][i];
				in > TimeSecretData1[WORLD_GLORX][i];
			};

			in > ActD.PromptPodishFreeVisit;
			in > ActD.PromptIncubatorFreeVisit;
			in > ActD.PromptChangeCycleCount;

			in > i;
			if(i != 3791)
				ErrH.Abort("Bad Save Signature");

			in > i;
			if(i != 11083)
				ErrH.Abort("Bad Save Signature");
			break;
	};
//	if(strlen(CHECK_BSIGN_DATA) < NUM_CONTROL_BSIGN)
//		ErrH.Abort("Bad Load Word in Boozeena");
};

void GeneralSystemSave(XStream& out)
{
	int i;

//	if(strlen(CHECK_BSIGN_DATA) < NUM_CONTROL_BSIGN)
//		ErrH.Abort("Bad Save Word in Boozeena");

	out < (int)(6);
	out < ViewX;
	out < ViewY;
	out < GlobalFuryLevel;
	SaveProtoCrypt(out);
	out < ChangeEnergy;
	out < ChangeArmor;

//Version3--------------------------------
	out < ActD.SpobsDestroy;
	out < ActD.ThreallDestroy;	
	out < ActD.SpummyRunner;

	for(i = 0;i < NUM_CHECK_BSIGN;i++)
		out < CHECK_BSIGN_INDEX[i];	

	for(i = 0;i < NUM_CHECK_BSIGN;i++)
		out < CHECK_BSIGN_DATA[i];

//Version4--------------------------------
	out < ActD.SpobsDestroyActive;
	out < ActD.ThreallDestroyActive;
	for(i = 0;i < MAX_TIME_SECRET;i++){
		out < TimeSecretEnable[WORLD_FOSTRAL][i];
		out < TimeSecretEnable[WORLD_GLORX][i];		
//Version6-------------------------------
		out < TimeSecretType[WORLD_FOSTRAL][i];
		out < TimeSecretType[WORLD_GLORX][i];

		out < TimeSecretData0[WORLD_FOSTRAL][i];
		out < TimeSecretData0[WORLD_GLORX][i];

		out < TimeSecretData1[WORLD_FOSTRAL][i];
		out < TimeSecretData1[WORLD_GLORX][i];
//-------------------------------------------
	};
//Version5-------------------------------
	out < ActD.PromptPodishFreeVisit;
	out < ActD.PromptIncubatorFreeVisit;
	out < ActD.PromptChangeCycleCount;
//-------------------------------------------
	out < (int)(3791);
	out < (int)(11083);

};

void MemoryStorageType::Init(int mElement,int eSize)
{
	StorageClusterType* p;
	NumCluster = 1;
	ElementSize = eSize;
	MaxElement = mElement;	
	p = new StorageClusterType;
	p ->Init(this);
	Tail = p;
	p->Next = NULL;
};

void MemoryStorageType::Free(void)
{
	StorageClusterType* p;
	StorageClusterType* pn;
	p = Tail;
	while(p){
		pn = p->Next;
		p->Free();
		delete p;
		p = pn;
	};
};

GeneralObject* MemoryStorageType::GetPoint(void)
{
	StorageClusterType* g;
	StorageClusterType* n;
	g = Tail;
	n = NULL;
	while(g){
		if(g->CheckSpace())
			return(g->GetPoint());
		n = g;
		g = g->Next;
	};
	if(n->Next != NULL)
		ErrH.Abort("Bad Units Heap");

	g = new StorageClusterType;
	g->Init(this);
	g->Next = NULL;
	if(n) n->Next = g;
	else ErrH.Abort("Bad Tail in Units Heap");
	return(g->GetPoint());
};

void MemoryStorageType::PutPoint(GeneralObject* p)
{
	p->Cluster->PutPoint(p);
	if(p->Cluster->CheckSpace() == 2 && p->Cluster != Tail){
		p->Cluster->Free();
		delete p->Cluster;
	};
};

void StorageClusterType::Init(MemoryStorageType* p)
{
	int i;
	MaxPoint = p->MaxElement;
	PointSize = p->ElementSize;
	PointData = new char*[MaxPoint];
	Data = new char[MaxPoint*PointSize];
	Next = NULL;
	for(i = 0;i < MaxPoint;i++) PointData[i] = Data + i * PointSize;
	CurrPoint = MaxPoint;
};

void StorageClusterType::Free(void)
{
	delete[] PointData;
	delete[] Data;
};

char StorageClusterType::CheckSpace(void)
{
	if(CurrPoint >= MaxPoint) return 2;
	else if(CurrPoint <= 0) return 0;
	return 1;
};

GeneralObject* StorageClusterType::GetPoint(void)
{
	GeneralObject* p;
	if(CurrPoint <= 0) ErrH.Abort("Memory stack empty");
	p = (GeneralObject*)(PointData[--CurrPoint]);
	p->Cluster = this;
	return p;
};

void StorageClusterType::PutPoint(GeneralObject* p)
{
	if(CurrPoint >= MaxPoint) ErrH.Abort("Memory stack overflow");
	if((long long)(p) < (long long)(Data) || (long long)(p) >= (long long)(Data + MaxPoint*PointSize)) ErrH.Abort("Bad Units Memory Area");
	PointData[CurrPoint++] = (char*)(p);
//	p = NULL;
};

void UnitList::Add2Next(GeneralObject* p,GeneralObject* t)
{
	p->PrevTypeList = t;
	p->NextTypeList = t->NextTypeList;
	if(t->NextTypeList) t->NextTypeList->PrevTypeList = p;
	t->NextTypeList = p;
	Num++;
};

void UnitList::Add2Prev(GeneralObject* p,GeneralObject* t)
{
	if(t == Tail) Tail = p;
	p->NextTypeList = t;
	p->PrevTypeList = t->PrevTypeList;
	if(t->PrevTypeList) t->PrevTypeList->NextTypeList = p;
	t->PrevTypeList = p;
	Num++;
};

void UnitList::ConnectTypeList(GeneralObject* p)
{
/*	GeneralObject* n;
	GeneralObject* np;
	int y;

	if(!Tail){
		p -> NextTypeList = p -> PrevTypeList = NULL;
		Tail = p;
	}else{
		y = p->R_curr.y;
		n = Tail;
		while(n){
			np = n;
			if(n->R_curr.z > y)
				break;
			n = n->NextTypeList;
		};			
		Add2Next(p,np);
	};*/

	if(!Tail){
		p -> NextTypeList = p -> PrevTypeList = NULL;
		Tail = p;
	}else{
		Tail -> PrevTypeList = p;
		p -> NextTypeList = Tail;
		p -> PrevTypeList = NULL;
		Tail = p;
	};
	Num++;
};

//-----------------------------------------------------Network List-------------------------------------------

void UnitListType::Init(void)
{
	Num = 0;
	Tail = NULL;
};

void UnitListType::Free(void)
{
	GeneralObject* p;
	GeneralObject* pp;
	p = Tail;
	while(p){
		pp = p->NextTypeList;
		FreeUnit(p);
		p = pp;
	};
};

void UnitListType::FreeUnit(GeneralObject* p)
{
	DisconnectTypeList(p);
	delete p;
};

void UnitListType::Quant(void)
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
		if(p->Status & SOBJ_DISCONNECT)
			FreeUnit(p);
		p = pp;
	};
};

void UnitListType::NetEvent(int type,int id)
{
};

void UnitListType::ConnectTypeList(GeneralObject* p)
{
	if(!Tail){
		p -> NextTypeList = p -> PrevTypeList = NULL;
		Tail = p;
	}else{
		Tail -> PrevTypeList = p;
		p -> NextTypeList = Tail;
		p -> PrevTypeList = NULL;
		Tail = p;
	};
	Num++;
};

void UnitListType::DisconnectTypeList(GeneralObject* p)
{
	if((!p->PrevTypeList) && (!p->NextTypeList)) Tail = NULL;
	else{
		if(p->PrevTypeList) p->PrevTypeList->NextTypeList = p->NextTypeList;
		else Tail = p->NextTypeList;
		if(p->NextTypeList) p->NextTypeList->PrevTypeList = p->PrevTypeList;
	};
	Num--;
};

void UnitBaseListType::ConnectTypeList(GeneralObject* p)
{
	if(!Tail){
		p -> NextTypeList = p -> PrevTypeList = NULL;
		Tail = p;
	}else{
		Tail -> PrevTypeList = p;
		p -> NextTypeList = Tail;
		p -> PrevTypeList = NULL;
		Tail = p;
	};
	Num++;
	GameD.ConnectBaseList((BaseObject*)p);
};

void UnitBaseListType::DisconnectTypeList(GeneralObject* p)
{
	GameD.DisconnectBaseList((BaseObject*)p);

	if((!p->PrevTypeList) && (!p->NextTypeList)) Tail = NULL;
	else{
		if(p->PrevTypeList) p->PrevTypeList->NextTypeList = p->NextTypeList;
		else Tail = p->NextTypeList;
		if(p->NextTypeList) p->NextTypeList->PrevTypeList = p->PrevTypeList;
	};
	Num--;
};

//----------------------------------------------------------------

GeneralObject* UnitListType::GetNetObject(int id)
{
	GeneralObject* p;

	p = Tail;
	while(p){
		if(p->NetID == id)
			return p;
		p = p->NextTypeList;
	};
	return NULL;
};

GeneralObject* UnitList::GetNetObject(int id)
{
	GeneralObject* p;

	p = Tail;
	while(p){
		if(p->NetID == id)
			return p;
		p = p->NextTypeList;
	};
	return NULL;
};

void UnitList::NetEvent(int type,int id)
{
};

#ifdef _DEBUG
extern XStream fout;
#endif

void GameObjectDispatcher::NetEvent(void)
{
	int id,type;

	//restore_connection();
	NETWORK_IN_STREAM.receive();

	while((type = NETWORK_IN_STREAM.current_event())){
		if(!(type & AUXILIARY_EVENT)){
			id = NETWORK_IN_STREAM.current_ID();
			switch(GET_NETWORK_ID(id)){
				case NID_SHELL:
					ActD.ShellNetEvent(type,id);
					break;
				case NID_GLOBAL:
					sBunchReceive(id);
					break;
				case NID_VANGER:
					ActD.NetEvent(type,id);
					break;
				case NID_SENSOR:
					if(type == UPDATE_OBJECT) NetworkSetLocation(id);
					break;
				case NID_STUFF:
 					ItemD.NetEvent(type,id);
					break;
				case NID_DEVICE:
					ItemD.NetDevice(type,id);
					break;
				case NID_TNT:
					if(type == UPDATE_OBJECT) NetworkSetTnt(id);
					break;
				case NID_SLOT:
					NetSlotEvent(type,id);
					break;
				default:
#ifdef _DEBUG
				
					fout.SetRadix(16);
					fout < "Ignore:  Type:" <= type;
					fout.SetRadix(10);
					fout < "  EvID:" <= id < "  nID:" <= GET_NETWORK_ID(id) < "\n";
#endif
					NETWORK_IN_STREAM.ignore_event();
					break;
			};
		};
		NETWORK_IN_STREAM.next_event();
	};	
	NETWORK_OUT_STREAM.send();
};

void GeneralObject::NetEvent(int type,int id,int creator,int time,int x,int y,int radius)
{
};

int NetInit(ServerFindChain* p)
{
	std::cout<<"NetInit - [start]"<<std::endl;
	int i;
	if(!connect_to_server(p))
		return 0;

	std::cout<<"NetInit - [1]"<<std::endl;
	NetworkON = 1;
	
	LocalStationID = GlobalStationID << 26;
	for(i = 0;i < MAX_NID_OBJECT;i++)
		LocalStationCounter[i] = 0;

	LocalNetEnvironment  =  0;

	set_time_by_server(10);
	std::cout<<"NetInit - [1.5]"<<std::endl;
	my_player_body.clear();
	
	NETWORK_OUT_STREAM.register_name((char*)(aciGetPlayerName() ? aciGetPlayerName() : "Finger"), (char*)(aciGetPlayerPassword() ? aciGetPlayerPassword() : "Password"));
	std::cout<<"NetInit - [2]"<<std::endl;
	//NETWORK_OUT_STREAM.register_name(aciGetPlayerName() ? aciGetPlayerName() : "Finger", "Password");
	my_player_body.color = aciGetPlayerColor();
	std::cout<<"NetInit - [3]"<<std::endl;
	send_player_body(my_player_body);
	std::cout<<"NetInit - [4]"<<std::endl;
	total_players_data_list_query();
	std::cout<<"NetInit - [5]"<<std::endl;

	GloryPlaceNum = 0;
	GloryPlaceData = NULL;
	UsedCheckNum = 0;

	if(object_ID_offsets[NID_DEVICE >> 16] > object_ID_offsets[NID_STUFF >> 16]) stuff_ID_offsets = object_ID_offsets[(NID_DEVICE & (~(1 << 31))) >> 16];
	else stuff_ID_offsets = object_ID_offsets[(NID_STUFF & (~(1 << 31))) >> 16];

	std::cout<<"NetInit - [ok]"<<std::endl;
	return 1;
}

void NetworkWorldClose(void)
{
//	int i;
	leave_world();
//	for(i = 0;i < MAX_NID_OBJECT;i++) LocalStationCounter[i] = 0;
};

int NetworkLoadEnable = 0;

extern uvsVanger* Gamer; //zMod fixed
void aciPrepareWorldsMenu(void);
void NetworkWorldOpen(void)
{
	int i;
	int art_log = 0;

	listElem* p;
	uvsItem* n;


	NetworkLoadEnable = set_world(CurrentWorld,map_size_y);
//	NETWORK_OUT_STREAM.set_position(ViewX,ViewY,round((fabs(curGMap -> xsize*sinTurnInvFlt) + fabs(curGMap -> ysize*cosTurnInvFlt))*0.5));
//	NETWORK_OUT_STREAM.send(1);	

	//zNfo - инициализация game type

	switch(my_server_data.GameType){
		case PASSEMBLOSS:
			if(!GloryPlaceData){
				GloryRnd.aiInit(my_server_data.InitialRND);
				UsedCheckNum = 0;
				GloryPlaceNum = my_server_data.Passembloss.CheckpointsNumber;
				GloryPlaceData = new GloryPlace[GloryPlaceNum];
				for(i=0;i<GloryPlaceNum; i++)
					GloryPlaceData[i].Init(i);
			};
			aciOpenWorldLink(WORLD_FOSTRAL,WORLD_GLORX);
			aciOpenWorldLink(WORLD_FOSTRAL,WORLD_WEEXOW);
			aciOpenWorldLink(WORLD_GLORX,WORLD_XPLO);
			aciOpenWorldLink(WORLD_GLORX,WORLD_NECROSS);
			aciOpenWorldLink(WORLD_GLORX,WORLD_KHOX);
			aciOpenWorldLink(WORLD_NECROSS,WORLD_ARKONOY);
			aciOpenWorldLink(WORLD_NECROSS,WORLD_BOOZEENA);
			aciOpenWorldLink(WORLD_XPLO,WORLD_THREALL);
			aciOpenWorldLink(WORLD_HMOK,WORLD_HMOK);
			aciPrepareWorldsMenu();
			break;
		case VAN_WAR:
			if(!my_server_data.Van_War.WorldAccess){
				aciOpenWorldLink(WORLD_FOSTRAL,WORLD_GLORX);
				aciOpenWorldLink(WORLD_FOSTRAL,WORLD_WEEXOW);
				aciOpenWorldLink(WORLD_GLORX,WORLD_XPLO);
				aciOpenWorldLink(WORLD_GLORX,WORLD_NECROSS);
				aciOpenWorldLink(WORLD_GLORX,WORLD_KHOX);
				aciOpenWorldLink(WORLD_NECROSS,WORLD_ARKONOY);
				aciOpenWorldLink(WORLD_NECROSS,WORLD_BOOZEENA);
				aciOpenWorldLink(WORLD_XPLO,WORLD_THREALL);
				aciOpenWorldLink(WORLD_HMOK,WORLD_HMOK);
			};			
			aciPrepareWorldsMenu();
			break;
	};

	
	if(NetworkLoadEnable){
		for(i = 0;i < TntTableSize;i++){
			NETWORK_OUT_STREAM.create_permanent_object(TntObjectData[i]->NetID,TntObjectData[i]->R_curr.x,TntObjectData[i]->R_curr.y,TntObjectData[i]->radius);
			NETWORK_OUT_STREAM < (int)(0);
			NETWORK_OUT_STREAM.end_body();
		};

		for(i = 0;i < NumLocation;i++){
			NETWORK_OUT_STREAM.create_permanent_object(LocationData[i]->NetID,LocationData[i]->R_curr.x,LocationData[i]->R_curr.y,LocationData[i]->radius);
			NETWORK_OUT_STREAM < (uchar)(LocationData[i]->Mode);
			NETWORK_OUT_STREAM.end_body();
		};

		switch(my_server_data.GameType){
			case VAN_WAR:
				art_log = my_server_data.Van_War.ArtefactsUsing;
				break;
			case MECHOSOMA:
				art_log = my_server_data.Mechosoma.ArtefactsUsing;
				break;
			case PASSEMBLOSS:
				art_log = my_server_data.Passembloss.ArtefactsUsing;
				break;
		};

/*		for(i = 0;i < ProtoCryptTableSize[CurrentWorld];i++){
			if(ProtoCryptTable[CurrentWorld][i].SensorType == 1 || ProtoCryptTable[CurrentWorld][i].SensorType == 2 || 
			ProtoCryptTable[CurrentWorld][i].SensorType == 6 || ProtoCryptTable[CurrentWorld][i].SensorType == 7){
				uvsCreateNewItem(ProtoCryptTable[CurrentWorld][i].R_curr.x,ProtoCryptTable[CurrentWorld][i].R_curr.y,ProtoCryptTable[CurrentWorld][i].R_curr.z,uvsGenerateItemForCrypt(ProtoCryptTable[CurrentWorld][i].SensorType),CurrentWorld);
			}else{
				if(art_log)
					uvsCreateNewItem(ProtoCryptTable[CurrentWorld][i].R_curr.x,ProtoCryptTable[CurrentWorld][i].R_curr.y,ProtoCryptTable[CurrentWorld][i].R_curr.z,ProtoCryptTable[CurrentWorld][i].z0,CurrentWorld);
			};
		};

		p = WorldTable[GameD.cWorld]->Pitem;
		while(p){
			n = (uvsItem*)(p);
			addDevice(n->pos_x,n->pos_y,n->pos_z,n->type,n->param1,n->param2,NULL);
			p = p->next;
		};*/

		
		if(art_log){
			for(i = 0;i < ProtoCryptTableSize[CurrentWorld];i++){
				if((ProtoCryptTable[CurrentWorld][i].SensorType == 3 || ProtoCryptTable[CurrentWorld][i].SensorType == 5)
				   && ProtoCryptTable[CurrentWorld][i].z0 != UVS_ITEM_TYPE::SECTOR && ProtoCryptTable[CurrentWorld][i].z0 != UVS_ITEM_TYPE::SPUMMY && ProtoCryptTable[CurrentWorld][i].z0 != UVS_ITEM_TYPE::PEELOT)
					uvsCreateNewItem(ProtoCryptTable[CurrentWorld][i].R_curr.x,ProtoCryptTable[CurrentWorld][i].R_curr.y,ProtoCryptTable[CurrentWorld][i].R_curr.z,ProtoCryptTable[CurrentWorld][i].z0,CurrentWorld);
			};
		};

		p = WorldTable[GameD.cWorld]->Pitem;
		while(p){
			n = (uvsItem*)(p);
			addDevice(n->pos_x,n->pos_y,n->pos_z,n->type,n->param1,n->param2,NULL);
			p = p->next;
		};		
	};
	FreeList(WorldTable[CurrentWorld]->Pitem);
};

void GeneralObject::GetUser(void)
{
};

char* Convert(char* s,int back = 0);

void aiMessageType::Load(Parser& in)
{
	int i;
	char* n;

	in.search_name("TypeString");
	Type = in.get_int();
	if(iRussian) in.search_name("rNumString");
	else in.search_name("NumString");
	Num = in.get_int();

	if(Num >= AI_MESSAGE_MAX_STRING)
		ErrH.Abort("Bad aiMessage Num String");

	for(i = 0;i < Num;i++){
		in.search_name("Time:");
		Time[i] = in.get_int();
		in.search_name("Color:");
		Color[i] = in.get_int();

		if(iRussian) in.search_name("rString:");
		else in.search_name("String:");
		n = in.get_name();

		if(strlen(n) >= (AI_MESSAGE_LEN_STRING - 1))
			ErrH.Abort("Bad aiMessage String");
		strcpy(Data[i],/*Convert(*/n/*)*/);		
	};

/*	for(i = 0;i < Num;i++){
		in.search_name("Time:");
		Time[i] = in.get_int();
		in.search_name("String:");
		n = in.get_name();
		Data[i] = new char[strlen(n) + 1];
		strcpy(Data[i],n);
	};*/
	LastFrame = 0;
};

void aiMessageType::Send(int speed,int ind,int sf)
{
	int i,c;
/*	static int PrevSpeed = 0;
	if(abs(speed) != 0) PrevSpeed = 10;
	else PrevSpeed--;
	if(sf && PrevSpeed > MAX_AI_MESSAGE_SPEED) return;

	if(abs(LastFrame - frame) > AI_MESSAGE_DELTA){*/
		aiMessageBuffer.align_type = Type;
		c = 0;
		for(i = 0;i < Num;i++){
			if(ind & 1){
				aiMessageBuffer.add_str(c,(unsigned char*)(Data[i]));
				aiMessageBuffer.TimeBuf[c] = Time[i];
				aiMessageBuffer.ColBuf[c] = Color[i];
				c++;
			};			
			ind >>= 1;
		};
		aiMessageBuffer.NumStr = c;
		aciSendPrompt(&aiMessageBuffer);
/*	};
	LastFrame = frame;*/
};

int aiMessageType::GetTime(int ind)
{
	int c,i;
	c = 0;
	for(i = 0;i < Num;i++){
		if(ind & 1) c += strlen(Data[i]);		
		ind >>= 1;
	};
	return c;
};

void VangerUnit::SendCameraData(void)
{
	events_out < (short int)ViewX; 
	events_out < (short int)ViewY; 
	events_out < (short int)ViewZ; 
	events_out < (short int)TurnAngle; 
	events_out < (short int)SlopeAngle; 
	events_out < (short int)curGMap -> xside;
	events_out < (short int)curGMap -> yside;
}

void NetworkSetTnt(int id)
{
	if(!NetworkLoadEnable) TntObjectData[(id & 0xffff) - 1]->NetID = id;
	TntObjectData[(id & 0xffff) - 1]->NetEvent();
};

extern int aciCurJumpCount;

void aiMessageTouch(int speed,int ind)
{
	if(ind >= aiNumMessage)
		return;
//		ErrH.Abort("Bad aiMessage overflow");

	if(ind > AI_MESSAGE_ZNOY) {
		aiMessageQueue.Send(ind,speed);
	} else if(aciWorldLinkExist(CurrentWorld,ind)) {
		aiMessageQueue.Send(ind,speed,1);
	} else if(aciCurJumpCount > 0) {
		aiMessageQueue.Send(ind,speed,4 | 8);
	} else {
		aiMessageQueue.Send(ind,speed,2);
	}
};


void FreeAll(int mode)
{
	switch(mode){
		case 0:
			 GeneralSystemFree();	
			 break;
		case 1:
			GeneralSystemClose();
			MLfree();
			delete vMap;
			GeneralTableFree();
			GeneralSystemFree();
			break;
		case 2:
			MLfree();
			delete vMap;
			GeneralTableFree();
			GeneralSystemFree();
			break;
	};
	GeneralSystemSkip = 1;
};

void PalConstructor::Init(void)
{
	Tail = NULL;
	pPrev = NULL;
	PalEnable = 1;
};

void PalConstructor::Set(int mode,int time,uchar* p1,uchar* p2)
{
	PalPoint* p;
	p = new PalPoint;
	p->Set(mode,time,p1,p2);

	if(!Tail){
		p->Next = p->Prev = NULL;
		Tail = p;
	}else{
		Tail -> Prev = p;
		p -> Next = Tail;
		p -> Prev = NULL;
		Tail = p;
	};
};

void PalConstructor::Clear(PalPoint* p)
{
	if((!p->Prev) && (!p->Next)) Tail = NULL;
	else{
		if(p->Prev) p->Prev->Next = p->Next;
		else Tail = p->Next;
		if(p->Next) p->Next->Prev = p->Prev;
	};
};

void PalConstructor::Free(void)
{
	PalPoint* p;
	PalPoint* pp;
	p = Tail;
	while(p){
		pp = p->Next;
		delete p;
		p = pp;
	};
	Tail = NULL;
};

void PalConstructor::Quant(void)
{
	PalPoint* p;
	PalPoint* pp;
	int m;

	if(ActD.Active == NULL || uvsKronActive == 0)
		return;

	p = NULL;
	m = 0;
	pp = Tail;

	while(pp){
		if(pp->Mode > m){
			p = pp;
			m = pp->Mode;
		};
		pp = pp->Next;
	};

	if(p){
		if(pPrev != p && pPrev != NULL){
			if(p->Mode == CPAL_CHANGE_CYCLE)
				p->Set(CPAL_RESTORE_CHANGE,p->Time);
			else 
				p->Set(p->Mode,p->Time);
		};
		pPrev = p;
		p->Quant();
		if(p->Time <= 0){
			Clear(p);
			delete p;
		};
	}else{
		pPrev = NULL;
		if(PalEnable && !palTr -> quant())
			pal_iter();
	};
};

void PalPoint::Quant(void)
{
	int i;
	if(Mode == CPAL_CHANGE_CYCLE || Mode == CPAL_RESTORE_CHANGE){
		for(i = 0;i < 768;i++){
			FirstColor[i] += DeltaColor[i];
			palbufOrg[i] = palbuf[i] = (uchar)(FirstColor[i] >> 16);
		};
		CurrentWorldLightParam += DeltaWorldLightParam;
		light_modulation = CurrentWorldLightParam >> 8;
	}else{
		for(i = 0;i < 768;i++){
			FirstColor[i] += DeltaColor[i];
			palbufOrg[i] = palbuf[i] = (uchar)(FirstColor[i] >> 16);
		};
	};
	XGR_SetPal(palbuf,0,255);
	Time--;
};

void PalPoint::Set(int mode,int time,uchar* p1,uchar* p2)
{
	int i,t,j;
	Mode = mode;
	Time = time;
	switch(Mode){
		case CPAL_CHANGE_CYCLE:
			for(i = 0;i < 768;i++){
				FirstColor[i] = ((int)(palbuf/*Org*/[i])) << 16;
				DeltaColor[i] = ((((int)(p1[i])) << 16) - FirstColor[i]) / Time;
			};
			CurrentWorldLightParam = light_modulation << 8;
			DeltaWorldLightParam = ((WorldLightParam[CurrentWorld][WorldPalCurrent] << 8) - CurrentWorldLightParam) / time;
			break;
		case CPAL_SHOW_PASSAGE:
			for(i = 0;i < 768;i++){
				if(i / 3 >= BEGCOLOR[5] && i / 3 <=  ENDCOLOR[5]){
					FirstColor[i] = ((int)(palbufSrc[i])) << 16;
					DeltaColor[i] = 0;
				}else{
					FirstColor[i] = 0;
					DeltaColor[i] = (((int)(palbufSrc[i])) << 16) / Time;
				};
			};
			break;
		case CPAL_HIDE_PASSAGE:
			PalCD.PalEnable = 0;
			for(i =0;i < 768;i++){
				FirstColor[i] = ((int)(palbuf/*Org*/[i])) << 16;
				if(i / 3 >= BEGCOLOR[5] && i / 3 <=  ENDCOLOR[5]) DeltaColor[i] = 0;
				else DeltaColor[i] = -FirstColor[i] / Time;
			};
			break;
		case CPAL_RESTORE_CHANGE:
			for(i = 0;i < 768;i++){
				t = FirstColor[i] + DeltaColor[i] * Time;
				FirstColor[i] = ((int)(palbuf/*Org*/[i])) << 16;
				DeltaColor[i] = (t - FirstColor[i]) / Time;
			};
			CurrentWorldLightParam = light_modulation << 8;
			DeltaWorldLightParam = ((WorldLightParam[CurrentWorld][WorldPalCurrent] << 8) - CurrentWorldLightParam) / time;
			break;
		case CPAL_PASSAGE_TO:
			PalCD.PalEnable = 0;
			for(i =0;i < 768;i++){
				FirstColor[i] = ((int)(palbuf/*Org*/[i])) << 16;
				DeltaColor[i] = ((63 << 16) - FirstColor[i]) / Time;					
			};
			break;
		case CPAL_PASSAGE_FROM:
			PalCD.PalEnable = 0;
			for(i =0;i < 768;i++){
				FirstColor[i] = ((int)(palbuf[i])) << 16;
				DeltaColor[i] = ((palbufSrc[i] << 16) - FirstColor[i]) / Time;
			};
			break;
		case CPAL_SPOBS_TO:
			for(i = BEGCOLOR[SPOBS_PAL_TERRAIN]*3;i <= ENDCOLOR[SPOBS_PAL_TERRAIN]*3;i++){
				palbufSrc[i] = palbufOrg[i] = 5 * (i - BEGCOLOR[SPOBS_PAL_TERRAIN]*3) / (3*(ENDCOLOR[SPOBS_PAL_TERRAIN] - BEGCOLOR[SPOBS_PAL_TERRAIN]));
			};

			for(i = 0;i < 768;i++){
				FirstColor[i] = ((int)(palbuf[i])) << 16;
				DeltaColor[i] = ((((int)(palbufOrg[i])) << 16) - FirstColor[i]) / Time;
			};			
			break;
		case CPAL_THREALL_TO:
			for(j = 0;j < TERRAIN_MAX;j++){
				for(i = BEGCOLOR[j];i <= ENDCOLOR[j];i++){
					if(j == 3 || j == 5 || j == 6 || j == 4) break;
					palbufSrc[i*3] = palbufOrg[i*3] = 63 * (i - BEGCOLOR[j]) / (ENDCOLOR[j] - BEGCOLOR[j]);
					palbufSrc[i*3 + 1] = palbufOrg[i*3 + 1] = 63 *(i - BEGCOLOR[j]) / (ENDCOLOR[j] - BEGCOLOR[j]);
					palbufSrc[i*3 + 2] = palbufOrg[i*3 + 2] = 63 * (i - BEGCOLOR[j]) / (ENDCOLOR[j] - BEGCOLOR[j]);
				};
			};
			for(i = 0;i < 768;i++){
				FirstColor[i] = ((int)(palbuf[i])) << 16;
				DeltaColor[i] = ((((int)(palbufOrg[i])) << 16) - FirstColor[i]) / Time;
			};
			break;
	};
};

void aiMessageList::Open(void)
{
	Time = Num = 0;
	View = Tail = NULL;	
};

void aiMessageList::Close(void)
{
	aiMessageListElem* p;
	aiMessageListElem* pp;

	p = Tail;
	while(p){
		pp = p->Next;
		Disconnect(p);
		delete p;
		p = pp;
	};
};

void aiMessageList::Send(int ind,int speed,int n,int sf)
{
	aiMessageListElem* p;
	static int PrevSpeed = 0;
	if(!ActD.Active || !(ActD.Active->ExternalDraw)) return;
	if(abs(speed) != 0) PrevSpeed = 10;
	else PrevSpeed--;
	if(sf && PrevSpeed > MAX_AI_MESSAGE_SPEED) return;	
	if(abs(aiMessageData[ind].LastFrame - frame) > AI_MESSAGE_DELTA){
		p = Tail;
		while(p){
			if(p->Index == ind) return;
			p = p->Next;
		};

		p = new aiMessageListElem;
		p->Speed = speed;
		p->Mask = n;
		p->SpeedFlag = sf;
		p->Index = ind;
		p->Luck = 0;
		p->Dominance = 0;
		p->TabuTask = 0;

		Connect(p);
/*		if(!View){
			View = p;
			aiMessageData[p->Index].Send(p->Speed,p->Mask,0);
			Time = aiMessageData[p->Index].GetTime(p->Mask) / 2 + 2;
		};*/
	};
	aiMessageData[ind].LastFrame = frame;
};

void aiMessageList::SendLuck(int luck)
{
	aiMessageListElem* p;
//	if(!ActD.Active || !(ActD.Active->ExternalDraw)) return;

	p = Tail;
	while(p){
		if(p->Index == AI_MESSAGE_INDEX_LUCK && p != View){
			p->Luck += luck;
			return;
		};
		p = p->Next;
	};

	p = new aiMessageListElem;
	p->Speed = 0;
	p->Mask = 0;
	p->SpeedFlag = 0;
	p->Index = AI_MESSAGE_INDEX_LUCK;
	p->Luck = luck;
	p->Dominance = 0;
	p->TabuTask = 0;

	Connect(p);
/*	if(!View){
		View = p;
		aiPromptShowLuckMessage(p->Luck);
		Time = 100;
	};*/
};

void aiMessageList::SendDominance(int dom)
{
	aiMessageListElem* p;
//	if(!ActD.Active || !(ActD.Active->ExternalDraw)) return;

	p = Tail;
	while(p){
		if(p->Index == AI_MESSAGE_INDEX_DOMINANCE && p != View){
			p->Dominance += dom;
			return;
		};
		p = p->Next;
	};

	p = new aiMessageListElem;
	p->Speed = 0;
	p->Mask = 0;
	p->SpeedFlag = 0;
	p->Index = AI_MESSAGE_INDEX_DOMINANCE;
	p->Luck = 0;
	p->Dominance = dom;
	p->TabuTask = 0;

	Connect(p);
/*	if(!View){
		View = p;
		aiPromptShowDominanceMessage(p->Dominance);
		Time = 100;
	};*/
};

void aiMessageList::SendTabuTask(int luck)
{
	aiMessageListElem* p;	

	p = Tail;
	while(p){
		if(p->Index == AI_MESSAGE_INDEX_TABUTASK && p != View){
			p->TabuTask += luck;
			return;
		};
		p = p->Next;
	};

	p = new aiMessageListElem;
	p->Speed = 0;
	p->Mask = 0;
	p->SpeedFlag = 0;
	p->Index = AI_MESSAGE_INDEX_TABUTASK;
	p->Luck = 0;
	p->Dominance = 0;
	p->TabuTask = luck;

	Connect(p);
/*	if(!View){
		View = p;
		aiPromptShowTaskMessage(p->TabuTask);
		Time = 100;
	};*/
};

void aiMessageList::Quant(void)
{
	aiMessageListElem* p;
	if(View){
		if(Time > 0){
			Time--;
		}else{
			p = View->Prev;
			Disconnect(View);
			delete View;
			if(p){
				switch(p->Index){
					case AI_MESSAGE_INDEX_LUCK:
						aiPromptLuckMessage(p->Luck);
						Time = 70;
						break;
					case AI_MESSAGE_INDEX_DOMINANCE:
						aiPromptDominanceMessage(p->Dominance);
						Time = 50;
						break;
					case AI_MESSAGE_INDEX_TABUTASK:
						aiPromptTaskMessage(p->TabuTask);
						Time = 50;
						break;
					default:
						aiMessageData[p->Index].Send(p->Speed,p->Mask,0);
						Time = aiMessageData[p->Index].GetTime(p->Mask) / 2 + 10;
						break;
				};
			};
			View = p;
		};
	}else{
		if(Tail){
			View = Tail;
			switch(View->Index){
				case AI_MESSAGE_INDEX_LUCK:
					aiPromptLuckMessage(View->Luck);
					Time = 70;
					break;
				case AI_MESSAGE_INDEX_DOMINANCE:
					aiPromptDominanceMessage(View->Dominance);
					Time = 50;
					break;
				case AI_MESSAGE_INDEX_TABUTASK:
					aiPromptTaskMessage(View->TabuTask);
					Time = 50;
					break;
				default:
					aiMessageData[View->Index].Send(View->Speed,View->Mask,0);
					Time = aiMessageData[View->Index].GetTime(View->Mask) / 2 + 10;
					break;
			};
		};		
	};
};

void aiMessageList::Connect(aiMessageListElem* p)
{
	if(!Tail){
		p->Next = p->Prev = NULL;
		Tail = p;
	}else{
		Tail -> Prev = p;
		p -> Next = Tail;
		p -> Prev = NULL;
		Tail = p;
	};
	Num++;
};

void aiMessageList::Disconnect(aiMessageListElem* p)
{
	if((!p->Prev) && (!p->Next)) Tail = NULL;
	else{
		if(p->Prev) p->Prev->Next = p->Next;
		else Tail = p->Next;
		if(p->Next) p->Next->Prev = p->Prev;
	};
	Num--;
};

void OpenCrypt(int world,int id)
{
	TimeSecretEnable[world][id] = 1;
};

void sBunchCreate(int& id,int world,int data,int cycle)
{
	id = NET_SBUNCH_COUNTER | (world << 22);
	NETWORK_OUT_STREAM.create_permanent_object(id,0,0,0);
	NETWORK_OUT_STREAM < (int)(cycle);
	NETWORK_OUT_STREAM < (int)(data);	
	NETWORK_OUT_STREAM.end_body();
};

void sBunchSend(int id,int data,int cycle)
{
	NETWORK_OUT_STREAM.update_object(id,0,0);
	NETWORK_OUT_STREAM < (int)(cycle);
	NETWORK_OUT_STREAM < (int)(data);	
	NETWORK_OUT_STREAM.end_body();	
};

void sBunchReceive(int id)
{
	int t;
	uvsBunch *pb = WorldTable[id >> 22]->escT[0]->Pbunch;
	NETWORK_IN_STREAM > t;
	 pb -> currentStage = t;
	NETWORK_IN_STREAM > t;
	pb -> cycleTable[pb -> currentStage].cirtQ = t;
};

void NetworkQuant(void)
{
	if(NetworkON)
		GameD.NetEvent();
};

int CheckThreallMessiah(void)
{
	if(ActD.Active && GetStuffObject(ActD.Active,ACI_MECHANIC_MESSIAH)) return 1;
	return 0;
};

extern char* iVideoPath;
extern char* iVideoPathDefault;

void PassageImageType::Open(char* name)
{
	short st;
	XStream in(0);
	XBuffer buf(1024);

	buf.init();
	buf < iVideoPathDefault < "svs/" < name;
	if(!in.open(buf.address(),XS_IN)){
		buf.init();
		buf < iVideoPath < "svs/" < name;
		if(!in.open(buf.address(),XS_IN)){
			Name = NULL;
			return;
		};
	};
	
	in.read(Pal,768);
	in > st;
	xSize = st;
	in > st;
	ySize = st;
	in.close();
	Name = new char[strlen(buf.address()) + 1];
	strcpy(Name,buf.address());
};

void PassageImageType::Close(void)
{
	if(Name) {
		delete[] Name;
	}
};

const int MAX_PASSAGE_IMAGE_SHOW_TIME = 200;

void PassageImageType::Show(void)
{
	int i,j;
	int delta[768],start[768];
	int cclx,ccly,ccrx,ccry,ccm;
	short st;

	XStream in;
	if(!Name) return;
	in.open(Name,XS_IN);
	in.read(Pal,768);
	in > st;
	in > st;
	Data = new char[xSize*ySize];
	in.read(Data,xSize*ySize);
	in.close();

	for(i = 0;i < 768;i++){
		start[i] = palbuf[i] << 16;
		delta[i] = -start[i] / MAX_PASSAGE_IMAGE_SHOW_TIME;
	};

	for(j = 0;j < MAX_PASSAGE_IMAGE_SHOW_TIME;j++){
		for(i = 0;i < 768;i++){
			start[i] += delta[i];
			palbuf[i] = (uchar)(start[i] >> 16);
		};
		XGR_SetPal(palbuf,0,255);
	};

//	PassageImageType::Hide();
//	XGR_PutSpr(0,0,xSize,ySize,Data,XGR_BLACK_FON);
	
	ccm = XGR_Obj.ClipMode;
	XGR_SetClipMode(XGR_CLIP_ALL);
	cclx = XGR_Obj.clipLeft;
	ccly = XGR_Obj.clipTop;
	ccrx = XGR_Obj.clipRight;
	ccry = XGR_Obj.clipBottom;
	XGR_SetClip(0,0,XGR_MAXX,XGR_MAXY);
	XGR_PutSpr(0,0,xSize,ySize,Data,XGR_BLACK_FON);	
	XGR_Flush(0,0,XGR_MAXX,XGR_MAXY);
	XGR_SetClip(cclx,ccly,ccrx,ccry);
	XGR_SetClipMode(ccm);

	for(i = 0;i < 768;i++){
		start[i] = 0;
		delta[i] = ((int)(Pal[i]) << 16) / MAX_PASSAGE_IMAGE_SHOW_TIME;
	};

	for(j = 0;j < MAX_PASSAGE_IMAGE_SHOW_TIME;j++){
		for(i = 0;i < 768;i++){
			start[i] += delta[i];
			palbuf[i] = (uchar)(start[i] >> 16);
		};
		XGR_SetPal(palbuf,0,255);
	};
	delete[] Data;
};

void PassageImageType::Hide(void)
{
	int i,j;
	int delta[768],start[768];

	if(!Name) return;

	for(i = 0;i < 768;i++){
		start[i] = Pal[i] << 16;
		delta[i] = -start[i] / MAX_PASSAGE_IMAGE_SHOW_TIME;
	};

	for(j = 0;j < MAX_PASSAGE_IMAGE_SHOW_TIME;j++){
		for(i = 0;i < 768;i++){
			start[i] += delta[i];
			palbuf[i] = (uchar)(start[i] >> 16);
		};
		XGR_SetPal(palbuf,0,255);
	};
	aciSetRedraw();
};

extern int iChatON;
void IncomingChatMessage(void)
{
	if(!iChatON)
		aiMessageQueue.Send(AI_MESSAGE_INCOMING_MESSAGE,0,0xff,0);
};
