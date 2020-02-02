/* ---------------------------- INCLUDE SECTION ----------------------------- */

#include "../global.h"
#include "../sound/hsound.h"


#include "../network.h"
#include "../xjoystick.h"

#include "ivmap.h"
#include "iworld.h"
#include "i_str.h"

#include "hfont.h"
#include "iscreen.h"
#include "iscript.h"
#include "i_mem.h"
#include "ikeys.h"

#include "controls.h"

#include "ext_evnt.h"

#include "../actint/item_api.h"
#include "../actint/a_consts.h"
#include "../actint/actint.h"

#include "../actint/mlconsts.h"
#include "../actint/mlstruct.h"

#include "../actint/acsconst.h"
#include "../actint/aci_evnt.h"
#include "../actint/aci_scr.h"

#ifndef _WIN32
#include <arpa/inet.h> // ntohl() FIXME: remove
#endif

#include "../sound/hmusic.h"
#include "../sound/hsound.h"


#ifdef _NT
#define EXTQUIT_VERIFY	if(XWaitWhileInactive()) ErrH.Exit();
#else
#define EXTQUIT_VERIFY
#endif

//#define CLOCK() 	(SDL_GetTicks())*18/1000)

/* ----------------------------- EXTERN SECTION ----------------------------- */

#ifdef _ACI_ESCAVE_DEBUG_
extern int aci_dgMoodNext;
#endif

extern iStringElement** iControlsStr;

extern int MusicON;
extern int SoundVolumePanning;
extern int KeepON;

extern int RecorderMode;

extern int iCurMultiGame;
extern int SoundVolumeCD;

extern int MuteLog;
extern int EffectInUsePriory,EffectInUse;

extern iScreenDispatcher* iScrDisp;

extern int aciAutoRun;

extern int aciML_ToolzerON;
extern int aciLoadLog;
extern int aciBreathFlag;

extern int Dead;

extern int ExclusiveLog,DIBLog;
extern int actintLowResFlag;
extern int actintActiveFlag;

extern int iEvLineID;
extern int acsScreenID;

extern int actIntLog;
extern char* aci_iScreenID;
extern unsigned char* actIntPal;
extern actIntDispatcher* aScrDisp;

extern int iScreenLastInput;

extern int iScreenChat;
extern int iChatON;

/* --------------------------- PROTOTYPE SECTION ---------------------------- */

int iGetEscaveTime(void);

void aciVMapPrepare(void);

void iPrepareHallOfFame(void);
void iBlockJoystickOption(int mode);
void iInitProxyOptions(void);
//#define iMOVE_MOUSE_OBJECTS

#ifdef iMOVE_MOUSE_OBJECTS
iScreenObject* iGetScreenObj(int x,int y);
void iShowObjCoords(iScreenObject* p);
void iMoveMouseObj(int dx,int dy);
void iWriteScreenSummary(void);
#endif

iListElement* iGetObject(const char* scr,const char* obj);

void aciAutoSave(void);

void shotFlush(void);

void iGetIP(void);

void iInitStrings(void);
void iInitKeepOptions(void);
void iKeepCleanUp(void);
int GetKeepSpace(void);
void KeepCleanUp(void);

void aLoadFonts32(void);

void iPrepareResults(void);
void iPreparePlayerResults(int id);
void iCreateServer(void);

void iChatInit(void);
void iChatQuant(int flush = 0);
void iChatFinit(void);
void iChatKeyQuant(SDL_Event *k);
void iChatMouseQuant(int x,int y,int bt);

void iSoundQuant(int value = 0);

void iPrepareSaveNames(void);
void iInitServersList(int inet = 0);
void iUpdateServersList(int inet = 0);
void iServersListUp(int inet = 0);
void iServersListDown(int inet = 0);
void iConnect2Server(void);
void iInitControlKey(int id,int num);
void iDeleteControl(int vkey,int id);
void iDeleteControlKey(int id,int num);
void iUpdateMultiGameName(void);
void iSetMultiGameParameters(void);
void iGetMultiGameParameters(void);
void iLockMultiGameParameters(void);
void iUnlockMultiGameParameters(void);
ServerFindChain* iGetCurServer(void);

void SetSoundVolume(int);
void EffectsOff(void);

int acsQuant(void);
void acsHandleExtEvent(int code,int data0 = 0,int data1 = 0,int data2 = 0);

void ipal_init(unsigned char* p);
void ipal_iter(int r);

void iKeyTrap(int k);

void iInitMultiGames(void);

void aci_setMatrixBorder(void);
void aci_setScMatrixBorder(void);

void aciSet_aMouse(void);
void aciSet_iMouse(void);

void aciShowLocationPicture(void);
void aciShowLocation(void);
void aciHideLocation(void);

void aciCloseGate(void);
void aciOpenGate(void);

void aciSwapMatrices(void);
void aciCancelMatrix(void);
void aciShowScMatrix(void);

#ifdef _DEBUG
void aciChangeMouseItem(void);
void iMapShot(void);
#endif

void iShopForcedExit(void);
void iLockShop(void);
void iUnlockShop(void);

void put_fon(int x,int y,int sx,int sy,unsigned char* buf);
void get_fon(int x,int y,int sx,int sy,unsigned char* buf);

void loadMouseFrames(void);

void scale_buf(int sx,int sy,int sc,unsigned char* p,unsigned char* p1);
void put_buf(int x,int y,int sx,int sy,unsigned char* buf,unsigned char* mask,int lev,int hide_mode);
void put_level(int x,int y,int sx,int sy,int lev);
void get_buf(int x,int y,int sx,int sy,uchar* buf);
void get_col_buf(int x,int y,int sx,int sy,uchar* buf);
void iPutStr(int x,int y,int fnt,unsigned char* str,int mode,int scale,int space,int level,int hide_mode);
int iStrLen(char* p,int f,int space);
void ivMapPrepare(void);
void InitScreenFonts(void);
void LoadingMessage(int flush = 0);

void put_map(int x,int y,int sx,int sy);

void i_get_scrfon(int x,int y,int sx,int sy,unsigned char* buf);
void i_put_scrfon(int x,int y,int sx,int sy,unsigned char* buf);
void i_mem_putspr(int x,int y,int sx,int sy,unsigned char* buf);
void mem_putspr(int x,int y,int sx,int sy,int bsx,int bsy,unsigned char* src,unsigned char* dest);

void iInit(void);
void map_init(void);
void init_hfonts(void);
void free_hfonts(void);
void InitScreenFonts(void);

void i_preExtQuant(void);
void i_postExtQuant(void);

void RegisterValues(void);
void iPrepareOptions(void);
void iregRender(int x0,int y0,int x1,int y1);

void MS_init(void);

void recorder_synchro(unsigned int frame);
void KeyCenter(SDL_Event *key);

void i_getpal(unsigned char* p);

void aci_iScreenQuant(void);

void i_slake_pal(unsigned char* p,int delta);
void i_evince_pal(unsigned char* p,int delta);

void iSaveData(void);
void iLoadData(void);
void iSaveControls(void);
void iLoadControls(void);

void aciSaveData(void);
void aciLoadData(void);

void uvsQuant(void);

void iMS_LeftPress(int, int, int);
void iMS_LeftUnpress(int, int, int);
void iMS_RightPress(int, int, int);
void iMS_RightUnpress(int, int, int);
void iMS_Movement(int, int, int);

void aMS_LeftPress(int, int, int);
void aMS_LeftUnpress(int, int, int);
void aMS_RightPress(int, int, int);
void aMS_RightUnpress(int, int, int);
void aMS_Movement(int, int, int);

#ifdef _DEBUG
void aciResizeItem(double delta);
#endif

void iSecondInit(void);

/* --------------------------- DEFINITION SECTION --------------------------- */

#define _SAVE_SCREEN_

const int  iFRAME_CYCLE = 10;

HFont** HFntTable = NULL;
iScreenFont** iScrFontTable;
int iNumFonts = 1;
uchar* iscrPal;

const char* CurServerName = "Server";
const char* CurPlayerName = "Vanger";

int iMouseX = 0;
int iMouseY = 0;
int iMousePrevX = 0;
int iMousePrevY = 0;

int iForcedExitFlag = 0;
int iTheEND = 0;

int iFrameCount = 0;
int iFrameFlag = 0;

int aciCurCredits07 = 0;

int iScreenLog = 0;
int iFirstInit = 0;
int iScreenActive = 0;

int iMultiFlag = 0;

short MouseSizeX = 0;
short MouseSizeY = 0;
short MouseSize = 0;
unsigned char* MouseFrames;

int MouseSize2 = 0;

short aMouseSizeX = 0;
short aMouseSizeY = 0;
short aMouseSize = 0;
unsigned char* aMouseFrames;

short iMouseSizeX = 0;
short iMouseSizeY = 0;
short iMouseSize = 0;
unsigned char* iMouseFrames;

int iStartupLog = 0;
int iPause	= 0;

int iEndGameFlag = 0;

int iCurHall = 0;
int iEscaveTimer = 0;

#ifdef _DEBUG
int iTimerLog = 1;
int iBoundsLog = 0;

int iEscaveShotCount = 0;
int iEscaveShopShotCount = 0;
int iMainMenuShotCount = 0;
#endif

int iAbortGameFlag = 0;
int iAbortGameMode = 1;

int iMouseLPressFlag = 0;
int iMouseRPressFlag = 0;

#ifdef RUSSIAN_VERSION
int iRussian = 1;
#else
int iRussian = 0;
#endif

int iSlotNumber = 0;
int iCDTrackLog = 0;

int iFirstServerSearch = 0;
int iCurServer = 0;
ServerFindChain* iFirstServerPtr;

XBuffer* iResBuf;
const char* iVideoPath = "resource/video/";
const char* iVideoPathDefault = "resource/video/";

const char* iSTR_WORLD_NONE_CHAR = iSTR_WORLD_NONE_CHAR1;
const char* iSTR_WORLD_FOSTRAL_CHAR = iSTR_WORLD_FOSTRAL_CHAR1;
const char* iSTR_WORLD_GLORX_CHAR = iSTR_WORLD_GLORX_CHAR1;
const char* iSTR_WORLD_NECROSS_CHAR = iSTR_WORLD_NECROSS_CHAR1;
const char* iSTR_WORLD_XPLO_CHAR = iSTR_WORLD_XPLO_CHAR1;
const char* iSTR_WORLD_KHOX_CHAR = iSTR_WORLD_KHOX_CHAR1;
const char* iSTR_WORLD_BOOZEENA_CHAR = iSTR_WORLD_BOOZEENA_CHAR1;
const char* iSTR_WORLD_WEEXOW_CHAR = iSTR_WORLD_WEEXOW_CHAR1;
const char* iSTR_WORLD_HMOK_CHAR = iSTR_WORLD_HMOK_CHAR1;
const char* iSTR_WORLD_THREALL_CHAR = iSTR_WORLD_THREALL_CHAR1;
const char* iSTR_WORLD_ARK_A_ZNOY_CHAR = iSTR_WORLD_ARK_A_ZNOY_CHAR1;
const char* iSTR_COLOR_GREEN_CHAR = iSTR_COLOR_GREEN_CHAR1;
const char* iSTR_COLOR_ORANGE_CHAR = iSTR_COLOR_ORANGE_CHAR1;
const char* iSTR_COLOR_BLUE_CHAR = iSTR_COLOR_BLUE_CHAR1;
const char* iSTR_COLOR_YELLOW_CHAR = iSTR_COLOR_YELLOW_CHAR1;
const char* iSTR_MUTE_ALL = iSTR_MUTE_ALL1;
const char* iSTR_CLEAR_LOG = iSTR_CLEAR_LOG1;
const char* iSTR_BACKGROUND = iSTR_BACKGROUND1;
const char* iSTR_LEAVE_CHAT = iSTR_LEAVE_CHAT1;
const char* iSTR_All = iSTR_All1;
const char* iSTR_Yellow = iSTR_Yellow1;
const char* iSTR_Orange = iSTR_Orange1;
const char* iSTR_Blue = iSTR_Blue1;
const char* iSTR_Green = iSTR_Green1;
const char* iSTR_CleanUp = iSTR_CleanUp1;
const char* iSTR_SpaceInUse = iSTR_SpaceInUse1;
const char* iSTR_MBytes = iSTR_MBytes1;
const char* iSTR_MP_Kills = iSTR_MP_Kills1;
const char* iSTR_MP_Deaths = iSTR_MP_Deaths1;
const char* iSTR_MP_DeathTimeMin = iSTR_MP_DeathTimeMin1;
const char* iSTR_MP_DeathTimeMax = iSTR_MP_DeathTimeMax1;
const char* iSTR_MP_Kills_rate = iSTR_MP_Kills_rate1;
const char* iSTR_MP_Deaths_rate = iSTR_MP_Deaths_rate1;
const char* iSTR_MP_Ware01_Delivery = iSTR_MP_Ware01_Delivery1;
const char* iSTR_MP_Ware02_Delivery = iSTR_MP_Ware02_Delivery1;
const char* iSTR_MP_Min_delivery_time = iSTR_MP_Min_delivery_time1;
const char* iSTR_MP_Max_delivery_time = iSTR_MP_Max_delivery_time1;
const char* iSTR_MP_StolenWares = iSTR_MP_StolenWares1;
const char* iSTR_MP_Wares_lost = iSTR_MP_Wares_lost1;
const char* iSTR_MP_Total_time = iSTR_MP_Total_time1;
const char* iSTR_MP_Min_checkpoint_time = iSTR_MP_Min_checkpoint_time1;
const char* iSTR_MP_Max_checkpoint_time = iSTR_MP_Max_checkpoint_time1;
const char* iSTR_MP_1st_part_delivery = iSTR_MP_1st_part_delivery1;
const char* iSTR_MP_2nd_part_delivery = iSTR_MP_2nd_part_delivery1;
const char* iSTR_MP_Mechos_frame = iSTR_MP_Mechos_frame1;
const char* iSTR_MP_Mechos_assembled_in = iSTR_MP_Mechos_assembled_in1;
const char* iSTR_NONE = iSTR_NONE1;
const char* iSTR_EMPTY_SLOT = iSTR_EMPTY_SLOT1;
const char* iSTR_Checkpoints_Number = iSTR_Checkpoints_Number1;
const char* iSTR_DefaultPassword = iSTR_DefaultPassword1;

#ifdef iMOVE_MOUSE_OBJECTS
iScreenObject* iMovedObj = NULL;
#endif

int iProxyTerrFlag = 0;
int iProxyServerStrTerr = -1;
int iProxyServerTerr = -1;
int iProxyPortStrTerr = -1;
int iProxyPortTerr = -1;

int iProxyUsage = 0;
char* iProxyServer = NULL;
int iProxyPort = 1080;
int iServerPort = 2197;

void init_hfonts(void)
{
	int i;
	XBuffer XBuf;
	if(!HFntTable){
		HFntTable = new HFont*[iNumFonts];
		for(i = 0; i < iNumFonts; i ++){
			XBuf.init();
			XBuf < "resource/iscreen/fonts/hfont";
			if(i < 10)  XBuf < "0";
			XBuf <= i < ".fnh";
			HFntTable[i] = new HFont;
			HFntTable[i] -> load(XBuf.GetBuf());
		}
	}
}

void free_hfonts(void)
{
	int i;
	for(i = 0; i < iNumFonts; i ++)
		delete HFntTable[i];
	delete[] HFntTable;
	HFntTable = NULL;
}

#define I_TIME_DELTA	1

void iInit(void)
{
	if(!iFirstInit){
		loadMouseFrames();
		InitScreenFonts();
		iResBuf = new XBuffer(2048);
		aLoadFonts32();

		iInitStrings();
	}

#ifdef _BINARY_SCRIPT_
	if(!iFirstInit){
		if(iRussian)
			ParseScript("resource/iscreen/oftr2.scb");
		else
			ParseScript("resource/iscreen/oftr.scb");
	}
#else
	if(!iFirstInit){
		if(iRussian)
			ParseScript("iscreen/oftr2.scr","resource/iscreen/oftr2.scb");
		else
			ParseScript("iscreen/oftr.scr","resource/iscreen/oftr.scb");
	}
#endif

	if(iFirstInit){
		iScrDisp -> flags &= ~SD_EXIT;
		iScrDisp -> ret_val = 0;
	}

	ivMapPrepare();
#ifndef _ACI_SKIP_MAINMENU_
	map_init();
#endif

	XStream ff("resource/iscreen/mainmenu.pal",XS_IN);
	iscrPal = new uchar[768];
	ff.read(iscrPal,768);

	ipal_init(iscrPal);

	iScreenLog = 1;
}

void set_mouse(void)
{
	XGR_MouseInit(XGR_MAXX/2,XGR_MAXY/2,MouseSizeX,MouseSizeY,MouseSize,MouseFrames);

	XGR_MouseSetSpot(MouseSizeX/2,0);
	XGR_MouseSetSeq(0,0,XGM_NONE,0);

	iMouseLPressFlag = 0;
	iMouseRPressFlag = 0;
}

//TEST stalkerg
void iPreInitFirst() {
	if (!iFirstInit) {
		aScrDisp->i_init();
	}
	iSecondInit();
	
	iScreen* p;
	if(actIntLog){
		p = (iScreen*)iScrDisp -> get_object(aci_iScreenID);
		if(!p) ErrH.Abort("Bad aci_iScreenID...");
		iScrDisp -> curScr = p;
		if(NetworkON){
			iEscaveTimer = -1;
			iEndGameFlag = 0;
		}
	}
	else {
#ifndef _ACI_SKIP_MAINMENU_
		iEvLineID = 0;
		iScrDisp -> init_texts();
		if(iMultiFlag){
			iScrDisp -> curScr = (iScreen*)iScrDisp -> get_object("MultiGame Results screen");
			iPrepareResults();
			iMultiFlag = 0;
			iEndGameFlag = 1;
		}
		else
			iScrDisp -> curScr = (iScreen*)iScrDisp -> get_object(iScrDisp -> t_scrID);
#endif
	}
	iInitControls();

	iScreenOffs = iScrDisp -> curScr -> ScreenOffs;

#ifdef _ACI_SKIP_MAINMENU_
	if(actIntLog){
#endif
	init_hfonts();
	if(!actIntLog)
		iScrDisp -> prepare();
	iScrDisp -> curScr -> prepare();
	iScrDisp -> alloc_mem();

#ifdef _ACI_SKIP_MAINMENU_
	}
#endif

	if(!iFirstInit){
		RegisterValues();
		iPrepareOptions();
	}
}

void iQuantFirst(void)
{
	iScreen* p;
	if(actIntLog){
		p = (iScreen*)iScrDisp -> get_object(aci_iScreenID);
		if(!p) ErrH.Abort("Bad aci_iScreenID...");
		iScrDisp -> curScr = p;
		if(NetworkON){
			iEscaveTimer = -1;
			iEndGameFlag = 0;
		}
	}
	else {
#ifndef _ACI_SKIP_MAINMENU_
		iEvLineID = 0;
		iScrDisp -> init_texts();
		if(iMultiFlag){
			iScrDisp -> curScr = (iScreen*)iScrDisp -> get_object("MultiGame Results screen");
			iPrepareResults();
			iMultiFlag = 0;
			iEndGameFlag = 1;
		}
		else
			iScrDisp -> curScr = (iScreen*)iScrDisp -> get_object(iScrDisp -> t_scrID);
#endif
	}
	iInitControls();

	iScreenOffs = iScrDisp -> curScr -> ScreenOffs;

#ifdef _ACI_SKIP_MAINMENU_
	if(actIntLog){
#endif
	init_hfonts();
	if(!actIntLog)
		iScrDisp -> prepare();
	iScrDisp -> curScr -> prepare();
	iScrDisp -> alloc_mem();

#ifdef _ACI_SKIP_MAINMENU_
	}
#endif

	if(!iFirstInit){
		RegisterValues();
		iPrepareOptions();
#ifndef _ACI_SKIP_MAINMENU_
		if(iRussian)
#ifdef _ISCREEN_GERMAN_
			iSetOptionValueCHR(iPLAYER_NAME2,"Vanger");
#else
			iSetOptionValueCHR(iPLAYER_NAME2,"‚ ­ЈҐа");
#endif
		else
			iSetOptionValueCHR(iPLAYER_NAME2,"Vanger");
		iSetOptionValueCHR(iPLAYER_PASSWORD,iSTR_DefaultPassword);
		if(iRussian){
#ifdef _ISCREEN_GERMAN_
            iSetOptionValueCHR(iHOST_NAME,"vangers.pp.ru");
//			iSetOptionValueCHR(iHOST_NAME,"www.imagicgames.de");
#else
            iSetOptionValueCHR(iHOST_NAME,"vangers.pp.ru");
#endif
		}
		else
            iSetOptionValueCHR(iHOST_NAME,"vangers.pp.ru");
//            iSetOptionValueCHR(iHOST_NAME,"www.imagicgames.com");
		iSetOptionValueCHR(iSERVER_NAME,iSTR_NONE);
		iSetOptionValueCHR(iPROXY_SERVER,"192.1.1.1");
		iSetOptionValueCHR(iPROXY_PORT,"1080");
		iSetOptionValueCHR(iSERVER_PORT,"2197");
		iGetIP();
#endif
		iInitMultiGames();

		CurServerName = iGetOptionValueCHR(iSERVER_NAME);
		CurPlayerName = iGetOptionValueCHR(iPLAYER_NAME2);
	}
	aciSet_iMouse();

	KBD_init();
	MS_init();
	XGR_MouseHide();

#ifdef _ACI_SKIP_MAINMENU_
	if(actIntLog){
#endif

	if(!actIntLog){
		iLoadData();
	}

	iStartupLog = 1;
	iScrDisp -> redraw();
	put_map(iScreenOffs,0,I_RES_X,I_RES_Y);
	iStartupLog = 0;

#ifdef _ACI_SKIP_MAINMENU_
	}
#endif
	if(!iFirstInit){
		iLoadControls();
		iInitControlObjects();
	}

	iFirstInit = 1;
	iScreenActive = 1;
}

void iQuantPrepare(void)
{
	if(actIntLog){
		aciML_D -> redraw_items();
		iForcedExitFlag = 0;
	}
	else {
#ifndef _ACI_SKIP_MAINMENU_
		iLoadData();
#endif
	}

	if(iScrDisp -> ActiveEv){
		iScrDisp -> ActiveEv -> flags &= ~EV_ACTIVE;
		iScrDisp -> ActiveEv = NULL;
	}
}

int iQuantSecond(void)
{
	SDL_Event *k;
#ifdef _DEBUG
	int cr;
	int count = 0;
	XBuffer* XBufBMP,*XBufPAL;
#endif
#ifdef _ACI_SKIP_MAINMENU_
	if(!actIntLog){
		iScrDisp -> flags |= SD_EXIT;
		iScrDisp -> ret_val = 1;
#ifdef _ACI_STARTUP_LOAD_GAME_
		iScrDisp -> ret_val = ACI_LOAD_CODE;
#endif
	}
#endif
	if(!iPause) {
		if(actIntLog && NetworkON){
			if(iEscaveTimer == -1)
				iEscaveTimer = iGetEscaveTime();

			iEscaveTimer--;
			if(iEscaveTimer < 0){
				iScrDisp -> flags |= SD_EXIT;
				iScrExitCode = 1;
				aScrDisp -> events -> clear();
			}
		}
		if(iForcedExitFlag && iScrDisp -> ActiveEv && iScrDisp -> ActiveEv -> cur_time <= 1){
			iScrDisp -> flags |= SD_EXIT;
			iForcedExitFlag = 0;
			if(iTheEND){
				Dead = 1;
				acsHandleExtEvent(ACS_GLOBAL_EXIT);
			}
		}
		if(!(iScrDisp -> flags & SD_EXIT)){
			if(iKeyPressed(iKEY_EXIT)){
				i_slake_pal(iscrPal,32);
				if(!actIntLog){
					iSaveControls();
					iSaveData();
				}
				if(actIntLog)
					acsHandleExtEvent(ACS_GLOBAL_EXIT);
				aciPrepareEndImage();
				iScrExitCode = 0;
				XGR_MouseHide();
				return 1;
			}
			if(!(iScrDisp -> flags & SD_INPUT_STRING)) {
				while(KeyBuf -> size) {
					k = KeyBuf -> get();
					if(iChatON)
						iChatKeyQuant(k);
					if((k->type == SDL_KEYDOWN && k->key.keysym.scancode == SDL_SCANCODE_ESCAPE) && actIntLog){
						iPause ^= 1;
						acsScreenID = 2;
					}

					if(k->type == SDL_KEYDOWN && k->key.keysym.scancode == SDL_SCANCODE_F11) {
						shotFlush();
					}

					if (k->type == SDL_KEYDOWN || k->type == SDL_KEYUP) {
						iKeyTrap(k->key.keysym.scancode);
					} else if (k->type == SDL_JOYBUTTONDOWN || k->type == SDL_JOYBUTTONUP) {
						iKeyTrap(k->jbutton.button | SDLK_JOYSTICK_BUTTON_MASK);
					} else if (k->type == SDL_CONTROLLERBUTTONDOWN || k->type == SDL_CONTROLLERBUTTONUP) {
						iKeyTrap(k->cbutton.button | SDLK_GAMECONTROLLER_BUTTON_MASK);
					} else if (k->type == SDL_JOYHATMOTION) {
						iKeyTrap((k->jhat.value + 10*k->jhat.hat) | SDLK_JOYSTICK_HAT_MASK);
					}
				}
			} /*else {
				k = JoystickWhatsPressedNow();
				if(k)
					KeyBuf -> put(k,CUR_KEY_PRESSED);
			}*/
			if(iScrDisp -> flags & MS_LEFT_PRESS){
				iScrDisp -> flags &= ~MS_LEFT_PRESS;
				iKeyTrap(iMOUSE_LEFT_PRESS_CODE);
				if(iChatON) iChatMouseQuant(iMouseX,iMouseY,1);
			}
			if(iScrDisp -> flags & MS_RIGHT_PRESS){
				iScrDisp -> flags &= ~MS_RIGHT_PRESS;
				iKeyTrap(iMOUSE_RIGHT_PRESS_CODE);
				if(iChatON) iChatMouseQuant(iMouseX,iMouseY,1);
			}
			if(iScrDisp -> flags & MS_MOVED){
				iScrDisp -> flags &= ~MS_MOVED;
				iKeyTrap(iMOUSE_MOVE_CODE);
				if(iMouseLPressFlag)
					iKeyTrap(iMOUSE_LEFT_MOVE);
				if(iMouseRPressFlag)
					iKeyTrap(iMOUSE_RIGHT_MOVE);
			}

#ifdef iMOVE_MOUSE_OBJECTS
			iMovedObj = iGetScreenObj(iMouseX + iScreenOffs,iMouseY);
			iShowObjCoords(iMovedObj);
#endif
			if(!iPause && actIntLog)
				uvsQuant();

			if(NetworkON) void_network_quant();

			if(!iPause){
				iFrameCount ++;
				if(iFrameCount >= iFRAME_CYCLE){
					iFrameFlag = 1;
					iFrameCount = 0;
				}

				if(actIntLog) aci_iScreenQuant();
				iScrDisp -> EventQuant();
				if(actIntLog && !iScreenOffs) aciML_D -> flush();

				if(actIntLog)
					ipal_iter(iScreenOffs);

				if(actIntLog && !iScreenOffs && aciBreathFlag){
					iSoundQuant();
				}

				if(EventFlag){
					i_postExtQuant();
					EventFlag = 0;
				}
				if(iChatON) iChatQuant(1);
			}
			return 0;
		}
		if(iScrDisp -> ret_val == ACI_LOAD_CODE) aciLoadLog = 1;
		if(!actIntLog){
			iSaveControls();
			iSaveData();
		}
		return 1;
	}
	else {
		if(actIntLog && !iScreenOffs && acsScrD -> curScrID >= 3)
			ipal_iter(0);
		if(acsQuant()){
			iPause = 0;
			if(acsScrD -> curScrID >= 3){
				aciCloseGate();
			}
			put_map(iScreenOffs,0,I_RES_X,I_RES_Y);
			if(acsScrD -> curScrID >= 3){
				aciOpenGate();
			}
			else
				XGR_Flush(0,0,XGR_MAXX,XGR_MAXY);
		}
		return 0;
	}
}

void iFinitQuant(void)
{
	if(actIntLog){
		aciHideLocation();
	}
	else {
		iSaveData();
	}

#ifdef _ACI_SKIP_MAINMENU_
	if(actIntLog){
#endif
	free_hfonts();
	iScrDisp -> free_mem();

#ifdef _ACI_SKIP_MAINMENU_
	}
#endif
	ivMap -> finit();

	if(!actIntLog) iScrDisp -> finit_texts();

	XGR_MouseSetPromptData(NULL);
	XGR_MouseObj.PromptDeltaX = 0;

	XGR_MouseHide();
	iScreenActive = 0;

//	iHandleExtEvent(iEXT_UPDATE_MUSIC_MODE);
	iHandleExtEvent(iEXT_UPDATE_MUSIC_VOLUME);
}

void iOutEscaveInit(void)
{
	if(actIntLog && !(aScrDisp -> flags & AS_ISCREEN_INV_MODE)){
		aciML_SendEvent(AML_SHUTDOWN_EV);
	}
}

int iOutEscaveQuant(void)
{
#ifndef _ACI_NO_SHOP_ANIMATION_
	if(actIntLog && !(aScrDisp -> flags & AS_ISCREEN_INV_MODE)){
		if(!(aciML_D -> flags & AML_END_SHUTDOWN) && !(aciML_D -> flags & AML_PAUSE)){
			aciML_D -> quant();
			aciML_D -> flush();
			return 0;
		}
	}
#endif
	return 1;
}

void iOutEscaveFinit(void)
{
	if(actIntLog){
		aciML_D -> flags &= ~AML_END_SHUTDOWN;
	}
}

void restore_mouse_cursor(void)
{
	XGR_MouseSetCursor(MouseSizeX,MouseSizeY,MouseSize,MouseFrames);
	XGR_MouseSetSpot(MouseSizeX/2,0);
	XGR_MouseSetSeq(0,0,XGM_NONE,0);

	XGR_MouseRedraw();
}

void set_mouse_cursor(char* p,int sx,int sy)
{
	XGR_MouseSetCursor(sx,sy,1,p);
	XGR_MouseSetSpot(sx/2,sy/2);
	XGR_MouseSetSeq(0,0,XGM_NONE,0);

	XGR_MouseRedraw();
}

void MS_init(void)
{
	set_mouse();

	XGR_MouseSetPressHandler(XGM_LEFT_BUTTON,iMS_LeftPress);
	XGR_MouseSetUnPressHandler(XGM_LEFT_BUTTON,iMS_LeftUnpress);

	XGR_MouseSetPressHandler(XGM_RIGHT_BUTTON,iMS_RightPress);
	XGR_MouseSetUnPressHandler(XGM_RIGHT_BUTTON,iMS_RightUnpress);

	XGR_MouseSetMoveHandler(iMS_Movement);
}

void iMS_LeftPress(int fl, int x, int y)
{
	iScrDisp -> flags |= MS_LEFT_PRESS;
	iMouseX = x;
	iMouseY = y;

	iMouseLPressFlag = 1;
	aciML_ToolzerON = 1;

	if(actIntLog)
		aMS_LeftPress(0,x,y);

	XGR_MouseSetSeq(0,1,XGM_PLAY_ONCE,1);
}

void iMS_LeftUnpress(int fl, int x, int y)
{
	iMouseX = x;
	iMouseY = y;

	aciML_ToolzerON = 0;
	iMouseLPressFlag = 0;

	if(actIntLog)
		aMS_LeftUnpress(0,x,y);

	XGR_MouseSetSeq(XGR_MouseCurFrame,XGR_MouseNumFrames - 1,XGM_PLAY_ONCE,1);
}

void iMS_RightPress(int fl, int x, int y)
{
	iScrDisp -> flags |= MS_RIGHT_PRESS;
	iMouseX = x;
	iMouseY = y;

	iMouseRPressFlag = 1;

	if(actIntLog)
		aMS_RightPress(0,x,y);

	XGR_MouseSetSeq(0,1,XGM_PLAY_ONCE,1);
}

void iMS_RightUnpress(int fl, int x, int y)
{
	iMouseX = x;
	iMouseY = y;

	iMouseRPressFlag = 0;

	if(actIntLog)
		aMS_RightUnpress(0,x,y);

	XGR_MouseSetSeq(XGR_MouseCurFrame,XGR_MouseNumFrames - 1,XGM_PLAY_ONCE,1);
}

void InitScreenFonts(void)
{
	int i,sz;
	XBuffer XBuf;
	XStream fh;

	char* buf;
	iScrFontTable = new iScreenFont*[I_FONT_SIZE];
	for(i = 0; i < I_FONT_SIZE; i ++){
		XBuf.init();
		XBuf < "resource/iscreen/fonts/sfont";
		if(i < 10)
			XBuf < "0";
		XBuf <= i < ".fnt";

		fh.open(XBuf.GetBuf(),XS_IN);
		sz = fh.size();

		buf = new char[sz];
		fh.read(buf,sz);
		fh.close();

		iScrFontTable[i] = new iScreenFont(buf,sz);
	}
}

iScreen* iCreateScreen(const char* id,iScreen* tpl)
{
	iScreen* p = new iScreen;
	p -> init_id(id);
	if(tpl)
		tpl -> copy(p);
	return p;
}

iScreenObject* iCreateScreenObject(const char* id,iScreenObject* tpl)
{
	iTriggerObject* trg;
	iScreenObject* p;
	if(tpl && tpl -> flags & OBJ_TRIGGER){
		trg = new iTriggerObject;
		p = (iScreenObject*)trg;
	}
	else
		p = new iScreenObject;

	p -> init_id(id);

	if(tpl)
		tpl -> copy(p);
	return p;
}

iTriggerObject* iCreateTriggerObject(const char* id,iScreenObject* tpl)
{
	iTriggerObject* trg;
	iScreenObject* p;

	trg = new iTriggerObject;
	p = (iScreenObject*)trg;

	p -> init_id(id);

	if(tpl)
		tpl -> copy(p);

	trg -> flags |= OBJ_TRIGGER;

	return trg;
}

iScreenElement* iCreateScreenElement(int tp,const char* id,iScreenElement* tpl)
{
	iScreenElement* p = NULL;
	iStringElement* str;
	iBitmapElement* bmp;
	iScrollerElement* scr;
	iTerrainElement* tr;
	iS_StringElement* sstr;
	iAVIElement* avi;
	iAVIBorderElement* avi_b;

	switch(tp){
		case I_STRING_ELEM:
			str = new iStringElement;
			p = (iScreenElement*)str;
			break;
		case I_BITMAP_ELEM:
			bmp = new iBitmapElement;
			p = (iScreenElement*)bmp;
			break;
		case I_SCROLLER_ELEM:
			scr = new iScrollerElement;
			p = (iScreenElement*)scr;
			break;
		case I_TERRAIN_ELEM:
			tr = new iTerrainElement;
			p = (iScreenElement*)tr;
			break;
		case I_S_STRING_ELEM:
			sstr = new iS_StringElement;
			p = (iScreenElement*)sstr;
			break;
		case I_AVI_ELEM:
			avi = new iAVIElement;
			p = (iScreenElement*)avi;
			break;
		case I_AVI_BORDER_ELEM:
			avi_b = new iAVIBorderElement;
			p = (iScreenElement*)avi_b;
			break;
	}
	if(!p)
		ErrH.Abort("Error creating element...");
	if(id)
		p -> init_id(id);
	if(tpl)
		tpl -> copy(p);
	return p;
}

iScreenEvent* iCreateScreenEvent(iScreenEvent* tpl)
{
	iScreenEvent* p = new iScreenEvent;
	if(tpl)
		tpl -> copy(p);
	return p;
}

iScreenEventCommand* iCreateEventCommand(iScreenEventCommand* tpl)
{
	iScreenEventCommand* p = new iScreenEventCommand;
	if(tpl)
		tpl -> copy(p);
	return p;
}

void iInitText(iScreenObject* obj, char* text, int text_len, int font, int space, int null_level)
{
	int i, y = 0, t_sz;
	char* buf;
	iStringElement* p;
	buf = new char[text_len];
	memcpy(buf, text, text_len);

	for(i = 0; i < text_len; i ++) {
		if(buf[i] == '\r' || buf[i] == '\n') {
			buf[i] = 0;
		}
	}
	i = 0;
	while(i < text_len) {
		while(!buf[i]) {
			i++;
		}
		if(i < text_len) {
			p = new iStringElement;
			t_sz = strlen(buf + i) + 1;
			if(t_sz) {
				p->string = new char[t_sz];
				strcpy(p->string, buf + i);
				p->flags |= EL_TEXT_STRING;

				p->font = font;
				p->null_level = null_level;
				p->space = space;

				p->lY = y;
				y += HFntTable[p->font]->SizeY + p->space;
				obj->add_element((iListElement*)p);
			}
			while(buf[i]) {
				i++;
			}
		}
	}
	delete[] buf;
}

void iInitS_Text(iScreenObject* obj,char* text,int text_len,int font,int space,int null_level)
{
	int i,y = 0,t_sz;
	char* buf;
	iS_StringElement* p;
	buf = new char[text_len];
	memcpy(buf,text,text_len);

	for(i = 0; i < text_len; i ++){
		if(buf[i] == '\r' || buf[i] == '\n') buf[i] = 0;
	}
	i = 0;
	while(i < text_len){
		while(!buf[i]) i ++;
		if(i < text_len){
			p = new iS_StringElement;
			t_sz = strlen(buf + i) + 1;
			if(t_sz){
				p -> string = new char[t_sz];
				strcpy(p -> string,buf + i);
				p -> flags |= EL_TEXT_STRING;

				p -> font = font;
				p -> null_level = null_level;
				p -> space = space;

				p -> lY = y;
				y += iScrFontTable[p -> font] -> SizeY + p -> space;
				obj -> add_element((iListElement*)p);
			}
			while(buf[i]) i ++;
		}
	}
}

void i_change_res(void)
{
	int emode = ExclusiveLog ? XGR_EXCLUSIVE : 0;

	/*if(actintLowResFlag){
		if(XGR_ReInit(640,480,emode)) ErrH.Abort("Error video initialization");
	}*/
	XGR_Flip();
	set_mouse();
}

void iSecondInit(void)
{
	ivMapPrepare();
	map_init();
	if(actIntLog)
		aciVMapPrepare();

	iScrDisp -> flags &= ~SD_EXIT;
}

void iScrQuantPrepare(void)
{
	actIntLog = 1;

	if(iScreenLog){
		iSecondInit();
		iQuantFirst();
		iQuantPrepare();
	}
}

void iScrQuantFinit(void)
{
	static unsigned char pal_buf[768];
	if(iScreenLog){
		iFinitQuant();
		set_key_nadlers(&KeyCenter, NULL);

		i_slake_pal(iscrPal,16);

		if(actintActiveFlag){
			i_getpal(pal_buf);
			i_change_res();
		}

		aciSendEvent2actint(EV_EVINCE_PALETTE,NULL);
	}
	actIntLog = 0;
}

void i_getpal(unsigned char* p)
{
	XGR_GetPal(p);
}

void i_slake_pal(unsigned char* p,int delta)
{
	int i,col,scale = 256;

	unsigned char* buf = new unsigned char[768];

	i_getpal(p);

	while(scale > 0){
		for(i = 0; i < 768; i ++){
			col = p[i];
			buf[i] = (col * scale) >> 8;
		}
		XGR_SetPal(buf,0,255);

		SDL_Delay(10);
		scale -= delta;
	}

	memset(buf,0,768);
	XGR_SetPal(buf,0,255);
	XGR_Fill(0);

	//XGR_Flush(0,0,XGR_MAXX,XGR_MAXY);
	delete[] buf;
}


void i_evince_pal(unsigned char* p,int delta)
{
	int i,col,scale = 0;

	unsigned char* buf = new unsigned char[768];

	while(scale < 256){
		for(i = 0; i < 768; i ++){
			col = p[i];
			buf[i] = (col * scale) >> 8;
		}
		XGR_SetPal(buf,0,255);

		SDL_Delay(10);
		scale += delta;
	}

	XGR_SetPal(p,0,255);

	//XGR_Flush(0,0,XGR_MAXX,XGR_MAXY);
	delete[] buf;
}

void aci_setMatrixBorder(void)
{
	int id,i,t,sz;
	XBuffer XBuf;
	iBitmapElement* p = (iBitmapElement*)aScrDisp -> curLocData -> objList[ACI_MATRIX_BORDER_ID];
	if(!p) return;

	short sx,sy;
	if(aScrDisp -> curMatrix){
		id = aScrDisp -> curMatrix -> internalID;

		XBuf < "resource/actint/iscreen/matrix/mt";
		if(id < 10) XBuf < "0";
		XBuf <= id < ".bmp";

		XStream fh(XBuf.GetBuf(),XS_IN);
		fh > sx > sy;
		fh.read(p -> fdata,sx * sy);
		fh.close();

		sz = sx * sy;
		for(i = 0; i < sz; i ++){
			t = ((unsigned char*)p -> fdata)[i];
			if(t)
				t += p -> bmp_null_level;
			else
				t = HFONT_NULL_LEVEL;

			p -> fdata[i] = t;
		}
	}
	else {
		memset(p -> fdata,HFONT_NULL_LEVEL,p -> SizeX * p -> SizeY);
	}
}

void aci_setScMatrixBorder(void)
{
	XBuffer XBuf;
//	if(!aScrDisp -> secondMatrix) return;
	iBitmapElement* p = (iBitmapElement*)aScrDisp -> curLocData -> objList[ACI_SMATRIX_BORDER_ID];
	if(!p) return;

	if(!aScrDisp -> secondMatrix){
		memset(p -> fdata,HFONT_NULL_LEVEL,p -> bSizeX * p -> bSizeY);
		return;
	}

	short sx,sy;
	int id = aScrDisp -> secondMatrix -> internalID;

	XBuf < "resource/actint/iscreen/matrix/mt";
	if(id < 10) XBuf < "0";
	XBuf <= id < ".bmp";

	XStream fh(XBuf.GetBuf(),XS_IN);
	fh > sx > sy;
	fh.read(p -> fdata,sx * sy);
	fh.close();

	int i,t,sz = sx * sy;

	for(i = 0; i < sz; i ++){
		t = ((unsigned char*)p -> fdata)[i];
		if(t)
			t += p -> bmp_null_level;
		else
			t = HFONT_NULL_LEVEL;

		p -> fdata[i] = t;
	}
}

void aciSwapMatrices(void)
{
	_iSTORE_OFFS_;

	int i,max_timer,num1,num2,timer = 0;

	iScreenObject* m_owner = NULL,*m2_owner = NULL;
	iBitmapElement* b,*b1;

	b = (iBitmapElement*)aScrDisp -> curLocData -> objList[ACI_MATRIX_BORDER_ID];
	b1 = (iBitmapElement*)aScrDisp -> curLocData -> objList[ACI_SMATRIX_BORDER_ID];
	if(b) m_owner = (iScreenObject*)b -> owner;
	if(b1) m2_owner = (iScreenObject*)b1 -> owner;

	unsigned char* fon_buf;

	int v_flag = XGR_MouseVisible();
	if(v_flag)
		XGR_MouseHide();

#ifndef _ACI_NO_SHOP_ANIMATION_
	num1 = aScrDisp -> curLocData -> numMatrixShutters1;
	num2 = aScrDisp -> curLocData -> numMatrixShutters2;
	max_timer = aScrDisp -> curLocData -> numMatrixShutters1 + aScrDisp -> curLocData -> numMatrixShutters2;
	for(i = 0; i < num1; i ++){
		aScrDisp -> curLocData -> MatrixShutters1[i] -> prepare(ACI_SHUTTER_CLOSE);
		aScrDisp -> curLocData -> MatrixShutters1[i] -> loadData();
	}
	for(i = 0; i < num2; i ++){
		aScrDisp -> curLocData -> MatrixShutters2[i] -> prepare(ACI_SHUTTER_CLOSE);
		aScrDisp -> curLocData -> MatrixShutters2[i] -> loadData();
	}

	fon_buf = new unsigned char[XGR_MAXX * XGR_MAXY];
	i_get_scrfon(0,0,XGR_MAXX,XGR_MAXY,fon_buf);

	SOUND_GATE();

	timer = 0;
	while(timer < max_timer){
		timer = 0;
		for(i = 0; i < num1; i ++)
			timer += aScrDisp -> curLocData -> MatrixShutters1[i] -> Quant();
		for(i = 0; i < num2; i ++)
			timer += aScrDisp -> curLocData -> MatrixShutters2[i] -> Quant();
		i_put_scrfon(0,0,XGR_MAXX,XGR_MAXY,fon_buf);
		for(i = 0; i < num1; i ++)
			aScrDisp -> curLocData -> MatrixShutters1[i] -> Redraw();
		for(i = 0; i < num2; i ++)
			aScrDisp -> curLocData -> MatrixShutters2[i] -> Redraw();
		XGR_Flush(0,0,XGR_MAXX,XGR_MAXY);
		xtClearMessageQueue();
	}
#endif

	aScrDisp -> swap_matrices();

	aci_setMatrixBorder();
	aci_setScMatrixBorder();

	if(m_owner){
		m_owner -> putfon();
		m_owner -> redraw();
	}
	if(m2_owner){
		m2_owner -> curHeightScale = 0;
		m2_owner -> putfon();
		m2_owner -> redraw();
	}

	put_map(iScreenOffs,0,I_RES_X,I_RES_Y);
	aScrDisp -> curMatrix -> redraw();

	iScrDisp -> curScr -> show_avi();

#ifdef _ACI_NO_SHOP_ANIMATION_
	XGR_Flush(0,0,XGR_MAXX,XGR_MAXY);
#else
	i_get_scrfon(0,0,XGR_MAXX,XGR_MAXY,fon_buf);

	for(i = 0; i < num1; i ++){
		aScrDisp -> curLocData -> MatrixShutters1[i] -> prepare(ACI_SHUTTER_OPEN);
	}
	for(i = 0; i < num2; i ++){
		aScrDisp -> curLocData -> MatrixShutters2[i] -> prepare(ACI_SHUTTER_OPEN);
	}

	SOUND_GATE();

	timer = 0;
	while(timer < max_timer){
		timer = 0;
		for(i = 0; i < num1; i ++)
			timer += aScrDisp -> curLocData -> MatrixShutters1[i] -> Quant();
		for(i = 0; i < num2; i ++)
			timer += aScrDisp -> curLocData -> MatrixShutters2[i] -> Quant();
		i_put_scrfon(0,0,XGR_MAXX,XGR_MAXY,fon_buf);
		for(i = 0; i < num1; i ++)
			aScrDisp -> curLocData -> MatrixShutters1[i] -> Redraw();
		for(i = 0; i < num2; i ++)
			aScrDisp -> curLocData -> MatrixShutters2[i] -> Redraw();
		XGR_Flush(0,0,XGR_MAXX,XGR_MAXY);
		xtClearMessageQueue();
	}

	for(i = 0; i < num1; i ++){
		aScrDisp -> curLocData -> MatrixShutters1[i] -> freeData();
	}
	for(i = 0; i < num2; i ++){
		aScrDisp -> curLocData -> MatrixShutters2[i] -> freeData();
	}
	delete[] fon_buf;
#endif

	if(v_flag)
		XGR_MouseShow();

}

#define FADE_SPEED	60
void aciCancelMatrix(void)
{
	_iSTORE_OFFS_;

	int i,num2,timer = 0;

	iScreenObject* m_owner = NULL,*m2_owner = NULL;
	iBitmapElement* b,*b1;

	b = (iBitmapElement*)aScrDisp -> curLocData -> objList[ACI_MATRIX_BORDER_ID];
	b1 = (iBitmapElement*)aScrDisp -> curLocData -> objList[ACI_SMATRIX_BORDER_ID];
	if(b) m_owner = (iScreenObject*)b -> owner;
	if(b1) m2_owner = (iScreenObject*)b1 -> owner;

	unsigned char* fon_buf;

	int v_flag = XGR_MouseVisible();
	if(v_flag)
		XGR_MouseHide();

#ifndef _ACI_NO_SHOP_ANIMATION_
	num2 = aScrDisp -> curLocData -> numMatrixShutters2;
	for(i = 0; i < num2; i ++){
		aScrDisp -> curLocData -> MatrixShutters2[i] -> prepare(ACI_SHUTTER_CLOSE);
		aScrDisp -> curLocData -> MatrixShutters2[i] -> loadData();
	}

	fon_buf = new unsigned char[XGR_MAXX * XGR_MAXY];
	i_get_scrfon(0,0,XGR_MAXX,XGR_MAXY,fon_buf);

	SOUND_GATE();

	timer = 0;
	while(timer < num2){
		timer = 0;
		for(i = 0; i < num2; i ++)
			timer += aScrDisp -> curLocData -> MatrixShutters2[i] -> Quant();
		i_put_scrfon(0,0,XGR_MAXX,XGR_MAXY,fon_buf);
		for(i = 0; i < num2; i ++)
			aScrDisp -> curLocData -> MatrixShutters2[i] -> Redraw();
		XGR_Flush(0,0,XGR_MAXX,XGR_MAXY);
		xtClearMessageQueue();
	}
#endif

	aScrDisp -> cancel_matrix();

	aci_setMatrixBorder();
	aci_setScMatrixBorder();

	if(m_owner){
		m_owner -> putfon();
		m_owner -> redraw();
	}
	if(m2_owner){
		m2_owner -> curHeightScale = 0;
		m2_owner -> putfon();
		m2_owner -> redraw();
	}

	put_map(iScreenOffs,0,I_RES_X,I_RES_Y);

	if(aScrDisp -> curMatrix)
		aScrDisp -> curMatrix -> redraw();

	iScrDisp -> curScr -> show_avi();

#ifdef _ACI_NO_SHOP_ANIMATION_
	XGR_Flush(0,0,XGR_MAXX,XGR_MAXY);
#else
	i_get_scrfon(0,0,XGR_MAXX,XGR_MAXY,fon_buf);

	for(i = 0; i < num2; i ++){
		aScrDisp -> curLocData -> MatrixShutters2[i] -> prepare(ACI_SHUTTER_OPEN);
	}

	SOUND_GATE();

	timer = 0;
	while(timer < num2){
		timer = 0;
		for(i = 0; i < num2; i ++)
			timer += aScrDisp -> curLocData -> MatrixShutters2[i] -> Quant();
		i_put_scrfon(0,0,XGR_MAXX,XGR_MAXY,fon_buf);
		for(i = 0; i < num2; i ++)
			aScrDisp -> curLocData -> MatrixShutters2[i] -> Redraw();
		XGR_Flush(0,0,XGR_MAXX,XGR_MAXY);
		xtClearMessageQueue();
	}
	for(i = 0; i < num2; i ++){
		aScrDisp -> curLocData -> MatrixShutters2[i] -> freeData();
	}
	delete[] fon_buf;
#endif

	if(v_flag)
		XGR_MouseShow();

	_iRESTORE_OFFS_;
}

void aciShowScMatrix(void)
{
	_iSTORE_OFFS_;

	int i,num2,timer = 0;

	SOUND_GATE();

	iScreenObject* m_owner = NULL;
	iBitmapElement* b;

	b = (iBitmapElement*)aScrDisp -> curLocData -> objList[ACI_SMATRIX_BORDER_ID];
	if(b) m_owner = (iScreenObject*)b -> owner;

	unsigned char* fon_buf;

	int v_flag = XGR_MouseVisible();
	if(v_flag)
		XGR_MouseHide();

#ifndef _ACI_NO_SHOP_ANIMATION_
	num2 = aScrDisp -> curLocData -> numMatrixShutters2;
	for(i = 0; i < num2; i ++){
		aScrDisp -> curLocData -> MatrixShutters2[i] -> prepare(ACI_SHUTTER_CLOSE);
		aScrDisp -> curLocData -> MatrixShutters2[i] -> loadData();
	}

	fon_buf = new unsigned char[XGR_MAXX * XGR_MAXY];
	i_get_scrfon(0,0,XGR_MAXX,XGR_MAXY,fon_buf);

	timer = 0;
	while(timer < num2){
		timer = 0;
		for(i = 0; i < num2; i ++)
			timer += aScrDisp -> curLocData -> MatrixShutters2[i] -> Quant();
		i_put_scrfon(0,0,XGR_MAXX,XGR_MAXY,fon_buf);
		for(i = 0; i < num2; i ++)
			aScrDisp -> curLocData -> MatrixShutters2[i] -> Redraw();
		XGR_Flush(0,0,XGR_MAXX,XGR_MAXY);
		xtClearMessageQueue();
	}
#endif

	aci_setScMatrixBorder();

	if(m_owner){
		m_owner -> curHeightScale = 256;
		m_owner -> putfon();
		m_owner -> redraw();
	}

	put_map(iScreenOffs,0,I_RES_X,I_RES_Y);

	if(aScrDisp -> curMatrix)
		aScrDisp -> curMatrix -> redraw();

	aScrDisp -> secondMatrix -> redraw();

	iScrDisp -> curScr -> show_avi();

#ifdef _ACI_NO_SHOP_ANIMATION_
	XGR_Flush(0,0,XGR_MAXX,XGR_MAXY);
#else
	i_get_scrfon(0,0,XGR_MAXX,XGR_MAXY,fon_buf);

	for(i = 0; i < num2; i ++){
		aScrDisp -> curLocData -> MatrixShutters2[i] -> prepare(ACI_SHUTTER_OPEN);
	}

	SOUND_GATE();

	timer = 0;
	while(timer < num2){
		timer = 0;
		for(i = 0; i < num2; i ++)
			timer += aScrDisp -> curLocData -> MatrixShutters2[i] -> Quant();
		i_put_scrfon(0,0,XGR_MAXX,XGR_MAXY,fon_buf);
		for(i = 0; i < num2; i ++)
			aScrDisp -> curLocData -> MatrixShutters2[i] -> Redraw();
		XGR_Flush(0,0,XGR_MAXX,XGR_MAXY);
		xtClearMessageQueue();
	}
	for(i = 0; i < num2; i ++){
		aScrDisp -> curLocData -> MatrixShutters2[i] -> freeData();
	}
	delete[] fon_buf;
#endif

	if(v_flag)
		XGR_MouseShow();

	_iRESTORE_OFFS_;
}

#define ACI_DOOR_DELAY		10
void aciShowLocation(void)
{
	int i,timer,max_timer = 0;

//	  StopCDTRACK();
	SlowCDTRACK();

	_iSTORE_OFFS_;

	unsigned char* fon_buf;

	aciAutoSave();

#ifdef _ACI_NO_SHOP_ANIMATION_
	aciML_D -> quant();
	aciML_D -> flush();
	XGR_Flush(0,0,XGR_MAXX,XGR_MAXY);
	XGR_SetPal(iscrPal,0,255);
	XGR_MouseShow();
	return;
#endif

	max_timer = aScrDisp -> curLocData -> numGateShutters;

	if(!max_timer){
		aciML_D -> quant();
		aciML_D -> flush();
		XGR_Flush(0,0,XGR_MAXX,XGR_MAXY);
		i_evince_pal(iscrPal,8);
		XGR_MouseShow();
		return;
	}

	if(XGR_MouseVisible())
		XGR_MouseHide();
/*
	for(i = 0; i < max_timer; i ++){
		aScrDisp -> curLocData -> GateShutters[i] -> loadData();
		aScrDisp -> curLocData -> GateShutters[i] -> prepare(ACI_SHUTTER_OPEN);
	}
*/
	fon_buf = new unsigned char[XGR_MAXX * XGR_MAXY];
	aciML_D -> quant();
	aciML_D -> flush(0);
	i_get_scrfon(0,0,XGR_MAXX,XGR_MAXY,fon_buf);

	//TODO: stalkerg Створки сломаны тут!
	for(i = 0; i < max_timer; i ++)
		aScrDisp -> curLocData -> GateShutters[i] -> Redraw();

	XGR_Flip();
	//XGR_Flush(0,0,XGR_MAXX,XGR_MAXY);
	//i_evince_pal(iscrPal,8);

	timer = CLOCK();
	for(i = 0; i < ACI_DOOR_DELAY; i ++){
		while(CLOCK() < timer + I_TIME_DELTA){
			xtClearMessageQueue();
		}
		SlowCDTRACK();
		timer = CLOCK();
	}

	SOUND_GATE();

	timer = 0;
	while(timer < max_timer){
		timer = 0;
		for(i = 0; i < max_timer; i ++){
			timer += aScrDisp -> curLocData -> GateShutters[i] -> Quant();
			SlowCDTRACK();
		}

		i_put_scrfon(0,0,XGR_MAXX,XGR_MAXY,fon_buf);

		for(i = 0; i < max_timer; i ++)
			aScrDisp -> curLocData -> GateShutters[i] -> Redraw();

		//XGR_Flush(0,0,XGR_MAXX,XGR_MAXY);
		XGR_Flip();
		
		xtClearMessageQueue();
	}

	for(i = 0; i < max_timer; i ++){
		aScrDisp -> curLocData -> GateShutters[i] -> freeData();
	}
	delete []fon_buf;

	StopCDTRACK();
	XGR_MouseShow();

	_iRESTORE_OFFS_;
}

void aciOpenGate(void)
{
	int i,timer,max_timer = 0;

	_iSTORE_OFFS_;

	unsigned char* fon_buf;

#ifdef _ACI_NO_SHOP_ANIMATION_
	XGR_MouseShow();
	return;
#endif
	SOUND_GATE();

	max_timer = aScrDisp -> curLocData -> numGateShutters;

	if(!max_timer){
		XGR_MouseShow();
		return;
	}

	if(XGR_MouseVisible())
		XGR_MouseHide();

	for(i = 0; i < max_timer; i ++){
		aScrDisp -> curLocData -> GateShutters[i] -> loadData();
		aScrDisp -> curLocData -> GateShutters[i] -> prepare(ACI_SHUTTER_OPEN);
	}

	fon_buf = new unsigned char[XGR_MAXX * XGR_MAXY];
	i_get_scrfon(0,0,XGR_MAXX,XGR_MAXY,fon_buf);

	for(i = 0; i < max_timer; i ++)
		aScrDisp -> curLocData -> GateShutters[i] -> Redraw();

	//XGR_Flush(0,0,XGR_MAXX,XGR_MAXY);

	timer = 0;
	while(timer < max_timer){
		timer = 0;
		for(i = 0; i < max_timer; i ++)
			timer += aScrDisp -> curLocData -> GateShutters[i] -> Quant();

		i_put_scrfon(0,0,XGR_MAXX,XGR_MAXY,fon_buf);

		for(i = 0; i < max_timer; i ++)
			aScrDisp -> curLocData -> GateShutters[i] -> Redraw();

		//XGR_Flush(0,0,XGR_MAXX,XGR_MAXY);
		xtClearMessageQueue();
	}

	for(i = 0; i < max_timer; i ++){
		aScrDisp -> curLocData -> GateShutters[i] -> freeData();
	}
	delete[] fon_buf;

	XGR_MouseShow();
	iKeyClear();

	_iRESTORE_OFFS_;
}

void aciHideLocation(void)
{
	int i,timer,max_timer = 0;

	_iSTORE_OFFS_;

	unsigned char* fon_buf;

#ifdef _ACI_NO_SHOP_ANIMATION_
	//XGR_Flush(0,0,XGR_MAXX,XGR_MAXY);
	return;
#endif

	SOUND_GATE();

	max_timer = aScrDisp -> curLocData -> numGateShutters;

	if(!max_timer){
		XGR_Flush(0,0,XGR_MAXX,XGR_MAXY);
		return;
	}

	int v_flag = XGR_MouseVisible();
	if(v_flag)
		XGR_MouseHide();

	for(i = 0; i < max_timer; i ++){
		aScrDisp -> curLocData -> GateShutters[i] -> loadData();
		aScrDisp -> curLocData -> GateShutters[i] -> prepare(ACI_SHUTTER_CLOSE);
	}

	fon_buf = new unsigned char[XGR_MAXX * XGR_MAXY];
	aciML_D -> quant();
	aciML_D -> flush();
	i_get_scrfon(0,0,XGR_MAXX,XGR_MAXY,fon_buf);
	//XGR_Flush(0,0,XGR_MAXX,XGR_MAXY);

	timer = 0;
	while(timer < max_timer){
		timer = 0;
		for(i = 0; i < max_timer; i ++)
			timer += aScrDisp -> curLocData -> GateShutters[i] -> Quant();

		i_put_scrfon(0,0,XGR_MAXX,XGR_MAXY,fon_buf);

		for(i = 0; i < max_timer; i ++)
			aScrDisp -> curLocData -> GateShutters[i] -> Redraw();

		//XGR_Flush(0,0,XGR_MAXX,XGR_MAXY);
		xtClearMessageQueue();
	}

	for(i = 0; i < max_timer; i ++){
		aScrDisp -> curLocData -> GateShutters[i] -> freeData();
	}
	delete[] fon_buf;

	if(v_flag)
		XGR_MouseShow();

	_iRESTORE_OFFS_;

//	StartWTRACK();
}

void aciCloseGate(void)
{
	int i,timer,max_timer = 0;

	_iSTORE_OFFS_;

	unsigned char* fon_buf;

#ifdef _ACI_NO_SHOP_ANIMATION_
	//XGR_Flush(0,0,XGR_MAXX,XGR_MAXY);
	return;
#endif

	SOUND_GATE();

	max_timer = aScrDisp -> curLocData -> numGateShutters;

	if(!max_timer){
		//XGR_Flush(0,0,XGR_MAXX,XGR_MAXY);
		return;
	}

	int v_flag = XGR_MouseVisible();

	if(v_flag)
		XGR_MouseHide();

	for(i = 0; i < max_timer; i ++){
		aScrDisp -> curLocData -> GateShutters[i] -> loadData();
		aScrDisp -> curLocData -> GateShutters[i] -> prepare(ACI_SHUTTER_CLOSE);
	}

	fon_buf = new unsigned char[XGR_MAXX * XGR_MAXY];
	i_get_scrfon(0,0,XGR_MAXX,XGR_MAXY,fon_buf);
	//XGR_Flush(0,0,XGR_MAXX,XGR_MAXY);

	timer = 0;
	while(timer < max_timer){
		timer = 0;
		for(i = 0; i < max_timer; i ++)
			timer += aScrDisp -> curLocData -> GateShutters[i] -> Quant();

		i_put_scrfon(0,0,XGR_MAXX,XGR_MAXY,fon_buf);

		for(i = 0; i < max_timer; i ++)
			aScrDisp -> curLocData -> GateShutters[i] -> Redraw();

		//XGR_Flush(0,0,XGR_MAXX,XGR_MAXY);
		xtClearMessageQueue();
	}

	for(i = 0; i < max_timer; i ++){
		aScrDisp -> curLocData -> GateShutters[i] -> freeData();
	}
	delete[] fon_buf;

	_iRESTORE_OFFS_;
}

void i_get_scrfon(int x,int y,int sx,int sy,unsigned char* buf)
{
	int i,index,s_index;
	unsigned char* p = (unsigned char*)XGR_VIDEOBUF;

	index = 0;
	s_index = x + y * XGR_MAXX;

	for(i = 0; i < sy; i ++){
		memcpy(buf + index,p + s_index,sx);
		index += sx;
		s_index += XGR_MAXX;
	}
}

void i_put_scrfon(int x,int y,int sx,int sy,unsigned char* buf)
{
	int i,index,s_index;
	unsigned char* p = (unsigned char*)XGR_VIDEOBUF;

	index = 0;
	s_index = x + y * XGR_MAXX;

	for(i = 0; i < sy; i ++){
		memcpy(p + s_index,buf + index,sx);
		index += sx;
		s_index += XGR_MAXX;
	}
}

void iShopForcedExit(void)
{
/*
	if(iScrDisp -> ActiveEv){
		iScrDisp -> ActiveEv -> flags &= ~EV_ACTIVE;
		iScrDisp -> ActiveEv = NULL;
	}
	iScrDisp -> key_trap(AML_FORCED_EXIT_CODE);
*/
	iForcedExitFlag = 1;
}

void iLockShop(void)
{
	iScreenObject* p = (iScreenObject*)iScrDisp -> curScr -> get_object("Button04");
	if(p) p -> flags |= OBJ_LOCKED;
}

void iUnlockShop(void)
{
	iScreenObject* p = (iScreenObject*)iScrDisp -> curScr -> get_object("Button04");
	if(p) p -> flags &= ~OBJ_LOCKED;
}

void iLockExit(void)
{
	iScreenObject* p = (iScreenObject*)iScrDisp -> curScr -> get_object("Button01");
	if(p) p -> flags |= OBJ_LOCKED;
}

void iUnlockExit(void)
{
	iScreenObject* p = (iScreenObject*)iScrDisp -> curScr -> get_object("Button01");
	if(p) p -> flags &= ~OBJ_LOCKED;
}

void iSaveData(void)
{
#ifndef _ACI_SKIP_MAINMENU_
	if(!RecorderMode){
		XStream fh("options.dat",XS_OUT);
		iScrDisp -> save_data(&fh);
		fh < aciAutoRun;
		fh.close();
	}
#endif
}

void iLoadData(void)
{
	XStream fh(0);

	if(!RecorderMode){
		if(fh.open("options.dat",XS_IN)){
			iScrDisp -> load_data(&fh);
			fh > aciAutoRun;
			fh.close();
		}
	}

	iHandleExtEvent(iEXT_UPDATE_SOUND_MODE);
	iHandleExtEvent(iEXT_UPDATE_MUSIC_MODE);
	iHandleExtEvent(iEXT_UPDATE_TUTORIAL_MODE);
	iHandleExtEvent(iEXT_UPDATE_SOUND_VOLUME);
	iHandleExtEvent(iEXT_UPDATE_MUSIC_VOLUME);

	iHandleExtEvent(iEXT_UPDATE_TUTORIAL_MODE);

	iHandleExtEvent(iEXT_INIT_KEEP_OPTIONS);

	iHandleExtEvent(iEXT_INIT_JOYSTICK);
	iHandleExtEvent(iEXT_INIT_PROXY);
	iHandleExtEvent(iEXT_INIT_PORT);
}

void iMS_Movement(int v, int x, int y)
{
	iScrDisp -> flags |= MS_MOVED;

	iMousePrevX = iMouseX;
	iMousePrevY = iMouseY;

	iMouseX = x;
	iMouseY = y;

	if(actIntLog)
		aMS_Movement(v,x,y);
}

void iKeyTrap(int k)
{
	if(!iPause){
		if(!iScrDisp -> ActiveEv){
			if(!actIntLog){
				iScrDisp -> CheckScanCode(k);
			}
			else {
				aciML_D -> key_trap(k,AML_KEY);
				aScrDisp -> iKeyTrap(k);
			}

			if(!(iScrDisp -> flags & SD_LOCK_EVENTS))
				iScrDisp -> curScr -> CheckScanCode(k);
		}
	}
}

void i_mem_putspr(int x,int y,int sx,int sy,unsigned char* ptr)
{
	int i,j,_x,_y,_x1,_y1,_sx,_sy,dx = 0,dy = 0;
	unsigned char* scrBuf,*memBuf;

	_x = (x > 0) ? x : 0;
	_y = (y > 0) ? y : 0;

	dx = _x - x;
	dy = _y - y;

	_x1 = (x + sx < XGR_MAXX) ? (x + sx) : XGR_MAXX;
	_y1 = (y + sy < XGR_MAXY) ? (y + sy) : XGR_MAXY;

	_sx = _x1 - _x;
	_sy = _y1 - _y;

	if(_sx <= 0 || _sy <= 0) return;

	scrBuf = (unsigned char*)XGR_VIDEOBUF + _y * XGR_MAXX + _x;
	memBuf = ptr + dy * sx + dx;

	for(i = 0; i < _sy; i ++){
		for(j = 0; j < _sx; j ++){
			if(memBuf[j]){
				scrBuf[j] = memBuf[j];
			}
		}
		scrBuf += XGR_MAXX;
		memBuf += sx;
	}
}

void iKeyClear(void)
{
	KeyBuf -> clear();
}

void iHandleExtEvent(int code,int data)
{
	int mode;
	switch(code){
		case iEXT_INIT_SOCKETS:
			XSocketInit();
			iGetIP();
			break;
		case iEXT_INIT_PORT:
			iServerPort = atoi(iGetOptionValueCHR(iSERVER_PORT));
			break;
		case iEXT_RESTORE_PORT:
			iSetOptionValueCHR(iSERVER_PORT,"2197");
			iServerPort = 2197;
			break;
		case iEXT_SELECT_HALL:
			iCurHall = data;
			iPrepareHallOfFame();
			break;
		case iEXT_INIT_PROXY:
			iInitProxyOptions();
			break;
		case iEXT_INIT_JOYSTICK_OBJ:
			iBlockJoystickOption(JoystickAvailable);
			break;
		case iEXT_INIT_JOYSTICK:
			JoystickMode = iGetOptionValue(iJOYSTICK_TYPE);
			JoystickStickSwitchButton = iGetControlCode(iKEY_JOYSTICK_SWITCH);
			break;
		case iEXT_UPDATE_SOUND_MODE:
			if(!iGetOptionValue(iSOUND_ON)) {
				EffectInUsePriory = 1;
				EffectsOn(!iScreenActive);
				MuteLog = 0;
			} else {
				EffectsOff();
			}
			break;
		case iEXT_UPDATE_TUTORIAL_MODE:
			if(iGetOptionValue(iTUTORIAL_ON))
				XGR_MouseObj.DisablePrompt();
			else
				XGR_MouseObj.EnablePrompt();
			break;
		case iEXT_UPDATE_MUSIC_MODE:
#ifndef _NO_CDAUDIO_
//			mode = xsGetStatusCD();
			mode = xsGetStatusMusic();
			if(!iGetOptionValue(iMUSIC_ON)){
				MusicON = 1;
				if(mode != XCD_PLAYING){
					StartCDTRACK();
				}
			}
			else {
				if(mode == XCD_PLAYING){
					StopCDTRACK();
				}
				MusicON = 0;
			}
//			SoundVolumeCD = xsGetVolumeCD();
			SoundVolumeCD = xsGetVolumeMusic();
#endif
			break;
		case iEXT_UPDATE_MUSIC_VOLUME:
#ifndef _NO_CDAUDIO_
//			xsSetVolumeCD(iGetOptionValue(iMUSIC_VOLUME_CUR)*256/iGetOptionValue(iMUSIC_VOLUME_MAX));
			xsSetVolumeMusic(iGetOptionValue(iMUSIC_VOLUME_CUR)*256/iGetOptionValue(iMUSIC_VOLUME_MAX));
//			SoundVolumeCD = xsGetVolumeCD();
			SoundVolumeCD = xsGetVolumeMusic();
#endif
			break;
		case iEXT_UPDATE_SOUND_VOLUME:
			if(!MuteLog && EffectInUsePriory){
				if(!iGetOptionValue(iSOUND_ON)){
					SetSoundVolume(iGetOptionValue(iSOUND_VOLUME_CUR)*256/iGetOptionValue(iSOUND_VOLUME_MAX));
				}
			}
			break;
		case iEXT_PREPARE_SLOTS:
			iPrepareSaveNames();
			break;
		case iEXT_SET_SLOT:
			iSlotNumber = data;
			break;
		case iEXT_INIT_SERVERS_LIST:
			iFirstServerPtr = avaible_servers.first();
			iInitServersList();
			break;
		case iEXT_INIT_iSERVERS_LIST:
			iFirstServerPtr = avaible_servers.first();
			iInitServersList(1);
			break;
		case iEXT_UPDATE_SERVERS_LIST:
			iUpdateServersList();
			break;
		case iEXT_UPDATE_iSERVERS_LIST:
			iUpdateServersList(1);
			break;
		case iEXT_UP_SERVERS_LIST:
			iServersListUp();
			break;
		case iEXT_UP_iSERVERS_LIST:
			iServersListUp(1);
			break;
		case iEXT_DN_SERVERS_LIST:
			iServersListDown();
			break;
		case iEXT_DN_iSERVERS_LIST:
			iServersListDown(1);
			break;
		case iEXT_SAVE_GAME:
			if(actIntLog && !iPause && !NetworkON){
				if(!(aScrDisp -> flags & AS_INV_MOVE_ITEM)){
					iPause = 1;
					acsScreenID = aScrDisp -> curLocData -> SaveScreenID;
				}
			}
			break;
		case iEXT_CHOOSE_SERVER:
			iCurServer = data;
			break;
		case iEXT_CHECK_SERVER_CONFIG:
//zmod fixed
			if(is_current_server_configured(iGetOptionValueCHR(iSERVER_NAME),&my_server_data, &z_my_server_data)){
				iUpdateOptionValue(iSERVER_NAME);
				iSetMultiGameParameters();
				iUpdateMultiGameName();
				iEvLineID = 3;
			}
			else {
				iUpdateMultiGameName();
				iEvLineID = 4;
			}
//#ifdef ZMOD_BETA
//			NetRnd.Init(my_server_data.InitialRND);
//#endif
			break;
		case iEXT_CHECK_END_FLAG:
			if(iEndGameFlag)
				iEvLineID = 1;
			else
				iEvLineID = 3;
			break;
		case iEXT_CONNECT:
			iConnect2Server();
			break;
		case iEXT_FIRST_SEARCH:
			if(!iFirstServerSearch){
//				  iFirstServerSearch = 1;
				iEvLineID = 1;
			}
			else
				iEvLineID = 2;
			break;
		case iEXT_INIT_CONTROL:
			iInitControlKey(data & 0xFF,(data >> 8) & 0xFF);
			break;
		case iEXT_DELETE_CONTROL:
			iDeleteControlKey(data & 0xFF,(data >> 8) & 0xFF);
			break;
		case iEXT_RESET_CONTROLS:
			iResetControls();
			iInitControlObjects();
			break;
		case iEXT_UPDATE_OPTION:
			iUpdateOptionValue(data);
			break;
		case iEXT_SELECT_GAME:
			iCurMultiGame = data;
			iUpdateMultiGameName();
			break;
		case iEXT_SEND_CONFIG_DATA:
			iGetMultiGameParameters();
//zmod fixed
			if(!send_server_data(iGetOptionValueCHR(iSERVER_NAME),&my_server_data,&z_my_server_data)){
				iUpdateOptionValue(iSERVER_NAME);
				iUpdateMultiGameName();
				iSetMultiGameParameters();
				iEvLineID = 3;
			}
			else {
				iUpdateOptionValue(iSERVER_NAME);
				iUpdateMultiGameName();
				iEvLineID = 4;
			}
//#ifdef ZMOD_BETA
//			NetRnd.Init(my_server_data.InitialRND);
//#endif
			break;
		case iEXT_CHOOSE_PARAMS_SCREEN:
			iEvLineID = iCurMultiGame + 1;
			break;
		case iEXT_ACTIVATE_CHAT:
			if(!iChatON){
				iScreenChat = 1;
				iChatInit();
			}
			break;
		case iEXT_DEACTIVATE_CHAT:
			if(iChatON){
				iChatFinit();
				put_map(iScreenOffs,0,I_RES_X,I_RES_Y);
				//XGR_Flush(0,0,XGR_MAXX,XGR_MAXY);
			}
			break;
		case iEXT_LOCK_PARAMS:
			iLockMultiGameParameters();
			break;
		case iEXT_UNLOCK_PARAMS:
			iUnlockMultiGameParameters();
			break;
		case iEXT_DISCONNECT:
			disconnect_from_server();
			NetworkON = 0;
			break;
		case iEXT_INIT_KEEP_OPTIONS:
			iInitKeepOptions();
			break;
		case iEXT_KEEP_CLEAN_UP:
			iKeepCleanUp();
			break;
		case iEXT_INIT_RESULT:
			iPreparePlayerResults(data);
			break;
		case iEXT_CREATE_SERVER:
			iCreateServer();
			break;
		case iEXT_PREPARE_MP_RESULTS:
			iPrepareResults();
			break;
		//case iFULLSCREEN:
		//	std::cout<<"iFULLSCREEN"<<std::endl;
		//	break;

	}
}

void iPrepareSaveNames(void)
{
	int i,len,time_len;
	XBuffer XBuf;
	XBuffer XBufStr;
	XStream fh(0);

	iScreen* scr = (iScreen*)iScrDisp -> get_object("Loading screen");
	iScreenObject* obj,*obj1;
	if(!scr) return;
	iStringElement* el,*el1;

	for(i = 0; i <= ACS_NUM_SLOTS; i ++){
		XBuf.init();
		XBufStr.init();
		XBuf < "savegame/save";
		if(i < 10) XBuf < "0";
		XBuf <= i < ".dat";

		XBufStr < "SlotStr" <= i;
		el = (iStringElement*)scr -> get_object(XBufStr.address());

		XBufStr.init();
		XBufStr < "SlotStrTime" <= i;
		el1 = (iStringElement*)scr -> get_object(XBufStr.address());

		if(el){
			obj = (iScreenObject*)el -> owner;
			if(el1){
				obj1 = (iScreenObject*)el1 -> owner;
			}
			if(fh.open(XBuf.address(),XS_IN)){
				fh > len;

				time_len = (len >> 16) & 0xFF;
				len &= 0xFF;

				fh.read(el -> string,len);
				el -> string[len] = 0;

				if(el1){
					if(time_len){
						fh.read(el1 -> string,time_len);
						el1 -> string[time_len] = 0;
						obj1 -> flags &= ~OBJ_LOCKED;
					}
					else {
						el1 -> string[0] = 0;
						obj1 -> flags |= OBJ_LOCKED;
					}
				}
				fh.close();
				obj -> flags &= ~OBJ_LOCKED;
			}
			else {
				strcpy(el -> string,iSTR_EMPTY_SLOT);
				obj -> flags |= OBJ_LOCKED;
				if(el1){
					el1 -> string[0] = 0;
					obj1 -> flags |= OBJ_LOCKED;
				}
			}
			el -> init_size();
			el -> init_align();
			obj -> flags |= OBJ_REINIT;
			if(el1){
				el1 -> init_size();
				el1 -> init_align();
				obj1 -> flags |= OBJ_REINIT;
			}
		}
	}
}

void iUpdateServersList(int inet)
{
	if(!inet)
		avaible_servers.find_servers();
	else
		avaible_servers.find_servers_in_the_internet(iGetOptionValueCHR(iHOST_NAME),iServerPort);

	iFirstServerPtr = avaible_servers.first();
	iInitServersList(inet);
}

void iInitServersList(int inet)
{
	int i,sz;
	iScreen* scr;
	iStringElement* el;
	iScreenObject* obj;
	XBuffer XBuf;
	ServerFindChain* p;

	if(!inet)
		scr = (iScreen*)iScrDisp -> get_object("Search server screen");
	else
		scr = (iScreen*)iScrDisp -> get_object("iSearch server screen");
	if(!scr) return;

	p = iFirstServerPtr;

	for(i = 0; i < ACI_NUM_SERVERS; i ++){
		XBuf.init();
		XBuf < "ServerStr" <= i;
		el = (iStringElement*)scr -> get_object(XBuf.address());
		if(el){
			obj = (iScreenObject*)el -> owner;
			memset(el -> string,0,ACI_SERVER_NAME_LEN + 1);
			if(p){
				sz = strlen(p -> name);
				if(sz > ACI_SERVER_NAME_LEN)
					memcpy(el -> string,p -> name,ACI_SERVER_NAME_LEN);
				else
					strcpy(el -> string,p -> name);
				p = p -> next;
				obj -> flags &= ~OBJ_LOCKED;
			}
			else
				obj -> flags |= (OBJ_LOCKED | OBJ_NOT_UNLOCK);
			el -> init_size();
			el -> init_align();
			obj -> flags |= OBJ_MUST_REDRAW;
		}
	}
}

void iServersListUp(int inet)
{
	int i;
	ServerFindChain* p = iFirstServerPtr;
	for(i = 0; i < ACI_NUM_SERVERS; i ++){
		if(p) p = p -> prev;
	}
	if(p){
		iFirstServerPtr = p;
		iInitServersList(inet);
	}
}

void iServersListDown(int inet)
{
	int i;
	ServerFindChain* p = iFirstServerPtr;
	for(i = 0; i < ACI_NUM_SERVERS; i ++){
		if(p) p = p -> next;
	}
	if(p){
		iFirstServerPtr = p;
		iInitServersList(inet);
	}
}

void iConnect2Server(void)
{
	int i;
	ServerFindChain* p = iFirstServerPtr;
	for(i = 0; i < iCurServer; i ++){
		p = p -> next;
	}
	if(!p)
		iEvLineID = 1;
	else {
		if(NetInit(p))
			iEvLineID = 2;
		else
			iEvLineID = 1;
	}
}

ServerFindChain* iGetCurServer(void)
{
	int i;
	ServerFindChain* p = iFirstServerPtr;
	for(i = 0; i < iCurServer; i ++){
		p = p -> next;
	}
	return p;
}

#ifdef _DEBUG
int iMapShotCount = 0;
void iMapShot(void)
{
	unsigned char* p;
	XBuffer XBuf;
	XStream fh;

	XBuf < "imapshot";
	if(iMapShotCount < 10)
		XBuf < "0";

	XBuf <= iMapShotCount < ".bmp";
	fh.open(XBuf.address(),XS_OUT);

	p = new unsigned char[I_RES_X * I_RES_Y];
	get_buf(iScreenOffs,0,I_RES_X,I_RES_Y,p);

	fh < (short)I_RES_X < (short)I_RES_Y;
	fh.write(p,I_RES_X * I_RES_Y);

	fh.close();
	iMapShotCount ++;
	delete p;
}
#endif

void iSoundQuant(int value)
{
	switch(value){
		case ACI_NOISE_SOUND:
			SOUND_NOISE();
			break;
		case ACI_SPEECH0_SOUND:
			SOUND_TALK0();
			break;
		case ACI_SPEECH1_SOUND:
			SOUND_TALK1();
			break;
		case ACI_SPEECH2_SOUND:
			SOUND_TALK2();
			break;
		case ACI_SPEECH3_SOUND:
			SOUND_TALK2();
			break;
		case ACI_WND_SOUND:
			SOUND_WINDOW();
			break;
		case ACI_GATE_SOUND:
			SOUND_GATE();
			break;
		case ACI_L0_OUTIN_SOUND:
		case ACI_L2_OUTIN_SOUND:
			SOUND_OUTIN();
			break;
		case ACI_L0_SWARM_SOUND:
		case ACI_L1_SWARM_SOUND:
			SOUND_SWARM();
			break;
		case ACI_L0_ANGRY_SOUND:
		case ACI_L1_ANGRY_SOUND:
		case ACI_L5_ANGRY_SOUND:
			SOUND_ANGRY();
			break;
		case ACI_L1_STEP0_SOUND:
		case ACI_L2_STEP_SOUND:
		case ACI_L3_STEP_SOUND:
		case ACI_L4_STEP_SOUND:
			SOUND_STEP();
			break;
		case ACI_L2_GETOUT_SOUND:
		case ACI_L3_GETOUT_SOUND:
		case ACI_L5_GETOUT_SOUND:
		case ACI_L6_GETOUT_SOUND:
			SOUND_GETOUT();
			break;
		case ACI_L4_HAND_SOUND:
			SOUND_HAND();
			break;
		case ACI_L5_SCRATCH_SOUND:
		case ACI_L6_SCRATCH_SOUND:
			SOUND_SCRATCH();
			break;
		case ACI_L5_CLAP_SOUND:
		case ACI_L6_CLAP_SOUND:
			SOUND_CLAP();
			break;
	}
}

void iInitControlKey(int id,int num)
{
	//Stalkerg
	if (iScreenLastInput == SDL_SCANCODE_ESCAPE) {
		return;
	}
	//std::cout<<"iInitControlKey id:"<<id<<" num:"<<num<<std::endl;
	iSetControlCode(id,iScreenLastInput,num);
	iDeleteControl(iScreenLastInput,id * iKEY_OBJECT_SIZE + num);
}

void iDeleteControlKey(int id,int num)
{
	//std::cout<<"iDeleteControlKey"<<std::endl;
	int index;
	iScreenObject* obj;
	iResetControlCode(id,num);

	index = id * iKEY_OBJECT_SIZE + num;

	if(iControlsStr && iControlsStr[index]){
		strcpy(iControlsStr[index] -> string,iSTR_NONE);
		obj = (iScreenObject*)iControlsStr[index] -> owner;
		obj -> flags |= OBJ_REINIT;
		obj -> flags |= OBJ_MUST_REDRAW;
	}
}

int getWID(void)
{
	return aScrDisp -> wMap -> world_ptr[aScrDisp -> curLocData -> WorldID] -> uvsID;
}

void iInitKeepOptions(void)
{
	int sz;
	iScreenObject* p;
	if(iGetOptionValue(iKEEP_MODE)){
		sz = GetKeepSpace();
		iResBuf -> init();
		*iResBuf < iSTR_SpaceInUse < " " <= sz < " " < iSTR_MBytes;
		iSetOptionValueCHR(iKEEP_IN_USE,iResBuf -> address());
		iSetOptionValueCHR(iKEEP_CLEAN_UP,iSTR_CleanUp);
		p = (iScreenObject*)iGetObject("Graphics screen","CleanUpObj");
		if(p) p -> flags &= ~OBJ_LOCKED;
		KeepON = 1;
	}
	else {
		iSetOptionValueCHR(iKEEP_IN_USE,"");
		iSetOptionValueCHR(iKEEP_CLEAN_UP,"");
		p = (iScreenObject*)iGetObject("Graphics screen","CleanUpObj");
		if(p) p -> flags |= OBJ_LOCKED;
		KeepON = 0;
	}
}

void iKeepCleanUp(void)
{
	KeepCleanUp();
	iInitKeepOptions();
}

void iCreateServer(void)
{
	if(KDWIN::create_server(iServerPort)){
		iEvLineID = 2;
		iFirstServerPtr = avaible_servers.first();
		iCurServer = 0;
	}
	else {
		iEvLineID = 1;
	}
}

void iInitStrings(void)
{
	if(iRussian){
		iSTR_WORLD_NONE_CHAR = iSTR_WORLD_NONE_CHAR2;
		iSTR_WORLD_FOSTRAL_CHAR = iSTR_WORLD_FOSTRAL_CHAR2;
		iSTR_WORLD_GLORX_CHAR = iSTR_WORLD_GLORX_CHAR2;
		iSTR_WORLD_NECROSS_CHAR = iSTR_WORLD_NECROSS_CHAR2;
		iSTR_WORLD_XPLO_CHAR = iSTR_WORLD_XPLO_CHAR2;
		iSTR_WORLD_KHOX_CHAR = iSTR_WORLD_KHOX_CHAR2;
		iSTR_WORLD_BOOZEENA_CHAR = iSTR_WORLD_BOOZEENA_CHAR2;
		iSTR_WORLD_WEEXOW_CHAR = iSTR_WORLD_WEEXOW_CHAR2;
		iSTR_WORLD_HMOK_CHAR = iSTR_WORLD_HMOK_CHAR2;
		iSTR_WORLD_THREALL_CHAR = iSTR_WORLD_THREALL_CHAR2;
		iSTR_WORLD_ARK_A_ZNOY_CHAR = iSTR_WORLD_ARK_A_ZNOY_CHAR2;
		iSTR_COLOR_GREEN_CHAR = iSTR_COLOR_GREEN_CHAR2;
		iSTR_COLOR_ORANGE_CHAR = iSTR_COLOR_ORANGE_CHAR2;
		iSTR_COLOR_BLUE_CHAR = iSTR_COLOR_BLUE_CHAR2;
		iSTR_COLOR_YELLOW_CHAR = iSTR_COLOR_YELLOW_CHAR2;
		iSTR_MUTE_ALL = iSTR_MUTE_ALL2;
		iSTR_CLEAR_LOG = iSTR_CLEAR_LOG2;
		iSTR_BACKGROUND = iSTR_BACKGROUND2;
		iSTR_LEAVE_CHAT = iSTR_LEAVE_CHAT2;
		iSTR_All = iSTR_All2;
		iSTR_Yellow = iSTR_Yellow2;
		iSTR_Orange = iSTR_Orange2;
		iSTR_Blue = iSTR_Blue2;
		iSTR_Green = iSTR_Green2;
		iSTR_CleanUp = iSTR_CleanUp2;
		iSTR_SpaceInUse = iSTR_SpaceInUse2;
		iSTR_MBytes = iSTR_MBytes2;

		iSTR_MP_Kills = iSTR_MP_Kills2;
		iSTR_MP_Deaths = iSTR_MP_Deaths2;
		iSTR_MP_DeathTimeMin = iSTR_MP_DeathTimeMin2;
		iSTR_MP_DeathTimeMax = iSTR_MP_DeathTimeMax2;
		iSTR_MP_Kills_rate = iSTR_MP_Kills_rate2;
		iSTR_MP_Deaths_rate = iSTR_MP_Deaths_rate2;
		iSTR_MP_Ware01_Delivery = iSTR_MP_Ware01_Delivery2;
		iSTR_MP_Ware02_Delivery = iSTR_MP_Ware02_Delivery2;
		iSTR_MP_Min_delivery_time = iSTR_MP_Min_delivery_time2;
		iSTR_MP_Max_delivery_time = iSTR_MP_Max_delivery_time2;
		iSTR_MP_StolenWares = iSTR_MP_StolenWares2;
		iSTR_MP_Wares_lost = iSTR_MP_Wares_lost2;
		iSTR_MP_Total_time = iSTR_MP_Total_time2;
		iSTR_MP_Min_checkpoint_time = iSTR_MP_Min_checkpoint_time2;
		iSTR_MP_Max_checkpoint_time = iSTR_MP_Max_checkpoint_time2;
		iSTR_MP_1st_part_delivery = iSTR_MP_1st_part_delivery2;
		iSTR_MP_2nd_part_delivery = iSTR_MP_2nd_part_delivery2;
		iSTR_MP_Mechos_frame = iSTR_MP_Mechos_frame2;
		iSTR_MP_Mechos_assembled_in = iSTR_MP_Mechos_assembled_in2;
		iSTR_NONE = iSTR_NONE2;
		iSTR_EMPTY_SLOT = iSTR_EMPTY_SLOT2;
		iSTR_Checkpoints_Number = iSTR_Checkpoints_Number2;
		iSTR_DefaultPassword = iSTR_DefaultPassword2;
	}
	else {
		iSTR_WORLD_NONE_CHAR = iSTR_WORLD_NONE_CHAR1;
		iSTR_WORLD_FOSTRAL_CHAR = iSTR_WORLD_FOSTRAL_CHAR1;
		iSTR_WORLD_GLORX_CHAR = iSTR_WORLD_GLORX_CHAR1;
		iSTR_WORLD_NECROSS_CHAR = iSTR_WORLD_NECROSS_CHAR1;
		iSTR_WORLD_XPLO_CHAR = iSTR_WORLD_XPLO_CHAR1;
		iSTR_WORLD_KHOX_CHAR = iSTR_WORLD_KHOX_CHAR1;
		iSTR_WORLD_BOOZEENA_CHAR = iSTR_WORLD_BOOZEENA_CHAR1;
		iSTR_WORLD_WEEXOW_CHAR = iSTR_WORLD_WEEXOW_CHAR1;
		iSTR_WORLD_HMOK_CHAR = iSTR_WORLD_HMOK_CHAR1;
		iSTR_WORLD_THREALL_CHAR = iSTR_WORLD_THREALL_CHAR1;
		iSTR_WORLD_ARK_A_ZNOY_CHAR = iSTR_WORLD_ARK_A_ZNOY_CHAR1;
		iSTR_COLOR_GREEN_CHAR = iSTR_COLOR_GREEN_CHAR1;
		iSTR_COLOR_ORANGE_CHAR = iSTR_COLOR_ORANGE_CHAR1;
		iSTR_COLOR_BLUE_CHAR = iSTR_COLOR_BLUE_CHAR1;
		iSTR_COLOR_YELLOW_CHAR = iSTR_COLOR_YELLOW_CHAR1;
		iSTR_MUTE_ALL = iSTR_MUTE_ALL1;
		iSTR_CLEAR_LOG = iSTR_CLEAR_LOG1;
		iSTR_BACKGROUND = iSTR_BACKGROUND1;
		iSTR_LEAVE_CHAT = iSTR_LEAVE_CHAT1;
		iSTR_All = iSTR_All1;
		iSTR_Yellow = iSTR_Yellow1;
		iSTR_Orange = iSTR_Orange1;
		iSTR_Blue = iSTR_Blue1;
		iSTR_Green = iSTR_Green1;
		iSTR_CleanUp = iSTR_CleanUp1;
		iSTR_SpaceInUse = iSTR_SpaceInUse1;
		iSTR_MBytes = iSTR_MBytes1;

		iSTR_MP_Kills = iSTR_MP_Kills1;
		iSTR_MP_Deaths = iSTR_MP_Deaths1;
		iSTR_MP_DeathTimeMin = iSTR_MP_DeathTimeMin1;
		iSTR_MP_DeathTimeMax = iSTR_MP_DeathTimeMax1;
		iSTR_MP_Kills_rate = iSTR_MP_Kills_rate1;
		iSTR_MP_Deaths_rate = iSTR_MP_Deaths_rate1;
		iSTR_MP_Ware01_Delivery = iSTR_MP_Ware01_Delivery1;
		iSTR_MP_Ware02_Delivery = iSTR_MP_Ware02_Delivery1;
		iSTR_MP_Min_delivery_time = iSTR_MP_Min_delivery_time1;
		iSTR_MP_Max_delivery_time = iSTR_MP_Max_delivery_time1;
		iSTR_MP_StolenWares = iSTR_MP_StolenWares1;
		iSTR_MP_Wares_lost = iSTR_MP_Wares_lost1;
		iSTR_MP_Total_time = iSTR_MP_Total_time1;
		iSTR_MP_Min_checkpoint_time = iSTR_MP_Min_checkpoint_time1;
		iSTR_MP_Max_checkpoint_time = iSTR_MP_Max_checkpoint_time1;
		iSTR_MP_1st_part_delivery = iSTR_MP_1st_part_delivery1;
		iSTR_MP_2nd_part_delivery = iSTR_MP_2nd_part_delivery1;
		iSTR_MP_Mechos_frame = iSTR_MP_Mechos_frame1;
		iSTR_MP_Mechos_assembled_in = iSTR_MP_Mechos_assembled_in1;
		iSTR_NONE = iSTR_NONE1;
		iSTR_EMPTY_SLOT = iSTR_EMPTY_SLOT1;
		iSTR_Checkpoints_Number = iSTR_Checkpoints_Number1;
		iSTR_DefaultPassword = iSTR_DefaultPassword1;
	}
}

#ifdef iMOVE_MOUSE_OBJECTS

#define iMOVED_OBJ_COUNT	10
int iMovedObjCount = 0;
iScreenObject* iGetScreenObj(int x,int y)
{
	iScreenObject* p = (iScreenObject*)iScrDisp -> curScr -> objList -> last;
	iMovedObjCount ++;

	if(iMovedObjCount < iMOVED_OBJ_COUNT){
		return iMovedObj;
	}
	else {
		iMovedObjCount = 0;

		if(iMovedObj && iMovedObj -> CheckXY(x,y)) return iMovedObj;
		while(p){
			if(p -> CheckXY(x,y)) return p;
			p = (iScreenObject*)p -> prev;
		}
		return NULL;
	}
}

#define iMOVED_OBJ_X	750
#define iMOVED_OBJ_Y	550

#define iMOVED_OBJ_SX	 40
#define iMOVED_OBJ_SY	 40

void iShowObjCoords(iScreenObject* p)
{
	int x,y,sx,sy;

	XGR_Rectangle(iMOVED_OBJ_X,iMOVED_OBJ_Y,iMOVED_OBJ_SX,iMOVED_OBJ_SY,0,0,XGR_FILLED);

	if(iMovedObj){
		x = iMovedObj -> PosX - iScreenOffs;
		y = iMovedObj -> PosY;

		iResBuf -> init();
		*iResBuf <= x < "\n" <= y;

		sx = XGR_TextWidth(iResBuf -> address());
		sy = XGR_TextHeight(iResBuf -> address());
		XGR_OutText(iMOVED_OBJ_X + (iMOVED_OBJ_SX - sx)/2,iMOVED_OBJ_Y + (iMOVED_OBJ_SY - sy)/2,255,iResBuf -> address());
	}
	XGR_Flush(iMOVED_OBJ_X,iMOVED_OBJ_Y,iMOVED_OBJ_SX,iMOVED_OBJ_SY);
}

void iMoveMouseObj(int dx,int dy)
{
	if(!iMovedObj) return;

	iMovedObj -> putfon();
	iregRender(iMovedObj -> PosX,iMovedObj -> PosY,iMovedObj -> PosX + iMovedObj -> SizeX,iMovedObj -> PosY + iMovedObj -> SizeY);
	put_map(iMovedObj -> PosX,iMovedObj -> PosY,iMovedObj -> SizeX,iMovedObj -> SizeY);
	iMovedObj -> flush();

	iMovedObj -> PosX += dx;
	iMovedObj -> PosY += dy;

	iMovedObj -> nPosX += dx;
	iMovedObj -> nPosY += dy;

	iMovedObj -> getfon();
	iMovedObj -> redraw();
	iMovedObj -> flush();
}

int iSummaryLog = 0;
void iWriteScreenSummary(void)
{
	int i,sz;
	char* ptr;

	XStream fh;
	if(!iSummaryLog){
		fh.open("screen.txt",XS_OUT);
		iSummaryLog = 1;
	}
	else {
		fh.open("screen.txt",XS_OUT | XS_APPEND | XS_NOREPLACE);
	}

	iScreenObject* p = (iScreenObject*)iScrDisp -> curScr -> objList -> last;
	fh < "\r\n// ------------ Screen: \"" < iScrDisp -> curScr -> ID_ptr < "\" ------------\r\n\r\n";

	while(p){
		iResBuf -> init();
		*iResBuf < iScrDisp -> curScr -> ID_ptr < "_" < p -> ID_ptr;
		ptr = iResBuf -> address();
		sz = strlen(ptr);
		for(i = 0; i < sz; i ++){
			if(ptr[i] == ' ') ptr[i] = '_';
		}
		*iResBuf < "_Pos";

		fh < "#define " < iResBuf -> address() < "X\t" <= p -> nPosX < "\r\n";
		fh < "#define " < iResBuf -> address() < "Y\t" <= p -> nPosY < "\r\n";

		p = (iScreenObject*)p -> prev;
	}
	fh.close();
}
#endif

void iBlockJoystickOption(int mode)
{
	iScreenObject* obj = (iScreenObject*)iGetObject("Controls screen2","Joystick Option");
	if(obj){
		if(mode)
			obj -> flags &= ~OBJ_LOCKED;
		else
			obj -> flags |= OBJ_LOCKED;
	}
}

void iInitProxyOptions(void)
{
	int mode;
	iScreenObject* obj;
	iStringElement* p1,*p2,*p3,*p4;

	p1 = (iStringElement*)iGetOptionObj(iPROXY_SERVER_STR);
	p2 = (iStringElement*)iGetOptionObj(iPROXY_SERVER);
	p3 = (iStringElement*)iGetOptionObj(iPROXY_PORT_STR);
	p4 = (iStringElement*)iGetOptionObj(iPROXY_PORT);

	if(!iProxyTerrFlag){
		iProxyTerrFlag = 1;
		iProxyServerStrTerr = p1 -> terrainNum;
		iProxyServerTerr = p2 -> terrainNum;
		iProxyPortStrTerr = p3 -> terrainNum;
		iProxyPortTerr = p4 -> terrainNum;
	}

	mode = iGetOptionValue(iPROXY_USAGE);

	if(mode){
		p1 -> terrainNum = iProxyServerStrTerr;
		p2 -> terrainNum = iProxyServerTerr;
		p3 -> terrainNum = iProxyPortStrTerr;
		p4 -> terrainNum = iProxyPortTerr;
	}
	else {
		p1 -> terrainNum = -1;
		p2 -> terrainNum = -1;
		p3 -> terrainNum = -1;
		p4 -> terrainNum = -1;
	}
	obj = (iScreenObject*)p1 -> owner;
	obj -> flags |= OBJ_REINIT;
	obj -> flags |= OBJ_MUST_REDRAW;
	if(!mode) obj -> flags |= OBJ_LOCKED;
	else obj -> flags &= ~OBJ_LOCKED;

	obj = (iScreenObject*)p2 -> owner;
	obj -> flags |= OBJ_REINIT;
	obj -> flags |= OBJ_MUST_REDRAW;
	if(!mode) obj -> flags |= OBJ_LOCKED;
	else obj -> flags &= ~OBJ_LOCKED;

	obj = (iScreenObject*)p3 -> owner;
	obj -> flags |= OBJ_REINIT;
	obj -> flags |= OBJ_MUST_REDRAW;
	if(!mode) obj -> flags |= OBJ_LOCKED;
	else obj -> flags &= ~OBJ_LOCKED;

	obj = (iScreenObject*)p4 -> owner;
	obj -> flags |= OBJ_REINIT;
	obj -> flags |= OBJ_MUST_REDRAW;
	if(!mode) obj -> flags |= OBJ_LOCKED;
	else obj -> flags &= ~OBJ_LOCKED;

	iProxyUsage = iGetOptionValue(iPROXY_USAGE);
	iProxyServer = iGetOptionValueCHR(iPROXY_SERVER);
	iProxyPort = atoi(iGetOptionValueCHR(iPROXY_PORT));
}

void aciShowLocationPicture(void)
{
	int i,max_timer = 0;

	max_timer = aScrDisp -> curLocData -> numGateShutters;

	if(!max_timer)
		return;

	if(XGR_MouseVisible())
		XGR_MouseHide();

	XGR_Fill(0);
	for(i = 0; i < max_timer; i ++){
		aScrDisp -> curLocData -> GateShutters[i] -> loadData();
		aScrDisp -> curLocData -> GateShutters[i] -> prepare(ACI_SHUTTER_OPEN);
		aScrDisp -> curLocData -> GateShutters[i] -> Redraw();
	}
	//XGR_Flush(0,0,XGR_MAXX,XGR_MAXY);
	i_evince_pal(iscrPal,8);
}

void iGetIP(void)
{
	//Should be BigEndian in anycase
	int IP = XSocketLocalHostExternADDR.host;
	XBuffer XBuf;

	XBuf <= (IP & 0xff) < "." <= ((IP >> 8) & 0xff) < "." <= ((IP >> 16) & 0xff) < "." <= ((IP >> 24) & 0xff);
	iSetOptionValueCHR(iIP_ADDRESS,XBuf.address());
}

