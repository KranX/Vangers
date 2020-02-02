
/* ---------------------------- INCLUDE SECTION ----------------------------- */

#include "../global.h"

#include "../network.h"
#include "../xjoystick.h"

#include "ivmap.h"
#include "iworld.h"

#include "hfont.h"
#include "iscreen.h"
#include "controls.h"

#include "../actint/item_api.h"
#include "../actint/a_consts.h"
#include "../actint/actint.h"

#include "i_mem.h"
#include "ikeys.h"

/* ----------------------------- STRUCT SECTION ----------------------------- */

// MP Game Parameters...
#define iMP_INITIAL_CASH	0
#define iMP_ARTEFACTS_USING	1
#define iMP_IN_ESCAVE_TIME	2

// VAN-WAR...
#define iMP_TEAM_MODE		3
#define iMP_NASCENCY		4
#define iMP_ACCESS		5
#define iMP_MAX_KILLS		6
#define iMP_MAX_TIME		7

// MECHOSOMA...
#define iMP_WORLD		4
#define iMP_QUANTITY1		5
#define iMP_QUANTITY2		6
#define iMP_ONE_AT_A_TIME	7

// PASSEMBLOSS...
#define iMP_CHECKPOINTS_NUM	3
#define iMP_ESCAVE	4

// MUSTODONT...
#define iMP_MECHOS_NAME 	4

enum iMultiGameIDs
{
	iMP_VAN_WAR,
	iMP_MECHOSOMA,
	iMP_PASSEMBLOSS,
	iMP_HUNTAGE,
	iMP_MUSTODONT,

	iMP_MAX_ID
};

// iMultiGameParameter::type values...
#define iMP_STRING_NUMBER	0x00
#define iMP_TRIGGER_STATE	0x01

struct iMultiGameParameter
{
	int type;

	int MaxValue;
	int MinValue;

	int NumObjects;
	iListElement** objData;

	void alloc_mem(int num);
	void add_obj(iListElement* p,int id = 0){ objData[id] = p; }

	void block_obj(void);
	void unblock_obj(void);

	void set_value(int num);
	int get_value(void);

	iMultiGameParameter(void);
	~iMultiGameParameter(void);
};

struct iMultiGameData
{
	int NumParams;
	iMultiGameParameter** pData;

	void alloc_mem(int num);

	void block(void);
	void unblock(void);

	int get_value(int id);
	void set_value(int id,int num);

	iMultiGameData(void);
	~iMultiGameData(void);
};

#define iMP_MAX_PLACE		10
#define iMP_MAX_RESULT		10
#define iMP_MAX_HALL_RESULT	10

struct iMultiResultString
{
	char* place;
	char* prmString;
	char* prmNum;

	iListElement* placePtr;
	iListElement* stringPtr;
	iListElement* numberPtr;

	void init(const char* scr,char* pl,char* str,char* num);
	void redraw(void);

	void set_data(const char* pl,const char* ptr,const char* num);

	iMultiResultString(void);
};

/* ----------------------------- EXTERN SECTION ----------------------------- */

extern iScreenDispatcher* iScrDisp;
extern int aci_SecondMatrixID;
extern int aciCurMechosID;

extern int iRussian;
extern int iCurHall;

extern int actIntLog;
extern int iEvLineID;

extern int MuteLog;
extern int EffectInUsePriory,EffectInUse;

extern XBuffer* iResBuf;

extern char* iSTR_MP_Kills;
extern char* iSTR_MP_Deaths;
extern char* iSTR_MP_DeathTimeMin;
extern char* iSTR_MP_DeathTimeMax;
extern char* iSTR_MP_Kills_rate;
extern char* iSTR_MP_Deaths_rate;
extern char* iSTR_MP_Ware01_Delivery;
extern char* iSTR_MP_Ware02_Delivery;
extern char* iSTR_MP_Min_delivery_time;
extern char* iSTR_MP_Max_delivery_time;
extern char* iSTR_MP_StolenWares;
extern char* iSTR_MP_Wares_lost;
extern char* iSTR_MP_Total_time;
extern char* iSTR_MP_Min_checkpoint_time;
extern char* iSTR_MP_Max_checkpoint_time;
extern char* iSTR_MP_1st_part_delivery;
extern char* iSTR_MP_2nd_part_delivery;
extern char* iSTR_MP_Mechos_frame;
extern char* iSTR_MP_Mechos_assembled_in;
extern char* iSTR_NONE;
extern char* iSTR_Checkpoints_Number;


// English version...
const char* iJoystickButtons1[32] =
{
       "JoyBT1",
       "JoyBT2",
       "JoyBT3",
       "JoyBT4",
       "JoyBT5",
       "JoyBT6",
       "JoyBT7",
       "JoyBT8",
       "JoyBT9",
       "JoyBT10",
       "JoyBT11",
       "JoyBT12",
       "JoyBT13",
       "JoyBT14",
       "JoyBT15",
       "JoyBT16",
       "JoyBT17",
       "JoyBT18",
       "JoyBT19",
       "JoyBT20",
       "JoyBT21",
       "JoyBT22",
       "JoyBT23",
       "JoyBT24",
       "JoyBT25",
       "JoyBT26",
       "JoyBT27",
       "JoyBT28",
       "JoyBT29",
       "JoyBT30",
       "JoyBT31",
       "JoyBT32",
};

// Local version...
const char* iJoystickButtons2[32] =
{
       "JoyBT1",
       "JoyBT2",
       "JoyBT3",
       "JoyBT4",
       "JoyBT5",
       "JoyBT6",
       "JoyBT7",
       "JoyBT8",
       "JoyBT9",
       "JoyBT10",
       "JoyBT11",
       "JoyBT12",
       "JoyBT13",
       "JoyBT14",
       "JoyBT15",
       "JoyBT16",
       "JoyBT17",
       "JoyBT18",
       "JoyBT19",
       "JoyBT20",
       "JoyBT21",
       "JoyBT22",
       "JoyBT23",
       "JoyBT24",
       "JoyBT25",
       "JoyBT26",
       "JoyBT27",
       "JoyBT28",
       "JoyBT29",
       "JoyBT30",
       "JoyBT31",
       "JoyBT32",
};

// English version...
const char* iJoystickStickSwitch1[9] =
{
       "JoyDownLeft",
       "JoyDown",
       "JoyDownRight",
       "JoyLeft",
       "JoyCenter",
       "JoyRight",
       "JoyUpLeft",
       "JoyUp",
       "JoyUpRight"
};

// English version...
const char* iJoystickStickSwitch2[9] =
{
       "JoyDownLeft",
       "JoyDown",
       "JoyDownRight",
       "JoyLeft",
       "JoyCenter",
       "JoyRight",
       "JoyUpLeft",
       "JoyUp",
       "JoyUpRight"
};


/* --------------------------- PROTOTYPE SECTION ---------------------------- */

iScreen* create_screen(void);		// Test function...
void RegisterValues(void);

void iSortPlayers(int mode = 0);
void iPreparePlayerResults(int id);

void SetSoundVolume(int);
void EffectsOnOff(void);

iListElement* iGetObject(const char* scr,const char* obj);
void iPrepareOptions(void);

void iPrepareControls(void);
void iProcessControlsScreen(const char* name);
void iInitControlObjects(void);
void iDeleteControl(int vkey,int id);

void iInitMultiGames(void);
void iSetMultiGameParameter(int game_id,int p_id,int num);
int iGetMultiGameParameter(int game_id,int p_id);

void iSetMultiGameParameters(void);
void iGetMultiGameParameters(void);

void iLockMultiGameParameters(void);
void iUnlockMultiGameParameters(void);

void iUpdateMultiGameName(void);

/* --------------------------- DEFINITION SECTION --------------------------- */

int* AVI_index;

int* iLanguageIndex;
int* iMemoryIndex;
int* iSoundIndex;
int* iSoundVolume;
int* iSoundVolumeMax;

iScreenOption** iScrOpt = NULL;
iStringElement** iControlsStr = NULL;
iMultiGameData** iMP_Games;

int iCurMultiGame = iMP_VAN_WAR;

iMultiResultString** iPlaces;
iMultiResultString** iResults;
iMultiResultString** iHallPlaces;

int iNumPlayers = 0;
PlayerData** iPlayers = NULL;
double* iPlayerRatings = NULL;

void i_preExtQuant(void)
{
	iScreen* scr;
	iScreen* scrTmp;
	switch(CurEvComm -> EvType){
		case EV_EXT_SCREEN_JUMP:
			if(!strcmp(CurEvComm -> pvalue,"ExtScreen00")){
				scr = iScrDisp -> get_ext_screen("ExtScreen00");
				if(!scr){
					scrTmp = (iScreen*)iScrDisp -> get_object("Test template");
					scr = iCreateScreen("ExtScreen00",scrTmp);
					scr -> init();

					iScrDisp -> add_ext_screen(scr);
				}
			}
			if(!strcmp(CurEvComm -> pvalue,"ExtScreen01")){
				scr = iScrDisp -> get_ext_screen("ExtScreen01");
				if(!scr){
					scr = create_screen();
					scr -> init();

					iScrDisp -> add_ext_screen(scr);
				}
			}
			break;
	}
}

void i_postExtQuant(void)
{
}

iScreen* create_screen(void)
{
	iScreenEvent* ev;
	iScreenEventCommand* ec;
	iScreenObject* obj;

	iScreen* p = iCreateScreen("ExtScreen01");

/* ----------------- ‘®§¤ ­ЁҐ Ё Ё­ЁжЁ «Ё§ жЁп iScreenEvent ------------------ */
	ev = iCreateScreenEvent();
	ev -> time = 7;

	ec = iCreateEventCommand();
	ec -> EvType = EV_HIDE_SCREEN;
	ec -> time = 3;
	ec -> start_time = 0;

	ev -> add_command(ec);

	ec = iCreateEventCommand();
	ec -> EvType = EV_RISE_SCREEN;		// ’ЁЇл Є®¬ ­¤ - enum EventTypes ў ISCREEN.H
	ec -> time = 3;
	ec -> start_time = 4;

	ev -> add_command(ec);

	ec = iCreateEventCommand();
	ec -> EvType = EV_SCREEN_JUMP;
	ec -> init_objID("Main Screen");
	ec -> objType = I_SCREEN;		// ’ЁЇ ®ЎкҐЄв , б Є®в®ал¬ а Ў®в Ґв Є®¬ ­¤ :
						// enum EvObjTypes ў ISCREEN.H
	ec -> start_time = 3;

	ev -> add_command(ec);

	ev -> add_scancode(SDL_SCANCODE_SPACE);
	ev -> add_scancode(SDL_SCANCODE_RETURN);
	ev -> add_scancode(SDL_SCANCODE_ESCAPE);

	p -> add_event((iListElement*)ev);
/* -------------------------------------------------------------------------- */

/* ----------------- ‘®§¤ ­ЁҐ Ё Ё­ЁжЁ «Ё§ жЁп iScreenObject ----------------- */
	obj = iCreateScreenObject("Obj00");
	obj -> align_x = ALIGN_R;		// ‘¬. enum AlignTypes ў ISCREEN.H
	obj -> align_x_offs = 30;
	obj -> align_y = ALIGN_CENTER;

	// ---------- iBitmapElement (begin) ----------
	iBitmapElement* bt = (iBitmapElement*)iCreateScreenElement(I_BITMAP_ELEM);	// ’ЁЇл н«Ґ¬Ґ­в®ў - enum iElementTypes ў ISCREEN.H
	bt -> init_name("bitmap/coin.bmp");    // €¬п BMP,§ ¤ ­ЁҐ Ё¬Ґ­Ё н«Ґ¬Ґ­в  - init_id(char* p)

	bt -> align_x = ALIGN_CENTER;
	bt -> align_y = ALIGN_CENTER;

	bt -> bmp_null_level = 145;
	bt -> null_level = -10;

	obj -> add_element((iListElement*)bt);
	// ---------- iBitmapElement (end) ----------

	ev = iCreateScreenEvent();
	ev -> time = 21;
	ev -> add_scancode(-1);
	ev -> add_scancode(-2);
	ev -> add_scancode('P');

	ec = iCreateEventCommand();
	ec -> EvType = EV_SCALE_OBJECT;
	ec -> value = 128;
	ec -> time = 10;
	ec -> start_time = 0;
	ec -> objType = I_OBJECT;
	ec -> init_objID("Obj00");

	ev -> add_command(ec);

	ec = iCreateEventCommand();
	ec -> EvType = EV_SCALE_OBJECT;
	ec -> value = 256;
	ec -> time = 10;
	ec -> start_time = 10;
	ec -> objType = I_OBJECT;
	ec -> init_objID("Obj00");

	ev -> add_command(ec);
	obj -> add_event((iListElement*)ev);

	obj -> init();
/* -------------------------------------------------------------------------- */

	p -> add_object(obj);

/* ----------------- ‘®§¤ ­ЁҐ Ё Ё­ЁжЁ «Ё§ жЁп iTriggerObject ----------------- */
	iTriggerObject* trg = iCreateTriggerObject("Test trigger");
	trg -> align_x = ALIGN_L;
	trg -> align_x_offs = 30;

	trg -> PosY = 250;		// ‡ ¤ ­ЁҐ Є®®а¤Ё­ вл ®ЎкҐЄв  ­ Їап¬го,
					// Ґб«Ё гбв ­®ўЁвм ҐйҐ Ё align_y, в® Є®®а¤Ё­ в 
					// Ё§¬Ґ­Ёвбп Ї®б«Ґ ўл§®ў  init() ў б®®вўҐвбвўЁЁ б align_y
	trg -> num_state = 3;
	trg -> alloc_state();


	// ---------- iStringElement (begin) ----------
	iStringElement* str = (iStringElement*)iCreateScreenElement(I_STRING_ELEM);
	str -> align_x = ALIGN_CENTER;
	str -> align_y = ALIGN_CENTER;
	str -> font = 0;
	str -> init_string("FIRST");

	trg -> add_element((iListElement*)str);
	trg -> StateElem[0] = (iListElement*)str;
	// ---------- iStringElement (end) ----------

	// ---------- iStringElement (begin) ----------
	str = (iStringElement*)iCreateScreenElement(I_STRING_ELEM);
	str -> align_x = ALIGN_CENTER;
	str -> align_y = ALIGN_CENTER;
	str -> font = 0;
	str -> init_string("SECOND");

	trg -> add_element((iListElement*)str);
	trg -> StateElem[1] = (iListElement*)str;
	// ---------- iStringElement (end) ----------

	// ---------- iStringElement (begin) ----------
	str = (iStringElement*)iCreateScreenElement(I_STRING_ELEM);
	str -> align_x = ALIGN_CENTER;
	str -> align_y = ALIGN_CENTER;
	str -> font = 0;
	str -> init_string("THIRD");

	trg -> add_element((iListElement*)str);
	trg -> StateElem[2] = (iListElement*)str;
	// ---------- iStringElement (end) ----------

	ev = iCreateScreenEvent();
	ev -> time = 5;
	ev -> add_scancode(LEFT_MOUSE);
	ev -> add_scancode(RIGHT_MOUSE);
	ev -> add_scancode('T');

	ec = iCreateEventCommand();
	ec -> EvType = EV_CHANGE_TRIGGER;
	ec -> time = 5;
	ec -> start_time = 0;
	ec -> objType = I_TRIGGER;
	ec -> init_objID("Test trigger");

	ev -> add_command(ec);

	trg -> add_event((iListElement*)ev);
	trg -> init();

	p -> add_object(trg);
/* -------------------------------------------------------------------------- */

/* ----- ‘®§¤ ­ЁҐ б¬ бив ЎЁа®ў ­­®© ®Ў« бвЁ (®ЎкҐЄв  б iTerrainElement) ----- */

	obj = iCreateScreenObject("TERRAIN_OBJECT");
	obj -> PosY = 150;
	obj -> align_x = ALIGN_L;
	obj -> align_x_offs = 50;
	obj -> flags |= OBJ_TERRAIN;		// !!!

	// ---------- iTerrainElement (begin) ----------
	iTerrainElement* terr = (iTerrainElement*)iCreateScreenElement(I_TERRAIN_ELEM);

	terr -> scale_strength = 128;
	terr -> SizeX = terr -> SizeY = 80;
	terr -> def_null_level = 20;

	obj -> add_element((iListElement*)terr);
	// ---------- iTerrainElement (end) ----------
	obj -> init();

	p -> add_terr_object(obj);		// !!!
/* -------------------------------------------------------------------------- */

/* ------------------- ‘®§¤ ­ЁҐ Ё Ё­ЁжЁ «Ё§ жЁп бЄа®««Ґа  ------------------- */

	obj = iCreateScreenObject("Scroller object");

	obj -> PosY = 350;
	obj -> align_x = ALIGN_L;
	obj -> align_x_offs = 30;
	// ---------- iScrollerElement (begin) ----------
	iScrollerElement* scl = (iScrollerElement*)iCreateScreenElement(I_SCROLLER_ELEM,"Test scroller");       // ‡¤Ґбм Ё¬  н«Ґ¬Ґ­в  § ¤ Ґвбп, в.Є. ­  ­ҐЈ®
														// Ўг¤гв ббл«ЄЁ Ё§ б®ЎлвЁ©
	scl -> init_name("bitmap/scroller.bmp");
	scl -> bmp_null_level = 110;

	scl -> Value = scl -> prevValue = 3;
	scl -> maxValue = 8;

	scl -> dir = I_RIGHT;
	scl -> space = 2;

	obj -> add_element((iListElement*)scl);
	// ---------- iScrollerElement (end) ----------

	ev = iCreateScreenEvent();
	ev -> time = 5;
	ev -> add_scancode(LEFT_MOUSE);
	ev -> add_scancode(RIGHT_MOUSE);

	ec = iCreateEventCommand();
	ec -> EvType = EV_CHANGE_SCROLLER;
	ec -> time = 5;
	ec -> start_time = 0;
	ec -> objType = I_ELEMENT;
	ec -> init_objID("Test scroller");

	ev -> add_command(ec);

	obj -> add_event((iListElement*)ev);
	obj -> init();
	p -> add_object(obj);

/* -------------------------------------------------------------------------- */

/* ------------------ ‘®§¤ ­ЁҐ Ё Ё­ЁжЁ «Ё§ жЁп AVI ®ЎкҐЄв  ------------------ */

	obj = iCreateScreenObject("AVI object");
	obj -> align_x = obj -> align_y = ALIGN_CENTER;
	obj -> flags |= OBJ_AVI_PRESENT;		// !!!

	// ---------- iAVIElement (begin) ----------
	iAVIElement* avi = (iAVIElement*)iCreateScreenElement(I_AVI_ELEM,"Test AVI");
	avi -> border_level = 20;
	avi -> border_size = 16;
	avi -> null_level = 83;
	avi -> add_name("avi/mech1.avi");

	obj -> add_element((iListElement*)avi);
	// ---------- iAVIElement (end) ----------

	obj -> init();
	p -> add_object(obj);

/* -------------------------------------------------------------------------- */

	return p;
}

void RegisterValues(void)
{
	iScrollerElement* sl;
	iTriggerObject* trg;
	iScreen* scr;

	AVI_index = &aciCurMechosID;

	scr = (iScreen*)iScrDisp -> get_object("Options screen");
	if(!scr) return;

	trg = (iTriggerObject*)scr -> get_object("MemTrig");
	if(trg){
		iMemoryIndex = &trg -> state;
	}
	else
		iMemoryIndex = NULL;

	trg = (iTriggerObject*)scr -> get_object("LangTrig");
	if(trg){
		iLanguageIndex = &trg -> state;
	}
	else
		iLanguageIndex = NULL;

	trg = (iTriggerObject*)scr -> get_object("SndTrig");
	if(trg){
		iSoundIndex = &trg -> state;
	}
	else
		iSoundIndex = NULL;

	sl = (iScrollerElement*)scr -> get_object("VolumeScroller");
	if(sl){
		iSoundVolume = &sl -> Value;
		iSoundVolumeMax = &sl -> maxValue;
	}
	else {
		iSoundVolume = iSoundVolumeMax = NULL;
	}
}

void iSetFullscreen(int state) {
	XGR_Obj.set_fullscreen(state);
}

void iPrepareOptions(void)
{
	int i;
	iScrOpt = new iScreenOption*[iMAX_OPTION_ID];
	for(i = 0; i < iMAX_OPTION_ID; i ++)
		iScrOpt[i] = NULL;

	iScrOpt[iJOYSTICK_TYPE] = new iScreenOption(iTRIGGER,0,"Joystick screen","JoyTypeID");
	
	iScrOpt[iSOUND_ON] = new iScreenOption(iTRIGGER,0,"Sound screen","SoundTrig");
	iScrOpt[iSOUND_VOLUME_CUR] = new iScreenOption(iSCROLLER,0,"Sound screen","SndVolumeScroller");
	iScrOpt[iSOUND_VOLUME_MAX] = new iScreenOption(iSCROLLER,1,"Sound screen","SndVolumeScroller");

	iScrOpt[iMECH_SOUND] = new iScreenOption(iTRIGGER,0,"Sound screen","MechSndTrig");
	iScrOpt[iBACK_SOUND] = new iScreenOption(iTRIGGER,0,"Sound screen","BackSndTrig");

	iScrOpt[iMUSIC_ON] = new iScreenOption(iTRIGGER,0,"Sound screen","MusicTrig");
	iScrOpt[iPANNING_ON] = new iScreenOption(iTRIGGER,0,"Sound screen","PanningTrig");
	iScrOpt[iMUSIC_VOLUME_CUR] = new iScreenOption(iSCROLLER,0,"Sound screen","MusVolumeScroller");
	iScrOpt[iMUSIC_VOLUME_MAX] = new iScreenOption(iSCROLLER,1,"Sound screen","MusVolumeScroller");

	iScrOpt[iTUTORIAL_ON] = new iScreenOption(iTRIGGER,0,"Graphics screen","TutorialTrig");
	iScrOpt[iDETAIL_SETTING] = new iScreenOption(iTRIGGER,0,"Graphics screen","DetailTrig");

	iScrOpt[iSCREEN_RESOLUTION] = new iScreenOption(iTRIGGER,0,"Graphics screen","ResTrig");

	iScrOpt[iPLAYER_COLOR3] = new iScreenOption(iTRIGGER,0,"Identification screen2","ColorTrig");
	iScrOpt[iPLAYER_COLOR] = new iScreenOption(iTRIGGER,0,"Identification screen","ColorTrig");

	iScrOpt[iPLAYER_NAME2] = new iScreenOption(iSTRING,0,"Identification screen","PlayerName");
	iScrOpt[iPLAYER_NAME_CR] = new iScreenOption(iSTRING,0,"Identification screen2","PlayerName");
	iScrOpt[iPLAYER_NAME3] = new iScreenOption(iSTRING,0,"Configuration screen 2","PlayerName");

	iScrOpt[iPLAYER_COLOR2] = new iScreenOption(iTRIGGER,0,"Configuration screen 2","ColorTrig");
	iScrOpt[iPLAYER_PASSWORD] = new iScreenOption(iSTRING,0,"Identification screen","PasswordString");
	iScrOpt[iPLAYER_PASSWORD_CR] = new iScreenOption(iSTRING,0,"Identification screen2","PasswordString");

	iScrOpt[iPLAYER_PASSWORD] -> add_link(iScrOpt[iPLAYER_PASSWORD_CR]);
	iScrOpt[iPLAYER_PASSWORD_CR] -> add_link(iScrOpt[iPLAYER_PASSWORD]);

	iScrOpt[iPLAYER_NAME3] -> add_link(iScrOpt[iPLAYER_NAME2]);
	iScrOpt[iPLAYER_NAME2] -> add_link(iScrOpt[iPLAYER_NAME3]);

	iScrOpt[iPLAYER_NAME_CR] -> add_link(iScrOpt[iPLAYER_NAME2]);
	iScrOpt[iPLAYER_NAME_CR] -> add_link(iScrOpt[iPLAYER_NAME3]);

	iScrOpt[iPLAYER_NAME2] -> add_link(iScrOpt[iPLAYER_NAME_CR]);
	iScrOpt[iPLAYER_NAME3] -> add_link(iScrOpt[iPLAYER_NAME_CR]);

	iScrOpt[iPLAYER_COLOR] -> add_link(iScrOpt[iPLAYER_COLOR2]);
	iScrOpt[iPLAYER_COLOR] -> add_link(iScrOpt[iPLAYER_COLOR3]);

	iScrOpt[iPLAYER_COLOR2] -> add_link(iScrOpt[iPLAYER_COLOR]);
	iScrOpt[iPLAYER_COLOR2] -> add_link(iScrOpt[iPLAYER_COLOR3]);

	iScrOpt[iPLAYER_COLOR3] -> add_link(iScrOpt[iPLAYER_COLOR]);
	iScrOpt[iPLAYER_COLOR3] -> add_link(iScrOpt[iPLAYER_COLOR2]);

	iScrOpt[iSERVER_NAME] = new iScreenOption(iSTRING,0,"Server Config screen","ServerName");
	iScrOpt[iSERVER_NAME2] = new iScreenOption(iSTRING,0,"Server Info screen","ServerName");
	iScrOpt[iSERVER_NAME2] -> add_link(iScrOpt[iSERVER_NAME]);
	iScrOpt[iSERVER_NAME] -> add_link(iScrOpt[iSERVER_NAME2]);

	iScrOpt[iMPGAME0_ID] = new iScreenOption(iSTRING,0,"Select Game","GameStr0");
	iScrOpt[iMPGAME1_ID] = new iScreenOption(iSTRING,0,"Select Game","GameStr1");
	iScrOpt[iMPGAME2_ID] = new iScreenOption(iSTRING,0,"Select Game","GameStr2");
	iScrOpt[iMPGAME3_ID] = new iScreenOption(iSTRING,0,"Select Game","GameStr3");
	iScrOpt[iMPGAME4_ID] = new iScreenOption(iSTRING,0,"Select Game","GameStr4");

	iScrOpt[iMPGAME0_ID] -> flags |= iOPTION_NO_SAVE;
	iScrOpt[iMPGAME1_ID] -> flags |= iOPTION_NO_SAVE;
	iScrOpt[iMPGAME2_ID] -> flags |= iOPTION_NO_SAVE;
	iScrOpt[iMPGAME3_ID] -> flags |= iOPTION_NO_SAVE;
	iScrOpt[iMPGAME4_ID] -> flags |= iOPTION_NO_SAVE;

	iScrOpt[iCUR_MPGAME_ID] = new iScreenOption(iSTRING,0,"Server Config screen","GameNameStr");
	iScrOpt[iCUR_MPGAME_ID] -> flags |= iOPTION_NO_SAVE;
	iScrOpt[iCUR_MPGAME_ID2] = new iScreenOption(iSTRING,0,"Server Info screen","GameNameStr");
	iScrOpt[iCUR_MPGAME_ID2] -> flags |= iOPTION_NO_SAVE;

	iScrOpt[iCUR_MPGAME_ID] -> add_link(iScrOpt[iCUR_MPGAME_ID2]);
	iScrOpt[iCUR_MPGAME_ID2] -> add_link(iScrOpt[iCUR_MPGAME_ID]);

	iScrOpt[iHOST_NAME] = new iScreenOption(iSTRING,0,"INet Host screen","HostName");
	iScrOpt[iIP_ADDRESS] = new iScreenOption(iSTRING,0,"INet Host screen","IP_AddrID");
	iScrOpt[iIP_ADDRESS] -> flags |= iOPTION_NO_SAVE;

	iScrOpt[iKEEP_IN_USE] = new iScreenOption(iSTRING,0,"Graphics screen","InUseStr");
	iScrOpt[iKEEP_IN_USE] -> flags |= iOPTION_NO_SAVE;
	iScrOpt[iKEEP_CLEAN_UP] = new iScreenOption(iSTRING,0,"Graphics screen","CleanUpStr");
	iScrOpt[iKEEP_CLEAN_UP] -> flags |= iOPTION_NO_SAVE;

	iScrOpt[iKEEP_MODE] = new iScreenOption(iTRIGGER,0,"Graphics screen","KeepTrig");
	iScrOpt[iDESTR_MODE] = new iScreenOption(iTRIGGER,0,"Graphics screen","DestrTrig");
	
	iScrOpt[iPROXY_USAGE] = new iScreenOption(iTRIGGER,0,"INet Host screen","ProxyTrg");
	iScrOpt[iPROXY_SERVER] = new iScreenOption(iSTRING,0,"INet Host screen","ProxyServerName");
	iScrOpt[iPROXY_PORT] = new iScreenOption(iSTRING,0,"INet Host screen","ProxyPortNum");

	iScrOpt[iSERVER_PORT] = new iScreenOption(iSTRING,0,"Port screen","PortStrID");

	iScrOpt[iPROXY_SERVER_STR] = new iScreenOption(iSTRING,0,"INet Host screen","ProxyServerStrID");
	iScrOpt[iPROXY_SERVER_STR] -> flags |= iOPTION_NO_SAVE;
	iScrOpt[iPROXY_PORT_STR] = new iScreenOption(iSTRING,0,"INet Host screen","ProxyPortStrID");
	iScrOpt[iPROXY_PORT_STR] -> flags |= iOPTION_NO_SAVE;
	
	//Stalkerg hack for save fake menu options.
	iScrOpt[iCAMERA_TURN] = new iScreenOption(iTRIGGER,0,NULL,"CameraTurn");
	iScrOpt[iCAMERA_TURN]->objPtr = new iTriggerObject();
	iScrOpt[iCAMERA_SLOPE] = new iScreenOption(iTRIGGER,0,NULL,"CameraSlope");
	iScrOpt[iCAMERA_SLOPE]->objPtr = new iTriggerObject();
	iScrOpt[iCAMERA_SCALE] = new iScreenOption(iTRIGGER,0,NULL,"CameraScale");
	iScrOpt[iCAMERA_SCALE]->objPtr = new iTriggerObject();
	
	iScrOpt[iFULLSCREEN] = new iScreenOption(iTRIGGER,0,"Graphics screen","FullScreenTrig");
	((iTriggerObject *)iScrOpt[iFULLSCREEN]->objPtr)->callback = &iSetFullscreen;
	
	iPrepareControls();
}

iScreenOption::iScreenOption(int tp,int vtp,const char* scr,const char* obj)
{
	flags = 0;
	ObjectType = tp;
	ValueType = vtp;
	objPtr = iGetObject(scr,obj);

	pLinks = new XList;
}

void iScreenOption::add_link(iScreenOption* p)
{
	iScreenOptionPtr* ptr = new iScreenOptionPtr;
	ptr -> optionPtr = p;

	pLinks -> AddElement((XListElement*)ptr);
}

iListElement* iGetObject(const char* scr, const char* obj)
{
	iScreen* p;
	iListElement* el = NULL;
	if(scr) {
		p = (iScreen*)iScrDisp -> get_object(scr);
		el = p -> get_object(obj);
	} else {
		el = iScrDisp -> get_object(obj);
	}
	return el;
}

int iScreenOption::GetValueINT(void)
{
	int ret = 0;
	if(objPtr){
		switch(ObjectType){
			case iTRIGGER:
				if(ValueType == iOPTION_VALUE_CUR)
					ret = ((iTriggerObject*)objPtr) -> state;
				else
					ret = ((iTriggerObject*)objPtr) -> num_state;
				break;
			case iSCROLLER:
				if(ValueType == iOPTION_VALUE_CUR)
					ret = ((iScrollerElement*)objPtr) -> Value;
				else
					ret = ((iScrollerElement*)objPtr) -> maxValue;
				break;
		}
	}
	return ret;
}

void iScreenOption::save(XStream* fh)
{
	int ret = 0;
	char* ptr;

	if(flags & iOPTION_NO_SAVE) return;

	if(objPtr){
		switch(ObjectType){
			case iTRIGGER:
				if(ValueType == iOPTION_VALUE_CUR)
					ret = ((iTriggerObject*)objPtr) -> state;
				else
					ret = ((iTriggerObject*)objPtr) -> num_state;
				*fh < ret;
				break;
			case iSCROLLER:
				if(ValueType == iOPTION_VALUE_CUR)
					ret = ((iScrollerElement*)objPtr) -> Value;
				else
					ret = ((iScrollerElement*)objPtr) -> maxValue;
				*fh < ret;
				break;
			case iSTRING:
				if(ValueType == iOPTION_VALUE_CUR){
					ptr = ((iStringElement*)objPtr) -> string;
					ret = strlen(ptr);
					*fh < ret;
					fh -> write(ptr,ret);
				}
				break;
			case iS_STRING:
				if(ValueType == iOPTION_VALUE_CUR){
					ptr = ((iS_StringElement*)objPtr) -> string;
					ret = strlen(ptr);
					*fh < ret;
					fh -> write(ptr,ret);
				}
				break;
		}
	}
}

void iScreenOption::load(XStream* fh)
{
	int ret = 0;
	char* ptr;
	iScreenObject* obj;

	if(flags & iOPTION_NO_SAVE) return;

	if(objPtr){
		switch(ObjectType){
			case iTRIGGER:
				*fh > ret;
				if(ValueType == iOPTION_VALUE_CUR)
					((iTriggerObject*)objPtr) -> state = ret;
				else
					((iTriggerObject*)objPtr) -> num_state = ret;
				((iTriggerObject*)objPtr) -> trigger_init();
				break;
			case iSCROLLER:
				*fh > ret;
				if(ValueType == iOPTION_VALUE_CUR)
					((iScrollerElement*)objPtr) -> Value = ret;
				else
					((iScrollerElement*)objPtr) -> maxValue = ret;
				((iScrollerElement*)objPtr) -> scroller_init();
				break;
			case iSTRING:
				if(ValueType == iOPTION_VALUE_CUR){
					*fh > ret;
					ptr = ((iStringElement*)objPtr) -> string;
					fh -> read(ptr,ret);
					ptr[ret] = ptr[ret + 1] = 0;
					((iStringElement*)objPtr) -> init_size();
					((iStringElement*)objPtr) -> init_align();
					obj = (iScreenObject*)((iStringElement*)objPtr) -> owner;
					obj -> flags |= OBJ_REINIT;
				}
				break;
			case iS_STRING:
				if(ValueType == iOPTION_VALUE_CUR){
					*fh > ret;
					ptr = ((iS_StringElement*)objPtr) -> string;
					fh -> read(ptr,ret);
					ptr[ret] = ptr[ret + 1] = 0;
					((iS_StringElement*)objPtr) -> init_size();
					((iS_StringElement*)objPtr) -> init_align();
					obj = (iScreenObject*)((iS_StringElement*)objPtr) -> owner;
					obj -> flags |= OBJ_REINIT;
				}
				break;
		}
	}
}

void iScreenOption::SetValueINT(int val)
{
	iScreenOptionPtr* p = (iScreenOptionPtr*)pLinks -> fPtr;
	if(objPtr){
		switch(ObjectType){
			case iTRIGGER:
				if(ValueType == iOPTION_VALUE_CUR){
					((iTriggerObject*)objPtr) -> state = val;
					((iTriggerObject*)objPtr) -> trigger_init();
				}
				break;
			case iSCROLLER:
				if(ValueType == iOPTION_VALUE_CUR){
					((iScrollerElement*)objPtr) -> Value = val;
					((iScrollerElement*)objPtr) -> scroller_init();
				}
				break;
		}
	}
	while(p){
		if(p -> optionPtr -> objPtr){
			switch(p -> optionPtr -> ObjectType){
				case iTRIGGER:
					if(p -> optionPtr -> ValueType == iOPTION_VALUE_CUR){
						((iTriggerObject*)p -> optionPtr -> objPtr) -> state = val;
						((iTriggerObject*)p -> optionPtr -> objPtr) -> trigger_init();
					}
					break;
				case iSCROLLER:
					if(p -> optionPtr -> ValueType == iOPTION_VALUE_CUR){
						((iScrollerElement*)p -> optionPtr -> objPtr) -> Value = val;
						((iScrollerElement*)p -> optionPtr -> objPtr) -> scroller_init();
					}
					break;
			}
		}
		p = (iScreenOptionPtr*)p -> next;
	}
}

char* iScreenOption::GetValueCHR(void)
{
	if(objPtr){
		switch(ObjectType){
			case iSTRING:
				if(ValueType == iOPTION_VALUE_CUR){
					return ((iStringElement*)objPtr) -> string;
				}
				break;
			case iS_STRING:
				if(ValueType == iOPTION_VALUE_CUR){
					return ((iS_StringElement*)objPtr) -> string;
				}
				break;
		}
	}
	return NULL;
}

void iScreenOption::update(void)
{
	int value_int;
	char* value_ptr;

	if(objPtr){
		switch(ObjectType){
			case iTRIGGER:
			case iSCROLLER:
				value_int = GetValueINT();
				SetValueINT(value_int);
				break;
			case iSTRING:
			case iS_STRING:
				value_ptr = GetValueCHR();
				SetValueCHR(value_ptr);
				break;
		}
	}
}

void iScreenOption::SetValueCHR(const char* p)
{
	iScreenObject* obj;
	iScreenOptionPtr* ptr = (iScreenOptionPtr*)pLinks -> fPtr;
	if(objPtr){
		switch(ObjectType){
			case iSTRING:
				if(ValueType == iOPTION_VALUE_CUR){
					strcpy(((iStringElement*)objPtr) -> string,p);
					((iStringElement*)objPtr) -> init_size();
					((iStringElement*)objPtr) -> init_align();
					obj = (iScreenObject*)((iStringElement*)objPtr) -> owner;
					obj -> flags |= OBJ_REINIT;
					obj -> flags |= OBJ_MUST_REDRAW;
				}
				break;
			case iS_STRING:
				if(ValueType == iOPTION_VALUE_CUR){
					strcpy(((iS_StringElement*)objPtr) -> string,p);
					((iS_StringElement*)objPtr) -> init_size();
					((iS_StringElement*)objPtr) -> init_align();
					obj = (iScreenObject*)((iS_StringElement*)objPtr) -> owner;
					obj -> flags |= OBJ_REINIT;
					obj -> flags |= OBJ_MUST_REDRAW;
				}
				break;
		}
	}
	while(ptr){
		switch(ptr -> optionPtr -> ObjectType){
			case iSTRING:
				if(ptr -> optionPtr -> ValueType == iOPTION_VALUE_CUR){
					strcpy(((iStringElement*)ptr -> optionPtr -> objPtr) -> string,p);
					((iStringElement*)ptr -> optionPtr -> objPtr) -> init_size();
					((iStringElement*)ptr -> optionPtr -> objPtr) -> init_align();
					obj = (iScreenObject*)((iStringElement*)ptr -> optionPtr -> objPtr) -> owner;
					obj -> flags |= OBJ_REINIT;
					obj -> flags |= OBJ_MUST_REDRAW;
				}
				break;
			case iS_STRING:
				if(ptr -> optionPtr -> ValueType == iOPTION_VALUE_CUR){
					strcpy(((iS_StringElement*)ptr -> optionPtr -> objPtr) -> string,p);
					((iS_StringElement*)ptr -> optionPtr -> objPtr) -> init_size();
					((iS_StringElement*)ptr -> optionPtr -> objPtr) -> init_align();
					obj = (iScreenObject*)((iS_StringElement*)ptr -> optionPtr -> objPtr) -> owner;
					obj -> flags |= OBJ_REINIT;
					obj -> flags |= OBJ_MUST_REDRAW;
				}
				break;
		}
		ptr = (iScreenOptionPtr*)ptr -> next;
	}
}

int iGetOptionValue(int id)
{
	if(iScrOpt && iScrOpt[id])
		return iScrOpt[id] -> GetValueINT();
	else
		return 1;
}

iListElement* iGetOptionObj(int id)
{
	if(iScrOpt && iScrOpt[id])
		return iScrOpt[id] -> objPtr;
	return NULL;
}

void iSetOptionValue(int id,int val)
{
	if(iScrOpt && iScrOpt[id])
		iScrOpt[id] -> SetValueINT(val);
}

void iUpdateOptionValue(int id)
{
	if(iScrOpt && iScrOpt[id])
		iScrOpt[id] -> update();
}

char* iGetOptionValueCHR(int id)
{
	if(iScrOpt && iScrOpt[id])
		return iScrOpt[id] -> GetValueCHR();
	else
		return NULL;
}

void iSetOptionValueCHR(int id, const char* p)
{
	if(iScrOpt && iScrOpt[id])
		iScrOpt[id] -> SetValueCHR(p);
}

void iPrepareControls(void)
{
	int i;
	if(iControlsStr) return;

	iControlsStr = new iStringElement*[iKEY_MAX_ID * iKEY_OBJECT_SIZE];
	for(i = 0; i < iKEY_MAX_ID * iKEY_OBJECT_SIZE; i ++)
		iControlsStr[i] = NULL;

	iProcessControlsScreen("Controls screen");
	iProcessControlsScreen("Controls screen 2");
	iProcessControlsScreen("Controls screen 3");
	iProcessControlsScreen("Controls screen 4");
	iProcessControlsScreen("Controls screen 5");
	iProcessControlsScreen("Controls screen 6");
	iProcessControlsScreen("Controls screen 7");
	iProcessControlsScreen("Joystick screen");
}

void iProcessControlsScreen(const char* name)
{
	int i,j;
	XBuffer XBuf;
	iStringElement* el;
	iScreen* scr = (iScreen*)iScrDisp -> get_object(name);

	if(scr){
		for(i = 1; i < iKEY_MAX_ID; i ++){
			for(j = 0; j < iKEY_OBJECT_SIZE; j ++){
				if(!iControlsStr[i * iKEY_OBJECT_SIZE + j]){
					XBuf.init();
					XBuf < "KeyID" <= i;
					if(j) XBuf < "_" <= j;
					el = (iStringElement*)scr -> get_object(XBuf.address());
					if(el) iControlsStr[i * iKEY_OBJECT_SIZE + j] = el;
				}
			}
		}
	}
}

void iInitControlObjects(void)
{
	//std::cout<<"iInitControlObjects"<<std::endl;
	int i,j,key,index;
	const char* str;
	iScreenObject* obj;
	for(i = 0; i < iKEY_MAX_ID; i ++){
		for(j = 0; j < iKEY_OBJECT_SIZE; j ++){
			index = i * iKEY_OBJECT_SIZE + j;
			if(iControlsStr[index]) {
				key = iGetControlCode(i, j);
				//NEED Full Rewrite
				/*if(!(key & iJOYSTICK_MASK)){
					str = iGetKeyNameText(key,iRussian);
				}
				else {
					str = iGetJoyBtnNameText(key,iRussian);
				}*/
				str = iGetKeyNameText(key, iRussian);
				
				if (str) {
					if(strcasecmp(iControlsStr[index]->string,str)) {
						strcpy(iControlsStr[index]->string,str);
						obj = (iScreenObject*)iControlsStr[index] -> owner;
						obj -> flags |= OBJ_REINIT;
						obj -> flags |= OBJ_MUST_REDRAW;
					}
				} else {
					if(strcasecmp(iControlsStr[index] -> string,iSTR_NONE)) {
						strcpy(iControlsStr[index] -> string,iSTR_NONE);
						obj = (iScreenObject*)iControlsStr[index] -> owner;
						obj -> flags |= OBJ_REINIT;
						obj -> flags |= OBJ_MUST_REDRAW;
					}
				}
				
			}
		}
	}
}

void iDeleteControl(int vkey,int id)
{
	//std::cout<<"iDeleteControl vkey:"<<vkey<<" id:"<<id<<std::endl;
	int i,j,key,index;
	iScreenObject* obj;
	for(i = 0; i < iKEY_MAX_ID; i ++){
		for(j = 0; j < iKEY_OBJECT_SIZE; j ++){
			index = i * iKEY_OBJECT_SIZE + j;
			if(index != id && iControlsStr[index]){
				key = iGetControlCode(i,j);
				if(key == vkey){
					iSetControlCode(i,0,j);
					strcpy(iControlsStr[index] -> string,iSTR_NONE);
					obj = (iScreenObject*)iControlsStr[index] -> owner;
					obj -> flags |= OBJ_REINIT;
					obj -> flags |= OBJ_MUST_REDRAW;
				}
			}
		}
	}
}

iMultiGameParameter::iMultiGameParameter(void)
{
	type = iMP_STRING_NUMBER;
	MinValue = MaxValue = 0;

	NumObjects = 0;
	objData = NULL;
}

iMultiGameParameter::~iMultiGameParameter(void)
{
	if(objData)
		delete objData;
}

void iMultiGameParameter::alloc_mem(int num)
{
	NumObjects = num;
	objData = new iListElement*[num];
}

int iMultiGameParameter::get_value(void)
{
	iTriggerObject* trg;
	iStringElement* str;

	switch(type){
		case iMP_STRING_NUMBER:
			str = (iStringElement*)objData[0];
			return atoi(str -> string);
			break;
		case iMP_TRIGGER_STATE:
			trg = (iTriggerObject*)objData[0];
			return trg -> state;
			break;
	}
	return 0;
}

void iMultiGameParameter::set_value(int num)
{
	int i;

	iTriggerObject* trg;
	iStringElement* str;
	iScreenObject* obj;

	switch(type){
		case iMP_STRING_NUMBER:
			iResBuf -> init();
			*iResBuf <= num;
			for(i = 0; i < NumObjects; i ++){
				str = (iStringElement*)objData[i];
				strcpy(str -> string,iResBuf -> address());
				str -> init_size();
				str -> init_align();
				obj = (iScreenObject*)str -> owner;
				obj -> flags |= OBJ_REINIT;
				obj -> flags |= OBJ_MUST_REDRAW;
			}
			break;
		case iMP_TRIGGER_STATE:
			for(i = 0; i < NumObjects; i ++){
				trg = (iTriggerObject*)objData[i];
				trg -> state = num;
				trg -> trigger_init();
			}
			break;
	}
}

void iMultiGameParameter::block_obj(void)
{
	int i;

	iTriggerObject* trg;
	iStringElement* str;
	iScreenObject* obj;

	switch(type){
		case iMP_STRING_NUMBER:
			for(i = 0; i < NumObjects; i ++){
				str = (iStringElement*)objData[i];
				obj = (iScreenObject*)str -> owner;
				obj -> flags |= OBJ_LOCKED;
			}
			break;
		case iMP_TRIGGER_STATE:
			for(i = 0; i < NumObjects; i ++){
				trg = (iTriggerObject*)objData[i];
				trg -> flags |= OBJ_LOCKED;
			}
			break;
	}
}

void iMultiGameParameter::unblock_obj(void)
{
	int i;

	iTriggerObject* trg;
	iStringElement* str;
	iScreenObject* obj;

	switch(type){
		case iMP_STRING_NUMBER:
			for(i = 0; i < NumObjects; i ++){
				str = (iStringElement*)objData[i];
				obj = (iScreenObject*)str -> owner;
				obj -> flags &= ~OBJ_LOCKED;
			}
			break;
		case iMP_TRIGGER_STATE:
			for(i = 0; i < NumObjects; i ++){
				trg = (iTriggerObject*)objData[i];
				trg -> flags &= ~OBJ_LOCKED;
			}
			break;
	}
}

iMultiGameData::iMultiGameData(void)
{
	NumParams = 0;
	pData = NULL;
}

iMultiGameData::~iMultiGameData(void)
{
	int i;
	if(NumParams){
		for(i = 0; i < NumParams; i ++)
			delete pData[i];
		delete[] pData;
	}
}

void iMultiGameData::alloc_mem(int num)
{
	int i;
	NumParams = num;
	pData = new iMultiGameParameter*[num];
	for(i = 0; i < num; i ++)
		pData[i] = new iMultiGameParameter;
}

void iMultiGameData::block(void)
{
	int i;
	for(i = 0; i < NumParams; i ++)
		pData[i] -> block_obj();
}

void iMultiGameData::unblock(void)
{
	int i;
	for(i = 0; i < NumParams; i ++)
		pData[i] -> unblock_obj();
}

int iMultiGameData::get_value(int id)
{
	return pData[id] -> get_value();
}

void iMultiGameData::set_value(int id,int num)
{
	pData[id] -> set_value(num);
}

void iSetMultiGameParameter(int game_id,int p_id,int num)
{
	iMP_Games[game_id] -> set_value(p_id,num);
}

int iGetMultiGameParameter(int game_id,int p_id)
{
	return iMP_Games[game_id] -> get_value(p_id);
}

void iInitMultiGames(void)
{
	int i;
	iScreen* scr;
	iListElement* el;

	XBuffer XNum;
	XBuffer XStr;
	XBuffer XPlace;

	iMP_Games = new iMultiGameData*[iMP_MAX_ID];
	for(i = 0; i < iMP_MAX_ID; i ++){
		iMP_Games[i] = new iMultiGameData;
	}
	scr = (iScreen*)iScrDisp -> get_object("Game Params 0");
	if(!scr) ErrH.Abort("iMP_Screen not found...");
	iMP_Games[iMP_VAN_WAR] -> alloc_mem(8);

	el = scr -> get_object("BeebosNumberID");
	if(!el) ErrH.Abort("iObject not found...");
	iMP_Games[iMP_VAN_WAR] -> pData[iMP_INITIAL_CASH] -> alloc_mem(1);
	iMP_Games[iMP_VAN_WAR] -> pData[iMP_INITIAL_CASH] -> add_obj(el);

	el = scr -> get_object("ArtefactsTrig");
	if(!el) ErrH.Abort("iObject not found...");
	iMP_Games[iMP_VAN_WAR] -> pData[iMP_ARTEFACTS_USING] -> type = iMP_TRIGGER_STATE;
	iMP_Games[iMP_VAN_WAR] -> pData[iMP_ARTEFACTS_USING] -> alloc_mem(1);
	iMP_Games[iMP_VAN_WAR] -> pData[iMP_ARTEFACTS_USING] -> add_obj(el);

	el = scr -> get_object("TimeTrig");
	if(!el) ErrH.Abort("iObject not found...");
	iMP_Games[iMP_VAN_WAR] -> pData[iMP_IN_ESCAVE_TIME] -> type = iMP_TRIGGER_STATE;
	iMP_Games[iMP_VAN_WAR] -> pData[iMP_IN_ESCAVE_TIME] -> alloc_mem(1);
	iMP_Games[iMP_VAN_WAR] -> pData[iMP_IN_ESCAVE_TIME] -> add_obj(el);

	el = scr -> get_object("BirthTrig");
	if(!el) ErrH.Abort("iObject not found...");
	iMP_Games[iMP_VAN_WAR] -> pData[iMP_NASCENCY] -> type = iMP_TRIGGER_STATE;
	iMP_Games[iMP_VAN_WAR] -> pData[iMP_NASCENCY] -> alloc_mem(1);
	iMP_Games[iMP_VAN_WAR] -> pData[iMP_NASCENCY] -> add_obj(el);

	el = scr -> get_object("TeamTrig");
	if(!el) ErrH.Abort("iObject not found...");
	iMP_Games[iMP_VAN_WAR] -> pData[iMP_TEAM_MODE] -> type = iMP_TRIGGER_STATE;
	iMP_Games[iMP_VAN_WAR] -> pData[iMP_TEAM_MODE] -> alloc_mem(1);
	iMP_Games[iMP_VAN_WAR] -> pData[iMP_TEAM_MODE] -> add_obj(el);

	el = scr -> get_object("AccessTrig");
	if(!el) ErrH.Abort("iObject not found...");
	iMP_Games[iMP_VAN_WAR] -> pData[iMP_ACCESS] -> type = iMP_TRIGGER_STATE;
	iMP_Games[iMP_VAN_WAR] -> pData[iMP_ACCESS] -> alloc_mem(1);
	iMP_Games[iMP_VAN_WAR] -> pData[iMP_ACCESS] -> add_obj(el);

	el = scr -> get_object("MaxKillsID");
	if(!el) ErrH.Abort("iObject not found...");
	iMP_Games[iMP_VAN_WAR] -> pData[iMP_MAX_KILLS] -> alloc_mem(1);
	iMP_Games[iMP_VAN_WAR] -> pData[iMP_MAX_KILLS] -> add_obj(el);

	el = scr -> get_object("MaxTimeID");
	if(!el) ErrH.Abort("iObject not found...");
	iMP_Games[iMP_VAN_WAR] -> pData[iMP_MAX_TIME] -> alloc_mem(1);
	iMP_Games[iMP_VAN_WAR] -> pData[iMP_MAX_TIME] -> add_obj(el);

	scr = (iScreen*)iScrDisp -> get_object("Game Params 1");
	if(!scr) ErrH.Abort("iMP_Screen not found...");
	iMP_Games[iMP_MECHOSOMA] -> alloc_mem(8);

	el = scr -> get_object("BeebosNumberID");
	if(!el) ErrH.Abort("iObject not found...");
	iMP_Games[iMP_MECHOSOMA] -> pData[iMP_INITIAL_CASH] -> alloc_mem(1);
	iMP_Games[iMP_MECHOSOMA] -> pData[iMP_INITIAL_CASH] -> add_obj(el);

	el = scr -> get_object("ArtefactsTrig");
	if(!el) ErrH.Abort("iObject not found...");
	iMP_Games[iMP_MECHOSOMA] -> pData[iMP_ARTEFACTS_USING] -> type = iMP_TRIGGER_STATE;
	iMP_Games[iMP_MECHOSOMA] -> pData[iMP_ARTEFACTS_USING] -> alloc_mem(1);
	iMP_Games[iMP_MECHOSOMA] -> pData[iMP_ARTEFACTS_USING] -> add_obj(el);

	el = scr -> get_object("TimeTrig");
	if(!el) ErrH.Abort("iObject not found...");
	iMP_Games[iMP_MECHOSOMA] -> pData[iMP_IN_ESCAVE_TIME] -> type = iMP_TRIGGER_STATE;
	iMP_Games[iMP_MECHOSOMA] -> pData[iMP_IN_ESCAVE_TIME] -> alloc_mem(1);
	iMP_Games[iMP_MECHOSOMA] -> pData[iMP_IN_ESCAVE_TIME] -> add_obj(el);

	el = scr -> get_object("TeamTrig");
	if(!el) ErrH.Abort("iObject not found...");
	iMP_Games[iMP_MECHOSOMA] -> pData[iMP_TEAM_MODE] -> type = iMP_TRIGGER_STATE;
	iMP_Games[iMP_MECHOSOMA] -> pData[iMP_TEAM_MODE] -> alloc_mem(1);
	iMP_Games[iMP_MECHOSOMA] -> pData[iMP_TEAM_MODE] -> add_obj(el);

	el = scr -> get_object("OneTimeNumID");
	if(!el) ErrH.Abort("iObject not found...");
	iMP_Games[iMP_MECHOSOMA] -> pData[iMP_ONE_AT_A_TIME] -> alloc_mem(1);
	iMP_Games[iMP_MECHOSOMA] -> pData[iMP_ONE_AT_A_TIME] -> add_obj(el);

	iMP_Games[iMP_MECHOSOMA] -> pData[iMP_WORLD] -> type = iMP_TRIGGER_STATE;
	iMP_Games[iMP_MECHOSOMA] -> pData[iMP_WORLD] -> alloc_mem(3);
	el = scr -> get_object("WorldTrig");
	if(!el) ErrH.Abort("iObject not found...");
	iMP_Games[iMP_MECHOSOMA] -> pData[iMP_WORLD] -> add_obj(el);
	el = scr -> get_object("ItemStrTrig1");
	if(!el) ErrH.Abort("iObject not found...");
	iMP_Games[iMP_MECHOSOMA] -> pData[iMP_WORLD] -> add_obj(el,1);
	el = scr -> get_object("ItemStrTrig2");
	if(!el) ErrH.Abort("iObject not found...");
	iMP_Games[iMP_MECHOSOMA] -> pData[iMP_WORLD] -> add_obj(el,2);

	el = scr -> get_object("ItemsNumberID1");
	if(!el) ErrH.Abort("iObject not found...");
	iMP_Games[iMP_MECHOSOMA] -> pData[iMP_QUANTITY1] -> alloc_mem(1);
	iMP_Games[iMP_MECHOSOMA] -> pData[iMP_QUANTITY1] -> add_obj(el);

	el = scr -> get_object("ItemsNumberID2");
	if(!el) ErrH.Abort("iObject not found...");
	iMP_Games[iMP_MECHOSOMA] -> pData[iMP_QUANTITY2] -> alloc_mem(1);
	iMP_Games[iMP_MECHOSOMA] -> pData[iMP_QUANTITY2] -> add_obj(el);

	scr = (iScreen*)iScrDisp -> get_object("Game Params 2");
	if(!scr) ErrH.Abort("iMP_Screen not found...");
	iMP_Games[iMP_PASSEMBLOSS] -> alloc_mem(5);

	el = scr -> get_object("BeebosNumberID");
	if(!el) ErrH.Abort("iObject not found...");
	iMP_Games[iMP_PASSEMBLOSS] -> pData[iMP_INITIAL_CASH] -> alloc_mem(1);
	iMP_Games[iMP_PASSEMBLOSS] -> pData[iMP_INITIAL_CASH] -> add_obj(el);

	el = scr -> get_object("ArtefactsTrig");
	if(!el) ErrH.Abort("iObject not found...");
	iMP_Games[iMP_PASSEMBLOSS] -> pData[iMP_ARTEFACTS_USING] -> type = iMP_TRIGGER_STATE;
	iMP_Games[iMP_PASSEMBLOSS] -> pData[iMP_ARTEFACTS_USING] -> alloc_mem(1);
	iMP_Games[iMP_PASSEMBLOSS] -> pData[iMP_ARTEFACTS_USING] -> add_obj(el);

	el = scr -> get_object("TimeTrig");
	if(!el) ErrH.Abort("iObject not found...");
	iMP_Games[iMP_PASSEMBLOSS] -> pData[iMP_IN_ESCAVE_TIME] -> type = iMP_TRIGGER_STATE;
	iMP_Games[iMP_PASSEMBLOSS] -> pData[iMP_IN_ESCAVE_TIME] -> alloc_mem(1);
	iMP_Games[iMP_PASSEMBLOSS] -> pData[iMP_IN_ESCAVE_TIME] -> add_obj(el);

	el = scr -> get_object("ChecksInputStrID");
	if(!el) ErrH.Abort("iObject not found...");
	iMP_Games[iMP_PASSEMBLOSS] -> pData[iMP_CHECKPOINTS_NUM] -> alloc_mem(1);
	iMP_Games[iMP_PASSEMBLOSS] -> pData[iMP_CHECKPOINTS_NUM] -> add_obj(el);

	el = scr -> get_object("EscaveTrig");
	if(!el) ErrH.Abort("iObject not found...");
	iMP_Games[iMP_PASSEMBLOSS] -> pData[iMP_ESCAVE] -> type = iMP_TRIGGER_STATE;
	iMP_Games[iMP_PASSEMBLOSS] -> pData[iMP_ESCAVE] -> alloc_mem(1);
	iMP_Games[iMP_PASSEMBLOSS] -> pData[iMP_ESCAVE] -> add_obj(el);

	scr = (iScreen*)iScrDisp -> get_object("Game Params 3");
	if(!scr) ErrH.Abort("iMP_Screen not found...");
	iMP_Games[iMP_HUNTAGE] -> alloc_mem(3);

	el = scr -> get_object("BeebosNumberID");
	if(!el) ErrH.Abort("iObject not found...");
	iMP_Games[iMP_HUNTAGE] -> pData[iMP_INITIAL_CASH] -> alloc_mem(1);
	iMP_Games[iMP_HUNTAGE] -> pData[iMP_INITIAL_CASH] -> add_obj(el);

	el = scr -> get_object("ArtefactsTrig");
	if(!el) ErrH.Abort("iObject not found...");
	iMP_Games[iMP_HUNTAGE] -> pData[iMP_ARTEFACTS_USING] -> type = iMP_TRIGGER_STATE;
	iMP_Games[iMP_HUNTAGE] -> pData[iMP_ARTEFACTS_USING] -> alloc_mem(1);
	iMP_Games[iMP_HUNTAGE] -> pData[iMP_ARTEFACTS_USING] -> add_obj(el);

	el = scr -> get_object("TimeTrig");
	if(!el) ErrH.Abort("iObject not found...");
	iMP_Games[iMP_HUNTAGE] -> pData[iMP_IN_ESCAVE_TIME] -> type = iMP_TRIGGER_STATE;
	iMP_Games[iMP_HUNTAGE] -> pData[iMP_IN_ESCAVE_TIME] -> alloc_mem(1);
	iMP_Games[iMP_HUNTAGE] -> pData[iMP_IN_ESCAVE_TIME] -> add_obj(el);

	scr = (iScreen*)iScrDisp -> get_object("Game Params 4");
	if(!scr) ErrH.Abort("iMP_Screen not found...");
	iMP_Games[iMP_MUSTODONT] -> alloc_mem(5);

	el = scr -> get_object("BeebosNumberID");
	if(!el) ErrH.Abort("iObject not found...");
	iMP_Games[iMP_MUSTODONT] -> pData[iMP_INITIAL_CASH] -> alloc_mem(1);
	iMP_Games[iMP_MUSTODONT] -> pData[iMP_INITIAL_CASH] -> add_obj(el);

	el = scr -> get_object("ArtefactsTrig");
	if(!el) ErrH.Abort("iObject not found...");
	iMP_Games[iMP_MUSTODONT] -> pData[iMP_ARTEFACTS_USING] -> type = iMP_TRIGGER_STATE;
	iMP_Games[iMP_MUSTODONT] -> pData[iMP_ARTEFACTS_USING] -> alloc_mem(1);
	iMP_Games[iMP_MUSTODONT] -> pData[iMP_ARTEFACTS_USING] -> add_obj(el);

	el = scr -> get_object("TimeTrig");
	if(!el) ErrH.Abort("iObject not found...");
	iMP_Games[iMP_MUSTODONT] -> pData[iMP_IN_ESCAVE_TIME] -> type = iMP_TRIGGER_STATE;
	iMP_Games[iMP_MUSTODONT] -> pData[iMP_IN_ESCAVE_TIME] -> alloc_mem(1);
	iMP_Games[iMP_MUSTODONT] -> pData[iMP_IN_ESCAVE_TIME] -> add_obj(el);

	el = scr -> get_object("TeamTrig");
	if(!el) ErrH.Abort("iObject not found...");
	iMP_Games[iMP_MUSTODONT] -> pData[iMP_TEAM_MODE] -> type = iMP_TRIGGER_STATE;
	iMP_Games[iMP_MUSTODONT] -> pData[iMP_TEAM_MODE] -> alloc_mem(1);
	iMP_Games[iMP_MUSTODONT] -> pData[iMP_TEAM_MODE] -> add_obj(el);

	el = scr -> get_object("MechosTrig");
	if(!el) ErrH.Abort("iObject not found...");
	iMP_Games[iMP_MUSTODONT] -> pData[iMP_MECHOS_NAME] -> type = iMP_TRIGGER_STATE;
	iMP_Games[iMP_MUSTODONT] -> pData[iMP_MECHOS_NAME] -> alloc_mem(1);
	iMP_Games[iMP_MUSTODONT] -> pData[iMP_MECHOS_NAME] -> add_obj(el);

	iPlaces = new iMultiResultString*[iMP_MAX_PLACE];
	iResults = new iMultiResultString*[iMP_MAX_RESULT];
	iHallPlaces = new iMultiResultString*[iMP_MAX_HALL_RESULT];

	for(i = 0; i < iMP_MAX_PLACE; i ++){
		iPlaces[i] = new iMultiResultString;

		XStr.init();
		XStr < "PrmStr" <= i;

		XNum.init();
		XNum < "NumStr" <= i;

		XPlace.init();
		XPlace < "PlaceStr" <= i;

		iPlaces[i] -> init("MultiGame Results screen",XPlace.address(),XStr.address(),XNum.address());
	}
	for(i = 0; i < iMP_MAX_RESULT; i ++){
		iResults[i] = new iMultiResultString;

		XStr.init();
		XStr < "PrmStr" <= i;

		XNum.init();
		XNum < "NumStr" <= i;

		XPlace.init();
		XPlace < "PlaceStr" <= i;

		iResults[i] -> init("MultiGame Results screen 2",XPlace.address(),XStr.address(),XNum.address());
	}
	for(i = 0; i < iMP_MAX_HALL_RESULT; i ++){
		iHallPlaces[i] = new iMultiResultString;

		XStr.init();
		XStr < "PrmStr" <= i;

		XNum.init();
		XNum < "NumStr" <= i;

		XPlace.init();
		XPlace < "PlaceStr" <= i;

		iHallPlaces[i] -> init("HallOfFame screen",XPlace.address(),XStr.address(),XNum.address());
	}
}

void iGetMultiGameParameters(void)
{
	int value;
	time_t tm;

	my_server_data.GameType = iCurMultiGame;

	switch(iCurMultiGame){
		case iMP_VAN_WAR:
			value = iGetMultiGameParameter(iMP_VAN_WAR,iMP_INITIAL_CASH);
			my_server_data.Van_War.InitialCash = value;

			value = iGetMultiGameParameter(iMP_VAN_WAR,iMP_ARTEFACTS_USING);
			my_server_data.Van_War.ArtefactsUsing = value;

			value = iGetMultiGameParameter(iMP_VAN_WAR,iMP_IN_ESCAVE_TIME);
			my_server_data.Van_War.InEscaveTime = value;

			value = iGetMultiGameParameter(iMP_VAN_WAR,iMP_TEAM_MODE);
			my_server_data.Van_War.TeamMode = value;

			value = iGetMultiGameParameter(iMP_VAN_WAR,iMP_NASCENCY);
			my_server_data.Van_War.Nascency = value - 1;

			value = iGetMultiGameParameter(iMP_VAN_WAR,iMP_ACCESS);
			my_server_data.Van_War.WorldAccess = value;

			value = iGetMultiGameParameter(iMP_VAN_WAR,iMP_MAX_KILLS);
			my_server_data.Van_War.MaxKills = value;

			value = iGetMultiGameParameter(iMP_VAN_WAR,iMP_MAX_TIME);
			my_server_data.Van_War.MaxTime = value;
			break;
		case iMP_MECHOSOMA:
			value = iGetMultiGameParameter(iMP_MECHOSOMA,iMP_INITIAL_CASH);
			my_server_data.Mechosoma.InitialCash = value;

			value = iGetMultiGameParameter(iMP_MECHOSOMA,iMP_ARTEFACTS_USING);
			my_server_data.Mechosoma.ArtefactsUsing = value;

			value = iGetMultiGameParameter(iMP_MECHOSOMA,iMP_IN_ESCAVE_TIME);
			my_server_data.Mechosoma.InEscaveTime = value;

			value = iGetMultiGameParameter(iMP_MECHOSOMA,iMP_TEAM_MODE);
			my_server_data.Mechosoma.TeamMode = value;

			value = iGetMultiGameParameter(iMP_MECHOSOMA,iMP_WORLD);
			my_server_data.Mechosoma.World = value;

			value = iGetMultiGameParameter(iMP_MECHOSOMA,iMP_QUANTITY1);
			my_server_data.Mechosoma.ProductQuantity1 = value;

			value = iGetMultiGameParameter(iMP_MECHOSOMA,iMP_QUANTITY2);
			my_server_data.Mechosoma.ProductQuantity2 = value;

			value = iGetMultiGameParameter(iMP_MECHOSOMA,iMP_ONE_AT_A_TIME);
			my_server_data.Mechosoma.One_at_a_time = value;
			break;
		case iMP_PASSEMBLOSS:
			value = iGetMultiGameParameter(iMP_PASSEMBLOSS,iMP_INITIAL_CASH);
			my_server_data.Passembloss.InitialCash = value;

			value = iGetMultiGameParameter(iMP_PASSEMBLOSS,iMP_ARTEFACTS_USING);
			my_server_data.Passembloss.ArtefactsUsing = value;

			value = iGetMultiGameParameter(iMP_PASSEMBLOSS,iMP_IN_ESCAVE_TIME);
			my_server_data.Passembloss.InEscaveTime = value;

			time(&tm);

			value = iGetMultiGameParameter(iMP_PASSEMBLOSS,iMP_CHECKPOINTS_NUM);
			my_server_data.Passembloss.CheckpointsNumber = value;

			value = iGetMultiGameParameter(iMP_PASSEMBLOSS,iMP_ESCAVE);
			my_server_data.Passembloss.RandomEscave = value - 1;
//			my_server_data.Passembloss.RandomEscave = tm % 3;
			break;
		case iMP_HUNTAGE:
			value = iGetMultiGameParameter(iMP_HUNTAGE,iMP_INITIAL_CASH);
			my_server_data.Huntage.InitialCash = value;

			value = iGetMultiGameParameter(iMP_HUNTAGE,iMP_ARTEFACTS_USING);
			my_server_data.Huntage.ArtefactsUsing = value;

			value = iGetMultiGameParameter(iMP_HUNTAGE,iMP_IN_ESCAVE_TIME);
			my_server_data.Huntage.InEscaveTime = value;
			break;
		case iMP_MUSTODONT:
			value = iGetMultiGameParameter(iMP_MUSTODONT,iMP_INITIAL_CASH);
			my_server_data.Mustodont.InitialCash = value;

			value = iGetMultiGameParameter(iMP_MUSTODONT,iMP_ARTEFACTS_USING);
			my_server_data.Mustodont.ArtefactsUsing = value;

			value = iGetMultiGameParameter(iMP_MUSTODONT,iMP_IN_ESCAVE_TIME);
			my_server_data.Mustodont.InEscaveTime = value;

			value = iGetMultiGameParameter(iMP_MUSTODONT,iMP_TEAM_MODE);
			my_server_data.Mustodont.TeamMode = value;
			break;
	}
}

void iSetMultiGameParameters(void)
{
	int value = 0;

	iCurMultiGame = my_server_data.GameType;

	switch(iCurMultiGame){
		case iMP_VAN_WAR:
			value = my_server_data.Van_War.InitialCash;
			iSetMultiGameParameter(iMP_VAN_WAR,iMP_INITIAL_CASH,value);

			value = my_server_data.Van_War.ArtefactsUsing;
			iSetMultiGameParameter(iMP_VAN_WAR,iMP_ARTEFACTS_USING,value);

			value = my_server_data.Van_War.InEscaveTime;
			iSetMultiGameParameter(iMP_VAN_WAR,iMP_IN_ESCAVE_TIME,value);

			value = my_server_data.Van_War.TeamMode;
			iSetMultiGameParameter(iMP_VAN_WAR,iMP_TEAM_MODE,value);

			value = my_server_data.Van_War.Nascency + 1;
			if(value > 3) value = 0;
			iSetMultiGameParameter(iMP_VAN_WAR,iMP_NASCENCY,value);

			value = my_server_data.Van_War.WorldAccess;
			iSetMultiGameParameter(iMP_VAN_WAR,iMP_ACCESS,value);

			value = my_server_data.Van_War.MaxKills;
			iSetMultiGameParameter(iMP_VAN_WAR,iMP_MAX_KILLS,value);

			value = my_server_data.Van_War.MaxTime;
			iSetMultiGameParameter(iMP_VAN_WAR,iMP_MAX_TIME,value);
			break;
		case iMP_MECHOSOMA:
			value = my_server_data.Mechosoma.InitialCash;
			iSetMultiGameParameter(iMP_MECHOSOMA,iMP_INITIAL_CASH,value);

			value = my_server_data.Mechosoma.ArtefactsUsing;
			iSetMultiGameParameter(iMP_MECHOSOMA,iMP_ARTEFACTS_USING,value);

			value = my_server_data.Mechosoma.InEscaveTime;
			iSetMultiGameParameter(iMP_MECHOSOMA,iMP_IN_ESCAVE_TIME,value);

			value = my_server_data.Mechosoma.TeamMode;
			iSetMultiGameParameter(iMP_MECHOSOMA,iMP_TEAM_MODE,value);

			value = my_server_data.Mechosoma.World;
			iSetMultiGameParameter(iMP_MECHOSOMA,iMP_WORLD,value);

			value = my_server_data.Mechosoma.ProductQuantity1;
			iSetMultiGameParameter(iMP_MECHOSOMA,iMP_QUANTITY1,value);

			value = my_server_data.Mechosoma.ProductQuantity2;
			iSetMultiGameParameter(iMP_MECHOSOMA,iMP_QUANTITY2,value);
			break;
		case iMP_PASSEMBLOSS:
			value = my_server_data.Passembloss.InitialCash;
			iSetMultiGameParameter(iMP_PASSEMBLOSS,iMP_INITIAL_CASH,value);

			value = my_server_data.Passembloss.ArtefactsUsing;
			iSetMultiGameParameter(iMP_PASSEMBLOSS,iMP_ARTEFACTS_USING,value);

			value = my_server_data.Passembloss.InEscaveTime;
			iSetMultiGameParameter(iMP_PASSEMBLOSS,iMP_IN_ESCAVE_TIME,value);

			value = my_server_data.Passembloss.CheckpointsNumber;
			iSetMultiGameParameter(iMP_PASSEMBLOSS,iMP_CHECKPOINTS_NUM,value);

			value = my_server_data.Passembloss.RandomEscave + 1;
			if(value > 3) value = 0;
			iSetMultiGameParameter(iMP_PASSEMBLOSS,iMP_ESCAVE,value);
			break;
		case iMP_HUNTAGE:
			value = my_server_data.Huntage.InitialCash;
			iSetMultiGameParameter(iMP_HUNTAGE,iMP_INITIAL_CASH,value);

			value = my_server_data.Huntage.ArtefactsUsing;
			iSetMultiGameParameter(iMP_HUNTAGE,iMP_ARTEFACTS_USING,value);

			value = my_server_data.Huntage.InEscaveTime;
			iSetMultiGameParameter(iMP_HUNTAGE,iMP_IN_ESCAVE_TIME,value);
			break;
		case iMP_MUSTODONT:
			value = my_server_data.Mustodont.InitialCash;
			iSetMultiGameParameter(iMP_MUSTODONT,iMP_INITIAL_CASH,value);

			value = my_server_data.Mustodont.ArtefactsUsing;
			iSetMultiGameParameter(iMP_MUSTODONT,iMP_ARTEFACTS_USING,value);

			value = my_server_data.Mustodont.InEscaveTime;
			iSetMultiGameParameter(iMP_MUSTODONT,iMP_IN_ESCAVE_TIME,value);

			value = my_server_data.Mustodont.TeamMode;
			iSetMultiGameParameter(iMP_MUSTODONT,iMP_TEAM_MODE,value);
			break;
	}
}

void iUpdateMultiGameName(void)
{
	char* name = NULL;
	switch(iCurMultiGame){
		case 0:
			name = iGetOptionValueCHR(iMPGAME0_ID);
			break;
		case 1:
			name = iGetOptionValueCHR(iMPGAME1_ID);
			break;
		case 2:
			name = iGetOptionValueCHR(iMPGAME2_ID);
			break;
		case 3:
			name = iGetOptionValueCHR(iMPGAME3_ID);
			break;
		case 4:
			name = iGetOptionValueCHR(iMPGAME4_ID);
			break;
	}
	if(!name) return;
	iSetOptionValueCHR(iCUR_MPGAME_ID,name);
}

void iLockMultiGameParameters(void)
{
	int i;
	for(i = 0; i < iMP_MAX_ID; i ++){
		iMP_Games[i] -> block();
	}
}

void iUnlockMultiGameParameters(void)
{
	int i;
	for(i = 0; i < iMP_MAX_ID; i ++){
		iMP_Games[i] -> unblock();
	}
}

iMultiResultString::iMultiResultString(void)
{
	place = new char[256];
	prmString = new char[256];
	prmNum = new char[256];

	placePtr = stringPtr = numberPtr = NULL;
}

void iMultiResultString::init(const char* scr,char* pl,char* str,char* num)
{
	placePtr = iGetObject(scr,pl);
	stringPtr = iGetObject(scr,str);
	numberPtr = iGetObject(scr,num);
}

void iMultiResultString::redraw(void)
{
	iScreenObject* obj;
	iStringElement* el;

	if(placePtr){
		el = (iStringElement*)placePtr;
		obj = (iScreenObject*)el -> owner;

		iResBuf -> init();
		*iResBuf < place;

		strcpy(el -> string,iResBuf -> address());
		obj -> flags |= OBJ_REINIT;
		obj -> flags |= OBJ_MUST_REDRAW;
	}
	if(stringPtr){
		el = (iStringElement*)stringPtr;
		obj = (iScreenObject*)el -> owner;

		strcpy(el -> string,prmString);
		obj -> flags |= OBJ_REINIT;
		obj -> flags |= OBJ_MUST_REDRAW;
		if(!strlen(prmString))
			obj -> flags |= OBJ_LOCKED;
	}
	if(numberPtr){
		el = (iStringElement*)numberPtr;
		obj = (iScreenObject*)el -> owner;

		iResBuf -> init();
		*iResBuf < prmNum;

		strcpy(el -> string,iResBuf -> address());
		obj -> flags |= OBJ_REINIT;
		obj -> flags |= OBJ_MUST_REDRAW;
	}
}

void iMultiResultString::set_data(const char* pl,const char* ptr,const char* num)
{
	strcpy(place,pl);
	strcpy(prmString,ptr);
	strcpy(prmNum,num);
}

void iPrepareResults(void)
{
	int i,num;
	XBuffer XPlace,XNum;

	XNum.digits = 3;

	iSortPlayers();
	num = (iNumPlayers < iMP_MAX_PLACE) ? iNumPlayers : iMP_MAX_PLACE;

	for(i = 0; i < num; i ++){
		XPlace.init();
		XPlace <= i + 1 < ".";
		XNum.init();

		XNum <= (int)round(iPlayerRatings[i]);

		iPlaces[i] -> set_data(XPlace.address(),iPlayers[i] -> name,XNum.address());
		iPlaces[i] -> redraw();
	}

	for(i = num; i < iMP_MAX_PLACE; i ++){
		iPlaces[i] -> set_data("","","");
		iPlaces[i] -> redraw();
	}
}

void iSortPlayers(int mode)
{
	int i,flag = 1,num;
	double tmp;

	PlayerData* p = players_list.first();
	num = players_list.size();

	if(iPlayers) delete iPlayers;
	if(iPlayerRatings) delete iPlayerRatings;

	iPlayers = new PlayerData*[num];
	iPlayerRatings = new double[num];

	iNumPlayers = 0;
	for(i = 0; i < num; i ++){
		// Играющие отображаются всегда, прошедшие - только в результатах игры
		if(p -> name && (p -> status == GAMING_STATUS || (p -> status == FINISHED_STATUS && !mode))){
			iPlayers[iNumPlayers] = p;
			iPlayerRatings[iNumPlayers] = p -> body.rating;
			iNumPlayers ++;
		}
		p = (PlayerData*)p -> next;
	}

	if(mode){
		while(flag){
			flag = 0;
			for(i = 0; i < iNumPlayers - 1; i ++){
				if(iCurMultiGame == iMP_VAN_WAR || iCurMultiGame == iMP_HUNTAGE){
					if(iPlayers[i] -> body.kills - iPlayers[i] -> body.deaths < iPlayers[i + 1] -> body.kills - iPlayers[i + 1] -> body.deaths){
						p = iPlayers[i];
						iPlayers[i] = iPlayers[i + 1];
						iPlayers[i + 1] = p;
						flag = 1;
					}
				}
				if(iCurMultiGame == iMP_MECHOSOMA){
					if(iPlayers[i] -> body.MechosomaStat.ItemCount1 + iPlayers[i] -> body.MechosomaStat.ItemCount2 < iPlayers[i + 1] -> body.MechosomaStat.ItemCount1 + iPlayers[i + 1] -> body.MechosomaStat.ItemCount2){
						p = iPlayers[i];
						iPlayers[i] = iPlayers[i + 1];
						iPlayers[i + 1] = p;
						flag = 1;
					}
				}
				if(iCurMultiGame == iMP_PASSEMBLOSS){
					if(iPlayers[i] -> body.PassemblossStat.CheckpointLighting < iPlayers[i + 1] -> body.PassemblossStat.CheckpointLighting){
						p = iPlayers[i];
						iPlayers[i] = iPlayers[i + 1];
						iPlayers[i + 1] = p;
						flag = 1;
					}
				}
				if(iCurMultiGame == iMP_MUSTODONT){
					if(!iPlayers[i] -> body.MustodontStat.MakeTime && iPlayers[i + 1] -> body.MustodontStat.MakeTime){
						p = iPlayers[i];
						iPlayers[i] = iPlayers[i + 1];
						iPlayers[i + 1] = p;
						flag = 1;
					}
					if(iPlayers[i] -> body.MustodontStat.MakeTime && iPlayers[i + 1] -> body.MustodontStat.MakeTime && iPlayers[i] -> body.MustodontStat.MakeTime > iPlayers[i + 1] -> body.MustodontStat.MakeTime){
						p = iPlayers[i];
						iPlayers[i] = iPlayers[i + 1];
						iPlayers[i + 1] = p;
						flag = 1;
					}
				}
			}
		}
	}
	else {
		while(flag){
			flag = 0;
			for(i = 0; i < iNumPlayers - 1; i ++){
				if(iPlayerRatings[i] < iPlayerRatings[i + 1]){
					p = iPlayers[i];
					tmp = iPlayerRatings[i];

					iPlayers[i] = iPlayers[i + 1];
					iPlayerRatings[i] = iPlayerRatings[i + 1];

					iPlayers[i + 1] = p;
					iPlayerRatings[i + 1] = tmp;

					flag = 1;
				}
			}
		}
	}
}

/*
void iSortPlayers(int mode)
{
	int i,flag = 1,num;

	PlayerData* p = players_list.first();
	num = players_list.ListSize;

	if(iPlayers) delete iPlayers;
	iPlayers = new PlayerData*[num];

	iNumPlayers = 0;
	for(i = 0; i < num; i ++){
		if(p -> name){
			iPlayers[iNumPlayers] = p;
			iNumPlayers ++;
		}
		p = (PlayerData*)p -> next;
	}

	while(flag){
		flag = 0;
		for(i = 0; i < iNumPlayers - 1; i ++){
			if(iCurMultiGame == iMP_VAN_WAR || iCurMultiGame == iMP_HUNTAGE){
				if(iPlayers[i] -> body.kills - iPlayers[i] -> body.deaths < iPlayers[i + 1] -> body.kills - iPlayers[i + 1] -> body.deaths){
					p = iPlayers[i];
					iPlayers[i] = iPlayers[i + 1];
					iPlayers[i + 1] = p;
					flag = 1;
				}
			}
			if(iCurMultiGame == iMP_MECHOSOMA){
				if(iPlayers[i] -> body.MechosomaStat.ItemCount1 + iPlayers[i] -> body.MechosomaStat.ItemCount2 < iPlayers[i + 1] -> body.MechosomaStat.ItemCount1 + iPlayers[i + 1] -> body.MechosomaStat.ItemCount2){
					p = iPlayers[i];
					iPlayers[i] = iPlayers[i + 1];
					iPlayers[i + 1] = p;
					flag = 1;
				}
			}
			if(iCurMultiGame == iMP_PASSEMBLOSS){
				if(!mode){
					if(!iPlayers[i] -> body.PassemblossStat.TotalTime && iPlayers[i + 1] -> body.PassemblossStat.TotalTime){
						p = iPlayers[i];
						iPlayers[i] = iPlayers[i + 1];
						iPlayers[i + 1] = p;
						flag = 1;
					}
					if(iPlayers[i] -> body.PassemblossStat.TotalTime && iPlayers[i + 1] -> body.PassemblossStat.TotalTime && iPlayers[i] -> body.PassemblossStat.TotalTime > iPlayers[i + 1] -> body.PassemblossStat.TotalTime){
						p = iPlayers[i];
						iPlayers[i] = iPlayers[i + 1];
						iPlayers[i + 1] = p;
						flag = 1;
					}
				}
				else {
					if(iPlayers[i] -> body.PassemblossStat.CheckpointLighting < iPlayers[i + 1] -> body.PassemblossStat.CheckpointLighting){
						p = iPlayers[i];
						iPlayers[i] = iPlayers[i + 1];
						iPlayers[i + 1] = p;
						flag = 1;
					}
				}
			}
			if(iCurMultiGame == iMP_MUSTODONT){
				if(!iPlayers[i] -> body.MustodontStat.MakeTime && iPlayers[i + 1] -> body.MustodontStat.MakeTime){
					p = iPlayers[i];
					iPlayers[i] = iPlayers[i + 1];
					iPlayers[i + 1] = p;
					flag = 1;
				}
				if(iPlayers[i] -> body.MustodontStat.MakeTime && iPlayers[i + 1] -> body.MustodontStat.MakeTime && iPlayers[i] -> body.MustodontStat.MakeTime > iPlayers[i + 1] -> body.MustodontStat.MakeTime){
					p = iPlayers[i];
					iPlayers[i] = iPlayers[i + 1];
					iPlayers[i + 1] = p;
					flag = 1;
				}
			}
		}
	}
}
*/

void iPreparePlayerResults(int id)
{
	int i,num,num0,num1,num2;
	XBuffer XBuf;
	PlayerData* p = iPlayers[id];

	if(iCurMultiGame == iMP_VAN_WAR){
		XBuf.init();
		XBuf <= p -> body.kills;
		iResults[0] -> set_data("",iSTR_MP_Kills,XBuf.address());

		XBuf.init();
		XBuf <= p -> body.deaths;
		iResults[1] -> set_data("",iSTR_MP_Deaths,XBuf.address());
/*
		XBuf.init();
		if(p -> body.VanVarStat.MinLiveTime)
			XBuf <= p -> body.VanVarStat.MinLiveTime;
		else
			XBuf < "-";
		iResults[2] -> set_data("",iSTR_MP_DeathTimeMin,XBuf.address());

		XBuf.init();
		if(p -> body.VanVarStat.MaxLiveTime)
			XBuf <= p -> body.VanVarStat.MaxLiveTime;
		else
			XBuf < "-";
		iResults[3] -> set_data("",iSTR_MP_DeathTimeMax,XBuf.address());
*/
		num = p -> body.VanVarStat.KillFreq;
		num0 = num / 3600;
		num1 = (num - num0 * 3600) / 60;
		num2 = num % 60;

		XBuf.init();
		if(num)
			XBuf <= num0 < ":" <= num1 < ":" <= num2;
		else
			XBuf < "-";
		iResults[2] -> set_data("",iSTR_MP_Kills_rate,XBuf.address());

		num = p -> body.VanVarStat.DeathFreq;
		num0 = num / 3600;
		num1 = (num - num0 * 3600) / 60;
		num2 = num % 60;

		XBuf.init();
		if(p -> body.VanVarStat.DeathFreq)
			XBuf <= num0 < ":" <= num1 < ":" <= num2;
		else
			XBuf < "-";
		iResults[3] -> set_data("",iSTR_MP_Deaths_rate,XBuf.address());

		for(i = 0; i < 4; i ++)
			iResults[i] -> redraw();

		for(i = 4; i < iMP_MAX_RESULT; i ++){
			iResults[i] -> set_data("","","");
			iResults[i] -> redraw();
		}
	}
	if(iCurMultiGame == iMP_MECHOSOMA){
		XBuf.init();
		XBuf <= p -> body.kills;
		iResults[0] -> set_data("",iSTR_MP_Kills,XBuf.address());

		XBuf.init();
		XBuf <= p -> body.deaths;
		iResults[1] -> set_data("",iSTR_MP_Deaths,XBuf.address());

		XBuf.init();
		XBuf <= p -> body.MechosomaStat.ItemCount1;
		iResults[2] -> set_data("",iSTR_MP_Ware01_Delivery,XBuf.address());

		XBuf.init();
		XBuf <= p -> body.MechosomaStat.ItemCount2;
		iResults[3] -> set_data("",iSTR_MP_Ware02_Delivery,XBuf.address());

/*
		XBuf.init();
		if(p -> body.MechosomaStat.MinTransitTime)
			XBuf <= p -> body.MechosomaStat.MinTransitTime;
		else
			XBuf < "-";
		iResults[4] -> set_data("",iSTR_MP_Min_delivery_time,XBuf.address());
*/

		num = p -> body.MechosomaStat.MaxTransitTime;
		num0 = num / 3600;
		num1 = (num - num0 * 3600) / 60;
		num2 = num % 60;

		XBuf.init();
		if(p -> body.MechosomaStat.MaxTransitTime)
			XBuf <= num0 < ":" <= num1 < ":" <= num2;
		else
			XBuf < "-";
		iResults[4] -> set_data("",iSTR_MP_Max_delivery_time,XBuf.address());

		XBuf.init();
		XBuf <= p -> body.MechosomaStat.SneakCount;
		iResults[5] -> set_data("",iSTR_MP_StolenWares,XBuf.address());

		XBuf.init();
		XBuf <= p -> body.MechosomaStat.LostCount;
		iResults[6] -> set_data("",iSTR_MP_Wares_lost,XBuf.address());

		for(i = 0; i < 7; i ++)
			iResults[i] -> redraw();

		for(i = 7; i < iMP_MAX_RESULT; i ++){
			iResults[i] -> set_data("","","");
			iResults[i] -> redraw();
		}
	}
	if(iCurMultiGame == iMP_PASSEMBLOSS){
		XBuf.init();
		XBuf <= p -> body.kills;
		iResults[0] -> set_data("",iSTR_MP_Kills,XBuf.address());

		XBuf.init();
		XBuf <= p -> body.deaths;
		iResults[1] -> set_data("",iSTR_MP_Deaths,XBuf.address());

		XBuf.init();
		XBuf <= p -> body.PassemblossStat.CheckpointLighting;
		iResults[2] -> set_data("",iSTR_Checkpoints_Number,XBuf.address());

		num = p -> body.PassemblossStat.TotalTime;
		num0 = num / 3600;
		num1 = (num - num0 * 3600) / 60;
		num2 = num % 60;

		XBuf.init();
		if(p -> body.PassemblossStat.TotalTime)
			XBuf <= num0 < ":" <= num1 < ":" <= num2;
		else
			XBuf < "-";
		iResults[3] -> set_data("",iSTR_MP_Total_time,XBuf.address());

/*
		XBuf.init();
		if(p -> body.PassemblossStat.MinTime)
			XBuf <= p -> body.PassemblossStat.MinTime;
		else
			XBuf < "-";
		iResults[3] -> set_data("",iSTR_MP_Min_checkpoint_time,XBuf.address());

		XBuf.init();
		if(p -> body.PassemblossStat.MaxTime)
			XBuf <= p -> body.PassemblossStat.MaxTime;
		else
			XBuf < "-";
		iResults[4] -> set_data("",iSTR_MP_Max_checkpoint_time,XBuf.address());
*/

		for(i = 0; i < 4; i ++)
			iResults[i] -> redraw();

		for(i = 4; i < iMP_MAX_RESULT; i ++){
			iResults[i] -> set_data("","","");
			iResults[i] -> redraw();
		}
	}
	if(iCurMultiGame == iMP_HUNTAGE){
		XBuf.init();
		XBuf <= p -> body.kills;
		iResults[0] -> set_data("",iSTR_MP_Kills,XBuf.address());

		XBuf.init();
		XBuf <= p -> body.deaths;
		iResults[1] -> set_data("",iSTR_MP_Deaths,XBuf.address());

		for(i = 0; i < 2; i ++)
			iResults[i] -> redraw();

		for(i = 2; i < iMP_MAX_RESULT; i ++){
			iResults[i] -> set_data("","","");
			iResults[i] -> redraw();
		}
	}
	if(iCurMultiGame == iMP_MUSTODONT){
		XBuf.init();
		XBuf <= p -> body.kills;
		iResults[0] -> set_data("",iSTR_MP_Kills,XBuf.address());

		XBuf.init();
		XBuf <= p -> body.deaths;
		iResults[1] -> set_data("",iSTR_MP_Deaths,XBuf.address());

		XBuf.init();
		if(p -> body.MustodontStat.PartTime1)
			XBuf <= p -> body.MustodontStat.PartTime1;
		else
			XBuf < "-";
		iResults[2] -> set_data("",iSTR_MP_1st_part_delivery,XBuf.address());

		XBuf.init();
		if(p -> body.MustodontStat.PartTime2)
			XBuf <= p -> body.MustodontStat.PartTime2;
		else
			XBuf < "-";
		iResults[3] -> set_data("",iSTR_MP_2nd_part_delivery,XBuf.address());

		XBuf.init();
		if(p -> body.MustodontStat.BodyTime)
			XBuf <= p -> body.MustodontStat.BodyTime;
		else
			XBuf < "-";
		iResults[4] -> set_data("",iSTR_MP_Mechos_frame,XBuf.address());

		XBuf.init();
		if(p -> body.MustodontStat.MakeTime)
			XBuf <= p -> body.MustodontStat.MakeTime;
		else
			XBuf < "-";
		iResults[5] -> set_data("",iSTR_MP_Mechos_assembled_in,XBuf.address());

		for(i = 0; i < 6; i ++)
			iResults[i] -> redraw();

		for(i = 6; i < iMP_MAX_RESULT; i ++){
			iResults[i] -> set_data("","","");
			iResults[i] -> redraw();
		}
	}
}

const char* STR_NONE1 = "NONE";
const char STR_NONE2[] = {(char)0x8D, (char)0x85, (char)0x92}; //cp866 - НЕТ

const char* STR_JOYSTICK_KEY_NAME[] = {
	"jbutton_1",
	"jbutton_2",
	"jbutton_3",
	"jbutton_4",
	"jbutton_5",
	"jbutton_6",
	"jbutton_7",
	"jbutton_8",
	"jbutton_9",
	"jbutton_10",
	"jbutton_11",
	"jbutton_12",
	"jbutton_13",
	"jbutton_14",
	"jbutton_15",
	"jbutton_16",
	"jbutton_17",
	"jbutton_18",
	"jbutton_19",
	"jbutton_20",
	"jbutton_21"
};

const char* get_joystick_hat_name(int key) {
	switch(key) {
		case SDL_HAT_CENTERED:
			return "jhat_centered";
		case SDL_HAT_UP:
			return "jhat_up";
		case SDL_HAT_RIGHT:
			return "jhat_right";
		case SDL_HAT_DOWN:
			return "jhat_down";
		case SDL_HAT_LEFT:
			return "jhat_left";
		case SDL_HAT_RIGHTUP:
			return "jhat_rightup";
		case SDL_HAT_RIGHTDOWN:
			return "jhat_rightdown";
		case SDL_HAT_LEFTUP:
			return "jhat_leftup";
		case SDL_HAT_LEFTDOWN:
			return "jhat_leftdown";
			
	}
	return "jhat_unknow";
}

const char* iGetKeyNameText(int vkey,int lang)
{
	//std::cout<<"iGetKeyNameText:"<<vkey<<" lang:"<<lang<<std::endl;
	/*char* ret = NULL;
	if(vkey >= 0 && vkey < 256){
		ret = (lang) ? iKeyNamesText2[vkey] : iKeyNamesText1[vkey];
	}
	if(vkey>=256&&vkey-256<256)
		ret = iKeyNamesSDL[vkey-256];
	return ret;*/
	if (vkey == 0) {
		if (lang) {
			return STR_NONE2;
		} else {
			return STR_NONE1;
		}
	}
	if (vkey & SDLK_JOYSTICK_BUTTON_MASK) {
		if ((vkey ^ SDLK_JOYSTICK_BUTTON_MASK) < 20)
			return STR_JOYSTICK_KEY_NAME[vkey ^ SDLK_JOYSTICK_BUTTON_MASK];
		else
			return "jbutton_unknow";
	} else if (vkey & SDLK_GAMECONTROLLER_BUTTON_MASK) {
		return SDL_GameControllerGetStringForButton( (SDL_GameControllerButton)(vkey ^ SDLK_GAMECONTROLLER_BUTTON_MASK));
	} else if (vkey & SDLK_JOYSTICK_HAT_MASK) {
		return get_joystick_hat_name( (vkey ^ SDLK_JOYSTICK_HAT_MASK) % 10 );
	} else if (vkey & SDLK_SCANCODE_MASK) {
		return SDL_GetKeyName(vkey);
	} else {
		return SDL_GetKeyName(SDL_GetKeyFromScancode((SDL_Scancode)vkey));
	}
}

const char* iGetJoyBtnNameText(int vkey,int lang)
{
	const char* ret;
	if(vkey & VK_BUTTON){
		if(vkey >= VK_BUTTON_1 && vkey <= VK_BUTTON_32) {
			ret = (lang)
				? iJoystickButtons2[vkey - VK_BUTTON_1]
				: iJoystickButtons1[vkey - VK_BUTTON_1];
			return ret;
		}
		else
			return NULL; //WARNING NEED VIEW!!!
	}
	if(vkey & VK_STICK_SWITCH){
		if(vkey >= VK_STICK_SWITCH_1 && vkey <= VK_STICK_SWITCH_9) {
			ret = (lang)
				? iJoystickStickSwitch2[vkey - VK_STICK_SWITCH_1]
				: iJoystickStickSwitch1[vkey - VK_STICK_SWITCH_1];
			return ret;
		}
		else
			return NULL; //WARNING NEED VIEW!!! 
	}
	return NULL; //WARNING NEED VIEW!!!
}

void iPrepareHallOfFame(void)
{
	int i,num;
	RatingData* p;
	XBuffer XPlace,XNum;
	TopList top_list(iCurHall);

	XNum.digits = 3;

	num = top_list.size();
	if(num > iMP_MAX_HALL_RESULT) num = iMP_MAX_HALL_RESULT;
	p = top_list.first();

	for(i = 0; i < num; i ++){
		XPlace.init();
		XPlace <= i + 1 < ".";

		XNum.init();
		XNum <= (int)round(p -> rating);

		iHallPlaces[i] -> set_data(XPlace.address(),p -> name,XNum.address());
		iHallPlaces[i] -> redraw();

		p = p -> next;
	}
	for(i = num; i < iMP_MAX_HALL_RESULT; i ++){
		iHallPlaces[i] -> set_data("","","");
		iHallPlaces[i] -> redraw();
	}
}

int iGetEscaveTime(void)
{
	int value = iGetMultiGameParameter(iCurMultiGame,iMP_IN_ESCAVE_TIME);
	return (value + 1) * 1200;
}
