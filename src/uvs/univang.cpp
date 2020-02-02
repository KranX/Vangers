/*
		uniVang-STAND Source File
		(C)1997 by K-D Lab
		Author: K-D Lab::KranK, KoTo
*/

#include "../global.h"

//XStream VOVA("VOVA.LST", XS_OUT);

#ifdef _DEBUG
//#define _DEMO_
#define STAND_REPORT
#define TABU_REPORT
#define TOTAL_CASH
#define ALL_ITEM_IN_SHOP
#define CHANGE_CYCLE_ON_NETWORK
#endif
//#define MAX_ITEM_ON_WORLD 100 //unused
#define CIRT_FOR_CONLARVER 16
#define CASH_FOR_DOLLY 20000

#define MAX_TABUTASK_RESERVED 100

#include "../common.h"
#include "../network.h"
//#include "..\win32f.h"

#include "../actint/item_api.h"
#include "../actint/credits.h"

#ifdef _ROAD_
#include "../units/uvsapi.h"
#include "../units/compas.h"
#include "../runtime.h"
#endif

#include "../3d/3d_math.h"
#include "../3d/parser.h"
#include "../terra/vmap.h"
#include "../sound/hsound.h"
#include "diagen.h"
#include "univang.h"

const int TABUTASK_BAD = ACI_TABUTASK_FAILED;
const int TABUTASK_GOOD = ACI_TABUTASK_SUCCESSFUL;

int RACE_WAIT =  300;
int uvsKronActive = 0;
int uvsVersion = 14;
int uvsLoadVersion = 0;
int uvsCurrentWorldUnable = 0;
int uvsCurrentWorldUnableBefore = 0;
int uvsEndHarvest = 0;
int uvsQuantFlag = 0;

int uvsTabuTaskX = 0;
int uvsTabuTaskY = 0;
int uvsTabuTaskFlag = 0;
//int uvsGamerActive = 1;

/* ----------------------------- EXTERN SECTION ---------------------------- */
extern int Dead,Quit;
extern int GameQuantReturnValue;
extern int NetworkON;
extern NetRndType NetRnd;
extern int ChangeArmor;
extern int dgAbortStatus;

void LoadingMessage(int flush = 0);
void ChangeVanger(void);
/* --------------------------- PROTOTYPE SECTION --------------------------- */
//void WaitGame(int, int);
int uvsSecretON(void);
void uvs_aciChangeOneItem(int, int, int, int);
int uvs_aciKillOneItem(int id,int x,int y);
void uvs_aciChangeOneItem(int id_from,int id_to,int x,int y);
void uvs_aciChangeOneItem(int id_from,int id_to,uvsActInt* ptr);
void uvsDeleteItem(listElem*& pe, int type, int count);

void uvsCheckAllItem(void);

char* get_string(XStream& pfile);
void put_string(XStream&, char*);
void ReadPriceList( void  );

void aciSaveData(void);
int thief_on_world(void);
void uvsSetTownName(char*);
void uvsChangeGoodsInList(int world );

int uvsReturnTreasureStatus(int Type, int TreasureStatus = uvsTreasureInGamer);
int GetItem_as_name( char* );
int GetMechos_as_name( char* );
int ItemCount(listElem* Pitem, int ItemType, int what = 0);
uvsItem* GetItem(listElem*, int, int);
uvsItem* GetItem_as_type( listElem* pe,  int status, int work = 0, int index = 0);
uvsVanger* FindFreeVanger(void);
uvsEscave* uvsGetEscaveByID(int type);
uvsSpot* uvsGetSpotByID(int type);

int uvsCheckItem(int type);
void uvsCheckVangerTabuTask(uvsVanger*, int);
void uvsCheckItemOnTabuTask(listElem*&, int ID, int IDCount, int what = 1 );
void uvsChangeTabuTask(int, int);
void uvsRestoreTabuTask(void);

void uvsPassagerFromStatistic(void);
int uvsPoponkaIDON(listElem* Pitem);

int uvsKillerNow(void);
int get_power(int);
int ActInt_to_Item( int );
int uvsMechosType_to_AciInt(int);
extern uchar* actIntPal;

void uvsRestoreVanger(void);
void FirstShopPrepare(int how);
void uvsMechosSave(XStream&);
void uvsMechosLoad(XStream&);

void MLReset(void);

int GeneralMapReload = 0;
extern uchar** WorldPalData;

const int uvsVANGER_ARRIVAL = 0;
const int uvsVANGER_FAILED = 1;
const int uvsVANGER_KILLED = 2;

/* --------------------------- DEFINITION SECTION -------------------------- */

const char* BIOS_NAMES[BIOS_MAX] = {
	"Eleepods", "Beeboorats", "Zeexes"
	};

#ifdef _DEMO_
const int CAR_COLOR[24] = {0, 1, 0, 0, 0, 0, 2, 0,  0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0};
//const int CAR_DEMO[24] = {1, 1, 0, 0, 0, 0, 2, 1,  0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0};
const int CAR_DEMO[24] = {1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0};
//const int CAR_COLOR[5] = {0, 1, 2, 0, 1};
#endif

static const char* PrmSignature = "uniVang-ParametersFile_Ver_1";
static const char* PrmNoneGame = "none";
static const char* PrmWrongMsg = "Wrong Parameter File";
static const char* PrmWrongValue = "Wrong Value in Parameter File";

static uvsEscave *_pe_ = NULL;
static uvsSpot *_ps_ = NULL;
static const char* uvsLastEscaveName = "";
static int uvsGoodsON = 0;

static int GAME_RAITING[V_RACE_VANGER+1] = {100, 50, 30, 10, 5, 1, 0};
//static int BONUS[V_RACE_VANGER] = {1130, 410, 310, 110, 30, 20};

static const char* PrmGameName[GAME_MAX] = {
	"RACE",
	"HARVEST",
	};

static const char* PrmTargetWorkName[UVS_TABUTASK_WORK::NONE+1] = {
		"KILL",
		"DEFENSE",
		"GO_OUT",
		"DELIVER",
		"DELIVER_ONE",
		"GO_WITHOUT_DAMAGE",
		"GO_WITHOUT_GRIB",
		"GO_FLY",
		"CHANGE_CYCLE",
		"SHOT_ANY",
		"END_RACE_RAFFA",
		"END_RACE",
		"END_RACE_WEAPON",
		"KILL_BEEB",
		"KILL_GOLD_BEEB",
		"DELIVER_OR",
		"DELIVER_BAD_CIRT",
		"ARK_NOY",
		"END_CYCLE_IN_WATER",
		"none"
};

static const char* PrmTargetName[UVS_TARGET::NONE] = {
		"none",
		"ESCAVE",
		"SPOT",
		"PASSAGE",
		"DOLLY" ,
		"VANGER",
		"ITEM",
		"FLY_FARMER",
		"BEEB",
		"GRIB",
		"RAFFA",
		"HIVE",
		"CRYPT",
		"VANGERS",
		"RACE",
		"WHIRLPOOL",
		"SWAMP",
		"SHEILD"
};

static const char* PrmFileName[] = {
	"worlds.prm",   //0
	"escaves.prm",   //1
	"spots.prm",        //2
	"passages.prm", //3
	"bunches.prm",   //4
	"vangers.prm",  //5
	"item.prm",           //6
	"car.prm",             //7
	"price.prm",          //8
	"crypt.prm",         //9
	"tabutask.prm"   //10
	};

//zmod2005
//static char* uvsStrings[] = {
//	"Day"
//	};

#ifdef STAND_REPORT
XStream stand("stand.dmp", XS_OUT);
#endif

#ifdef TABU_REPORT
XStream tabu("tabu_event", XS_OUT);
#endif

static listElem* WorldTail = NULL;
listElem* EscaveTail= NULL;
listElem* SpotTail = NULL;
listElem* PassageTail = NULL;
static listElem* BunchTail = NULL;
static listElem* TabuTaskTypeTail = NULL;
uvsTabuTaskType **TabuTable;

listElem* DollyTail = NULL;

uvsMechosType** uvsMechosTable;
uvsItemType** uvsItemTable;
const char *uvsTownName = NULL;

uvsActInt* GMechos;
uvsActInt* GGamerMechos;
uvsActInt* GItem;
uvsActInt* GGamer = NULL;
uvsActInt* GCharItem = NULL;
uvsActInt* GTreasureItem = NULL;
//uvsMechosList* GGamerList = NULL;
uvsGamerResult GamerResult;

uvsWorld* WorldTable[WORLD_MAX];
int uvsCurrentCycle = 0;
#ifdef	  _ROAD_
	extern int CurrentWorld;
#else
static int CurrentWorld;
#endif

static int currentVanger = 1;
static int DollyIndex = 1;
int uvsRandomWorld = 0;
int uvsUnikumeMechos = 1;
int uvsQuantity = 0;
int uvsGamerWaitGame = 0;
time_t uvsFirstTime = 0;
int uvsNetworkChangeWorld = 0;

static unsigned int ChainMap[MAIN_WORLD_MAX*MAIN_WORLD_MAX] = {
	0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFF1, 0xFFFFFFF1,
	0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF, 0xFFFFFFFF,
	0xFFFFFFF1,0xFFFFFFFF,0xFFFFFFFF, 0xFFFFFFF1,
	0xFFFFFFF1,0xFFFFFFFF,0xFFFFFFF1, 0xFFFFFFFF
};

static int uvsPriceList[2][3] = {
	{1, 2, 3},
	{1, 3, 2}
};

uvsContimer ConTimer;		// глобальное время

int cVngNumber;
int c_All_Number;		// число рабов
int c_S_Number; 	// число рабов
int c_R_Number; 	// число рэнжеров
int c_A_Number; 	// число Воров
int  MAX_TABUTASK = 0;		// количество tabutask
int MAX_ITEM_TYPE = 0;	//  число инвентори
int MAX_MECHOS_TYPE = 0;  //  число типов мехосов
int MAX_MECHOS_MAIN = 0;  //  число типов мехосов
int MAX_MECHOS_RAFFA = 0;  //  число типов мехосов
int MAX_MECHOS_CONSTRACTOR = 0;  //  число типов мехосов

int MAX_PART_MACHOS = 2; // число запчастей к машине

int GamePoint;
int uvsBunchNumber = 0;
int uvsPoponkaID = 0;

uvsElement* ETail = NULL;	// список всех динамических объектов
uvsElement* deleteETail = NULL; // список всех удаляемых динамических объектов

uvsVanger* Gamer;

void PrmFile::init(const char* name){
	Parser* prs = new Parser(name);
	len = prs -> length();
	buf = prs -> address();

	int i = 0,mode = 1;
	char c;
	while(i < len){
		c = buf[i];
		if(c == '\"'){
			buf[i++] = '\0';
			while(i < len){
				if(buf[i] == '\"') break;
				i++;
				}
			if(i == len) ErrH.Abort("Wrong PRM format, second <\"> is absent");
			else buf[i] = '\0';
			}
		else
			if(c == ' ' || c == '\t' || c == '\r' || c == ',' || c == '\n') buf[i] = '\0';
		i++;
		}
	i = 0;
	while(i < len){
		c = buf[i];
		if(mode){ if(c == '/' && buf[i + 1] == '*'){ buf[i] = '\0'; mode = 0; } }
		else {
			if(c == '*' && buf[i + 1] == '/'){ buf[i] = buf[i + 1] = '\0'; i++; mode = 1; }
			else if(c != '\n') buf[i] = '\0';
			}
		i++;
		}
	index = 0;
}

char* PrmFile::getAtom(void){
	char* p = buf + index;
	while(index < len && !(*p)) p++, index++;
	if(index == len) return NULL;
	char* ret = p;
	while(index < len && *p) p++, index++;
	return ret;
}

void listElem::link(listElem*& tail){
	if(!tail) {
		tail = this;
	} else {
		tail -> prev -> next = this;
		prev = tail -> prev;
	}
	next = NULL;
	tail -> prev = this;
}

void uvsElement::elink(uvsElement*& tail){
	if(!tail) tail = this;
	else {
		tail -> eprev -> enext = this;
		eprev = tail -> eprev;
		}
	enext = NULL;
	tail -> eprev = this;
}

void uvsElement::delink(uvsElement*& tail){
	if( tail == this) tail = this -> enext;
	if(eprev -> enext) eprev -> enext = enext;
	if(enext){
		enext -> eprev = eprev;
	} else if ( tail ){
		tail -> eprev = eprev;
	}
	if( tail ) tail -> eprev -> enext  = NULL;
}

void uvsElement::eunlink(uvsElement*& tail){
	if(this == tail) tail = enext;
	if(eprev -> enext) eprev -> enext = enext;
	if(enext) enext -> eprev = eprev;
}

listElem* listElem::seekName(const char* name){
	listElem* p = this;
	char* s;
	while(p){
		s = p -> GetName();
		if(!strcmp(name,s)) return p;
		p = p -> next;
		}
	return NULL;
}

int getRW(int n){
	int r = n;

	while( r == n )
		r = RND(MAIN_WORLD_MAX);

	return r;
}

void uniVangPrepare(void){
	std::cout<<"uniVangPrepare"<<std::endl;
	uvsEscave* pe;
	PrmFile pfile;
	char* atom;
	int i;

	aci_curLocationName = "";
	uvsLastEscaveName = "";
	DollyIndex = 1;
	uvsQuantFlag = 0;
	uvsPoponkaID = RND(DG_POPONKA_MAX);
	uniVangDelete();

	CurrentWorld = 0;

	if (NetworkON){
		NetStatisticInit();
		NetStatisticGameStart();

		RNDVAL = my_server_data.InitialRND|1;

		switch (my_server_data.GameType){
		case VAN_WAR:
			break;
		case MECHOSOMA:
			break;
		case PASSEMBLOSS:
			break;
		case HUNTAGE:
			break;
		case MUSTODONT:
			uvsUnikumeMechos = 0;
			break;
		};
	}

	uvsTownName = NULL;
	// item initing
	pfile.init(PrmFileName[6]);
	if(strcmp(PrmSignature,pfile.getAtom())) ErrH.Abort(PrmWrongMsg,XERR_USER,-1,PrmFileName[6]);

	MAX_ITEM_TYPE = atoi(pfile.getAtom());
	uvsItemTable = new uvsItemType*[MAX_ITEM_TYPE];

	for( i = 0; i < MAX_ITEM_TYPE; i++){
		uvsItemTable[i] = new uvsItemType(&pfile);
	}//  end for i
	pfile.finit();

	// worlds initing
	pfile.init(PrmFileName[0]);
	if(strcmp(PrmSignature,pfile.getAtom())) ErrH.Abort(PrmWrongMsg,XERR_USER,-1,PrmFileName[0]);
	uvsWorld* pw;
	while((atom = pfile.getAtom()) != NULL) (pw = new uvsWorld(&pfile,atom)) -> link(WorldTail);
	pfile.finit();

	// escaves initing
	pfile.init(PrmFileName[1]);
	if(strcmp(PrmSignature,pfile.getAtom())) ErrH.Abort(PrmWrongMsg,XERR_USER,-1,PrmFileName[1]);
	while((atom = pfile.getAtom()) != NULL) (pe = new uvsEscave(&pfile,atom)) -> link(EscaveTail);
	pfile.finit();

	// spots initing
	pfile.init(PrmFileName[2]);
	if(strcmp(PrmSignature,pfile.getAtom())) ErrH.Abort(PrmWrongMsg,XERR_USER,-1,PrmFileName[2]);
	uvsSpot* ps;
	while((atom = pfile.getAtom()) != NULL) (ps = new uvsSpot(&pfile,atom)) -> link(SpotTail);
	pfile.finit();

	// passages initing
	pfile.init(PrmFileName[3]);
	if(strcmp(PrmSignature,pfile.getAtom())) ErrH.Abort(PrmWrongMsg,XERR_USER,-1,PrmFileName[3]);
	uvsPassage* pp;
	while((atom = pfile.getAtom()) != NULL) (pp = new uvsPassage(&pfile,atom)) -> link(PassageTail);
	pfile.finit();

// mechos initing
	pfile.init(PrmFileName[7]);
	if(strcmp(PrmSignature,pfile.getAtom())) ErrH.Abort(PrmWrongMsg,XERR_USER,-1,PrmFileName[7]);

	MAX_MECHOS_MAIN = atoi(pfile.getAtom());
	MAX_MECHOS_RAFFA = atoi(pfile.getAtom());
	MAX_MECHOS_CONSTRACTOR = atoi(pfile.getAtom());
	MAX_MECHOS_TYPE = MAX_MECHOS_MAIN + MAX_MECHOS_RAFFA + MAX_MECHOS_CONSTRACTOR;
	uvsMechosTable = new uvsMechosType*[MAX_MECHOS_TYPE];

	for( i = 0; i < MAX_MECHOS_TYPE; i++){
		uvsMechosTable[i] = new uvsMechosType(&pfile);
	}//  end for i
	pfile.finit();

	// TabuTask Initing
	MAX_TABUTASK = 0;
	pfile.init(PrmFileName[10]);
	if(strcmp(PrmSignature,pfile.getAtom())) ErrH.Abort(PrmWrongMsg,XERR_USER,-1,PrmFileName[10]);
	uvsTabuTaskType* pt;
	while((atom = pfile.getAtom()) != NULL) (pt = new uvsTabuTaskType(&pfile,atom)) -> link(TabuTaskTypeTail);
	pfile.finit();

	if (_pe_) _pe_ -> TabuTaskCount = MAX_TABUTASK - _pe_ -> TabuTaskID;
	if (_ps_) _ps_ -> TabuTaskCount = MAX_TABUTASK - _ps_ -> TabuTaskID;

	_ps_ = NULL;
	_pe_ = NULL;

	TabuTable = new uvsTabuTaskType*[MAX_TABUTASK];
	pt = (uvsTabuTaskType*)TabuTaskTypeTail;
	for( i = 0; i < MAX_TABUTASK; i++){
		TabuTable[i] = pt;
		pt = (uvsTabuTaskType*)pt -> next;
	}
	// stationary objects linking 
	pw = (uvsWorld*)WorldTail;
	int __t = 1;
	int _t = 1;
	while(pw){
		i = 0;
		if (pw -> escTmax){
			pw -> escT = new uvsEscave*[pw -> escTmax];
			pe = (uvsEscave*)EscaveTail;
			while(pe){
				if (__t)
					pe -> link_good();
				if(pe -> Pworld == pw){
					if ( i == pw -> escTmax ) ErrH.Abort(PrmWrongMsg,XERR_USER,-1,"escave graiter when MAX");
					pw -> escT[i++] = pe;
				}
				pe = (uvsEscave*)pe -> next;
			}
			__t = 0;
		}
		i = 0;
		if (pw -> sptTmax){
			pw -> sptT = new uvsSpot*[pw -> sptTmax];
			ps = (uvsSpot*)SpotTail;
			while(ps){
				if (_t)
					ps -> link_good();
				if(ps -> Pworld == pw) {
					if ( i == pw -> sptTmax ) ErrH.Abort(PrmWrongMsg,XERR_USER,-1,"spot graiter when MAX");
					pw -> sptT[i++] = ps;
				}
				ps = (uvsSpot*)ps -> next;
				}
			_t = 0;
		}
		i = 0;
		pw -> pssT = new uvsPassage*[pw -> pssTmax];
		pp = (uvsPassage*)PassageTail;
		while(pp){
			if(pp -> Pworld == pw) {
				if ( i == pw -> pssTmax ) ErrH.Abort(PrmWrongMsg,XERR_USER,-1,"passage graiter when MAX");
				pw -> pssT[i++] = pp;
			}
			pp = (uvsPassage*)pp -> next;
			}
		pw = (uvsWorld*)pw -> next;
		}

	//znfo генерация итемов в мире
	for( i = 0; i < MAX_ITEM_TYPE; i++){
		if ( uvsItemTable[i] -> type == UVS_ITEM_STATUS::WEAPON) {
#ifdef ALL_ITEM_IN_SHOP
			if(1)
#else
			if (i == UVS_ITEM_TYPE::MACHOTINE_GUN_LIGHT ||
			    i == UVS_ITEM_TYPE::SPEETLE_SYSTEM_LIGHT ||
			    i == UVS_ITEM_TYPE::GHORB_GEAR_LIGHT )
#endif
				for( int j = 0; j < MAIN_WORLD_MAX; j++) WorldTable[j] -> generate_item( i );
			else
				WorldTable[RND(3)] -> generate_item( i );

		} else if (uvsItemTable[i] -> type == UVS_ITEM_STATUS::DEVICE){

#ifdef ALL_ITEM_IN_SHOP
			for( int j = 0; j < MAIN_WORLD_MAX; j++) WorldTable[j] -> generate_item( i );
#else

			switch(i){
			case UVS_ITEM_TYPE::COPTE_RIG:
				WorldTable[2] -> generate_item( i );
				break;
			case UVS_ITEM_TYPE::CROT_RIG:
				WorldTable[0] -> generate_item( i );
				break;
			case UVS_ITEM_TYPE::CUTTE_RIG:
				WorldTable[1] -> generate_item( i );
				break;
			case UVS_ITEM_TYPE::RADAR_DEVICE:
				WorldTable[RND(3)] -> generate_item( i );
				break;
			}//  end switch
#endif
		} else
			for( int j = 0; j < MAIN_WORLD_MAX; j++) WorldTable[j] -> generate_item( i );
	}//  end for i

	// bunches initing
	pfile.init(PrmFileName[4]);
	if(strcmp(PrmSignature,pfile.getAtom())) ErrH.Abort(PrmWrongMsg,XERR_USER,-1,PrmFileName[4]);
	uvsBunch* pb;
	while((atom = pfile.getAtom()) != NULL) (pb = new uvsBunch(&pfile,atom)) -> link(BunchTail);
	pfile.finit();

	currentVanger = 1;
	// vangers initing
	pfile.init(PrmFileName[5]);
	if(strcmp(PrmSignature,pfile.getAtom())) ErrH.Abort(PrmWrongMsg,XERR_USER,-1,PrmFileName[5]);

	pe = (uvsEscave*) EscaveTail;
	cVngNumber = atoi(pfile.getAtom());
	int meanN = cVngNumber/(MAIN_WORLD_MAX-1);	// среднее число с-вангеров на мир (эскэйв)
	int j = 0;
	uvsVanger* v;
	uvsMechos* pm;
	meanN = 0;

	//zNfo инициализация мехосов
	for( int k = 0; k < MAX_MECHOS_MAIN + MAX_MECHOS_RAFFA; k++){
		pe = WorldTable[RND(3)] -> escT[0];

#ifdef ALL_ITEM_IN_SHOP
		if (1){
#else

			
		if (
			(uvsMechosTable[k] -> type == UVS_CAR_TYPE::RAFFA) ||
			(!strcmp(uvsMechosTable[k] -> name, "OxidizeMonk")) ||
			(!strcmp(uvsMechosTable[k] -> name, "Piercator") ) ||
			NetworkON
		){
#endif
			pe = (uvsEscave*) EscaveTail;
			while(pe){
				for( i = 0; i < 4; i++){
					if (meanN++ < cVngNumber)
						(v = new uvsVanger(pe)) -> elink(ETail);

					(pm = new uvsMechos(k)) -> link(pe -> Pshop -> Pmechos);
					pm -> sort();
					if ( (MAX_MECHOS_MAIN)/2 >= pm -> type || pm -> type >= MAX_MECHOS_MAIN) pm -> status |= UVS_MECHOS_USED::USES;

//					if (NetworkON)
						pm -> status |= UVS_MECHOS_USED::USES;
				}//  end for i
				pe = (uvsEscave*)pe -> next;
			}//  end while
		} else {
			for( i = 0; i < 4; i++){
				if (meanN++ < cVngNumber)
					(v = new uvsVanger(pe)) -> elink(ETail);

				(pm = new uvsMechos(k)) -> link(pe -> Pshop -> Pmechos);
				pm -> sort();
				if ( (MAX_MECHOS_MAIN)/2 >= pm -> type || pm -> type >= MAX_MECHOS_MAIN) pm -> status |= UVS_MECHOS_USED::USES;

//				if (NetworkON)
					pm -> status |= UVS_MECHOS_USED::USES;
			}//  end for i
		}
	}//  end for k
/*	pe = (uvsEscave*) EscaveTail;

	for( i = 0; i < cVngNumber; i++){
		(v = new uvsVanger(pe)) -> elink(ETail);
//		(pm = new uvsMechos(RND(MAX_MECHOS_MAIN + MAX_MECHOS_RAFFA))) -> link(pe -> Pshop -> Pmechos);
//		if ( (MAX_MECHOS_MAIN)/2 >= pm -> type	||  pm -> type >= MAX_MECHOS_MAIN) pm -> status |= UVS_MECHOS_USED::USES;

		if (NetworkON)
			pm -> status |= UVS_MECHOS_USED::USES;

		if(++j == meanN){
			j = 0;
			if(pe -> next){
				pe = (uvsEscave*)pe -> next;
			}//  end if
		}//  end if
	}//  end for i*/

	pe = (uvsEscave*) EscaveTail;
	j = RND(MAX_MECHOS_CONSTRACTOR);

	for( i = 0; i < MAX_MECHOS_CONSTRACTOR; i++,j++){
		if ( j == MAX_MECHOS_CONSTRACTOR ) j = 0;
		(pm = new uvsMechos(MAX_MECHOS_MAIN + MAX_MECHOS_RAFFA + j)) -> link(pe -> Pshop -> Pmechos);
		pm -> status |= UVS_MECHOS_USED::USES;

		if (NetworkON && uvsUnikumeMechos)
			uvsMechosTable[pm -> type] -> constractor = 3;

		if(pe -> next){
			pe = (uvsEscave*)pe -> next;
		}else {
			pe = (uvsEscave*) EscaveTail;
		}//  end if
	}//  end for i

	pfile.finit();

	ReadPriceList();

#ifdef _ROAD_

	if (NetworkON){

//zNfo инициализация своего РНД
		
		RNDVAL *= SDL_GetTicks();
		RNDVAL |= 1;

		uvsRandomWorld = 0;
		uvsQuantity = 0;
		CurrentWorld = 0;
		GamerResult.luck = 100;
		uvsNetworkChangeWorld = 0;

		switch (my_server_data.GameType){
		case VAN_WAR:
			aciUpdateCurCredits(my_server_data.Van_War.InitialCash);
			if (my_server_data.Van_War.Nascency == 255 || my_server_data.Van_War.Nascency == -1)
				uvsRandomWorld = my_server_data.Van_War.Nascency;
			else
				CurrentWorld = my_server_data.Van_War.Nascency;

			if (my_server_data.Van_War.WorldAccess)
				uvsNetworkChangeWorld = 1;
			break;
		case MECHOSOMA:
			aciUpdateCurCredits(my_server_data.Mechosoma.InitialCash);
			CurrentWorld = my_server_data.Mechosoma.World;
			uvsQuantity = my_server_data.Mechosoma.One_at_a_time;
			uvsNetworkChangeWorld = 1;
			break;
		case PASSEMBLOSS:
			aciUpdateCurCredits(my_server_data.Passembloss.InitialCash);
//			CurrentWorld = my_server_data.Passembloss.RandomEscave;
			if (my_server_data.Passembloss.RandomEscave == 255 || my_server_data.Passembloss.RandomEscave == -1)
				uvsRandomWorld = my_server_data.Passembloss.RandomEscave;
			else
				CurrentWorld = my_server_data.Passembloss.RandomEscave;
			break;
		case HUNTAGE:
			aciUpdateCurCredits(my_server_data.Huntage.InitialCash);
			break;
		case MUSTODONT:
			aciUpdateCurCredits(my_server_data.Mustodont.InitialCash);
			uvsUnikumeMechos = my_server_data.Mustodont.UniqueMechosName + MAX_MECHOS_TYPE - MAX_MECHOS_CONSTRACTOR;
			break;
		};

		if (uvsRandomWorld)
			CurrentWorld = RND(3);
	} else
		CurrentWorld = 0;

	WorldTable[ CurrentWorld ] -> GamerVisit++;

	Gamer = v;

	v -> gIndex = 0;

	v -> shape = UVS_VANGER_SHAPE::GAMER;
	Gamer -> status = UVS_VANGER_STATUS::IN_SHOP; // znfo - ванго в магазе
	v -> Pworld = WorldTable[ CurrentWorld ];
	v -> Pescave = v -> Pworld -> escT[0];

#ifdef _DEMO_
	v -> Pescave -> Pbunch -> currentStage = 0;
#endif

	//zNfo  DEFAULT MECHOS 
	// 16 = моток
	int MechosID = 0;
	if (NetworkON) switch (z_my_server_data.mod_id) {
		case Z_MODS_RAFARUN_ID:		{ MechosID = 16; break; } // моток
		case Z_MODS_TRAKTRIAL_ID:	{ MechosID =  7; break; } // аттрактор
		case Z_MODS_NEPTUN_ID:		{ MechosID = 21; break; } // жаба
		case Z_MODS_TEST_ID:		{ MechosID =  5; break; } // дряхлый душегуб
		default: MechosID = 5; // дряхлый душегуб
	}
	v -> Pescave -> Pshop -> sellMechos(v -> Pmechos, MechosID);
	v -> Pmechos -> type = MechosID;
	//zNfo  /DEFAULT MECHOS 

	v -> Pmechos -> sort();
	uvsCurrentCycle = v -> Pworld -> escT[0] -> Pbunch -> currentStage;

	int ObjNum = 0;
	pp = (uvsPassage*)PassageTail;
	while(pp){
		ObjNum++;
		pp = (uvsPassage*)pp -> next;
		}
	pe = (uvsEscave*)EscaveTail;
	while(pe){
		ObjNum++;
		pe = (uvsEscave*)pe -> next;
		}
	ps = (uvsSpot*)SpotTail;
	while(ps){
		ObjNum++;
		ps = (uvsSpot*)ps -> next;
		}
#else
	CurrentWorld = -1;
#endif

	ConTimer.Start();
	GamerResult.Init();
}

void GeneralSystemInit(void);
void GeneralSystemFree(void);
void GeneralSystemClose(void);
void GeneralSystemOpen(void);

extern uchar* palbufOrg;
extern uchar* palbufSrc;
extern uchar* palbuf;
extern int WorldPalCurrent;
extern int light_modulation;
extern int WorldLightParam[WORLD_MAX][3];

void uvsAddStationaryObjs(void){
#ifdef _ROAD_

	GeneralSystemOpen();

#endif
}

void uvsRestoreVanger(void){
#ifdef _ROAD_
	uvsElement* p = ETail;
	uvsVanger* v;

	v = Gamer;
	if (!NetworkON) {
		if( v -> Pescave) {
			addVanger(v,v->Pescave, 1);
		} else {
			addVanger(v,v->Pspot, 1);
		}
	} else {
		if( v -> Pescave ){
			int x, y;
			NetworkGetStart(v -> Pescave -> name, x, y);
			addVanger(v,x,y, 1);
		}else{
			int x, y;
			NetworkGetStart(v -> Pspot -> name, x, y);
			addVanger(v,x, y, 1);
		}
	}
	while(p){
		if(p -> type == UVS_OBJECT::VANGER ){
			v = (uvsVanger*)p;
			if ( v -> shape != UVS_VANGER_SHAPE::GAMER ){
				if(v -> Pworld -> gIndex == CurrentWorld && (v -> status == UVS_VANGER_STATUS::MOVEMENT
						|| v -> status == UVS_VANGER_STATUS::FREE_MOVEMENT
						|| v -> status == UVS_VANGER_STATUS::AIM_HANDLE
						|| v -> status == UVS_VANGER_STATUS::RACE_HUNTER
						|| v -> status == UVS_VANGER_STATUS::GATHERING
						|| v -> status == UVS_VANGER_STATUS::WAIT_GAMER
						|| v -> status == UVS_VANGER_STATUS::RACE)){

					if ( v -> Pmechos -> color < 3 && 
					    WorldTable[v -> Pmechos -> color] -> escT[0] -> Pbunch -> status == UVS_BUNCH_STATUS::UNABLE && 
					    v -> shape != UVS_VANGER_SHAPE::GAMER_SLAVE){
							v -> Pmechos -> color = 3;
							if(v -> shape == UVS_VANGER_SHAPE::BUNCH_SLAVE)
								v -> shape = UVS_VANGER_SHAPE::RANGER;
					}
					addVanger(v,v -> pos_x,v -> pos_y);
				}
			}
		}
		p = p -> enext;
	}
	StartWTRACK();
#endif
}

void uvsWorldReload(int newW){
#ifdef _ROAD_
	uvsElement* p = ETail;
	uvsVanger* v;
	while(p){
		if(p -> type == UVS_OBJECT::VANGER && ((uvsVanger*)p) -> shape != UVS_VANGER_SHAPE::GAMER){
			v = (uvsVanger*)p;
			if(v -> Pworld -> gIndex == newW && (v -> status == UVS_VANGER_STATUS::MOVEMENT
				|| v -> status == UVS_VANGER_STATUS::FREE_MOVEMENT
				|| v -> status == UVS_VANGER_STATUS::AIM_HANDLE
				|| v -> status == UVS_VANGER_STATUS::RACE_HUNTER
				|| v -> status == UVS_VANGER_STATUS::GATHERING
				|| v -> status == UVS_VANGER_STATUS::WAIT_GAMER
				|| v -> status == UVS_VANGER_STATUS::RACE)){

				if ( v -> Pmechos -> color < 3 && 
				    WorldTable[v -> Pmechos -> color] -> escT[0] -> Pbunch -> status == UVS_BUNCH_STATUS::UNABLE && 
				    v -> shape != UVS_VANGER_SHAPE::GAMER_SLAVE){
						v -> Pmechos -> color = 3;
						if(v -> shape == UVS_VANGER_SHAPE::BUNCH_SLAVE)
							v -> shape = UVS_VANGER_SHAPE::RANGER;
				}
				addVanger(v,v -> pos_x,v -> pos_y);
			}
		}
		p = p -> enext;
	}
	CurrentWorld = newW;
	if (WorldTable[CurrentWorld]-> escTmax){
		uvsCurrentCycle = WorldTable[CurrentWorld] -> escT[0] -> Pbunch -> currentStage;

		 if (Gamer -> Pworld -> escT[0] -> Pbunch -> status == UVS_BUNCH_STATUS::UNABLE)
			uvsCurrentWorldUnable = 1;
		else
			uvsCurrentWorldUnable = 0;
	} else {
		uvsCurrentCycle = 0;
		uvsCurrentWorldUnable = 0;
	}

	if (NetworkON && WorldTable[CurrentWorld]-> escTmax){
		uvsBunch* pb = WorldTable[CurrentWorld]-> escT[0] -> Pbunch;

		pb -> cycleTable[pb -> currentStage].cirtQ = pb -> cycleTable[pb -> currentStage].cirtMAX;
		sBunchSend(pb -> id, pb -> cycleTable[pb -> currentStage].cirtQ, pb -> currentStage);
	}

	StartWTRACK();
#endif
}

uvsWorld* getWorld(int n){
	//if ( n >= 0 ) return WorldTable[n]; else return NULL;
	int i = 0;
	listElem* p = WorldTail;
	while(i != n && p) i++, p = p -> next;
	return (uvsWorld*)p;
}

void uvsContimer::Start(void){
	counter = 0;
	day = 1;
	hour = min = sec = 0;
}

void uvsContimer::load(XStream& pfile){
	pfile > counter;
	pfile > day > hour > min > sec;
}

void uvsContimer::save(XStream& pfile){
	pfile < counter;
	pfile < day < hour < min < sec;
}

void uvsBunchCheck(char* subj){
/*	listElem* l = BunchTail;
	while(l){
		((uvsBunch*)l) -> Check(subj);
		l = l -> next;
	}*/

	if (Gamer -> shape != UVS_VANGER_SHAPE::GAMER)
		ErrH.Abort("Bad GAMER", XERR_USER, -1, subj);
}

void uvsBunch::Check(char* subj){
	int i;
	for(i = 0;i < cycleN;i++){
		if (cycleTable[i].Pdolly -> gIndex !=  (biosNindex*3 + i + 1))
			ErrH.Abort("Bunch Dolly incorrect", XERR_USER, -1, subj);
	}
}

void uvsContimer::Quant(void){
	counter++;
	if(++sec == 60){
		sec = 0;
		listElem* l = BunchTail;
		while(l){
			((uvsBunch*)l) -> QuantCirt(counter);
			l = l -> next;
		}

		if(++min == 60){
			min = 0;

			if (hour & 0x00000001){
				GamerResult.dominance--;
				ShowDominanceMessage(-1);
				if (GamerResult.dominance < -100) GamerResult.dominance = -100;
			}

			if(NetworkON)
				Gamer -> Pworld -> updateResource();

			l = EscaveTail;
			while(l){
				((uvsEscave*)l) -> QuantChange(hour);
				l = l -> next;
			}

			if(++hour == 24){
				hour = 0;
				++day;

				if (day == 1000)
					day = 0;

				if ((day&3) == 0)
					uvsRestoreTabuTask();
			}
		}
	}
}

char* uvsContimer::GetTime(void){
	sTime.init();
	//sTime < uvsStrings[0] < strdup(" ") <= day < strdup(", ");//zmod2005
	sTime < "Day " <= day < ", ";
	if(hour < 10) sTime < "0";
	sTime <= hour < ":";
	if(min < 10) sTime < "0";
	sTime <= min < ":";
	if(sec < 10) sTime < "0";
	sTime <= sec;
	return sTime.GetBuf();
}

uvsItemType::uvsItemType(PrmFile* pfile){
	char *s;
	s = pfile -> getAtom();
	name = new char[strlen(s)+1];
	strcpy(name, s);

	type = atoi(pfile -> getAtom());
	SteelerTypeFull = atoi(pfile -> getAtom());
	SteelerTypeEmpty = atoi(pfile -> getAtom());
	size  = atoi(pfile -> getAtom());
	count  = atoi(pfile -> getAtom());
	param1	= atoi(pfile -> getAtom());
	param2	= atoi(pfile -> getAtom());
	gamer_use = 0;
}

uvsWorld::uvsWorld(PrmFile* pfile,char* atom){
	name = new char[strlen(atom)+1];
	strcpy(name, atom);

	x_size = 1 << atoi(pfile -> getAtom());
	y_size = 1 << atoi(pfile -> getAtom());

	escTmax = sptTmax = pssTmax = 0;
	escT = NULL;
	sptT = NULL;
	if(CurrentWorld < WORLD_MAX){
		gIndex = CurrentWorld;
		WorldTable[CurrentWorld++] = this;
		}
	else{
		gIndex = -1;
		ErrH.Abort("uvsWorld::uvsWorld : not have it world in list");
	}
	Pitem = NULL;
//	Pcrypt = NULL;
	Panymal = NULL;
	GamerVisit = 0;
	locked = 0;
}

uvsWorld::~uvsWorld(void){
	uvsElement *pe;
	if (name) delete[] name;

	FreeList( Pitem );

/*	while( Pcrypt ){
		pc = Pcrypt;
		Pcrypt = Pcrypt -> next;
		delete (uvsCrypt*)pc;
	}//  while*/

	while( Panymal ){
		pe = Panymal;
		Panymal = Panymal -> enext;
		switch (pe -> type){
		case UVS_OBJECT::FLY_FARMER:
			delete (uvsFlyFarmer*)pe;
			break;
		}//  case
	}//  while
}

uvsMechosType::uvsMechosType(PrmFile* pfile){
	char *s;
	s = pfile -> getAtom();
	name = new char[strlen(s)+1];
	strcpy(name, s);

	type = atoi(pfile -> getAtom());
	price = atoi(pfile -> getAtom());							  //  цена
	sell_price = atoi(pfile -> getAtom());						 //  цена
	box[0] =  atoi(pfile -> getAtom());							//  вместимость( количество слотов данного вида)
	box[1] =  atoi(pfile -> getAtom());							//  вместимость( количество слотов данного вида)
	box[2] =  atoi(pfile -> getAtom());							//  вместимость( количество слотов данного вида)
	box[3] =  atoi(pfile -> getAtom());							//  вместимость( количество слотов данного вида)
	gamer_use = 0;
	gamer_kill = 0;
	MaxSpeed =  atoi(pfile -> getAtom());
	MaxArmor =  atoi(pfile -> getAtom());
	MaxEnergy =  atoi(pfile -> getAtom());
	DeltaEnergy =  atoi(pfile -> getAtom());
	DropEnergy =  atoi(pfile -> getAtom());
	DropTime =  atoi(pfile -> getAtom());
	MaxFire =  atoi(pfile -> getAtom());
	MaxWater =  atoi(pfile -> getAtom());
	MaxOxigen =  atoi(pfile -> getAtom());
	MaxFly =  atoi(pfile -> getAtom());
	MaxDamage =  atoi(pfile -> getAtom());
	MaxTeleport =  atoi(pfile -> getAtom());
	constractor = 0;
}

uvsWorld::uvsWorld(XStream& pfile ){
	int i, ii, count = 0;

	name = get_string(pfile);
	pfile > x_size; x_size = 1 << x_size;
	pfile > y_size; y_size = 1 << y_size;

	escTmax = sptTmax = pssTmax = 0;
	escT = NULL;
	sptT = NULL;
	pfile > gIndex;
	pfile > GamerVisit;
	if(gIndex < WORLD_MAX && gIndex != -1){
		WorldTable[gIndex] = this;
	} else {
		ErrH.Abort("uvsWorld::uvsWorld : not have it world in list");
	}

	pfile > count;
	uvsItem *pi;
	Pitem = NULL;
	for( i = 0; i < count; i++){
		((listElem*)(pi = new uvsItem(pfile))) -> link(Pitem);
	}

/*	pfile > count;
	uvsCrypt *pc;
	Pcrypt = NULL;
	for( i = 0; i < count; i++){
		((listElem*)(pc = new uvsCrypt(pfile))) -> link(Pcrypt);
	}*/

	pfile > count;
	Panymal = NULL;
	for( i = 0; i < count; i++){
		pfile > ii;
		switch( ii ){
		case UVS_OBJECT::FLY_FARMER:
			(new uvsFlyFarmer(pfile)) -> elink(Panymal);
			break;
		}
	}

	locked = 0;
}

void uvsWorld::save(XStream& pfile ){
	int i, count = 0;

	put_string(pfile, name);
	pfile < get_power(x_size);
	pfile < get_power(y_size);

	pfile < gIndex;
	pfile < GamerVisit;

	listElem *pi = Pitem;
	while(pi){
		count++;
		pi = pi -> next;
	}

	pfile < count;
	pi = Pitem;
	for( i = 0; i < count; i++){
		((uvsItem*)pi) -> save(pfile);
		pi = pi -> next;
	}

/*	count = 0;
	listElem *pc = Pcrypt;
	while(pc){
		count++;
		pc = pc -> next;
	}

	pfile < count;
	pc = Pcrypt;
	for( i = 0; i < count; i++){
		((uvsCrypt*)pc) -> save(pfile);
		pc = pc -> next;
	}
*/
	count = 0;
	uvsElement *pe = Panymal;
	while(pe){
		count++;
		pe = pe -> enext;
	}

	pfile < count;
	pe = Panymal;
	for( i = 0; i < count; i++){
		((uvsFlyFarmer*)pe) -> save(pfile);
		pe = pe -> enext;
	}
}

void uvsWorld::Quant(void){
	uvsElement* pe = Panymal;

	if ( gIndex != CurrentWorld ){
		while(pe){
			pe -> Quant();
			pe = pe -> enext;
		}//  while
	}//  if
}

uvsDolly* uvsWorld::generateDolly(int biosInd){
	uvsDolly* pd = (uvsDolly*)DollyTail;
	while(pd){
		if( pd -> Pworld == this && pd -> biosNindex == biosInd){
			return NULL;
			break;
		}
		pd = (uvsDolly*)pd -> next;
	}
	(pd = new uvsDolly(this,biosInd)) -> link(DollyTail);
	return pd;
}

uvsPassage* uvsWorld::getPassage(uvsWorld* aim){
	for(int i = 0;i < pssTmax;i++) if(pssT[i] -> Poutput == aim) return pssT[i];
	return NULL;
}

uvsEscave::uvsEscave(PrmFile* pfile,char* atom){
	char *s;
	name = new char[strlen(atom)+1];
	strcpy(name, atom);

	Pworld = (uvsWorld*)WorldTail -> seekName(pfile -> getAtom());
	Pshop = new uvsShop();

	Pshop -> Price = new int[MAX_ITEM_TYPE*3];
	memset( Pshop -> Price, 0, MAX_ITEM_TYPE*3*sizeof(int));

	Pshop -> ItemHere = new char[MAX_ITEM_TYPE];
	memset( Pshop -> ItemHere, 0, MAX_ITEM_TYPE*sizeof(char));

//#ifdef ZMOD_BETA
//	//znfo - add /conlarver and cirtainer into shop
//	if (NetworkON && z_my_server_data.mod_id==Z_MODS_TEST_ID) {
//		if (GetItem(Pshop->Pitem, UVS_ITEM_TYPE::CONLARVER,0) == NULL)
//			Pshop->addItem(new uvsItem( UVS_ITEM_TYPE::CONLARVER));
//		if (GetItem(Pshop->Pitem, UVS_ITEM_TYPE::CIRTAINER,0) == NULL)
//			Pshop->addItem(new uvsItem( UVS_ITEM_TYPE::CIRTAINER));
//	}
//#endif

	memset(PassagerStatistic, 0, WORLD_MAX*sizeof(int));
	PassagerFrom = 0;
	PassagerWait = 0;
	lastTabuTaskGood = 0;

	for( int i = 0; i < MAX_ITEM_TYPE; i++)
		Pshop -> Price[ MAX_ITEM_TYPE*2 + i] = (RND(3) << 16) + RND(2);


	pos_x = atoi(pfile -> getAtom());
	pos_y = atoi(pfile -> getAtom());

	if(!Pworld) ErrH.Abort(PrmWrongValue,XERR_USER,-1,"World Name");
	if(pos_x < 0 || pos_x >= Pworld -> x_size) ErrH.Abort(PrmWrongValue,XERR_USER,-1,"pos_x");
	if(pos_y < 0 || pos_y >= Pworld -> y_size) ErrH.Abort(PrmWrongValue,XERR_USER,-1,"pos_y");

	TradeCount = 0;
	Ptrade = NULL;
	Pgood = NULL;
	TabuTaskGood = 0;

	Pworld -> escTmax++;
	Pitem = NULL;
	GamerVisit = 0;

	s = pfile -> getAtom();
	uvsItem *pi = NULL;
	Pitem = NULL;

	if (strcmp( s, PrmNoneGame)){
		int type;
		type = GetItem_as_name( s );

		if( type == -1)
			ErrH.Abort(PrmWrongValue,XERR_USER,-1,"TREASURE Name");
		pi = new uvsItem(type);
		pi -> param1 = 0;
		pi -> param2 = Pworld -> gIndex;

		pi -> link(Pitem);
	}

	s = pfile -> getAtom();
	uvsTradeItem *pt;

	while( strcmp( s, PrmNoneGame) ){
		TradeCount++;
		(pt = new uvsTradeItem) -> link( Ptrade );

		pt -> type = GetItem_as_name( s );
		if(pt -> type == -1)
			ErrH.Abort(PrmWrongValue,XERR_USER,-1,"GOODS Name");

		s = pfile -> getAtom();
		pt -> town_name = new char[strlen(s)+1];
		strcpy(pt -> town_name, s);

		s = pfile -> getAtom();
	}

	locked = 0;
}

uvsEscave::~uvsEscave(void){
	listElem* le;
	if (name) delete[] name;;
	delete Pshop;

	while( Ptrade ){
		le = Ptrade;
		Ptrade = Ptrade -> next;
		delete((uvsTradeItem*) le );
	}

	while( Pgood ){
		le = Pgood;
		Pgood = Pgood -> next;
		delete((uvsTradeItem*) le );
	}
}

uvsSpot::uvsSpot(PrmFile* pfile,char* atom){
	name = new char[strlen(atom)+1];
	strcpy(name, atom);

	TradeCount = 0;
	Pgood = NULL;
	Ptrade = NULL;
	Pitem = NULL;
	TabuTaskGood = 0;
	lastTabuTaskGood = 0;

	PassagerFrom = 0;
	PassagerWait = 0;
	Pworld = (uvsWorld*)WorldTail -> seekName(pfile -> getAtom());

	if (Pworld -> escTmax){
		Pshop = new uvsShop();

		Pshop -> Price = new int[MAX_ITEM_TYPE*3];
		memset( Pshop -> Price, 0, MAX_ITEM_TYPE*3*sizeof(int));

		Pshop -> ItemHere = new char[MAX_ITEM_TYPE];
		memset( Pshop -> ItemHere, 0, MAX_ITEM_TYPE*sizeof(char));
		Pshop -> ItemHere[0] = 1;

		for( int i = 0; i < MAX_ITEM_TYPE; i++)
			Pshop -> Price[ MAX_ITEM_TYPE*2 + i] = (RND(3) << 16) + RND(2);
	} else
		Pshop = NULL;

	memset(PassagerStatistic, 0, WORLD_MAX*sizeof(int));

	pos_x = atoi(pfile -> getAtom());
	pos_y = atoi(pfile -> getAtom());

	if(!Pworld) ErrH.Abort(PrmWrongValue,XERR_USER,-1,"World Name");
	if(pos_x < 0 || pos_x >= Pworld -> x_size) ErrH.Abort(PrmWrongValue,XERR_USER,-1,"pos_x");
	if(pos_y < 0 || pos_y >= Pworld -> y_size) ErrH.Abort(PrmWrongValue,XERR_USER,-1,"pos_y");

	Pworld -> sptTmax++;
	locked = 0;
	GamerVisit = 0;

	char* s = pfile -> getAtom();
	uvsItem *pi = NULL;
	Pitem = NULL;
	if (strcmp( s, PrmNoneGame)){
		int type = GetItem_as_name( s );

		if( type == -1)
			ErrH.Abort(PrmWrongValue,XERR_USER,-1,"TREASURE Name");

		pi = new uvsItem(type);
		pi -> param1 = 0;
		pi -> param2 = Pworld -> gIndex;

		pi -> link( Pitem );
	}

	s = pfile -> getAtom();
	uvsTradeItem *pt;

	while( strcmp( s, PrmNoneGame) ){
		TradeCount++;
		(pt = new uvsTradeItem) -> link( Ptrade );

		pt -> type = GetItem_as_name( s );
		if(pt -> type == -1)
			ErrH.Abort(PrmWrongValue,XERR_USER,-1,"GOODS Name");

		s = pfile -> getAtom();
		pt -> town_name = new char[strlen(s)+1];
		strcpy(pt -> town_name, s);

		s = pfile -> getAtom();
	}
}

void uvsEscave::link_good(void){
	listElem* pt = Ptrade;
	uvsTradeItem* pl;
	uvsEscave* pe;
	uvsSpot* ps;

	while( pt ){
		if ( ps = (uvsSpot*)SpotTail -> seekName(((uvsTradeItem*)pt ) -> town_name) ){
			(pl = new uvsTradeItem) -> link( ps -> Pgood );
			pl -> type = ((uvsTradeItem*)pt) -> type;
		} else if ( pe = (uvsEscave*)EscaveTail -> seekName(((uvsTradeItem*)pt ) -> town_name) ){
			(pl = new uvsTradeItem) -> link( pe -> Pgood );
			pl -> type = ((uvsTradeItem*)pt) -> type;
		} else
			ErrH.Abort("uvsEscave::link_good - not town");
		pt = pt -> next;
	}//  end while
}

int  uvsEscave::SeekID(int type){

	if (TabuTaskID <= type && ((TabuTaskID + TabuTaskCount) > type))
		return TabuTaskID;
	return -1;
}

int  uvsSpot::SeekID(int type){

	if (TabuTaskID <= type && ((TabuTaskID + TabuTaskCount) > type))
		return TabuTaskID;
	return -1;
}

int  uvsEscave::ItemCoef(int type){
	listElem* pt = Ptrade;

	while( pt ){
		if (((uvsTradeItem*) pt ) -> type == type){
			int price = Pshop -> Price[ MAX_ITEM_TYPE*2 + type];
			return uvsPriceList[price&0x0000FFFF][((Pbunch -> currentStage + (price>>16))%3)];
		}
		pt = pt -> next;
	}//  end while
	return 0;
}

int  uvsSpot::ItemCoef(int type){
	listElem* pt = Ptrade;

	while( pt ){
		if (((uvsTradeItem*) pt ) -> type == type){
			int price = Pshop -> Price[ MAX_ITEM_TYPE*2 + type];
			return uvsPriceList[price&0x0000FFFF][((Pworld -> escT[0] -> Pbunch -> currentStage + (price>>16))%3)];
		}
		pt = pt -> next;
	}//  end while
	return 0;
}

void uvsSpot::link_good(void){
	listElem* pt = Ptrade;
	uvsTradeItem* pl;
	uvsEscave* pe;
	uvsSpot* ps;

	while( pt ){
		if ( ps = (uvsSpot*)SpotTail -> seekName(((uvsTradeItem*)pt ) -> town_name) ){
			(pl = new uvsTradeItem) -> link( ps -> Pgood );
			pl -> type = ((uvsTradeItem*)pt) -> type;
		} else if ( pe = (uvsEscave*)EscaveTail -> seekName(((uvsTradeItem*)pt ) -> town_name) ){
			(pl = new uvsTradeItem) -> link( pe -> Pgood );
			pl -> type = ((uvsTradeItem*)pt) -> type;
		} else
			ErrH.Abort("uvsSpot::link_good - not town");
		pt = pt -> next;
	}//  end while
}

uvsSpot::~uvsSpot(void){
	listElem* le;
	if (name) delete[] name;
	if (Pshop) delete Pshop;
	if (Pitem) delete (uvsItem*)Pitem;

	while( Ptrade ){
		le = Ptrade;
		Ptrade = Ptrade -> next;
		delete((uvsTradeItem*)le);
	}

	while( Pgood ){
		le = Pgood;
		Pgood = Pgood -> next;
		delete((uvsTradeItem*) le );
	}
}

int uvsEscave::may_go_away(uvsMechos*& Pm, int shape){
	uvsMechos* _m = Pm;
	if ( locked || Pworld -> locked ) return 0;

//if ( shape == UVS_VANGER_SHAPE::RANGER || shape == UVS_VANGER_SHAPE::BUNCH_SLAVE) Pshop -> sellMechos(Pm);
	if ( shape == UVS_VANGER_SHAPE::BUNCH_SLAVE) Pshop -> sellMechos(Pm);
	if( Pm ) {
		if (Pm != _m){
			Pm -> color = Pbunch -> biosNindex;
			if ( shape == UVS_VANGER_SHAPE::RANGER ) Pm -> color = 3;
		}
		return 1;
	}else return 0;
}

int uvsSpot::may_go_away(uvsMechos*& Pm){

	if ( !Pm ) ErrH.Abort("uvsSpot::may_go_away - no mechos");

	if ( locked || Pworld -> locked ) return 0;
	else return 1;
}

uvsEscave::uvsEscave(XStream& pfile){
	char *s;
	int count;

	name = get_string(pfile);
	s = get_string(pfile);
	TradeCount = 0;
	Ptrade = NULL;
	Pgood = NULL;
	Pworld = (uvsWorld*)WorldTail -> seekName(s);
	Pshop = new uvsShop(pfile);
	delete[] s;

//#ifdef ZMOD_BETA
//	//znfo - add conlarver and cirtainer into shop
//	if (NetworkON && z_my_server_data.mod_id==Z_MODS_TEST_ID) {
//		if (GetItem(Pshop->Pitem, UVS_ITEM_TYPE::CONLARVER,0) == NULL)
//			Pshop->addItem(new uvsItem( UVS_ITEM_TYPE::CONLARVER));
//		if (GetItem(Pshop->Pitem, UVS_ITEM_TYPE::CIRTAINER,0) == NULL)
//			Pshop->addItem(new uvsItem( UVS_ITEM_TYPE::CIRTAINER));
//	}
//#endif

	pfile > pos_x;
	pfile > pos_y;

	if(!Pworld) ErrH.Abort(PrmWrongValue,XERR_USER,-1,"World Name");
	if(pos_x < 0 || pos_x >= Pworld -> x_size) ErrH.Abort(PrmWrongValue,XERR_USER,-1,"pos_x");
	if(pos_y < 0 || pos_y >= Pworld -> y_size) ErrH.Abort(PrmWrongValue,XERR_USER,-1,"pos_y");

	Pworld -> escTmax++;
	pfile > locked;
	pfile > GamerVisit;

	pfile > count;
	uvsItem *pi;
	Pitem = NULL;
	int i;

	for( i = 0; i < count; i++){
		((listElem*)(pi = new uvsItem(pfile))) -> link(Pitem);
	}

	pfile > TradeCount;
	pfile > TabuTaskGood;

	if (uvsLoadVersion > 11)
		pfile > lastTabuTaskGood;
	else
		lastTabuTaskGood = TabuTaskGood;

	uvsTradeItem *pt;

	for( i = 0;  i < TradeCount; i++){
		(pt = new uvsTradeItem) -> link( Ptrade );

		pfile > pt -> type;
		pt -> town_name = get_string(pfile);
	}

	for( i = 0;  i < WORLD_MAX; i++){
		pfile > PassagerStatistic[i];
	}

	if (uvsLoadVersion > 7)
		pfile > PassagerFrom;
	else
		PassagerFrom = 0;

	if (uvsLoadVersion > 8)
		pfile > PassagerWait;
	else
		PassagerWait = 0;
}

uvsSpot::uvsSpot(XStream& pfile){
	char *s;
	name = get_string(pfile);
	s = get_string(pfile);
	TradeCount = 0;
	Ptrade = NULL;
	Pgood = NULL;
	Pworld = (uvsWorld*)WorldTail -> seekName(s);
	if (Pworld -> escTmax){
		Pshop = new uvsShop(pfile);
	}  else
		Pshop = NULL;
	delete[] s;

	pfile > pos_x;
	pfile > pos_y;

	if(!Pworld) ErrH.Abort(PrmWrongValue,XERR_USER,-1,"World Name");
	if(pos_x < 0 || pos_x >= Pworld -> x_size) ErrH.Abort(PrmWrongValue,XERR_USER,-1,"pos_x");
	if(pos_y < 0 || pos_y >= Pworld -> y_size) ErrH.Abort(PrmWrongValue,XERR_USER,-1,"pos_y");

	Pworld -> sptTmax++;
	pfile > locked;
	pfile > GamerVisit;

	int count;
	pfile > count;
	uvsItem *pi;
	Pitem = NULL;
	int i;

	for( i = 0; i < count; i++){
		((listElem*)(pi = new uvsItem(pfile))) -> link(Pitem);
	}

	pfile > TradeCount;
	pfile > TabuTaskGood;

	if (uvsLoadVersion > 11)
		pfile > lastTabuTaskGood;
	else
		lastTabuTaskGood = TabuTaskGood;

	uvsTradeItem *pt;

	for( i = 0;  i < TradeCount; i++){
		(pt = new uvsTradeItem) -> link( Ptrade );

		pfile > pt -> type;
		pt -> town_name = get_string(pfile);
	}

	for( i = 0;  i < WORLD_MAX; i++){
		pfile > PassagerStatistic[i];
	}

	if (uvsLoadVersion > 7)
		pfile > PassagerFrom;
	else
		PassagerFrom = 0;

	if (uvsLoadVersion > 8)
		pfile > PassagerWait;
	else
		PassagerWait = 0;
}

void uvsEscave::save(XStream& pfile){
	int i;
	int count;

	put_string(pfile, name);
	put_string(pfile, Pworld -> name);
	Pshop -> save(pfile);

	pfile < pos_x;
	pfile < pos_y;

	pfile < locked;
	pfile < GamerVisit;

	listElem *pi = Pitem;
	count  = 0;
	while(pi){
		count++;
		pi = pi -> next;
	}

	pfile < count;
	pi = Pitem;
	for( int i = 0; i < count; i++){
		((uvsItem*)pi) -> save(pfile);
		pi = pi -> next;
	}

	pfile < TradeCount;
	pfile < TabuTaskGood;
	pfile < lastTabuTaskGood;
	uvsTradeItem *pt = (uvsTradeItem*)Ptrade;

	for( i = 0;  i < TradeCount; i++){
		pfile < pt -> type;
		put_string(pfile, pt -> town_name);
		pt = (uvsTradeItem*)(pt -> next);
	}

	for( i = 0;  i < WORLD_MAX; i++){
		pfile < PassagerStatistic[i];
	}

	pfile < PassagerFrom;
	pfile < PassagerWait;
}

void uvsSpot::save(XStream& pfile){
	put_string(pfile, name);
	put_string(pfile, Pworld -> name);
	if ( Pshop )
		Pshop -> save(pfile);

	pfile < pos_x;
	pfile < pos_y;

	pfile < locked;
	pfile < GamerVisit;

	listElem *pi = Pitem;
	int count  = 0;
	while(pi){
		count++;
		pi = pi -> next;
	}

	pfile < count;
	pi = Pitem;
	int i;
	for( i = 0; i < count; i++){
		((uvsItem*)pi) -> save(pfile);
		pi = pi -> next;
	}

	pfile < TradeCount;
	pfile < TabuTaskGood;
	pfile < lastTabuTaskGood;

	uvsTradeItem *pt = (uvsTradeItem*)Ptrade;

	for( i = 0;  i < TradeCount; i++){
		pfile < pt -> type;
		put_string(pfile, pt -> town_name);
		pt = (uvsTradeItem*)(pt -> next);
	}

	for( i = 0;  i < WORLD_MAX; i++){
		pfile < PassagerStatistic[i];
	}
	pfile < PassagerFrom;
	pfile < PassagerWait;
}

void uvsShop::addMechos(uvsMechos* Pm){
	((listElem*)Pm) -> link( Pmechos );
}

void uvsShop::addItem(uvsItem* Pi){
	((listElem*)Pi) -> link( Pitem );
}

int  uvsShop::FindMechos(int Type){
	listElem *pm = Pmechos;
	while(pm){
		if (((uvsMechos*)pm) ->type == Type)
			return 1;
		pm = pm -> next;
	}
	return 0;
}

void uvsShop::updateList(listElem* pl){
	while(pl){
		ItemHere[((uvsItem*)pl)-> type] = 1;
		pl = pl -> next;
	}
}

void uvsShop::get_list_from_ActInt( uvsActInt*& Mechos, uvsActInt*& Item){
	uvsActInt* pa;
	uvsItem* pl;
	int dead_world = (Gamer -> Pworld -> escT[0] -> Pbunch -> status == UVS_BUNCH_STATUS::UNABLE);
	//Pmechos = NULL;

	int first_constr = MAX_MECHOS_TYPE - MAX_MECHOS_CONSTRACTOR;
	pa = Mechos;
	while(pa){
		if (Mechos -> type >= first_constr ){
			int _type = Mechos -> type - first_constr;
			Mechos -> type =  first_constr + (_type>>2);
			_type &= 3;
			uvsMechosTable[Mechos -> type] -> constractor = _type;
		}
		(new uvsMechos( Mechos -> type )) -> link( Pmechos );
		Mechos = (uvsActInt*)Mechos -> next;
		delete (uvsActInt*)pa;
		pa = Mechos;
	}
	Mechos = NULL;

	pa = Item;
	while(pa){
		int tmp_type = ActInt_to_Item( Item -> type);

		if ( uvsItemTable[ tmp_type ] -> type != UVS_ITEM_STATUS::GOODS){
			(pl = new uvsItem( tmp_type )) -> link( Pitem );

			if (pl -> type == UVS_ITEM_TYPE::CONLARVER || dead_world){
				pl -> param1 = Item -> param1;
				pl -> param2 = Item -> param2;
			}
		} else {
			if (tmp_type == UVS_ITEM_TYPE::ELEEPOD ||
			     tmp_type == UVS_ITEM_TYPE::BEEBOORAT ||
			     tmp_type == UVS_ITEM_TYPE::ZEEX){
				if (Gamer -> Pescave )
					Gamer -> Pescave -> PassagerWait = 0;
				else
					Gamer -> Pspot -> PassagerWait = 0;
			}//  end if
		}//  end if

		Item = (uvsActInt*)Item -> next;
		delete (uvsActInt*)pa;
		pa = Item;
	}
	Item = NULL;
	updateList(Pitem);
}

void uvsVanger::addInventory(void){
//	DBGCHECK;
	int i;
	uvsItem *pi;
	char* ItemHere;

	if (Pescave)
		ItemHere = Pescave -> Pshop -> ItemHere;
	else
		ItemHere = Pspot -> Pshop -> ItemHere;

	if ( uvsMechosTable[Pmechos -> type] -> type == ((int)UVS_CAR_TYPE::RAFFA) ) return;

	if (shape == UVS_VANGER_SHAPE::KILLER){
		pi = new uvsItem(UVS_ITEM_TYPE::SPEETLE_SYSTEM_HEAVY);
		if(!addItem(pi)) delete pi;

		pi = new uvsItem(UVS_ITEM_TYPE::SPEETLE_SYSTEM_LIGHT);
		if(!addItem(pi)) delete pi;

		pi = new uvsItem(UVS_ITEM_TYPE::SPEETLE_SYSTEM_LIGHT);
		if(!addItem(pi)) delete pi;

		for( int i = 0; i < 10; i++){
			pi = new uvsItem(UVS_ITEM_TYPE::SPEETLE_SYSTEM_AMMO1);
			addItem(pi, 1);
		}
	}

	i = RND(6);
	while(!ItemHere[i]){
		i = RND(6);
	}
	pi = new uvsItem(i);

	if(!addItem(pi)) {
		 ((uvsItem*)pi) -> type = 0;
		 if(!addItem(pi)) delete pi;
	}

	if ( !ItemHere[UVS_ITEM_TYPE::TERMINATOR] ){
		i = RND(6);
		while(!ItemHere[i]){
			i = RND(6);
		}
		pi = new uvsItem(i);
	} else {
		pi = new uvsItem(UVS_ITEM_TYPE::TERMINATOR);
	}

	if(!addItem(pi)) delete (uvsItem*)pi;

	if (!RND(3) && ItemHere[UVS_ITEM_TYPE::COPTE_RIG]){
		pi = new uvsItem(UVS_ITEM_TYPE::COPTE_RIG);
		if(!addItem(pi)) delete (uvsItem*)pi;
	}

	if ( (shape == UVS_VANGER_SHAPE::KILLER ||
	     shape == UVS_VANGER_SHAPE::CARAVAN ||
	     shape == UVS_VANGER_SHAPE::RANGER ||
	     shape == UVS_VANGER_SHAPE::THIEF)
	     && (ItemHere[UVS_ITEM_TYPE::RADAR_DEVICE])
	     ){
		pi = new uvsItem(UVS_ITEM_TYPE::RADAR_DEVICE);

		if(!addItem(pi)) delete (uvsItem*)pi;
	}
//	DBGCHECK;
}

void uvsTabuTaskType::activateVANGER(void){
	uvsVanger *pv = FindFreeVanger();

	switch(work_on_target){
	case UVS_TABUTASK_WORK::GO_OUT:
		pv -> goNewEscave();

		if (pv -> Pescave){
			pv -> pos_x = pv -> Pescave -> pos_x + 1024;
			pv -> pos_y = pv -> Pescave -> pos_y;
		}else{
			pv -> pos_x = pv -> Pspot -> pos_x + 1024;
			pv -> pos_y = pv -> Pspot -> pos_y;
		}

		pv -> status = UVS_VANGER_STATUS::FREE_MOVEMENT;
		Ptarget = (uvsTarget*)pv;
		break;
	case UVS_TABUTASK_WORK::KILL:
		if (pv -> Pescave){
			pv -> set_target(pv -> Pworld -> sptT[0], UVS_TARGET::SPOT);

			pv -> pos_x = pv -> Pescave -> pos_x + 1024;
			pv -> pos_y = pv -> Pescave -> pos_y;

		}else{
			pv -> set_target(pv -> Pworld -> escT[0], UVS_TARGET::ESCAVE);
			pv -> pos_x = pv -> Pspot -> pos_x + 1024;
			pv -> pos_y = pv -> Pspot -> pos_y;
		}

		pv -> addInventory();
		pv -> status = UVS_VANGER_STATUS::FREE_MOVEMENT;
		Ptarget = (uvsTarget*)pv;
		break;
	case UVS_TABUTASK_WORK::DEFENSE:
		if (pv -> Pescave)
			pv -> set_target(pv -> Pescave, UVS_TARGET::ESCAVE);
		else
			pv -> set_target(pv -> Pspot, UVS_TARGET::SPOT);

		pv -> pos_x = pv -> Pworld -> pssT[0] -> pos_x + 1024;
		pv -> pos_y = pv -> Pworld -> pssT[0] -> pos_y;

		pv -> addInventory();
		pv -> status = UVS_VANGER_STATUS::FREE_MOVEMENT;
		Ptarget = (uvsTarget*) pv;
		break;
	};
}

void uvsTabuTaskType::activateDOLLY(void){
	switch(work_on_target){
	case UVS_TABUTASK_WORK::CHANGE_CYCLE:
		*real_count = Gamer -> Pworld -> escT[0] -> Pbunch -> nextCycle() + 1;
		break;
	};
}

int uvsTabuTaskType::Erase(int param1, int& param2){
	if (status == UVS_TABUTASK_STATUS::ACTIVE){
		switch( target ){
		case UVS_TARGET::PASSAGE:
		case UVS_TARGET::RACE:
		case UVS_TARGET::VANGERS:
		case UVS_TARGET::ESCAVE:
		case UVS_TARGET::SPOT:
		case UVS_TARGET::DOLLY:
			param2 |= ((int)(UVS_TABUTASK_STATUS::BAD)<<16);

			ShowTaskMessage(-luck);
			GamerResult.luck -=  luck;
			if (GamerResult.luck < 0) GamerResult.luck = 0;
			break;
		case UVS_TARGET::VANGER:
			switch(work_on_target){
			case UVS_TABUTASK_WORK::GO_OUT:
			case UVS_TABUTASK_WORK::KILL:
			case UVS_TABUTASK_WORK::DEFENSE:
				param2 |= ((int)(UVS_TABUTASK_STATUS::GOOD)<<16);

				ShowTaskMessage(-luck);
				GamerResult.luck -=  luck;
				if (GamerResult.luck < 0) GamerResult.luck = 0;

				if (((uvsVanger*)Ptarget) -> shape == UVS_VANGER_SHAPE::TABUTASK)
					((uvsVanger*)Ptarget) -> shape = UVS_VANGER_SHAPE::BUNCH_SLAVE;
				break;
			};
			break;
		case UVS_TARGET::ITEM:
			switch(work_on_target){
			case UVS_TABUTASK_WORK::DELIVER:
			case UVS_TABUTASK_WORK::DELIVER_ONE:
			case UVS_TABUTASK_WORK::DELIVER_OR:
				param2 |= ((int)(UVS_TABUTASK_STATUS::BAD)<<16);

				ShowTaskMessage(-luck);
				GamerResult.luck -= luck;
				if (GamerResult.luck < 0) GamerResult.luck = 0;
				break;
			};//  end switch
			break;
		case UVS_TARGET::FLY_FARMER:
			break;
		case UVS_TARGET::SHEILD:
		case UVS_TARGET::SWAMP:
		case UVS_TARGET::WHIRLPOOL:
		case UVS_TARGET::BEEB:
		case UVS_TARGET::GRIB:
		case UVS_TARGET::HIVE:
		case UVS_TARGET::CRYPT:
		case UVS_TARGET::RAFFA:
			param2 |= ((int)(UVS_TABUTASK_STATUS::BAD)<<16);

			ShowTaskMessage(-luck);
			GamerResult.luck -= luck;
			if (GamerResult.luck < 0) GamerResult.luck = 0;
			break;
		case UVS_TARGET::NONE:
			break;
		};
		status = UVS_TABUTASK_STATUS::SLEEP;
		return 1;
	}//  end if
	return 0;
}

int uvsTabuTaskType::checkCycleEvent(int param1, int& param2){
	if ((param1 > cycle) && (status == UVS_TABUTASK_STATUS::ACTIVE)){
		switch( target ){
		case UVS_TARGET::PASSAGE:
		case UVS_TARGET::SPOT:
		case UVS_TARGET::ESCAVE:
		case UVS_TARGET::RACE:
		case UVS_TARGET::VANGERS:
			param2 |= ((int)(UVS_TABUTASK_STATUS::BAD)<<16);
			
			ShowTaskMessage(-luck);
			GamerResult.luck -=  luck;
			if (GamerResult.luck < 0) GamerResult.luck = 0;
			status = UVS_TABUTASK_STATUS::SLEEP;
			break;
		case UVS_TARGET::DOLLY:
			if (work_on_target == UVS_TABUTASK_WORK::END_CYCLE_IN_WATER && FuckOFFinWater()){
				param2 |= ((int)(UVS_TABUTASK_STATUS::GOOD)<<16);
				uvsChangeTownTabuTask(param2 & 0x0000FFFF);

				ShowTaskMessage(luck);
				GamerResult.luck += luck;
				if (GamerResult.luck > 100) GamerResult.luck = 100;
				status = UVS_TABUTASK_STATUS::OK;
			} else {
				param2 |= ((int)(UVS_TABUTASK_STATUS::BAD)<<16);

				ShowTaskMessage(-luck);
				GamerResult.luck -=  luck;
				if (GamerResult.luck < 0) GamerResult.luck = 0;
				status = UVS_TABUTASK_STATUS::SLEEP;
			}
			break;
		case UVS_TARGET::VANGER:
			switch(work_on_target){
			case UVS_TABUTASK_WORK::GO_OUT:
			case UVS_TABUTASK_WORK::DEFENSE:
				param2 |= ((int)(UVS_TABUTASK_STATUS::GOOD)<<16);
				uvsChangeTownTabuTask(param2 & 0x0000FFFF);

				ShowTaskMessage(luck);
				GamerResult.luck += luck;
				if (GamerResult.luck > 100) GamerResult.luck = 100;
				status = UVS_TABUTASK_STATUS::OK;

				if (((uvsVanger*)Ptarget) -> shape == UVS_VANGER_SHAPE::TABUTASK)
					((uvsVanger*)Ptarget) -> shape = UVS_VANGER_SHAPE::BUNCH_SLAVE;
				break;
			case UVS_TABUTASK_WORK::KILL:
				param2 |= ((int)(UVS_TABUTASK_STATUS::BAD)<<16);

				ShowTaskMessage(-luck);
				GamerResult.luck -= luck;
				if (GamerResult.luck < 0) GamerResult.luck = 0;
				status = UVS_TABUTASK_STATUS::SLEEP;

				if (((uvsVanger*)Ptarget) -> shape == UVS_VANGER_SHAPE::TABUTASK)
					((uvsVanger*)Ptarget) -> shape = UVS_VANGER_SHAPE::BUNCH_SLAVE;
				break;
			};
			break;
		case UVS_TARGET::ITEM:
			switch(work_on_target){
			case UVS_TABUTASK_WORK::DELIVER:
			case UVS_TABUTASK_WORK::DELIVER_ONE:
			case UVS_TABUTASK_WORK::DELIVER_OR:
				param2 |= ((int)(UVS_TABUTASK_STATUS::BAD)<<16);

				ShowTaskMessage(-luck);
				GamerResult.luck -= luck;
				if (GamerResult.luck < 0) GamerResult.luck = 0;
				status = UVS_TABUTASK_STATUS::SLEEP;
				break;
			};//  end switch
			break;
		case UVS_TARGET::FLY_FARMER:
			break;
		case UVS_TARGET::BEEB:
		case UVS_TARGET::GRIB:
		case UVS_TARGET::HIVE:
		case UVS_TARGET::CRYPT:
		case UVS_TARGET::WHIRLPOOL:
		case UVS_TARGET::SWAMP:
		case UVS_TARGET::SHEILD:
		case UVS_TARGET::RAFFA:
			param2 |= ((int)(UVS_TABUTASK_STATUS::BAD)<<16);

			ShowTaskMessage(-luck);
			GamerResult.luck -= luck;
			if (GamerResult.luck < 0) GamerResult.luck = 0;
			status = UVS_TABUTASK_STATUS::SLEEP;
			break;
		case UVS_TARGET::NONE:
			break;
		};
//		status = UVS_TABUTASK_STATUS::SLEEP;
		return 1;
	}//  end if
	return 0;
}

void uvsTabuTaskType::activate(void){
	int i;

	if (status == UVS_TABUTASK_STATUS::ACTIVE) return;

	status = UVS_TABUTASK_STATUS::ACTIVE;

	switch( target ){
	case UVS_TARGET::RACE:
		break;
	case UVS_TARGET::DOLLY:
		*real_count = 0;
		activateDOLLY();
		break;
	case UVS_TARGET::VANGER:
		*real_count = 0;
		activateVANGER();
		break;
	case UVS_TARGET::VANGERS:
	case UVS_TARGET::ESCAVE:
	case UVS_TARGET::SPOT:
	case UVS_TARGET::PASSAGE:
	case UVS_TARGET::ITEM:
	case UVS_TARGET::FLY_FARMER:
	case UVS_TARGET::BEEB:
	case UVS_TARGET::GRIB:
	case UVS_TARGET::HIVE:
	case UVS_TARGET::CRYPT:
	case UVS_TARGET::WHIRLPOOL:
	case UVS_TARGET::SWAMP:
	case UVS_TARGET::SHEILD:
	case UVS_TARGET::RAFFA:
		for( i = 0; i < item_number; i++)
			real_count[i] = 0;
		break;
	case UVS_TARGET::NONE:
		break;
	};
}

void uvsVanger::get_list_from_ActInt( uvsActInt*& Item, uvsActInt*& Mechos){
	uvsActInt* pa;
	uvsItem* pi;
	int _tabu_taskID_;
	int _tabu_taskCOUNT_;

	if (Pescave){
		_tabu_taskID_ = Pescave -> TabuTaskID;
		_tabu_taskCOUNT_ = Pescave -> TabuTaskCount;
	} else {
		_tabu_taskID_ = Pspot -> TabuTaskID;
		_tabu_taskCOUNT_ = Pspot -> TabuTaskCount;
	}

	//Pitem = NULL;
	pa = Item;
	while(pa){
		Item = (uvsActInt*)Item -> next;
		(pi = new uvsItem(ActInt_to_Item(pa -> type))) -> link(Pitem);

		uvsDomChangeFromItem(pi -> type, 1, 0);

		pi -> param1 = pa -> param1;
		pi -> param2 = pa -> param2;

		pi -> pos_x = pa -> pos_x;
		pi -> pos_y = pa -> pos_y;

//		if ( (pi -> type == UVS_ITEM_TYPE::TABUTASK) && ((pi -> param2 & 0xFFFF0000)  == 0)){
//			TabuTable[pi -> param2 ] -> activate();
//		}

		delete (uvsActInt*)pa;
		pa = Item;
	}
	Item = NULL;

	if ( Mechos ){
		int first_constr = MAX_MECHOS_TYPE - MAX_MECHOS_CONSTRACTOR;
		if (Mechos -> type >= first_constr ){
			int _type = Mechos -> type - first_constr;
			Mechos -> type =  first_constr + (_type>>2);
			_type &= 3;
			Pmechos = new uvsMechos(Mechos -> type);
			uvsMechosTable[Mechos -> type] -> constractor = _type;
		} else {
			Pmechos = new uvsMechos(Mechos -> type);
		}

		if (strcmp(uvsMechosTable[Pmechos -> type] -> name, "LawnMower"))
			SetMotorFile(uvsMechosTable[Pmechos -> type] -> type);
		else
			SetMotorFile(6);
#ifndef _DEMO_
		Pmechos -> color = Mechos -> param1;
#else
		Pmechos -> color = CAR_COLOR[Mechos -> type];
#endif
		Pmechos -> teleport = Mechos -> param2;
		delete Mechos;
	}
	Mechos = NULL;

//	uvsCheckItemOnTabuTask(Pitem, _tabu_taskID_, _tabu_taskCOUNT_, 0);

	pi = (uvsItem*)Pitem;

	uvsTabuTaskFlag = 0;
	while(pi){
		if ( (pi -> type == UVS_ITEM_TYPE::TABUTASK) && ((pi -> param2 & 0xFFFF0000)  == 0)){
			if (!TabuTable[pi -> param2 ] -> status){
				uvsTabuTaskX = pi -> pos_x;
				uvsTabuTaskY = pi -> pos_y;
				uvsTabuTaskFlag = 1;
			}

			TabuTable[pi -> param2 ] -> activate();
		}

		pi = (uvsItem*)pi -> next;
	}

	pa = GTreasureItem;

	while( pa ){
		uvsActInt* pl = pa;
		pa = (uvsActInt*)pa -> next;

		if (pl -> type == ACI_RUBOX){
			GamerResult.rubbox_count--;
			pl -> delink(GTreasureItem);
			delete pl;
		}
	}//  end while

	if(GTreasureItem){
		if (Pescave){
			((uvsItem*)Pescave -> Pitem) -> param1 = 0;
		} else {
			((uvsItem*)Pspot -> Pitem) -> param1 = 0;
		}
		delete (uvsActInt*)GTreasureItem;
		GTreasureItem = NULL;

	} else {

		if (Pescave){
			if ( ((uvsItem*)Pescave -> Pitem) && (((uvsItem*)Pescave -> Pitem) -> param1 == uvsTreasureInShop)){
				Pworld -> escT[0] -> Pbunch -> status |= (int)UVS_BUNCH_STATUS::KILL_GAMER;

				ShowDominanceMessage(-15);
				GamerResult.dominance -= 15;
				if (GamerResult.dominance < 0) GamerResult.dominance = 0;

				((uvsItem*)Pescave -> Pitem) -> param1 = uvsTreasureInGamer;
			}
		} else {
			if ( ((uvsItem*)Pspot -> Pitem) && (((uvsItem*)Pspot -> Pitem) -> param1 == uvsTreasureInShop)){
				Pworld -> escT[0] -> Pbunch -> status |= (int)UVS_BUNCH_STATUS::KILL_GAMER;
					
				ShowDominanceMessage(-15);
				GamerResult.dominance -= 15;
				if (GamerResult.dominance < 0) GamerResult.dominance = 0;

				((uvsItem*)Pspot -> Pitem) -> param1 = uvsTreasureInGamer;
			}
		}
	}
}

void uvsShop::prepare_list_for_ActInt( uvsActInt*& Mechos, uvsActInt*& Item, int where){
	listElem* pe;
	listElem* pb;
	listElem* pn;
	listElem* pl;
	uvsActInt* pd = NULL;
	uvsBunch *pm;
	int cq, mq;
	int priceQ;
	int color = -1;
	int raffa_on = 0;
	int dead_world = (Gamer -> Pworld -> escT[0] -> Pbunch -> status == UVS_BUNCH_STATUS::UNABLE);
	int m_count = 0;

#ifdef TOTAL_CASH
	aciUpdateCurCredits(799999);
#endif

	char* lm = new char[MAX_MECHOS_TYPE];
	char* li = new char[MAX_ITEM_TYPE];

	memset( lm, 0, MAX_MECHOS_TYPE);
	memset( li, 0, MAX_ITEM_TYPE);

	pb = NULL;
	pe = Pmechos;

	if (WorldTable[ CurrentWorld ] -> escTmax){
		pm  = WorldTable[ CurrentWorld ] -> escT[0] -> Pbunch;
		color = CurrentWorld;

		cq = pm -> cycleTable[ pm -> currentStage ].cirtQ;
		mq = pm -> cycleTable[ pm -> currentStage ].cirtMAX;
		priceQ = pm -> cycleTable[ pm -> currentStage ].priceQ;
	} else {
		pm = NULL;
		cq = mq = 1;
		priceQ = 100;
	}

	while( pe ){
		m_count++;
		if ( uvsMechosTable[((uvsMechos*)pe) -> type] -> type == UVS_CAR_TYPE::RAFFA )
			raffa_on = 1;
#ifdef _DEMO_
		if (CAR_DEMO[((uvsMechos*)pe) -> type])
#endif
		if ( (!lm[((uvsMechos*)pe) -> type]) || dead_world){
			 pn = new uvsActInt;

			 int first_constr = MAX_MECHOS_TYPE - MAX_MECHOS_CONSTRACTOR;

			 if (( ((uvsMechos*)pe) -> type) >= first_constr){
				int _type = ( ((uvsMechos*)pe) -> type) - first_constr;
				((uvsActInt*)pn) -> type =  first_constr + (_type<<2) + uvsMechosTable[( ((uvsMechos*)pe) -> type)] -> constractor;

				if (uvsMechosTable[( ((uvsMechos*)pe) -> type)] -> constractor == 3)
					((uvsActInt*)pn) -> price = uvsMechosTable[( ((uvsMechos*)pe) -> type)] -> price;
				else
					((uvsActInt*)pn) -> price = 1;
				 ((uvsActInt*)pn) -> sell_price = uvsMechosTable[( ((uvsMechos*)pe) -> type)] -> sell_price;
				 ((uvsActInt*)pn) -> param1 = color;
			} else {
				((uvsActInt*)pn) -> type = ( ((uvsMechos*)pe) -> type);

				 ((uvsActInt*)pn) -> price = uvsMechosTable[( ((uvsMechos*)pe) -> type)] -> price;
				 ((uvsActInt*)pn) -> sell_price = uvsMechosTable[( ((uvsMechos*)pe) -> type)] -> sell_price;
				 ((uvsActInt*)pn) -> param1 = color;
			}

			if (((uvsActInt*)pn) -> price != 1 && !NetworkON){
				 ((uvsActInt*)pn) -> price = (((uvsActInt*)pn) -> price*1.5*cq + ((uvsActInt*)pn) -> price*(mq-cq))/mq;
				 ((uvsActInt*)pn) -> sell_price = (((uvsActInt*)pn) -> sell_price*cq + ((uvsActInt*)pn) -> sell_price*0.5*(mq-cq))/mq;

				double t = ((double)(GamerResult.dominance+GamerResult.add_dominance))/100.0;

				if ( t > 1.0 ) t = 1.0;
				if ( t < -1.0 ) t = -1.0;

				 if ( t > 0.0 ){
					 double dt = ( ((uvsActInt*)pn) -> price - ((uvsActInt*)pn) -> sell_price)/2.0;
					((uvsActInt*)pn) -> price -= t*dt;
					((uvsActInt*)pn) -> sell_price +=  t*dt;
				 } else {
					 ((uvsActInt*)pn) -> price *= (1.0 - t);
					((uvsActInt*)pn) -> sell_price *= (1.0 + t);
				 }

				 if (((uvsActInt*)pn) -> sell_price > ((uvsActInt*)pn) -> price) ((uvsActInt*)pn) -> sell_price = ((uvsActInt*)pn) -> price;

				 if (dead_world)
					 ((uvsActInt*)pn) -> sell_price = ((uvsActInt*)pn) -> price = 0;

			}

//			 lm[((uvsMechos*)pe) -> type] = 1;
#ifdef _DEMO_
			 ((uvsActInt*)pn) -> param1 = CAR_COLOR[( ((uvsMechos*)pe) -> type)];
#endif
//			if (( ((uvsMechos*)pe) -> type) >= first_constr && dead_world){
//				delete pn;
//			} else
				pn -> link(pb);
		}

		 pl = pe;
		 pe = pe -> next;
		if ( !lm[((uvsMechos*)pl) -> type]){
			lm[((uvsMechos*)pl) -> type] = 1;
		
			((uvsMechos*)pl) -> delink(Pmechos);
			/*;if ( pl == Pmechos ) Pmechos = pe;
			if(pl -> prev -> next) pl -> prev -> next = pe;
			if(pe)
				pe -> prev = pl -> prev;*/

			delete ((uvsMechos*)pl);
		}
	}

	if (!raffa_on && where ){
		pn = new uvsActInt;
		int _type_ = MAX_MECHOS_MAIN + RND(MAX_MECHOS_RAFFA);
		((uvsActInt*)pn) -> type = _type_;

		((uvsActInt*)pn) -> price = uvsMechosTable[_type_] -> price;
		((uvsActInt*)pn) -> sell_price = uvsMechosTable[_type_] -> sell_price;
#ifndef _DEMO_
		((uvsActInt*)pn) -> param1 = color;
#else
		((uvsActInt*)pn) -> param1 = CAR_COLOR[((uvsActInt*)pn) -> type];
#endif
		
		if (!NetworkON){
			((uvsActInt*)pn) -> price = (((uvsActInt*)pn) -> price*1.5*cq + ((uvsActInt*)pn) -> price*(mq-cq))/mq;
			 ((uvsActInt*)pn) -> sell_price = (((uvsActInt*)pn) -> sell_price*cq + ((uvsActInt*)pn) -> sell_price*0.5*(mq-cq))/mq;

			 double t = ((double)(GamerResult.dominance+GamerResult.add_dominance))/100.0;

			 if ( t > 1.0 ) t = 1.0;
			 if ( t < -1.0 ) t = -1.0;

			 if ( t > 0.0){
				 double dt = (((uvsActInt*)pn) -> price - ((uvsActInt*)pn) ->sell_price)/2;
				((uvsActInt*)pn) -> price -= t*dt;
				((uvsActInt*)pn) -> sell_price +=  t*dt;
			 } else {
				 ((uvsActInt*)pn) -> price *= (1.0 - t);
				((uvsActInt*)pn) -> sell_price *= (1.0 + t);
			 }
		}//  end if

		 if (((uvsActInt*)pn) -> sell_price > ((uvsActInt*)pn) -> price) ((uvsActInt*)pn) -> sell_price = ((uvsActInt*)pn) -> price;

		 if (dead_world)
					 ((uvsActInt*)pn) -> sell_price = ((uvsActInt*)pn) -> price = 0;

		pn -> link(pb);
	}

	Mechos = (uvsActInt*)pb;
	pb = NULL;

	delete[] lm;

	pe = Pitem;
	uvsItem *ps;

	int _coef_ = RND(11);

	while( pe ){
		int item_on = 0;

#ifndef ALL_ITEM_IN_SHOP
		if(!NetworkON && ((uvsItem*)pe)->type==UVS_ITEM_TYPE::CIRTAINER){
			item_on = 1;
			switch(Gamer -> Pworld -> gIndex){
			case 0:
				//if (GamerResult.game_elr_result > 100)
				if (WorldTable[0] -> escT[0] -> Pbunch -> cycleTable[1].Pgame -> all > 1)
					item_on = 0;
				break;
			case 1:
				if ( GamerResult.game_ker_result >= 100)
					item_on = 0;
				if ( GamerResult.game_pip_result >= 100)
					item_on = 0;
				break;
			case 2:
				if ( GamerResult.game_zyk_result >= 100)
					item_on = 0;
				break;
			};
		}
#endif

//		if( !( (!uvsItemTable[ ((uvsItem*)pe) -> type ] -> size && uvsItemTable[ ((uvsItem*)pe) -> type ] -> type == UVS_ITEM_STATUS::WEAPON && (((uvsItem*)pe) -> type*2 > ConTimer.hour) && (ConTimer.day == 1)) ||
//			(uvsItemTable[ ((uvsItem*)pe) -> type ] -> type == UVS_ITEM_STATUS::DEVICE && (((uvsItem*)pe) -> type - UVS_ITEM_TYPE::COPTE_RIG + 1) > ConTimer.day) || item_on) ){
		if (!item_on){
			pn = new uvsActInt;
			((uvsActInt*)pn) -> type = ( ((uvsItem*)pe) -> ActIntType());
			((uvsActInt*)pn) -> price = Price[( ((uvsItem*)pe) -> type)];

			((uvsActInt*)pn) -> param1 = ((uvsItem*)pe) -> param1;
			((uvsActInt*)pn) -> param2 = ((uvsItem*)pe) -> param2;

			((uvsActInt*)pn) -> sell_price = Price[( ((uvsItem*)pe) -> type) + MAX_ITEM_TYPE];

			switch( uvsItemTable[( ((uvsItem*)pe) -> type)] -> type){
			case UVS_ITEM_STATUS::DEVICE:
			case UVS_ITEM_STATUS::WEAPON:
				if (!dead_world){
					((uvsActInt*)pn) -> param1 = uvsItemTable[ ((uvsItem*)pe) -> type] -> param1;
					((uvsActInt*)pn) -> param2 = uvsItemTable[ ((uvsItem*)pe) -> type] -> param2;
				}


				if ( ((uvsItem*)pe) -> type == UVS_ITEM_TYPE::TERMINATOR || ((uvsItem*)pe) -> type == UVS_ITEM_TYPE::TERMINATOR2){
					((uvsActInt*)pn) -> price *= ((uvsActInt*)pn) -> param2;
					((uvsActInt*)pn) -> sell_price *= ((uvsActInt*)pn) -> param2;
				}

				((uvsActInt*)pn) -> price = (((uvsActInt*)pn) -> price*1.5*cq + ((uvsActInt*)pn) -> price*(mq-cq))/mq;
				((uvsActInt*)pn) -> sell_price = (((uvsActInt*)pn) -> sell_price*cq + ((uvsActInt*)pn) -> sell_price*0.5*(mq-cq))/mq;
				break;
			case UVS_ITEM_STATUS::AMMO:
				((uvsActInt*)pn) -> param1 = uvsItemTable[ ((uvsItem*)pe) -> type] -> param1;
				((uvsActInt*)pn) -> param2 = uvsItemTable[ ((uvsItem*)pe) -> type] -> param2;

				((uvsActInt*)pn) -> price *= ((uvsActInt*)pn) -> param2;
				((uvsActInt*)pn) -> sell_price *= ((uvsActInt*)pn) -> param2;

				((uvsActInt*)pn) -> price = (((uvsActInt*)pn) -> price*1.5*cq + ((uvsActInt*)pn) -> price*(mq-cq))/mq;
				((uvsActInt*)pn) -> sell_price = (((uvsActInt*)pn) -> sell_price*cq + ((uvsActInt*)pn) -> sell_price*0.5*(mq-cq))/mq;
				break;
			case UVS_ITEM_STATUS::ARTIFACT:
				break;
			case UVS_ITEM_STATUS::GOODS:
				((uvsActInt*)pn) -> price = ((uvsActInt*)pn) -> price*( 100 + _coef_ - 5)/100;
				((uvsActInt*)pn) -> sell_price = ((uvsActInt*)pn) -> sell_price*( 100 + _coef_ - 5)/100;
				break;
			case UVS_ITEM_STATUS::THING:
			case UVS_ITEM_STATUS::TREASURE:
			case UVS_ITEM_STATUS::MECHOS_PART:
				break;
			};

			if (!NetworkON){
				double t = ((double)(GamerResult.dominance+GamerResult.add_dominance))/100.0;

				if ( t > 1.0 ) t = 1.0;
				if ( t < -1.0 ) t = -1.0;

				 if ( t > 0.0){
					 double dt = (((uvsActInt*)pn) -> price - ((uvsActInt*)pn) ->sell_price)/2;
					((uvsActInt*)pn) -> price -= t*dt;
					((uvsActInt*)pn) -> sell_price +=  t*dt;
				 } else {
					 ((uvsActInt*)pn) -> price *= (1.0 - t);
					((uvsActInt*)pn) -> sell_price *= (1.0 + t);
				 }

				if (((uvsActInt*)pn) -> sell_price == 0) ((uvsActInt*)pn) -> sell_price = 1;
				if (((uvsActInt*)pn) -> price == 0) ((uvsActInt*)pn) -> price = 1;

				 if (((uvsActInt*)pn) -> sell_price > ((uvsActInt*)pn) -> price) ((uvsActInt*)pn) -> sell_price = ((uvsActInt*)pn) -> price;

				 if (dead_world)
						 ((uvsActInt*)pn) -> sell_price = ((uvsActInt*)pn) -> price = 0;
			}//  end if

			 if ( uvsItemTable[(((uvsItem*)pe)->type)]->type != UVS_ITEM_STATUS::WEAPON || !li[((uvsItem*)pe) -> type] || dead_world ){
				 if (( ((uvsItem*)pe) -> type) != UVS_ITEM_TYPE::TERMINATOR2 && ( ((uvsItem*)pe) -> type) != UVS_ITEM_TYPE::TERMINATOR)
					li[((uvsItem*)pe) -> type] = 1;

				pn -> link(pb);

				ps = (uvsItem*)pe;
				pe = pe -> next;
				ps -> delink(Pitem);
				delete ps;
			 } else {
				 pe = pe -> next;
				 delete pn;
			 }
		}else {
			pe = pe -> next;
		}//  end if

	}//  end while

	if (pd) pd -> link(pb);
/*	FreeList( Pitem );
	Pitem = NULL;*/
	
	Item = (uvsActInt*)pb;

	if (where){
		pe = Gamer -> Pescave -> Pitem;
	} else {
		pe = Gamer -> Pspot -> Pitem;
	}

	if (GTreasureItem) ErrH.Abort("uvsShop::prepare_list_for_ActInt - unresolved treasure");

	pb = NULL;
	if(pe && !((uvsItem*)pe) -> param1 && (GamerResult.luck + GamerResult.add_luck > RND(100)) ){
		 pn = new uvsActInt;
		 ((uvsActInt*)pn) -> type = ( ((uvsItem*)pe) -> ActIntType());
		 ((uvsActInt*)pn) -> param1 = 0;
		 ((uvsActInt*)pn) -> param2 = ( ((uvsItem*)pe) -> param2);

		 ((uvsActInt*)pn) -> price = 1;
		 ((uvsActInt*)pn) -> sell_price = 1;

		 ((uvsItem*)pe) -> param1 = uvsTreasureInShop;

		 pn -> link(pb);
	}

//	if (GamerResult.rubbox_count < 2 && !where && (!strcmp(Gamer ->Pspot -> name, "Ogorod")) && (GamerResult.luck + GamerResult.add_luck > RND(30)) ){
	if ( ((GamerResult.rubbox_count < 1) || (GamerResult.rubbox_count < 2 && dgD -> bStatus >= 1)) && !where && (!strcmp(Gamer ->Pspot -> name, "Ogorod")) && (GamerResult.luck + GamerResult.add_luck > RND(30)) ){
		 pn = new uvsActInt;
		 ((uvsActInt*)pn) -> type = ACI_RUBOX;
		 ((uvsActInt*)pn) -> param1 = uvsItemTable[UVS_ITEM_TYPE::RUBBOX] -> param1;
		 ((uvsActInt*)pn) -> param2 = uvsItemTable[UVS_ITEM_TYPE::RUBBOX] -> param2;
		 ((uvsActInt*)pn) -> price = 1;
		 ((uvsActInt*)pn) -> sell_price = 1;

		 GamerResult.rubbox_count++;
		 pn -> link(pb);
	}

	GTreasureItem = (uvsActInt*)pb;
	delete[] li;
}

void uvsVanger::prepare_list_for_ActInt( uvsActInt*& Item, uvsActInt*& Mechos, uvsShop* ps, int where){
	listElem* pe;
	listElem* pb;
	listElem* pn;
	listElem *pg;
	uvsBunch *pm;
	int cq, mq;
	int priceQ;
	int _tabu_taskID_;
	int _tabu_taskCOUNT_;
	int dead_world;

	if (where){
		pg = Pescave -> Pgood;
		_tabu_taskID_ = Pescave -> TabuTaskID;
		_tabu_taskCOUNT_ = Pescave -> TabuTaskCount;
	} else {
		pg = Pspot -> Pgood;
		_tabu_taskID_ = Pspot -> TabuTaskID;
		_tabu_taskCOUNT_ = Pspot -> TabuTaskCount;
	}

	if (Pworld -> escTmax)
		dead_world = (Gamer -> Pworld -> escT[0] -> Pbunch -> status == UVS_BUNCH_STATUS::UNABLE);
	else {
		dead_world = 0;
		_tabu_taskID_ = -1;
		_tabu_taskCOUNT_ = 0;
	}

	if (Pworld -> escTmax){
		pm  = Pworld -> escT[0] -> Pbunch;

		cq = pm -> cycleTable[ pm -> currentStage ].cirtQ;
		mq = pm -> cycleTable[ pm -> currentStage ].cirtMAX;
		priceQ = pm -> cycleTable[ pm -> currentStage ].priceQ;
	} else {
		pm = NULL;
		cq = mq = 1;
		priceQ = 100;
	}
	pb = NULL;

	if ( Pmechos ){
		pn = new uvsActInt;
		int first_constr = MAX_MECHOS_TYPE - MAX_MECHOS_CONSTRACTOR;

		if (Pmechos -> type >= first_constr){
			int _type = Pmechos -> type - first_constr;
			((uvsActInt*)pn) -> type =  first_constr + (_type<<2) + uvsMechosTable[Pmechos -> type] -> constractor;

			if (uvsMechosTable[Pmechos -> type] -> constractor == 3)
				((uvsActInt*)pn) -> price = uvsMechosTable[Pmechos -> type] -> price;
			else
				((uvsActInt*)pn) -> price = 1;

			((uvsActInt*)pn) -> sell_price = uvsMechosTable[Pmechos -> type] -> sell_price;
			((uvsActInt*)pn) -> param1 = Pmechos -> color;
			((uvsActInt*)pn) -> param2 = Pmechos -> teleport;
		} else {
			((uvsActInt*)pn) -> type =  Pmechos -> type;
			((uvsActInt*)pn) -> price = uvsMechosTable[Pmechos -> type] -> price;
			((uvsActInt*)pn) -> sell_price = uvsMechosTable[Pmechos -> type] -> sell_price;
			((uvsActInt*)pn) -> param1 = Pmechos -> color;
			((uvsActInt*)pn) -> param2 = Pmechos -> teleport;
		}

		if (!NetworkON){
			if (WorldTable[ CurrentWorld ] -> escTmax){
				 ((uvsActInt*)pn) -> price = (((uvsActInt*)pn) -> price*1.5*cq + ((uvsActInt*)pn) -> price*(mq-cq))/mq;
				 ((uvsActInt*)pn) -> sell_price = (((uvsActInt*)pn) -> sell_price*cq + ((uvsActInt*)pn) -> sell_price*0.5*(mq-cq))/mq;
			}

			double t = ((double)(GamerResult.dominance+GamerResult.add_dominance))/100.0;

			if ( t > 1.0 ) t = 1.0;
			if ( t < -1.0 ) t = -1.0;

			 if ( t > 0.0){
				 double dt = (((uvsActInt*)pn) -> price - ((uvsActInt*)pn) ->sell_price)/2;
				((uvsActInt*)pn) -> price -= t*dt;
				((uvsActInt*)pn) -> sell_price +=  t*dt;
			 } else {
				 ((uvsActInt*)pn) -> price *= (1.0 - t);
				((uvsActInt*)pn) -> sell_price *= (1.0 + t);
			 }
		}//  end if

		 if (ChangeArmor != -1){
			 ((uvsActInt*)pn) -> sell_price = ((uvsActInt*)pn) -> sell_price*( 1.0 + (( (ChangeArmor>>16) - uvsMechosTable[Pmechos -> type] -> MaxArmor)/double(uvsMechosTable[Pmechos -> type] -> MaxArmor*2.0)));
		 }

		 if (((uvsActInt*)pn) -> sell_price > ((uvsActInt*)pn) -> price) ((uvsActInt*)pn) -> sell_price = ((uvsActInt*)pn) -> price;

		 if (dead_world)
			((uvsActInt*)pn) -> sell_price = ((uvsActInt*)pn) -> price = 0;

		pn -> link(pb);
	}

	Mechos = (uvsActInt*)pb;

	pb = NULL;
	pe = Pitem;

	while( pe ){
		 pn = new uvsActInt;
		 ((uvsActInt*)pn) -> type = ( ((uvsItem*)pe) -> ActIntType());
		 ((uvsActInt*)pn) -> param1 = ( ((uvsItem*)pe) -> param1);
		 ((uvsActInt*)pn) -> param2 = ( ((uvsItem*)pe) -> param2);
		 ((uvsActInt*)pn) -> pos_x = ( ((uvsItem*)pe) -> pos_x);
		 ((uvsActInt*)pn) -> pos_y = ( ((uvsItem*)pe) -> pos_y);

		 if (WorldTable[ CurrentWorld ] -> escTmax){
			 ((uvsActInt*)pn) -> price = ps -> Price[( ((uvsItem*)pe) -> type)];
			 ((uvsActInt*)pn) -> sell_price = ps -> Price[( ((uvsItem*)pe) -> type) + MAX_ITEM_TYPE];

			switch( uvsItemTable[( ((uvsItem*)pe) -> type)] -> type){
			case UVS_ITEM_STATUS::DEVICE:
			case UVS_ITEM_STATUS::WEAPON:
				if ( ((uvsItem*)pe) -> type == UVS_ITEM_TYPE::TERMINATOR || ((uvsItem*)pe) -> type == UVS_ITEM_TYPE::TERMINATOR2){
					((uvsActInt*)pn) -> price *= ((uvsActInt*)pn) -> param2;
					((uvsActInt*)pn) -> sell_price *= ((uvsActInt*)pn) -> param2;
				} else {

					if (uvsItemTable[( ((uvsItem*)pe) -> type)] -> param2 ){
						((uvsActInt*)pn) -> price -= (( uvsItemTable[((uvsItem*)pe) -> type] -> param2 - ((uvsActInt*)pn) -> param2)*(((uvsActInt*)pn) -> price))/(uvsItemTable[ ((uvsItem*)pe) -> type] -> param2*2);
						((uvsActInt*)pn) -> sell_price -= (( uvsItemTable[( ((uvsItem*)pe) -> type)] -> param2- ((uvsActInt*)pn) -> param2)*(((uvsActInt*)pn) -> sell_price))/(uvsItemTable[ ((uvsItem*)pe) -> type] -> param2*2);
					}
				}

				if (!NetworkON){
					((uvsActInt*)pn) -> price = (((uvsActInt*)pn) -> price*1.5*cq + ((uvsActInt*)pn) -> price*(mq-cq))/mq;
					((uvsActInt*)pn) -> sell_price = (((uvsActInt*)pn) -> sell_price*cq + ((uvsActInt*)pn) -> sell_price*0.5*(mq-cq))/mq;
				}
				break;
			case UVS_ITEM_STATUS::AMMO:
				((uvsActInt*)pn) -> price *= ((uvsActInt*)pn) -> param2;
				((uvsActInt*)pn) -> sell_price *= ((uvsActInt*)pn) -> param2;

				if (!NetworkON){
					((uvsActInt*)pn) -> price = (((uvsActInt*)pn) -> price*1.5*cq + ((uvsActInt*)pn) -> price*(mq-cq))/mq;
					((uvsActInt*)pn) -> sell_price = (((uvsActInt*)pn) -> sell_price*cq + ((uvsActInt*)pn) -> sell_price*0.5*(mq-cq))/mq;
				}
				break;
			case UVS_ITEM_STATUS::ARTIFACT:
				break;
			case UVS_ITEM_STATUS::GOODS:
				if ( ((uvsItem*)pe) -> type == UVS_ITEM_TYPE::ELEECH || ((uvsItem*)pe) -> type == UVS_ITEM_TYPE::WEEZYK){
					int coef = (((uvsItem*)pe) -> param1 & 0x0000FFFF) + 1;
					if (coef > 3) coef = 3;

					((uvsActInt*)pn) -> sell_price *= coef;
				} else	if (((uvsItem*)pe) -> type == UVS_ITEM_TYPE::TABUTASK){
					int _ID_ = ((uvsItem*)pe) -> param2 & 0x0000FFFF;
					((uvsActInt*)pn) -> sell_price = 0;

					if ( (_ID_ >= _tabu_taskID_) && (_ID_ < _tabu_taskID_ + _tabu_taskCOUNT_) && ( ((uvsActInt*)pn) -> type ==TABUTASK_GOOD) ){
						((uvsActInt*)pn) -> sell_price = TabuTable[_ID_] -> cash;
					}
				} else if ( ((uvsItem*)pe) -> type == UVS_ITEM_TYPE::ELEEPOD && !dgAbortStatus){
					if (_tabu_taskID_ == (((uvsItem*)pe) -> param2>>16))  {
						uvs_aciKillOneItem( ((uvsActInt*)pn) ->type, ((uvsActInt*)pn) -> pos_x, ((uvsActInt*)pn) -> pos_y);

						aciUpdateCurCredits(aciGetCurCredits() + 6000);
						delete (uvsActInt*)pn;
						pn = NULL;
					}
					break;
				} else if ( ((uvsItem*)pe) -> type == UVS_ITEM_TYPE::BEEBOORAT && !dgAbortStatus){
					if (_tabu_taskID_ == (((uvsItem*)pe) -> param2>>16)){
						uvs_aciKillOneItem( ((uvsActInt*)pn) ->type, ((uvsActInt*)pn) -> pos_x, ((uvsActInt*)pn) -> pos_y);

						aciUpdateCurCredits(aciGetCurCredits() + 2000);
						delete (uvsActInt*)pn;
						pn = NULL;
					}
					break;
				} else if (((uvsItem*)pe) -> type == UVS_ITEM_TYPE::ZEEX && !dgAbortStatus){
					if (_tabu_taskID_ == (((uvsItem*)pe) -> param2>>16)){
						uvs_aciKillOneItem( ((uvsActInt*)pn) ->type, ((uvsActInt*)pn) -> pos_x, ((uvsActInt*)pn) -> pos_y);

						aciUpdateCurCredits(aciGetCurCredits() + 5000);
						delete (uvsActInt*)pn;
						pn = NULL;
					}
					break;
				} else {
					uvsTradeItem* pl = (uvsTradeItem*)pg;
					while(pl){
						if (pl -> type == ((uvsItem*)pe) -> type){
							int coef = (((uvsItem*)pe) -> param1 & 0x0000FFFF);

							if (coef)
								((uvsActInt*)pn) -> sell_price /= (1<<(coef-1));
							else
								((uvsActInt*)pn) -> sell_price *= 2;
							break;
						}//  end if
						pl = (uvsTradeItem*)pl -> next;
					}//  end while

					if (!pl){
						int coef = ((uvsItem*)pe) -> GetCoef();

						if (coef < 0 ) ErrH.Abort("uvsItem :: dont have town");

						((uvsActInt*)pn) -> sell_price *= coef;
						((uvsActInt*)pn) -> sell_price /=2;
					} //  end  if

					if (((uvsItem*)pe) -> type == UVS_ITEM_TYPE::KERNOBOO || ((uvsItem*)pe) -> type == UVS_ITEM_TYPE::PIPETKA ){
						((uvsActInt*)pn) -> sell_price *= ((uvsItem*)pe) -> param2;
						((uvsActInt*)pn) -> price *= ((uvsItem*)pe) -> param2;
					}
//					((uvsActInt*)pn) -> price = ((uvsActInt*)pn) -> sell_price;
				}//  end if
				break;
			case UVS_ITEM_STATUS::THING:
			case UVS_ITEM_STATUS::TREASURE:
			case UVS_ITEM_STATUS::MECHOS_PART:
				break;
			};
		 } else {
			 ((uvsActInt*)pn) -> price = 1;
			 ((uvsActInt*)pn) -> sell_price = 1;
		 }

		 if (pn){
			 if (((uvsItem*)pe) -> type != UVS_ITEM_TYPE::TABUTASK && !NetworkON){
				 double t = ((double)(GamerResult.dominance+GamerResult.add_dominance))/100.0;

				if ( t > 1.0 ) t = 1.0;
				if ( t < -1.0 ) t = -1.0;

				 if ( t > 0.0){
					 double dt = (((uvsActInt*)pn) -> price - ((uvsActInt*)pn) ->sell_price)/2;
					((uvsActInt*)pn) -> price -= t*dt;
					((uvsActInt*)pn) -> sell_price +=  t*dt;
				 } else {
					 ((uvsActInt*)pn) -> price *= (1.0 - t);
					((uvsActInt*)pn) -> sell_price *= (1.0 + t);
				 }


				if (((uvsActInt*)pn) -> sell_price > ((uvsActInt*)pn) -> price) ((uvsActInt*)pn) -> sell_price = ((uvsActInt*)pn) -> price;
			 }

			if (((uvsActInt*)pn) -> sell_price == 0) ((uvsActInt*)pn) -> sell_price = 1;
			if (((uvsActInt*)pn) -> price == 0) ((uvsActInt*)pn) -> price = 1;

			if (((uvsActInt*)pn) -> type == ACI_DEAD_ELEECH ||
			    ((uvsActInt*)pn) -> type == ACI_ROTTEN_KERNOBOO ||
			    ((uvsActInt*)pn) -> type == ACI_ROTTEN_PIPETKA ||
			    ((uvsActInt*)pn) -> type == ACI_ROTTEN_WEEZYK ){
				((uvsActInt*)pn) -> sell_price = 0;
				((uvsActInt*)pn) -> price = 0;
			}

			if (dead_world)
				((uvsActInt*)pn) -> sell_price = ((uvsActInt*)pn) -> price = 0;

			 pn -> link(pb);
		 }
		pe = pe -> next;
	}

	Item = (uvsActInt*)pb;

	if (Pworld -> escTmax){
		if (where)
			aciUpdateCurCredits(aciGetCurCredits() + sellCirt()*400);
		uvsCheckItemOnTabuTask(Pitem, _tabu_taskID_, _tabu_taskCOUNT_);
		if (!dgAbortStatus)
			GamerResult.game_result =  game_result( aci_curLocationName );
		else
			GamerResult.game_result = 0;
	}

	if (Pmechos){
		delete Pmechos;
		Pmechos = NULL;
	}

	FreeList(Pitem);
	Pitem = NULL;

/*	if (where){
		pe = Pescave -> Pitem;
	} else {
		pe = Pspot -> Pitem;
	}

	if (GTreasureItem) ErrH.Abort("uvsVanger::prepare_list_for_ActInt - unresolved treasure");

	pb = NULL;
	if(pe && !((uvsItem*)pe) -> param1 && (GamerResult.luck + GamerResult.add_luck > RND(100)) ){
		 pn = new uvsActInt;
		 ((uvsActInt*)pn) -> type = ( ((uvsItem*)pe) -> ActIntType());
		 ((uvsActInt*)pn) -> param1 = 0;
		 ((uvsActInt*)pn) -> param2 = ( ((uvsItem*)pe) -> param2);

		 ((uvsActInt*)pn) -> price = 1;
		 ((uvsActInt*)pn) -> sell_price = 1;

		 ((uvsItem*)pe) -> param1 = uvsTreasureInShop;

		 pn -> link(pb);
	}

	if (!where && (!strcmp(Pspot -> name, "Ogorod")) && (GamerResult.luck + GamerResult.add_luck > RND(30)) ){
		 pn = new uvsActInt;
		 ((uvsActInt*)pn) -> type = ACI_RUBOX;
		 ((uvsActInt*)pn) -> param1 = uvsItemTable[UVS_ITEM_TYPE::RUBBOX] -> param1;
		 ((uvsActInt*)pn) -> param2 = uvsItemTable[UVS_ITEM_TYPE::RUBBOX] -> param2;
		 ((uvsActInt*)pn) -> price = 1;
		 ((uvsActInt*)pn) -> sell_price = 1;
		 pn -> link(pb);
	}

	GTreasureItem = (uvsActInt*)pb;*/
}

void uvsShop::sellMechos(uvsMechos*& Pm, int type){
	int last_type = type;
	int first_constr = MAX_MECHOS_TYPE - MAX_MECHOS_CONSTRACTOR;
	if ( !Pmechos ) return;

	if (!type) type = ~0;

	if (Pm) addMechos( Pm );

	Pm = (uvsMechos*)Pmechos;

	while( Pm && ((Pm -> type >= first_constr) || ((Pm -> status & type) == 0))){
		Pm = (uvsMechos*)Pm -> next;
	}

	if (last_type){
		uvsMechos*_pm_ = Pm;
		while( Pm ){
			if ((uvsMechosTable[Pm -> type]->price > uvsMechosTable[_pm_ -> type]->price) &&  (Pm -> type < first_constr))
				_pm_ = Pm;
			Pm = (uvsMechos*)Pm -> next;
		}
		Pm = _pm_;
	} else {
		char *MechosHere = new char[first_constr];
		memset(MechosHere, 0, first_constr);

		uvsMechos*_pm_ = (uvsMechos*)Pmechos;
		while( _pm_ ){
			if (_pm_ -> type < first_constr)
				MechosHere[_pm_ -> type] = 1;
			_pm_ = (uvsMechos*)_pm_ -> next;
		}//  end while

		int _i_ = RND(first_constr);
		while(!MechosHere[_i_])
			_i_ = RND(first_constr);

		_pm_ = (uvsMechos*)Pmechos;
		while( _pm_ ){
			if (_pm_ -> type == _i_) break;
			_pm_ = (uvsMechos*)_pm_ -> next;
		}//  end while

		delete[] MechosHere;
		Pm = _pm_;
	}

	if(Pm && (Pm -> type < first_constr)) {
		Pm -> delink(Pmechos);
		Pm -> sort();
/*		if (((listElem*)Pm) == Pmechos ){
			Pmechos = Pmechos -> next;
			if (Pmechos)
				Pmechos -> prev = Pm -> prev;
		} else {
			if (Pm -> next)
				Pm -> next -> prev = Pm -> prev;
			else
				Pmechos -> prev = Pm -> prev;
			Pm -> prev -> next = Pm -> next;
		}*/
	} else {
		Pm = (uvsMechos*)Pmechos;

		if (Pm -> type >= first_constr){
			Pm = new uvsMechos(0);
			Pm -> sort();
			return;
		} else {
			Pm -> delink(Pmechos);
			Pm -> sort();
			return;
		}

/*		Pmechos = Pmechos -> next;
		if (Pmechos)
			Pmechos -> prev = Pm -> prev;*/
		return;
	}
}

/*void uvsShop::sellItem(uvsItem*& Pi, int& cash){
	if ( !Pitem ) return;

	//if (Pi) additem( Pi );

	Pi = (uvsItem*)Pitem;
	Pitem = Pitem -> next;
	if(Pitem) {
		Pitem -> prev = (listElem*)Pi -> prev;
	}
} */

uvsPassage::uvsPassage(PrmFile* pfile,char* atom){
	name = new char[strlen(atom)+1];
	strcpy(name, atom);

	Pworld = (uvsWorld*)WorldTail -> seekName(pfile -> getAtom());
	Poutput = (uvsWorld*)WorldTail -> seekName(pfile -> getAtom());

	pos_x = atoi(pfile -> getAtom());
	pos_y = atoi(pfile -> getAtom());

	if(!Pworld || !Poutput) ErrH.Abort(PrmWrongValue,XERR_USER,-1,"World Name");
	if(pos_x < 0 || pos_x >= Pworld -> x_size) ErrH.Abort(PrmWrongValue,XERR_USER,-1,"pos_x");
	if(pos_y < 0 || pos_y >= Pworld -> y_size) ErrH.Abort(PrmWrongValue,XERR_USER,-1,"pos_y");

	Pworld -> pssTmax++;
	locked = 0;
	//world_locked = 0;
}

uvsPassage::~uvsPassage(void){
	if (name) {
		delete[] name;
	}
}

uvsPassage::uvsPassage(XStream& pfile){
	char *s;
	name = get_string(pfile);
	s = get_string(pfile);

	Pworld = (uvsWorld*)WorldTail -> seekName(s);
	delete[] s;

	s = get_string(pfile);
	Poutput = (uvsWorld*)WorldTail -> seekName(s);
	delete[] s;

	pfile > pos_x;
	pfile > pos_y;

	if(!Pworld || !Poutput) ErrH.Abort(PrmWrongValue,XERR_USER,-1,"World Name");
	if(pos_x < 0 || pos_x >= Pworld -> x_size) ErrH.Abort(PrmWrongValue,XERR_USER,-1,"pos_x");
	if(pos_y < 0 || pos_y >= Pworld -> y_size) ErrH.Abort(PrmWrongValue,XERR_USER,-1,"pos_y");

	Pworld -> pssTmax++;
	pfile > locked;
	//world_locked = 0;
}

void uvsPassage::save(XStream& pfile){
	put_string(pfile, name);
	put_string(pfile, Pworld -> name);
	put_string(pfile, Poutput -> name);

	pfile < pos_x;
	pfile < pos_y;

	pfile < locked;
}

uvsCultGame::uvsCultGame(PrmFile* pfile,char* atom){
	int i;
	char* s;
	listElem* ps;

	for(i = 0; i < GAME_MAX; i++) if (!strcmp(atom, PrmGameName[i])) break;
	if ( i == GAME_MAX ) ErrH.Abort(PrmWrongValue,XERR_USER,-1,"Game Name");
	GameType = i;

	switch(GameType){
	case UVS_GAME_TYPE::RACE:
		s = pfile -> getAtom();
		ps = (uvsEscave*)EscaveTail -> seekName(s);
		if ( !ps )	{
			ps = (uvsSpot*)SpotTail -> seekName(s);
			if ( !ps ) ErrH.Abort(PrmWrongValue,XERR_USER,-1,"Town");
			townBeg = ps;
			typeTownBeg = UVS_TOWN::SPOT;
		} else {
			townBeg = ps;
			typeTownBeg  = UVS_TOWN::ESCAVE;
		}

		s = pfile -> getAtom();
		i = GetItem_as_name( s );
		if ( i == -1 )
			ErrH.Abort(PrmWrongValue,XERR_USER,-1,"GOODS Name");
		GoodsTypeBeg = i;

		GoodsTypeBegCount = atoi(pfile -> getAtom());

		s = pfile -> getAtom();
		ps = (uvsEscave*)EscaveTail -> seekName(s);
		if ( !ps )	{
			ps = (uvsSpot*)SpotTail -> seekName(s);
			if ( !ps ) ErrH.Abort(PrmWrongValue,XERR_USER,-1,"Town");
			townEnd = (listElem*)ps;
			typeTownEnd = UVS_TOWN::SPOT;
		} else {
			townEnd = (listElem*)ps;
			typeTownEnd  = UVS_TOWN::ESCAVE;
		}

		s = pfile -> getAtom();
		i = GetItem_as_name( s );
		if ( i == -1 )
			ErrH.Abort(PrmWrongValue,XERR_USER,-1,"GOODS Name");
		GoodsTypeEnd = i;

		GoodsTypeEndCount = atoi(pfile -> getAtom());
		result = 0;
		if (GoodsTypeBegCount) GoodsBegCount = GoodsTypeBegCount; else GoodsBegCount = V_RACE_VANGER;
		GoodsEndCount = 0;

		s = pfile -> getAtom();
		i = GetItem_as_name( s );
		//if ( i == -1 )
		//	ErrH.Abort(PrmWrongValue,XERR_USER,-1,"GOODS Name");
		GoodsTypeLast = i;

		total = start = flag = first = all = sucks = 0;
		score = NULL;
		break;
	case UVS_GAME_TYPE::HARVEST:
		townBeg = NULL;
		typeTownBeg = -1;

		s = pfile -> getAtom();
		i = GetItem_as_name( s );
		if ( i == -1 )
			ErrH.Abort(PrmWrongValue,XERR_USER,-1,"GOODS Name");
		GoodsTypeBeg = i;

		GoodsTypeBegCount = atoi(pfile -> getAtom());

		s = pfile -> getAtom();
		ps = (uvsEscave*)EscaveTail -> seekName(s);
		if ( !ps )	{
			ps = (uvsSpot*)SpotTail -> seekName(s);
			if ( !ps ) ErrH.Abort(PrmWrongValue,XERR_USER,-1,"Town");
			townEnd = (listElem*)ps;
			typeTownEnd = UVS_TOWN::SPOT;
		} else {
			townEnd = (listElem*)ps;
			typeTownEnd  = UVS_TOWN::ESCAVE;
		}

		GoodsTypeEnd = -1;

		s = pfile -> getAtom();
		i = GetItem_as_name( s );
		GoodsTypeLast = i;

		score = new uchar[V_MAX_VANGER];
		memset(score, 0, V_MAX_VANGER);

		total = start = flag = first = all = sucks = 0;
		break;
	}
}

uvsBunch::uvsBunch(PrmFile* pfile,char* atom){
	status = 0;
	Pescave = (uvsEscave*)EscaveTail -> seekName(atom);
	Pescave -> Pbunch = this;

	char* s = pfile -> getAtom();
	int i;
	for(i = 0;i < BIOS_MAX;i++) if(!strcasecmp(s,BIOS_NAMES[i])) break;
	if(i == BIOS_MAX) ErrH.Abort(PrmWrongValue,XERR_USER,-1,"Bios Name");
	biosNindex = i;

	cycleN = atoi(pfile -> getAtom());
	if(cycleN <= 0) ErrH.Abort(PrmWrongValue,XERR_USER,-1,"cycleN");

	cycleTable = new uvsCultStage[cycleN];
	for(i = 0;i < cycleN;i++){
		s = pfile -> getAtom();
		cycleTable[i].name = new char[strlen(s)+1];
		strcpy(cycleTable[i].name, s);

		cycleTable[i].cirtMAX = atoi(pfile -> getAtom());
		cycleTable[i].time = atoi(pfile -> getAtom())*60;
		cycleTable[i].priceQ = atoi(pfile -> getAtom());
		cycleTable[i].GamerCirt = 0;

		if(cycleTable[i].cirtMAX <= 0) ErrH.Abort(PrmWrongValue,XERR_USER,-1,"cirtQ");
		cycleTable[i].cirtQ = 0;

		s = pfile -> getAtom();
		cycleTable[i].pal_name = new char[strlen(s)+1];
		strcpy(cycleTable[i].pal_name, s);

		s = pfile -> getAtom();
		if ( strcmp(s, PrmNoneGame) ){
			cycleTable[i].Pgame = new uvsCultGame(pfile, s);

			//ErrH.Abort(PrmWrongValue,XERR_USER,-1,"Game");
		} else
			cycleTable[i].Pgame = NULL;
	}

	currentStage = RND(cycleN);

	for(i = 0;i < cycleN;i++){
		int out  = 0;
		while( !out ){
			out = 1;
			cycleTable[i].Pdolly = WorldTable[getRW(Pescave -> Pworld -> gIndex)] -> generateDolly(biosNindex);
			if ( cycleTable[i].Pdolly == NULL  ) out = 0;
		}//  end while
	}//  end for i

	randVRes();

	if ( cycleTable[currentStage].Pgame ) begin_game();
}

uvsBunch::~uvsBunch(void){
	int i;
	if ( cycleN ){
		for( i = 0; i < cycleN; i++){
			cycleTable[i].free();
		}
		delete[] cycleTable;
	}
}

void uvsCultStage::free(void){
	if (name) {
		delete[] name;
	}
	name = NULL;
	if (pal_name) {
		delete[] pal_name;
	}
	pal_name = NULL;
	if( Pgame ) {
		delete Pgame;
	}
	Pgame = NULL;
	Pdolly = NULL;
}

uvsBunch::uvsBunch(XStream& pf, PrmFile* pfile,char* atom){
	Pescave = (uvsEscave*)EscaveTail -> seekName(atom);
	Pescave -> Pbunch = this;

	char* s = pfile -> getAtom();
	int i, j;

	for(i = 0;i < BIOS_MAX;i++) if(!strcasecmp(s,BIOS_NAMES[i])) break;
	if(i == BIOS_MAX) ErrH.Abort(PrmWrongValue,XERR_USER,-1,"Bios Name");
	biosNindex = i;

	cycleN = atoi(pfile -> getAtom());
	if(cycleN <= 0) ErrH.Abort(PrmWrongValue,XERR_USER,-1,"cycleN");

	cycleTable = new uvsCultStage[cycleN];
	for(i = 0;i < cycleN;i++){
		s = pfile -> getAtom();
		cycleTable[i].name = new char[strlen(s)+1];
		strcpy(cycleTable[i].name, s);

		cycleTable[i].cirtMAX = atoi(pfile -> getAtom());
		cycleTable[i].time = atoi(pfile -> getAtom())*60;
		cycleTable[i].priceQ = atoi(pfile -> getAtom());
		if(cycleTable[i].cirtMAX <= 0) ErrH.Abort(PrmWrongValue,XERR_USER,-1,"cirtQ");
		//cycleTable[i].cirtQ = 0;

		s = pfile -> getAtom();
		cycleTable[i].pal_name = new char[strlen(s)+1];
		strcpy(cycleTable[i].pal_name, s);

		s = pfile -> getAtom();
		if ( strcmp(s, PrmNoneGame) ){
			cycleTable[i].Pgame = new uvsCultGame(pfile, s);

			//ErrH.Abort(PrmWrongValue,XERR_USER,-1,"Game");
		} else
			cycleTable[i].Pgame = NULL;
	}

	for(i = 0;i < cycleN;i++){
		pf > cycleTable[i].cirtQ;
		pf > cycleTable[i].GamerCirt;

		if (uvsLoadVersion < 5)
			cycleTable[i].GamerCirt = 4;

		if ( cycleTable[i].Pgame ){
			pf > cycleTable[i].Pgame -> GoodsBegCount;
			pf > cycleTable[i].Pgame -> GoodsEndCount;
			pf > cycleTable[i].Pgame -> result;
			pf > cycleTable[i].Pgame -> total;
			pf > cycleTable[i].Pgame -> flag;
			pf > cycleTable[i].Pgame -> first;
			pf > cycleTable[i].Pgame -> all;
			pf > cycleTable[i].Pgame -> sucks;
			pf > cycleTable[i].Pgame -> start;

			if ( cycleTable[i].Pgame -> GameType == UVS_GAME_TYPE::HARVEST ){
				for(j = 0; j < cycleTable[i].Pgame -> total; j++)
					pf > cycleTable[i].Pgame -> score[j];
			}
		}

		int cd;
		pf > cd;
//		cd = biosNindex*3 + i + 1;
		uvsDolly* pd = (uvsDolly*)DollyTail;

		while(pd){
			if(pd -> gIndex == cd ) break;
			pd = (uvsDolly*)pd -> next;
			}
		if(!pd)
			ErrH.Abort(PrmWrongValue,XERR_USER,-1,"Dolly");
		cycleTable[i].Pdolly = pd;
	}

	pf > currentStage;
	pf > vngResource;
	pf > status;
	randVRes();
}

void uvsBunch::save(XStream& pfile){
	int i, j;
	for(i = 0;i < cycleN;i++){
		pfile < cycleTable[i].cirtQ;
		pfile < cycleTable[i].GamerCirt;

		if ( cycleTable[i].Pgame ){
			 pfile < cycleTable[i].Pgame -> GoodsBegCount;
			 pfile < cycleTable[i].Pgame -> GoodsEndCount;
			 pfile < cycleTable[i].Pgame -> result;
			 pfile < cycleTable[i].Pgame -> total;
			 pfile < cycleTable[i].Pgame -> flag;
			 pfile < cycleTable[i].Pgame -> first;
			 pfile < cycleTable[i].Pgame -> all;
			 pfile < cycleTable[i].Pgame -> sucks;
			 pfile < cycleTable[i].Pgame -> start;

			 if ( cycleTable[i].Pgame -> GameType == UVS_GAME_TYPE::HARVEST ){
				for(j = 0; j < cycleTable[i].Pgame -> total; j++)
					pfile < cycleTable[i].Pgame -> score[j];
			}
		}
		pfile < cycleTable[i].Pdolly -> gIndex;
	}

	pfile < currentStage;
	pfile <  vngResource;
	pfile <  status;
}

void uvsWorld::updateResource(void){
	int i;

	if ( !escTmax ) return;

	if (gIndex == 1){
		if (((uvsItem*)escT[0] -> Pitem) -> param1 == uvsTreasureInWait){
			((uvsItem*)escT[0] -> Pitem) -> param1 = 0;
			GamerResult.BoorawchickGoHimself = 1;
		}//  end if
	}//  end if

	for(i = 0; i < escTmax; i++) {
		escT[i] -> Pshop -> updateResource();

		if (escT[i] -> PassagerWait)
			escT[i] -> PassagerWait++;

		if (escT[i] -> PassagerWait > 1 && !RND(5))
			escT[i] -> PassagerWait = 0;

		if (escT[i] -> PassagerWait > 2 && !RND(3))
			escT[i] -> PassagerWait = 0;

		if (escT[i] -> PassagerWait > 3 )
			escT[i] -> PassagerWait = 0;
	}

	for(i = 0; i < sptTmax; i++) {
		sptT[i] -> Pshop -> updateResource();

		if (sptT[i] -> PassagerWait)
			sptT[i] -> PassagerWait++;

		if (sptT[i] -> PassagerWait > 1 && !RND(5))
			sptT[i] -> PassagerWait = 0;

		if (sptT[i] -> PassagerWait > 2 && !RND(3))
			sptT[i] -> PassagerWait = 0;

		if (sptT[i] -> PassagerWait > 3 )
			sptT[i] -> PassagerWait = 0;
	}
}

void uvsWorld::ChangeGoodsInList(int world){
	listElem *pi = Pitem;

	while( pi ){
		uvsChangeGoodsParam(((uvsItem*)pi) -> type, ((uvsItem*)pi) -> param1, ((uvsItem*)pi) -> param2, world);
		pi = pi -> next;
	}
}

void uvsWorld::ChangeTabuTaskInList(int type, int status){
	listElem *pi = Pitem;

	while( pi ){
		if (((uvsItem*)pi) -> type == UVS_ITEM_TYPE::TABUTASK && ( ((uvsItem*)pi) -> param2 == type)){
			if (status == UVS_TABUTASK_STATUS::BAD){
				GamerResult.luck -= TabuTable[ ((uvsItem*)pi) -> param2 & 0x0000FFFF] -> luck;
				if (GamerResult.luck < 0) GamerResult.luck = 0;
			} else {
				GamerResult.luck += TabuTable[ ((uvsItem*)pi) -> param2 & 0x0000FFFF] -> luck;
				if (GamerResult.luck > 100) GamerResult.luck = 100;
			}
			((uvsItem*)pi) -> param2 |= (status <<16);
		}
		pi = pi -> next;
	}//  end while
}

void uvsShop::updateResource(void){
	int i, j;
	uvsItem* pi;


	for( i = UVS_ITEM_TYPE::MACHOTINE_GUN_LIGHT; i <= UVS_ITEM_TYPE::CRUSTEST_CANNON; i++){
		if (  GetItem( Pitem, i, 0) == NULL  && !RND(3)  && ItemHere[i]) {
			addItem(pi = new uvsItem(i));
		}
	}
	for( i = UVS_ITEM_TYPE::TERMINATOR; i <= UVS_ITEM_TYPE::TERMINATOR2; i++){
		if (  GetItem( Pitem, i, 0) == NULL  && !RND(3) && ItemHere[i]) {
			addItem(pi = new uvsItem(i));
		}
	}

/*	for( i = UVS_ITEM_TYPE::AMPUTATOR; i <= UVS_ITEM_TYPE::DEGRADATOR; i++){
		if (  GetItem( Pitem, i, 0) == NULL  && !RND(3) && ItemHere[i]) {
			addItem(pi = new uvsItem(i));
		}
	}*/

	for( i = UVS_ITEM_TYPE::SPEETLE_SYSTEM_AMMO0; i <= UVS_ITEM_TYPE::GLUEK; i++){
#ifdef _DEMO_
		if (i != UVS_ITEM_TYPE::CRUSTEST_CANNON_AMMO)
#endif
		if (  GetItem( Pitem, i, 0) == NULL )
			for( j = 0; j < 4 + RND(10); j++ ) {
				addItem(pi = new uvsItem(i));
				//pi -> param2 = uvsItemTable[pi -> type] -> count;
			}
	}

	if (  GetItem( Pitem, UVS_ITEM_TYPE::CIRTAINER, 0) == NULL ) addItem(new uvsItem(UVS_ITEM_TYPE::CIRTAINER));
}

void uvsBunch::QuantCirt(int counter){
	int i;
	for(i = 0; i < cycleN; i++){
		if( i != currentStage && (counter%cycleTable[i].time == 0)){
			cycleTable[i].cirtQ >>= 1;
		}
	}
}

void uvsEscave::QuantChange(int counter){
	listElem *pm = Pshop -> Pmechos;

	while( pm ){
		if ( ((uvsMechos*)pm) -> type <= ((MAX_MECHOS_MAIN)/2 + counter) ) ((uvsMechos*)pm) -> status |= UVS_MECHOS_USED::USES;
		pm = pm -> next;
	}
}

void uvsBunch::begin_game( void ){
	uvsCultGame *pg = cycleTable[currentStage].Pgame;

	switch( pg -> GameType ){
	case UVS_GAME_TYPE::RACE:
		begin_race();
		break;
	case UVS_GAME_TYPE::HARVEST:
		begin_harvest();
		break;
	}
}

int uvsBunch::addGOODS_any(uvsCultGame* pg){
	uvsElement* e = ETail;
	while(e){
		if( ((uvsVanger*)e) -> type == UVS_OBJECT::VANGER && (((uvsVanger*)e) -> status == UVS_VANGER_STATUS::ESCAVE_WAITING ||
			((uvsVanger*)e) -> status == UVS_VANGER_STATUS::SPOT_WAITING)) {
#ifdef _ROAD_
			if ( ((uvsVanger*)e) -> shape != UVS_VANGER_SHAPE::GAMER ){
#endif
			if ( ((uvsVanger*)e) -> addGOODS( pg ) ) return 1;
#ifdef _ROAD_
		}
#endif
		}//  end if
		e = e -> enext;
	}//  end while
	return 0;
}

void uvsBunch::begin_race( void ){
	uvsCultGame *pg = cycleTable[currentStage].Pgame;
	char *name;

	if ( pg -> typeTownBeg == UVS_TOWN::ESCAVE )
		name  = ((uvsEscave*)(pg -> townBeg)) -> name;
	else
		name = ((uvsSpot*)(pg -> townBeg)) -> name;


#ifdef STAND_REPORT
	stand < ConTimer.GetTime();
	stand < "\nRace begin in " < name < "\n\n";
#endif

	pg -> GoodsBegCount = pg -> GoodsTypeBegCount;
	pg -> GoodsEndCount = pg -> GoodsTypeEndCount;
	pg -> result = 0;
	pg -> start = 1;
	pg -> total = 0;
	pg -> flag = 0;

	if (!pg -> GoodsTypeBegCount)
		pg -> GoodsBegCount = V_RACE_VANGER;
	 else
		pg -> GoodsBegCount = pg -> GoodsTypeBegCount;

	if ((!pg -> GoodsTypeBegCount) && (!strcmp( aci_curLocationName,  name) && Gamer -> status == UVS_VANGER_STATUS::IN_SHOP) ) {
		uvsEndHarvest = 1;

		if(GCharItem) delete GCharItem;

		if ( pg -> typeTownBeg == UVS_TOWN::ESCAVE){
			GCharItem = new uvsActInt;
			GCharItem -> type = uvsSetItemType(pg -> GoodsTypeBeg, 0, 0);
			GCharItem -> param1 = Pescave -> Pworld -> gIndex<<16;
			GCharItem -> price = ((uvsEscave*)(pg -> townBeg)) -> Pshop -> Price[pg -> GoodsTypeBeg];
			GCharItem -> sell_price = ((uvsEscave*)(pg -> townBeg)) -> Pshop -> Price[pg -> GoodsTypeBeg + MAX_ITEM_TYPE];
//			((uvsEscave*)(pg -> townBeg)) -> Pshop -> addItem( new uvsItem(pg -> GoodsTypeBeg));
		} else {
			GCharItem = new uvsActInt;
			GCharItem -> type = uvsSetItemType(pg -> GoodsTypeBeg, 0, 0);
			GCharItem -> param1 = Pescave -> Pworld -> gIndex<<16;
			GCharItem -> price = ((uvsSpot*)(pg -> townBeg)) -> Pshop -> Price[pg -> GoodsTypeBeg];
			GCharItem -> sell_price = ((uvsSpot*)(pg -> townBeg)) -> Pshop -> Price[pg -> GoodsTypeBeg + MAX_ITEM_TYPE];
//			((uvsSpot*)(pg -> townBeg)) -> Pshop -> addItem( new uvsItem(pg -> GoodsTypeBeg));
		}
	}	else if ((!strcmp( aci_curLocationName,  name)) && Gamer -> status == UVS_VANGER_STATUS::IN_SHOP) {
		uvsEndHarvest = 1;

		if(GCharItem) delete GCharItem;
		GCharItem = NULL;
		GamerResult.get_weezyk = 1;
		pg -> flag = 1;

		if ( (GamerResult.luck + GamerResult.add_luck > RND(100)) && pg -> GoodsBegCount){
			pg -> GoodsBegCount--;
			GamerResult.get_weezyk = 2;
			pg -> flag = 2;

			pg -> start = 0;
			if ( pg -> typeTownBeg == UVS_TOWN::ESCAVE){
				GCharItem = new uvsActInt;
				GCharItem -> type = uvsSetItemType(pg -> GoodsTypeBeg, 0, 0);
				GCharItem -> param1 = Pescave -> Pworld -> gIndex<<16;
				GCharItem -> price = ((uvsEscave*)(pg -> townBeg)) -> Pshop -> Price[pg -> GoodsTypeBeg];
				GCharItem -> sell_price = ((uvsEscave*)(pg -> townBeg)) -> Pshop -> Price[pg -> GoodsTypeBeg + MAX_ITEM_TYPE];
	//			((uvsEscave*)(pg -> townBeg)) -> Pshop -> addItem( new uvsItem(pg -> GoodsTypeBeg));
			} else {
				GCharItem = new uvsActInt;
				GCharItem -> type = uvsSetItemType(pg -> GoodsTypeBeg, 0, 0);
				GCharItem -> param1 = Pescave -> Pworld -> gIndex<<16;
				GCharItem -> price = ((uvsSpot*)(pg -> townBeg)) -> Pshop -> Price[pg -> GoodsTypeBeg];
				GCharItem -> sell_price = ((uvsSpot*)(pg -> townBeg)) -> Pshop -> Price[pg -> GoodsTypeBeg + MAX_ITEM_TYPE];
	//			((uvsSpot*)(pg -> townBeg)) -> Pshop -> addItem( new uvsItem(pg -> GoodsTypeBeg));
			}
		} else
			addGOODS_any(pg);
	}

	uvsVanger* v = FindFreeVanger();
	if (v){
		v -> shape = UVS_VANGER_SHAPE::BUNCH_SLAVE;

		if ( pg -> typeTownBeg == UVS_TOWN::ESCAVE ){
			v -> Pescave =	((uvsEscave*)(pg -> townBeg));
			v -> Pspot = NULL;
			v -> Pworld = v -> Pescave -> Pworld;
			v -> status = UVS_VANGER_STATUS::ESCAVE_WAITING;
		}else{
			v -> Pescave = NULL;
			v -> Pspot =  ((uvsSpot*)(pg -> townBeg));
			v -> Pworld = v -> Pspot -> Pworld;
			v -> status = UVS_VANGER_STATUS::SPOT_WAITING;
		}

		v -> add_to_game(pg);
		v -> locTimer = RACE_WAIT + (RACE_WAIT*(GamerResult.dominance+GamerResult.add_dominance))/100;
	}

	uvsElement* e = ETail;
	while(e){
		if( ((uvsVanger*)e) -> type == UVS_OBJECT::VANGER && (((uvsVanger*)e) -> status == UVS_VANGER_STATUS::ESCAVE_WAITING ||
			((uvsVanger*)e) -> status == UVS_VANGER_STATUS::SPOT_WAITING)) {
#ifdef _ROAD_
			if ( ((uvsVanger*)e) -> shape != UVS_VANGER_SHAPE::GAMER ){
#endif
			if (((uvsVanger*)e) -> add_to_game( pg ) )
				((uvsVanger*)e) -> locTimer = RACE_WAIT +  (RACE_WAIT*(GamerResult.dominance+GamerResult.add_dominance))/100;
			else if (((uvsVanger*)e) -> status == UVS_VANGER_STATUS::SPOT_WAITING)
				((uvsVanger*)e) -> status = UVS_VANGER_STATUS::SPOT_SLEEPING;
			else
				((uvsVanger*)e) -> status = UVS_VANGER_STATUS::ESCAVE_SLEEPING;
#ifdef _ROAD_
		}
#endif
		}
		e = e -> enext;
	}
}

void uvsBunch::begin_harvest( void ){
	int i;
	uvsCultGame *pg = cycleTable[currentStage].Pgame;
	uvsWorld* pw = Pescave -> Pworld;

#ifdef STAND_REPORT
	stand < ConTimer.GetTime();
	stand < "\nHarvest begin on " < pw -> name < "\n\n";
#endif

	if (uvsGamerWaitGame && !uvsEndHarvest) {
		uvsEndHarvest = 1;
	}
	
	pg -> GoodsBegCount = 0;		//  количество принимаемого товара в городе1
	pg -> GoodsEndCount = 0;		//  количество принимаемого товара в городе2
	pg -> result = 0;				//  текущие результат
	pg -> total = 0;				//  количество участников

	if (!pg -> score ) pg -> score = new uchar[V_MAX_VANGER];

	pg -> score[pg -> total++] = 3 + RND(5);

	//if ( pw -> gIndex == CurrentWorld ) return;

	// вызвать процедуру KRONa

	if (!NetworkON){ //znfo farmer adding
		for( i = 0; i < pg -> GoodsTypeBegCount/* + RND(3)*/; i++){
			if ( (pw -> gIndex == CurrentWorld) && uvsKronActive )
				addFarmer(RND(pw->x_size), RND(pw->y_size), V_SPEED_DOLLY - RND(V_SPEED_DOLLY*2+1),
							V_SPEED_DOLLY - RND(V_SPEED_DOLLY*2+1),pg -> GoodsTypeBeg,
							FLY_FARMER_CORN, 0);
			else
				( new uvsFlyFarmer(pg -> GoodsTypeBeg, pw)) -> elink(pw -> Panymal);
		}//  end for i
	}//  end if
}

int uvsBunch::game_exit( void ){
	int i;

	if (cycleTable[currentStage].Pdolly -> status || cycleTable[currentStage].Pdolly -> Pworld -> gIndex > 3) return 0;

	for ( i = 1; i <= cycleN; i++){
		int stage = (i + currentStage)%cycleN;
		uvsCultGame *pg = cycleTable[stage].Pgame;
		if (cycleTable[stage].Pdolly -> status == UVS_DOLLY_STATUS::UNABLE) continue;

		if ( pg ){
			char *name;
			switch(pg -> GameType){
			case UVS_GAME_TYPE::RACE:
				if ( pg -> typeTownBeg == UVS_TOWN::ESCAVE )
					name  = ((uvsEscave*)(pg -> townBeg)) -> name;
				else
					name = ((uvsSpot*)(pg -> townBeg)) -> name;

#ifdef _ROAD_
				if ( !strcmp( aci_curLocationName,  name) ) return stage;
#endif
				break;
			case UVS_GAME_TYPE::HARVEST:
				return stage;
				break;
			}//  end switch
		}//  end if
	}//  end for
	return -1;
}

int uvsBunch::game_wait_end( void ){
	int i;
	uvsCultGame *pg = cycleTable[currentStage].Pgame;

	if ( pg  &&  pg -> GameType == UVS_GAME_TYPE::HARVEST && !cycleTable[currentStage].Pdolly -> status  && cycleTable[currentStage].Pdolly -> Pworld -> gIndex < 4){
		i = nextCycle();
		return i;
	}
	return -1;
}

void uvsBunch::end_game( void ){
	uvsCultGame *pg = cycleTable[currentStage].Pgame;

	switch( pg -> GameType ){
	case UVS_GAME_TYPE::RACE:
		end_race();
		break;
	case UVS_GAME_TYPE::HARVEST:
		end_harvest();
		break;
	}
}

void uvsBunch::end_race( void ){
	uvsCultGame *pg = cycleTable[currentStage].Pgame;
	uvsItem* pi;

#ifdef _ROAD_
	//if ( CurrentWorld != -1 ){
	if ( uvsKronActive ){
		if (uvsapiDestroyItem(	pg -> GoodsTypeBeg, pg -> GoodsTypeLast ) || (GamerResult.get_eleech && pg -> GoodsTypeBeg == UVS_ITEM_TYPE::ELEECH ) ){
			GamerResult.luck -= 10;
			ShowLuckMessage(-10);
			if (GamerResult.luck < 0) GamerResult.luck = 0;

			pg -> all++;
			pg -> sucks++;
			if (pg -> GoodsTypeBeg == UVS_ITEM_TYPE::ELEECH)
				GamerResult.dead_eleech = 1;
			else
				GamerResult.dead_weezyk = 1;
	#ifdef STAND_REPORT
			stand < "\n" < "GAMER NOT FINISHED\n";
	#endif
		}
	} else {
#ifdef _ROAD_
		if (uvs_aciKillItem(uvsSetItemType(pg -> GoodsTypeBeg, 0, -1), uvsSetItemType(pg -> GoodsTypeLast, 0, -1)) ||
			(GamerResult.get_eleech && pg -> GoodsTypeBeg == UVS_ITEM_TYPE::ELEECH)){
			GamerResult.luck -= 10;
			ShowLuckMessage(-10);
			if (GamerResult.luck < 0) GamerResult.luck = 0;

			pg -> all++;
			pg -> sucks++;
			if (pg -> GoodsTypeBeg == UVS_ITEM_TYPE::ELEECH)
				GamerResult.dead_eleech = 1;
			else
				GamerResult.dead_weezyk = 1;

	#ifdef STAND_REPORT
			stand < "\n" < "GAMER NOT FINISHED\n";
	#endif
		}
#endif
	}
#endif

	if (GCharItem && GCharItem -> type == uvsSetItemType(pg -> GoodsTypeBeg, 0, 0) ){
		delete GCharItem;
		GCharItem = NULL;
	}
/*
	if (!pg -> GoodsTypeBegCount) {
		if ( pg -> typeBeg ){
			pi = GetItem( ((uvsEscave*)(pg -> townBeg)) -> Pshop -> Pitem, (pg -> GoodsTypeBeg), 0);
			while ( pi )  {
				((uvsItem*)pi) -> delink(((uvsEscave*)(pg -> townBeg)) -> Pshop -> Pitem);
				delete (uvsItem*)pi;
				pi = GetItem( ((uvsEscave*)(pg -> townBeg)) -> Pshop -> Pitem, (pg -> GoodsTypeBeg), 0);
			}
		} else {
			pi = GetItem( ((uvsSpot*)(pg -> townBeg)) -> Pshop -> Pitem, (pg -> GoodsTypeBeg), 0);
			while ( pi ){
				((uvsItem*)pi) -> delink(((uvsSpot*)(pg -> townBeg)) -> Pshop -> Pitem);
				delete (uvsItem*)pi;
				pi = GetItem( ((uvsSpot*)(pg -> townBeg)) -> Pshop -> Pitem, (pg -> GoodsTypeBeg), 0);
			}
		}
	}*/

	for ( int i = 0; i < WORLD_MAX; i++ ){
		listElem*& Item = WorldTable[i] -> Pitem;
		pi = GetItem( Item, (pg -> GoodsTypeBeg), 0);
		while ( pi ){
			((uvsItem*)pi) -> delink(Item);
			delete (uvsItem*)pi;
			pi = GetItem( Item, (pg -> GoodsTypeBeg), 0);
		}
	}

	uvsElement* e = ETail;
	while(e){
		if( ((uvsVanger*)e) -> type == UVS_OBJECT::VANGER ){
			pi = GetItem( ((uvsVanger*)e) -> Pitem, pg -> GoodsTypeBeg, 0);

			while ( pi ){
				((uvsItem*)pi) -> delink( ((uvsVanger*)e) -> Pitem);
				delete (uvsItem*)pi;
				pi = GetItem( ((uvsVanger*)e) -> Pitem, pg -> GoodsTypeBeg, 0);
			}//  end while
		}//  end while
		e = e -> enext;
	}

#ifdef STAND_REPORT
	stand < "\n" < ConTimer.GetTime();
	stand < "\n" < "Race Finished " <= pg -> total < "\n\n\n";
#endif

}

void uvsBunch::end_harvest( void ){
	uvsCultGame *pg = cycleTable[currentStage].Pgame;
	uvsItem* pi;
	uvsElement*& Panymal = Pescave -> Pworld -> Panymal;
	uvsElement* pe;
	int i, count;

	if (pg -> result && GamerResult.game_result && (GamerResult.last_game_type == UVS_GAME_ID::PIPETKA || GamerResult.last_game_type == UVS_GAME_ID::KERNOBOO)){
		uvsEndHarvest = 1;
		count = 1;
		pg -> result--;

		for( i = 0; i < pg -> result; i++)
				if ( pg -> score[i] >= GamerResult.game_result ) count++;

		for( i = pg -> result; i < pg -> total; i++)
				if ( pg -> score[i] > GamerResult.game_result ) count++;

			pg -> result = count;
			count = GamerResult.game_result;
			GamerResult.game_result = pg -> result;

			uvsCheckKronIventTabuTask(UVS_KRON_EVENT::END_RACE, count, pg -> result, pg -> GoodsTypeBeg);

			if ( pg -> result == 1 ){
				if ((pg -> total > 0) && (pg -> total - pg -> first > 0)){
					GamerResult.luck += pg -> total - pg -> first;
					ShowLuckMessage(pg -> total - pg -> first);
					if (GamerResult.luck > 100) GamerResult.luck = 100;
				}

				pg -> first++;
			}

			if ( pg -> result <= V_RACE_VANGER ){
	#ifdef _ROAD_
				//aciCurCredits += BONUS[pg -> result-1];
	#endif

				GamerResult.game_total	=  ++pg -> total;

				if (GamerResult.game_total > 1){
					if ( !strcmp( ((uvsSpot*)(pg -> townEnd)) -> name , "Lampasso") )
						GamerResult.last_game_type = UVS_GAME_ID::KERNOBOO;
						//GamerResult.game_ker_result += GAME_RAITING[ pg -> result-1];
					else if (!strcmp( ((uvsSpot*)(pg -> townEnd)) -> name , "Ogorod"))
						GamerResult.last_game_type = UVS_GAME_ID::PIPETKA;
						//GamerResult.game_pip_result += GAME_RAITING[ pg -> result-1];
				}
			}
			if (pg -> total <= pg -> result && pg -> result > 1){
				GamerResult.luck -= 5;
				ShowLuckMessage(-5);
				if (GamerResult.luck < 0) GamerResult.luck = 0;
			}
			pg -> all++;
	}

#ifdef _ROAD_
	//if ( CurrentWorld != -1 ){
	if ( uvsKronActive ){
		if (uvsapiDestroyItem(	pg -> GoodsTypeBeg, pg -> GoodsTypeLast )){
/*			GamerResult.luck -= 10;*/
			if (GamerResult.luck < 0) GamerResult.luck = 0;
		}
	} else {
		if (uvs_aciKillItem(uvsSetItemType(pg -> GoodsTypeBeg, 0, 1), uvsSetItemType(pg -> GoodsTypeLast, 0, 1))){
/*			GamerResult.luck -= 10;*/
			if (GamerResult.luck < 0) GamerResult.luck = 0;
		}
	}
#endif

	//  вызвать событие для KRONa

	while(Panymal){
		pe = Panymal;
		Panymal = Panymal -> enext;
		if ( pe -> type == UVS_OBJECT::FLY_FARMER ){
			Pescave -> Pworld -> remove_target( (uvsTarget*)((uvsFlyFarmer*)pe), UVS_TARGET::FLY_FARMER);
//			pe -> delink(Pescave -> Pworld -> Panymal);
			delete (uvsFlyFarmer*)pe;
		}
	}
	Panymal = NULL;

	for (i = 0; i < WORLD_MAX; i++ ){
		listElem*& Item = WorldTable[i] -> Pitem;
		pi = GetItem( Item, (pg -> GoodsTypeBeg), 0);
		while ( pi ){
			((uvsItem*)pi) -> delink(Item);
			delete (uvsItem*)pi;
			pi = GetItem( Item, (pg -> GoodsTypeBeg), 0);
		}//  while
	}//  for i

	uvsElement* e = ETail;
	while(e){
		if( ((uvsVanger*)e) -> type == UVS_OBJECT::VANGER ){
			pi = GetItem( ((uvsVanger*)e) -> Pitem, pg -> GoodsTypeBeg, 0);

			while ( pi ){
				((uvsItem*)pi) -> delink( ((uvsVanger*)e) -> Pitem);
				delete (uvsItem*)pi;
				pi = GetItem( ((uvsVanger*)e) -> Pitem, pg -> GoodsTypeBeg, 0);
			}//  while
		}
		e = e -> enext;
	}//  while

/*	uvsActInt* pr  = GItem;
	uvsActInt* pl;

	while(pr){

		if (pr -> type == uvsSetItemType(pg -> GoodsTypeBeg, 0, 0) ){
			pl = pr;
			pr = (uvsActInt*)pr -> next;
			pl -> delink(GItem);
			delete pl;
		} else
			pr = (uvsActInt*)pr -> next;
	}*/

#ifdef STAND_REPORT
	stand < "\n" < ConTimer.GetTime();
	if (GamerResult.game_result)
		stand < "\t in " <= pg -> result < " position  <- GAMER FINISH  \n";
	stand < "\n" < "Harvest Finished " <= pg -> total < " vangers play this.\n\n\n";
#endif

	/*delete (pg -> score);
	pg -> score = NULL;*/
	pg -> total = 0;
}

int uvsVanger::addGOODS(uvsCultGame* pg){
	uvsItem *pi;
	if (( pg -> typeTownBeg == UVS_TOWN::ESCAVE && ((uvsEscave*)(pg -> townBeg)) == Pescave ) ||
	   ( pg -> typeTownBeg == UVS_TOWN::SPOT && ((uvsSpot*)(pg -> townBeg)) == Pspot )){

		if ( pg -> GoodsBegCount > 0) {
			pi = new uvsItem(pg -> GoodsTypeBeg);
			pi -> param1 = Pworld -> gIndex<<16;
			 if (!addItem(pi)) {
				delete pi;
				return 0;
			 }
			 pg -> GoodsBegCount--;
			 return 1;
		} else return 0;
	}
	return 0;
}

int uvsVanger::add_to_race( uvsCultGame* pg ){
	if ( !(Pmechos -> status & UVS_MECHOS_USED::RACE) ) return 0;

	if (pg -> total >= V_RACE_VANGER )
		return 0;


	if ( pg -> GoodsBegCount > 0) {
		 addItem( new uvsItem(pg -> GoodsTypeBeg), 1);
		 pg -> GoodsBegCount--;
	}

	pg -> total++;
#ifdef STAND_REPORT
	stand < ConTimer.GetTime();
	stand < "\t add to race -> Vanger::";

	switch (shape){
	case UVS_VANGER_SHAPE::BUNCH_SLAVE:
		stand < "slave\n";
		break;
	case UVS_VANGER_SHAPE::CARAVAN:
		stand < "caravan\n";
		break;
	case UVS_VANGER_SHAPE::RANGER:
		stand < "ranger\n";
		break;
	case UVS_VANGER_SHAPE::THIEF:
		stand < "thief\n";
		break;
	case UVS_VANGER_SHAPE::TABUTASK:
		stand < "tabutask\n";
		break;
	}
#endif

	addInventory();

	if (pg -> GoodsTypeBegCount == 1){
		if ( pg -> typeTownBeg == UVS_TOWN::ESCAVE )
			status = UVS_VANGER_STATUS::GAME_HUNTER_BEGIN_ESCAVE;
		else
			status = UVS_VANGER_STATUS::GAME_HUNTER_BEGIN_SPOT;
	} else {
		if ( pg -> typeTownBeg == UVS_TOWN::ESCAVE )
			status = UVS_VANGER_STATUS::GAME_BEGIN_ESCAVE;
		else
			status = UVS_VANGER_STATUS::GAME_BEGIN_SPOT;
	}

	if ( pg -> typeTownEnd == UVS_TOWN::ESCAVE){
		uvsEscave* pe = (uvsEscave*) (pg -> townEnd);

		uvsOrder* ord = orderT; 		// первый элемент
		ord -> type = UVS_ORDER::MOVING;
		ord -> event = UVS_EVENT::GO_ESCAVE;
		ord -> target = UVS_TARGET::ESCAVE;
		ord -> Ptarget = (uvsTarget*)pe;
		//Pescave = pe;
	} else {
		uvsSpot* pe = (uvsSpot*) (pg -> townEnd);

		uvsOrder* ord = orderT; 		// первый элемент
		ord -> type = UVS_ORDER::MOVING;
		ord -> event = UVS_EVENT::GO_SPOT;
		ord -> target = UVS_TARGET::SPOT;
		ord -> Ptarget = (uvsTarget*)pe;
		//Pspot = pe;
	}
	return 1;
}

int uvsVanger::add_to_game( uvsCultGame* pg ){
	if ( Pescave != (uvsEscave*)(pg -> townBeg) && Pspot != (uvsSpot*)(pg -> townBeg) ) return 0;

	switch( pg -> GameType ){
	case UVS_GAME_TYPE::RACE:
		return add_to_race( pg );
		break;
	}
	return 0;
}

void uvsBunch::Quant(void){

	if ( status == UVS_BUNCH_STATUS::UNABLE ) return;

	if(--vngResource < 0)  randVRes();;
	int& cv = cycleTable[currentStage].cirtQ;
	int mv = cycleTable[currentStage].cirtMAX;

	if (cycleTable[currentStage].Pdolly -> status == UVS_DOLLY_STATUS::UNABLE )
		cv = mv +1;

	if(cv >= mv){
		int _i = 0;
		uvsWorld* pw = Pescave -> Pworld;
		pw -> updateResource();
		cv -= mv;

		if ( cycleTable[currentStage].Pgame && !NetworkON ) end_game();

		if(++currentStage == cycleN) currentStage = 0;

		while( cycleTable[currentStage].Pdolly -> status == UVS_DOLLY_STATUS::UNABLE  ){
			if(++currentStage == cycleN) currentStage = 0;

			_i++;
			if (_i > cycleN) {
				status = UVS_BUNCH_STATUS::UNABLE;
				currentStage = 0;
				//if( Pescave -> Pworld -> gIndex == CurrentWorld ) uvsCurrentCycle = 0;
				if( Pescave -> Pworld -> gIndex == Gamer -> Pworld -> gIndex ) {
					uvsCurrentCycle = 0;
					uvsCurrentWorldUnable = 1;
				}

#ifdef STAND_REPORT
			stand < ConTimer.GetTime();
			stand < "  bunch  in " < Pescave -> name < " on stage " <= currentStage < " unable\n";
#endif
				return;
			}
		}

		uvsChangeGoodsInList( Pescave -> Pworld -> gIndex);

		if( Pescave -> Pworld -> gIndex == Gamer -> Pworld -> gIndex ) {
			uvsCurrentCycle = currentStage;
			SOUND_CYCLE();
		}
#ifdef STAND_REPORT
		stand <  ConTimer.GetTime() < "\t changed cycle in "< Pescave -> Pworld -> name < " on :: "< cycleTable[currentStage].name < "\n\n";
#endif

#ifdef _ROAD_
		if(pw -> gIndex == CurrentWorld){
			BunchEvent( BUNCH_CHANGE_CYCLE );
		}
#endif

		if ( cycleTable[currentStage].Pgame && !NetworkON) begin_game();
	}
}

int  uvsBunch::nextCycle(void){
	int i = currentStage + 1;
	int _i = 0;

	if ( status == UVS_BUNCH_STATUS::UNABLE ) return -1;

	if( i == cycleN) i = 0;

	while( cycleTable[i].Pdolly -> status == UVS_DOLLY_STATUS::UNABLE  ){
		if(++i == cycleN) i = 0;

		_i++;
		if (_i > cycleN) {
			status = UVS_BUNCH_STATUS::UNABLE;
			currentStage = 0;
			if( Pescave -> Pworld -> gIndex == Gamer -> Pworld -> gIndex ) {
				uvsCurrentCycle = 0;
				uvsCurrentWorldUnable = 1;
			}
			return -1;
		}
	}//  end while
	return i;
}

uvsDolly::uvsDolly(uvsWorld* pw,int biosInd){
	biosNindex = biosInd;
	gIndex = DollyIndex++;
	Pworld = pw;
	type = UVS_OBJECT::DOLLY;

	pos_x = RND(pw -> x_size);
	pos_y = RND(pw -> y_size);

	status = 0;

	setSpeed();

	tail_cInd = tail_counter = 0;
	memset(tail_x, -1, sizeof(tail_x));
	memset(tail_y, -1, sizeof(tail_y));

	elink(ETail);
}

uvsDolly::uvsDolly(XStream& pfile){
	int cw;
	pfile > biosNindex;
	pfile > gIndex;
	pfile > cw;

	Pworld = WorldTable[cw];
	type = UVS_OBJECT::DOLLY;

	pfile > pos_x;
	pfile > pos_y;

	pfile > status;

	setSpeed();

	tail_cInd = tail_counter = 0;
	memset(tail_x, -1, sizeof(tail_x));
	memset(tail_y, -1, sizeof(tail_y));
	elink(ETail);
}

void uvsDolly::save(XStream& pfile){
	pfile < biosNindex;
	pfile < gIndex;
	pfile < Pworld -> gIndex;

	pfile < pos_x;
	pfile < pos_y;

	pfile < status;
}

void uvsDolly::setSpeed(void){
	x_speed = y_speed = 0;
//#ifdef ZMOD_BETA
//	//znfo - dolly motion
//	if (!NetworkON) {
//		while(!x_speed && !y_speed){
//			x_speed = V_SPEED_DOLLY - RND(V_SPEED_DOLLY*2+1);
//			y_speed = V_SPEED_DOLLY - RND(V_SPEED_DOLLY*2+1);
//		}
//	} else {
//		while(!x_speed && !y_speed){
//			x_speed = V_SPEED_DOLLY - NetRnd.Get(V_SPEED_DOLLY*2+1);
//			y_speed = V_SPEED_DOLLY - NetRnd.Get(V_SPEED_DOLLY*2+1);
//		}
//	}
//#else
	while(!x_speed && !y_speed){
		x_speed = V_SPEED_DOLLY - RND(V_SPEED_DOLLY*2+1);
		y_speed = V_SPEED_DOLLY - RND(V_SPEED_DOLLY*2+1);
		}
//#endif
}

void uvsDolly::Quant(void){
	if (status == UVS_DOLLY_STATUS::UNABLE || status == UVS_DOLLY_STATUS::CATCH) return;

	if (status) ErrH.Abort("uvsDolly::DAST IN MEMORY...");

	if ((Pworld->gIndex == CurrentWorld) && uvsKronActive) {
		ActiveQuant();
	} else {
		pos_x = (pos_x + x_speed) & (Pworld -> x_size - 1);
		pos_y = (pos_y + y_speed) & (Pworld -> y_size - 1);

		if(!tail_counter){
			tail_x[tail_cInd] = pos_x;
			tail_y[tail_cInd] = pos_y;
			tail_cInd = (tail_cInd + 1)%DOLLY_TAIL_LEN;
			tail_counter++;
			}
		else if(++tail_counter == DOLLY_TAIL_PERIOD) tail_counter = 0;

		if(!RND(128)) setSpeed();
	}
}

uvsFlyFarmer::uvsFlyFarmer(XStream& fin){
	int cw;
	fin > pos_x;
	fin > pos_y;
	fin > timer;
	fin > corn ;
	fin > corn_type;
	fin > cw;

	type = UVS_OBJECT::FLY_FARMER;
	Pworld = WorldTable[cw];

	setSpeed();
}

void uvsFlyFarmer::save(XStream& fout){
	fout < type;
	fout < pos_x;
	fout < pos_y;
	fout < timer;
	fout < corn ;
	fout < corn_type;
	fout < Pworld -> gIndex;
}

void uvsFlyFarmer::setSpeed(void){
	x_speed = y_speed = 0;
	while(!x_speed && !y_speed){
		x_speed = V_SPEED_DOLLY - RND(V_SPEED_DOLLY*2+1);
		y_speed = V_SPEED_DOLLY - RND(V_SPEED_DOLLY*2+1);
	}
}

void uvsFlyFarmer::Quant(void){

	if ((Pworld -> gIndex == CurrentWorld) && uvsKronActive ) return;

	pos_x = (pos_x + x_speed) & (Pworld -> x_size - 1);
	pos_y = (pos_y + y_speed) & (Pworld -> y_size - 1);

	if(++timer > FLY_FARMER_FIRST_TIME){
		if ( corn && !RND(144)){
			GrowCorn();
		}
	}

	if( timer > FLY_FARMER_LIVE || !RND(10)){
		Pworld -> remove_target( (uvsTarget*)this, UVS_TARGET::FLY_FARMER);
	}

	if(!RND(256)) setSpeed();
}

void uvsWorld::remove_target( uvsTarget* Pt, int _type){
	uvsCultGame* pg;
	uvsElement* pe = ETail;

	pg = escT[0] -> Pbunch -> cycleTable[escT[0] -> Pbunch -> currentStage].Pgame;

	if (!pg)
		ErrH.Abort("uvsWorld::remove_target : don't know where go ");

	while(pe){
		if( pe -> type == UVS_OBJECT::VANGER && ((uvsVanger*)pe) -> Pworld == this
			&&	((uvsVanger*)pe) -> orderT -> Ptarget == Pt){
			 if (pg -> typeTownEnd == UVS_TOWN::SPOT){
				 ((uvsVanger*)pe) -> set_target((uvsTarget*)((uvsSpot*)pg -> townEnd), UVS_TARGET::SPOT);
			} else {
				((uvsVanger*)pe) -> set_target( (uvsTarget*)((uvsEscave*)pg -> townEnd), UVS_TARGET::ESCAVE);
			}
		}
		pe = pe -> enext;
	}
}

void uvsVanger::break_harvest(void){
	uvsCultGame* pg;

	pg = Pworld -> escT[0] -> Pbunch -> cycleTable[Pworld -> escT[0] -> Pbunch -> currentStage].Pgame;

	if (!pg) {
		ErrH.Abort("uvsVanger::break_harvest : don't now where go ");
		return;
	}

	if (pg -> typeTownEnd == UVS_TOWN::SPOT){
		if (Pspot && !strcmp(Pspot -> name, ((uvsSpot*)(pg -> townEnd)) -> name)) return;
		set_target((uvsTarget*)((uvsSpot*)pg -> townEnd), UVS_TARGET::SPOT);
	} else {
		if (Pescave && !strcmp(Pescave -> name, ((uvsEscave*)(pg -> townEnd)) -> name)) return;
		set_target( (uvsTarget*)((uvsEscave*)pg -> townEnd), UVS_TARGET::ESCAVE);
	}
}

void uvsFlyFarmer::GrowCorn(void){
	uvsItem* pi;
	uvsItem* _pi;
	uvsElement* pv = ETail;

	pi = new uvsItem(corn_type);
	pi -> param2 = RND(5)+1;
	pi -> pos_x = pos_x;
	pi -> pos_y = pos_y;
	corn--;

	while( pv ){
		if ( pv -> type == UVS_OBJECT::VANGER &&
			((uvsVanger*)pv) -> Pworld == Pworld &&
			UVS_VANGER_SHAPE::GAMER !=  ((uvsVanger*)pv) -> shape &&
			UVS_VANGER_SHAPE::GAMER_SLAVE !=  ((uvsVanger*)pv) -> shape &&
			((uvsVanger*)pv) -> status == UVS_VANGER_STATUS::MOVEMENT &&
			(UVS_VANGER_SHAPE::BUNCH_SLAVE !=  ((uvsVanger*)pv) -> shape || ((uvsVanger*)pv) -> Pmechos -> color == 1))
		{
			if ( abs(((uvsVanger*)pv) -> pos_y - pos_y) < FLY_CORN_DELTA && abs(((uvsVanger*)pv) -> pos_x - pos_x) < FLY_CORN_DELTA){

				_pi = GetItem( ((uvsVanger*)pv) -> Pitem, corn_type, 0);
				if (_pi){
					_pi -> param2 += RND(5) + 1;

					if (_pi -> param2 > 10 ) _pi -> param2 = 10;
					delete pi;
				} else
					if(!((uvsVanger*)pv) -> addItem(pi, 1)) ErrH.Abort("uvsFlyFarmer::GrowCorn : don't add CORN");
				((uvsVanger*)pv) -> set_target((uvsTarget*)this, UVS_TARGET::FLY_FARMER);
				return;
			}
		}
		pv = pv -> enext;
	}
	pi -> param2 = 1;
	pi -> link( Pworld -> Pitem);
}

void uvsVanger::set_target(uvsTarget* pt, int _type){
	uvsOrder* ord = orderT; 		// первый элемент
	switch(_type){
		case UVS_TARGET::ESCAVE:
			ord -> type = UVS_ORDER::MOVING;
			ord -> event = UVS_EVENT::GO_ESCAVE;
			ord -> target = _type;
			ord -> Ptarget = pt;
			status = UVS_VANGER_STATUS::MOVEMENT;
			break;
		case UVS_TARGET::SPOT:
			ord -> type = UVS_ORDER::MOVING;
			ord -> event = UVS_EVENT::GO_SPOT;
			ord -> target = _type;
			ord -> Ptarget = pt;
			status = UVS_VANGER_STATUS::MOVEMENT;
			break;
		case UVS_TARGET::VANGER:
			ord -> type = UVS_ORDER::MOVING;
			ord -> event = UVS_EVENT::GO_KILL;
			ord -> target = _type;
			ord -> Ptarget = pt;
			status = UVS_VANGER_STATUS::MOVEMENT;
			break;
		case UVS_TARGET::VANGERS:
		case UVS_TARGET::PASSAGE:
		case UVS_TARGET::DOLLY:
		case UVS_TARGET::ITEM:
		case UVS_TARGET::BEEB:
		case UVS_TARGET::GRIB:
		case UVS_TARGET::HIVE:
		case UVS_TARGET::CRYPT:
		case UVS_TARGET::WHIRLPOOL:
		case UVS_TARGET::SWAMP:
		case UVS_TARGET::SHEILD:
		case UVS_TARGET::NONE:
			ErrH.Abort("uvsVanger::set_target : go nothing");
			break;
		case UVS_TARGET::FLY_FARMER:
			ord -> type = UVS_ORDER::MOVING;
			ord -> event = UVS_EVENT::GO_CORN;
			ord -> target = _type;
			ord -> Ptarget = pt;
			status = UVS_VANGER_STATUS::GATHERING;
			break;
	}
}

int uvsVanger::addItem(uvsItem* pi, int w ){
	int l_box[4];
	int k_box[4];
	int i;

	if ( !w ){
		memset(l_box, 0, sizeof(int)*4);
		listElem* pe = Pitem;

	//   проверка загруженности

		while(pe){
			l_box[uvsItemTable[((uvsItem*)pe) -> type] -> size]++;

			pe = pe -> next;
		}

		l_box[ uvsItemTable[((uvsItem*)pi) -> type ] -> size]++;

		memcpy(k_box, uvsMechosTable[Pmechos -> type] -> box, sizeof(int)*4 );

		for(i = 0; i < 4; i++){
			if( l_box[i] > k_box[i])
				return 0;
			else if ( i != 3) k_box[i+1] += k_box[i] - l_box[i];
		}
	}
	pi -> link(Pitem);
	return 1;
}

uvsVanger::uvsVanger(uvsEscave* pe){
	type = UVS_OBJECT::VANGER;	// common uvsElement type
	gIndex = currentVanger++;

	rating = 1;	// minimal
	status = UVS_VANGER_STATUS::ESCAVE_SLEEPING;		// состояние движения
	shape = UVS_VANGER_SHAPE::BUNCH_SLAVE;			// принадлежность "души"
	owner = pe -> Pbunch;
	Pworld = pe -> Pworld;
	Pescave = owner -> Pescave;
	Pspot = NULL;
	Ppassage = NULL;
	Pmechos = NULL; 			// указатель на мехос
	Pitem = NULL;					// указатель на инвентори

	pos_x = pe -> pos_x;
	pos_y = pe -> pos_y;
	biosNindex = RND(BIOS_MAX);				    // bios случайный
	orderT = new uvsOrder[ORDER_V_MAX];
	speed = 10 + RND(V_SPEED_MAX);
	locTimer = 0;
//	cirtStage = cirtQ = 0;
}

void uvsVanger::UpdateAllTarget(void){
	for ( int i = 0; i < ORDER_V_MAX; i++){
		switch (orderT[i].target){
		case UVS_TARGET::FLY_FARMER:
			break;
		case UVS_TARGET::NONE:
			break;
		case UVS_TARGET::PASSAGE:
			break;
		case UVS_TARGET::ESCAVE:
			break;
		case UVS_TARGET::SPOT:
			break;
		case UVS_TARGET::DOLLY:
			break;
		case UVS_TARGET::VANGER:{
			if (shape != UVS_VANGER_SHAPE::GAMER) ErrH.Abort("uvsVanger :: load - target vanger not in Gamer");

			uvsElement* p = ETail;
			uvsVanger* v;
			while(p){
				if(p -> type == UVS_OBJECT::VANGER){
					v = (uvsVanger*)p;
					if(v -> gIndex == gIndex ) {
						orderT[i].Ptarget = (uvsTarget*)v;
						break;
					}//  end if
				}//  end if
				p = p -> enext;
			}//  end while
			if (!p) ErrH.Abort("UpDateAllTarger - dont find target");
			gIndex = 0;
		}
			break;
		case UVS_TARGET::ITEM:
			break;
		}//  end switch
	}//  end for
}

//stalkerg Создаём вангера
uvsVanger::uvsVanger(XStream& pfile){
	type = UVS_OBJECT::VANGER;	// common uvsElement type

	int i;
	int count;
	char *s;
	char is;
	pfile > gIndex; // номер вангера
	pfile > rating; // minimal
	pfile > status; 	// состояние движения
	pfile > shape;			// принадлежность "души"
	pfile > i;
	Pworld	= WorldTable[i];
		
	pfile > is;
	if(is) Pmechos = new uvsMechos(pfile); else Pmechos = NULL;

	pfile > count;
	uvsItem *pi;
	Pitem = NULL;

	for( i = 0; i < count; i++) {
		(pi = new uvsItem(pfile)) -> link(Pitem);
	}

	

	s = get_string(pfile);
	if ( s ) {
		Pescave = (uvsEscave*)EscaveTail -> seekName(s);
		owner = Pescave -> Pbunch;
		delete[] s;
	}else owner = NULL;

	s = get_string(pfile);
	if ( s ) {
		Pescave = (uvsEscave*)EscaveTail -> seekName(s);
		delete[] s;
	} else {
		Pescave = NULL;
	}

	s = get_string(pfile);
	if ( s ) {
		Pspot = (uvsSpot*)SpotTail -> seekName(s);
		delete[] s;
	}else Pspot = NULL;

	s = get_string(pfile);
	if ( s ) {
		Ppassage = (uvsPassage*)PassageTail -> seekName(s);
		delete[] s;
	}else Ppassage = NULL;

	pfile > pos_x;
	pfile > pos_y;
	pfile > biosNindex;				    // bios случайный
	orderT = new uvsOrder[ORDER_V_MAX];

	for ( i = 0; i < ORDER_V_MAX; i++){
		pfile > orderT[i].type;
		pfile > orderT[i].event;
		pfile > orderT[i].target;

		switch (orderT[i].target){
		case UVS_TARGET::FLY_FARMER:
			s = get_string(pfile);
			orderT[i].Ptarget = Pworld -> get_anymal( pos_x, pos_y, UVS_OBJECT::FLY_FARMER);
			if (!orderT[i].Ptarget) ErrH.Abort("uvsVanger::load - bad FlyFarmer");
			break;
		case UVS_TARGET::NONE:
			s = get_string(pfile);
			orderT[i].Ptarget = NULL;
			break;
		case UVS_TARGET::PASSAGE:
			s = get_string(pfile);
			orderT[i].Ptarget = (uvsTarget*)((uvsPassage*)PassageTail -> seekName(s));
			delete[] s;
			break;
		case UVS_TARGET::ESCAVE:
			s = get_string(pfile);
			orderT[i].Ptarget = (uvsTarget*)((uvsEscave*)EscaveTail -> seekName(s));
			delete[] s;
			break;
		case UVS_TARGET::SPOT:
			s = get_string(pfile);
			orderT[i].Ptarget = (uvsTarget*)((uvsSpot*)SpotTail -> seekName(s));
			delete[] s;
			break;
		case UVS_TARGET::DOLLY:  {
			int cd;
			s = get_string(pfile);
			pfile > cd;

			uvsDolly* pd = (uvsDolly*)DollyTail;
			while(pd){
				if(pd -> gIndex == cd ) break;
				pd = (uvsDolly*)pd -> next;
			}
			if(!pd) ErrH.Abort(PrmWrongValue,XERR_USER,-1,"Dolly");
			orderT[i].Ptarget = (uvsTarget*)pd;
		}
			break;
		case UVS_TARGET::VANGER:
			s = get_string(pfile);

//			if (shape != UVS_VANGER_SHAPE::GAMER && shape != UVS_VANGER_SHAPE::KILLER)
//				ErrH.Abort("uvsVanger :: load - target vanger not in Gamer");

			if (shape == UVS_VANGER_SHAPE::GAMER)
				pfile > gIndex;
			break;
		case UVS_TARGET::ITEM:
			s = get_string(pfile);
			break;
		case UVS_TARGET::HIVE:
			orderT[i].target = UVS_TARGET::NONE;
			if (uvsLoadVersion < 1)
				s = get_string(pfile);
			break;
		case UVS_TARGET::CRYPT:
			orderT[i].target = UVS_TARGET::NONE;
			if (uvsLoadVersion == 1)
				s = get_string(pfile);
			break;
		case UVS_TARGET::WHIRLPOOL:
			orderT[i].target = UVS_TARGET::NONE;
			if (uvsLoadVersion < 4)
				s = get_string(pfile);
			break;
		case UVS_TARGET::SWAMP:
			orderT[i].target = UVS_TARGET::NONE;
			if (uvsLoadVersion < 5)
				s = get_string(pfile);
			break;
		case UVS_TARGET::SHEILD:
			orderT[i].target = UVS_TARGET::NONE;
			if (uvsLoadVersion < 5)
				s = get_string(pfile);
			break;
		case UVS_TARGET::VANGERS:
			orderT[i].target = UVS_TARGET::NONE;
			if (uvsLoadVersion == 1)
				s = get_string(pfile);
			break;
		default:
			orderT[i].target = UVS_TARGET::NONE;
			break;
		}
	}

	pfile > speed;
	locTimer = 1;

	if ( shape == UVS_VANGER_SHAPE::THIEF ){
		uvsItem *pi = GetItem_as_type( Pitem,  UVS_ITEM_STATUS::TREASURE);

		if ( !pi ) shape = UVS_VANGER_SHAPE::RANGER;
	};
	
}

void uvsVanger::save(XStream& pfile){
	int i, count = 0;
	pfile < gIndex;
	pfile < rating; // minimal
	pfile < status; 	// состояние движения
	pfile < shape;			// принадлежность "души"

	pfile < Pworld -> gIndex;

	if(Pmechos) {
		pfile < char(1);
		Pmechos -> save(pfile);
	} else
		pfile < char(0);

	listElem *pi = Pitem;
	count = 0;
	while( pi ){
		count++;
		pi = pi -> next;
	}

	pfile < count;
	pi = Pitem;
	for( i = 0; i < count; i++){
		((uvsItem*)pi) -> save(pfile);
		pi = pi -> next;
	}

	if(owner) put_string(pfile,  owner -> Pescave -> name); else pfile < ((char)0);
	if(Pescave) put_string(pfile,  Pescave -> name); else pfile < ((char)0);
	if(Pspot) put_string(pfile,  Pspot -> name);  else pfile < ((char)0);
	if(Ppassage) put_string(pfile,	Ppassage -> name);  else pfile < ((char)0);

	pfile < pos_x;
	pfile < pos_y;
	pfile < biosNindex;				    // bios случайный

	for ( i = 0; i < ORDER_V_MAX; i++){
		pfile < orderT[i].type;
		pfile < orderT[i].event;
		pfile < orderT[i].target;

		switch(orderT[i].target) {
		case UVS_TARGET::NONE:
			pfile < ((char)0);
			break;
		case UVS_TARGET::PASSAGE:
			put_string(pfile,  ((uvsPassage*)(orderT[i].Ptarget)) -> name );
			break;
		case UVS_TARGET::ESCAVE:
			put_string(pfile,  ((uvsEscave*)(orderT[i].Ptarget)) -> name);
			break;
		case UVS_TARGET::SPOT:
			put_string(pfile,  ((uvsSpot*)(orderT[i].Ptarget)) -> name);
			break;
		case UVS_TARGET::DOLLY:
			pfile < ((char)0);
			pfile < (((uvsDolly*)(orderT[i].Ptarget)) -> gIndex);
			break;
		case UVS_TARGET::VANGER:
			pfile < ((char)0);
			if (shape == UVS_VANGER_SHAPE::GAMER)
				pfile < (((uvsVanger*)(orderT[i].Ptarget)) -> gIndex);
			break;
		case UVS_TARGET::ITEM:
			pfile < ((char)0);
			break;
		case UVS_TARGET::FLY_FARMER:
			pfile < ((char)0);
			break;
		}//  end switch
	}//  end for i

	pfile < speed;
//	pfile < cirtStage < cirtQ;
}

uvsTarget* uvsWorld::get_anymal( int pos_x, int pos_y, int _type){
	uvsElement* pe = Panymal;
	uvsFlyFarmer* pf;

	while(pe){
		if (pe -> type == _type){
			switch(_type){
				case UVS_OBJECT::FLY_FARMER:
					pf = (uvsFlyFarmer*)pe;
					if (abs(pf -> pos_x - pos_x) < FLY_CORN_DELTA && abs(pf -> pos_y - pos_y) < FLY_CORN_DELTA)
						return (uvsTarget*)pf;
					break;
				default:
					ErrH.Abort("uvsWorld::get_anymal : seek nothing");
					break;
			}
		}
		pe = pe -> enext;
	}
	return (uvsTarget*)((uvsFlyFarmer*)Panymal);
}

void uvsWorld::getItemFromCrypt( int type){
/*	listElem *pc = Pcrypt;
	uvsItem *pi;

	while( pc ){
		if ( ((uvsCrypt*)pc) -> type == type ){
			int itype;
			int icount;
			int _type_ = 0;

			switch (type){
			case UVS_CRYPT::DEVICE:
				itype = UVS_ITEM_TYPE::COPTE_RIG;
				icount = UVS_ITEM_TYPE::RADAR_DEVICE + 1 - UVS_ITEM_TYPE::COPTE_RIG;
				(pi = new uvsItem(itype  + RND(icount))) -> link( Pitem );
#ifdef _ROAD_
				//pi -> param1 = UVS_DEVICE_POWER;
#endif
				break;
			case UVS_CRYPT::ARMO_LIGHT:
				itype = UVS_ITEM_TYPE::MACHOTINE_GUN_LIGHT;
				icount = UVS_ITEM_TYPE::TERMINATOR2 + 1 - itype;

				_type_ = RND(icount);
				while( !uvsItemTable[itype + _type_]->size )
					_type_ = RND(icount);

				(pi = new uvsItem(itype  + _type_)) -> link( Pitem );
				//pi -> param2 = 8;
				break;
			case UVS_CRYPT::ARMO_HEAVY:
				itype = UVS_ITEM_TYPE::MACHOTINE_GUN_LIGHT;
				icount = UVS_ITEM_TYPE::TERMINATOR2 +1 - itype;

				_type_ = RND(icount);
				while( uvsItemTable[itype + _type_]->size )
					_type_ = RND(icount);

				(pi = new uvsItem(itype  + _type_)) -> link( Pitem );
				//pi -> param2 = 8;
				break;
			case UVS_CRYPT::AMMO:
				itype = UVS_ITEM_TYPE::SPEETLE_SYSTEM_AMMO0;
				icount = UVS_ITEM_TYPE::CRUSTEST_CANNON_AMMO +1 - UVS_ITEM_TYPE::SPEETLE_SYSTEM_AMMO0;

				_type_ = RND(icount);

				(pi = new uvsItem(itype  + _type_)) -> link( Pitem );
				//pi -> param2 = uvsItemTable[_type_]->count;
				break;
			}

			pi -> pos_x = ((uvsCrypt*)pc) -> pos_x;
			pi -> pos_y = ((uvsCrypt*)pc) -> pos_y;
			pi -> pos_z = ((uvsCrypt*)pc) -> pos_z;
		}
		pc = pc -> next;
	}//  end while*/
}

int uvsSpot::game_soon( void ){
	return Pworld -> escT[0] -> Pbunch -> game_soon( name );
}

int uvsSpot::bring_game_GOODS( listElem*& pi ){
	return Pworld -> escT[0] -> Pbunch -> bring_game_GOODS( name, pi );
}

int uvsSpot::game_now( uvsCultGame*& pg) {
	return Pworld -> escT[0] -> Pbunch -> game_now( name, pg);
}

int uvsEscave::game_soon( void ){
	return	Pbunch -> game_soon( name );
}

int uvsEscave::real_hunter_now(uvsVanger* pv){
	if ( locked || Pworld -> locked ) return 0;

	return	Pbunch -> real_hunter_now(pv);
}

int uvsSpot::real_hunter_now(uvsVanger* pv){
	if ( locked || Pworld -> locked ) return 0;

	return	Pworld -> escT[0] -> Pbunch -> real_hunter_now(pv);
}

int uvsEscave::game_now( uvsCultGame*& pg ){
	return	Pbunch -> game_now( name, pg );
}

int uvsEscave::bring_game_GOODS( listElem*& pi ){
	return Pbunch -> bring_game_GOODS( name, pi );
}

int uvsBunch::game_soon( char * Pname){
	int i = 1;

	if (cycleTable[currentStage].Pdolly -> status || cycleTable[currentStage].Pdolly -> Pworld -> gIndex > 3) return 0;

	while(cycleTable[(currentStage+i)%cycleN].Pdolly -> status == UVS_DOLLY_STATUS::UNABLE){
		i++;
		if ( i >= cycleN) return 0;
	}

	uvsCultGame* pg = cycleTable[(currentStage+i)%cycleN].Pgame;

	if ( !pg ) return 0;

	switch (pg -> GameType ){
		case UVS_GAME_TYPE::RACE:
			if ( pg -> typeTownBeg == UVS_TOWN::ESCAVE && !strcmp(((uvsEscave*)(pg -> townBeg)) -> name, Pname)) return 1;
			if ( pg -> typeTownBeg == UVS_TOWN::SPOT && !strcmp(((uvsSpot*)(pg -> townBeg)) -> name, Pname)) return 1;
		break;
	}
	return 0;
}

int uvsBunch::real_hunter_now(uvsVanger* pv){
	uvsCultStage &pc = cycleTable[currentStage];
	uvsCultGame* pg = pc.Pgame;

	if (pc.Pdolly -> status == UVS_DOLLY_STATUS::UNABLE) return 0;

	if ( !pg ) return 0;

	if (pg -> GameType == UVS_GAME_TYPE::RACE){
		if (pg -> start){
			if (pg -> GoodsBegCount > 0){
				pv -> addItem( new uvsItem(pg -> GoodsTypeBeg), 1);
				pg -> GoodsBegCount--;
			}
			return 1;
		}//  end if pg -> start
		return 0;
	}//  end if pg -> GameType

	return 0;
}

int uvsVanger::game_result( const char* name ){
	if (!Pworld -> escTmax) return 0;

	uvsBunch *pb = Pworld -> escT[0] -> Pbunch;
	uvsCultGame* pg = pb -> cycleTable[pb -> currentStage].Pgame;

	if ( !pg ) return 0;

	switch( pg -> GameType ){
	case UVS_GAME_TYPE::RACE:
		return race_result(pg, name);
		break;
	case UVS_GAME_TYPE::HARVEST:
		return harvest_result(pg, name);
		break;
	}
	return 0;
}

int uvsVanger::harvest_result( uvsCultGame*  pg, const char* name ){
	uvsItem* pi;
	char* nn;
	int count = 0, GoodsType = -1;

	if ( (pg -> typeTownEnd == UVS_TOWN::ESCAVE && !strcmp( nn = ((uvsEscave*)(pg -> townEnd)) -> name, name))
		|| ( pg -> typeTownEnd == UVS_TOWN::SPOT && !strcmp( nn = ((uvsSpot*)(pg -> townEnd)) -> name, name)) ){
		GoodsType = pg -> GoodsTypeBeg;
	}

	if ( GoodsType !=-1 ){
		count = ItemCount( Pitem, GoodsType, 1 );

		if (GoodsType == UVS_ITEM_TYPE::PIPETKA){
			count += 5*ItemCount( Pitem, UVS_ITEM_TYPE::PIPKA );
			GamerResult.last_game_type = UVS_GAME_ID::PIPETKA;
		}else if (GoodsType == UVS_ITEM_TYPE::KERNOBOO){
			count += 5*ItemCount( Pitem, UVS_ITEM_TYPE::NOBOOL );
			GamerResult.last_game_type = UVS_GAME_ID::KERNOBOO;
		}

		if (count){
#ifdef STAND_REPORT
			stand < ConTimer.GetTime();
			stand < "\t in " < nn < " with " <= count < " corn  <- GAMER FINISH  \n";
#endif
			pg -> result = pg -> total + 1;
		}

		pi = GetItem( Pitem, GoodsType, 0 );
		if ( pi ){
			uvsActInt* pa = GGamer;
			uvsActInt* pp;
			while( pa ){
				pp = (uvsActInt*)pa -> next;

				if (uvsItemTable[ pi -> type ] -> SteelerTypeFull == pa -> type){
					pa -> delink(GGamer);
					delete pa;
				}
				pa = pp;
			}
//			pi = GetItem( Pitem, GoodsType, 0 );
//			if (!pi) ErrH.Abort("uvsVanger::harvest_result :: bad Pitem");
			uvs_aciKillItem(uvsItemTable[ pi -> type ] -> SteelerTypeFull);

			while(pi){

				((uvsItem*)pi) -> delink( Pitem );
				delete pi;
				pi = GetItem( Pitem, GoodsType, 0 );
			}
		}// if count

		return ( count );
	}//  if GoodsType != -1
	return 0;
}

int uvsVanger::race_result( uvsCultGame*  pg, const char* name ){
	uvsItem* pi;
	char* nn;

	if ( (pg -> typeTownEnd == UVS_TOWN::ESCAVE && !strcmp( nn = ((uvsEscave*)(pg -> townEnd)) -> name, name))
		|| ( pg -> typeTownEnd == UVS_TOWN::SPOT && !strcmp( nn = ((uvsSpot*)(pg -> townEnd)) -> name, name)) ) {

		pi = GetItem( Pitem, pg -> GoodsTypeEnd, 0 );
		int count = ItemCount( Pitem, pg -> GoodsTypeEnd);

		if ( pi ){
#ifdef _ROAD_
			uvs_aciKillItem(uvsItemTable[ pi -> type ] -> SteelerTypeFull);

			if (pg -> typeTownEnd == UVS_TOWN::ESCAVE)
				aciUpdateCurCredits(aciGetCurCredits() + (count-1)*((uvsEscave*)(pg -> townEnd)) -> Pshop -> Price[pg -> GoodsTypeEnd  + MAX_ITEM_TYPE]);
			else
				aciUpdateCurCredits(aciGetCurCredits() + (count-1)*((uvsSpot*)(pg -> townEnd)) -> Pshop -> Price[pg -> GoodsTypeEnd + MAX_ITEM_TYPE]);

			uvsActInt* pa = GGamer;
			uvsActInt* pp;
			while( pa ){
				pp = (uvsActInt*)pa -> next;

				if (uvsItemTable[ pi -> type ] -> SteelerTypeFull == pa -> type){
					pa -> delink(GGamer);
					delete pa;
				}
				pa = pp;
			}

			while( pi ){

				((uvsItem*)pi) -> delink( Pitem );
				delete pi;
				pi = GetItem( Pitem, pg -> GoodsTypeEnd, 0 );
			}//  end while

			/*if (pg -> typeEnd)
				aciCurCredits += (uvsItemTable[pg -> GoodsTypeEnd] -> sell_price*((uvsEscave*)(pg -> townEnd)) -> Pshop -> Price[pg -> GoodsTypeEnd])/100;
			else
				aciCurCredits += (uvsItemTable[pg -> GoodsTypeEnd] -> sell_price*((uvsSpot*)(pg -> townEnd)) -> Pshop -> Price[pg -> GoodsTypeEnd])/100;*/
#endif
			pg -> result++;
#ifdef STAND_REPORT
			stand < ConTimer.GetTime();
			stand < "\t in " < nn < " with result " <= pg -> result < " <- GAMER FINISH   from " <= (pg -> total+1) < "\t" <= count < "\n";
#endif
			uvsCheckKronIventTabuTask(UVS_KRON_EVENT::END_RACE, count, pg -> result, pg -> GoodsTypeEnd);

			if ( pg -> result == 1 ){
				if ((pg -> total > 0) && (pg -> total - pg -> first > 0)){
					GamerResult.luck += pg -> total - pg -> first;
					ShowLuckMessage( pg -> total - pg -> first);
					if (GamerResult.luck > 100) GamerResult.luck = 100;
				}

				pg -> first++;
			}

			if ( pg -> result <= V_RACE_VANGER ){
#ifdef _ROAD_
				//aciCurCredits += BONUS[pg -> result-1];
#endif
				if ( !strcmp( nn , "Podish") && GamerResult.get_eleech ){
					GamerResult.last_game_type = UVS_GAME_ID::ELEECH;
					//GamerResult.game_elr_result += GAME_RAITING[ pg -> result-1];
					GamerResult.get_eleech = 0;
				}else if ( !strcmp( nn , "B-Zone") )
					GamerResult.last_game_type = UVS_GAME_ID::WEEZYK;
					//GamerResult.game_zyk_result += GAME_RAITING[ pg -> result-1];

				GamerResult.game_total	=  ++pg -> total;
			}//  end if

			pg -> all++;

			if (pg -> total <= pg -> result && pg -> result > 1){
				GamerResult.luck -= 5;
				ShowLuckMessage(-5);
				if (GamerResult.luck < 0) GamerResult.luck = 0;
			}
			return ( pg -> result );
		} else if ( !strcmp( nn , "B-Zone") && pg -> flag){
				GamerResult.last_game_type = UVS_GAME_ID::WEEZYK;
				GamerResult.game_total	=  ++pg -> total;
				return ( pg -> result );
		}//  end if (pi)
	}//  end if (pi)
	return 0;
}

int uvsBunch::bring_game_GOODS( char * Pname, listElem*& pe){
	uvsCultGame* pg = cycleTable[currentStage].Pgame;

	if ( !pg ) return 0;

	switch(pg -> GameType){
	case UVS_GAME_TYPE::RACE:
		return bring_race_GOODS(Pname, pe);
		break;
	case UVS_GAME_TYPE::HARVEST:
		return bring_harvest_GOODS(Pname, pe);
		break;
	}
	return 0;
}

int uvsBunch::bring_harvest_GOODS( char * Pname, listElem*& pe){
	char *nn;
	uvsCultGame* pg = cycleTable[currentStage].Pgame;
	uvsItem *pi = NULL;
	int GoodsType = -1, count;

	if ( (pg -> typeTownEnd == UVS_TOWN::ESCAVE && !strcmp( nn = ((uvsEscave*)(pg -> townEnd)) -> name, Pname))
		|| ( pg -> typeTownEnd == UVS_TOWN::SPOT && !strcmp( nn = ((uvsSpot*)(pg -> townEnd)) -> name, Pname)) ) {
		GoodsType = pg -> GoodsTypeBeg;
	}

	if ( GoodsType != -1 ){
		pi = GetItem( pe, GoodsType, 0 );
		if (pi)
			count = ItemCount( pe, GoodsType );
		else
			count = 0;

		while( pi ){
			pi -> delink( pe );
			delete pi;
			pi = GetItem( pe, pg -> GoodsTypeEnd, 0 );
		}

		//  поставить обработчик присвоения места
		if ( count ){
			pg -> score[pg -> total++] = count;

#ifdef STAND_REPORT
			stand < ConTimer.GetTime();
			stand < "\t in " < nn < " finished with " <= count < " corns  <- any vanger \n";
#endif
		return (V_RACE_SCORE*(count));
		}
	}
	return 0;
}

int uvsBunch::bring_race_GOODS( char * Pname, listElem*& pe){
	char *nn;
	uvsCultGame* pg = cycleTable[currentStage].Pgame;
	uvsItem* pi;

	if ( (pg -> typeTownEnd == UVS_TOWN::ESCAVE && !strcmp( nn = ((uvsEscave*)(pg -> townEnd)) -> name, Pname))
		|| ( pg -> typeTownEnd == UVS_TOWN::SPOT && !strcmp( nn = ((uvsSpot*)(pg -> townEnd)) -> name, Pname)) ) {

		pi = GetItem( pe, pg -> GoodsTypeEnd, 0 );

		if ( pi ){
			pg -> result++;

#ifdef STAND_REPORT
			stand < ConTimer.GetTime();
			stand < "\t in " < nn < " with result " <= pg -> result < "  <- ";
#endif
		}

		while( pi ){
			pi -> delink( pe );
			delete pi;
			pi = GetItem( pe, pg -> GoodsTypeEnd, 0 );
		}

		if ( pg -> GoodsEndCount  && pi ){

			pg -> GoodsEndCount--;
			return (V_RACE_SCORE*(pg -> GoodsEndCount+1));
		}
#ifdef STAND_REPORT
		else if ( pi )
			stand < " finished any\n";
#endif
	}
	return 0;
}

void uvsEscave::add_goods_to_shop( void ){ //znfo - добавка товаров в магазин
	int i, n;
	uvsItem *pi;
	uvsTradeItem *pt = (uvsTradeItem*)Ptrade;

//#ifdef ZMOD_BETA
//	//znfo - безусловное удаление кукиша в сингле
//	if (!NetworkON) {
//#endif
		pi = GetItem(Pshop->Pitem, UVS_ITEM_TYPE::CONLARVER,0);
		if (pi){
			pi -> delink(Pshop ->Pitem);
			delete pi;
		}
//#ifdef ZMOD_BETA
//	}
//#endif

	if (Pbunch -> status == UVS_BUNCH_STATUS::UNABLE) return;

	while( pt ){
		int k = 0;

		if (NetworkON){
			n = uvsQuantity;
				
			if (n > 32) n = 32;

			if (!uvsGoodsON)
				n = 0;
		} else {
			n = 4 + RND(5) + 4;

			if (pt -> type == UVS_ITEM_TYPE::POPONKA && !NetworkON)
				n = DG_POPONKA_MAX - 1;
		}

		for( i = 0; i < n; i++,k++) {

			if ( k == uvsPoponkaID) k++;
			if (k == DG_POPONKA_MAX) k = 0;

			(pi =new uvsItem( pt -> type )) -> link( Pshop -> Pitem );
			pi -> param1 = Pworld -> gIndex<<16;

			if (pt -> type == UVS_ITEM_TYPE::POPONKA)
				pi -> param2 = k;

/*			if (pt -> type == UVS_ITEM_TYPE::POPONKA){
				pi -> param2 = RND(DG_POPONKA_MAX);
				while(pi -> param2 == uvsPoponkaID)
					pi -> param2 = RND(DG_POPONKA_MAX);
			}*/
		}
		pt = (uvsTradeItem*)(pt -> next);
	}
#ifdef ALL_ITEM_IN_SHOP
	if (1){
#else
	if (!NetworkON && Gamer->CirtDelivery()>=CIRT_FOR_CONLARVER){
#endif
		pi = GetItem(Pshop ->Pitem, UVS_ITEM_TYPE::CONLARVER,0);

		if (!pi){
			(pi =new uvsItem( UVS_ITEM_TYPE::CONLARVER )) -> link( Pshop -> Pitem );
		}
	}

#ifdef ALL_ITEM_IN_SHOP
	int may_tabu = 2;
#else
	int may_tabu = 0;
#endif

	switch(Gamer -> Pworld -> gIndex){
	case 0:
		may_tabu += GamerResult.nymbos_total + GamerResult.phlegma_total;
		break;
	case 1:
		may_tabu += GamerResult.heroin_total + GamerResult.shrub_total;
		break;
	case 2:
		may_tabu += GamerResult.poponka_total + GamerResult.toxick_total;
		break;
	}//  end switch

	int tabu_on = 0;
	if (!NetworkON && (may_tabu > 1 /*|| GamerResult.luck >= 70*/)){
		for( i = TabuTaskID + RND(TabuTaskCount); i < TabuTaskID + TabuTaskCount; i++)
			if (!TabuTable[i] -> status &&	TabuTable[i] -> is_able()){
				(pi =new uvsItem( UVS_ITEM_TYPE::TABUTASK )) -> link( Pshop -> Pitem );
				pi -> param1 = Pworld -> gIndex<<16;
				pi -> param2 = i;
				tabu_on = 1;
				break;
			}

		if (!tabu_on)
			for( i = TabuTaskID; i < TabuTaskID + TabuTaskCount; i++)
			if (!TabuTable[i] -> status &&	TabuTable[i] -> is_able()){
				(pi =new uvsItem( UVS_ITEM_TYPE::TABUTASK )) -> link( Pshop -> Pitem );
				pi -> param1 = Pworld -> gIndex<<16;
				pi -> param2 = i;
				tabu_on = 1;
				break;
			}

		
#ifdef ALL_ITEM_IN_SHOP
		if(!PassagerWait){
#else
		if (TabuTaskGood > 0 && !PassagerWait){
#endif
			PassagerWait = 1;
			switch(Pworld -> gIndex){
			case 0:
				(pi =new uvsItem( UVS_ITEM_TYPE::ELEEPOD )) -> link( Pshop -> Pitem );
				pi -> param1 = Pworld -> gIndex<<16;
				pi -> param2 = TabuTaskID | (Pworld -> sptT[0] -> TabuTaskID <<16);
				break;
			case 1:
				(pi =new uvsItem( UVS_ITEM_TYPE::BEEBOORAT )) -> link( Pshop -> Pitem );
				pi -> param1 = Pworld -> gIndex<<16;
				pi -> param2 = TabuTaskID | (Pworld -> sptT[RND(Pworld -> sptTmax)] -> TabuTaskID <<16);
				break;
			case 2:{
					(pi =new uvsItem( UVS_ITEM_TYPE::ZEEX )) -> link( Pshop -> Pitem );
					pi -> param1 = Pworld -> gIndex<<16;

					int WorldID = RND(3);

					if (RND(2) && WorldID != 2)
						pi -> param2 = TabuTaskID | (WorldTable[WorldID] -> escT[0] -> TabuTaskID <<16);
					else
						pi -> param2 = TabuTaskID | (WorldTable[WorldID] -> sptT[RND(WorldTable[WorldID] -> sptTmax)] -> TabuTaskID <<16);
			       }
				break;
			}//  end switch
		}//  end if
	}//  end if
}

void uvsSpot::add_goods_to_shop( void ){
	int i, n;
	uvsItem *pi;
	uvsTradeItem *pt = (uvsTradeItem*)Ptrade;

	if (Pworld -> escT[0] -> Pbunch -> status == UVS_BUNCH_STATUS::UNABLE) return;

	while( pt ){
		int k = 0;

		if (NetworkON){
			n = uvsQuantity;
			if (n > 32) n = 32;

			if (!uvsGoodsON)
				n = 0;
		} else {
			n = 4 + RND(5) + 4;

			if (pt -> type == UVS_ITEM_TYPE::POPONKA && !NetworkON)
				n = DG_POPONKA_MAX - 1;
		}

		for( i = 0; i < n; i++,k++) {

			if ( k == uvsPoponkaID) k++;
			if (k == DG_POPONKA_MAX) k = 0;

			(pi =new uvsItem( pt -> type )) -> link( Pshop -> Pitem );
			pi -> param1 = Pworld -> gIndex<<16;

			if (pt -> type == UVS_ITEM_TYPE::POPONKA)
				pi -> param2 = k;

/*			if (pt -> type == UVS_ITEM_TYPE::POPONKA){
				pi -> param2 = RND(DG_POPONKA_MAX);
				while(pi -> param2 == uvsPoponkaID)
					pi -> param2 = RND(DG_POPONKA_MAX);
			}*/
		}
		pt = (uvsTradeItem*)(pt -> next);
	}//  end while

#ifdef ALL_ITEM_IN_SHOP
	int may_tabu = 2;
#else
	int may_tabu = 0;
#endif

	switch(Gamer -> Pworld -> gIndex){
	case 0:
		may_tabu += GamerResult.nymbos_total + GamerResult.phlegma_total;
		break;
	case 1:
		may_tabu += GamerResult.heroin_total + GamerResult.shrub_total;
		break;
	case 2:
		may_tabu += GamerResult.poponka_total + GamerResult.toxick_total;
		break;
	}//  end switch

	int tabu_on = 0;
	if (!NetworkON && (may_tabu > 1 /*|| GamerResult.luck >= 70*/)){
		for( i = TabuTaskID + RND(TabuTaskCount); i < TabuTaskID + TabuTaskCount; i++)
			if (!TabuTable[i] -> status &&	TabuTable[i] -> is_able()){
				(pi =new uvsItem( UVS_ITEM_TYPE::TABUTASK )) -> link( Pshop -> Pitem );
				pi -> param1 = Pworld -> gIndex<<16;
				pi -> param2 = i;
				tabu_on = 1;
				break;
			}

		if (!tabu_on)
			for( i = TabuTaskID; i < TabuTaskID + TabuTaskCount; i++)
			if (!TabuTable[i] -> status &&	TabuTable[i] -> is_able()){
				(pi =new uvsItem( UVS_ITEM_TYPE::TABUTASK )) -> link( Pshop -> Pitem );
				pi -> param1 = Pworld -> gIndex<<16;
				pi -> param2 = i;
				tabu_on = 1;
				break;
			}

#ifdef ALL_ITEM_IN_SHOP
			if(!PassagerWait){
#else
		if (TabuTaskGood > 0 && !PassagerWait){
#endif
			PassagerWait = 1;
			switch(Pworld -> gIndex){
			case 0:
				(pi =new uvsItem( UVS_ITEM_TYPE::ELEEPOD )) -> link( Pshop -> Pitem );
				pi -> param1 = Pworld -> gIndex<<16;
				pi -> param2 = TabuTaskID | (Pworld -> escT[0] -> TabuTaskID <<16);
				break;
			case 1:
				(pi =new uvsItem( UVS_ITEM_TYPE::BEEBOORAT )) -> link( Pshop -> Pitem );
				pi -> param1 = Pworld -> gIndex<<16;

				if (RND(2))
					pi -> param2 = TabuTaskID | (Pworld -> escT[0] -> TabuTaskID <<16);
				else {
					int where;

					if (Pworld -> sptT[0] == this)
						where = 1;
					else
						where = 0;

					pi -> param2 = TabuTaskID | (Pworld -> sptT[where] -> TabuTaskID <<16);
				}
				break;
			case 2:{
					(pi =new uvsItem( UVS_ITEM_TYPE::ZEEX )) -> link( Pshop -> Pitem );
					pi -> param1 = Pworld -> gIndex<<16;

					int WorldID = RND(3);

					if (RND(2) && WorldID != 2)
						pi -> param2 = TabuTaskID | (WorldTable[WorldID] -> sptT[RND(WorldTable[WorldID] -> sptTmax)] -> TabuTaskID <<16);
					else
						pi -> param2 = TabuTaskID | (WorldTable[WorldID] -> escT[0] -> TabuTaskID <<16);
			       }
				break;
			}//  end switch
		}//  end if
	}//  end if
}

int uvsBunch::game_now( char * Pname, uvsCultGame*& Pg){
	uvsCultGame* pg = cycleTable[currentStage].Pgame;

	Pg = NULL;

	if ( !pg ) return 0;
	//if ( (!pg -> GoodsEndCount || (!pg -> GoodsBegCount) && pg -> total > 6 )) return 0;
	if ( (!pg -> GoodsEndCount) || (!pg -> GoodsBegCount) || (pg -> total > V_RACE_VANGER )) return 0;

	Pg = pg;
	switch (pg -> GameType ){
		case UVS_GAME_TYPE::RACE:
			if ( pg -> typeTownBeg == UVS_TOWN::ESCAVE && !strcmp(((uvsEscave*)(pg -> townBeg) ) -> name, Pname) ) return 1;
			if ( pg -> typeTownBeg == UVS_TOWN::SPOT && !strcmp(((uvsSpot*)(pg -> townBeg) ) -> name, Pname) ) return 1;
		break;
	}
	return 0;
}

void uvsVanger::Quant(void){
	uvsCultGame* pg;
	
	c_All_Number++;
	switch(shape){
		case UVS_VANGER_SHAPE::BUNCH_SLAVE:
			c_S_Number++;
			switch(status){
				case UVS_VANGER_STATUS::GAME_BEGIN_ESCAVE:
					if ( Pescave -> locked || (--locTimer > 0)) break;
					status = UVS_VANGER_STATUS::RACE;
#ifdef _ROAD_
					if(isActive()) addVanger(this,Pescave);
#endif
					break;
				case UVS_VANGER_STATUS::GAME_HUNTER_BEGIN_ESCAVE:
					if ( !Pescave -> real_hunter_now(this) || (--locTimer > 0) ) break;
					status = UVS_VANGER_STATUS::RACE_HUNTER;
#ifdef _ROAD_
						if(isActive()) addVanger(this,Pescave);
#endif
					break;
				case UVS_VANGER_STATUS::GAME_BEGIN_SPOT:
					if ( Pspot -> locked || (--locTimer > 0)) break;
					status = UVS_VANGER_STATUS::RACE;
#ifdef _ROAD_
						if(isActive()) addVanger(this,Pspot);
#endif
					break;
				case UVS_VANGER_STATUS::GAME_HUNTER_BEGIN_SPOT:
					if ( !Pspot -> real_hunter_now(this) || (--locTimer > 0)) break;
					status = UVS_VANGER_STATUS::RACE_HUNTER;
		#ifdef _ROAD_
					if(isActive()) addVanger(this,Pspot);
		#endif
					break;
				case UVS_VANGER_STATUS::ESCAVE_SLEEPING:
					if(isBunchReady() && (Pescave -> may_go_away(Pmechos, shape))){
						if (prepare_killer()){
							go_to_GAMER();
						} else if(Pescave -> game_soon() && RND(2)){
							status = UVS_VANGER_STATUS::ESCAVE_WAITING;
							break;
						} else if (Pescave -> game_now(pg)){
							add_to_game(pg);
						} else if(!RND(2)) {
							Event(UVS_EVENT::GO_FROM_ESCAVE);
						} else {
							Event(UVS_EVENT::GO_CIRT);
						}

						if (status != UVS_VANGER_STATUS::ESCAVE_SLEEPING)
							owner -> randVRes();
#ifdef _ROAD_
						if(isActive()) addVanger(this,Pescave);
#endif
					}
					break;
				case UVS_VANGER_STATUS::SPOT_SLEEPING:
					if(isBunchReady() && (!Pspot -> locked)){
						if(Pspot -> game_soon() && !RND(3)){
							status = UVS_VANGER_STATUS::SPOT_WAITING;
							break;
						} else if (Pspot -> game_now(pg))
							add_to_game(pg);
						else
							Event(UVS_EVENT::GO_FROM_SPOT);
						if (status != UVS_VANGER_STATUS::SPOT_SLEEPING)
							owner -> randVRes();
#ifdef _ROAD_
						if(isActive()) addVanger(this,Pspot);
#endif
						}
					break;
				case UVS_VANGER_STATUS::MOVEMENT:
				case UVS_VANGER_STATUS::FREE_MOVEMENT:
				case UVS_VANGER_STATUS::RACE:
				case UVS_VANGER_STATUS::RACE_HUNTER:
				case UVS_VANGER_STATUS::GATHERING:
				case UVS_VANGER_STATUS::WAIT_GAMER:
					break;
				case UVS_VANGER_STATUS::PASSAGE_GAP:
					if(!--locTimer){
						if(makeJump()){
#ifdef _ROAD_
						if(isActive()) addVanger(this,Ppassage);
#endif
						}else
							locTimer = 1;
					}
					break;
				case UVS_VANGER_STATUS::AIM_HANDLE:
					if(!--locTimer) aimHandle();
					break;
				}
			break;
		case UVS_VANGER_SHAPE::CARAVAN:
		case UVS_VANGER_SHAPE::RANGER:
		case UVS_VANGER_SHAPE::THIEF:
			if ( shape == UVS_VANGER_SHAPE::THIEF)
				c_A_Number++;
			else
				c_R_Number++;
			switch(status){
				case UVS_VANGER_STATUS::GAME_BEGIN_ESCAVE:
					if ( Pescave -> locked || (--locTimer > 0)) break;
					status = UVS_VANGER_STATUS::RACE;
#ifdef _ROAD_
						if(isActive()) addVanger(this,Pescave);
#endif
					break;
				case UVS_VANGER_STATUS::GAME_HUNTER_BEGIN_ESCAVE:
					if ( !Pescave -> real_hunter_now(this) || (--locTimer > 0)) break;
					status = UVS_VANGER_STATUS::RACE_HUNTER;
#ifdef _ROAD_
						if(isActive()) addVanger(this,Pescave);
#endif
					break;

				case UVS_VANGER_STATUS::GAME_BEGIN_SPOT:
					if ( Pspot -> locked || (--locTimer > 0)) break;
					status = UVS_VANGER_STATUS::RACE;
#ifdef _ROAD_
						if(isActive()) addVanger(this,Pspot);
#endif
					break;
				case UVS_VANGER_STATUS::GAME_HUNTER_BEGIN_SPOT:
					if ( !Pspot -> real_hunter_now(this) || (--locTimer > 0)) break;
					status = UVS_VANGER_STATUS::RACE_HUNTER;
#ifdef _ROAD_
						if(isActive()) addVanger(this,Pspot);
#endif
					break;

				case UVS_VANGER_STATUS::ESCAVE_SLEEPING:
					if(isBunchReady() && (Pescave -> may_go_away(Pmechos, shape))){
//					if(Pescave -> may_go_away(Pmechos, shape)){
						if(Pescave -> game_soon() ){
							status = UVS_VANGER_STATUS::ESCAVE_WAITING;
							break;
						} else if (Pescave -> game_now(pg))
							add_to_game(pg);
						else if (shape !=  UVS_VANGER_SHAPE::THIEF && !RND(48)){
//						else if (shape !=  UVS_VANGER_SHAPE::THIEF && !RND(2)){
							Event(UVS_EVENT::THIEF);
						} else if(shape !=  UVS_VANGER_SHAPE::THIEF && !RND(5))
							Event(UVS_EVENT::THIEF_FROM_SHOP);
						else if(!RND(2))
							Event(UVS_EVENT::GO_FROM_ESCAVE);
						else
							Event(UVS_EVENT::GO_NEW_ESCAVE);
						if (status != UVS_VANGER_STATUS::ESCAVE_SLEEPING)
							owner -> randVRes();
#ifdef _ROAD_
						if(isActive()) addVanger(this,Pescave);
#endif
						}
					break;
				case UVS_VANGER_STATUS::SPOT_SLEEPING:
					if(isBunchReady() && (!Pspot -> locked)){
//					if(!Pspot -> locked){
						if(Pspot -> game_soon()){
							status = UVS_VANGER_STATUS::SPOT_WAITING;
							break;
						} else if (Pspot -> game_now(pg))
							add_to_game(pg);
						else if (shape !=  UVS_VANGER_SHAPE::THIEF && !RND(48)){
//						else if (shape !=  UVS_VANGER_SHAPE::THIEF && !RND(2)){
							thief_from_spot(Pspot);
						} else
							Event(UVS_EVENT::GO_FROM_SPOT);

						if (status != UVS_VANGER_STATUS::SPOT_SLEEPING)
							owner -> randVRes();
#ifdef _ROAD_
						if(isActive()) addVanger(this,Pspot);
#endif
						}
					break;
				case UVS_VANGER_STATUS::MOVEMENT:
				case UVS_VANGER_STATUS::FREE_MOVEMENT:
				case UVS_VANGER_STATUS::RACE:
				case UVS_VANGER_STATUS::RACE_HUNTER:
				case UVS_VANGER_STATUS::GATHERING:
				case UVS_VANGER_STATUS::WAIT_GAMER:

					break;
				case UVS_VANGER_STATUS::PASSAGE_GAP:
					if(!--locTimer){
						if(makeJump()){
#ifdef _ROAD_
							if(isActive()) addVanger(this,Ppassage);
#endif
							}
						else locTimer = 1;
						}
					break;
				}
			break;
		case UVS_VANGER_SHAPE::TABUTASK:
			switch(status){
			case UVS_VANGER_STATUS::SPOT_SLEEPING:
				Event(UVS_EVENT::GO_FROM_SPOT);

				if (status != UVS_VANGER_STATUS::SPOT_SLEEPING)
					owner -> randVRes();
#ifdef _ROAD_
				if(isActive()) addVanger(this,Pspot);
#endif
				break;
			case UVS_VANGER_STATUS::ESCAVE_SLEEPING:
				if(!RND(2))
					Event(UVS_EVENT::GO_FROM_ESCAVE);
				else
					Event(UVS_EVENT::GO_NEW_ESCAVE);

				if (status != UVS_VANGER_STATUS::ESCAVE_SLEEPING)
					owner -> randVRes();
#ifdef _ROAD_
				if(isActive()) addVanger(this,Pescave);
#endif
				break;
			case UVS_VANGER_STATUS::MOVEMENT:
			case UVS_VANGER_STATUS::FREE_MOVEMENT:
				break;
			case UVS_VANGER_STATUS::PASSAGE_GAP:
					if(!--locTimer){
						if(makeJump()){
#ifdef _ROAD_
							if(isActive()) addVanger(this,Ppassage);
#endif
						} else locTimer = 1;
					}
					break;
			};//  end switch
			break;
		case UVS_VANGER_SHAPE::KILLER:
			switch(status){
			case UVS_VANGER_STATUS::MOVEMENT:
			case UVS_VANGER_STATUS::FREE_MOVEMENT:
			case UVS_VANGER_STATUS::WAIT_GAMER:
				break;
			case UVS_VANGER_STATUS::PASSAGE_GAP:
					if(!--locTimer){
						if(makeJump()){
#ifdef _ROAD_
							if(isActive()) addVanger(this,Ppassage);
#endif
						} else locTimer = 1;
					}
					break;
			}//  end switch
			break;
		case UVS_VANGER_SHAPE::GAMER:
			switch(status){
#ifdef _ROAD_
			case UVS_VANGER_STATUS::GO_NEW_WORLD:
				/*{
					Pworld -> getItemFromCrypt(UVS_CRYPT::DEVICE);
					Pworld -> getItemFromCrypt(UVS_CRYPT::AMMO);
					Pworld -> getItemFromCrypt(UVS_CRYPT::ARMO_LIGHT);
					Pworld -> getItemFromCrypt(UVS_CRYPT::ARMO_HEAVY);
				} else {
					Pworld -> updateCrypt();
				}*/

				EffectsOff();

				Pworld -> GamerVisit++;

				if (WorldTable[Pworld -> gIndex]-> escTmax){
					uvsCurrentCycle = WorldTable[Pworld -> gIndex] -> escT[0] -> Pbunch -> currentStage;
					 if (Gamer -> Pworld -> escT[0] -> Pbunch -> status == UVS_BUNCH_STATUS::UNABLE)
						uvsCurrentWorldUnable = 1;
					else
						uvsCurrentWorldUnable = 0;
				} else {
					uvsCurrentCycle = 0;
					uvsCurrentWorldUnable = 0;
				}

				if (WorldTable[Pworld -> gIndex] -> escTmax)
					aci_curLocationName = WorldTable[Pworld -> gIndex] -> escT[0] -> name;
				else
					aci_curLocationName = "";
				
				ChangeWorld(Pworld -> gIndex);				// смена мира
				addVanger(this,pos_x, pos_y,1); // человек
				uvsWorldReload(Pworld -> gIndex);
				if ( !Pworld -> GamerVisit ){
					ShowDominanceMessage(10);
					GamerResult.dominance += 10;
					if (GamerResult.dominance > 100) GamerResult.dominance = 100;
				}


#ifdef STAND_REPORT
				stand < ConTimer.GetTime();
				stand < "\nTeleport on the World  " < Pworld -> name < "  visit " <= Pworld -> GamerVisit < "\n";
#endif
				status = UVS_VANGER_STATUS::MOVEMENT;
				break;
#endif
			case UVS_VANGER_STATUS::GO_NEW_ESCAVE:
				Pworld -> GamerVisit++;

				if (WorldTable[Pworld -> gIndex]-> escTmax){
					uvsCurrentCycle = WorldTable[Pworld -> gIndex] -> escT[0] -> Pbunch -> currentStage;
					if (Gamer -> Pworld -> escT[0] -> Pbunch -> status == UVS_BUNCH_STATUS::UNABLE)
						uvsCurrentWorldUnable = 1;
					else
						uvsCurrentWorldUnable = 0;
				} else {
					uvsCurrentCycle = 0;
					uvsCurrentWorldUnable = 0;
				}

				ChangeWorld(Pworld -> gIndex, 0);				// смена мира
				if ( !Pworld -> GamerVisit ){
					ShowDominanceMessage(10);
					GamerResult.dominance += 10;
					if (GamerResult.dominance > 100) GamerResult.dominance = 100;
				}
#ifdef STAND_REPORT
				stand < ConTimer.GetTime();
				stand < "\nTeleport on the World  " < Pworld -> name < "  visit " <= Pworld -> GamerVisit < "\n";
#endif
				status = UVS_VANGER_STATUS::SHOPPING;
				locTimer = 1;
				break;
			case UVS_VANGER_STATUS::MOVE_TO_MECHOS:
					ChangeVanger();
					addVanger(this,pos_x, pos_y,1); // человек
					uvsWorldReload(Pworld -> gIndex);
					status = UVS_VANGER_STATUS::MOVEMENT;
					break;
				case UVS_VANGER_STATUS::PASSAGE_GAP:
					if(!--locTimer) makeJump();
					break;
				case UVS_VANGER_STATUS::SHOPPING:
#ifdef _ROAD_
					if(--locTimer) break;
					status = UVS_VANGER_STATUS::IN_SHOP;
					GameQuantReturnValue = RTO_ESCAVE_ID;
#endif
					break;
				}
			break;
		case UVS_VANGER_SHAPE::GAMER_SLAVE:
			break;
		}
}

//znfo event (for bots?)
void uvsVanger::Event(UvsEventType ev){

	switch(ev){
		case UVS_EVENT::THIEF:
			thief_from_escave(Pescave);
			break;
		case UVS_EVENT::THIEF_FROM_SHOP:
			thief_from_shop(Pescave -> Pshop);
			break;
		case UVS_EVENT::GO_FROM_ESCAVE:
			go_from_escave();
			break;
		case UVS_EVENT::GO_FROM_SPOT:
			go_from_spot();
			break;
		case UVS_EVENT::GO_CIRT:
			goCirt();
			break;
		case UVS_EVENT::GO_NEW_ESCAVE:
			if (shape == UVS_VANGER_SHAPE::GAMER)
				status = UVS_VANGER_STATUS::GO_NEW_ESCAVE;
			else
				goNewEscave();
			break;
		case UVS_EVENT::SPOT_ARRIVAL:
			spotArrival();
			if ( shape != UVS_VANGER_SHAPE::GAMER ){
				status = UVS_VANGER_STATUS::SPOT_SLEEPING;
			}else {
				status = UVS_VANGER_STATUS::SHOPPING;
				locTimer = LOC_TIME;
			}
			break;
		case UVS_EVENT::ESCAVE_ARRIVAL:
			homeArrival();
			if ( shape != UVS_VANGER_SHAPE::GAMER ){
				status = UVS_VANGER_STATUS::ESCAVE_SLEEPING;
			} else {
				status = UVS_VANGER_STATUS::SHOPPING;
				locTimer = LOC_TIME;
			}
			break;
		case UVS_EVENT::PASSAGE_ARRIVAL:
			status = UVS_VANGER_STATUS::PASSAGE_GAP;
			if ( shape != UVS_VANGER_SHAPE::GAMER ){
				shiftOrders();
			}
			locTimer = shape == UVS_VANGER_SHAPE::GAMER ? 1 : V_PGAP_TIME;
			break;
		case UVS_EVENT::AIM_ARRIVAL:
			aimHandle();
			break;
		case UVS_EVENT::FATALITY:
			destroy();
			break;
		case UVS_EVENT::GO_NEW_WORLD:
			status = UVS_VANGER_STATUS::GO_NEW_WORLD;

		break;
		};
}

void uvsVanger::go_from_escave(void){
	uvsItem* pi;
	uvsSpot *ps = NULL;
	char *name = NULL;

	addInventory();

	int n = RND(Pescave -> TradeCount);

	if ( Pescave -> TradeCount > 0 ){
		listElem *pl = Pescave -> Ptrade;
		for( int i = 0; i < n; i++) pl = pl -> next;

		if ( shape != UVS_VANGER_SHAPE::THIEF ){
			pi = new uvsItem( ((uvsTradeItem*)pl) -> type );

			if (pi -> type == UVS_ITEM_TYPE::POPONKA){
				pi -> param2 = RND(DG_POPONKA_MAX);
				while(pi -> param2 == uvsPoponkaID)
					pi -> param2 = RND(DG_POPONKA_MAX);
			}
			addItem( pi, 1 );
			pi -> param1 = Pworld -> gIndex<<16;
		}
		name = ((uvsTradeItem*)pl) -> town_name;
	}
	uvsEscave* pe;

	if ( name == NULL ){
		ps = Pworld -> getRandSpot();
		uvsOrder* ord = orderT; 		// первый элемент
		ord -> type = UVS_ORDER::MOVING;
		ord -> event = UVS_EVENT::GO_SPOT;
		ord -> target = UVS_TARGET::SPOT;
		ord -> Ptarget = (uvsTarget*)ps;
		Pspot = ps;
	} else	if ( ps = (uvsSpot*)SpotTail -> seekName(name) ){
		uvsOrder* ord = orderT; 		// первый элемент
		ord -> type = UVS_ORDER::MOVING;
		ord -> event = UVS_EVENT::GO_SPOT;
		ord -> target = UVS_TARGET::SPOT;
		ord -> Ptarget = (uvsTarget*)ps;
		Pspot = ps;
	} else if ( pe = (uvsEscave*)EscaveTail -> seekName(name) ){
		uvsOrder* ord = orderT; 		// первый элемент
		ord -> type = UVS_ORDER::MOVING;
		ord -> event = UVS_EVENT::GO_ESCAVE;
		ord -> target = UVS_TARGET::ESCAVE;
		ord -> Ptarget = (uvsTarget*)pe;
	} else ErrH.Abort("uvsVanger::go_from_escave - go nothing");

	status = UVS_VANGER_STATUS::MOVEMENT;
}

void uvsVanger::thief_from_escave( uvsEscave* pe){
	uvsItem* pi;

	if (!strcmp(pe -> Pworld -> name, "Glorx")) return;
	if ( !(Pmechos -> status & UVS_MECHOS_USED::CARAVAN) ) return;

	pi = (uvsItem*)(pe -> Pitem);

	if ( !pi || pi -> param1) return;

	pi -> param1 = uvsTreasureThief;
	pi = new uvsItem( ((uvsItem*)pi) -> type);
	pi -> param2 = Pworld -> gIndex;
	if ( !addItem( pi, 1 ) ) ErrH.Abort("uvsVanger::thief_from_escave");
#ifdef STAND_REPORT
	stand < "thief from " < pe -> name < "\n";
#endif

	shape = UVS_VANGER_SHAPE::THIEF;
	//go_from_escave();
	goNewEscave();
	//Event(UVS_EVENT::GO_NEW_ESCAVE);
}

void uvsVanger::thief_from_spot( uvsSpot* ps){
	uvsItem* pi;

	if ( !(Pmechos -> status & UVS_MECHOS_USED::CARAVAN) ) return;

	pi = (uvsItem*)(ps -> Pitem);

	if ( !pi || pi -> param1 ) return;

	pi -> param1 = uvsTreasureThief;
	pi = new uvsItem( ((uvsItem*)pi) -> type);
	pi -> param2 = Pworld -> gIndex;

	if ( !addItem( pi, 1 ) ) ErrH.Abort("uvsVanger::thief_from_escave");
#ifdef STAND_REPORT
	stand < "thief from " < ps -> name < "\n";
#endif

	shape = UVS_VANGER_SHAPE::THIEF;
	//go_from_escave();
	goNewEscave();
	//Event(UVS_EVENT::GO_NEW_ESCAVE);
}

void uvsVanger::thief_from_shop( uvsShop* ps){
	uvsItem* pi;
	if ( ps -> locked ) return;
	if ( !(Pmechos -> status & UVS_MECHOS_USED::CARAVAN) ) return;

	pi = GetItem_as_type( ps -> Pitem,  UVS_ITEM_STATUS::TREASURE);

	if ( !pi ) return;

	listElem* tmp = (listElem*)ps -> Pitem;
	
	//((uvsItem*)pi) -> delink( tmp );
	pi->delink( tmp );
	if ( !addItem( pi, 1 ) ) ErrH.Abort("uvsVanger::thief_from_shop");

	shape = UVS_VANGER_SHAPE::THIEF;
	goNewEscave();
	//Event(UVS_EVENT::GO_NEW_ESCAVE);
}

void uvsVanger::go_from_spot(void){
	int i, n;
	uvsItem* pi;
	char* name = NULL;

	addInventory();

	n = RND(Pspot -> TradeCount);
	if ( Pspot -> TradeCount > 0 ){
		listElem *pl = Pspot -> Ptrade;
		for( i = 0; i < n; i++) pl = pl -> next;

		if ( shape != UVS_VANGER_SHAPE::THIEF ){
			pi = new uvsItem( ((uvsTradeItem*)pl) -> type );

			if (pi -> type == UVS_ITEM_TYPE::POPONKA){
				pi -> param2 = RND(DG_POPONKA_MAX);
				while(pi -> param2 == uvsPoponkaID)
					pi -> param2 = RND(DG_POPONKA_MAX);
			}

			pi -> param1 = Pworld -> gIndex<<16;
			addItem( pi, 1 );
		}
		name = ((uvsTradeItem*)pl) -> town_name;
	}
	uvsSpot* ps;
	uvsEscave* pe;

	if ( name == NULL ){
		pe = Pworld -> escT[0];
		uvsOrder* ord = orderT; 		// первый элемент
		ord -> type = UVS_ORDER::MOVING;
		ord -> event = UVS_EVENT::GO_ESCAVE;
		ord -> target = UVS_TARGET::ESCAVE;
		ord -> Ptarget = (uvsTarget*)pe;
		Pescave = pe;
	} else	if ( ps = (uvsSpot*)SpotTail -> seekName(name) ){
		uvsOrder* ord = orderT; 		// первый элемент
		ord -> type = UVS_ORDER::MOVING;
		ord -> event = UVS_EVENT::GO_SPOT;
		ord -> target = UVS_TARGET::SPOT;
		ord -> Ptarget = (uvsTarget*)ps;
	} else if ( pe = (uvsEscave*)EscaveTail -> seekName(name) ){
		uvsOrder* ord = orderT; 		// первый элемент
		ord -> type = UVS_ORDER::MOVING;
		ord -> event = UVS_EVENT::GO_ESCAVE;
		ord -> target = UVS_TARGET::ESCAVE;
		ord -> Ptarget = (uvsTarget*)pe;
		Pescave = pe;
	} else ErrH.Abort("uvsVanger::go_from_spot - go nothing");
	status = UVS_VANGER_STATUS::MOVEMENT;
}

int uvsVanger::buildWay(uvsWorld* fromW,uvsWorld* toW){
	uvsWorld* nextW,*tmpW = NULL;
	uvsWorld *lastfromW = NULL, *lasttoW = NULL;
	uvsOrder* ord;
	uvsPassage* pp;
	int i = 0;

	if (fromW -> gIndex >= MAIN_WORLD_MAX ) {
		lastfromW = fromW;
		fromW = fromW -> pssT[0] -> Poutput;

		ord = orderT;
		ord -> type = UVS_ORDER::MOVING;
		ord -> event = UVS_EVENT::GO_PASSAGE;
		ord -> target = UVS_TARGET::PASSAGE;
		ord -> Ptarget = (uvsTarget*)lastfromW -> pssT[0];
		i = 1;
	}

	if (toW -> gIndex >= MAIN_WORLD_MAX ) {
		lasttoW = toW;
		toW = toW -> pssT[0] -> Poutput;
	}

	unsigned int linkage = ChainMap[toW -> gIndex + fromW -> gIndex*MAIN_WORLD_MAX],mI;

	if ( fromW == toW ) return 0;

	for(; i < 8; i++){
		mI = (linkage & (0x0000000F << 4*i)) >> 4*i;
		nextW = getWorld(mI);

		if(!nextW)	nextW = toW;
		pp = fromW -> getPassage(nextW);

		if ( !pp ) ErrH.Abort( "buildWay:: not passage");

		ord = orderT + i;
		ord -> type = UVS_ORDER::MOVING;
		ord -> event = UVS_EVENT::GO_PASSAGE;
		ord -> target = UVS_TARGET::PASSAGE;
		ord -> Ptarget = (uvsTarget*)pp;
		fromW = nextW;

		if ( fromW == toW ) break;
	}

	i++;
	if ( lasttoW ){
		pp = fromW -> getPassage(lasttoW);
		ord = orderT + i;
		ord -> type = UVS_ORDER::MOVING;
		ord -> event = UVS_EVENT::GO_PASSAGE;
		ord -> target = UVS_TARGET::PASSAGE;
		ord -> Ptarget = (uvsTarget*)pp;
		i++;
	}

	return (i);
}

void uvsVanger::goCirt(void){
	uvsItem* pi;
	uvsDolly* pdl = owner -> cycleTable[owner -> currentStage].Pdolly;

	if ( !(Pmechos -> status & UVS_MECHOS_USED::HARVEST) ) return;

	if ( pdl -> Pworld -> gIndex > 3 ) return;

	addInventory();

	if (pdl -> status == UVS_DOLLY_STATUS::CATCH){
		if (!RND(10) && uvsKillerNow() < 2)
			go_to_GAMER();
		else return;
	} else {
		pi = new uvsItem(UVS_ITEM_TYPE::CIRTAINER);
		if (!addItem(pi, 0)) {
			delete pi;
			return;
		}

		uvsWorld* fromW = Pworld;
		uvsWorld* toW = pdl -> Pworld;
		int offset;
		uvsOrder* ord;

		offset = buildWay(fromW,toW);

		ord = orderT + offset;
		ord -> type = UVS_ORDER::CIRT_GATHERING;
		ord -> event = UVS_EVENT::GO_CIRT;
		ord -> target = UVS_TARGET::DOLLY;
		ord -> Ptarget = (uvsTarget*)pdl;

		status = UVS_VANGER_STATUS::MOVEMENT;
	}
	//cirtStage = owner -> currentStage;
}

void uvsVanger::goHome(void){
	uvsWorld* fromW = Pworld;
	uvsWorld* toW = owner -> Pescave -> Pworld;
	uvsOrder* ord;
	int offset;

	offset = buildWay(fromW, toW);

	ord = orderT + offset;
	ord -> type = UVS_ORDER::MOVING;
	ord -> event = UVS_EVENT::GO_ESCAVE;
	ord -> target = UVS_TARGET::ESCAVE;
	ord -> Ptarget = (uvsTarget*)(owner -> Pescave);

	if ( !owner -> Pescave )
		ErrH.Abort("Error");

	status = UVS_VANGER_STATUS::MOVEMENT;
}

int uvsVanger::prepare_killer(void){

	if ( !(Pescave -> Pbunch -> status & ((int)(UVS_BUNCH_STATUS::KILL_GAMER))) ) return 0;

	if (uvsKillerNow() >= 2) return 0;

	if ( status == UVS_VANGER_STATUS::ESCAVE_SLEEPING ){
		Pescave -> Pshop -> sellMechos(Pmechos, (int)(UVS_MECHOS_USED::KILLER));
		Pmechos -> color = Pescave -> Pbunch -> biosNindex;

		if (Pmechos -> status & ((int)UVS_MECHOS_USED::KILLER)){
			shape = UVS_VANGER_SHAPE::KILLER;
			Pescave -> Pbunch -> status ^= (int)(UVS_BUNCH_STATUS::KILL_GAMER);
			addInventory();
			go_to_GAMER();
			return 1;
		}
	}
	return 0;
}

void uvsVanger::prepare_caravan(void){
	uvsShop *ps;
	uvsItem *pi;

	if ( status == UVS_VANGER_STATUS::ESCAVE_SLEEPING ){
		ps = Pescave -> Pshop;
	} else {
		ps = Pspot -> Pshop;
	}

	if ((Pmechos -> status & UVS_MECHOS_USED::CARAVAN) && (Pworld -> escT[0] -> Pbunch -> status != UVS_BUNCH_STATUS::UNABLE)){
		if ((pi = GetItem_as_type( ps -> Pitem,  UVS_ITEM_STATUS::ARTIFACT)) != NULL ) {
			((uvsItem*)pi) -> delink(  ps -> Pitem );
			addItem(pi, 1);
			shape = UVS_VANGER_SHAPE::CARAVAN;
		} else	if ((pi = GetItem_as_type( ps -> Pitem,  UVS_ITEM_STATUS::MECHOS_PART)) != NULL ) {
			((uvsItem*)pi) -> delink(  ps -> Pitem );
			addItem(pi, 1);
			shape = UVS_VANGER_SHAPE::CARAVAN;
		}
	}
	addInventory();
}

void uvsVanger::goNewEscave(void){
	if ( shape != UVS_VANGER_SHAPE::THIEF && shape != UVS_VANGER_SHAPE::TABUTASK){
		if (!prepare_killer() && !RND(10))
			prepare_caravan();
	} else {
		addInventory();
	}

	if ( shape == UVS_VANGER_SHAPE::KILLER ) return;

	uvsWorld* fromW = Pworld;
	uvsWorld* toW;
	do{
		toW= getWorld(getRW(fromW -> gIndex));
	}while(!toW -> escTmax);

	int offset;
	uvsOrder* ord;

	offset = buildWay(fromW,toW);

	ord = orderT + offset;
	ord -> type = UVS_ORDER::MOVING;
	ord -> event = UVS_EVENT::GO_ESCAVE;
	ord -> target = UVS_TARGET::ESCAVE;
	ord -> Ptarget = (uvsTarget*)(toW -> escT[0]);

	if ( !toW -> escT[0] )
		ErrH.Abort("Error");

	status = UVS_VANGER_STATUS::MOVEMENT;
}

#ifdef _ROAD_
extern int Quit;
extern int Dead;
#endif

int uvsVanger::makeJump(void){
	if(shape == UVS_VANGER_SHAPE::GAMER){
#ifdef _ROAD_
		EffectsOff();

		uvsWorld* aim = Ppassage -> Poutput;
		//Quit = 0;
		//Dead = 0;

//		ErrH.Abort("Limited version. Only one world available...");

		Ppassage = aim -> getPassage(Pworld);
		Pworld = aim;

		/*{
			Pworld -> getItemFromCrypt(UVS_CRYPT::DEVICE);
			Pworld -> getItemFromCrypt(UVS_CRYPT::AMMO);
			Pworld -> getItemFromCrypt(UVS_CRYPT::ARMO_LIGHT);
			Pworld -> getItemFromCrypt(UVS_CRYPT::ARMO_HEAVY);
		} else {
			Pworld -> updateCrypt();
		}*/

		Pworld -> GamerVisit++;

		if (WorldTable[aim -> gIndex]-> escTmax){
			uvsCurrentCycle = WorldTable[aim -> gIndex] -> escT[0] -> Pbunch -> currentStage;
			if (Gamer -> Pworld -> escT[0] -> Pbunch -> status == UVS_BUNCH_STATUS::UNABLE)
				uvsCurrentWorldUnable = 1;
			else
				uvsCurrentWorldUnable = 0;
		} else {
			uvsCurrentCycle = 0;
			uvsCurrentWorldUnable = 0;
		}

		if (WorldTable[aim -> gIndex] -> escTmax)
			aci_curLocationName = WorldTable[aim -> gIndex] -> escT[0] -> name;
		else
			aci_curLocationName = "";

		ChangeWorld(aim -> gIndex);				 // смена мира
		addVanger(this,Ppassage,1);					// человек
		uvsWorldReload(aim -> gIndex);

		if ( !Pworld -> GamerVisit ){
			ShowDominanceMessage(10);
			GamerResult.dominance += 10;
			if (GamerResult.dominance > 100) GamerResult.dominance = 100;
		}

#ifdef STAND_REPORT
	stand < ConTimer.GetTime();
	stand < "\nTeleport on the World  " < Pworld -> name < "  visit " <= Pworld -> GamerVisit < "\n";
#endif

#endif
		}
	else {

		uvsOrder* ord = orderT;
		uvsWorld* aim = Ppassage -> Poutput;

		if ( aim -> gIndex >= MAIN_WORLD_MAX) ErrH.Abort("uvsVanger::makeJump - it not real world");

		uvsPassage* pp = aim -> getPassage(Pworld);

		if(pp -> locked || aim -> locked) return 0;
		uvsCheckVangerTabuTask(this,uvsVANGER_ARRIVAL);

		if ( Pmechos -> color < 3 && 
		    WorldTable[Pmechos -> color] -> escT[0] -> Pbunch -> status == UVS_BUNCH_STATUS::UNABLE && 
		     shape != UVS_VANGER_SHAPE::GAMER_SLAVE){
				 Pmechos -> color = 3;
				if( shape == UVS_VANGER_SHAPE::BUNCH_SLAVE)
					shape = UVS_VANGER_SHAPE::RANGER;
		}

		Pworld = aim;
		Ppassage = pp;
		pos_x = pp -> pos_x;
		pos_y = pp -> pos_y;

		if ( shape == UVS_VANGER_SHAPE::KILLER )
			go_to_GAMER();

		switch(ord -> event){
			case UVS_EVENT::GO_ESCAVE:
				status = UVS_VANGER_STATUS::MOVEMENT;
				Pescave = (uvsEscave*)(ord -> Ptarget);
				break;
			case UVS_EVENT::GO_SPOT:
				status = UVS_VANGER_STATUS::MOVEMENT;
				Pspot = (uvsSpot*)(ord -> Ptarget);
				break;
			case UVS_EVENT::GO_PASSAGE:
				status = UVS_VANGER_STATUS::MOVEMENT;
				break;
			case UVS_EVENT::GO_CIRT:
				status = UVS_VANGER_STATUS::MOVEMENT;
				ord -> event = UVS_EVENT::GO_CIRT;
				break;
				}
		speed = 3;
		if ( shape !=  UVS_VANGER_SHAPE::THIEF)
			speed += RND(V_SPEED_MAX);
		}
	return 1;
}

void uvsVanger::aimHandle(void){
	uvsOrder* ord = orderT;
	switch(ord -> type){
		case UVS_ORDER::CIRT_GATHERING:
			gatherCirt((uvsDolly*)(ord -> Ptarget));
			shiftOrders();
			goHome();
			break;
		}
}

void uvsVanger::gatherCirt(uvsDolly* dolly){
	uvsItem* pi = GetItem(Pitem, UVS_ITEM_TYPE::CIRTAINER, 1);

	if( !pi ) return;

	pi -> param1 = dolly -> getCirt(pos_x, pos_y) | (1<<31);
	pi -> param2 = 1 << (dolly -> gIndex);
}

void uvsVanger::gatherCirt(int x, int y, int& p1, int& p2){
	listElem* pe = DollyTail;

#ifdef STAND_REPORT
	stand < "Gamer gather \n";
#endif
	p1 = p2 = 0;

	while( pe ){
		if ( (( uvsDolly*)pe) -> Pworld -> gIndex == CurrentWorld ){
			p1 |= (( uvsDolly*)pe) -> getCirt(x, y);
			p2 |= 1 << ((( uvsDolly*)pe) -> gIndex);
		}
		pe = pe -> next;
	}//  end while

	p1 |= 1<<31;
#ifdef STAND_REPORT
	stand < "Gamer gather end\n";
#endif
}

int uvsVanger::sellCirt(void){
	int  i = 0;
	int have_full = 0;
	int tmp_good = 0;
	int tmp_bad = 0;

	int tmp_total_good = 0;
	int tmp_total_bad = 0;

	int last_param2;
	int cool_index = 0;
	uvsActInt* pa = GGamer;

	uvsItem *pi;
	uvsBunch *pb = Pescave -> Pbunch;

	pi = GetItem(Pitem, UVS_ITEM_TYPE::CIRTAINER, 0);
	GamerResult.cirt_bad = -1;
	GamerResult.cirt_good = -1;

	while( pi ){
		tmp_good = 0;
		tmp_bad = 0;
		last_param2  = pi -> param2;

		if (pi -> param2){
			have_full = 1;

			for ( i = 0; i < pb -> cycleN; i++){
				//if( ((1 << (pb -> cycleTable[i].Pdolly -> gIndex)) & pi -> param2) && ((pi -> param1 >> (pb -> cycleTable[i].Pdolly -> gIndex*2)) & 3)) {
				if( ((1 << (pb -> cycleTable[i].Pdolly -> gIndex)) & last_param2)) {
					if (tmp_good < ((pi -> param1 >> (pb -> cycleTable[i].Pdolly -> gIndex*3)) & 7)){
						tmp_good = ((pi -> param1 >> (pb -> cycleTable[i].Pdolly -> gIndex*3)) & 7);
						cool_index = pb -> cycleTable[i].Pdolly -> gIndex;
					}
					last_param2 ^= (1 << (pb -> cycleTable[i].Pdolly -> gIndex));
				}
			}//  end for j

			if (last_param2)
				for ( i = 1; i < DollyIndex; i++){
					if( (1 << i) & last_param2) {
						if ( tmp_bad < ((pi -> param1 >> (i*3)) & 7)  )
							tmp_bad =  ((pi -> param1 >> (i*3)) & 7);
					}
				}


			if (tmp_good >= tmp_bad && tmp_good > 0 && !dgAbortStatus ){
				tmp_good = 0;

	//			if(cool_index == -1) ErrH.Abort("uvsVanger::sellCirt - good cirt not found but it have");

				if ( cool_index == pb -> cycleTable[pb -> currentStage].Pdolly -> gIndex ){
						tmp_good += 2*((pi -> param1 >> (cool_index*3)) & 7);
				} else {
						tmp_good += ((pi -> param1 >> (cool_index*3)) & 7);
				}

				for ( i = 0; i < pb -> cycleN; i++)
					if ( cool_index == pb -> cycleTable[i].Pdolly -> gIndex ) break;

				pb -> cycleTable[i].cirtQ += (((pi -> param1 >> (cool_index*3)) & 7));

				if (pb -> cycleTable[i].cirtQ >= pb -> cycleTable[i].cirtMAX){
					uvsCheckKronIventTabuTask(UVS_KRON_EVENT::END_CYCLE, i+1);
				}

				pb -> cycleTable[i].GamerCirt += (((pi -> param1 >> (cool_index*3)) & 7));
				tmp_total_good += tmp_good;

				uvs_aciChangeOneItem(uvsItemTable[UVS_ITEM_TYPE::CIRTAINER] -> SteelerTypeFull, uvsItemTable[UVS_ITEM_TYPE::CIRTAINER] -> SteelerTypeEmpty, pi -> pos_x, pi -> pos_y);

				pa = GGamer;
				while( pa && ((pi -> pos_x != pa -> pos_x) || (pi -> pos_y != pa -> pos_y))){
					pa = (uvsActInt*)pa -> next;
				}
				if (pa){
					pa -> type = ACI_EMPTY_CIRTAINER;
					pa -> param1 = 0;
					pa -> param2 = 0;
				}

				pi -> param1 = 0;
				pi -> param2 = 0;
			} else {
				tmp_total_bad += tmp_bad;

				if (tmp_bad > 0)
					uvsCheckKronIventTabuTask(UVS_KRON_EVENT::BAD_CIRT, tmp_bad);
			}
		}

		pi = GetItem(pi->next, UVS_ITEM_TYPE::CIRTAINER, 0 );
	}
#ifdef _ROAD_
	if (have_full){
		GamerResult.cirt_good = tmp_total_good;
		GamerResult.cirt_bad = tmp_total_bad;
	}

#ifdef STAND_REPORT
	stand < "cirt good "<= GamerResult.cirt_good < "  cirt bad " <= GamerResult.cirt_bad < "\n";
#endif

#endif
	if (GamerResult.cirt_good > 0)
		return GamerResult.cirt_good;
	else
		return 0;
}

void uvsVanger::_sellCirt_(void){
	int  i = 0;
	int have_full = 0;
	int tmp_good = 0;
	int tmp_bad = 0;

	int tmp_total_good = 0;
	int tmp_total_bad = 0;

	int last_param2;
	int cool_index = 0;
	uvsActInt* pa = GGamer;

	uvsItem *pi;

	if (!Pworld -> escTmax) return;

	uvsBunch *pb = Pworld -> escT[0] -> Pbunch;

	pi = GetItem(Pitem, UVS_ITEM_TYPE::CIRTAINER, 0);
	GamerResult.cirt_bad = -1;
	GamerResult.cirt_good = -1;

	while( pi ){
		tmp_good = 0;
		tmp_bad = 0;
		last_param2  = pi -> param2;

		if (pi -> param2){
			have_full = 1;

			for ( i = 0; i < pb -> cycleN; i++){
				//if( ((1 << (pb -> cycleTable[i].Pdolly -> gIndex)) & pi -> param2) && ((pi -> param1 >> (pb -> cycleTable[i].Pdolly -> gIndex*2)) & 3)) {
				if( ((1 << (pb -> cycleTable[i].Pdolly -> gIndex)) & last_param2)) {
					if (tmp_good < ((pi -> param1 >> (pb -> cycleTable[i].Pdolly -> gIndex*3)) & 7)){
						tmp_good = ((pi -> param1 >> (pb -> cycleTable[i].Pdolly -> gIndex*3)) & 7);
						cool_index = pb -> cycleTable[i].Pdolly -> gIndex;
					}
					last_param2 ^= (1 << (pb -> cycleTable[i].Pdolly -> gIndex));
				}
			}//  end for j

			if (last_param2)
				for ( i = 1; i < DollyIndex; i++){
					if( (1 << i) & last_param2) {
						if ( tmp_bad < ((pi -> param1 >> (i*3)) & 7)  )
							tmp_bad =  ((pi -> param1 >> (i*3)) & 7);
					}
				}


			if (tmp_good >= tmp_bad && tmp_good > 0){
				tmp_good = 0;

	//			if(cool_index == -1) ErrH.Abort("uvsVanger::sellCirt - good cirt not found but it have");

				if ( cool_index == pb -> cycleTable[pb -> currentStage].Pdolly -> gIndex ){
						tmp_good += 2*((pi -> param1 >> (cool_index*3)) & 7);
				} else {
						tmp_good += ((pi -> param1 >> (cool_index*3)) & 7);
				}

				tmp_total_good += tmp_good;
			} else {
				tmp_total_bad += tmp_bad;

//				if (tmp_bad > 0)
//					uvsCheckKronIventTabuTask(UVS_KRON_EVENT::BAD_CIRT, tmp_bad);
			}
		}

		pi = GetItem( pi -> next, UVS_ITEM_TYPE::CIRTAINER, 0 );
	}
#ifdef _ROAD_
	if (have_full){
		GamerResult.cirt_good = tmp_total_good;
		GamerResult.cirt_bad = tmp_total_bad;
	}

#ifdef STAND_REPORT
	stand < "cirt good "<= GamerResult.cirt_good < "  cirt bad " <= GamerResult.cirt_bad < "\n";
#endif

#endif
}

int uvsVanger::checkCycle(int stage){
	if (!Pworld -> escTmax) return 0;

	uvsBunch *pb = Pworld -> escT[0] -> Pbunch;

	if (pb -> cycleTable[stage].Pdolly -> status == UVS_DOLLY_STATUS::UNABLE)
		return 0;
	else
		return 1;
}

// ec - первый символ имени мира; если ec = 0 то проверять текущий мир как
//сейчас
int isCycleExist(int cycle,char ew){
	if(!ew)
		return Gamer -> checkCycle(cycle);
	else {
		uvsWorld *pw = (uvsWorld*)WorldTail;


		while(pw && pw -> name[0] != ew ){
			pw = (uvsWorld*)pw -> next;
		}

		if (!pw) ErrH.Abort("isCycleExist :: Incorrect world name");

		if (!pw -> escTmax) return 0;

		uvsBunch *pb = pw -> escT[0] -> Pbunch;

		if (pb -> cycleTable[cycle].Pdolly -> status == UVS_DOLLY_STATUS::UNABLE)
			return 0;
		else
			return 1;
	}
}

int getPreviousCycle(int cycle){
	uvsBunch *pb = Gamer -> Pworld -> escT[0] -> Pbunch;

	int i = cycle - 1;
	int _i = 0;

	if( i < 0 ) i = pb -> cycleN - 1;

	while( pb -> cycleTable[i].Pdolly -> status == UVS_DOLLY_STATUS::UNABLE  ){
		if( --i < 0 ) i = pb -> cycleN - 1;

		_i++;
		if (_i > pb -> cycleN) {
			return -1;
		}
	}//  end while
	return i;
}

//znfo larva aka dolly
void uvsVanger::checkDolly(void){
	int i;
	uvsItem *pi;
	pi = GetItem(Pitem, UVS_ITEM_TYPE::CONLARVER, 0);

	GamerResult.dolly = 0;
	GamerResult.dolly_cycle = -1;
	if (!pi || !pi -> param1) return;

	GamerResult.dolly = UVS_DOLLY_TYPE::ALIEN;
	if (Pworld -> escTmax) {

		uvsBunch *pb = Pworld -> escT[0] -> Pbunch;

		for( i = 0; i < pb -> cycleN; i++){
			if (pb -> cycleTable[i].Pdolly -> gIndex == pi -> param1){
				GamerResult.dolly = UVS_DOLLY_TYPE::OWN;
				GamerResult.dolly_cycle = i;
#ifdef STAND_REPORT
				stand < "dolly from cycle  "<= i  < "\n";
#endif
				if (Pescave){
					aciUpdateCurCredits(aciGetCurCredits() + CASH_FOR_DOLLY);
					pi -> param1 = 0;
					pi -> param2 = 0;

					GamerResult.luck += 10;
					ShowLuckMessage(10);
					if (GamerResult.luck > 100) GamerResult.luck = 100;

					SOUND_END_LARVA()
//					uvs_aciChangeItem(uvsItemTable[UVS_ITEM_TYPE::CONLARVER] -> SteelerTypeFull, uvsItemTable[UVS_ITEM_TYPE::CONLARVER] -> SteelerTypeEmpty);
					uvs_aciKillOneItem( uvsItemTable[UVS_ITEM_TYPE::CONLARVER] -> SteelerTypeFull, pi -> pos_x,  pi -> pos_y);
					pb -> cycleTable[i].Pdolly -> status = UVS_DOLLY_STATUS::UNABLE;

					pi -> delink(Pitem);
					delete pi;

					pb -> Quant();
				}//  end if
				break;
			}//  end if
		}//  end for

	}//  end if

#ifdef STAND_REPORT
	stand < "dolly  "<= GamerResult.dolly  < "\n";
#endif
}

void uvsVanger::_checkDolly_(void){
	int i;
	uvsItem *pi;
	pi = GetItem(Pitem, UVS_ITEM_TYPE::CONLARVER, 0);

	GamerResult.dolly = 0;
	GamerResult.dolly_cycle = -1;
	if (!pi || !pi -> param1) return;

	GamerResult.dolly = UVS_DOLLY_TYPE::ALIEN;
	if (Pworld -> escTmax) {
		uvsBunch *pb = Pworld -> escT[0] -> Pbunch;

		for( i = 0; i < pb -> cycleN; i++){
			if (pb -> cycleTable[i].Pdolly -> gIndex == pi -> param1){
				GamerResult.dolly = UVS_DOLLY_TYPE::OWN;
				GamerResult.dolly_cycle = i;
#ifdef STAND_REPORT
				stand < "dolly from cycle  "<= i  < "\n";
#endif
				break;
			}//  end if
		}//  end for
	}//  end if

#ifdef STAND_REPORT
	stand < "dolly  "<= GamerResult.dolly  < "\n";
#endif
}

int uvsDolly::getCirt(int x,int y){
	int i;
	int Range = 32000;
	int RangeL;
	int tmp;

	if (status) return 0;

#ifdef STAND_REPORT
	stand < "dolly  index"<= gIndex  < "\n";
	stand < "dolly  bois"<= biosNindex  < "\n\n";
#endif
	for( i = 0; i < DOLLY_TAIL_LEN; i++){
		RangeL = int(sqrt( double((tail_x[i] - x)*(tail_x[i] - x) + (tail_y[i] - y)*(tail_y[i] - y))));

		if ( RangeL < Range ) {
			Range = RangeL;
		}
	}

	if ( Range >= V_CIRT_R) 	//  4096
		tmp = 0;
	else if ( Range >= V_CIRT_R/2)	// 2048
		tmp = 1;
	else if ( Range >= V_CIRT_R/4)	//  1024
		tmp = 2;
	else if ( Range >= V_CIRT_R/6)	// 700
		tmp = 3;
	else if ( Range >= V_CIRT_R/8) //  512
		tmp = 4;
	else if ( Range >= V_CIRT_R/16) // 256
		tmp = 5;
	else if ( Range >= V_CIRT_R/40) // 100
		tmp = 6;
	else
		tmp = 7;

	return (tmp << (gIndex*3));
}

int uvsVanger::emptyContainer( uvsEscave* pe ){
	uvsItem *pi = (uvsItem*)Pitem;
	int tmp = 0;
	int tmp_p = 0;
	int out = 0;

	if (!pe) ErrH.Abort("uvsVanger:: empty container dont have Escave");

	while( !out ){
		out = 1;
		pi = GetItem(Pitem, UVS_ITEM_TYPE::CIRTAINER, 0);
		if( pi ){
			tmp += pe -> Pbunch -> addCirt(pi);
			((uvsItem*)pi) -> delink(Pitem);
			delete (uvsItem*)pi;
			out = 0;
		}
	}
	return tmp;
}

int uvsBunch::addCirt(uvsItem* pi){
	int  i = 0;
	int tmp = 0;

	for ( i = 0; i < cycleN; i++){
		if( (1 << cycleTable[i].Pdolly -> gIndex) & pi -> param2) {
			tmp += ((pi -> param1 >> (cycleTable[i].Pdolly -> gIndex*3)) & 7);
			cycleTable[i].cirtQ += ((pi -> param1 >> (cycleTable[i].Pdolly -> gIndex*3)) & 7);
		}
	}//  end for j
	return tmp;
}

void uvsActInt::delink(uvsActInt*& pe){
	if( pe == ((uvsActInt*)this) ) {
		pe = (uvsActInt*)this -> next;
	}
	if(prev -> next) {
		prev -> next = next;
	}
	if(next){
		next -> prev = prev;
	} else if ( pe ) {
		pe -> prev = prev;
	}
	if( pe ) {
		pe -> prev -> next  = NULL;
	}
}

void uvsActInt::save(XStream& fout){
	fout < pos_x;
	fout < pos_y;
	fout < type;
	fout < price;
	fout < sell_price;
	fout < param1;
	fout < param2;
}

void uvsActInt::load(XStream& fout, int what){
	fout > pos_x;
	fout > pos_y;
	fout > type;
	fout > price;
	fout > sell_price;

	if ((uvsLoadVersion < 3)&&what&&(uvsItemTable[ActInt_to_Item(type)] -> type == UVS_ITEM_STATUS::DEVICE)){
		fout > param2;
		fout > param1;
	} else {
		fout > param1;
		fout > param2;
	}
}

void uvsItem::delink(listElem*& pe){
	if( pe == ((listElem*)this) ) pe = ((listElem*)this) -> next;
	if(prev -> next) prev -> next = next;
	if(next)
		next -> prev = prev;
	else if ( pe )
		pe -> prev = prev;
	if( pe ) pe -> prev -> next  = NULL;
}

void uvsMechos::delink(listElem*& pe){
	if( pe == ((listElem*)this) ) pe = ((listElem*)this) -> next;
	if(prev -> next) prev -> next = next;
	if(next)
		next -> prev = prev;
	else if ( pe )
		pe -> prev = prev;
	if( pe ) pe -> prev -> next  = NULL;
}

void FreeList(listElem*& pe){
	uvsItem* pi = (uvsItem*)pe;

	while(pe){
		pe = pe -> next;
		delete (uvsItem*)pi;
		pi = (uvsItem*)pe;
	}
	pe = NULL;
}

/*void FreeMechosList(uvsMechosList*& pe){
	uvsMechosList* pi = (uvsMechosList*)pe;

	while(pe){
		pe = (uvsMechosList*)pe -> next;
		delete (uvsMechosList*)pi;
		pi = (uvsMechosList*)pe;
	}
	pe = NULL;
} */

void FreeFarmer(uvsElement*& pe){
	uvsFlyFarmer* pi = (uvsFlyFarmer*)pe;

	if (CurrentWorld != 1 ) return;

	while(pe){
		pi = (uvsFlyFarmer*)pe;
		pe = pe -> enext;
		if ( pi -> type == UVS_OBJECT::FLY_FARMER ){
			WorldTable[CurrentWorld] -> remove_target( (uvsTarget*)((uvsFlyFarmer*)pi), UVS_TARGET::FLY_FARMER);
//			pi -> delink(pe);
			delete (uvsFlyFarmer*)pi;
		}//  end if
	}// end while

	pe = NULL;
}

void uvsVanger::homeArrival(){
	int tmp;

	uvsCheckVangerTabuTask(this,uvsVANGER_ARRIVAL);

	Pspot = NULL;

	switch(shape){
		case UVS_VANGER_SHAPE::TABUTASK:
			tmp = Pescave -> bring_game_GOODS(Pitem);
#ifdef STAND_REPORT
			if (tmp){
				stand < " race finish tabutask\n";
			}
#endif
			Pescave->Pshop->updateList(Pitem);
			FreeList(Pitem);
			break;
		case UVS_VANGER_SHAPE::BUNCH_SLAVE:
			tmp = Pescave -> bring_game_GOODS(Pitem);
#ifdef STAND_REPORT
			if (tmp){
				stand < " race finish slave\n";
			}
#endif
			//changeRating( Pescave -> bring_GOODS(Pitem) );
			changeRating( tmp );

			if(!emptyContainer(Pescave)) changeRating(V_CIRTDELIVERY_SCORE);
			else changeRating(V_LOCTRIP_SCORE);
			if (RND(2))
				Pescave->Pshop->updateList(Pitem);
			FreeList(Pitem);
			break;
		case UVS_VANGER_SHAPE::CARAVAN:
			tmp = Pescave -> bring_game_GOODS(Pitem);
#ifdef STAND_REPORT
			if (tmp){
				stand < " race finish caravan\n";
			}
#endif
			//changeRating( Pescave -> bring_GOODS(Pitem) );
			changeRating( tmp );

			if(!emptyContainer(Pescave)) changeRating(V_LONGTRIP_SCORE);

			if ( !RND(3) ){
					sell_move_Item( Pescave -> Pshop, 0);
				}
			break;
		case UVS_VANGER_SHAPE::RANGER:
			tmp = Pescave -> bring_game_GOODS(Pitem);
#ifdef STAND_REPORT
			if (tmp){
				stand < " race finish ranger\n";
			}
#endif
			//changeRating( Pescave -> bring_GOODS(Pitem) );
			changeRating( tmp );

			if(!emptyContainer(Pescave)) changeRating(V_LONGTRIP_SCORE);
			Pescave->Pshop->updateList(Pitem);
			FreeList(Pitem);

			if( !RND(2)){
				// становиться рабом
				shape = UVS_VANGER_SHAPE::BUNCH_SLAVE;
				owner = Pworld -> escT[0] -> Pbunch;
				if (Pmechos)
					Pmechos -> color = Pworld -> escT[0] -> Pbunch -> biosNindex;
				}
			else
				if( !RND(2)) Event(UVS_EVENT::FATALITY);
			break;
		case UVS_VANGER_SHAPE::THIEF:
			if ( !Pescave -> arrest( this) ){
				tmp = Pescave -> bring_game_GOODS(Pitem);
#ifdef STAND_REPORT
				if (tmp){
					stand < " race finish thief\n";
				}
#endif
				//changeRating( Pescave -> bring_GOODS(Pitem) );
				changeRating( tmp );

				if(!emptyContainer(Pescave)) changeRating(V_LONGTRIP_SCORE);

				if ( !RND(3) ){
					sell_treasure_Item( Pescave -> Pshop);
				}
			}
			break;
		case UVS_VANGER_SHAPE::GAMER:
#ifdef _ROAD_
			status = UVS_VANGER_STATUS::SHOPPING;
			locTimer = LOC_TIME;

			if ( !Pescave -> GamerVisit ){
				GamerResult.dominance += 2;
				ShowDominanceMessage(2);
				if (GamerResult.dominance > 100) GamerResult.dominance = 100;
			}
			Pescave -> GamerVisit++;
#endif
			break;
		}
}

void uvsVanger::sell_treasure_Item( uvsShop* ps ){
	if ( ps -> locked ) return;

	uvsItem* pi = GetItem_as_type( Pitem,  UVS_ITEM_STATUS::TREASURE);
	if ( !pi )	{
		shape = UVS_VANGER_SHAPE::RANGER;
		return;
	}

	((uvsItem*)pi) -> delink(  Pitem );
	ps -> addItem(pi);

	if ( !GetItem_as_type( Pitem,  UVS_ITEM_STATUS::TREASURE) ) shape = UVS_VANGER_SHAPE::RANGER;
}

void uvsVanger::sell_move_Item( uvsShop* ps, int where ){
	uvsItem* pi = NULL;
	if ( ps -> locked ) return;

	if (where)
		pi = GetItem_as_type( Pitem,  UVS_ITEM_STATUS::MECHOS_PART);

	if ( !pi ) pi = GetItem_as_type( Pitem,  UVS_ITEM_STATUS::ARTIFACT);

	if ( !pi ){
		pi = GetItem_as_type( Pitem,  UVS_ITEM_STATUS::MECHOS_PART);

		if (!pi)
			shape = UVS_VANGER_SHAPE::RANGER;
		return;
	}

	((uvsItem*)pi) -> delink(  Pitem );
	ps -> addItem(pi);

	pi = GetItem_as_type( Pitem,  UVS_ITEM_STATUS::MECHOS_PART);

	if ( !pi ) pi = GetItem_as_type( Pitem,  UVS_ITEM_STATUS::ARTIFACT);
	if ( !pi )	shape = UVS_VANGER_SHAPE::RANGER;
}

int uvsEscave::arrest( uvsVanger* pv ){
	if (!Pitem) return 0;

	uvsItem* pi = GetItem( pv -> Pitem,  ((uvsItem*)Pitem) -> type, 0 );

	if ( !pi ) return 0;

	pv -> shape = UVS_VANGER_SHAPE::BUNCH_SLAVE;
	pv -> owner = this -> Pbunch;

	((uvsItem*)Pitem) -> param1 = 0;

	pi = GetItem_as_type( pv -> Pitem,  UVS_ITEM_STATUS::TREASURE);
	while( pi ){
		listElem* tmp = (listElem*)pv->Pitem;
		pi->delink( tmp );

		if (pi -> type != ((uvsItem*)Pitem) -> type)
			((listElem*)pi) -> link( Pshop -> Pitem);

		pi = GetItem_as_type( pi -> next,  UVS_ITEM_STATUS::TREASURE);
	}

	FreeList( pv -> Pitem );
	return 1;
}

void uvsVanger::spotArrival(){
	uvsCheckVangerTabuTask(this,uvsVANGER_ARRIVAL);

	Pescave =  NULL;

	switch(shape){
		case UVS_VANGER_SHAPE::TABUTASK:
			FreeList(Pitem);
			break;
		case UVS_VANGER_SHAPE::BUNCH_SLAVE:
			changeRating( Pspot -> bring_game_GOODS(Pitem) );
			if (RND(2))
				Pspot->Pshop->updateList(Pitem);
			FreeList(Pitem);
			break;
		case UVS_VANGER_SHAPE::CARAVAN:

			changeRating( Pspot -> bring_game_GOODS(Pitem) );

			if ( !RND(10) ){
				sell_move_Item( Pspot -> Pshop, 1);
			}
			break;
		case UVS_VANGER_SHAPE::RANGER:
			changeRating( Pspot -> bring_game_GOODS(Pitem) );
			Pspot->Pshop->updateList(Pitem);
			FreeList(Pitem);
			break;
		case UVS_VANGER_SHAPE::THIEF:
			changeRating( Pspot -> bring_game_GOODS(Pitem) );
			if ( !RND(10) ){
				sell_treasure_Item( Pspot -> Pshop);
			}
			break;
		case UVS_VANGER_SHAPE::GAMER:
#ifdef _ROAD_
			status = UVS_VANGER_STATUS::SHOPPING;
			locTimer = LOC_TIME;

			if ( !Pspot -> GamerVisit ){
				GamerResult.dominance += 2;
				ShowDominanceMessage(2);
				if (GamerResult.dominance > 100) GamerResult.dominance = 100;
			}
			Pspot -> GamerVisit++;
#endif
			break;
		}
}

void uvsVanger::changeRating(int val){
	rating += val;

	if ( uvsMechosTable[Pmechos -> type] -> type == UVS_CAR_TYPE::RAFFA ) {
		rating = 0;
		shape = UVS_VANGER_SHAPE::BUNCH_SLAVE;
		return;
	}

	if ( shape == UVS_VANGER_SHAPE::THIEF || shape == UVS_VANGER_SHAPE::CARAVAN) return;

	if(rating >= V_RANGER_MAX){
		rating -= V_RANGER_MAX;
		shape = UVS_VANGER_SHAPE::RANGER;
//		Pmechos -> color = 3;
	}
}

void uvsVanger::update(int x,int y){
	pos_x = x;
	pos_y = y;
}

void uvsVanger::shiftOrders(void){
	if(shape == UVS_VANGER_SHAPE::GAMER || NetworkON) return;

	uvsOrder* ord = orderT;

	switch ( orderT -> target ){
	case UVS_TARGET::NONE:
		ErrH.Abort("uvsVanger::shift order none");
		break;
	case UVS_TARGET::ESCAVE:
		Pescave = (uvsEscave*)(orderT -> Ptarget);
		break;
	case UVS_TARGET::SPOT:
		Pspot = (uvsSpot*)(orderT -> Ptarget);
		break;
	case UVS_TARGET::PASSAGE:
		Ppassage = (uvsPassage*)(orderT -> Ptarget);
		break;
	case UVS_TARGET::VANGER:
		break;
	case UVS_TARGET::ITEM:
		break;
	}//  end swith

	for(int i = 1;i < ORDER_V_MAX; i++){;
		orderT[i - 1].type = orderT[i].type;
		orderT[i - 1].event = orderT[i].event;
		orderT[i - 1].target = orderT[i].target;
		orderT[i - 1].Ptarget = orderT[i].Ptarget;
	}

	ord = orderT + ORDER_V_MAX - 1;
	ord -> type = UVS_ORDER::NONE;
	ord -> target = UVS_TARGET::NONE;
	ord -> Ptarget = NULL;
}

void uvsVanger::emuQuant(void){

#ifdef _ROAD_
	if((CurrentWorld == Pworld -> gIndex) && uvsKronActive) return;
#endif

	switch(shape){
		case UVS_VANGER_SHAPE::TABUTASK:
		case UVS_VANGER_SHAPE::BUNCH_SLAVE:
		case UVS_VANGER_SHAPE::RANGER:
		case UVS_VANGER_SHAPE::THIEF:
		case UVS_VANGER_SHAPE::CARAVAN:
		case UVS_VANGER_SHAPE::KILLER:
			switch(status){
				case UVS_VANGER_STATUS::ESCAVE_SLEEPING:
					break;
				case UVS_VANGER_STATUS::SPOT_SLEEPING:
					break;
				case UVS_VANGER_STATUS::MOVEMENT:
				case UVS_VANGER_STATUS::FREE_MOVEMENT:
				case UVS_VANGER_STATUS::RACE:
				case UVS_VANGER_STATUS::RACE_HUNTER:
				case UVS_VANGER_STATUS::GATHERING:
				case UVS_VANGER_STATUS::WAIT_GAMER:

//					if (UVS_VANGER_STATUS::FREE_MOVEMENT == status ) break;
					if (UVS_VANGER_STATUS::FREE_MOVEMENT == status && RND(3)) break;

					emuStep();
					emuCheck();
					break;
				}
			break;
		case UVS_VANGER_SHAPE::GAMER:
		case UVS_VANGER_SHAPE::GAMER_SLAVE:
			break;
		}

}

void uvsVanger::KillStatic(void){

	if (shape == UVS_VANGER_SHAPE::GAMER_SLAVE) return ;

	if (uvsMechosTable[Pmechos -> type] -> gamer_kill < 5){
		GamerResult.dominance += 5 - uvsMechosTable[Pmechos -> type] -> gamer_kill;
		ShowDominanceMessage(5 - uvsMechosTable[Pmechos -> type] -> gamer_kill);
	} else {
		GamerResult.dominance++;
		ShowDominanceMessage(1);
	}

	if (GamerResult.dominance > 100) GamerResult.dominance = 100;
	uvsMechosTable[Pmechos -> type] -> gamer_kill++;
}

void uvsVanger::destroy(int how){
	int i;
	if(shape == UVS_VANGER_SHAPE::GAMER){
#ifdef _ROAD_
		GameQuantReturnValue = RTO_LOADING3_ID;
		Dead = 1;
		return;
#endif
	}

	type = UVS_OBJECT::VANGER;	// common uvsElement type

	if (how)
		uvsCheckVangerTabuTask(this, uvsVANGER_KILLED);
	else
		uvsCheckVangerTabuTask(this, uvsVANGER_FAILED);

	rating = 1;	// minimal
	status = UVS_VANGER_STATUS::ESCAVE_SLEEPING;		// состояние движения
	shape = UVS_VANGER_SHAPE::BUNCH_SLAVE;			// принадлежность "души"


	Pworld = WorldTable[RND(3)];

	if (Pworld -> escT[0] -> Pbunch -> status == UVS_BUNCH_STATUS::UNABLE ){
		for( i = 0; i < 3; i++){
			if (WorldTable[i] -> escT[0] -> Pbunch -> status != UVS_BUNCH_STATUS::UNABLE){
				Pworld = WorldTable[i];
				break;
			}//  end if
		}//  end for
	} //  end if

	Pescave = Pworld -> escT[0];
	Pmechos -> color = Pescave -> Pbunch -> biosNindex;
	Pescave -> Pshop -> addMechos(Pmechos);
	Pmechos = NULL;
	owner = Pescave -> Pbunch;
	Pspot = NULL;
	Ppassage = NULL;
	FreeList( Pitem );
	Pitem = NULL;

	pos_x =  Pescave -> pos_x;
	pos_y =  Pescave -> pos_y;

	biosNindex = RND(BIOS_MAX);				    // bios случайный
	//orderT = new uvsOrder[ORDER_V_MAX];
	for ( i = 0; i < ORDER_V_MAX; i++){
		orderT[i].type = UVS_ORDER::NONE;
		orderT[i].target = UVS_TARGET::NONE;
		orderT[i].Ptarget = NULL;
	}
	speed = 3 + RND(V_SPEED_MAX);
	locTimer = 0;

	//eunlink(ETail);
	//delink(deleteETail);
}

void uvsVanger::emuStep(void){
	uvsOrder* ord = orderT;


	if( !speed ) ErrH.Abort("emuStep dont have speed");

	if(ord -> Ptarget -> pos_y > pos_y){ pos_y += speed + RND(2); pos_x += RND(5) - 2; }
	else if(ord -> Ptarget -> pos_y < pos_y){ pos_y -= speed + RND(2); pos_x += RND(5) - 2; }
	else
		if(ord -> Ptarget -> pos_x > pos_x) pos_x++;
		else if(ord -> Ptarget -> pos_x < pos_x) pos_x--;

}

void uvsVanger::go_to_GAMER( void ){
	shape = UVS_VANGER_SHAPE::KILLER;

	if ( Pworld == Gamer -> Pworld ){
		uvsOrder* ord;
		ord = orderT;
		ord -> type = UVS_ORDER::MOVING;
		ord -> event = UVS_EVENT::GO_KILL;
		ord -> target = UVS_TARGET::VANGER;
		ord -> Ptarget = (uvsTarget*)(Gamer);
		status = UVS_VANGER_STATUS::MOVEMENT;
	} else {
		uvsWorld* fromW = Pworld;
		uvsWorld* toW = Gamer -> Pworld;
		int offset;
		uvsOrder* ord;

		if ( toW -> gIndex < MAIN_WORLD_MAX ){
			offset = buildWay(fromW,toW);

			ord = orderT + offset;
			ord -> type = UVS_ORDER::MOVING;
			ord -> event = UVS_EVENT::GO_KILL;
			ord -> target = UVS_TARGET::VANGER;
			ord -> Ptarget = (uvsTarget*)(Gamer);
			status = UVS_VANGER_STATUS::MOVEMENT;
		} else {
			ord = orderT;
			ord -> type = UVS_ORDER::MOVING;
			ord -> event = UVS_EVENT::GO_KILL;
			ord -> target = UVS_TARGET::VANGER;
			ord -> Ptarget = (uvsTarget*)(Gamer);
			status = UVS_VANGER_STATUS::WAIT_GAMER;
		}
		//if ( !toW -> escT[0] )
		//	ErrH.Abort("");
	}
}

void uvsVanger::emuCheck(void){
	uvsOrder* ord = orderT;

	if (shape  == UVS_VANGER_SHAPE::KILLER){
		go_to_GAMER();
	}

	if(abs(ord -> Ptarget -> pos_y - pos_y) <= V_SPEED_MAX){
		pos_y = ord -> Ptarget -> pos_y;

		if ( (ord -> event == UVS_EVENT::GO_CIRT) && (abs(ord -> Ptarget -> pos_x - pos_x) <= V_CIRT_R/2)){
			Event(UVS_EVENT::AIM_ARRIVAL);
		} else	if(abs(ord -> Ptarget -> pos_x - pos_x) <= V_SPEED_MAX ){
			pos_x = ord -> Ptarget -> pos_x;
			pos_y = ord -> Ptarget -> pos_y;
			switch(ord -> event){
				case UVS_EVENT::GO_SPOT:
					Pspot = (uvsSpot*)(orderT -> Ptarget);
					Event(UVS_EVENT::SPOT_ARRIVAL);
					break;
				case UVS_EVENT::GO_ESCAVE:
					Pescave = (uvsEscave*)(orderT -> Ptarget);
					Event(UVS_EVENT::ESCAVE_ARRIVAL);
					break;
				case UVS_EVENT::GO_PASSAGE:
					//Ppassage = (uvsPassage*)( orderT -> Ptarget);
					Event(UVS_EVENT::PASSAGE_ARRIVAL);
					break;
				case UVS_EVENT::GO_CORN:
					break;
				case UVS_EVENT::GO_KILL:
					break;
				default:
					ErrH.Abort("emuCheck arrival to never.");
					break;
			}//  end switch
		}//  end if
	}//  end if
}

int lastMechos;
#ifndef _ROAD_
char *aci_curLocationName = "";
#endif

void uvsVanger::start_race(void){
	if (Pworld -> escTmax){
		uvsBunch *pb = Pworld -> escT[0] -> Pbunch;
		uvsCultGame* pg = pb -> cycleTable[pb -> currentStage].Pgame;

		if (pg){
			if(pg -> GameType == UVS_GAME_TYPE::RACE && !pg -> start){
				if (!GamerResult.get_weezyk && pg -> GoodsTypeBeg == UVS_ITEM_TYPE::WEEZYK)
					pg -> GoodsBegCount = 1;
				pg -> start = 1;
			}
		}//  end if pg
	}//  end if Pworld
}

void uvsVanger::prepare_diagen_statistic(void){
	int nymbos;
	int phlegma;
	int heroin;
	int shrub;
	int poponka;
	int toxick;

	uvsPassagerFromStatistic();

	//GamerResult.get_weezyk = 0;
	GamerResult.mechos_type = uvsMechosTable[Pmechos -> type] -> type;

	GamerResult.eleech = ItemCount(Pitem,UVS_ITEM_TYPE::ELEECH );

	nymbos = GamerResult.nymbos;
	phlegma = GamerResult.phlegma;
	heroin	 = GamerResult.heroin;
	shrub = GamerResult.shrub;
	poponka = GamerResult.poponka;
	toxick = GamerResult.toxick;

	GamerResult.eleepod = ItemCount(Pitem,UVS_ITEM_TYPE::ELEEPOD );
	GamerResult.beeboorat = ItemCount(Pitem,UVS_ITEM_TYPE::BEEBOORAT );
	GamerResult.zeex = ItemCount(Pitem,UVS_ITEM_TYPE::ZEEX );

	GamerResult.nymbos = ItemCount(Pitem,UVS_ITEM_TYPE::NYMBOS );
	GamerResult.phlegma = ItemCount(Pitem, UVS_ITEM_TYPE::PHLEGMA);
	GamerResult.heroin = ItemCount(Pitem,UVS_ITEM_TYPE::HEROIN );
	GamerResult.shrub = ItemCount(Pitem, UVS_ITEM_TYPE::SHRUB);
	GamerResult.poponka = ItemCount(Pitem,UVS_ITEM_TYPE::POPONKA );
	GamerResult.toxick = ItemCount(Pitem, UVS_ITEM_TYPE::TOXICK);

	GamerResult.unik_poponka = uvsPoponkaIDON(Pitem);

	if (NetworkON && my_server_data.GameType == MECHOSOMA){
		if (GamerResult.nymbos > nymbos)
			my_player_body.MechosomaStat.SneakCount += GamerResult.nymbos - nymbos;
		else
			my_player_body.MechosomaStat.LostCount -= GamerResult.nymbos - nymbos;

		if (GamerResult.phlegma > phlegma)
			my_player_body.MechosomaStat.SneakCount += GamerResult.phlegma - phlegma;
		else
			my_player_body.MechosomaStat.LostCount -= GamerResult.phlegma - phlegma;

		if (GamerResult.heroin > heroin)
			my_player_body.MechosomaStat.SneakCount += GamerResult.heroin - heroin;
		else
			my_player_body.MechosomaStat.LostCount -= GamerResult.heroin - heroin;

		if (GamerResult.shrub > shrub)
			my_player_body.MechosomaStat.SneakCount += GamerResult.shrub - shrub;
		else
			my_player_body.MechosomaStat.LostCount -= GamerResult.shrub - shrub;

		if (GamerResult.poponka > poponka)
			my_player_body.MechosomaStat.SneakCount += GamerResult.poponka - poponka;
		else
			my_player_body.MechosomaStat.LostCount -= GamerResult.poponka - poponka;

		if (GamerResult.toxick > toxick)
			my_player_body.MechosomaStat.SneakCount += GamerResult.toxick - toxick;
		else
			my_player_body.MechosomaStat.LostCount -= GamerResult.toxick - toxick;

		NetStatisticUpdate(NET_STATISTICS_CHECKPOINT);
		send_player_body(my_player_body);
	}

	GamerResult.pipetka = ItemCount(Pitem, UVS_ITEM_TYPE::PIPETKA, 1);
	GamerResult.kernoboo = ItemCount(Pitem, UVS_ITEM_TYPE::KERNOBOO, 1);
	GamerResult.weezyk = ItemCount(Pitem, UVS_ITEM_TYPE::WEEZYK);
	GamerResult.rubbox = ItemCount(Pitem, UVS_ITEM_TYPE::RUBBOX);

	GamerResult.dead_eleech_now = ItemCount(Pitem, UVS_ITEM_TYPE::DEAD_ELEECH);

	GamerResult.boot_sector = ItemCount(Pitem, UVS_ITEM_TYPE::SECTOR);
	GamerResult.peelot = ItemCount(Pitem, UVS_ITEM_TYPE::PEELOT);
	GamerResult.spummy = ItemCount(Pitem, UVS_ITEM_TYPE::SPUMMY);
	GamerResult.fun83 = ItemCount(Pitem, UVS_ITEM_TYPE::FUNCTION83);

	GamerResult.mechanic_messiah = ItemCount(Pitem, UVS_ITEM_TYPE::MECHANIC_MESSIAH);
	GamerResult.protractor = ItemCount(Pitem, UVS_ITEM_TYPE::PROTRACTOR);
	GamerResult.leepuringa = ItemCount(Pitem, UVS_ITEM_TYPE::LEEPURINGA);
	GamerResult.palochka = ItemCount(Pitem, UVS_ITEM_TYPE::PALOCHKA);
	GamerResult.pipka = ItemCount(Pitem, UVS_ITEM_TYPE::PIPKA);
	GamerResult.nobool = ItemCount(Pitem, UVS_ITEM_TYPE::NOBOOL);
	GamerResult.boorawchick = ItemCount(Pitem, UVS_ITEM_TYPE::BOORAWCHICK);
	GamerResult.pereponka = ItemCount(Pitem, UVS_ITEM_TYPE::PEREPONKA);
	GamerResult.zeefick = ItemCount(Pitem, UVS_ITEM_TYPE::ZEEFICK);

	GamerResult.thief_leepuringa = uvsReturnTreasureStatus(UVS_ITEM_TYPE::LEEPURINGA);
	GamerResult.thief_palochka = uvsReturnTreasureStatus(UVS_ITEM_TYPE::PALOCHKA);
	GamerResult.thief_pipka = uvsReturnTreasureStatus(UVS_ITEM_TYPE::PIPKA);
	GamerResult.thief_nobool = uvsReturnTreasureStatus(UVS_ITEM_TYPE::NOBOOL);
	GamerResult.thief_boorawchick = uvsReturnTreasureStatus(UVS_ITEM_TYPE::BOORAWCHICK);
	GamerResult.thief_pereponka = uvsReturnTreasureStatus(UVS_ITEM_TYPE::PEREPONKA);
	GamerResult.thief_zeefick = uvsReturnTreasureStatus(UVS_ITEM_TYPE::ZEEFICK);

#ifdef STAND_REPORT
	stand < "statistic:lucl " <= (GamerResult.luck + GamerResult.add_luck) < " dominionce "  <= (GamerResult.dominance+GamerResult.add_dominance) < "\n";
	stand < "statistic: nimbos " <= GamerResult.nymbos_total < " phlegma "  <= GamerResult.phlegma_total < "\n";
	stand < "statistic: heroin " <= GamerResult.heroin_total < " shrub "  <= GamerResult.shrub_total < "\n";
	stand < "statistic: poponka " <= GamerResult.poponka_total < " toxick "  <= GamerResult.toxick_total < " dead eleech " <= GamerResult.dead_eleech_now < " mechos " <= GamerResult.mechos_variant < "  \n";
#endif

	_checkDolly_();
	_sellCirt_();
}

void uvsVanger::prepare_shop(int how){
		uvsGamerWaitGame = 0;
		uvsCurrentWorldUnableBefore = uvsCurrentWorldUnable;
		uvsTownName = NULL;

		EffectsOff();

		if ( Pescave ){
			aci_curLocationName = Pescave -> name;
			uvsBunchNumber = Pescave -> Pbunch -> biosNindex;
		} else
			aci_curLocationName = Pspot -> name;

		if (strcmp(uvsLastEscaveName, aci_curLocationName))
			uvsGoodsON = 1;
		else
			uvsGoodsON = 0;

		uvsLastEscaveName = aci_curLocationName;

		if (!how) return;

		GGamerMechos = GGamer = GMechos = GItem = NULL;
		lastMechos = Pmechos -> type;

		prepare_diagen_statistic();

		dgD -> checkSession(aci_curLocationName); // rname - имя эскейва

		if (!dgAbortStatus)
			checkDolly();

		if ( Pescave ){
#ifdef _ROAD_
			Pescave -> Pshop -> locked = 1;
			Pescave -> add_goods_to_shop();
			Pescave -> Pshop ->  prepare_list_for_ActInt( GMechos, GItem, 1);

			uvsItem* pi = GetItem( Pitem, ((uvsItem*)Pescave -> Pitem) -> type, 0 );

			if (pi && (Pescave -> Pbunch -> status != UVS_BUNCH_STATUS::UNABLE) ){
				if (((uvsItem*)Pescave -> Pitem) -> param1 != uvsTreasureInGamer){
					aciUpdateCurCredits(aciGetCurCredits() + Pescave -> Pshop -> Price[((uvsItem*)Pescave -> Pitem) -> type + MAX_ITEM_TYPE]);

					ShowDominanceMessage(5);
					GamerResult.dominance += 5;
					if (GamerResult.dominance >100) GamerResult.dominance = 100;
				}

				uvs_aciKillItem(uvsSetItemType(pi -> type, 0, -1));
				pi -> delink(Pitem);
				delete pi;

				((uvsItem*)Pescave -> Pitem) -> param1 = 0;
			}
//			GamerResult.beeb = aciCurCredits;
#endif
			prepare_list_for_ActInt(GGamer, GGamerMechos, Pescave -> Pshop, 1);
#ifdef STAND_REPORT
		stand < ConTimer.GetTime() < " GAMER arrival to " < aci_curLocationName < " -> " <= Pescave -> GamerVisit < " \n";
#endif
		} else {
#ifdef _ROAD_
//			GamerResult.beeb = aciCurCredits;
#endif
			if (Pspot -> Pshop){
				Pspot -> Pshop -> locked = 1;
				Pspot -> add_goods_to_shop();
				Pspot -> Pshop -> prepare_list_for_ActInt( GMechos, GItem, 0);

				uvsItem* pi = GetItem( Pitem, ((uvsItem*)Pspot -> Pitem) -> type, 0 );

				if ( (Pworld -> escT[0] -> Pbunch -> status != UVS_BUNCH_STATUS::UNABLE) &&pi && (pi -> type != UVS_ITEM_TYPE::NOBOOL) && (pi -> type != UVS_ITEM_TYPE::PIPKA) ){
					if (((uvsItem*)Pspot -> Pitem) -> param1 != uvsTreasureInGamer){
						aciUpdateCurCredits(aciGetCurCredits() + Pspot -> Pshop -> Price[((uvsItem*)Pspot -> Pitem) -> type + MAX_ITEM_TYPE]);

						ShowDominanceMessage(5);
						GamerResult.dominance += 5;
						if (GamerResult.dominance >100) GamerResult.dominance = 100;
					}

					uvs_aciKillItem(uvsSetItemType(pi -> type, 0, -1));
					pi -> delink(Pitem);
					delete pi;

					((uvsItem*)Pspot -> Pitem) -> param1 = 0;
				}
			}
			prepare_list_for_ActInt(GGamer, GGamerMechos, Pspot -> Pshop, 0);
#ifdef STAND_REPORT
		stand < ConTimer.GetTime() < " GAMER arrival to " < aci_curLocationName < " -> " <= Pspot -> GamerVisit < " \n";
#endif
		}

		if (thief_on_world()){
			if (GamerResult.theif_on_world)  GamerResult.theif = 0;
			else {
				GamerResult.theif  = 1;
				GamerResult.theif_on_world = 1;
			}
		} else {
			GamerResult.theif_on_world = 0;
		}

#ifdef STAND_REPORT
		if (GamerResult.theif_on_world)
			stand < "thief on the world\n";
#endif
}

int uvsVanger::CirtDelivery(void){
	int Total = 0;
	uvsBunch *pb = Pworld -> escT[0] -> Pbunch;

	for( int i = 0; i < pb -> cycleN; i++)
		Total += pb -> cycleTable[i].GamerCirt;

	return Total;
}

void uvsChangeCycle(void){
	uvsBunch *pb = Gamer -> Pworld -> escT[0] -> Pbunch;
	int i = pb -> currentStage;

	pb -> cycleTable[i].cirtQ += pb -> cycleTable[i].cirtMAX;
}

int uvsVanger::GoodTabuTask(void){
	if (Pescave)
		return Pescave -> TabuTaskGood;
	else
		return Pspot -> TabuTaskGood;
}

int uvsVanger::newGoodTabuTask(void){
	if (Pescave)
		return (Pescave -> TabuTaskGood - Pescave -> lastTabuTaskGood);
	else
		return (Pspot -> TabuTaskGood - Pspot -> lastTabuTaskGood);
}

int uvsVanger::GoodWorldTabuTask(void){
	int good = 0;
	int i;

	for (i = 0; i < Pworld -> escTmax; i++){
		good += Pworld -> escT[i] -> TabuTaskGood;
	}

	for (i = 0; i < Pworld -> sptTmax; i++){
		good += Pworld -> sptT[i] -> TabuTaskGood;
	}

	return good;
}

void uvsVanger::get_shop(int how){
	int count;
	int UnicumON = 0;
	listElem* pt;
	uvsItem *pi = NULL;

		if ( Pescave ){
			Pescave -> Pshop ->  get_list_from_ActInt( GMechos, GItem);
			Pescave -> Pshop -> locked = 0;
			pt = Pescave -> Pgood;

			Pescave ->  lastTabuTaskGood = Pescave -> TabuTaskGood;
	
			if (NetworkON && my_server_data.GameType == MUSTODONT){
				UnicumON = Pescave -> Pshop ->FindMechos(uvsUnikumeMechos);
			}

			pi = GetItem(Pescave -> Pshop -> Pitem, ((uvsItem*)Pescave -> Pitem) -> type, 0 );
			if (pi){
				pi -> delink(Pescave -> Pshop -> Pitem);
				delete pi;

				((uvsItem*)Pescave -> Pitem) -> param1 = 0;
			}

			pi = GetItem(Pescave -> Pshop -> Pitem, UVS_ITEM_TYPE::BOORAWCHICK, 0 );
			if (pi){
				pi -> delink(Pescave -> Pshop -> Pitem);
#ifdef STAND_REPORT
stand < ConTimer.GetTime() < "BOORAWCHICK go home\n";
#endif
				delete pi;

				((uvsItem*)WorldTable[1] -> escT[0] -> Pitem) ->param1 = uvsTreasureInWait;
			}

		} else if (Pspot -> Pshop){
			Pspot -> Pshop ->  get_list_from_ActInt( GMechos, GItem);
			Pspot -> Pshop -> locked = 0;
			pt = Pspot -> Pgood;

			Pspot ->  lastTabuTaskGood = Pspot -> TabuTaskGood;

			if (NetworkON && my_server_data.GameType == MUSTODONT){
				UnicumON = Pspot -> Pshop ->FindMechos(uvsUnikumeMechos);
			}

			pi = GetItem(Pspot -> Pshop -> Pitem, ((uvsItem*)Pspot -> Pitem) -> type, 0 );
			if (pi){
				pi -> delink(Pspot -> Pshop -> Pitem);
				delete pi;

				((uvsItem*)Pspot -> Pitem) -> param1 = 0;
			}

			pi = GetItem(Pspot -> Pshop -> Pitem, UVS_ITEM_TYPE::BOORAWCHICK, 0 );
			if (pi){
				pi -> delink(Pspot -> Pshop -> Pitem);
				delete pi;
#ifdef STAND_REPORT
stand < ConTimer.GetTime() < "BOORAWCHICK go home\n";
#endif
				((uvsItem*)WorldTable[1] -> escT[0] -> Pitem) ->param1 = uvsTreasureInWait;
			}
		}

		 get_list_from_ActInt(GGamer, GGamerMechos);

		count = GamerResult.nymbos - ItemCount(Pitem,UVS_ITEM_TYPE::NYMBOS );
		GamerResult.nymbos = ItemCount(Pitem,UVS_ITEM_TYPE::NYMBOS );
		if (count > 0){
			uvsTradeItem* pl = (uvsTradeItem*)pt;
			while(pl){
				if (pl -> type == UVS_ITEM_TYPE::NYMBOS){
					GamerResult.nymbos_total += count;
					if (GamerResult.nymbos_total >= GamerResult.nymbos_bonus*20) GamerResult.nymbos_bonus++;
					break;
				}
				pl = (uvsTradeItem*)pl -> next;
			}//  end while

			if (NetworkON && my_server_data.GameType == MECHOSOMA){
				my_player_body.MechosomaStat.ItemCount1 += count;
				send_player_body(my_player_body);
			}
		}

		count = GamerResult.phlegma - ItemCount(Pitem, UVS_ITEM_TYPE::PHLEGMA);
		GamerResult.phlegma = ItemCount(Pitem, UVS_ITEM_TYPE::PHLEGMA);
		if (count > 0){
			uvsTradeItem* pl = (uvsTradeItem*)pt;
			while(pl){
				if (pl -> type == UVS_ITEM_TYPE::PHLEGMA){
					GamerResult.phlegma_total += count;
					if (GamerResult.phlegma_total >= GamerResult.phlegma_bonus*20) GamerResult.phlegma_bonus++;
					break;
				}
				pl = (uvsTradeItem*)pl -> next;
			}//  end while
			if (NetworkON && my_server_data.GameType == MECHOSOMA){
				my_player_body.MechosomaStat.ItemCount2 += count;
				send_player_body(my_player_body);
			}
		}
		//stalkerg Где то тут
		count = GamerResult.heroin - ItemCount(Pitem,UVS_ITEM_TYPE::HEROIN );
		GamerResult.heroin = ItemCount(Pitem,UVS_ITEM_TYPE::HEROIN );
		if (count > 0){
			uvsTradeItem* pl = (uvsTradeItem*)pt;
			while(pl){
				if (pl -> type == UVS_ITEM_TYPE::HEROIN){
					GamerResult.heroin_total += count;
					if (GamerResult.heroin_total >= GamerResult.heroin_bonus*20) GamerResult.heroin_bonus++;
					break;
				}
				pl = (uvsTradeItem*)pl -> next;
			}//  end while
			if (NetworkON && my_server_data.GameType == MECHOSOMA){
				my_player_body.MechosomaStat.ItemCount1 += count;
				send_player_body(my_player_body);
			}
		}

		count = GamerResult.shrub - ItemCount(Pitem,UVS_ITEM_TYPE::SHRUB );
		GamerResult.shrub = ItemCount(Pitem,UVS_ITEM_TYPE::SHRUB );
		if (count > 0){
			uvsTradeItem* pl = (uvsTradeItem*)pt;
			while(pl){
				if (pl -> type == UVS_ITEM_TYPE::SHRUB){
					GamerResult.shrub_total += count;
					if (GamerResult.shrub_total >= GamerResult.shrub_bonus*20) GamerResult.shrub_bonus++;
					break;
				}
				pl = (uvsTradeItem*)pl -> next;
			}//  end while
			if (NetworkON && my_server_data.GameType == MECHOSOMA){
				my_player_body.MechosomaStat.ItemCount2 += count;
				send_player_body(my_player_body);
			}
		}

		count = GamerResult.poponka - ItemCount(Pitem,UVS_ITEM_TYPE::POPONKA );
		GamerResult.poponka = ItemCount(Pitem,UVS_ITEM_TYPE::POPONKA );
		if (count > 0){
			uvsTradeItem* pl = (uvsTradeItem*)pt;
			while(pl){
				if (pl -> type == UVS_ITEM_TYPE::POPONKA){
					GamerResult.poponka_total += count;
					if (GamerResult.poponka_total >= GamerResult.poponka_bonus*20) GamerResult.poponka_bonus++;
					break;
				}
				pl = (uvsTradeItem*)pl -> next;
			}//  end while
			if (NetworkON && my_server_data.GameType == MECHOSOMA){
				my_player_body.MechosomaStat.ItemCount1 += count;
				send_player_body(my_player_body);
			}
		}

		count = GamerResult.toxick - ItemCount(Pitem,UVS_ITEM_TYPE::TOXICK );
		GamerResult.toxick = ItemCount(Pitem,UVS_ITEM_TYPE::TOXICK );
		if (count > 0){
			uvsTradeItem* pl = (uvsTradeItem*)pt;
			while(pl){
				if (pl -> type == UVS_ITEM_TYPE::TOXICK){
					GamerResult.toxick_total += count;
					if (GamerResult.toxick_total >= GamerResult.toxick_bonus*20) GamerResult.toxick_bonus++;
					break;
				}
				pl = (uvsTradeItem*)pl -> next;
			}//  end while
			if (NetworkON && my_server_data.GameType == MECHOSOMA){
				my_player_body.MechosomaStat.ItemCount2 += count;
				send_player_body(my_player_body);
			}
		}

		if (NetworkON && my_server_data.GameType == MECHOSOMA){

			if (my_player_body.MechosomaStat.ItemCount2 >= my_server_data.Mechosoma.ProductQuantity2 
		     && my_player_body.MechosomaStat.ItemCount1 >= my_server_data.Mechosoma.ProductQuantity1){
				time_t _t_;
				time(&_t_);

				my_player_body.MechosomaStat.MaxTransitTime = _t_ - uvsFirstTime;
				GameOverID = GAME_OVER_NETWORK;
				GameQuantReturnValue = RTO_LOADING3_ID;
				send_player_body(my_player_body);
			}
		}

		if (NetworkON && my_server_data.GameType == MUSTODONT){

			if (UnicumON && !my_player_body.MustodontStat.BodyTime){
				time_t _t_;
				time(&_t_);
				my_player_body.MustodontStat.BodyTime = _t_ - uvsFirstTime;
			}

			if (uvsMechosTable[uvsUnikumeMechos] -> constractor && !my_player_body.MustodontStat.PartTime1){
				time_t _t_;
				time(&_t_);
				my_player_body.MustodontStat.PartTime1 = _t_ - uvsFirstTime;
			}

			if ((uvsMechosTable[uvsUnikumeMechos] -> constractor == 3) && !my_player_body.MustodontStat.PartTime2){
				time_t _t_;
				time(&_t_);
				my_player_body.MustodontStat.PartTime2 = _t_ - uvsFirstTime;
				my_player_body.MustodontStat.MakeTime = _t_ - uvsFirstTime;
			}
		}

		if (!GamerResult.leepuringa) {
			GamerResult.leepuringa = ItemCount(Pitem, UVS_ITEM_TYPE::LEEPURINGA);

			if (GamerResult.leepuringa){
				GamerResult.add_luck += 50;
				ShowLuckMessage(50);
			}
		} else {
			GamerResult.leepuringa = ItemCount(Pitem, UVS_ITEM_TYPE::LEEPURINGA);

			if (!GamerResult.leepuringa){
				GamerResult.add_luck -= 50;
				ShowLuckMessage(-50);
			}
		}

		if (!GamerResult.mechanic_messiah) {
			GamerResult.mechanic_messiah = ItemCount(Pitem, UVS_ITEM_TYPE::MECHANIC_MESSIAH);

			if (GamerResult.mechanic_messiah){
				GamerResult.add_dominance += 20;
				ShowDominanceMessage(20);
			}
		} else {
			GamerResult.mechanic_messiah = ItemCount(Pitem, UVS_ITEM_TYPE::MECHANIC_MESSIAH);

			if (!GamerResult.mechanic_messiah){
				GamerResult.add_dominance -= 20;
				ShowDominanceMessage(-20);
			}
		}

		if (!GamerResult.protractor) {
			GamerResult.protractor = ItemCount(Pitem, UVS_ITEM_TYPE::PROTRACTOR);

			if (GamerResult.protractor){
				GamerResult.add_luck += 20;
				ShowLuckMessage(20);
			}
		} else {
			GamerResult.protractor = ItemCount(Pitem, UVS_ITEM_TYPE::PROTRACTOR);

			if (!GamerResult.protractor){
				GamerResult.add_luck -= 20;
				ShowLuckMessage(-20);
			}
		}

		if (!GamerResult.boorawchick) {
			GamerResult.boorawchick = ItemCount(Pitem, UVS_ITEM_TYPE::BOORAWCHICK);

			if (GamerResult.boorawchick){
				GamerResult.add_dominance += 50;
				ShowDominanceMessage(50);
			}
		} else {
			GamerResult.boorawchick = ItemCount(Pitem, UVS_ITEM_TYPE::BOORAWCHICK);

			if (!GamerResult.boorawchick){
				GamerResult.add_dominance -= 50;
				ShowDominanceMessage(-50);
			}
		}

/*		if (GamerResult.luck > 100) GamerResult.luck = 100;
		if (GamerResult.luck < 0) GamerResult.luck = 0;
		if (GamerResult.dominance < -100) GamerResult.dominance = -100;
		if (GamerResult.dominance > 100) GamerResult.dominance = 100;
*/
		GamerResult.LocalInit();
		GamerResult.get_eleech = ItemCount(Pitem,UVS_ITEM_TYPE::ELEECH );
		GamerResult.get_weezyk = ItemCount(Pitem,UVS_ITEM_TYPE::WEEZYK );

		start_race();

		if (  lastMechos != Pmechos -> type ) {
			if (!uvsMechosTable[Pmechos -> type] -> gamer_use){
				GamerResult.mechos_variant++;
				if (uvsMechosTable[Pmechos -> type] -> constractor){
					ShowDominanceMessage(9);
					GamerResult.dominance += 9;
				} else {
					GamerResult.dominance += 5;
					ShowDominanceMessage(5);
				}
				if (GamerResult.dominance > 100) GamerResult.dominance = 100;
			}

			ChangeArmor = -1;
//			ChangeArmor = uvsMechosTable[Pmechos -> type] -> MaxArmor;
			uvsMechosTable[Pmechos -> type] -> gamer_use++;
#ifndef _DEMO_
			Pmechos -> color = Pworld -> escT[0] -> Pbunch -> biosNindex;
#else
			Pmechos -> color = CAR_COLOR[Pmechos -> type];
#endif
		}

#ifdef STAND_REPORT
		stand < ConTimer.GetTime() < " GAMER leave " < aci_curLocationName < "\n";
#endif
		if (uvsTownName){
			status = UVS_VANGER_STATUS::SHOPPING;
			locTimer = 1;

			uvsEscave* pe = (uvsEscave*)EscaveTail -> seekName(uvsTownName);
			if ( !pe )	{
				uvsSpot* ps = (uvsSpot*)SpotTail -> seekName(uvsTownName);
				if ( !ps ) ErrH.Abort("uvsTownName wrong ",XERR_USER,-1,uvsTownName);
				Pspot = ps;
				Pescave = NULL;
			} else {
				Pescave = pe;
				Pspot = NULL;
			}
		} else
			status = UVS_VANGER_STATUS::MOVEMENT;

		if (WorldTable[CurrentWorld] -> escTmax)
			aci_curLocationName = WorldTable[CurrentWorld] -> escT[0] -> name;
		else
			aci_curLocationName = "";
}

#ifdef _ROAD_
UvsTargetType uvsVanger::getOrder(uvsTarget*& pt){
	pt = orderT -> Ptarget;
	if( orderT -> target < 0 || orderT -> target > 7) {
/*		char st[2];
		memset(st, 0, 2);
		st[0] = '0' + orderT -> target;
		ErrH.Abort(" uvsVanger::getOrder - unindefic target", XERR_USER, -1, st);*/

		if (Pworld -> gIndex > 2)
			owner = WorldTable[RND(3)] -> escT[0] -> Pbunch;
		else
			owner = Pworld -> escT[0] -> Pbunch;
		goHome();

		return orderT -> target;
	}

	return orderT -> target;
}
#endif

int uvsVanger::CurrentDolly(void){
	if (Pworld -> escTmax){
		uvsBunch *pb = Pworld -> escT[0] -> Pbunch;
		return pb -> cycleTable[ pb -> currentStage ].Pdolly -> gIndex;
	} else
		return -1;
}

void uvsQuant(void){
//	DBGCHECK;
//_MEM_STATISTIC_("BEFOR uvsQuant -> ");
//	uvsBunchCheck("Before Quant");
//	uvsCheckAllItem();

	NetworkQuant();

	if (NetworkON && ConTimer.day == 1 && ConTimer.hour == 0 && ConTimer.min == 0 && ConTimer.sec == 0){
		uvsBunch* pb = (uvsBunch*)BunchTail;

		while(pb){
			sBunchCreate(pb -> id, pb -> Pescave -> Pworld -> gIndex,0,pb -> currentStage);
			pb = (uvsBunch*)pb -> next;
		}
//		sBunchCreate(int& id,int world,int data);
		ConTimer.sec = 1;
	}

	if (!RND(1024) && NetworkON && uvsNetworkChangeWorld){
		if (Gamer -> Pworld -> gIndex < 3){
			uvsBunch *pb = Gamer -> Pworld -> escT[0] -> Pbunch;
			pb -> cycleTable[pb -> currentStage].cirtQ++;
			sBunchSend(pb -> id, pb -> cycleTable[pb -> currentStage].cirtQ, pb -> currentStage);
		}//  end fi
	}//  end if

	if (!RND(1024) && !NetworkON && (Gamer -> status == UVS_VANGER_STATUS::IN_SHOP)){
		if (Gamer -> Pworld -> gIndex < 3){
			uvsBunch *pb = Gamer -> Pworld -> escT[0] -> Pbunch;
			pb -> cycleTable[pb -> currentStage].cirtQ++;
		}//  end fi
	}//  end if

	uvsQuantFlag ^= 1;

	if (uvsQuantFlag &&  Gamer -> status == UVS_VANGER_STATUS::IN_SHOP) return;

	ConTimer.Quant();

	uvsElement* p;

	if(NetworkON) {
		Gamer -> Quant();

		listElem* l = BunchTail;
		while(l){
			((uvsBunch*)l) -> Quant();
			l = l -> next;
		}

		//znfo HERE CALLED DOLLY QUANT in network game
		p = ETail;
		while(p){ 
			switch (p->type) {
			case UVS_OBJECT::DOLLY:
			//case UVS_OBJECT::VANGER:
			//case UVS_OBJECT::FLY_FARMER:
				p->Quant();
			}
			p = p->enext;
		}

		return;
	}

	c_All_Number = c_S_Number = c_R_Number = c_A_Number = 0;

	for(int i = 0;i < EMU_MAX;i++){
		p = ETail;
		while(p){
			p -> emuQuant();
			p = p -> enext;
			}
		}

	p = ETail;
	while(p){
		p -> Quant();
		p = p -> enext;
		}

	listElem* l = BunchTail;
	while(l){
		((uvsBunch*)l) -> Quant();
		l = l -> next;
		}

	l = WorldTail;
	while(l){
		((uvsWorld*)l) -> Quant();
		l = l -> next;
		}

	uvsElement*& ip = deleteETail;
	while(ip){
		p = ip;
		ip = ip -> eprev;
		delete ((uvsVanger*)p);
		}

//	uvsBunchCheck("After Quant");
//	DBGCHECK;
//_MEM_STATISTIC_("AFTER uvsQuant -> ");
}

void uniVangSave(XStream &pfile){
	int count, i;
// worlds initing
//	pfile.open("save", XS_OUT);
	pfile < uvsVersion;
	pfile < uvsGamerWaitGame;

	count = 0;
	listElem*p = WorldTail;
	while( p ){
		count++;
		p = p -> next;
	}
	pfile < count;

	p = WorldTail;
	for( i = 0; i < count; i++ ){
		((uvsWorld*)p) -> save(pfile);
		p = p -> next;
	}

// dolly initing
	count = 0;
	p = DollyTail;
	while( p ){
		count++;
		p = p -> next;
	}
	pfile < count;
	p = DollyTail;
	for( i = 0; i < count; i++){
		((uvsDolly*)p) -> save(pfile);
		p = p -> next;
	}

	// escaves initing
	count = 0;
	p = EscaveTail;
	while( p ){
		count++;
		p = p -> next;
	}
	pfile < count;
	p = EscaveTail;
	for( i = 0; i < count; i++){
		((uvsEscave*)p) -> save(pfile);
		p = p -> next;
	}

	// spots initing
	count = 0;
	p = SpotTail;
	while( p ){
		count++;
		p = p -> next;
	}
	pfile < count;
	p = SpotTail;
	for( i =0; i < count; i++){
		((uvsSpot*)p) -> save(pfile);
		p = p -> next;
	}

	// passages initing
	count = 0;
	p = PassageTail;
	while( p ){
		count++;
		p = p -> next;
	}
	pfile < count;
	p = PassageTail;
	for(i = 0; i < count; i++){
		((uvsPassage*)p)-> save(pfile);
		p = p -> next;
	}

	// bunches initing
	count = 0;
	p = BunchTail;
	while( p ){
		count++;
		p = p -> next;
	}
	pfile < count;
	p = BunchTail;
	for(i = 0; i < count; i++){
		((uvsBunch*)p) -> save(pfile);
		p = p -> next;
	}

// vangers initing
	pfile < cVngNumber;

	uvsElement* e = ETail;
	while(e){
		if( e -> type == UVS_OBJECT::VANGER) ((uvsVanger*)e) -> save(pfile);
		e = e -> enext;
	}

	uvsTabuTaskType *pt = (uvsTabuTaskType*)TabuTaskTypeTail;
	int j;

	for(i = 0; i < MAX_TABUTASK; i++){
		pfile < pt -> status;

		for(j = 0; j < pt -> item_number; j++)
			pfile < pt -> real_count[j];

		if (pt -> target == UVS_TARGET::VANGER && pt -> status)
			pfile < ((uvsVanger*)pt -> Ptarget)-> gIndex;
		pt = (uvsTabuTaskType*)pt -> next;
	}

	for(i = MAX_TABUTASK; i < MAX_TABUTASK_RESERVED; i++){
		pfile < int(0);
		pfile < int(0);
	}

	pfile < CurrentWorld;
	pfile < lastMechos;
	ConTimer.save(pfile);
	GamerResult.Save(pfile);
	uvsMechosSave(pfile);
	pfile < uvsPoponkaID;
//	pfile.close();
}

void CloseCyclicPal(void);
void OpenCyclicPal(void);
void GeneralTableOpen(void);

void uniVangLoad(XStream &pfile){
//	XStream pfile;
	PrmFile pf;
	int count, i;

	uniVangDelete();

	uvsTownName = NULL;
	uvsQuantFlag = 0;

	ETail = NULL;

	pfile > uvsLoadVersion;
	if (uvsLoadVersion > 12)
		pfile > uvsGamerWaitGame;
	else
		uvsGamerWaitGame = 0;

	// item initing
	pf.init(PrmFileName[6]);
	if(strcmp(PrmSignature,pf.getAtom())) ErrH.Abort(PrmWrongMsg,XERR_USER,-1,PrmFileName[6]);

	MAX_ITEM_TYPE = atoi(pf.getAtom());
	uvsItemTable = new uvsItemType*[MAX_ITEM_TYPE];
	for( i = 0; i < MAX_ITEM_TYPE; i++)
		uvsItemTable[i] = new uvsItemType(&pf);
	pf.finit();

	// mechos initing
	pf.init(PrmFileName[7]);
	if(strcmp(PrmSignature,pf.getAtom())) ErrH.Abort(PrmWrongMsg,XERR_USER,-1,PrmFileName[7]);

	MAX_MECHOS_MAIN = atoi(pf.getAtom());
	MAX_MECHOS_RAFFA = atoi(pf.getAtom());
	MAX_MECHOS_CONSTRACTOR = atoi(pf.getAtom());
	MAX_MECHOS_TYPE = MAX_MECHOS_MAIN + MAX_MECHOS_RAFFA + MAX_MECHOS_CONSTRACTOR;
	uvsMechosTable = new uvsMechosType*[MAX_MECHOS_TYPE];

	for( i = 0; i < MAX_MECHOS_TYPE; i++){
		uvsMechosTable[i] = new uvsMechosType(&pf);
	}//  end for i

	pf.finit();

// worlds initing
//	pfile.open("save", XS_IN);
	pfile > count;

	WorldTail = NULL;
	uvsWorld* pw;
	for( i = 0; i < count; i++ ) (pw = new uvsWorld(pfile)) -> link(WorldTail);

	DollyTail = NULL;
	// dolly initing
	pfile > count;
	uvsDolly* pd;
	for( i = 0; i < count; i++)  (pd = new uvsDolly(pfile)) -> link(DollyTail);

	// escaves initing
	pfile > count;
	uvsEscave* pe;
	for( i = 0; i < count; i++)  (pe = new uvsEscave(pfile)) -> link(EscaveTail);

	// spots initing
	pfile > count;
	uvsSpot* ps;
	for( i =0; i < count; i++) (ps = new uvsSpot(pfile)) -> link(SpotTail);

	// passages initing
	pfile > count;
	uvsPassage* pp;
	for(i = 0; i < count; i++) (pp = new uvsPassage(pfile)) -> link(PassageTail);

	// bunches initing
	pfile > count;
	uvsBunch* pb;
	pf.init(PrmFileName[4]);
	if(strcmp(PrmSignature,pf.getAtom())) ErrH.Abort(PrmWrongMsg,XERR_USER,-1,PrmFileName[4]);
	for(i = 0; i < count; i++) (pb = new uvsBunch(pfile, &pf, pf.getAtom())) -> link(BunchTail);
	pf.finit();

	// TabuTask Initing
	MAX_TABUTASK = 0;
	char* atom;
	pf.init(PrmFileName[10]);
	if(strcmp(PrmSignature,pf.getAtom())) ErrH.Abort(PrmWrongMsg,XERR_USER,-1,PrmFileName[10]);
	uvsTabuTaskType* pt;
	while((atom = pf.getAtom()) != NULL) (pt = new uvsTabuTaskType(&pf,atom)) -> link(TabuTaskTypeTail);
	pf.finit();

	if (_pe_) _pe_ -> TabuTaskCount = MAX_TABUTASK - _pe_ -> TabuTaskID;
	if (_ps_) _ps_ -> TabuTaskCount = MAX_TABUTASK - _ps_ -> TabuTaskID;

	_ps_ = NULL;
	_pe_ = NULL;

	TabuTable = new uvsTabuTaskType*[MAX_TABUTASK];
	pt = (uvsTabuTaskType*)TabuTaskTypeTail;
	for( i = 0; i < MAX_TABUTASK; i++){
		TabuTable[i] = pt;
		pt = (uvsTabuTaskType*)pt -> next;
	}

	// stationary objects linking
	pw = (uvsWorld*)WorldTail;
	while(pw){
		i = 0;

		if (pw -> escTmax){
			pw -> escT = new uvsEscave*[pw -> escTmax];
			pe = (uvsEscave*)EscaveTail;
			while(pe){
				if(pe -> Pworld == pw) pw -> escT[i++] = pe;
				pe = (uvsEscave*)pe -> next;
				}
		}

		i = 0;
		if (pw -> sptTmax){
			pw -> sptT = new uvsSpot*[pw -> sptTmax];
			ps = (uvsSpot*)SpotTail;
			while(ps){
				if(ps -> Pworld == pw) pw -> sptT[i++] = ps;
				ps = (uvsSpot*)ps -> next;
				}
		}

		i = 0;
		pw -> pssT = new uvsPassage*[pw -> pssTmax];
		pp = (uvsPassage*)PassageTail;
		while(pp){
			if(pp -> Pworld == pw) pw -> pssT[i++] = pp;
			pp = (uvsPassage*)pp -> next;
			}
		pw = (uvsWorld*)pw -> next;
		}

	// vangers initing
	pfile > cVngNumber;

	pe = (uvsEscave*)EscaveTail;
	uvsVanger* v;
	int j = 0;

	Gamer = NULL;
	for(i = 0;i < cVngNumber;i++){
		(v = new uvsVanger(pfile));
		if( v -> shape == UVS_VANGER_SHAPE::GAMER  ){
			Gamer = v;
			v -> shape = UVS_VANGER_SHAPE::GAMER;
		}
		v -> elink(ETail);
	}

	if (!Gamer) ErrH.Abort("uniVangLoad : Gamer not FOUND");

	uvsElement* e = ETail;
	while(e){
		if ( e -> type == UVS_OBJECT::VANGER){
			v = (uvsVanger*)e;
			if (v -> shape == UVS_VANGER_SHAPE::KILLER )
				v -> go_to_GAMER();
		}
		e = e -> enext;
	}
	Gamer -> UpdateAllTarget();

	pt = (uvsTabuTaskType*)TabuTaskTypeTail;

	for(i = 0; i < MAX_TABUTASK; i++){
		int _i_;
		pfile > pt -> status;

		for(j = 0; j < pt -> item_number; j++)
			pt -> real_count[j] = 0;

		if (uvsLoadVersion < 3 || (i >= WorldTable[2] -> escT[0] -> TabuTaskID && uvsLoadVersion < 6) )
//		if (uvsLoadVersion < 3 )
			pfile > *pt -> real_count;
		else {
			for(j = 0; j < pt -> item_number; j++)
				pfile > pt -> real_count[j];
		}

		if (pt -> target == UVS_TARGET::VANGER && pt -> status){
			pfile > _i_;
			uvsElement* p = ETail;
			uvsVanger* v;
			while(p){
				if(p -> type == UVS_OBJECT::VANGER){
					v = (uvsVanger*)p;
					if(v -> gIndex == _i_ ) {
						pt -> Ptarget = (uvsTarget*)v;
						break;
					}//  end if
				}//  end if
				p = p -> enext;
			}//  end while
		}//  end if
		pt = (uvsTabuTaskType*)pt -> next;
	}
	int local;
	for(i = MAX_TABUTASK; i < MAX_TABUTASK_RESERVED; i++){
		pfile > local;
		pfile > local;
	}

#ifdef _ROAD_
	pfile > CurrentWorld;
	pfile > lastMechos;

	//Gamer = v;

	int ObjNum = 0;
	pp = (uvsPassage*)PassageTail;
	while(pp){
		ObjNum++;
		pp = (uvsPassage*)pp -> next;
		}
	pe = (uvsEscave*)EscaveTail;
	while(pe){
		ObjNum++;
		pe -> link_good();
		pe = (uvsEscave*)pe -> next;
		}
	ps = (uvsSpot*)SpotTail;
	while(ps){
		ObjNum++;
		ps -> link_good();
		ps = (uvsSpot*)ps -> next;
		}

	//AllocateGamePort(ObjNum);
#else
	CurrentWorld = -1;
#endif
#ifdef _ROAD_
	if (Gamer -> Pworld -> escTmax){
		uvsCurrentCycle = Gamer -> Pworld -> escT[0] -> Pbunch -> currentStage;
		if (Gamer -> Pworld -> escT[0] -> Pbunch -> status == UVS_BUNCH_STATUS::UNABLE){
			uvsCurrentWorldUnable = 1;
			uvsCurrentWorldUnableBefore = 1;
		} else {
			uvsCurrentWorldUnable = 0;
			uvsCurrentWorldUnableBefore = 0;
		}
	} else {
		uvsCurrentCycle = 0;
		uvsCurrentWorldUnable = 0;
		uvsCurrentWorldUnableBefore = 0;
	}
#endif

	ConTimer.load(pfile);
	GamerResult.Load(pfile);
	uvsMechosLoad(pfile);
	
	/*for( i = 0; i < MAX_ITEM_TYPE; i++) {
		if (uvsItemTable[i] -> gamer_use) {
			std::cout<<"gamer use item:"<<uvsItemTable[i]->type<<std::endl;
		}
	}*/

	if (uvsLoadVersion > 10)
		pfile > uvsPoponkaID;

	if (uvsPoponkaID >= DG_POPONKA_MAX || uvsPoponkaID < 0)
		uvsPoponkaID = RND(DG_POPONKA_MAX);
	//pfile.close();


#ifdef STAND_REPORT
	stand < ConTimer.GetTime() < "  LOAD STAND\n";
#endif

	ReadPriceList();

	//XCon < "Worlds Loaded...\n\n";

	CloseCyclicPal();
	OpenCyclicPal();
};

void uniVangDelete(void){
	int i;

	uvsElement* ET;
	listElem *le;
	
	
	while (ETail){
		ET = ETail;
		ETail = ETail -> enext;
		if ( ET -> type == UVS_OBJECT::VANGER)
			delete ((uvsVanger*)ET);
		else
			delete ((uvsDolly*)ET);
	}
	ETail = NULL;

	if ( MAX_ITEM_TYPE ){
		for( i = 0; i < MAX_ITEM_TYPE; i++)
			delete uvsItemTable[i];
		delete[] uvsItemTable;
	}

	if ( MAX_MECHOS_TYPE ){
		for( i = 0; i < MAX_MECHOS_TYPE; i++)
			delete uvsMechosTable[i];
		delete[] uvsMechosTable;
	}

	while (WorldTail){
		le = WorldTail;
		WorldTail = WorldTail -> next;
		delete ((uvsWorld*)le);
	}

	WorldTail = NULL;

	DollyTail = NULL;

	while (EscaveTail){
		le = EscaveTail;
		EscaveTail = EscaveTail -> next;
		delete ((uvsEscave*)le);
	}
	EscaveTail = NULL;

	while (SpotTail){
		le = SpotTail;
		SpotTail = SpotTail -> next;
		delete (uvsSpot*)le;
	}
	SpotTail = NULL;

	while (PassageTail){
		le = PassageTail;
		PassageTail = PassageTail -> next;
		delete (uvsPassage*)le;
	}
	PassageTail = NULL;

	while (BunchTail){
		le = BunchTail;
		BunchTail = BunchTail -> next;
		delete ((uvsBunch*)le);
	}
	BunchTail = NULL;

	while (TabuTaskTypeTail){
		le = TabuTaskTypeTail;
		TabuTaskTypeTail = TabuTaskTypeTail -> next;
		delete ((uvsTabuTaskType*)le);
	}
	TabuTaskTypeTail = NULL;

	delete[] TabuTable;

//	CloseCyclicPal();
};

uvsShop::uvsShop(XStream& pfile){
	int i;
	Pmechos = NULL;
	Pitem = NULL;
	int count = 0;
	locked = 0;

	pfile > count;

	uvsMechos *pm;
	for(i = 0; i < count; i++){
		(pm = new uvsMechos(pfile)) -> link(Pmechos);
	}

	pfile > count;

	uvsItem *pi;
	for( i = 0; i < count; i++){
		(pi = new uvsItem(pfile)) -> link(Pitem);
	}

	Price = new int[MAX_ITEM_TYPE*3];
	ItemHere = new char[MAX_ITEM_TYPE];

	memset( Price, 0, MAX_ITEM_TYPE*3*sizeof(int));
	if (uvsLoadVersion > 0){
		for( int i = 0; i < MAX_ITEM_TYPE; i++)
			pfile > Price[ MAX_ITEM_TYPE*2 + i];
	} else {
		int i;
		for( i = 0; i < MAX_ITEM_TYPE; i++)
			Price[ MAX_ITEM_TYPE*2 + i] = (RND(3) << 16) + RND(2);

		for( i = 0; i < 64; i++)
			pfile > Price[ MAX_ITEM_TYPE*2 + i];
	}

	if (uvsLoadVersion > 9){
		for( int i = 0; i < MAX_ITEM_TYPE; i++)
			pfile > ItemHere[i];
	} else {
		memset(ItemHere,(char)1, MAX_ITEM_TYPE);
	}
}

uvsShop::~uvsShop(void){
	listElem* le;

	while( Pmechos){
		le = Pmechos;
		Pmechos = Pmechos -> next;
		delete ((uvsMechos*)le);
	}
	while( Pitem){
		le = Pitem;
		Pitem = Pitem -> next;
		delete ((uvsItem*)le);
	}

	delete[] Price;
	delete[] ItemHere;
}

void uvsShop::save(XStream& pfile){
	int i;
	int count = 0;

	listElem *pm = Pmechos;
	while(pm) {
		count++;
		pm = pm -> next;
	}
	pfile < count;
	pm = Pmechos;

	for(i = 0; i < count; i++){
		((uvsMechos*)pm) -> save(pfile);
		pm = pm -> next;
	}

	count = 0;

	pm = Pitem;
	while(pm) {
		count++;
		pm = pm -> next;
	}
	pfile < count;
	pm = Pitem;

	for(  i = 0; i < count; i++){
		((uvsItem*)pm) -> save(pfile);
		pm = pm -> next;
	}

	for( i = 0; i < MAX_ITEM_TYPE; i++)
		pfile < Price[ MAX_ITEM_TYPE*2 + i];

	for( i = 0; i < MAX_ITEM_TYPE; i++)
		pfile < ItemHere[i];

}

uvsMechos::uvsMechos(XStream& pfile){
	pfile > type;
	pfile > color;
	pfile > status;
	pfile > teleport;
}

void uvsMechos::save(XStream& pfile){
	pfile < type;
	pfile < color;
	pfile < status;
	pfile < teleport;
}

uvsItem::uvsItem(XStream& pfile){
	pfile > pos_x;
	pfile > pos_y;
	pfile > pos_z;
	pfile > type;

#ifdef _DEBUG
	if (uvsItemTable[type] -> type == UVS_ITEM_STATUS::MECHOS_PART)
		XCon <= type < "\n";
#endif

	if ((uvsLoadVersion < 3)&&(uvsItemTable[type] -> type == UVS_ITEM_STATUS::DEVICE)){
		pfile > param2;
		pfile > param1;
	} else {
		pfile > param1;
		pfile > param2;
	}
}

int uvsItem::ActIntType(void){
	int i;

	i = uvsItemTable[type] -> SteelerTypeEmpty;

	if (type  == UVS_ITEM_TYPE::TABUTASK){
		if (param2 & ((int)(UVS_TABUTASK_STATUS::GOOD)<<16))
			return TABUTASK_GOOD;

		if (param2 & ((int)(UVS_TABUTASK_STATUS::BAD)<<16))
			return TABUTASK_BAD;
	}

	if( (param1 || param2 )&& (uvsItemTable[type] -> type != UVS_ITEM_STATUS::GOODS))
		i = uvsItemTable[type] -> SteelerTypeFull;
	return i;
}

uvsVanger::~uvsVanger(void){
	//if(orderT)delete orderT;
	//(Pmechos)delete (uvsMechos*)Pmechos;
	FreeList(Pitem);
}

void uvsItem::save(XStream& pfile){
	pfile < pos_x;
	pfile < pos_y;
	pfile < pos_z;
	pfile < type;
	pfile < param1;
	pfile < param2;
}

int uvsItem::GetCoef(void){
	uvsEscave *pe = (uvsEscave*)EscaveTail;
	uvsSpot *ps = (uvsSpot*)SpotTail;

	while(pe){
		int coef = pe -> ItemCoef( type );
		if (coef)
			return coef;
		pe = (uvsEscave*)pe -> next;
	}

	while(ps){
		int coef = ps -> ItemCoef( type );
		if (coef)
			return coef;
		ps = (uvsSpot*)ps -> next;
	}

//	return -1;
	return 2;
};

void uvsWorld::generate_item(int n){
	int i;
	uvsItem *pi;

	if (!escTmax) return;

	if ( uvsItemTable[n] -> type != UVS_ITEM_STATUS::TREASURE ){
		int count = uvsItemTable[n] -> count;

//		if ( n == UVS_ITEM_TYPE::FUNCTION83 && NetworkON) count = 2;

		if ( count <= 0 ) return;
//		if (count == 0) count = 2;
		if ( uvsItemTable[n] -> type == UVS_ITEM_STATUS::AMMO) count = 20;
		if ( uvsItemTable[n] -> type == UVS_ITEM_STATUS::WEAPON) count = 1;

		for( i = 0; i < count; i++){
			if ( i >= (count/2)){
				int _i_ = RND(escTmax);
				escT[_i_] -> Pshop -> ItemHere[n] = 1;
				(pi = new uvsItem(n)) -> link(escT[_i_] -> Pshop -> Pitem);
			} else {
				int _i_ = RND(sptTmax);
				sptT[_i_] -> Pshop -> ItemHere[n] = 1;
				(pi = new uvsItem(n)) -> link(sptT[_i_] -> Pshop -> Pitem);
			}
#ifdef _ROAD_
			/*if( uvsItemTable[pi -> type] -> type == UVS_ITEM_STATUS::DEVICE ) pi -> param1 = UVS_DEVICE_POWER;
			if( uvsItemTable[pi -> type] -> type == UVS_ITEM_STATUS::AMMO ) pi -> param2 = uvsItemTable[pi -> type] -> count;
			if ( pi -> type == UVS_ITEM_TYPE::TERMINATOR ) pi -> param2 = UVS_TERMINATOR_POWER;
			if ( pi -> type == UVS_ITEM_TYPE::TERMINATOR2 ) pi -> param2 = UVS_TERMINATOR_POWER;*/
#endif
		}
	}
}

/*
uvsCrypt::uvsCrypt(PrmFile* pfile){
	name = strdup( pfile -> getAtom());
	type = atoi(pfile -> getAtom());
	pos_x = atoi(pfile -> getAtom());
	pos_y = atoi(pfile -> getAtom());
}
*/
uvsCrypt::~uvsCrypt(void){
//	if(name) free(name);
}

uvsCrypt::uvsCrypt( XStream &pfile ) {
//	name = get_string(pfile);
	pfile >  type;
	pfile >  pos_x;
	pfile >  pos_y;
	pfile >  pos_z;
}

void uvsCrypt::save( XStream &pfile ) {
	//put_string(pfile, name);
	pfile <  type;
	pfile <  pos_x;
	pfile <  pos_y;
	pfile <  pos_z;
}

char* get_string(XStream& pfile){
	char n;
	char *s;

	pfile > n;
	if ( n == 0 ) return NULL;
	s = new char[n+1];
	pfile.read(s, n);
	s[n] = 0;
	return s;
}

void put_string(XStream& pfile, char* s){
	char n = strlen(s);

	pfile < n;
	pfile.write(s, n);
}

int get_power(int t){
	int i, n;
	i = 0;
	n = 1;

	while( t != n ){
		i++;
		n <<= 1;
	}
	return i;
}

uvsItem* GetItem_as_type( listElem* pe,  int status, int work, int index){
	if ( !pe ) return NULL;

	while(1){
			while( uvsItemTable[((uvsItem*)pe)  -> type] -> type != status && pe -> next ){
				pe = pe -> next;
			}
			if(uvsItemTable[((uvsItem*)pe) -> type] -> type == status){
				if( !work ) return (uvsItem*)pe;
				else if ( ((uvsItem*)pe) -> param2 == index ) return (uvsItem*)pe;
			}
			if( !(pe -> next) ) return NULL;
			pe  = pe -> next;
	}
}

uvsItem* GetItem(listElem* pe, int inType, int work){
	if ( !pe ) return NULL;
	while(1){
			while( ((uvsItem*)pe) -> type != inType && pe -> next ){
				pe = pe -> next;
			}
			if(((uvsItem*)pe) -> type == inType){
				if( !work ) return (uvsItem*)pe;
				else if ( !(((uvsItem*)pe) -> param1) ) return (uvsItem*)pe;
			}
			if( !(pe -> next) ) return NULL;
			pe  = pe -> next;
	}
}

#ifdef _ROAD_
/*void goNextWorld(void)
{
	uvsWorld* aim = Gamer -> Pworld -> pssT[RND(Gamer -> Pworld -> pssTmax)] -> Poutput;

	ChangeWorld(aim -> gIndex);

	addVanger(Gamer,aim -> getPassage(Gamer -> Pworld),1);
	uvsWorldReload(aim -> gIndex);
	Gamer -> Ppassage = aim -> getPassage(Gamer -> Pworld);
	Gamer -> Pworld = aim;
} */
#endif

int ActInt_param2(int InType){
	return uvsItemTable[ActInt_to_Item(InType)] -> param2;
}

int ActInt_to_Item( int InType){
	int i;

	if (InType == TABUTASK_GOOD)
		return UVS_ITEM_TYPE::TABUTASK;

	if (InType == TABUTASK_BAD)
		return UVS_ITEM_TYPE::TABUTASK;

	for( i = 0; i < MAX_ITEM_TYPE; i++)
		if( uvsItemTable[i] -> SteelerTypeFull == InType ) return i;

	for( i = 0; i < MAX_ITEM_TYPE; i++)
		if( (uvsItemTable[i] -> SteelerTypeEmpty) == InType ) return i;

	return -1;
}

int uvsChangeGoodsParam( int type, int& param1, int& param2, int world){
	if( uvsItemTable[type] -> type == UVS_ITEM_STATUS::GOODS ) {
		if ((param1>>16) == world) {
			param1++;
			if (type == UVS_ITEM_TYPE::TABUTASK && ((param2 & 0xFFFF0000) == 0) )
				return TabuTable[param2] -> checkCycleEvent(param1 & 0x0000FFFF, param2);
		}// end if
	}//  end if
	return 0;
}

int uvsChangeTabuTask( int& param1, int& param2, int type, int status){
	
	if ( param2 == type){
		if (status == UVS_TABUTASK_STATUS::BAD){
//			ShowTaskMessage(-TabuTable[ param2 & 0x0000FFFF] -> luck);

			GamerResult.luck -= TabuTable[ param2 & 0x0000FFFF] -> luck;
			if (GamerResult.luck < 0) GamerResult.luck = 0;
		} else {
//			ShowTaskMessage(TabuTable[ param2 & 0x0000FFFF] -> luck);

			GamerResult.luck += TabuTable[ param2 & 0x0000FFFF] -> luck;
			if (GamerResult.luck > 100) GamerResult.luck = 100;
		}

		param2 |= (status <<16);
		return param2;
	}
	return 0;
}

int uvsMechosType_to_AciInt(int type){
	int first_constr = MAX_MECHOS_TYPE - MAX_MECHOS_CONSTRACTOR;
	int _type;
	int _type_;

	if ( type >= first_constr){
		 _type = type - first_constr;
		 _type_ =  first_constr + (_type<<2) + uvsMechosTable[type] -> constractor;
	} else
		_type_ = type;

	return _type_;
}

int GetItem_as_name( char* s ){
	int i;
	for(i = 0; i < MAX_ITEM_TYPE; i++) if (!strcmp(s, uvsItemTable[i] -> name)) return i;
	return -1;
}

int GetMechos_as_name( char* s ){
	int i;
	for(i = 0; i < MAX_MECHOS_TYPE; i++)
		if (!strcmp(s, uvsMechosTable[i] -> name)) return i;
	return -1;
}

int ItemCount(listElem* Pitem, int ItemType, int what ){
	int tmp = 0;
	uvsItem *pi;

	if (  UVS_ITEM_TYPE::PIPETKA == ItemType || ItemType == UVS_ITEM_TYPE::KERNOBOO)
		what = 1;

	pi = GetItem(Pitem, ItemType, 0);

	while( pi ){
		if (what)
			tmp += pi -> param2;
		else
			tmp++;
		pi = GetItem( pi -> next, ItemType, 0 );
	}
	return tmp;
}

int uvsPoponkaIDON(listElem* Pitem){
	uvsItem *pi;

	pi = GetItem(Pitem, UVS_ITEM_TYPE::POPONKA, 0);

	while( pi ){
		if (pi ->param2 == uvsPoponkaID)
			return 1;
		pi = GetItem( pi -> next, UVS_ITEM_TYPE::POPONKA, 0 );
	}
	return 0;
}

void WaitGameInTown(int cWorld){
	int ww;
	if ( (ww = WorldTable[cWorld] -> escT[0] -> Pbunch -> game_exit()) != -1 ){
		uvsGamerWaitGame = 1; 
		if (ww != WorldTable[ cWorld ] -> escT[0] -> Pbunch -> currentStage){
			while( WorldTable[ cWorld ] -> escT[0] -> Pbunch -> currentStage != ww )
				uvsQuant();
		} else {
			uvsEndHarvest = 0;
			while( !uvsEndHarvest )
				uvsQuant();
		}
	}
}

void WaitEndGameInTown(int cWorld){
	int ww;
	if ( (ww = WorldTable[cWorld] -> escT[0] -> Pbunch -> game_wait_end()) != -1 ){
		if (ww != WorldTable[ cWorld ] -> escT[0] -> Pbunch -> currentStage){
			while( WorldTable[ cWorld ] -> escT[0] -> Pbunch -> currentStage != ww )
				uvsQuant();
		} else {
			uvsEndHarvest = 0;
			while( !uvsEndHarvest )
				uvsQuant();
		}
	}
}


/*void WaitGame( int World, int k ){
	while( WorldTable[ World ] -> escT[0] -> Pbunch -> currentStage != k )
		uvsQuant();
}*/

void ReadPriceList( void  ){
	char *atom;
	int i;
	uvsEscave* pe;
	uvsSpot* ps;

	PrmFile pfile;
	pfile.init(PrmFileName[8]);

	if(strcmp(PrmSignature,pfile.getAtom())) ErrH.Abort(PrmWrongMsg,XERR_USER,-1,PrmFileName[8]);

	while( 1 ){
		if((atom = pfile.getAtom()) == NULL) break;

		for( i = 0; i < MAX_ITEM_TYPE; i++)
			if ( !strcmp( atom, uvsItemTable[i] -> name) ) break;

		if ( i == MAX_ITEM_TYPE ) {
			pe = (uvsEscave*)EscaveTail -> seekName( atom );
			ps = (uvsSpot*)SpotTail -> seekName( atom );

			if ( !ps && !pe ) ErrH.Abort(PrmWrongMsg,XERR_USER,-1,"Bad ITEM or Town NAME");
		} else {
			if ( pe ){
				pe -> Pshop -> Price[i] = atoi(pfile.getAtom());
				pe -> Pshop -> Price[i + MAX_ITEM_TYPE] = atoi(pfile.getAtom());
			} else if ( ps ){
				ps -> Pshop -> Price[i] = atoi(pfile.getAtom());
				ps -> Pshop -> Price[i + MAX_ITEM_TYPE] = atoi(pfile.getAtom());
			} else ErrH.Abort(PrmWrongMsg,XERR_USER,-1,"None Town Name");
		}
	}  //  end while

	pfile.finit();
}

int thief_on_world( void ){
	uvsElement* p = ETail;
	uvsVanger* v;
	uvsItem *pi;

	while(p){
		if(p -> type == UVS_OBJECT::VANGER ){
			v = (uvsVanger*)p;
			if ( v -> shape == UVS_VANGER_SHAPE::THIEF && v -> Pworld -> gIndex == CurrentWorld){
				if ((pi = GetItem_as_type( v -> Pitem,	UVS_ITEM_STATUS::TREASURE, 1, CurrentWorld)) != NULL ) {
//					AddTarget2Compas(CMP_TARGET_ITEM,(void*)(pi) ,"Leepuringa");
					return 1;
				}
			}
		}
		p = p -> enext;
	}
	return 0;
}

#ifdef _ROAD_
void _FirstShopPrepare(void){
		Gamer -> Pescave -> Pshop -> sellMechos(Gamer -> Pmechos);
}

void FirstShopPrepare(int how){ //znfo

	time(&uvsFirstTime);
	if( !how ){
		uvsEscave* Pescave = Gamer -> Pescave;
		uvsBunchNumber = Pescave -> Pbunch -> biosNindex;

		GGamerMechos = GGamer = GMechos = GItem = NULL;
		uvsLastEscaveName = aci_curLocationName = Pescave -> name;
		uvsGoodsON = 1;

		Pescave -> Pshop -> locked = 1;
		Pescave -> add_goods_to_shop();
		CurrentWorld = Gamer -> Pworld -> gIndex;
		Gamer -> prepare_list_for_ActInt( GGamer, GGamerMechos, Pescave -> Pshop, 1);
		Pescave -> Pshop ->  prepare_list_for_ActInt( GMechos, GItem, 1);
		//CurrentWorld = 0;
	} else{
		Gamer -> prepare_shop(0);
	}
}

void FinishFirstShopPrepare(int how){ //znfo 
	if( !how){
		lastMechos = -1;
		Gamer -> get_shop(1);

/*		uvsEscave* Pescave = Gamer -> Pescave;

		Pescave -> Pshop ->  get_list_from_ActInt( GMechos, GItem);
		Pescave -> Pshop -> locked = 0;
		Pescave -> GamerVisit++;

		lastMechos = -1;

		Gamer -> get_list_from_ActInt(GGamer, GGamerMechos);

		GamerResult.leepuringa = ItemCount(Gamer -> Pitem, UVS_ITEM_TYPE::LEEPURINGA);

		GamerResult.nymbos = ItemCount(Gamer -> Pitem,UVS_ITEM_TYPE::NYMBOS );
		GamerResult.phlegma = ItemCount(Gamer -> Pitem, UVS_ITEM_TYPE::PHLEGMA);
		GamerResult.heroin = ItemCount(Gamer -> Pitem,UVS_ITEM_TYPE::HEROIN );
		GamerResult.shrub = ItemCount(Gamer -> Pitem, UVS_ITEM_TYPE::SHRUB);
		GamerResult.poponka = ItemCount(Gamer -> Pitem,UVS_ITEM_TYPE::POPONKA );
		GamerResult.toxick = ItemCount(Gamer -> Pitem, UVS_ITEM_TYPE::TOXICK);

		if (GamerResult.leepuringa){
			GamerResult.add_luck += 50;
			ShowLuckMessage(50);
		}

		if (  lastMechos != Gamer -> Pmechos -> type ) {
			if (uvsMechosTable[Gamer -> Pmechos -> type] -> constractor){
				GamerResult.dominance += 9;
				ShowDominanceMessage(9);
			} else {
				GamerResult.dominance += 5;
				ShowDominanceMessage(5);
			}

			uvsMechosTable[Gamer -> Pmechos -> type] -> gamer_use++;
#ifndef _DEMO_
			Gamer -> Pmechos -> color = Gamer -> Pworld -> escT[0] -> Pbunch -> biosNindex;
#else
			Gamer -> Pmechos -> color = CAR_COLOR[Gamer -> Pmechos -> type];
#endif
		}

		if (Gamer -> Pworld -> escTmax)
			aci_curLocationName = Gamer -> Pescave -> name;
		else
			aci_curLocationName = "";
*/
	} else {
		Gamer -> get_shop(0);
	}

//	uvsRestoreVanger();
}
#endif

int uvsSetItemType(int type,int param1,int param2){
	if ( type == -1 ) return -1;

	if (type >= MAX_ITEM_TYPE)
		ErrH.Abort("uvsSetItemType :: type >= MAX_ITEM_TYPE");

	int i = uvsItemTable[type] -> SteelerTypeEmpty;

	if (type  == UVS_ITEM_TYPE::TABUTASK){
		if (param2 & ((int)(UVS_TABUTASK_STATUS::GOOD)<<16))
			return TABUTASK_GOOD;

		if (param2 & ((int)(UVS_TABUTASK_STATUS::BAD)<<16))
			return TABUTASK_BAD;
	}

	if( (param1 || param2) && (uvsItemTable[type] -> type != UVS_ITEM_STATUS::GOODS))
		i = uvsItemTable[type] -> SteelerTypeFull;
	return i;
}

void uvsGamerResult::Init(void){
	game_result = 0;
	game_elr_result = 0;
	game_ker_result = 0;
	game_pip_result = 0;
	game_zyk_result = 0;
	game_total = 0;
	dead_eleech = 0;
	dead_eleech_now = 0;
	dead_weezyk = 0;
	get_eleech = 0;
	get_weezyk = 0;
	eleech = 0;
	nymbos = 0;
	phlegma = 0;
	heroin = 0;
	shrub = 0;
	poponka = 0;
	toxick = 0;
	pipetka = 0;
	kernoboo = 0;
	weezyk = 0;
	eleepod = 0;
	beeboorat = 0;
	zeex = 0;
	rubbox = 0;
	nymbos_total = 0;
	phlegma_total = 0;
	heroin_total = 0;
	shrub_total = 0;
	poponka_total = 0;
	toxick_total = 0;
	theif = 0;
	theif_on_world = 0;
	cirt_bad  = -1;
	cirt_good  = -1;
	dolly = 0;
//	beeb = 0;
	mechos_variant = 0;
	mechos_type = 0;
	vanger_kill = 0;
	key = 0;
	leepuringa = 0;
	protractor = 0;
	mechanic_messiah = 0;
	boot_sector = 0;
	peelot = 0;
	spummy = 0;
	fun83 = 0;
	earth_unable = 0;
	last_game_type = 0;
	luck = 15;
	dominance = 0;
	palochka = 0;
	pipka = 0;
	nobool = 0;
	boorawchick = 0;
	pereponka = 0;
	zeefick = 0;
	add_luck = 0;
	add_dominance = 0;
	dolly_cycle = -1;
	rubbox_count = 0;
	thief_leepuringa  = 0;
	thief_palochka = 0;
	thief_pipka = 0;
	thief_nobool = 0;
	thief_boorawchick = 0;
	thief_pereponka = 0;
	thief_zeefick = 0;
	passagerOk = 0;
	nymbos_bonus = 1;
	phlegma_bonus = 1;
	heroin_bonus = 1;
	shrub_bonus = 1;
	poponka_bonus = 1;
	toxick_bonus = 1;
	BoorawchickGoHimself = 0;
	unik_poponka = 0;
}

void uvsGamerResult::LocalInit(void){
	game_result = 0;
	game_total = 0;
	dead_eleech = 0;
	dead_eleech_now = 0;
	dead_weezyk = 0;
	get_eleech = 0;
	get_weezyk = 0;
	eleech = 0;
/*	nymbos = 0;
	phlegma = 0;
	heroin = 0;
	shrub = 0;
	poponka = 0;
	toxick = 0;*/
	pipetka = 0;
	kernoboo = 0;
	weezyk = 0;
	eleepod = 0;
	beeboorat = 0;
	zeex = 0;
	rubbox = 0;
	theif = 0;
	cirt_bad  = -1;
	cirt_good  = -1;
	dolly = 0;
	vanger_kill = 0;
	leepuringa = 0;
	protractor = 0;
	palochka = 0;
	pipka = 0;
	nobool = 0;
	boorawchick = 0;
	pereponka = 0;
	zeefick = 0;
	last_game_type = 0;
}

void uvsGamerResult::Save(XStream& pfile){
	pfile < game_result;
	pfile <  game_total;
	pfile <  game_elr_result;
	pfile <  game_ker_result;
	pfile <  game_pip_result;
	pfile <  game_zyk_result;
	pfile <  dead_eleech;
	pfile <  dead_eleech_now;
	pfile <  dead_weezyk;
	pfile <  get_eleech;
	pfile <  get_weezyk;
	pfile <  eleech;
	pfile <  nymbos;
	pfile <  phlegma;
	pfile <  heroin;
	pfile <  shrub;
	pfile <  poponka;
	pfile <  toxick;
	pfile <  pipetka;
	pfile <  kernoboo;
	pfile <  weezyk;
	pfile <  eleepod;
	pfile <  beeboorat;
	pfile <  zeex;
	pfile <  rubbox;
	pfile <  nymbos_total;
	pfile <  phlegma_total;
	pfile <  heroin_total;
	pfile <  shrub_total;
	pfile <  poponka_total;
	pfile <  toxick_total;
	pfile <  theif;
	pfile <  theif_on_world;
	pfile <  cirt_bad;
	pfile <  cirt_good;
	pfile <  dolly;
//	pfile <  beeb;
	pfile <  mechos_variant;
	pfile <  mechos_type;
	pfile <  vanger_kill;
	pfile <  key;
	pfile <  leepuringa;
	pfile < earth_unable;
	pfile < last_game_type;
	pfile <  luck;
	pfile <  dominance;
	pfile <  palochka;
	pfile <  pipka;
	pfile <  nobool;
	pfile <  boorawchick;
	pfile <  pereponka;
	pfile <  zeefick;
	pfile < protractor;
	pfile < mechanic_messiah;
	pfile < boot_sector;
	pfile < peelot;
	pfile < spummy;
	pfile < fun83;
	pfile < add_luck;
	pfile < add_dominance;
	pfile < dolly_cycle;
	pfile < rubbox_count;
	pfile < thief_leepuringa;
	pfile < thief_palochka;
	pfile < thief_pipka;
	pfile < thief_nobool;
	pfile < thief_boorawchick;
	pfile < thief_pereponka;
	pfile < thief_zeefick;
	pfile < passagerOk;
	pfile <  nymbos_bonus;
	pfile < phlegma_bonus;
	pfile <  heroin_bonus;
	pfile <  shrub_bonus;
	pfile < poponka_bonus;
	pfile <  toxick_bonus;
	pfile < BoorawchickGoHimself;
	pfile < unik_poponka;

	for( int i = 0; i < (100 - 32); i++)
		pfile < int(0);
}

void uvsGamerResult::Load(XStream& pfile){
	pfile > game_result;
	pfile >  game_total;
	pfile >  game_elr_result;

	if (game_elr_result < 0 ) game_elr_result = 200;
	if (game_elr_result > 1000 ) game_elr_result = 1000;

	pfile >  game_ker_result;

	if (game_ker_result < 0 ) game_ker_result = 200;
	if (game_ker_result > 1000 ) game_ker_result = 1000;

	pfile >  game_pip_result;

	if (game_pip_result < 0 ) game_pip_result = 200;
	if (game_pip_result > 1000 ) game_pip_result = 1000;

	pfile >  game_zyk_result;

	if (game_zyk_result < 0 ) game_zyk_result = 200;
	if (game_zyk_result > 1000 ) game_zyk_result = 1000;

	pfile >  dead_eleech;
	pfile >  dead_eleech_now;
	pfile >  dead_weezyk;
	pfile >  get_eleech;
	pfile >  get_weezyk;
	pfile >  eleech;
	pfile >  nymbos;
	pfile >  phlegma;
	pfile >  heroin;
	pfile >  shrub;
	pfile >  poponka;
	pfile >  toxick;
	pfile >  pipetka;
	pfile >  kernoboo;
	pfile >  weezyk;
	pfile >  eleepod;
	pfile >  beeboorat;
	pfile >  zeex;
	pfile >  rubbox;
	pfile >  nymbos_total;
	pfile >  phlegma_total;
	pfile >  heroin_total;
	pfile >  shrub_total;
	pfile >  poponka_total;
	pfile >  toxick_total;
	pfile >  theif;
	pfile >  theif_on_world;
	pfile >  cirt_bad;
	pfile >  cirt_good;
	pfile >  dolly;
//	pfile >  beeb;
	pfile >  mechos_variant;
	pfile >  mechos_type;
	pfile >  vanger_kill;
	pfile >  key;
	pfile >  leepuringa;
	pfile > earth_unable;
	pfile > last_game_type;
	pfile >  luck;
	pfile >  dominance;
	pfile >  palochka;
	pfile >  pipka;
	pfile >  nobool;
	pfile >  boorawchick;
	pfile >  pereponka;
	pfile >  zeefick;
	pfile > protractor;
	pfile > mechanic_messiah;
	pfile > boot_sector;
	pfile > peelot;
	pfile > spummy;
	pfile > fun83;
	pfile > add_luck;
	pfile > add_dominance;
	pfile > dolly_cycle;
	pfile > rubbox_count;
	pfile > thief_leepuringa;
	pfile > thief_palochka;
	pfile > thief_pipka;
	pfile > thief_nobool;
	pfile > thief_boorawchick;
	pfile > thief_pereponka;
	pfile > thief_zeefick;
	pfile > passagerOk;
	pfile >  nymbos_bonus;
	pfile > phlegma_bonus;
	pfile >  heroin_bonus;
	pfile >  shrub_bonus;
	pfile > poponka_bonus;
	pfile >  toxick_bonus;
	pfile > BoorawchickGoHimself;
	pfile > unik_poponka;

	int local;
	for( int i = 0; i < (100-32); i++)
		pfile > local;
}

#ifndef _ROAD_
void AVIclose( void* ){}
#endif

extern int ChangeWorldSkipQuant;

void uvsPrepareQuant(void){
	if (!ChangeWorldSkipQuant)
		GeneralSystemClose();

		Gamer -> prepare_shop(1);

		MLReset();
		vMap->release();

		//status = UVS_VANGER_STATUS::IN_SHOP;
		int lastWorld = CurrentWorld;
		CurrentWorld = -1;

		if ( GamerResult.game_result ){
			WaitEndGameInTown(lastWorld);
		}

		uvsChangeRaiting();

		CurrentWorld = lastWorld;
		if ( Gamer -> Pworld -> escTmax){
			uvsCurrentCycle = Gamer -> Pworld -> escT[0] -> Pbunch -> currentStage;

			if (Gamer -> Pworld -> escT[0] -> Pbunch -> status == UVS_BUNCH_STATUS::UNABLE)
				uvsCurrentWorldUnable = 1;
			else
				uvsCurrentWorldUnable = 0;
		} else {
			uvsCurrentCycle = 0;
			uvsCurrentWorldUnable = 0;
		}
}

extern int GeneralSystemSkip;

void uvsCloseQuant(void){
	//if (!uvsTownName){
		LoadingMessage(1);
//		WorldTable[CurrentWorld] -> updateCrypt();
		GeneralSystemSkip = 1;
	//} else {
	//	GeneralSystemSkip = 0;
	//}

	GeneralMapReload = 1;
	GeneralSystemOpen();
	
	if (WorldTable[CurrentWorld]-> escTmax){
		uvsCurrentCycle = WorldTable[CurrentWorld] -> escT[0] -> Pbunch -> currentStage;
		if (Gamer -> Pworld -> escT[0] -> Pbunch -> status == UVS_BUNCH_STATUS::UNABLE)
			uvsCurrentWorldUnable = 1;
		else
			uvsCurrentWorldUnable = 0;
	} else {
		uvsCurrentCycle = 0;
		uvsCurrentWorldUnable = 0;
	}

	if (WorldTable[CurrentWorld] -> escTmax){
		WorldPalCurrent = uvsCurrentCycle;
		CloseBunchPal();
		memcpy(actIntPal,WorldPalData[WorldPalCurrent],768);
		memcpy(palbuf,actIntPal,768);
		memcpy(palbufOrg,actIntPal,768);
		memcpy(palbufSrc,actIntPal,768);
		light_modulation  = WorldLightParam[CurrentWorld][WorldPalCurrent];
	}

	Gamer -> get_shop(1);

	if (WorldTable[CurrentWorld] -> escTmax)
		aci_curLocationName = WorldTable[CurrentWorld] -> escT[0] -> name;
	else
		aci_curLocationName = "";

	if (!uvsTownName) {
		uvsRestoreVanger();
	} else {
		uvsTownName = NULL;
	}
}

void uvsMechosSave(XStream& fin){
	int i;

	for( i = MAX_MECHOS_TYPE - MAX_MECHOS_CONSTRACTOR; i < MAX_MECHOS_TYPE; i++)
		fin < uvsMechosTable[i] -> constractor;

	for( i = 0; i < MAX_MECHOS_TYPE; i++){
		fin < uvsMechosTable[i] -> gamer_use;
		fin < uvsMechosTable[i] -> gamer_kill;
	}

	for( i = 0; i < MAX_ITEM_TYPE; i++)
		fin < uvsItemTable[i] -> gamer_use;
}

void uvsMechosLoad(XStream& fout){
	int i;

	for( i = MAX_MECHOS_TYPE - MAX_MECHOS_CONSTRACTOR; i < MAX_MECHOS_TYPE; i++)
		fout > uvsMechosTable[i] -> constractor;

	for( i = 0; i < MAX_MECHOS_TYPE; i++){
		fout > uvsMechosTable[i] -> gamer_use;
		fout > uvsMechosTable[i] -> gamer_kill;
	}

	if (uvsLoadVersion > 0){
		for( i = 0; i < MAX_ITEM_TYPE; i++)
			fout > uvsItemTable[i] -> gamer_use;
	} else {
		for( i = 0; i < 64; i++)
			fout > uvsItemTable[i] -> gamer_use;
	}
}

void uvsMechos::sort(void){
	status &= UVS_MECHOS_USED::USES;
	if (type <  MAX_MECHOS_MAIN){

		if (uvsMechosTable[type] ->type == UVS_CAR_TYPE::ATW || uvsMechosTable[type] ->type == UVS_CAR_TYPE::TRACK || uvsMechosTable[type] ->type == UVS_CAR_TYPE::MICROBUS) {
			status |= UVS_MECHOS_USED::KILLER;
			status |= UVS_MECHOS_USED::CARAVAN;
		}

		status |= UVS_MECHOS_USED::RACE;
		status |= UVS_MECHOS_USED::HARVEST;
	}
}

void uvsCreateNewCrypt(int x, int y, int z, int type, int iWorld){
//	(new uvsCrypt(x, y, z, type)) -> link(WorldTable[iWorld] -> Pcrypt);
}

void uvsCreateNewItem(int x, int y, int z, int type, int iWorld){
	(new uvsItem(x, y, z, type)) -> link(WorldTable[iWorld] -> Pitem);
}

void uvsCreateItem_in_Crypt(void){
/*		WorldTable[CurrentWorld] -> getItemFromCrypt(UVS_CRYPT::DEVICE);
		WorldTable[CurrentWorld] -> getItemFromCrypt(UVS_CRYPT::AMMO);
		WorldTable[CurrentWorld] -> getItemFromCrypt(UVS_CRYPT::ARMO_LIGHT);
		WorldTable[CurrentWorld] -> getItemFromCrypt(UVS_CRYPT::ARMO_HEAVY);*/
}

int  uvsCatchDolly(uvsDolly* pd){
	uvsElement* p = ETail;
	uvsVanger* v;
	uvsOrder* ord;

	while(p){
		if(p -> type == UVS_OBJECT::VANGER ){
			v = (uvsVanger*)p;
			if ( v -> shape != UVS_VANGER_SHAPE::GAMER ){
				ord = v -> orderT;

				while(ord -> target == UVS_TARGET::PASSAGE) ord++;

				if (ord -> Ptarget == ((uvsTarget*)(pd)) )
					v -> go_to_GAMER();
			}//  end if
		}//  end if
		p = p -> enext;
	}//  end while
	pd -> status = UVS_DOLLY_STATUS::CATCH;
	SOUND_LARVA()

	WorldTable[pd -> biosNindex] -> escT[0] -> Pbunch -> status |= (int)UVS_BUNCH_STATUS::KILL_GAMER;

#ifdef STAND_REPORT
	stand < ConTimer.GetTime();
	stand < "  Catch Dolly " <= pd -> gIndex < " from world " < pd -> Pworld -> name < "\n";
#endif

	return pd -> gIndex;
}

int  uvsFreeDolly(int _x, int _y, int _index_){
	listElem* pd = DollyTail;

	while(pd){
		if (((uvsDolly*)pd) -> gIndex == _index_){
			if (CurrentWorld > 3){
//				((uvsDolly*)pd) -> status = UVS_DOLLY_STATUS::UNABLE;
				((uvsDolly*)pd) -> status = 0;
				uvsCheckKronIventTabuTask(UVS_KRON_EVENT::KILL_DOLLY, ((uvsDolly*)pd) -> biosNindex);
			} else
				((uvsDolly*)pd) -> status = 0;

			((uvsDolly*)pd) -> Pworld = WorldTable[CurrentWorld];
			((uvsDolly*)pd) -> pos_x = _x;
			((uvsDolly*)pd) -> pos_y = _y;
			((uvsDolly*)pd) -> setSpeed();

#ifdef STAND_REPORT
		stand < ConTimer.GetTime();
		stand < "  Free Dolly " <= ((uvsDolly*)pd)  -> gIndex < " in world " < WorldTable[CurrentWorld] -> name < "\n";
#endif
			GamerResult.luck -= 10;
			ShowLuckMessage(-10);
			if (GamerResult.luck < 0) GamerResult.luck = 0;
			return 0;
		}
		pd = pd -> next;
	}

	ErrH.Abort("uvsFreeDolly : dont find dolly");
	return 0;
}

int uvsgetDGdata(int code){
	switch( code ){
	case DG_EXTERNS::INV_UNIQUE_POPONKA:
			return GamerResult.unik_poponka;
			break;
		case DG_EXTERNS::ELR_RESULT:
//			if (GamerResult.get_eleech)
				return GamerResult.game_result;
//			else
//				return 0;
			break;
		case DG_EXTERNS::ELR_TOTAL:
			return (GamerResult.game_total);
			break;
		case DG_EXTERNS::ELR_RATING:
			return (GamerResult.game_elr_result);
			break;
		case DG_EXTERNS::KER_RESULT:
			return GamerResult.game_result;
			break;
		case DG_EXTERNS::KER_TOTAL:
			return (GamerResult.game_total);
			break;
		case DG_EXTERNS::KER_RATING:
			return (GamerResult.game_ker_result);
			break;
		case DG_EXTERNS::PIP_RESULT:
			return GamerResult.game_result;
			break;
		case DG_EXTERNS::PIP_TOTAL:
			return (GamerResult.game_total);
			break;
		case DG_EXTERNS::PIP_RATING:
			return (GamerResult.game_pip_result);
			break;
		case DG_EXTERNS::ZYK_RESULT:
			return GamerResult.game_result;
			break;
		case DG_EXTERNS::ZYK_TOTAL:
			return (GamerResult.game_total);
			break;
		case DG_EXTERNS::ZYK_RATING:
			return (GamerResult.game_zyk_result);
			break;
		case DG_EXTERNS::INV_BAD_CIRT:
			return (GamerResult.cirt_bad);
			break;
		case DG_EXTERNS::INV_GOOD_CIRT:
			return (GamerResult.cirt_good);
			break;
		case DG_EXTERNS::INV_NYMBOS:
			return (GamerResult.nymbos);
			break;
		case DG_EXTERNS::INV_PHLEGMA:
			return (GamerResult.phlegma);
			break;
		case DG_EXTERNS::INV_HEROIN:
			return (GamerResult.heroin);
			break;
		case DG_EXTERNS::INV_SHRUB:
			return (GamerResult.shrub);
			break;
		case DG_EXTERNS::INV_POPONKA:
			return (GamerResult.poponka);
			break;
		case DG_EXTERNS::INV_TOXICK:
			return (GamerResult.toxick);
			break;
		case DG_EXTERNS::INV_PIPETKA:
			return (GamerResult.pipetka);
			break;
		case DG_EXTERNS::INV_KERNOBOO:
			return (GamerResult.kernoboo);
			break;
		case DG_EXTERNS::INV_WEEZYK:
			return (GamerResult.weezyk);
			break;
		case DG_EXTERNS::INV_ELEECH:
			return (GamerResult.eleech);
			break;
		case DG_EXTERNS::INV_DEAD_ELEECH:
			return (GamerResult.dead_eleech_now);
			break;
		case DG_EXTERNS::INV_RUBBOX:
			return (GamerResult.rubbox);
			break;
		case DG_EXTERNS::KILLED_VANGERS:
			return (GamerResult.vanger_kill);
			break;
		case DG_EXTERNS::MECHOS_CHANGED:
			return (GamerResult.mechos_variant);
			break;
		case DG_EXTERNS::MECHOS_TYPE:
			return (GamerResult.mechos_type);
			break;
		case DG_EXTERNS::NYMBOS_DELIVERY:
			return (GamerResult.nymbos_total);
			break;
		case DG_EXTERNS::PHLEGMA_DELIVERY:
			return (GamerResult.phlegma_total);
			break;
		case DG_EXTERNS::HEROIN_DELIVERY:
			return (GamerResult.heroin_total);
			break;
		case DG_EXTERNS::SHRUB_DELIVERY:
			return (GamerResult.shrub_total);
			break;
		case DG_EXTERNS::POPONKA_DELIVERY:
			return (GamerResult.poponka_total);
			break;
		case DG_EXTERNS::TOXICK_DELIVERY:
			return (GamerResult.toxick_total);
			break;
		case DG_EXTERNS::NYMBOS_BONUS:
			return (GamerResult.nymbos_total >= GamerResult.nymbos_bonus*20);
			break;
		case DG_EXTERNS::PHLEGMA_BONUS:
			return (GamerResult.phlegma_total  >= GamerResult.phlegma_bonus*20);
			break;
		case DG_EXTERNS::HEROIN_BONUS:
			return (GamerResult.heroin_total  >= GamerResult.heroin_bonus*20);
			break;
		case DG_EXTERNS::SHRUB_BONUS:
			return (GamerResult.shrub_total >=  GamerResult.shrub_bonus*20);
			break;
		case DG_EXTERNS::POPONKA_BONUS:
			return (GamerResult.poponka_total >= GamerResult.poponka_bonus*20);
			break;
		case DG_EXTERNS::TOXICK_BONUS:
			return (GamerResult.toxick_total  >= GamerResult.toxick_bonus*20);
			break;
		case DG_EXTERNS::CYCLE:
			return (uvsCurrentCycle);
			break;
		case DG_EXTERNS::BEEBS:
			return (aciGetCurCredits());
			break;
		case DG_EXTERNS::WEEZYK_ASSIGNMENT:
			return (GamerResult.get_weezyk);
			break;
		case DG_EXTERNS::INV_BAD_LARVA:
			return (GamerResult.dolly == UVS_DOLLY_TYPE::ALIEN);
			break;
		case DG_EXTERNS::LARVA_CYCLE:
			return (GamerResult.dolly_cycle);
			break;
		case DG_EXTERNS::INV_GOOD_LARVA:
			return (GamerResult.dolly == UVS_DOLLY_TYPE::OWN);
			break;
		case DG_EXTERNS::LUCK:
			if (GamerResult.luck + GamerResult.add_luck > 100)
				return 100;
			if (GamerResult.luck + GamerResult.add_luck < 0)
				return 0;
			return (GamerResult.luck + GamerResult.add_luck);
			break;
		case DG_EXTERNS::INV_MESSIAH:
			if (uvsKronActive)
				return CheckThreallMessiah();
			else
				return (GamerResult.mechanic_messiah);
			break;
		case DG_EXTERNS::INV_ELEEPOD:
			return (GamerResult.eleepod);
			break;
		case DG_EXTERNS::INV_BEEBOORAT:
			return (GamerResult.beeboorat);
			break;
		case DG_EXTERNS::INV_ZEEX:
			return (GamerResult.zeex);
			break;
		case DG_EXTERNS::INV_LEEPURINGA:
			return (GamerResult.leepuringa);
			break;
		case DG_EXTERNS::INV_PALOCHKA:
			return (GamerResult.palochka);
			break;
		case DG_EXTERNS::INV_BOORAWCHICK:
			return (GamerResult.boorawchick);
			break;
		case DG_EXTERNS::INV_NOBOOL:
			return (GamerResult.nobool);
			break;
		case DG_EXTERNS::INV_PIPKA:
			return (GamerResult.pipka);
			break;
		case DG_EXTERNS::INV_PEREPONKA:
			return (GamerResult.pereponka);
			break;
		case DG_EXTERNS::INV_ZEEFICK:
			return (GamerResult.zeefick);
			break;
		case DG_EXTERNS::INV_PROTRACTOR:
			return (GamerResult.protractor);
			break;
		case DG_EXTERNS::INV_FUNCTION83:
			return (GamerResult.fun83);
			break;
		case DG_EXTERNS::INV_BOOTSECTOR:
			return (GamerResult.boot_sector);
			break;
		case DG_EXTERNS:: INV_PEELOT:
			return (GamerResult.peelot);
			break;
		case DG_EXTERNS::INV_SPUMMY:
			return (GamerResult.spummy);
			break;
		case DG_EXTERNS::DOMINANCE:
			if (GamerResult.dominance+GamerResult.add_dominance > 100 )
				return 100;
			if (GamerResult.dominance+GamerResult.add_dominance < -100)
				return -100;
			return (GamerResult.dominance+GamerResult.add_dominance);
			break;
		case DG_EXTERNS::TABUTASKS:
			return (Gamer -> GoodTabuTask());
			break;
		case DG_EXTERNS::FRESH_TABUTASKS:
			return (Gamer -> newGoodTabuTask());
			break;
		case DG_EXTERNS::WTABUTASKS:
			return (Gamer -> GoodWorldTabuTask());
			break;
		case DG_EXTERNS::CIRT_DELIVERY:
			return (Gamer -> CirtDelivery());
			break;
		case DG_EXTERNS::THIEF_LEEPURINGA:
			return (GamerResult.thief_leepuringa);
			break;
		case DG_EXTERNS::THIEF_PALOCHKA:
			return (GamerResult.thief_palochka);
			break;
		case DG_EXTERNS::THIEF_BOORAWCHICK:
			return (GamerResult.thief_boorawchick);
			break;
		case DG_EXTERNS::THIEF_NOBOOL:
			return (GamerResult.thief_nobool);
			break;
		case DG_EXTERNS::THIEF_PIPKA:
			return (GamerResult.thief_pipka);
			break;
		case DG_EXTERNS::THIEF_PEREPONKA:
			return (GamerResult.thief_pereponka);
			break;
		case DG_EXTERNS::THIEF_ZEEFICK:
			return (GamerResult.thief_zeefick);
			break;
		case DG_EXTERNS::TRUE_PASSENGER:
			return GamerResult.passagerOk;
			break;
		case DG_EXTERNS::HERE_LEEPURINGA:
			return (uvsReturnTreasureStatus(UVS_ITEM_TYPE::LEEPURINGA, uvsTreasureInShop) ||
					uvsReturnTreasureStatus(UVS_ITEM_TYPE::LEEPURINGA, 0));
			break;
		case DG_EXTERNS::HERE_PALOCHKA:
			return (uvsReturnTreasureStatus(UVS_ITEM_TYPE::PALOCHKA, uvsTreasureInShop) ||
				uvsReturnTreasureStatus(UVS_ITEM_TYPE::PALOCHKA, 0));
			break;
		case DG_EXTERNS::HERE_BOORAWCHICK:
			return (uvsReturnTreasureStatus(UVS_ITEM_TYPE::BOORAWCHICK, uvsTreasureInShop) ||
				uvsReturnTreasureStatus(UVS_ITEM_TYPE::BOORAWCHICK, 0));
			break;
		case DG_EXTERNS::HERE_NOBOOL:
			return (uvsReturnTreasureStatus(UVS_ITEM_TYPE::NOBOOL, uvsTreasureInShop) ||
				uvsReturnTreasureStatus(UVS_ITEM_TYPE::NOBOOL, 0));
			break;
		case DG_EXTERNS::HERE_PIPKA:
			return (uvsReturnTreasureStatus(UVS_ITEM_TYPE::PIPKA, uvsTreasureInShop) ||
				uvsReturnTreasureStatus(UVS_ITEM_TYPE::PIPKA, 0));
			break;
		case DG_EXTERNS::HERE_PEREPONKA:
			return (uvsReturnTreasureStatus(UVS_ITEM_TYPE::PEREPONKA, uvsTreasureInShop) ||
				uvsReturnTreasureStatus(UVS_ITEM_TYPE::PEREPONKA, 0));
			break;
		case DG_EXTERNS::HERE_ZEEFICK:
			return (uvsReturnTreasureStatus(UVS_ITEM_TYPE::ZEEFICK, uvsTreasureInShop) ||
				uvsReturnTreasureStatus(UVS_ITEM_TYPE::ZEEFICK, 0));
			break;
		case DG_EXTERNS::BOORAWCHICK_RETURN:
			return GamerResult.BoorawchickGoHimself;
			break;
		case DG_EXTERNS::ZIGGER_ZAGGER:
			return (uvsMechosTable[19] -> constractor == 1 || uvsMechosTable[19] -> constractor == 3);
			break;
		case DG_EXTERNS::CART:
			return (uvsMechosTable[19] -> constractor == 2 || uvsMechosTable[19] -> constractor == 3);
			break;
		case DG_EXTERNS::PUT_PUT:
			return (uvsMechosTable[20] -> constractor == 1 || uvsMechosTable[20] -> constractor == 3);
			break;
		case DG_EXTERNS::BOX:
			return (uvsMechosTable[20] -> constractor == 2 || uvsMechosTable[20] -> constractor == 3);
			break;
		case DG_EXTERNS::RATTLE:
			return (uvsMechosTable[21] -> constractor == 1 || uvsMechosTable[21] -> constractor == 3);
			break;
		case DG_EXTERNS::HEAVY_IRON:
			return (uvsMechosTable[21] -> constractor == 2 || uvsMechosTable[21] -> constractor == 3);
			break;
		case DG_EXTERNS::STAR:
			return (uvsMechosTable[22] -> constractor == 1 || uvsMechosTable[22] -> constractor == 3);
			break;
		case DG_EXTERNS::WHIZZER:
			return (uvsMechosTable[22] -> constractor == 2 || uvsMechosTable[22] -> constractor == 3);
			break;
		case DG_EXTERNS::HUSKER:
			return (uvsMechosTable[23] -> constractor == 1 || uvsMechosTable[23] -> constractor == 3);
			break;
		case DG_EXTERNS::COGGED_WIDGET:
			return (uvsMechosTable[23] -> constractor == 2 || uvsMechosTable[23] -> constractor == 3);
			break;
		}// end switrh
	return 0;
}

void uvsMakeGamerFromSlave(uvsVanger* pv){
	pv -> shape = UVS_VANGER_SHAPE::GAMER;
	pv -> status = UVS_VANGER_STATUS::MOVE_TO_MECHOS;
	pv -> Pworld = Gamer -> Pworld;

	Gamer -> shape = UVS_VANGER_SHAPE::GAMER_SLAVE;
	Gamer -> status = UVS_VANGER_STATUS::MOVEMENT;
	Gamer -> gIndex = pv -> gIndex;
	pv -> gIndex = 0;
	Gamer = pv;
}

uvsVanger* uvsMakeNewGamerInEscave(uvsEscave* pe, int what ){
	uvsElement* p = ETail;
	uvsVanger* v;
	uvsMechos* pm;
	listElem* pl;

//	FreeMechosList(GGamerList);

	while(p){
		if(p -> type == UVS_OBJECT::VANGER ){
			v = (uvsVanger*)p;
			if ( v -> status == UVS_VANGER_STATUS::ESCAVE_SLEEPING || v -> status == UVS_VANGER_STATUS::SPOT_SLEEPING){
				if (what){
					v -> locTimer = 1;
					v -> shape = UVS_VANGER_SHAPE::GAMER;
					v -> status = UVS_VANGER_STATUS::SHOPPING;

					v -> Pescave = pe;
					v -> Pspot = NULL;
					v -> Pworld = pe -> Pworld;

					Gamer -> shape = UVS_VANGER_SHAPE::GAMER_SLAVE;
					Gamer -> gIndex = v -> gIndex;
					v -> gIndex = 0;
					Gamer = v;
				} else
					pe -> Pshop -> sellMechos(Gamer -> Pmechos);

				pm = (uvsMechos*)pe -> Pshop -> Pmechos;
				pl = pe -> Pshop -> Pmechos -> next;

				while(pl){
					if (uvsMechosTable[pm -> type] -> price > uvsMechosTable[((uvsMechos*)pl) -> type] -> price)
						pm =(uvsMechos*)pl;
					pl = pl -> next;
				}

				if (((listElem*)pm) == pe -> Pshop -> Pmechos ){
					pe -> Pshop -> Pmechos = pe -> Pshop -> Pmechos -> next;
					if (pe -> Pshop -> Pmechos)
						pe -> Pshop -> Pmechos -> prev = pm -> prev;
				} else {
					if (pm -> next)
						pm -> next -> prev = pm -> prev;
					else
						pe -> Pshop -> Pmechos -> prev = pm -> prev;
					pm -> prev -> next = pm -> next;
				}

				pm -> type = RND(MAX_MECHOS_RAFFA) + MAX_MECHOS_MAIN;
				Gamer -> Pmechos = pm;
				if (!Gamer -> Pmechos)
					ErrH.Abort("uvsMakeNewGamer :: dont have any mechos in shop");

//				aciCurCredits -= uvsMechosTable[pm -> type] -> price;

//				if (aciCurCredits < 0)
//					aciCurCredits = 0;

				if (what){
//					aciCurCredits -= uvsMechosTable[pm -> type] -> price;
/*					if (!Gamer -> addItem(new uvsItem(UVS_ITEM_TYPE::SECTOR)))
						ErrH.Abort("uvsMakeNewGamerInEscave : dont add artifact");*/
				}
				if (aciGetCurCredits() < 0)
					aciUpdateCurCredits(0);
				return Gamer;
			}//  if status
		}//  end if vanger
		p = p -> enext;
	}//  end while
	ErrH.Abort("uvsMakeNewGamer :: dont find free vanger");
	return NULL;
}

/*
void uvsMakeMechosSlaveList(void){
	listElem* pb = NULL;
	uvsElement* p = ETail;
	uvsVanger* v;

	while(p){
		if(p -> type == UVS_OBJECT::VANGER ){
			v = (uvsVanger*)p;
			if ( v -> shape == UVS_VANGER_SHAPE::GAMER_SLAVE && v -> Pworld == WorldTable[CurrentWorld] ){
				(new uvsMechosList(uvsMechosType_to_AciInt(v -> Pmechos -> type), v)) -> link(pb);
			}//  end if shape
		}//  end if vanger
		p = p -> enext;
	}//  end while

	if (WorldTable[CurrentWorld] -> escTmax){
		(new uvsMechosList(-1, NULL)) -> link(pb);
		aci_curLocationName = WorldTable[CurrentWorld] -> escT[0] -> name;
	}

	GGamerList = (uvsMechosList*)pb;
}
*/

const int ELEECH[5] = { 2000, 1000, 300, 100, 50};
const int PIPETKA[3] = { 1500, 1000, 500 };
const int KERNOBOO[3] = { 3000, 1000, 300};
const int WEEZYK[1] = { 5000};

void uvsChangeRaiting( void ){

	if (GamerResult.game_result < 1 )
		return;

	switch (GamerResult.last_game_type){
	case UVS_GAME_ID::ELEECH:
		if (V_RACE_VANGER >= GamerResult.game_result)
			GamerResult.game_elr_result += GAME_RAITING[ GamerResult.game_result-1];

		if ((GamerResult.game_result-1) < 5)
			aciUpdateCurCredits(aciGetCurCredits() + ELEECH[GamerResult.game_result-1]);
		break;
	case UVS_GAME_ID::PIPETKA:
		if (V_RACE_VANGER >= GamerResult.game_result)
			GamerResult.game_pip_result += GAME_RAITING[ GamerResult.game_result-1];

		if ((GamerResult.game_result-1) < 3)
			aciUpdateCurCredits(aciGetCurCredits() + PIPETKA[GamerResult.game_result-1]);
		break;
	case UVS_GAME_ID::KERNOBOO:
		if (V_RACE_VANGER >= GamerResult.game_result)
			GamerResult.game_ker_result += GAME_RAITING[ GamerResult.game_result-1];

		if ((GamerResult.game_result-1) < 3)
			aciUpdateCurCredits(aciGetCurCredits() + KERNOBOO[GamerResult.game_result-1]);
		break;
	case UVS_GAME_ID::WEEZYK:
		if (V_RACE_VANGER >= GamerResult.game_result)
			GamerResult.game_zyk_result += GAME_RAITING[ GamerResult.game_result-1];

		if ((GamerResult.game_result-1) < 1)
			aciUpdateCurCredits(aciGetCurCredits() + WEEZYK[GamerResult.game_result-1]);
		break;
	}//  end switch
}

void uvsSetTownName(const char* _name_ ){
	uvsTownName = _name_;
}

int uvsGenerateItemForCrypt( int type ){
	int itype, icount, _type_;

	switch (type){
	case UVS_CRYPT::DEVICE:
		itype = UVS_ITEM_TYPE::AMPUTATOR;
		icount = UVS_ITEM_TYPE::MECHOSCOPE + 1 - itype;
//		icount = 0;
		return (itype  + RND(icount));
		break;
	case UVS_CRYPT::ARMO_LIGHT:
		itype = UVS_ITEM_TYPE::MACHOTINE_GUN_LIGHT;
		icount = UVS_ITEM_TYPE::TERMINATOR2 + 1 - itype;

		_type_ = RND(icount);
		while( !uvsItemTable[itype + _type_]->size
#ifdef _DEMO_
			|| !uvsItemTable[itype + _type_]->count
#endif
			)
			_type_ = RND(icount);

		return(itype  + _type_);
		break;
	case UVS_CRYPT::ARMO_HEAVY:
		itype = UVS_ITEM_TYPE::MACHOTINE_GUN_LIGHT;
		icount = UVS_ITEM_TYPE::TERMINATOR2 +1 - itype;

		_type_ = RND(icount);
		while( uvsItemTable[itype + _type_]->size
#ifdef _DEMO_
			|| !uvsItemTable[itype + _type_]->count
#endif
			)
			_type_ = RND(icount);

		return(itype  + _type_);
		//pi -> param2 = 8;
		break;
	case UVS_CRYPT::AMMO:
		itype = UVS_ITEM_TYPE::SPEETLE_SYSTEM_AMMO0;
		icount = UVS_ITEM_TYPE::GLUEK +1 - UVS_ITEM_TYPE::SPEETLE_SYSTEM_AMMO0;

		_type_ = RND(icount);
#ifdef _DEMO_
		if ( type + _type_ == UVS_ITEM_TYPE::CRUSTEST_CANNON_AMMO)
			_type_++;
#endif

		return(itype  + _type_);
		break;
	}
	return -1;
}

void uvsDomChangeFromItem( int type, int what, int where){
	if ( where){
		if (what){
			if (type == UVS_ITEM_TYPE::LEEPURINGA){
				GamerResult.add_luck += 50;
				ShowLuckMessage(50);
			}
			if (type == UVS_ITEM_TYPE::BOORAWCHICK){
				GamerResult.add_dominance += 50;
				ShowDominanceMessage(50);
			}
			if (type == UVS_ITEM_TYPE::PROTRACTOR){
				GamerResult.add_luck += 20;
				ShowLuckMessage(20);
			}
			if (type == UVS_ITEM_TYPE::MECHANIC_MESSIAH){
				GamerResult.add_dominance += 20;
				ShowDominanceMessage(20);
			}
		} else {
			if (type == UVS_ITEM_TYPE::LEEPURINGA){
				GamerResult.add_luck -= 50;
				ShowLuckMessage(-50);
			}
			if (type == UVS_ITEM_TYPE::BOORAWCHICK){
				GamerResult.add_dominance -= 50;
				ShowDominanceMessage(-50);
			}
			if (type == UVS_ITEM_TYPE::PROTRACTOR){
				GamerResult.add_luck -= 20;
				ShowLuckMessage(-20);
			}
			if (type == UVS_ITEM_TYPE::MECHANIC_MESSIAH){
				GamerResult.add_dominance -= 20;
				ShowDominanceMessage(-20);
			}
		}

/*		if (GamerResult.dominance > 100) GamerResult.dominance = 100;
		if (GamerResult.luck > 100) GamerResult.luck = 100;

		if (GamerResult.dominance < -100) GamerResult.dominance = -100;
		if (GamerResult.luck < 0 ) GamerResult.luck = 0;*/
	}

	if (what){
		if ( !uvsItemTable[type] -> gamer_use ){
			uvsItemTable[type] -> gamer_use = 1;

			switch( uvsItemTable[type] -> type ){
			case UVS_ITEM_STATUS::ARTIFACT:
				GamerResult.dominance += 15;
				ShowDominanceMessage(15);
				if (GamerResult.dominance > 100) GamerResult.dominance = 100;
				break;
			case UVS_ITEM_STATUS::WEAPON:
				GamerResult.dominance += 3;
				ShowDominanceMessage(3);
				if (GamerResult.dominance > 100) GamerResult.dominance = 100;
				break;
			case UVS_ITEM_STATUS::DEVICE:
				GamerResult.dominance += 3;
				ShowDominanceMessage(3);
				if (GamerResult.dominance > 100) GamerResult.dominance = 100;
				break;
			case UVS_ITEM_STATUS::MECHOS_PART:
				GamerResult.dominance += 5;
				ShowDominanceMessage(5);
				if (GamerResult.dominance > 100) GamerResult.dominance = 100;
				break;
			}//  end switch
		}//  end if
	}
}

void uvsChangeGoodsInList(int world ){
	uvsActInt *pa = GGamer;
	if (GGamer){
		while( pa ){
			int type = ActInt_to_Item( pa -> type);

			if (uvsChangeGoodsParam(type, pa -> param1, pa -> param2, world)){
//				uvs_aciChangeOneItem(pa -> type, uvsSetItemType(type, pa -> param1, pa -> param2), pa);
				uvs_aciChangeOneItem(pa -> type, uvsSetItemType(type, pa -> param1, pa -> param2), pa -> pos_x, pa -> pos_y);
				if (uvsSetItemType(type, pa -> param1, pa -> param2) == TABUTASK_GOOD){
					int param2 = (pa -> param2 & 0x0000FFFF);

					SOUND_SUCCESS();
					pa -> type = TABUTASK_GOOD;

					if (Gamer -> Pescave){
						if ( (Gamer -> Pescave -> TabuTaskID <= param2) && (Gamer -> Pescave -> TabuTaskCount +Gamer -> Pescave -> TabuTaskID > param2))
							pa -> sell_price = TabuTable[param2] -> cash;
					} else {
						if ( (Gamer -> Pspot -> TabuTaskID <= param2) && (Gamer -> Pspot -> TabuTaskCount +Gamer -> Pspot -> TabuTaskID > param2))
							pa -> sell_price = TabuTable[param2] -> cash;
					}
				} else {
					SOUND_FAILED();
					pa -> type = TABUTASK_BAD;
				}
			}

			pa = (uvsActInt*)(pa -> next);
		}//  end while
	}//  end if

	if (uvsKronActive)
		ChangeItemData(world);
	uvsWorld *pw = (uvsWorld*)WorldTail;

	while( pw ){
		pw -> ChangeGoodsInList(world);
		pw = (uvsWorld*)pw -> next;
	}
}

int uvsTagetID( char* s ){
	int i;
	for(i = 0; i < UVS_TARGET::NONE; i++) if (!strcmp(s, PrmTargetName[i])) return i;

	ErrH.Abort("uvsTabuTask:: Bad Target");
	return -1;
}

int uvsTagetWorkID( char* s ){
	int i;
	for(i = 0; i <= UVS_TABUTASK_WORK::NONE; i++) if (!strcmp(s, PrmTargetWorkName[i])) return i;

	ErrH.Abort("uvsTabuTask:: Bad Work");
	return -1;
}

uvsTabuTaskType::~uvsTabuTaskType(void){
	delete[] item;
	delete[] item_count;
	delete[] real_count;
}

uvsTabuTaskType::uvsTabuTaskType(PrmFile* pfile,char* atom){
	uvsEscave *pe = NULL;
	uvsSpot *ps = NULL;
	int i;

	pe = (uvsEscave*)EscaveTail -> seekName( atom );
	ps = (uvsSpot*)SpotTail -> seekName( atom );

	if (ps || pe){
		if (_pe_) _pe_ -> TabuTaskCount = MAX_TABUTASK - _pe_ -> TabuTaskID;
		if (_ps_) _ps_ -> TabuTaskCount = MAX_TABUTASK - _ps_ -> TabuTaskID;

		if (pe) {
			pe -> TabuTaskID = MAX_TABUTASK;
			_pe_ = pe;
			_ps_ = NULL;
		}

		if (ps) {
			ps -> TabuTaskID = MAX_TABUTASK;
			_ps_ = ps;
			_pe_ = NULL;
		}
		atom = pfile -> getAtom();
	}

	MAX_TABUTASK++;

	cash = atoi(atom);					//  стоимость
	luck = atoi(pfile -> getAtom());			//  параметр удачи
	cycle  = atoi(pfile -> getAtom());		//  время в циклах

	target = uvsTagetID(pfile -> getAtom());	//  тип-обьект задания

	if (target == UVS_TARGET::ESCAVE || target == UVS_TARGET::SPOT){
		atom = pfile -> getAtom();
	}

	work_on_target = uvsTagetWorkID(pfile -> getAtom());	//  цель для обьукта
	item_number = atoi(pfile -> getAtom()); //  количество предметов

	item = new int[item_number];
	item_count = new int[item_number];
	real_count = new int[item_number];

	for( i = 0; i < item_number; i++){
		if ( target != UVS_TARGET::VANGERS )
			item[i] = GetItem_as_name(pfile -> getAtom());	//  предмет дя задания
		else
			item[i] = atoi(pfile -> getAtom());
		item_count[i] = atoi(pfile -> getAtom());			//  количество для задания
		real_count[i] = 0;
	}

	status = 0;

	if ( target != UVS_TARGET::RACE)
		*real_count  = 0;
	else {
		*real_count = atoi(pfile -> getAtom());
		param = GetMechos_as_name(pfile -> getAtom());
	}

	Ptarget = NULL;

	if ( target == UVS_TARGET::ESCAVE ){
		uvsEscave *pe = (uvsEscave*)EscaveTail -> seekName(atom);

		if (pe)
			Ptarget = (uvsTarget*)pe;
		else
			ErrH.Abort("uvsTabuTaskType :: Bad Escave Name");
	} else if ( target == UVS_TARGET::SPOT){
		uvsSpot *ps = (uvsSpot*)SpotTail -> seekName(atom);

		if (ps)
			Ptarget = (uvsTarget*)ps;
		else
			ErrH.Abort("uvsTabuTaskType :: Bad Spot Name");
	}

	Pworld = (uvsWorld*)WorldTail -> seekName(pfile -> getAtom());

	if (!Pworld)
		ErrH.Abort("uvsTabuTaskType :: Bad World Name");
	priority = atoi(pfile -> getAtom());
}

int uvsTabuTaskType::is_able(void){
	int i;
	int world = Gamer -> Pworld -> gIndex;
	if ((work_on_target == UVS_TABUTASK_WORK::DELIVER_OR) && (uvsMechosTable[20] -> constractor == 3) )
		return 0;

	if ((target == UVS_TARGET::RACE) ){
		for( i = 0; i < item_number; i++ )
			if (!uvsCheckItem(item[i]))
				return 0;
	}

	if ((work_on_target == UVS_TABUTASK_WORK::DELIVER_OR || 
	     work_on_target == UVS_TABUTASK_WORK::DELIVER_ONE || 
	     work_on_target == UVS_TABUTASK_WORK::DELIVER) ){
		for( i = 0; i < item_number; i++ )
			if (!uvsCheckItem(item[i]))
				return 0;
	}

	switch( priority ){
	case 1:
		break;
	case 2:
		if (world == 0 && aciWorldLinkExist(0,1,0) == 0) return 0;
		if (world == 1 && dgD -> bStatus < 1) return 0;
		if (world == 2 && GamerResult.game_zyk_result == 0) return 0;
		break;
	case 3:
		if (world == 0 && uvsSecretON() == 0) return 0;
		if (world == 1 && dgD -> bStatus < 2) return 0;
		if (world == 2 && (uvsSecretON() == 0 || GamerResult.game_zyk_result == 0)) return 0;
		break;
	case 4:
		if (world == 0) return 0;
		if (world == 1 && (dgD -> bStatus < 2 || uvsSecretON() == 0)) return 0;
		if (world == 2) return 0;
		break;
	}//  end switch

	if (Pworld)
		return Pworld -> GamerVisit;
	else
		return 1;
}

char* uvsGetLarvaWorld(int n){
	uvsWorld *Pworld = Gamer->Pworld;

	if (Pworld -> escTmax){
		uvsBunch *pb = Pworld -> escT[0] -> Pbunch;

		return pb -> cycleTable[n].Pdolly -> Pworld -> name;
	} else
		return NULL;
}

uvsPassage* GetPassage(int fromWID, int toWID){
	uvsWorld *nextW;
	uvsWorld *fromW;

	if ( fromWID == toWID ) return NULL;

	if ( fromWID >= MAIN_WORLD_MAX ) {
		return WorldTable[fromWID] -> pssT[0];
	}

	if (toWID >= MAIN_WORLD_MAX ) {
		int lastID = toWID;
		toWID = WorldTable[toWID] -> pssT[0] -> Poutput -> gIndex;

		if (fromWID == toWID) return WorldTable[fromWID] -> getPassage( getWorld(lastID) );;
	}

	unsigned int linkage = ChainMap[toWID + fromWID*MAIN_WORLD_MAX],mI;


	mI = linkage & (0x0000000F);
	nextW = getWorld(mI);

	if(!nextW) nextW = getWorld(toWID);
	fromW = getWorld(fromWID);

	return	fromW -> getPassage(nextW);
}

uvsVanger* FindFreeVanger(void){
	uvsElement* p = ETail;
	uvsVanger* v;
	while(p){
		if(p -> type == UVS_OBJECT::VANGER && ((uvsVanger*)p) -> shape != UVS_VANGER_SHAPE::GAMER){
			v = (uvsVanger*)p;
			if( v -> status == UVS_VANGER_STATUS::SPOT_SLEEPING || v -> status == UVS_VANGER_STATUS::ESCAVE_SLEEPING){
				if (Gamer -> Pworld -> escTmax)
					v -> Pworld = Gamer -> Pworld;
				else
					v -> Pworld = WorldTable[RND(3)];

				v -> Pescave = Gamer -> Pescave;
				v -> Pspot = Gamer -> Pspot;
				v -> shape = UVS_VANGER_SHAPE::TABUTASK;

				if (v -> Pescave ){
					v -> pos_x = v -> Pescave -> pos_x + 1024;
					v -> pos_y = v -> Pescave -> pos_y + (1-RND(2))*1000;
				} else {
					v -> pos_x = v -> Pspot -> pos_x + 1024;
					v -> pos_y = v -> Pspot -> pos_y + (1-RND(2))*1000;
				}
				v -> Pworld -> escT[0] -> Pshop -> sellMechos(v -> Pmechos);

				if (!v -> Pmechos)
					return NULL;
				return v;
			}

		}
		p = p -> enext;
	}
	return NULL;
}

void uvsCheckVangerTabuTask(uvsVanger* pv, int how){
	int i;

	for( i = 0; i < MAX_TABUTASK; i++){
		if (TabuTable[i] -> status == UVS_TABUTASK_STATUS::ACTIVE){
			switch(TabuTable[i] -> target){
			case UVS_TARGET::VANGER:
				if ( TabuTable[i] -> Ptarget == ((uvsTarget*)pv) ){

					switch(TabuTable[i] -> work_on_target){
					case UVS_TABUTASK_WORK::GO_OUT:
						if ( how == uvsVANGER_ARRIVAL ){
							uvsChangeTabuTask(i, UVS_TABUTASK_STATUS::BAD);
							TabuTable[i]  -> status = UVS_TABUTASK_STATUS::SLEEP;
						} else {
							uvsChangeTabuTask(i, UVS_TABUTASK_STATUS::GOOD);
							uvsChangeTownTabuTask(i);
							TabuTable[i]  -> status = UVS_TABUTASK_STATUS::OK;
						}
						pv -> shape = UVS_VANGER_SHAPE::BUNCH_SLAVE;
//						TabuTable[i]  -> status = UVS_TABUTASK_STATUS::SLEEP;
						break;
					case UVS_TABUTASK_WORK::KILL:
						if ( how == uvsVANGER_FAILED || how == uvsVANGER_KILLED) {
							uvsChangeTabuTask(i, UVS_TABUTASK_STATUS::GOOD);
							uvsChangeTownTabuTask(i);
							pv -> shape = UVS_VANGER_SHAPE::BUNCH_SLAVE;
							TabuTable[i]  -> status = UVS_TABUTASK_STATUS::OK;
						}
						break;
					case UVS_TABUTASK_WORK::DEFENSE:
						if ( how == uvsVANGER_ARRIVAL ){
							uvsChangeTabuTask(i, UVS_TABUTASK_STATUS::BAD);
							TabuTable[i]  -> status = UVS_TABUTASK_STATUS::SLEEP;
						} else {
							uvsChangeTabuTask(i, UVS_TABUTASK_STATUS::GOOD);
							TabuTable[i]  -> status = UVS_TABUTASK_STATUS::OK;
							uvsChangeTownTabuTask(i);
						}
						pv -> shape = UVS_VANGER_SHAPE::BUNCH_SLAVE;
//						TabuTable[i]  -> status = UVS_TABUTASK_STATUS::SLEEP;
						break;
					}//  end switch
				}//  end if
				break;
			case UVS_TARGET::VANGERS:
				if ( how == uvsVANGER_KILLED){
					switch(TabuTable[i] -> work_on_target){
					case UVS_TABUTASK_WORK::KILL:
						int NEEDFlag = *TabuTable[i] -> item;
						int Mask = 0;

						if (pv -> status == UVS_VANGER_STATUS::RACE)
							Mask |= UVS_KRON_FLAG::ELEREC;
						else if (pv -> status == UVS_VANGER_STATUS::RACE_HUNTER)
							Mask |= UVS_KRON_FLAG::ZEEXL;
						else if (CurrentWorld == 1 && uvsCurrentCycle == 1)
							Mask |= UVS_KRON_FLAG::PIP;
						else if (CurrentWorld == 1 && uvsCurrentCycle == 2)
							Mask |= UVS_KRON_FLAG::KERN;

						switch(pv -> Pmechos -> color){
						case 0: Mask |= UVS_KRON_FLAG::ELIPOD;
							break;
						case 1: Mask |= UVS_KRON_FLAG::BEEBO;
							break;
						case 2: Mask |= UVS_KRON_FLAG::ZEEX;
							break;
						}//  end switch

						int KronFlag = Mask & NEEDFlag;

						if ( ( (NEEDFlag & 0x000000F0) == 0 || (KronFlag & 0x000000F0)) && ( (NEEDFlag & 0x00000F00) == 0 || (KronFlag & 0x00000F00)) ){
							(*TabuTable[i]	-> real_count)++;

							if ((*TabuTable[i]  -> real_count) >= (*TabuTable[i]  -> item_count)){
								uvsChangeTabuTask(i, UVS_TABUTASK_STATUS::GOOD);
								uvsChangeTownTabuTask(i);
								TabuTable[i] -> status = UVS_TABUTASK_STATUS::OK;
								*TabuTable[i] -> real_count = 0;
							}//  end if
						}
;
						break;
					}//  ens switch
				}//  end if
				break;
			case UVS_TARGET::RAFFA:
				if ( uvsMechosTable[ pv -> Pmechos -> type ] -> type == UVS_CAR_TYPE::RAFFA && how == uvsVANGER_KILLED ){

					(*TabuTable[i]	-> real_count)++;

					if ((*TabuTable[i]  -> real_count) >= (*TabuTable[i]  -> item_count)){
						uvsChangeTabuTask(i, UVS_TABUTASK_STATUS::GOOD);
						uvsChangeTownTabuTask(i);
						TabuTable[i] -> status = UVS_TABUTASK_STATUS::OK;
						*TabuTable[i] -> real_count = 0;
					}//  end if

				}//  end if
				break;
			}//  end switch
		}//  end if
	}//  end for i
}

void uvsCheckKronIventTabuTask(int KronType, int KronCount,  int MYresult, int MYitem ){
	int i;
#ifdef TABU_REPORT
	tabu < ConTimer.GetTime();
	tabu < " Kron Type " <= KronType < "KronCount " <= KronCount < "MYresult " <= MYresult < "MYitem " <= MYitem < "\n";
#endif
	for( i = 0; i < MAX_TABUTASK; i++){
		if ( TabuTable[i] -> status == UVS_TABUTASK_STATUS::ACTIVE){
			switch(TabuTable[i] -> target){
			case UVS_TARGET::DOLLY:
				if (KronType == UVS_KRON_EVENT::END_CYCLE){
					if((TabuTable[i] -> work_on_target == UVS_TABUTASK_WORK::CHANGE_CYCLE)
					&&( i >= Gamer -> Pescave -> TabuTaskID && i < Gamer -> Pescave -> TabuTaskCount + Gamer -> Pescave -> TabuTaskID)){
//						if ((*TabuTable[i] -> real_count) == KronCount){
							uvsChangeTabuTask(i, UVS_TABUTASK_STATUS::GOOD);
							uvsChangeTownTabuTask(i);
							TabuTable[i]  -> status = UVS_TABUTASK_STATUS::OK;
							*TabuTable[i]  -> real_count = 0;
//						}
					}//  end if
				} else if (KronType == UVS_KRON_EVENT::KILL_DOLLY){
					if(TabuTable[i] -> work_on_target == UVS_TABUTASK_WORK::KILL){
						if ((*TabuTable[i] -> item_count) == KronCount){
							uvsChangeTabuTask(i, UVS_TABUTASK_STATUS::GOOD);
							uvsChangeTownTabuTask(i);
							TabuTable[i]  -> status = UVS_TABUTASK_STATUS::OK;
							*TabuTable[i]  -> real_count = 0;
						}//  if
					}//// if
				}//  end fi
				break;
			case UVS_TARGET::BEEB:
				switch(KronType){
					case UVS_KRON_EVENT::BEEB:
						if (TabuTable[i] -> work_on_target == UVS_TABUTASK_WORK::KILL_BEEB){
							(*TabuTable[i] -> real_count) += KronCount;

							if ((*TabuTable[i] -> real_count) >= (*TabuTable[i] -> item_count)){
								uvsChangeTabuTask(i, UVS_TABUTASK_STATUS::GOOD);
								uvsChangeTownTabuTask(i);
								TabuTable[i]  -> status = UVS_TABUTASK_STATUS::OK;
								*TabuTable[i]  -> real_count = 0;
							}
						}
						break;
					case UVS_KRON_EVENT::GOLD_BEEB:
						if (TabuTable[i] -> work_on_target == UVS_TABUTASK_WORK::KILL_BEEB ||
						    TabuTable[i] -> work_on_target == UVS_TABUTASK_WORK::KILL_GOLD_BEEB){
							(*TabuTable[i] -> real_count) += KronCount;

							if ((*TabuTable[i] -> real_count) >= (*TabuTable[i] -> item_count)){
								uvsChangeTabuTask(i, UVS_TABUTASK_STATUS::GOOD);
								uvsChangeTownTabuTask(i);
								TabuTable[i]  -> status = UVS_TABUTASK_STATUS::OK;
								*TabuTable[i]  -> real_count = 0;
							}
						}
						break;
				}//  end switch
				break;
			case UVS_TARGET::GRIB:
				if (KronType == UVS_KRON_EVENT::GRIB){
					(*TabuTable[i] -> real_count) += KronCount;

					if ((*TabuTable[i] -> real_count) >= (*TabuTable[i] -> item_count)){
						uvsChangeTabuTask(i, UVS_TABUTASK_STATUS::GOOD);
						uvsChangeTownTabuTask(i);
						TabuTable[i]  -> status = UVS_TABUTASK_STATUS::OK;
						*TabuTable[i]  -> real_count = 0;
					}
				}//  end if
				break;
			case UVS_TARGET::HIVE:
				if (KronType == UVS_KRON_EVENT::HIVE){
					(*TabuTable[i] -> real_count) += KronCount;

					if ((*TabuTable[i] -> real_count) >= (*TabuTable[i] -> item_count)){
						uvsChangeTabuTask(i, UVS_TABUTASK_STATUS::GOOD);
						uvsChangeTownTabuTask(i);
						TabuTable[i]  -> status = UVS_TABUTASK_STATUS::OK;
						*TabuTable[i]  -> real_count = 0;
					}
				}//  end if
				break;
			case UVS_TARGET::CRYPT:
				if (KronType == UVS_KRON_EVENT::CRYPT){
					(*TabuTable[i] -> real_count) += KronCount;

					if ((*TabuTable[i] -> real_count) >= (*TabuTable[i] -> item_count)){
						uvsChangeTabuTask(i, UVS_TABUTASK_STATUS::GOOD);
						uvsChangeTownTabuTask(i);
						TabuTable[i]  -> status = UVS_TABUTASK_STATUS::OK;
						*TabuTable[i]  -> real_count = 0;
					}
				}//  end if
				break;
			case UVS_TARGET::WHIRLPOOL:
				if (KronType == UVS_KRON_EVENT::WHIRLPOOL){
					(*TabuTable[i] -> real_count) += KronCount;

					if ((*TabuTable[i] -> real_count) >= (*TabuTable[i] -> item_count)){
						uvsChangeTabuTask(i, UVS_TABUTASK_STATUS::GOOD);
						uvsChangeTownTabuTask(i);
						TabuTable[i]  -> status = UVS_TABUTASK_STATUS::OK;
						*TabuTable[i]  -> real_count = 0;
					}
				}//  end if
				break;
			case UVS_TARGET::SWAMP:
				if (KronType == UVS_KRON_EVENT::SWAMP){
					(*TabuTable[i] -> real_count) += KronCount;

					if ((*TabuTable[i] -> real_count) >= (*TabuTable[i] -> item_count)){
						uvsChangeTabuTask(i, UVS_TABUTASK_STATUS::GOOD);
						uvsChangeTownTabuTask(i);
						TabuTable[i]  -> status = UVS_TABUTASK_STATUS::OK;
						*TabuTable[i]  -> real_count = 0;
					}
				}//  end if
				break;
			case UVS_TARGET::SHEILD:
				if (KronType == UVS_KRON_EVENT::RECHARGE_SHEILD){
					(*TabuTable[i] -> real_count) += KronCount;

					if ((*TabuTable[i] -> real_count) >= (*TabuTable[i] -> item_count)){
						uvsChangeTabuTask(i, UVS_TABUTASK_STATUS::GOOD);
						uvsChangeTownTabuTask(i);
						TabuTable[i]  -> status = UVS_TABUTASK_STATUS::OK;
						*TabuTable[i]  -> real_count = 0;
					}
				}//  end if
				break;
			case UVS_TARGET::PASSAGE:
				if (KronType == UVS_KRON_EVENT::ARK_NOY){
					uvsChangeTabuTask(i, UVS_TABUTASK_STATUS::GOOD);
					uvsChangeTownTabuTask(i);
					TabuTable[i]  -> status = UVS_TABUTASK_STATUS::OK;
					*TabuTable[i]  -> real_count = 0;
				}//  end if
				break;
			case UVS_TARGET::ESCAVE:
			case UVS_TARGET::SPOT:
				switch(KronType){
					case UVS_KRON_EVENT::DAMAGE:
						if (TabuTable[i] -> work_on_target == UVS_TABUTASK_WORK::GO_WITHOUT_DAMAGE){
							uvsChangeTabuTask(i, UVS_TABUTASK_STATUS::BAD);
							TabuTable[i]  -> status = UVS_TABUTASK_STATUS::SLEEP;
							*TabuTable[i]  -> real_count = 0;
						}
						break;
					case UVS_KRON_EVENT::FLY_ONLINE:
						if (TabuTable[i] -> work_on_target == UVS_TABUTASK_WORK::GO_FLY){
							uvsChangeTabuTask(i, UVS_TABUTASK_STATUS::GOOD);
							TabuTable[i]  -> status = UVS_TABUTASK_STATUS::OK;
							uvsChangeTownTabuTask(i);
							*TabuTable[i]  -> real_count = 0;
						}
						break;
					case UVS_KRON_EVENT::GRIB:
						if (TabuTable[i] -> work_on_target == UVS_TABUTASK_WORK::GO_WITHOUT_GRIB){
							uvsChangeTabuTask(i, UVS_TABUTASK_STATUS::BAD);
							TabuTable[i]  -> status = UVS_TABUTASK_STATUS::SLEEP;
							*TabuTable[i]  -> real_count = 0;
						}
						break;
					case UVS_KRON_EVENT::BAD_CIRT:
						if (TabuTable[i] -> work_on_target == UVS_TABUTASK_WORK::DELIVER_BAD_CIRT){
							uvsTarget* pt;

							if (Gamer -> Pescave)
								pt = (uvsTarget*)Gamer -> Pescave;
							else
								pt = (uvsTarget*)Gamer -> Pspot;

							if (pt == TabuTable[i]	-> Ptarget){
								uvsChangeTabuTask(i, UVS_TABUTASK_STATUS::GOOD);
								uvsChangeTownTabuTask(i);
								TabuTable[i]  -> status = UVS_TABUTASK_STATUS::OK;
								*TabuTable[i]  -> real_count = 0;
							}//  end if
						}//  end if
						break;
				}//  end switch
				break;
			case UVS_TARGET::VANGERS:
				if (KronType == UVS_KRON_EVENT::SHOT){
					if (TabuTable[i] -> work_on_target == UVS_TABUTASK_WORK::SHOT_ANY){
						int NEEDFlag = *TabuTable[i] -> item;
						int KronFlag = KronCount & NEEDFlag;

						if ( ((KronFlag & 0x0000000F) == (NEEDFlag & 0x0000000F)) && ( (NEEDFlag & 0x000000F0) == 0 || (KronFlag & 0x000000F0)) && ( (NEEDFlag & 0x00000F00) == 0 || (KronFlag & 0x00000F00)) ){
							(*TabuTable[i] -> real_count)++;
#ifdef TABU_REPORT
	tabu < " count " <= (*TabuTable[i] -> real_count) < " need " <= (*TabuTable[i] -> item_count)< "\n ";
#endif
							if ((*TabuTable[i] -> real_count) >= (*TabuTable[i] -> item_count)){
								uvsChangeTabuTask(i, UVS_TABUTASK_STATUS::GOOD);
								uvsChangeTownTabuTask(i);
								TabuTable[i]  -> status = UVS_TABUTASK_STATUS::OK;
								*TabuTable[i]  -> real_count = 0;
							}//  end if
						}//  end if
					}//end if
				}//  end if
				break;
			case UVS_TARGET::RACE:
				if ( (KronType == UVS_KRON_EVENT::END_RACE) && (KronCount >= *TabuTable[i] -> item_count)
				     && ((MYresult == (*TabuTable[i] -> real_count)) || ((*TabuTable[i] -> real_count) == -1)) && (MYitem == *TabuTable[i] -> item)) {

					if (TabuTable[i] -> work_on_target != UVS_TABUTASK_WORK::END_RACE_RAFFA){

						if ((TabuTable[i] -> param == -1) || (Gamer -> Pmechos -> type == TabuTable[i] -> param)){
							uvsChangeTabuTask(i, UVS_TABUTASK_STATUS::GOOD);
							uvsChangeTownTabuTask(i);
							TabuTable[i]  -> status = UVS_TABUTASK_STATUS::OK;
						}
					} else if (uvsMechosTable[Gamer -> Pmechos -> type] -> type == UVS_CAR_TYPE::RAFFA){
						uvsChangeTabuTask(i, UVS_TABUTASK_STATUS::GOOD);
						uvsChangeTownTabuTask(i);
						TabuTable[i]  -> status = UVS_TABUTASK_STATUS::OK;
					}
				} else if (KronType == UVS_KRON_EVENT::WEAPON_ON && (TabuTable[i] -> work_on_target == UVS_TABUTASK_WORK::END_RACE_WEAPON)){

					if (Gamer -> Pworld -> gIndex < 3){
						uvsBunch *pb = Gamer -> Pworld -> escT[0] -> Pbunch;

						if (pb -> cycleTable[ pb -> currentStage ].Pgame && pb -> cycleTable[ pb -> currentStage ].Pgame -> GoodsTypeEnd == (*TabuTable[i] -> item)){
							uvsChangeTabuTask(i, UVS_TABUTASK_STATUS::BAD);
							TabuTable[i]  -> status = UVS_TABUTASK_STATUS::SLEEP;
						}//  end if
					}//  end if
				}//  end if
			}//  end if
		}//  end if status
	}//  end for i
}

void uvsCheckItemOnTabuTask(listElem*& pe, int ID, int IDcount, int what){
	int i, j, komplit;

	for( i = 0; i < MAX_TABUTASK; i++){
		if ( TabuTable[i] -> status == UVS_TABUTASK_STATUS::ACTIVE){
			if ((TabuTable[i] -> target == UVS_TARGET::ITEM) && ( i >= ID ) && ( i < ID + IDcount)){

				switch(TabuTable[i] -> work_on_target){
				case UVS_TABUTASK_WORK::DELIVER:
				case UVS_TABUTASK_WORK::DELIVER_ONE:
				case UVS_TABUTASK_WORK::DELIVER_OR:
					if (what){
						for( j = 0; j < TabuTable[i]  -> item_number; j++){
							int l_count = TabuTable[i] -> item_count[j] - TabuTable[i]  -> real_count[j];
							TabuTable[i]  -> real_count[j] += ItemCount(pe, TabuTable[i] -> item[j]);
							uvsDeleteItem(pe, TabuTable[i] -> item[j], l_count);
//							uvs_aciKillItem(uvsItemTable[  TabuTable[i] -> item[j] ] -> SteelerTypeFull);
						}
					} else {
						for( j = 0; j < TabuTable[i]  -> item_number; j++)
							TabuTable[i]  -> real_count[j] -= ItemCount(pe, TabuTable[i] -> item[j]);
					}

					if (!what) break;

					if (TabuTable[i] -> work_on_target != UVS_TABUTASK_WORK::DELIVER_OR){
						komplit = 1;

						for( j = 0; j < TabuTable[i]  -> item_number; j++)
							if (TabuTable[i]  -> real_count[j] < TabuTable[i] -> item_count[j])
								komplit = 0;
					} else {
						komplit = 0;

						for( j = 0; j < TabuTable[i]  -> item_number; j++)
							if (TabuTable[i]  -> real_count[j] >= TabuTable[i] -> item_count[j])
								komplit = 1;
					}

					if (komplit){
						uvsChangeTabuTask(i, UVS_TABUTASK_STATUS::GOOD);
						uvsChangeTownTabuTask(i);
						TabuTable[i]  -> status = UVS_TABUTASK_STATUS::OK;
						*TabuTable[i]  -> real_count = 0;
					}
					if (TabuTable[i] -> work_on_target == UVS_TABUTASK_WORK::DELIVER_ONE)
						for( j = 0; j < TabuTable[i]  -> item_number; j++)
							TabuTable[i]  -> real_count[j] = 0;
					break;
				}//  end switch
			} else if ( (TabuTable[i] -> target == UVS_TARGET::ESCAVE) || (TabuTable[i] -> target == UVS_TARGET::SPOT) ){
				int _ID_;

				if (TabuTable[i] -> target == UVS_TARGET::ESCAVE)
					_ID_ = ((uvsEscave*)TabuTable[i] -> Ptarget) -> TabuTaskID;
				else
					_ID_ = ((uvsSpot*)TabuTable[i] -> Ptarget) -> TabuTaskID;

				if (_ID_ == ID){
					switch(TabuTable[i] -> work_on_target){
					case UVS_TABUTASK_WORK::GO_WITHOUT_DAMAGE:
					case UVS_TABUTASK_WORK::GO_WITHOUT_GRIB:
//					case UVS_TABUTASK_WORK::GO_FLY:
						if (!what) break;
						uvsChangeTabuTask(i, UVS_TABUTASK_STATUS::GOOD);
						uvsChangeTownTabuTask(i);
						TabuTable[i]  -> status = UVS_TABUTASK_STATUS::OK;
						*TabuTable[i]  -> real_count = 0;
						break;
					case UVS_TABUTASK_WORK::DELIVER:
					case UVS_TABUTASK_WORK::DELIVER_ONE:
					case UVS_TABUTASK_WORK::DELIVER_OR:
					if (what){
						for( j = 0; j < TabuTable[i]  -> item_number; j++){
							int l_count = TabuTable[i] -> item_count[j] - TabuTable[i]  -> real_count[j];
							TabuTable[i]  -> real_count[j] += ItemCount(pe, TabuTable[i] -> item[j]);
							uvsDeleteItem(pe, TabuTable[i] -> item[j], l_count);
//							uvs_aciKillItem(uvsItemTable[  TabuTable[i] -> item[j] ] -> SteelerTypeFull);
						}
					} else {
						for( j = 0; j < TabuTable[i]  -> item_number; j++)
							TabuTable[i]  -> real_count[j] -= ItemCount(pe, TabuTable[i] -> item[j]);
					}

					if (!what) break;

					if (TabuTable[i] -> work_on_target != UVS_TABUTASK_WORK::DELIVER_OR){
						komplit = 1;

						for( j = 0; j < TabuTable[i]  -> item_number; j++)
							if (TabuTable[i]  -> real_count[j] < TabuTable[i] -> item_count[j])
								komplit = 0;
					} else {
						komplit = 0;

						for( j = 0; j < TabuTable[i]  -> item_number; j++)
							if (TabuTable[i]  -> real_count[j] >= TabuTable[i] -> item_count[j])
								komplit = 1;
					}

					if (komplit){
						uvsChangeTabuTask(i, UVS_TABUTASK_STATUS::GOOD);
						uvsChangeTownTabuTask(i);
						TabuTable[i]  -> status = UVS_TABUTASK_STATUS::OK;
						*TabuTable[i]  -> real_count = 0;
					}
					if (TabuTable[i] -> work_on_target == UVS_TABUTASK_WORK::DELIVER_ONE)
						for( j = 0; j < TabuTable[i]  -> item_number; j++)
							TabuTable[i]  -> real_count[j] = 0;
					break;
					}//  end switch
				}// end if _ID_
			}//  end if taget
		}//  end if status
	}//  end for i
}

void uvsChangeTabuTask(int type, int status){
	uvsActInt *pa = GGamer;

	if (status == UVS_TABUTASK_STATUS::BAD){
		ShowTaskMessage(-TabuTable[ type ] -> luck);
	} else {
		ShowTaskMessage(TabuTable[ type] -> luck);
	}
	
	if (GGamer){
		while( pa ){
			int _type_ = ActInt_to_Item( pa -> type);

			if (_type_ == UVS_ITEM_TYPE::TABUTASK && (pa -> param2 == type)){
				pa -> param2 |= (status <<16);

				if (status == UVS_TABUTASK_STATUS::BAD){
					GamerResult.luck -= TabuTable[ pa -> param2 & 0x0000FFFF] -> luck;
					if (GamerResult.luck < 0) GamerResult.luck = 0;
				} else {
					GamerResult.luck += TabuTable[ pa -> param2 & 0x0000FFFF] -> luck;
					if (GamerResult.luck > 100) GamerResult.luck = 100;
				}

				if (status == UVS_TABUTASK_STATUS::GOOD){
					SOUND_SUCCESS();
//					uvs_aciChangeOneItem(pa -> type, TABUTASK_GOOD, pa );
					uvs_aciChangeOneItem(pa -> type, TABUTASK_GOOD, pa -> pos_x, pa -> pos_y);

					pa -> type = TABUTASK_GOOD;

					int param2 = pa -> param2 & 0x0000FFFF;

					if (Gamer -> Pescave){
						if ( (Gamer -> Pescave -> TabuTaskID <= param2) && (Gamer -> Pescave -> TabuTaskCount +Gamer -> Pescave -> TabuTaskID > param2))
							pa -> sell_price = TabuTable[param2] -> cash;
					} else {
						if ( (Gamer -> Pspot -> TabuTaskID <= param2) && (Gamer -> Pspot -> TabuTaskCount +Gamer -> Pspot -> TabuTaskID > param2))
							pa -> sell_price = TabuTable[param2] -> cash;
					}
				} else {
					SOUND_FAILED()
					uvs_aciChangeOneItem(pa -> type, TABUTASK_BAD, pa -> pos_x, pa -> pos_y);
//					uvs_aciChangeOneItem(pa -> type, TABUTASK_BAD, pa);

					pa -> type = TABUTASK_BAD;
				}
			}

			pa = (uvsActInt*)(pa -> next);
		}//  end while
	}//  end if

	if (uvsKronActive)
		ChangeTabutaskItem(type, status);

	uvsWorld *pw = (uvsWorld*)WorldTail;

	while( pw ){
		pw -> ChangeTabuTaskInList(type, status);
		pw = (uvsWorld*)pw -> next;
	}
}

char* uvsGetNameByID(int type, int& ID){
	uvsEscave *pe = (uvsEscave*)EscaveTail;
	uvsSpot *ps = (uvsSpot*)SpotTail;

	while(pe){
		ID = pe -> SeekID( type );
		if (ID != -1)
			return pe -> name;
		pe = (uvsEscave*)pe -> next;
	}

	while(ps){
		ID = ps -> SeekID( type );
		if (ID != -1)
			return ps -> name;
		ps = (uvsSpot*)ps -> next;
	}

	return NULL;
}

void uvsActIntSell( uvsActInt *pa){
	uvsEscave *pe;
	uvsSpot *ps;
	char* NameTo;
	char *NameFrom;
	int ID;

	switch(pa -> type){
	case ACI_TABUTASK:
//	case TABUTASK_BAD:
//		GamerResult.luck -= TabuTable[pa -> param2 & 0x0000FFFF] -> luck;
		TabuTable[pa -> param2 & 0x0000FFFF] -> Erase(pa -> param1, pa -> param2);

		if (GamerResult.luck < 0) GamerResult.luck = 0;
		break;
	case TABUTASK_GOOD:
//		GamerResult.luck += TabuTable[pa -> param2 & 0x0000FFFF] -> luck;
//		if (GamerResult.luck > 100) GamerResult.luck = 100;
		break;
	case ACI_ELEEPOD:
	case ACI_BEEBOORAT:
	case ACI_ZEEX:
		pe = uvsGetEscaveByID(pa -> param2 & 0x0000FFFF);
		ps = uvsGetSpotByID(pa -> param2 & 0x0000FFFF);

		NameFrom = uvsGetNameByID(pa -> param2 & 0x0000FFFF, ID);
		NameTo = uvsGetNameByID(pa -> param2 >>16, ID);

		if (strcmp(NameTo, aci_curLocationName) && !RND(2) && strcmp(NameFrom, aci_curLocationName)){
			if (ps){
				ps -> Pworld -> escT[0] -> Pbunch -> status |= (int)UVS_BUNCH_STATUS::KILL_GAMER;
			}else if (pe){
				pe -> Pbunch -> status |= (int)UVS_BUNCH_STATUS::KILL_GAMER;
			}
		}//  end if

		break;
	}
}

void uvsActIntSellMechos( uvsActInt *pn){
	 int first_constr = MAX_MECHOS_TYPE - MAX_MECHOS_CONSTRACTOR;
	 uvsBunch* pm  = Gamer -> Pworld -> escT[0] -> Pbunch;

	int cq = pm -> cycleTable[ pm -> currentStage ].cirtQ;
	int mq = pm -> cycleTable[ pm -> currentStage ].cirtMAX;


	if ( ((uvsActInt*)pn) -> type >= first_constr && ((uvsActInt*)pn) -> price == 1){
		 ((uvsActInt*)pn) -> price = uvsMechosTable[( ((uvsActInt*)pn) -> type)] -> price;

		 ((uvsActInt*)pn) -> price = (((uvsActInt*)pn) -> price*1.5*cq + ((uvsActInt*)pn) -> price*(mq-cq))/mq;
		 ((uvsActInt*)pn) -> sell_price = (((uvsActInt*)pn) -> sell_price*cq + ((uvsActInt*)pn) -> sell_price*0.5*(mq-cq))/mq;

		 double t = ((double)(GamerResult.dominance+GamerResult.add_dominance))/100.0;

		 if ( t > 1.0 ) t = 1.0;
		 if ( t < -1.0 ) t = -1.0;

		 if ( t > 0.0 ){
			double dt = ( ((uvsActInt*)pn) -> price - ((uvsActInt*)pn) -> sell_price)/2.0;
			((uvsActInt*)pn) -> price -= t*dt;
			((uvsActInt*)pn) -> sell_price +=  t*dt;
		 } else {
			((uvsActInt*)pn) -> price *= (1.0 - t);
			((uvsActInt*)pn) -> sell_price *= (1.0 + t);
		 }

		 if (((uvsActInt*)pn) -> sell_price > ((uvsActInt*)pn) -> price) ((uvsActInt*)pn) -> sell_price = ((uvsActInt*)pn) -> price;
	}
}

void uvsKronDeleteItem(int type, int param1, int param2){
	uvsWorld *pw = Gamer -> Pworld;
	uvsItem* pi;

	if (type == UVS_ITEM_TYPE::BOORAWCHICK){
		((uvsItem*)WorldTable[1] -> escT[0] -> Pitem) ->param1 = uvsTreasureInWait;
	} else {

		switch(uvsItemTable[type] -> type){
		case UVS_ITEM_STATUS::MECHOS_PART:
//			if (ConTimer.day < 10)
//				ErrH.Abort("Its not MY");
			while ( pw == Gamer -> Pworld )
				pw = WorldTable[RND(3)];

			pi = new uvsItem(type);
			pi -> param1 = param1;
			pi -> param2 = param2;
			pi -> link(pw -> sptT[RND(pw -> sptTmax)] -> Pshop -> Pitem);
			break;
		case UVS_ITEM_STATUS::ARTIFACT:
//			if (ConTimer.day < 10)
//				ErrH.Abort("Its not MY");
		case UVS_ITEM_STATUS::TREASURE:

			while ( pw == Gamer -> Pworld )
				pw = WorldTable[RND(3)];

			if (((uvsItem*)pw -> escT[0] -> Pitem) -> type != type){
				pi = new uvsItem(type);
				pi -> param1 = param1;
				pi -> param2 = param2;
				pi -> link(pw -> escT[0] -> Pshop -> Pitem);
			} else {
				pi = new uvsItem(type);
				pi -> param1 = param1;
				pi -> param2 = param2;
				pi -> link(pw -> sptT[RND(pw -> sptTmax)] -> Pshop -> Pitem);
			}
			break;
		case UVS_ITEM_STATUS::GOODS:
			if ((type == UVS_ITEM_TYPE::TABUTASK) && ((param2 & 0xFFFF0000) == 0)){
				TabuTable[param2 & 0x0000FFFF] -> Erase( param1,  param2);
			} else if (type == UVS_ITEM_TYPE::ELEEPOD || type == UVS_ITEM_TYPE::BEEBOORAT || type == UVS_ITEM_TYPE::ZEEX ){
				uvsEscave *pe =  uvsGetEscaveByID(param2 & 0x0000FFFF);
				uvsSpot *ps =  uvsGetSpotByID(param2 & 0x0000FFFF);

				if (pe ){
					pe -> PassagerStatistic[CurrentWorld] += 1;
				}

				if (ps){
					ps -> PassagerStatistic[CurrentWorld] += 1;
				}
			}
			break;
		}//  end switch
	}//  end if
}

int uvsGameRaceNumber(void){
	uvsWorld* pw = Gamer -> Pworld;
	if (pw -> escTmax){
		uvsBunch *pb = pw -> escT[0] -> Pbunch;

		uvsCultGame *pg = pb -> cycleTable[pb -> currentStage].Pgame;

		if (pg && pg -> GameType ==  UVS_GAME_TYPE::RACE){
			return (pg -> total + 1);
		} else
			return 0;
	}else return 0;
}

int uvsGameRaceFinish(void){
	uvsWorld* pw = Gamer -> Pworld;
	if (pw -> escTmax){
		uvsBunch *pb = pw -> escT[0] -> Pbunch;

		uvsCultGame *pg = pb -> cycleTable[pb -> currentStage].Pgame;

		if (pg && pg -> GameType ==  UVS_GAME_TYPE::RACE){
			return pg -> result;
		} else
			return 0;
	}else return 0;
}

void uvsChangeTownTabuTask(int type){
	uvsEscave *pe = (uvsEscave*)EscaveTail;
	uvsSpot *ps = (uvsSpot*)SpotTail;
	int ID;

	while(pe){
		ID = pe -> SeekID( type );
		if (ID != -1){
			pe -> TabuTaskGood++;
			return;
		}
		pe = (uvsEscave*)pe -> next;
	}

	while(ps){
		ID = ps -> SeekID( type );
		if (ID != -1){
			ps -> TabuTaskGood++;
			return;
		}
		ps = (uvsSpot*)ps -> next;
	}
}

#ifdef _IN_FILE_
XStream ff("out", XS_OUT);
#endif

int uvsPrepareItemToDiagen(int type, int param1, int param2, int& what, int& ID, char*& NameFrom, char*& NameTo){

	switch(type){
	case UVS_ITEM_TYPE::POPONKA:
		what = DG_POPONKA;
		ID = param2;

		if (ID >= DG_POPONKA_MAX || ID < 0)
			ID = 0;

		NameFrom = NULL;
		NameTo = NULL;
		return 1;
		break;
	case UVS_ITEM_TYPE::TABUTASK:
		what = DG_TABUTASK;
#ifdef _IN_FILE_
		ff <= (param2 & 0x0000FFFF) < "\n";
#endif
		NameFrom = uvsGetNameByID(param2 & 0x0000FFFF, ID);
#ifdef _IN_FILE_
		ff < NameFrom < "\n";
#endif
		NameTo = NULL;
		ID = (param2 & 0x0000FFFF) - ID;
#ifdef _IN_FILE_
		ff <= ID < "\n";
#endif
		return 1;
		break;
	case UVS_ITEM_TYPE::ELEEPOD:
	case UVS_ITEM_TYPE::BEEBOORAT:
	case UVS_ITEM_TYPE::ZEEX:
		what = DG_PASSENGER;
		NameFrom = uvsGetNameByID(param2 & 0x0000FFFF, ID);
		NameTo = uvsGetNameByID(param2 >>16, ID);
		ID = 0;
		return 1;
		break;
	}//  end switch
	return 0;
}

void uvsRestoreTabuTask(void){
	int i;
	for( i = 0; i < MAX_TABUTASK; i++){
		if ( TabuTable[i] -> status == UVS_TABUTASK_STATUS::SLEEP)
			TabuTable[i] -> status = 0;
	}//  end for
}

void uvsChangeLuck(void){
	GamerResult.luck += 20;
	ShowLuckMessage(20);
	if (GamerResult.luck > 100) GamerResult.luck = 100;
}

void uvsChangeDominance(void){
	GamerResult.dominance += 20;
	ShowDominanceMessage(20);
	if (GamerResult.dominance > 100) GamerResult.dominance = 100;
}

uvsEscave* uvsGetEscaveByID(int type){
	uvsEscave *pe = (uvsEscave*)EscaveTail;
	int ID;

	while(pe){
		ID = pe -> SeekID( type );
		if (ID != -1)
			return pe;
		pe = (uvsEscave*)pe -> next;
	}

	return NULL;
}


uvsSpot* uvsGetSpotByID(int type){
	uvsSpot *ps = (uvsSpot*)SpotTail;
	int ID;

	while(ps){
		ID = ps -> SeekID( type );
		if (ID != -1)
			return ps;
		ps = (uvsSpot*)ps -> next;
	}

	return NULL;
}


uvsVanger* uvsCreateNetVanger(int CarType, int Color, int PassageIndex,int TownTabutaskID){
	uvsVanger *pv = FindFreeVanger();
	pv -> shape = UVS_VANGER_SHAPE::NETWORK;
	pv -> status = UVS_VANGER_STATUS::MOVEMENT;
	pv -> Pmechos -> type = CarType;
	pv -> Pmechos -> color = Color;
	pv -> Pworld = WorldTable[ CurrentWorld ];
	pv -> owner = NULL;

	if (pv -> Pitem) ErrH.Abort("uvsCreateNetVanger : Pitem not NULL");
	pv -> Pitem = NULL;

	if (PassageIndex != -1 && TownTabutaskID == -1){
		pv -> Pescave = NULL;
		pv -> Pspot = NULL;
		pv -> Ppassage = WorldTable[ CurrentWorld ] -> getPassage(WorldTable[PassageIndex]);
//		addVanger(pv, pv -> Ppassage);
	} else if (PassageIndex == -1 && TownTabutaskID != -1){
		pv -> Pescave =  uvsGetEscaveByID(TownTabutaskID);
		pv -> Pspot =  uvsGetSpotByID(TownTabutaskID);

//		if (pv -> Pescave)
//			addVanger(pv, pv -> Pescave);
//		else
//			addVanger(pv, pv -> Pspot);
	} else {
		pv -> Pescave =  NULL;
		pv -> Pspot =  NULL;
		pv -> Ppassage = NULL;
//		addVanger(pv, PassageIndex, TownTabutaskID);
	}
	return pv;
}

int uvsReturnTreasureStatus(int Type, int TreasureStatus){
	uvsEscave *pe = (uvsEscave*)EscaveTail;

	while(pe){
		if ( ((uvsItem*)pe -> Pitem) -> type == Type && ((uvsItem*)pe -> Pitem) -> param1 == TreasureStatus)
			return 1;
		pe = (uvsEscave*)pe -> next;
	}

	uvsSpot *ps = (uvsSpot*)SpotTail;

	while(ps){
		if ( ps -> Pitem && ((uvsItem*)ps -> Pitem) -> type == Type && ((uvsItem*)ps -> Pitem) -> param1 == TreasureStatus)
			return 1;
		ps = (uvsSpot*)ps -> next;
	}

	return 0;
}

int uvsReturnWorldGamerVisit(int where){
	if(where < 0 || where > 9) ErrH.Abort("uvsReturnWorldGamerVisit : Bad world");
	return WorldTable[where] -> GamerVisit;
}

// сколько раз был мире, английское имя которого на чинается с символа wc
int getWorldVisitStats(char wc){
	uvsWorld *pw = (uvsWorld*)WorldTail;

	while(pw && pw -> name[0] != wc ){
		pw = (uvsWorld*)pw -> next;
	}

	if (!pw) ErrH.Abort("getWorldVisitStats :: Incorrect world name");

	return pw -> GamerVisit;
}

// сколько пассажиров из эскейва, имя которого начинается на efrom,
//оставлено на мире wto
int getPassangerStats(char efrom, char wto){
	uvsWorld *pw = (uvsWorld*)WorldTail;

	while(pw && pw -> name[0] != wto ){
		pw = (uvsWorld*)pw -> next;
	}

	if (!pw) ErrH.Abort("getPassangerStats :: Incorrect world name");

	uvsEscave *pe = (uvsEscave*)EscaveTail;
	while(pe){
		if ( pe -> name[0] == efrom)
			return pe -> PassagerStatistic[pw -> gIndex];
		pe = (uvsEscave*)pe -> next;
	}

	uvsSpot *ps = (uvsSpot*)SpotTail;

	while(ps){
		if ( ps -> name[0] == efrom)
			return ps -> PassagerStatistic[pw -> gIndex];
		ps = (uvsSpot*)ps -> next;
	}

	return 0;
}

//возвращает количество пассажиров в трюме (до их изъятия при въезде в
//эскейв), едущих из эскейва, первый символ названия которого efrom
int getPassangerFrom(char efrom){
	uvsEscave *pe = (uvsEscave*)EscaveTail;
	while(pe){
		if ( pe -> name[0] == efrom)
			return pe -> PassagerFrom;
		pe = (uvsEscave*)pe -> next;
	}

	uvsSpot *ps = (uvsSpot*)SpotTail;

	while(ps){
		if ( ps -> name[0] == efrom)
			return ps -> PassagerFrom;
		ps = (uvsSpot*)ps -> next;
	}
	return 0;
}


void uvsPassagerFromStatistic(void){
	GamerResult.passagerOk = 0;
	char *NameTo;
	int ID;

	uvsEscave *pe = (uvsEscave*)EscaveTail;
	while(pe){
		pe -> PassagerFrom = 0;
		pe = (uvsEscave*)pe -> next;
	}

	uvsSpot *ps = (uvsSpot*)SpotTail;
	while(ps){
		ps -> PassagerFrom = 0;
		ps = (uvsSpot*)ps -> next;
	}

	uvsItem *pi = GetItem(Gamer -> Pitem, UVS_ITEM_TYPE::ELEEPOD, 0);

	while( pi ){
		pe = uvsGetEscaveByID(pi -> param2 & 0x0000FFFF);
		ps = uvsGetSpotByID(pi -> param2 & 0x0000FFFF);

		if (ps)
			ps -> PassagerFrom++;
		else
			pe -> PassagerFrom++;

		NameTo = uvsGetNameByID(pi -> param2 >> 16, ID);

		if (!strcmp(aci_curLocationName, NameTo))
			GamerResult.passagerOk++;

		pi = GetItem( pi -> next, UVS_ITEM_TYPE::ELEEPOD, 0 );
	}

	pi = GetItem(Gamer -> Pitem, UVS_ITEM_TYPE::BEEBOORAT, 0);

	while( pi ){
		pe = uvsGetEscaveByID(pi -> param2 & 0x0000FFFF);
		ps = uvsGetSpotByID(pi -> param2 & 0x0000FFFF);

		if (ps)
			ps -> PassagerFrom++;
		else
			pe -> PassagerFrom++;

		NameTo = uvsGetNameByID(pi -> param2 >> 16, ID);

		if (!strcmp(aci_curLocationName, NameTo))
			GamerResult.passagerOk++;

		pi = GetItem( pi -> next, UVS_ITEM_TYPE::BEEBOORAT, 0 );
	}

	pi = GetItem(Gamer -> Pitem, UVS_ITEM_TYPE::ZEEX, 0);

	while( pi ){
		pe = uvsGetEscaveByID(pi -> param2 & 0x0000FFFF);
		ps = uvsGetSpotByID(pi -> param2 & 0x0000FFFF);

		if (ps)
			ps -> PassagerFrom++;
		else
			pe -> PassagerFrom++;

		NameTo = uvsGetNameByID(pi -> param2 >> 16, ID);

		if (!strcmp(aci_curLocationName, NameTo))
			GamerResult.passagerOk++;

		pi = GetItem( pi -> next, UVS_ITEM_TYPE::ZEEX, 0 );
	}

#ifdef STAND_REPORT
	stand < "\nGood Passage " <= GamerResult.passagerOk < "\n";
#endif
}


void uvsTabuComplete(int i){
	uvsChangeTabuTask(i, UVS_TABUTASK_STATUS::GOOD);
	uvsChangeTownTabuTask(i);
}

void uvsTabuCreate(uvsActInt* pa, int num){
	int TabuTaskID;
	if (Gamer -> Pescave)
		TabuTaskID = Gamer -> Pescave -> TabuTaskID + num - 1;
	else
		TabuTaskID = Gamer -> Pspot -> TabuTaskID + num -1;

	pa -> param1 = Gamer -> Pworld -> gIndex<<16;
	pa -> param2 = TabuTaskID;
	pa -> price = 1;
	pa -> sell_price = 1;
}

void uvsBoorawchickZero(void){
	GamerResult.BoorawchickGoHimself = 0;
}

void uvsDeleteItem(listElem*& pe, int type, int count){
	uvsItem* pi = (uvsItem*)pe;
	uvsItem*pl;
	int i = 0;

	while(pi){
		if (pi -> type == type && i < count){
			pl = pi;
			pi = (uvsItem*)pi -> next;
			pl -> delink(pe);
			delete pl;
			i++;
		} else
			pi = (uvsItem*)pi -> next;
	}//  end while

	i = 0;
	uvsActInt* pa = GGamer;
	uvsActInt* pp;
	while( pa ){
		pp = (uvsActInt*)pa -> next;

		if (uvsItemTable[ type ] -> SteelerTypeFull == pa -> type && i < count){
			uvs_aciKillOneItem( pa ->type, pa -> pos_x, pa -> pos_y);
			pa -> delink(GGamer);
			delete pa;
			i++;
		}
		pa = pp;
	}//  end while
}//

int uvsCheckItemOnLive(int type){
	uvsBunch* pb = (uvsBunch*)BunchTail;
	uvsCultGame* pg;

	while(pb){
		pg = pb -> cycleTable[ pb -> currentStage ].Pgame;

		if (pg){
			if (pg -> GoodsTypeBeg == type) return 1;
		}
		pb = (uvsBunch*)pb -> next;
	}//  end while
	return 0;
}

int uvsCheckItem(int type){
	int i;
	uvsBunch* pb = (uvsBunch*)BunchTail;
	uvsCultGame* pg;

	while(pb){
		for( i = 0; i < pb -> cycleN; i++){
			pg = pb -> cycleTable[i].Pgame;

			if (pg){
				if (pg -> GoodsTypeBeg == type && pb -> cycleTable[i].Pdolly -> status) return 0;
			}
		}//  end for
		pb = (uvsBunch*)pb -> next;
	}//  end while
	return 1;
}

int uvsSecretON(void){
	int i, j;

	for(i = 0; i < 3; i++)
		for(j = 3; j < 10; j++)
			if (aciWorldLinkExist(i,j,0)) return 1;

	return 0;
}

void uvsOnKillItem(void){
	GamerResult.luck -= 10;
	ShowLuckMessage(-10);
	if (GamerResult.luck < 0) GamerResult.luck = 0;
}

int uvsKillerNow(void){
	uvsElement* p = ETail;
	int count  = 0;
	while(p){
		if(p -> type == UVS_OBJECT::VANGER && ((uvsVanger*)p) -> shape == UVS_VANGER_SHAPE::KILLER){
			count++;
		}
		p = p -> enext;
	}//  end while
	return count;
}
/*
void uvsCheckItemOne(listElem* pi){
	while(pi){
		if (uvsItemTable[ ((uvsItem*)pi) -> type ] -> type == UVS_ITEM_STATUS::MECHOS_PART ||  
		    uvsItemTable[ ((uvsItem*)pi) -> type ] -> type == UVS_ITEM_STATUS::ARTIFACT)
			ErrH.Abort("ITS VERY BAD THINHG");
		pi = pi -> next;
	}
}

void uvsCheckAllItem(void){
	listElem* pw = WorldTail;
	while(pw){
		uvsCheckItemOne(((uvsWorld*)pw) -> Pitem);
		pw = pw -> next;
	}

	pw = EscaveTail;
	while(pw){
		uvsCheckItemOne(((uvsEscave*)pw) -> Pshop -> Pitem);
		pw = pw -> next;
	}

	pw = SpotTail;
	while(pw){
		if (((uvsSpot*)pw) -> Pshop)
			uvsCheckItemOne(((uvsSpot*)pw) -> Pshop -> Pitem);
		pw = pw -> next;
	}

	uvsElement* p = ETail;
	while(p){
		if(p -> type == UVS_OBJECT::VANGER){
			uvsCheckItemOne(((uvsVanger*)p) -> Pitem);
		}
		p = p -> enext;
	}//  end while
}
*/
void uvsLuckChange(int delta ){
	GamerResult.luck += delta;
	ShowLuckMessage(delta);
	if (GamerResult.luck < 0) GamerResult.luck = 0;
	if (GamerResult.luck > 100) GamerResult.luck = 100;
}


int uvsWorldToCross(int fromWID, int toWID){
	uvsWorld* fromW = WorldTable[fromWID];
	uvsWorld* toW = WorldTable[toWID];
	uvsWorld* nextW,*tmpW = NULL;
	uvsWorld *lastfromW = NULL, *lasttoW = NULL;
	uvsPassage* pp;
	int i = 0;
	int _i_ = 0;

	if ( fromWID == 7 || toWID == 7)
		return -1;

	if (fromW -> gIndex >= MAIN_WORLD_MAX ) {
		lastfromW = fromW;
		fromW = fromW -> pssT[0] -> Poutput;

		_i_ += 1;
	}

	if (toW -> gIndex >= MAIN_WORLD_MAX ) {
		lasttoW = toW;
		toW = toW -> pssT[0] -> Poutput;
		_i_ += 1;
	}

	unsigned int linkage = ChainMap[toW -> gIndex + fromW -> gIndex*MAIN_WORLD_MAX],mI;

	if ( fromW == toW ) return _i_;

	for(i = 0; i < 8; i++, _i_++){
		mI = (linkage & (0x0000000F << 4*i)) >> 4*i;
		nextW = getWorld(mI);

		if(!nextW)	nextW = toW;
		pp = fromW -> getPassage(nextW);

		if ( !pp ) ErrH.Abort( "buildWay:: not passage");

		fromW = nextW;
		if ( fromW == toW ) break;
	}
	_i_++;
	return (_i_);
}
