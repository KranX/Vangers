#define _ROAD_
#include "zmod_client.h"

#include "global.h"


#define SCREENSHOT


#define ISCREEN
//#define ACTINT
#define SHOW_LOGOS
#define SHOW_IMAGES

#ifdef QUICK_LOAD
#undef ISCREEN
#undef SHOW_IMAGES
#undef SHOW_LOGOS
#endif

#include "_xsound.h"

#include "runtime.h"

#include "sqexp.h"
#include "backg.h"

#include "xjoystick.h"
#include "network.h"

#include "3d/3d_math.h"
#include "3d/3dgraph.h"
#include "3d/3dobject.h"
#include "3d/parser.h"

#include "actint/item_api.h"
#include "units/uvsapi.h"

#include "terra/world.h"
#include "terra/vmap.h"
#include "terra/render.h"

#include "particle/particle.h"
#include "particle/light.h"
#include "particle/partmap.h"
#include "particle/df.h"

#include "uvs/univang.h"
#include "uvs/screen.h"
#include "uvs/diagen.h"

#include "dast/poly3d.h"

#include "units/track.h"
#include "units/hobj.h"
#include "units/moveland.h"
#include "units/effect.h"
#include "units/items.h"
#include "units/sensor.h"
#include "units/mechos.h"
#include "terra/cpuid.h"

#ifdef ACTINT
#include "iscreen/ikeys.h"
#include "iscreen/hfont.h"
#include "iscreen/iscreen.h"
#include "iscreen/controls.h"
#include "actint/actint.h"
#endif

#include "palette.h"
#include "sound/hsound.h"


#ifndef DBGCHECK
#define DBGCHECK
#endif

#define RANDOMIZE

#ifdef MEMORY_STATISTICS
#define MEMSTAT(a) fmem < "\r\n------->>>>>> " < a < "\r\n";
#else
#define MEMSTAT(a)
#endif


// !!! The same params are in the mechos.cpp
#define SLOPE_MAX	Pi/6
#define MAX_ZOOM	384
#define MIN_ZOOM	128

#ifdef _DEBUG
XStream fmemory("memstats.dmp", XS_OUT);
#endif

/* ----------------------------- EXTERN SECTION ---------------------------- */
extern XStream fout;
extern uchar* iscrPal;
extern int MainRqCounter,AuxRqCounter;
extern int RQ_THREAD_COUNT;
extern int SlopeQualityFactor;
extern int camera_zmin;
extern int camera_slope_min;
extern int stop_camera;
extern int camera_moving_xy_enable;
extern int camera_moving_z_enable;
extern int camera_slope_enable;
extern int camera_rotate_enable;

extern int EngineNoise;
extern int BackgroundSound;

extern int aciGroundPressingEnabled;
extern int aciKeyboardLocked;
extern int SoundVolumePanning;

#ifdef ACTINT
extern int aciLoadLog;
extern actIntDispatcher* aScrDisp;
#endif

extern int NumHumanModel;
extern int dgRUSSIAN;
extern int iRussian;
extern int EffectInUsePriory,EffectInUse;

extern bool XGR_FULL_SCREEN;
void iPreInitFirst();

/* --------------------------- PROTOTYPE SECTION --------------------------- */
void ShowImageMousePress(int fl, int x, int y);
void ShowImageKeyPress(SDL_Event *k);
void ComlineAnalyze(int argc,char** argv);
void restore(void);
int NetInit(ServerFindChain* p);
void camera_reset();
int sqrint(int x);
void showF(int x0,int y0,int x1,int y1);
void showCell(void);
void showRadar(void);
void costab(void);
void buildField(void);
void MechPrepare(void);
void hObjPrepare(void);
void cycleTor(int& x,int& y);
void preCALC(void);
char* GetTargetName(const char* name);
void shotFlush(void);
void LoadingMessage(int flush = 0);
void vMapInit(void);
void theEND(void);
void ilandPrepare(void);
void aciSetCameraMenu(void);
void calc_view_factors();
void gameQuant(void);
void SetupPath(void);
char* getRC(void);

void GameTimerON_OFF(void);

void uvsRestoreVanger(void);

void MLload(void);
int MLquant(void);

#ifdef ACTINT
void aciLoadData(void);
void aInit(void);
void aRedraw(void);
void aKeyTrap(SDL_Event *k);
void actIntQuant(void);
void aciPrepareMenus(void);
int acsQuant(void);
void aciFreeAll(void);
void aciFreeUVS(void);
void aLoadFonts32(void);
char* Convert(const char* s,int back = 0);
char* ConvertUTF8(const char* s,int back = 0);
#ifndef ISCREEN
void aKeyInit(void);
#endif
#endif

#ifdef ISCREEN
void aci_LocationQuantPrepare(void);
void aci_LocationQuantFinit(void);
#endif

void KeyCenter(SDL_Event *key);
int distance(int,int);
extern int ibsout(int,int,void*,void*);
int sdlEventToCode(SDL_Event *event);

/* --------------------------- DEFINITION SECTION -------------------------- */

char* host_name = 0;
int host_port = DEFAULT_SERVER_PORT;

int network_log = 0;

const int FPS_PERIOD = 50;
int fps_frame,fps_start;
char fps_string[20];

int stop_all_except_me = 0;

int ErrHExcept = 1;
int RAM16;
int emode;
int vSoundON,vSoundOFF;
int message_mode = 0;

int setTimerLog = 1;
int GameOverTrigger = -1;

#ifdef MEMORY_STATISTICS
XStream fmem("MEMORY.DMP",XS_OUT);
int memQ,memIndex;
int memStart;
void memstatInit(void);
void memstatDumpLeak(void);
#endif

int page;
clock_t _Timer_;
int frame;
int Quit = 1;
int Dead;

XBuffer msg_buf(16000);

int view_lock = 0;
int debug_view = 0;
int StandLog;
int loadingStatus = 1;

int GameQuantReturnValue = 0;

int XSIDE = 200;	  //200 	  270
int YSIDE = 170;	  //170 	  210
int XSIZE = 2*XSIDE + 1;
int YSIZE = 2*YSIDE + 1;

int MainRqCounter,AuxRqCounter;

int MuteLog = 0;
int Verbose;
int DepthShow;

int SkipIntro = 0;

int PalIterLock = 0;

unsigned char* palbuf,*palbufA,*palbufC,*palbufOrg,*palbufInt,*palbufBlack;
//zmod
sqFont sysfont, zchatfont;
int* SI;
int* CO;

int zoom_delta;
int zoom_threshold;

int videoMode = 1;
int beebos;
int idOS,inHighPriority;

const char* roadFNTname = "road.fnt";
//zmod
const char* zchatFNTname = "zfont.fnt";

unsigned RNDVAL = 83;
unsigned realRNDVAL = 83;
int Geo = 0;
int Redraw = 1;
int Pause = 0;
int FirstDraw = 1;

int speed_correction_enabled = 1;
int prev_frame_time = 0;

iGameMap* curGMap;

const char* mapFName = "wrlds.dat";

int ColorShow = 1,SkipShow = 1;
int WorldPrm;
int shotNum;

int ExclusiveLog = 1;
int DIBLog = 0;
int MMXsuppress;
int SkipCD;

int CurrentWorld = 0;

int inTRall;
int TimerMode = 1;

extern int* AVI_index;

int TotalDrawFlag = 1;
int StartMainQuantFlag = 0;

int RecorderMode = 0;
char* RecorderName = NULL;

#ifdef _DEMO_
int aciCompleteGameFlag = 0;
#endif

int ShowImageMouseFlag = 0;
int ShowImageKeyFlag = 0;

#if defined(RUSSIAN_VERSION) && !defined(GERMAN_VERSION)
const char* ErrorVideoMss    = "";
const char* AVInotFound      = "";
const char* AVInotFoundMSS   = "";
#else
const char* ErrorVideoMss = "Error video initialization";
const char* AVInotFound   = "Error video initialization";
const char* AVInotFoundMSS = "Please ensure that the VANGERS CD-ROM is in the drive!";
#endif

const char* nVER = "Patch 4.20";


#include "video/winvideo.h"
sWinVideo winVideo;

void showModal(char* fname, float reelW, float reelH, float screenW, float screenH) {
	bool playing = true;
//	MSG msg;

	//flipGDISurface();
//WORK	winVideo.Open(fname);

	float w = reelW * screenW / 800.0f;
	float h = reelH * screenH / 600.0f;

/*WORK	winVideo.SetWin(XGR_hWnd, (screenW - w) / 2.0f, (screenH - h) / 2.0f, w, h);
	winVideo.SetSize(w, h); */

//	winVideo.FullScreen(1);		// 1 - FullScreen, 0 - Window

/*WORK	winVideo.Play();
	winVideo.HideCursor(1);	*/	// 1 - hide cursor, 0 - unhide cursor


//	winVideo.WaitEnd();

/*	while (playing) {
		if ( PeekMessage(&msg, 0, 0, 0, PM_REMOVE) ) {
			TranslateMessage( &msg );
			DispatchMessage( &msg );
			if (msg.message == WM_KEYDOWN) {
				playing = false;
			}
		} else {
			if (winVideo.IsComplete()) {
				playing = false;
			}
		}
	}*/


/*WORK	winVideo.Stop();
	winVideo.Close(); */
}



int xtInitApplication(void)
{
	XGraphWndID = "VANGERS";
	char* tmp;

#ifdef _DEMO_
	std::cout<<"\""<<XGraphWndID<<": One For The Road\" Cover Demo by K-D Lab (SDL Version)\n";
#else
#ifdef RUSSIAN_VERSION
	std::cout<<"\""<<XGraphWndID<<"\" by K-D Lab (SDL Version)\n";
#ifdef GERMAN_VERSION
	std::cout<<"Release (DE\n";
#else
	std::cout<<"Release (RUS)\n";
#endif
#else
	std::cout<<"\""<<XGraphWndID<<": One For The Road\" by K-D Lab (SDL Version)\n";
	std::cout<<"Release (ENG)\n";
#endif
#ifdef BETA_TESTING
	std::cout<<nVER<<"\n";
#endif
#endif

	_MEM_STATISTIC_("BEGIN WORK WITH MEMORY -> ");

#ifdef MEMORY_STATISTICS
	memstatInit();
#endif

//zmod
#ifdef _ZMOD_DEBUG_
	ZLog = ZLogOpen();
#endif

	//stalkerg:NEED SEE!!!
	//ComlineAnalyze(__argc,__argv);

//	ErrH.SetRestore(restore); #not implement
//	ErrH.SetFlags(XERR_CTRLBRK);


	RNDVAL = 83;
	realRNDVAL = SDL_GetTicks();

	costab();

	_MEM_STATISTIC_("COS TAB -> ");

	preCALC();

	_MEM_STATISTIC_("PRE CALC -> ");

	ilandPrepare();

	_MEM_STATISTIC_("ILANDPREPARE -> ");

//	vMapPrepare(mapFName,WorldPrm);

	XStream ffo(roadFNTname,XS_IN);
	void* fo = new char[ffo.size()];
	ffo.read(fo,ffo.size());
	ffo.close();
	sysfont.init(fo);

    //zmod
    XStream zch_ffo(zchatFNTname,XS_IN);
    void* zch_fo = new char[zch_ffo.size()];
    zch_ffo.read(zch_fo,zch_ffo.size());
    zch_ffo.close();
    zchatfont.init(zch_fo);

	palTr = new PaletteTransform;

#ifdef CDCHECK
	int getCDdrives(void);
	int isCDok(int);
	int d = getCDdrives();
	int mask = 1;
	int res = 0;
	for(int ii = 0;ii < 26;ii++,mask <<= 1)
		if(d & mask) res += isCDok(ii);
	if(!res) ErrH.Abort(AVInotFoundMSS);
#endif

	SetupPath();

	_MEM_STATISTIC_("FONT -> ");


	emode = ExclusiveLog ? XGR_EXCLUSIVE : 0;
	//emode |= XGR_HICOLOR;
	

	if(!videoMode){
		actintLowResFlag = 1;
#ifdef ISCREEN
		videoMode = 2;
#endif
	}
	videoMode = 1;
	float w = 800;
	float h = 600;

	switch(videoMode){
		case 1:
			w = 800;
			h = 600;
			break;
		case 2:
			w = 1024;
			h = 768;
			break;
		case 3:
			w = 1280;
			h = 720;
	}
	
	if(XGR_Init(w,h,emode)) ErrH.Abort(ErrorVideoMss);

//WORK	sWinVideo::Init();
//	::ShowCursor(0);

	//zmod
	if(!SkipIntro) {
//		showModal( "resource\\video\\intro\\logo1.avi", 512, 384, w, h );
//		showModal( "resource\\video\\intro\\logo2.avi", 512, 384, w, h );
//		showModal( "resource\\video\\intro\\intro.avi", 512, 380, w, h );
	}

//WORK	sWinVideo::Done();

	emode = ExclusiveLog ? XGR_EXCLUSIVE : 0;

/*	switch(videoMode){
		case 0:
			if(XGR_ReInit(640,480,emode)) ErrH.Abort(ErrorVideoMss);
			break;
		case 1:
			if(XGR_ReInit(800,600,emode)) ErrH.Abort(ErrorVideoMss);
			break;
		case 2:
			if(XGR_ReInit(1024,768,emode)) ErrH.Abort(ErrorVideoMss);
			break;
		}


	if(XGR_GetVideoLine(1) - XGR_GetVideoLine(0) != XGR_MAXX)
		ErrH.Abort("Bad maxx",XGR_GetVideoLine(1) - XGR_GetVideoLine(0));
*/
	XGR_Fill(0);
	LoadingMessage(1);

	_MEM_STATISTIC_("INIT VIDEO -> ");

	InstallSOUND();
	LoadResourceSOUND("resource/sound/effects/", 0);
	SetSoundVolume(256);

	if(XJoystickInit()){
		std::cout<<"Joystick found\n";
		JoystickMode = JOYSTICK_Joystick;
	} else {
		std::cout<<"Joystick not found"<<std::endl;
	}

	//XSocketInit();
#ifdef _DEBUG
	if(host_name && avaible_servers.talk_to_server(0,host_port,host_name))
		NetInit(avaible_servers.first());
#endif


	_MEM_STATISTIC_("INIT SOUND -> ");

	// Runtime objects init...

	xtCreateRuntimeObjectTable(RTO_MAX_ID);
	GameQuantRTO* gqObj = new GameQuantRTO;
	MainMenuRTO* mmObj = new MainMenuRTO;
	EscaveRTO* eObj = new EscaveRTO;
	EscaveOutRTO* eoObj = new EscaveOutRTO;
	FirstEscaveRTO* fObj = new FirstEscaveRTO;
	FirstEscaveOutRTO* foObj = new FirstEscaveOutRTO;
	PaletteTransformRTO* pObj = new PaletteTransformRTO;
	LoadingRTO1* lObj1 = new LoadingRTO1;
	LoadingRTO2* lObj2 = new LoadingRTO2;
	LoadingRTO3* lObj3 = new LoadingRTO3;
	ShowImageRTO* siObj = new ShowImageRTO;
	ShowAviRTO* saObj = new ShowAviRTO;

//	  siObj -> SetNumFiles(1);
//	  siObj -> SetName("resource\\iscreen\\bitmap\\kdlogo.bmp",0);
//	  siObj -> SetNext(RTO_MAIN_MENU_ID);

#if defined(RUSSIAN_VERSION) && !defined(GERMAN_VERSION)
	saObj -> SetNumFiles(3);
	//saObj -> SetName("resource\\video\\intro\\logo1.avi",0);
	//saObj -> SetName("resource\\video\\intro\\logo2.avi",1);
	//saObj -> SetName("resource\\video\\intro\\intro.avi",2);

	//	saObj -> SetFlag(0,AVI_RTO_HICOLOR);
//	saObj -> SetFlag(1,AVI_RTO_HICOLOR);
//	saObj -> SetFlag(2,AVI_RTO_HICOLOR);
#else
	saObj -> SetNumFiles(4);
	saObj -> SetName("resource/video/intro/logo0.avi",0);
	saObj -> SetName("resource/video/intro/logo1.avi",1);
	saObj -> SetName("resource/video/intro/logo2.avi",2);
	saObj -> SetName("resource/video/intro/intro.avi",3);
//znfo commented in zmod
//	saObj -> SetFlag(0,AVI_RTO_HICOLOR);
//	saObj -> SetFlag(1,AVI_RTO_HICOLOR);
//	saObj -> SetFlag(2,AVI_RTO_HICOLOR);
//	saObj -> SetFlag(3,AVI_RTO_HICOLOR);
//znfo
#endif
	saObj -> SetNext(RTO_MAIN_MENU_ID);

	xtRegisterRuntimeObject(gqObj);
	xtRegisterRuntimeObject(mmObj);
	xtRegisterRuntimeObject(eObj);
	xtRegisterRuntimeObject(eoObj);
	xtRegisterRuntimeObject(fObj);
	xtRegisterRuntimeObject(foObj);
	xtRegisterRuntimeObject(pObj);
	xtRegisterRuntimeObject(lObj1);
	xtRegisterRuntimeObject(lObj2);
	xtRegisterRuntimeObject(lObj3);
	xtRegisterRuntimeObject(siObj);
	//xtRegisterRuntimeObject(saObj);

	if(RecorderMode)
		XRec.Open(RecorderName,RecorderMode);
	else
		RNDVAL = SDL_GetTicks();

	_MEM_STATISTIC_("AFTER FIRST INIT -> ");

#ifdef CDCHECK
	mask = 1;
	res = 0;
	for(ii = 0;ii < 26;ii++,mask <<= 1)
		if(d & mask) res += isCDok(ii);
	if(!res) ErrH.Abort(AVInotFoundMSS);
#endif

	if(!setTimerLog){
		GameQuantRTO* p = (GameQuantRTO*)xtGetRuntimeObject(RTO_GAME_QUANT_ID);
		p -> SetTimer(0);
		}
	if(RecorderMode){
		GameQuantRTO* p = (GameQuantRTO*)xtGetRuntimeObject(RTO_GAME_QUANT_ID);
//		p -> SetTimer(1000/20);
		speed_correction_enabled = 0;
		}


	//STEAM
#ifdef _STEAM_API_
	if ( SteamAPI_RestartAppIfNecessary( k_uAppIdInvalid ) ) {
		// if Steam is not running or the game wasn't started through Steam, SteamAPI_RestartAppIfNecessary starts the
		// local Steam client and also launches this game again.
		
		// Once you get a public Steam AppID assigned for this game, you need to replace k_uAppIdInvalid with it and
		// removed steam_appid.txt from the game depot.
		
		SDL_Quit();
	}
	
	
	// Init Steam CEG
	if ( !Steamworks_InitCEGLibrary() )	{
		std::cout<<"Steamworks_InitCEGLibrary() failed"<<std::endl;
		std::cout<<"Fatal Error, Steam must be running to play this game (InitDrmLibrary() failed)."<<std::endl;
		SDL_Quit();
	}
	
	// Initialize SteamAPI, if this fails we bail out since we depend on Steam for lots of stuff.
	// You don't necessarily have to though if you write your code to check whether all the Steam
	// interfaces are NULL before using them and provide alternate paths when they are unavailable.
	//
	// This will also load the in-game steam overlay dll into your process.  That dll is normally
	// injected by steam when it launches games, but by calling this you cause it to always load,
	// even when not launched via steam.
	if ( !SteamAPI_Init() )	{
		std::cout<<"SteamAPI_Init() failed"<<std::endl;
		std::cout<<"Fatal Error, Steam must be running to play this game (SteamAPI_Init() failed)."<<std::endl;
		SDL_Quit();
	}
	
	// set our debug handler
	//SteamClient()->SetWarningMessageHook( &SteamAPIDebugTextHook );
	
	// Tell Steam where it's overlay should show notification dialogs, this can be top right, top left,
	// bottom right, bottom left. The default position is the bottom left if you don't call this.
	// Generally you should use the default and not call this as users will be most comfortable with
	// the default position.  The API is provided in case the bottom right creates a serious conflict
	// with important UI in your game.
	SteamUtils()->SetOverlayNotificationPosition( k_EPositionTopRight );
#endif
	
	if(SkipIntro)
		return RTO_MAIN_MENU_ID;
	return RTO_MAIN_MENU_ID;
}

extern int activeWTRACK;

void MainMenuRTO::Init(int id)
{
	Dead = 0;
//	activeWTRACK = 0;
_MEM_STATISTIC_("BEFORE MAIN MENU INIT -> ");
#ifdef ISCREEN
	if(!(flags & RTO_INIT_FLAG)){
		LoadingMessage(1);
		iInit();
_MEM_STATISTIC_("AFTER MAIN iINIT INIT -> ");

		palTr -> set(NULL,iscrPal,0,255,&Quit);
		palTr -> quant();

		iQuantFirst();
		iQuantPrepare();
_MEM_STATISTIC_("AFTER IQUANTFIRST INIT -> ");

		//XGR_Flush(0,0,I_RES_X,I_RES_Y);

		SetFlag(RTO_INIT_FLAG);
		rtoSetNextID(RTO_PALETTE_TRANSFORM_ID,RTO_MAIN_MENU_ID);
//		  rtoSetNextID(RTO_ESCAVE_INGATE_ID,RTO_FIRST_ESCAVE_ID);
	}
	else {
		iKeyClear();
		XGR_MouseShow();
	}
#else
	aLoadFonts32();
#endif
	
	XGR_Obj.set_fullscreen(iGetOptionValue(iFULLSCREEN));
_MEM_STATISTIC_("AFTER MAIN MENU INIT -> ");
}

int MainMenuRTO::Quant(void)
{
#ifdef ISCREEN
	int code;
	char* pal;
//znfo next line was commented in zmod
//	ShowImageRTO* p;
	
	
	MainMenuSoundQuant();
	if(flags & RTO_QUANT_FLAG){
		code = iQuantSecond();
		if(code){
			if(!iScrExitCode){
				pal = new char[768];
				i_slake_pal((unsigned char*)pal,32);
				delete[] pal;

				aciPrepareEndImage();
				return RTO_SHOW_IMAGE_ID;
			}
			rtoSetNextID(RTO_LOADING1_ID,RTO_FIRST_ESCAVE_ID);
			return RTO_LOADING1_ID;
		}
		//XGR_Flip();
		return 0;
	}
	else {
		SetFlag(RTO_QUANT_FLAG);
		return RTO_PALETTE_TRANSFORM_ID;
	}
	
#else
	return RTO_LOADING1_ID;
#endif
}

void MainMenuRTO::Finit(void)
{
#ifdef ISCREEN
	
	if(flags & RTO_FINIT_FLAG){
		//actintLowResFlag = !iGetOptionValue(iSCREEN_RESOLUTION);
		actintLowResFlag = 0;
		SoundVolumePanning = !iGetOptionValue(iPANNING_ON);
		aciGroundPressingEnabled = iGetOptionValue(iDESTR_MODE);
		EngineNoise = !iGetOptionValue(iMECH_SOUND);
		BackgroundSound = !iGetOptionValue(iBACK_SOUND);
		iFinitQuant();
		ClearFlag(RTO_ALL_FLAGS);
		
	}
	else
		SetFlag(RTO_FINIT_FLAG);
	XGR_MouseHide();
#endif
_MEM_STATISTIC_("AFTER MAIN MENU FINIT -> ");
}

void LoadingRTO1::Init(int id)
{
	RAM16 = 0;//iGetOptionValue(iDETAIL_SETTING);
	if(!MuteLog && EffectInUsePriory){
		if(!iGetOptionValue(iSOUND_ON)){
			SetSoundVolume(iGetOptionValue(iSOUND_VOLUME_CUR)*256/iGetOptionValue(iSOUND_VOLUME_MAX));
		} else {
			MuteLog = 1; EffectInUsePriory = EffectInUse = 0;
		}
	}

	if(!(flags & RTO_INIT_FLAG)){
		uniVangPrepare();
		uvsCreateItem_in_Crypt();
	}

#ifdef ACTINT
#ifndef ISCREEN
	aKeyInit();
#endif
	aInit();
#endif

#ifdef ACTINT
	XSIDE = aScrDisp -> curIbs -> SideX;
	YSIDE = aScrDisp -> curIbs -> SideY;
	XSIZE = 2*XSIDE;
	YSIZE = 2*YSIDE;
#else
	XSIDE = (XGR_MAXX)/2 - 4;
	YSIDE = (XGR_MAXY)/2 - 4;
	XSIZE = 2*XSIDE;
	YSIZE = 2*YSIDE;
#endif

	set_key_nadlers(&KeyCenter, NULL);

	graph3d_init();

	GeneralSystemInit();

//	palTr -> set(iscrPal,palbufInt,0,255,&Quit);
	palTr -> set(iscrPal,NULL,0,255,&Quit);

	Quit = 1;
	while(Quit){
		palTr -> quant();
		SDL_Delay(10);
	}
	LoadingMessage(1);
	diagenPrepare();
_MEM_STATISTIC_("AFTER LOADING RTO1 INIT -> ");
}

int LoadingRTO1::Quant(void)
{
	//XGR_Flip();
	if(!(flags & RTO_QUANT_FLAG)){
		SetFlag(RTO_QUANT_FLAG);
		return 0;
	}
#ifdef ISCREEN
	return NextID;
#else
	return RTO_LOADING2_ID;
#endif
}

void LoadingRTO1::Finit(void)
{
	ClearFlag(RTO_ALL_FLAGS);
_MEM_STATISTIC_("AFTER LOADING RTO1 FINIT -> ");
}

void EscaveOutRTO::Init(int id)
{
#ifdef ISCREEN
	iOutEscaveInit();
#endif
_MEM_STATISTIC_("AFTER ESCAVE INIT -> ");
}

int EscaveOutRTO::Quant(void)
{
#ifdef ISCREEN
	int val = iOutEscaveQuant();
	if(val){
		if(iAbortGameFlag){
			iAbortGameMode = 2;
			return RTO_LOADING3_ID;
		}
		else
			return NextID;
	}
	//XGR_Flip();
	return 0;
#else
	return NextID;
#endif
}

void EscaveOutRTO::Finit(void)
{
#ifdef ISCREEN
	iOutEscaveFinit();
	aci_LocationQuantFinit();
	if(!iAbortGameFlag)
		uvsCloseQuant();
#endif
_MEM_STATISTIC_("AFTER ESCAVE FINIT -> ");
}

void FirstEscaveOutRTO::Init(int id)
{
#ifdef ISCREEN
#ifndef _ACI_SKIP_SHOP_
	iOutEscaveInit();
#endif
#endif
_MEM_STATISTIC_("AFTER FIRST ESCZVE INIT -> ");
}

int FirstEscaveOutRTO::Quant(void)
{
#ifdef ISCREEN
	int val;
#ifndef _ACI_SKIP_SHOP_
	val = iOutEscaveQuant();
#else
	val = 1;
#endif
	if(val){
		if(iAbortGameFlag){
			iAbortGameMode = 0;
			return RTO_LOADING3_ID;
		}
		else
			return NextID;
	}
	//XGR_Flip();
	return 0;
#else
	return NextID;
#endif
}

void FirstEscaveOutRTO::Finit(void)
{
#ifdef ISCREEN
#ifndef _ACI_SKIP_SHOP_
	iOutEscaveFinit();
	aci_LocationQuantFinit();
#endif
#endif
_MEM_STATISTIC_("AFTER FIRST ESCAVE FINIT -> ");
}

void LoadingRTO2::Init(int id)
{
#ifdef ISCREEN
	FinishFirstShopPrepare(aciLoadLog);
	aciLoadLog = 0;
	i_change_res();
	LoadingMessage(1);
#endif

#ifdef _DEBUG
	StandScreenPrepare();
#endif
_MEM_STATISTIC_("\nBEFORE VMAP  -> ");
	vMapPrepare(mapFName,CurrentWorld);
	vMapInit();
_MEM_STATISTIC_("AFTER VMAP  -> ");

	LoadResourceSOUND(GetTargetName("sound/"), 0);
_MEM_STATISTIC_("AFTER SOUND  -> ");

_MEM_STATISTIC_("AFTER LOAD PARAM  -> ");
	MLload();
_MEM_STATISTIC_("AFTER ML LOAD  -> ");
	PrepareLight();
_MEM_STATISTIC_("AFTER PREPARE LIGHT  -> ");
	EffD.CalcWave();

	dastCreateResource("resource/bml/mole.bml");
_MEM_STATISTIC_("AFTER DAST  -> ");
//	dastCreateResourceSign("resource\\bml\\sign.fnt");

	GeneralTableInit();
_MEM_STATISTIC_("AFTER TABLE GENERAL  -> ");
	GeneralTableOpen();
_MEM_STATISTIC_("AFTER TABLE OPEN  -> ");

#ifdef ACTINT
	curGMap = new iGameMap(aScrDisp -> curIbs -> CenterX,aScrDisp -> curIbs -> CenterY,XSIDE,YSIDE);
#else
	curGMap = new iGameMap(XGR_MAXX/2,XGR_MAXY/2,XSIDE,YSIDE);
#endif
	Redraw = 1;

_MEM_STATISTIC_("AFTER curGMap  -> ");
	loadingStatus = 0;

	vMap -> quant();

	uvsAddStationaryObjs();

	uvsRestoreVanger();
	
#ifndef NEW_TNT
	RestoreBarell();
#endif
	gameQuant(); gameQuant();
	StartMainQuantFlag = 1;
	gameQuant();
#ifndef NEW_TNT
	RestoreFlagBarell();
#endif

#ifdef ISCREEN
	palTr -> set(NULL,palbufOrg,0,255);
#else
	XGR_SetPal(palbufOrg,0,255);
#endif

#ifdef _ACI_SKIP_SHOP_
	XGR_SetPal(palbuf,0,255);
#endif

	if(idOS == 1) vMap -> lockMem();
	XGR_Flush(0,0,XGR_MAXX,XGR_MAXY);
_MEM_STATISTIC_("AFTER LOADING RTO2 INIT -> ");
}

void FirstEscaveRTO::Init(int id)
{
_MEM_STATISTIC_("\nBEFORE FIRST ESCAVE RTO INIT -> ");
#ifdef ISCREEN
	CurrentWorld = 0;
	if(aciLoadLog){
		aciLoadData();
_MEM_STATISTIC_("AFTER ACILOADDATA INIT -> ");
	}
	FirstShopPrepare(aciLoadLog);
_MEM_STATISTIC_("AFTER FIRSTSHOP PREPARE INIT -> ");
#ifndef _ACI_SKIP_SHOP_
	aci_LocationQuantPrepare();
_MEM_STATISTIC_("AFTER LOADQUANT PREPARE INIT -> ");
	aciShowLocation();
_MEM_STATISTIC_("AFTER SHOWLOCATION INIT -> ");
#else
	aciPrepareMenus();
_MEM_STATISTIC_("AFTER PREPARE MENU INIT -> ");
#endif
#endif
_MEM_STATISTIC_("AFTER FIRST ESCAVE RTO INIT -> ");
}

int LoadingRTO2::Quant(void)
{
	//XGR_Flip();
	if(!(flags & RTO_QUANT_FLAG)){
		SetFlag(RTO_QUANT_FLAG);
		return 0;
	}
#ifdef ISCREEN
	return NextID;
#else
	return RTO_GAME_QUANT_ID;
#endif
}

void LoadingRTO2::Finit(void)
{
	ClearFlag(RTO_ALL_FLAGS);
_MEM_STATISTIC_("AFRET LOADING RTO2 FINIT -> ");
}

int FirstEscaveRTO::Quant(void)
{
//std::cout<<"FirstEscaveRTO::Quant"<<std::endl;

#ifdef ISCREEN
	int code;
#ifndef _ACI_SKIP_SHOP_
	code = iQuantSecond();
#else
	code = 1;
#endif
	if(code){
		if(!iScrExitCode)
			return RTO_SHOW_IMAGE_ID;

		rtoSetNextID(RTO_FIRST_ESCAVE_OUT_ID,RTO_LOADING2_ID);
		rtoSetNextID(RTO_LOADING2_ID,RTO_GAME_QUANT_ID);
		return RTO_FIRST_ESCAVE_OUT_ID;
	}
	//XGR_Flip();
	return 0;
#else
	return RTO_GAME_QUANT_ID;
#endif
}

void FirstEscaveRTO::Finit(void)
{
	if(flags & RTO_FINIT_FLAG){
		ClearFlag(RTO_ALL_FLAGS);
	}
	else {
		SetFlag(RTO_FINIT_FLAG);
	}
_MEM_STATISTIC_("AFTER FIRST ESCAVE FINIT -> ");
}

void GameQuantRTO::Init(int id)
{
	vMap -> lockHeap();
_MEM_STATISTIC_("AFTER GAME QUANT INIT -> ");
}

int GameQuantRTO::Quant(void)
{
	int ret = 0;
	if(Pause <= 1 || NetworkON){
		if(Pause) Pause++;

		gameQuant();
//		DBGCHECK
		frame++;
		if(++fps_frame == FPS_PERIOD) {
			sprintf(fps_string,"%.1f",(double)FPS_PERIOD/(SDL_GetTicks() - (int)fps_start)*1000);
#ifdef _DEBUG
			network_analysis(network_analysis_buffer,0);
#else
			if(curGMap -> prmFlag & PRM_FPS && NetworkON)
				short_network_analysis(network_analysis_buffer);
#endif
			fps_frame = 0;
			fps_start = SDL_GetTicks();


			}
		if(Redraw){
			XGR_Flush(0,0,XGR_MAXX,XGR_MAXY);
			Redraw = 0;
			frame = 0;
			_Timer_ = SDL_GetTicks();
			fps_start = SDL_GetTicks();
			}
//		if(!PalIterLock && !palTr -> quant()) pal_iter();
		if(!PalIterLock) PalCD.Quant();
		if(vSoundOFF){ vSoundOFF = 0; EffectsOff(); }
		if(vSoundON){ vSoundON = 0; EffectsOn(); }
		}
	else {
		if(GameQuantReturnValue || acsQuant()){
			Pause = 0;
		}
	}

	ret = GameQuantReturnValue;
	GameQuantReturnValue = 0;
	//XGR_Flip();
	return ret;
}

void EscaveRTO::Init(int id)
{
#ifdef ISCREEN
	uvsPrepareQuant();
	aci_LocationQuantPrepare();
	aciShowLocation();
#endif
_MEM_STATISTIC_("AFTER ESCAVE RTO INIT -> ");
}

int EscaveRTO::Quant(void)
{

#ifdef ISCREEN

	if(iQuantSecond()){
		if(!iScrExitCode)
			return RTO_SHOW_IMAGE_ID;

		rtoSetNextID(RTO_ESCAVE_OUT_ID,RTO_GAME_QUANT_ID);
		return RTO_ESCAVE_OUT_ID;
	}
	//XGR_Flip();
	return 0;
#else
	return RTO_GAME_QUANT_ID;
#endif
}

void EscaveRTO::Finit(void)
{
#ifdef ISCREEN
	if(flags & RTO_FINIT_FLAG){
		ClearFlag(RTO_ALL_FLAGS);
	}
	else {
		SetFlag(RTO_FINIT_FLAG);
	}
#endif
_MEM_STATISTIC_("AFTER ESCAVE RTO FINIT -> ");
#ifdef CDCHECK
	int mask = 1;
	int res = 0;
	int getCDdrives(void);
	int isCDok(int);
	int d = getCDdrives();
	for(int ii = 0;ii < 26;ii++,mask <<= 1)
		if(d & mask) res += isCDok(ii);
	if(!res) ErrH.Abort(AVInotFoundMSS);
#endif
}

int PaletteTransformRTO::Quant(void)
{
	int val = palTr -> quant();
	if(!val) return NextID;
	//XGR_Flip();
	return 0;
}

void LoadingRTO3::Init(int id)
{
	ShowImageRTO* p = (ShowImageRTO*)xtGetRuntimeObject(RTO_SHOW_IMAGE_ID);
	p -> SetNumFiles(1);

	StopCDTRACK();

#ifdef MEMORY_STATISTICS
	memStart = 0;
#endif

#ifdef ACTINT
	XGR_MouseHide();
	extern int actIntLog;
	if(!actIntLog){
		palTr -> set(palbuf,NULL,0,255,&Quit);
		Quit = 1;
		int cnt = CLOCK();
		while(Quit){
			if(CLOCK() != cnt){
				palTr -> quant();
				cnt = CLOCK();
				xtClearMessageQueue();
				}
			}
		}
#endif

	aciFreeUVS();
	FreeAll(iAbortGameMode);
	iAbortGameMode = 1;
	LocalEffectFree();
	iAbortGameFlag = 0;
#ifdef ACTINT

	aciFreeAll();
#endif

	SetNext(RTO_SHOW_IMAGE_ID);
	rtoSetNextID(RTO_SHOW_IMAGE_ID,RTO_MAIN_MENU_ID);

	GameOverTrigger = GameOverID;
	aciInitEndGame(GameOverID);
}

int LoadingRTO3::Quant(void)
{
	return NextID;
}

void LoadingRTO3::Finit(void)
{
	EffectsOff();

	/*TODO stalkerg
	if(XGR_MAXX != 800){
		if(XGR_ReInit(800,600,emode)) ErrH.Abort(ErrorVideoMss);
	}*/
}

void xtDoneApplication(void)
{
	restore();
}

void restore(void)
{
	KDWIN::destroy_server();
#ifdef _DEBUG
	network_analysis(network_analysis_buffer,1);
	fout < network_analysis_buffer.address();
#endif
#ifdef MEMORY_STATISTICS
	memStart = 0;
#endif
	RestoreSOUND();
	SDL_Quit();

//	  win32_dump_mem();

//	char buf[20];
//	sprintf(buf,"%.3f",(double)frame/((int)SDL_GetTicks() - (int)_Timer_)*1000);
//	XCon < buf < " average fps\n";

//	delete sqr3_matr;
}

extern int light_modulation;
extern int WorldLightParam[WORLD_MAX][3];
extern int CurrentWorldLightParam;

void PalettePrepare(void) {
	std::cout<<"PalettePrepare"<<std::endl;
//	palbufC = new unsigned char[768];
//	palbufA = new unsigned char[768];
//	palbufOrg = new unsigned char[768];
	palbuf = palbufA;
	int k;
	memset(palbuf,0,768);
	int i,j;
	memset(palbufC,0,768);

	if(CurrentWorld < MAIN_WORLD_MAX - 1){
		WorldPalCurrent = WorldTable[CurrentWorld]->escT[0]->Pbunch->currentStage;
		light_modulation  = WorldLightParam[CurrentWorld][WorldPalCurrent];
		memcpy(palbufC,WorldPalData[WorldPalCurrent],768);
	}else{
		WorldPalCurrent = 0;
		light_modulation  = WorldLightParam[CurrentWorld][0];
		XStream fpal(GetTargetName(vMap -> paletteName),XS_IN);
		fpal.read(palbufC,768);
		fpal.close();
	};

	XStream ffp("resource/pal/objects.pal",XS_IN);
	ffp.seek(3*(ENDCOLOR[TERRAIN_MAX - 1] + 1),XS_BEG);
	ffp.read(palbufC + 3*(ENDCOLOR[TERRAIN_MAX - 1] + 1),768 - 3 - 3*(ENDCOLOR[TERRAIN_MAX - 1]));
	ffp.close();

	palbufC[0] = palbufC[1] = palbufC[2] = 0;
//	palbufC[3*BEGCOLOR[1]] = palbufC[3*BEGCOLOR[1] + 1] = palbufC[3*BEGCOLOR[1] + 2] = 1;

	for(i = 0;i < TERRAIN_MAX;i++){
		palbufC[3*BEGCOLOR[i] + 0] >>= 1;
		palbufC[3*BEGCOLOR[i] + 1] >>= 1;
		palbufC[3*BEGCOLOR[i] + 2] >>= 1;
		}

	palbuf = palbufC;

	for(i = 0;i < 16;i++) palbuf[3*(224 + i) + 0] = palbuf[3*(224 + i) + 1] = palbuf[3*(224 + i) + 2] = i*4; //(uchar)(10 + (pow((double)i/16.,.70)*UNIT)*54/UNIT);
	memcpy(palbufOrg,palbuf,768);

#if defined(ISCREEN) && defined(ACTINT)
//	memcpy(palbuf + 0*32*3,palbufOrg + 32*3,32*3);
//	memcpy(palbuf + 2*32*3,palbufOrg + 32*3,32*3);
//	memcpy(palbuf + 3*32*3,palbufOrg + 32*3,32*3);
//	memset(palbuf + 0*32*3,0,32*3);
//	memset(palbuf + 2*32*3,0,32*3);
//	memset(palbuf + 3*32*3,0,32*3);

	if(palbufInt) {
		delete[] palbufInt;
	}
	palbufInt = new uchar[768];
	uchar r = palbufOrg[3*(32 + 10) + 0];
	uchar g = palbufOrg[3*(32 + 10) + 1];
	uchar b = palbufOrg[3*(32 + 10) + 2];
	for(i = 0;i < 256;i++){
		palbufInt[i*3 + 0] = r;
		palbufInt[i*3 + 1] = g;
		palbufInt[i*3 + 2] = b;
		}
#endif
	if(CurrentWorld < MAIN_WORLD_MAX - 1) {
		for(k = 0;k < WorldPalNum;k++)	memcpy(WorldPalData[k] + 128*3,palbufOrg + 128*3,128*3);
	};

	if(CurrentWorld == WORLD_XPLO && ActD.SpobsDestroy){
		for(i = BEGCOLOR[SPOBS_PAL_TERRAIN]*3;i <= ENDCOLOR[SPOBS_PAL_TERRAIN]*3;i++)
			palbuf[i] = palbufSrc[i] = palbufOrg[i] = 5 * (i - BEGCOLOR[SPOBS_PAL_TERRAIN]*3) / (3*(ENDCOLOR[SPOBS_PAL_TERRAIN] - BEGCOLOR[SPOBS_PAL_TERRAIN]));
	};

	if(CurrentWorld == WORLD_THREALL && ActD.ThreallDestroy){
		for(j = 0;j < TERRAIN_MAX;j++){
			if(j == 3 || j == 5 || j == 6 || j == 4) break;
			for(i = BEGCOLOR[j];i <= ENDCOLOR[j];i++){
				palbufSrc[i*3] = palbufOrg[i*3] = 63 * (i - BEGCOLOR[j]) / (ENDCOLOR[j] - BEGCOLOR[j]);
				palbufSrc[i*3 + 1] = palbufOrg[i*3 + 1] = 63 *(i - BEGCOLOR[j]) / (ENDCOLOR[j] - BEGCOLOR[j]);
				palbufSrc[i*3 + 2] = palbufOrg[i*3 + 2] = 63 * (i - BEGCOLOR[j]) / (ENDCOLOR[j] - BEGCOLOR[j]);
			};
		};
	};
};

void ComlineAnalyze(int argc,char** argv)
{
	int i,j;
	for(i = 1;i < argc;i++)
		if(argv[i][0] == '/'){
			j = 0;
			while(argv[i][j] == '/' || argv[i][j] == '-'){
				switch(argv[i][j + 1]){
#ifdef zRECORDER_ENABLED
					case 'W':
					case 'w':
						RecorderMode = XRC_RECORD_MODE;
						RecorderName = argv[i] + 2;
						SkipIntro = 1;
						break;
					case 'P':
					case 'p':
						RecorderMode = XRC_PLAY_MODE;
						RecorderName = argv[i] + 2;
						SkipIntro = 1;
						break;
#endif
					case 'X':
					case 'x':
						ExclusiveLog = 1;
						DIBLog = 0;
						break;
					case 'M':
					case 'm':
						MuteLog = 1;
						break;
					case 'U':
					case 'u':
						MMXsuppress = 1;
						break;
					case 'H':
					case 'h':
						inHighPriority = 1;
						break;
					case 'A':
					case 'a':
						DIBLog = 1;
						ExclusiveLog = 0;
						break;
					case 't':
						setTimerLog = 0;
//						speed_correction_enabled = 1;
						break;

#ifdef _DEMO_
						if(argv[i][j + 2] == 'e' && argv[i][j + 3] == 'e' && argv[i][j + 4] == 'b' && argv[i][j + 5] == 'o' && argv[i][j + 6] == 's' && argv[i][j + 7] == '$') beebos = 100000;
#else
						if(argv[i][j + 2] == 'E' && argv[i][j + 3] == 'E' && argv[i][j + 4] == 'b' && argv[i][j + 5] == 'o' && argv[i][j + 6] == 's' && argv[i][j + 7] == '!') beebos = 500000;
#endif
						break;
					case 'R':
					case 'r':
						if(!strcmp(argv[i] + 1,"RUSSIAN") || !strcmp(argv[i] + 1,"russian")){
							dgRUSSIAN = 1;
							iRussian = 1;
						}
						break;
					case '&':
						if(argv[i][j + 2] == '^') SkipCD = 1;
						break;
					case '0':
						videoMode = 0;
						break;
					case '1':
						videoMode = 1;
						break;
/* video 1024 not working correctly */
					case '2':
						videoMode = 2;
						break;
/* */
#ifdef _DEBUG
					case 'q':
						host_port = atoi(argv[i] + (j + 2));
						break;
					case 'N':
						extern char* iProxyServer;
						iProxyServer = argv[i] + (j + 2);
						break;
					case 'n':
						network_log = 1;
						//message_mode = 2;
						host_name = argv[i] + (j + 2);
						break;

					case 'G':
					case 'g':
						WorldPrm = atoi(argv[i] + j + 2);
						j += strlen(argv[i] + j + 2);
						break;
					case 'S':
					case 's':
						if(!stricmp(argv[i] + 1,"SKIPINTRO"))
							SkipIntro = 1;
						else
							NumHumanModel = atoi(argv[i] + j + 2) + 1;
						break;
					case 'I':
					case 'i':
						TimerMode = 1;
						break;
#else
					case 'S':
					case 's':
						if(!strcasecmp(argv[i] + 1,"SKIPINTRO")) {
						    std::cout<<"Skip intro\n";
							SkipIntro = 1;
                        }
						break;
#endif
					}
				j+=2;
				}
			}
		else
			//mapFName = strupr(strdup(argv[i]));
			mapFName = strdup(argv[i]);
}

void costab(void)
{
	SI = new int[PIx2];
	CO = new int[PIx2];
	for(int i = 0;i < PIx2;i++){
		SI[i] = round(UNIT*sin(GTOR(i)));
		CO[i] = round(UNIT*cos(GTOR(i)));
		}
}

//Render poster by stalkerg
extern void camera_quant(int X,int Y,int Turn,double V_abs);
void creat_poster_pixels_copy(SDL_Surface *src, SDL_Rect *srcrect, SDL_Surface *dst, SDL_Rect *dstrect) {
	int x,y, src_x, src_y, dst_x, dst_y;
	for (y = 0;y < srcrect->h; y++) {
		for (x = 0;x < srcrect->w; x++) {
			dst_y = dstrect->y+y;
			dst_x = dstrect->x+x;
			src_y = srcrect->y+y;
			src_x = srcrect->x+x;
			if (dst_y<dst->h&&dst_x<dst->w&&src_y<src->h&&src_x<src->w) {
				((unsigned char*)dst->pixels)[dst_y*dst->w+dst_x] = 
					((unsigned char*)src->pixels)[src_y*src->w+src_x];
			}
		}
	}
}
void creat_poster() {
	int iter, iter2;
	
	SDL_Surface *surface;
	SDL_Rect srcrect, dstrect;
	srcrect.w = 256;
	srcrect.h = 256;
 	srcrect.x = curGMap->xside-128;
 	srcrect.y = curGMap->yside-128;
	dstrect.w = 256;
	dstrect.h = 256;
	
	
    surface = SDL_CreateRGBSurface(0, map_size_x, map_size_y, 8,
		0, 0, 0, 0);
	surface->format = XGR_Obj.XGR_ScreenSurface->format;
	
	for (iter=0; iter<map_size_y/256;iter++) {
		for (iter2=0; iter2<map_size_x/256;iter2++) {
			
			dstrect.x = 256*iter2;
			dstrect.y = 256*iter;
			std::cout<<"dstrect.x:"<<dstrect.x<<" dstrect.y:"<<dstrect.y<<std::endl;
			
			camera_quant(256*iter2, 256*iter, 0, 0);
			/*actIntQuant();
			uvsQuant();
			BackD.restore();
			MLquant();*/
			//std::cout<<"TurnSecX:"<<TurnSecX<<" ViewX:"<<ViewX<<" ViewY:"<<ViewY<<" curGMap->xc:"<<curGMap->xc<<" curGMap->yc:"<<curGMap->yc
			//<<" curGMap->xside:"<<curGMap->xside<<" curGMap->yside:"<<curGMap->yside<<std::endl;
			vMap -> scaling(TurnSecX,ViewX,ViewY,curGMap->xc,curGMap->yc,curGMap->xside,curGMap->yside);
			
			creat_poster_pixels_copy(XGR_Obj.XGR_ScreenSurface, &srcrect, surface, &dstrect);
		}
	}

	SDL_SaveBMP(surface, "./poster.bmp");
}

void KeyCenter(SDL_Event *key)
{
	extern int entry_scan_code;
	SDL_Keymod mod;

	if(aciKeyboardLocked) {
#ifdef ACTINT
		aKeyTrap(key);
#endif
		return;
	}

	entry_scan_code = sdlEventToCode(key);
	switch(entry_scan_code) {
		case SDL_SCANCODE_ESCAPE:
#ifdef ESCAPE_EXIT
			disconnect_from_server();
			ErrH.Exit();
#endif
			std::cout<<"road.KeyCenter:"<<key<<std::endl;
			if(!Pause)
				Pause = 1;
//				  GameQuantReturnValue = RTO_LOADING3_ID;
			break;
#ifndef ACTINT
		case SDL_SCANCODE_F1:
			curGMap -> change(-3,-2);
			break;
		case SDL_SCANCODE_F2:
			curGMap -> change(3,2);
			break;
#endif
#ifdef _DEBUG
		case SDL_SCANCODE_F12:
			DBGCHECK
			break;
#endif
#ifdef SCREENSHOT
//  		case SDL_SCANCODE_F4:
//  			creat_poster();
//  			break;
		case SDL_SCANCODE_F11:
			shotFlush();
			break;
#endif
		case SDL_SCANCODE_T:
			mod = SDL_GetModState();
			if ((mod&KMOD_SHIFT)||(mod&KMOD_CTRL)) {
				GameTimerON_OFF();
			}
			break;
		case SDL_SCANCODE_F:
			mod = SDL_GetModState();
			if (mod&KMOD_CTRL) {
				curGMap -> prmFlag ^= PRM_FPS;
			}
#ifdef _DEBUG
			else
				message_mode++;
#endif
			break;
		case SDL_SCANCODE_F5:
			if(!Pause){
				camera_rotate_enable = 1 - camera_rotate_enable;
				aciSetCameraMenu();
			}
			break;
		case SDL_SCANCODE_F6:
			if(!Pause){
				camera_moving_xy_enable = 1 - camera_moving_xy_enable;
				camera_moving_z_enable = 1 - camera_moving_z_enable;
				aciSetCameraMenu();
			}
			break;
		case SDL_SCANCODE_F7:
			if(!Pause){
				camera_slope_enable = 1 - camera_slope_enable;
				aciSetCameraMenu();
			}
			break;
		}
	
	if (iKeyPressed(iKEY_ZOOM_IN)) {
		if(!Pause){
			if((camera_zmin -= 8) < curGMap -> xsize*MIN_ZOOM >> 8)
				camera_zmin = curGMap -> xsize*MIN_ZOOM >> 8;
		}
	}
	if (iKeyPressed(iKEY_ZOOM_OUT)) {
		if(!Pause){
			if((camera_zmin += 8) > curGMap -> xsize*MAX_ZOOM >> 8)
				camera_zmin = curGMap -> xsize*MAX_ZOOM >> 8;
		}
	}
	if (iKeyPressed(iKEY_ZOOM_STANDART)) {
		if(!Pause)
			camera_zmin = curGMap -> xsize;
	}
#ifdef ACTINT
	aKeyTrap(key);
#endif
}

iGameMap::iGameMap(int _x,int _y,int _xside,int _yside)
{
	xside = _xside;
	yside = _yside;
	xsize = 2*xside/* + 1*/;
	ysize = 2*yside/* + 1*/;

	sy = ysize;
	sx = xsize;

	_xI = _x; _yI = _y;
	x = _x - sx/2; y = _y - sy/2;

	xc = x + sx/2;
	yc = y + sy/2;

	ScreenCX = xc;
	ScreenCY = yc;

	UcutLeft = xc - xside;
	UcutRight = xc + xside;
	VcutUp = yc - yside;
	VcutDown = yc + yside;

//	  focus = (xside*540)/180;
	focus_flt = focus = 512;

/*
	if(x < getX() + 4) x = getX() + 4;
	if(y < getY() + 2) y = getY() + 2;
	if(x + sx + 2 >= getRX()) x = getRX() - sx - 2;
	if(y + sy + 4 >= getRY()) y = getRY() - sy - 4;
*/
	reset();

//	draw(1);
//	prmFlag |= PRM_INFO;
}

void iGameMap::change(int Dx,int Dy,int mode,int xcenter,int ycenter)
{
	std::cout<<"iGameMap::change"<<std::endl;
	int xsize_old = xsize;
	if(mode) {
		if(xside + Dx > XGR_MAXX/2 - 2 || xside + Dx < 100) {
			return;
		}
		if(yside + Dy > XGR_MAXY/2 - 2 || yside + Dy < 100) {
			return;
		}

		xside += Dx;
		yside += Dy;
	} else {
		xside = Dx;
		yside = Dy;
	}

	xsize = 2*xside + 0;
	ysize = 2*yside + 0;

	sy = ysize;
	sx = xsize;

	if(mode) {
		x = _xI - sx/2;
		y = _yI - sy/2;
	} else {
		x = xcenter - xside;
		y = ycenter - yside;
	}

	xc = x + sx/2;
	yc = y + sy/2;

	ScreenCX = xc;
	ScreenCY = yc;

	UcutLeft = xc - xside;
	UcutRight = xc + xside;
	VcutUp = yc - yside;
	VcutDown = yc + yside;

	camera_zmin = TurnSecX = TurnSecX*xsize/xsize_old;
	calc_view_factors();

/*
	if(x < getX() + 4) x = getX() + 4;
	if(y < getY() + 2) y = getY() + 2;
	if(x + sx + 2 >= getRX()) x = getRX() - sx - 2;
	if(y + sy + 4 >= getRY()) y = getRY() - sy - 4;
*/
	FirstDraw = 1;
	Redraw = 1;
}

void iGameMap::reset(void)
{
	std::cout<<"iGameMap::reset"<<std::endl;
//	ViewX = PlayerInitData.x;
//	ViewY = PlayerInitData.y;

//	ViewX = XCYCL(854);
//	ViewY = YCYCL(14826);

	ViewZ=focus;
	TurnAngle = 0;
	prmFlag = 0;
	SlopeAngle = 0;// -Pi/4;
	DepthShow = 0;
	camera_zmin = TurnSecX = xsize;
	TurnSecY = ysize;
	TurnSideX = TurnSecX >> 1;
	TurnSideY = TurnSecY >> 1;
	ScaleMapFlt = 1;
	ScaleMapInvFlt = 1;
	ScaleMap = 256;
	ScaleMapInv = 256;

	if(ViewX == 0 && ViewY == 0) {
		ViewX = XCYCL(1517);
		ViewY = YCYCL(15879);
		vMap -> accept(ViewY,ViewY);
	} else {
		vMap -> accept(ViewY,ViewY);
		GeneralLoadReleaseFlag = 1;
	}
	camera_reset();
}

void calc_view_factors()
{

	//if(!(XRec.flags & (XRC_RECORD_MODE | XRC_PLAY_MODE)) && prev_frame_time)
	//Stalkerg
	if((speed_correction_enabled | NetworkON) && prev_frame_time){
		int dt = SDL_GetTicks() - prev_frame_time;
		if(dt > 15 && dt < 200) {
			speed_correction_factor = (double)dt*((double)STANDART_FRAME_RATE/1000.)*speed_correction_tau + speed_correction_factor*(1 - speed_correction_tau);
		}
	} else {
		speed_correction_factor = 1;
	}
	//speed_correction_factor = 1;
	//std::cout<<"DT::"<<SDL_GetTicks() - prev_frame_time<<std::endl;

	prev_frame_time = SDL_GetTicks();

	cycleTor(ViewX,ViewY);
	TurnSecY = curGMap -> ysize*TurnSecX/curGMap -> xsize;
	TurnSideX = TurnSecX >> 1;
	TurnSideY = TurnSecY >> 1;
	ViewZ = focus*TurnSecX/curGMap -> xsize;
	ScaleMapFlt = (double)TurnSecX/curGMap -> xsize;
	ScaleMapInvFlt = (double)curGMap -> xsize/TurnSecX;
	ScaleMap = round(ScaleMapFlt*256);
	ScaleMapInv = round(ScaleMapInvFlt*256);
	DepthShow = SlopeAngle < -PI/512;

	ModulatedNORMAL = (double)NORMAL*light_modulation/256.;
	Light = Vector(DBV(ModulatedNORMAL,0,ModulatedNORMAL*SHADOWDEEP/256.).norm(ModulatedNORMAL));

	CurrShadowDirection =  DBV(-Cos(TurnAngle),-Sin(TurnAngle),(double)SHADOWDEEP/256.);

	if(DepthShow){
		A_g2s = DBM(SlopeAngle,X_AXIS)*DBM(TurnAngle,Z_AXIS);
		A_s2g = DBM(-TurnAngle,Z_AXIS)*DBM(-SlopeAngle,X_AXIS);
	} else {
		A_g2s = DBM(TurnAngle,Z_AXIS);
		A_s2g = DBM(-TurnAngle,Z_AXIS);
	}

	A_g2sZ = A_g2s*DBM(1,1,-1,DIAGONAL);

	sinTurnInvFlt = sinTurnFlt = Sin(TurnAngle);
	cosTurnInvFlt =cosTurnFlt = Cos(TurnAngle);
	sinTurnFlt *= ScaleMapInvFlt;
	cosTurnFlt *= ScaleMapInvFlt;
	sinTurnInvFlt *= ScaleMapFlt;
	cosTurnInvFlt *= ScaleMapFlt;

	sinTurn = round(UNIT*sinTurnFlt);
	cosTurn = round(UNIT*cosTurnFlt);
	sinTurnInv = round(UNIT*sinTurnInvFlt);
	cosTurnInv = round(UNIT*cosTurnInvFlt);

	// Inverse factors calculation
	double M_x = A_g2s[0]*focus_flt;
	double N_x = A_g2s[1]*focus_flt;
	double M_y = A_g2s[3]*focus_flt;
	double N_y = A_g2s[4]*focus_flt;
	double M_z = A_g2s[6];
	double N_z = A_g2s[7];
	double H = ViewZ << 16;

	Ha = N_y*H;
	Va = -N_x*H;

	Hb = -M_y*H;
	Vb = M_x*H;

	Oc = M_x*N_y - M_y*N_x;
	Hc = M_y*N_z - M_z*N_y;
	Vc = M_z*N_x - M_x*N_z;
}

void gameQuant(void)
{
	curGMap -> draw(1);
	if(loadingStatus) LoadingMessage();
	curGMap -> flush();
}

void iGameMap::flush()
{
	XGR_Flush(xc - xside,yc - yside,xsize,ysize);
}

void iGameMap::draw(int self)
{
	static XBuffer status;
	static int blink,clcnt;
	
	if(!MuteLog && ((ConTimer.counter&7) == 0)) {
		SoundQuant();
	}

	if(GeneralSystemSkip) {
		actIntQuant();
	}
	
	uvsQuant();

	if(GeneralSystemSkip && !ChangeWorldSkipQuant){
		if(curGMap) {
			BackD.restore();
			MLquant();
			//try {
				GameD.Quant();
			/*} catch (...) {
				std::cout<<"ERROR:Some GameD.Quant is error."<<std::endl;
			}*/
			
		}

		if(DepthShow) {
			if(SkipShow) {
				//Наклон изображения
				vMap -> SlopTurnSkip(TurnAngle,SlopeAngle,ViewZ,focus,ViewX,ViewY,xc,yc,xsize/2,ysize/2);
			} else {
				vMap -> scaling_3D(A_g2s,ViewZ,focus,ViewX,ViewY,xc,yc,xside,yside,TurnAngle);
			}
		} else {
			if(TurnAngle) {
				//Вращение
				vMap -> turning(TurnSecX,-TurnAngle,ViewX,ViewY,xc,yc,xside,yside);
			} else {
				vMap -> scaling(TurnSecX,ViewX,ViewY,xc,yc,xside,yside);
			}
		}
		
		
		//Отрисовка 3д моделей
		if(curGMap) {
			GameD.DrawQuant();
		}

	/*
		if(recorder_mode){
			if(blink){
				c_rectangle(xc + xside - 80 - 2,yc - yside + 60 - 2,8 + 6,16 + 4,224 + 15,224 + 15,OUTLINED);
				sysfont.draw(xc + xside - 80,yc - yside + 60,(unsigned char*)(recorder_mode == RECORDER_RECORD ? "W" : "R"),224 + 15,-1);
				}
			if(CLOCK() - clcnt > 10){
				blink = 1 - blink;
				clcnt = CLOCK();
				}
			}
	*/


	if(!FirstDraw) {
		//zmod chat onscreen
		if (message_dispatcher.ListSize>0 && message_dispatcher.new_messages) {
			XBuffer zChat;
			int zColor = zCOLOR_WHITE;
			int zCount = 0;
			MessageElement* msg = message_dispatcher.last();
   			while (msg && zCount<zCHAT_ROWLIMIT && msg->time+zCHAT_TIMELIMIT>SDL_GetTicks()) {
				zCount++;
				switch(msg->color) {
					case 0:	zColor = zCOLOR_GREEN;	break;
					case 1:	zColor = zCOLOR_ORANGE;	break;
					case 2:	zColor = zCOLOR_BLUE;	break;
					case 3:	zColor = zCOLOR_YELLOW;	break;
					default:zColor = zCOLOR_WHITE;
				}
				if (msg->message[0]=='$' && msg->message[1]==':')
					zColor = zCOLOR_RED;

				zChat.init();
				zChat < msg->message;
				zchatfont.draw(
					xc-xside+80,
					yc-yside+20+(zCHAT_ROWLIMIT*zCHAT_ROWHEIGHT)-(zCount*zCHAT_ROWHEIGHT),
					(unsigned char*)(zChat.GetBuf()),
					zColor, 
					zCOLOR_TRANSPARENT
				);

				if(msg == message_dispatcher.first()) break;
				msg = (MessageElement*)msg->prev;
  			}

			message_dispatcher.new_messages = zCount;
		}

		// All Debug Messages should be placed here
		if(prmFlag & PRM_FPS) {
			sysfont.draw(xc + xside - 150,yc - yside + 80,(unsigned char*)fps_string,224 + 15,-1);
			status.init();
			status <= ViewX < " " <= ViewY/* < ":" <= ActD.NumResolve*/;
			sysfont.draw(xc + xside - 150,yc - yside + 96,(unsigned char*)status.GetBuf(),224 + 15,-1);
			if(NetworkON)
				sysfont.drawtext(xc - xside + 3,yc - yside + 32,network_analysis_buffer.address(),255,-1);
			#ifdef SICHER_DEBUG
				status.init();
				status < "Speed_correction_factor: " <= speed_correction_factor < "\n";
				sysfont.drawtext(xc - xside + 3,yc + yside - 60,status.address(),255,-1);
			#endif
		}

		switch(message_mode % 3){
			case 0:
				break;

			case 1:
				status.init();
				if(ActD.Active){
					status <= ActD.Active->R_curr.x < "," <= ActD.Active->R_curr.y < "," <= ActD.Active->R_curr.z < ";" <= ActD.Active->radius;
					sysfont.draw(xc - xside + 3,yc - yside + 3,(unsigned char*)status.GetBuf(),224 + 15,-1);
					sysfont.draw(xc + xside - 80,yc - yside + 3,(unsigned char*)fps_string,224 + 15,-1);
					status.init();
					status <= (ActD.UnitStorage[ACTION_VANGER].Max - ActD.UnitStorage[ACTION_VANGER].ObjectPointer);
					sysfont.draw(UcutRight - 200,VcutDown - 70,(unsigned char*)status.GetBuf(),224 + 15,-1);
					}
				sysfont.drawtext(xc - xside + 3,yc - yside + 32,msg_buf.address(),255,-1);
				break;

			case 2:
				if(!NetworkON)
					message_mode = 0;
				status.init();
				status.SetDigits(8);
				status < "Frame: "      <= frame < "\n";
				status < "Time: " <= (double)GLOBAL_CLOCK()/256. < " sec\n";
				sysfont.drawtext(xc + xside - 120,yc - yside + 32,status.address(),255,-1);
				sysfont.drawtext(xc - xside + 3,yc - yside + 32,network_analysis_buffer.address(),255,-1);
				break;
			}
		}

		msg_buf.init();

		if(StandLog) {
			ScreenQuant();
		}

		FirstDraw = 0;
//2D Rendring in game.
#ifdef ACTINT
		//XGR_Obj.set_render_buffer(XGR_Obj.XGR_ScreenSurface2D);
		//XGR_Obj.fill(2);
		if(GeneralSystemSkip) {
			aScrDisp -> redraw();
		}
		aScrDisp -> flush();
		//aScrDisp->pal_flush();
		//XGR_Obj.set_render_buffer(XGR_Obj.XGR_ScreenSurface);
#endif
	};
	
}

void preCALC(void)
{
//	precalc_sqr3_matr();
}

void PrintError(char* error,char* msg)
{
	std::cout<<"--------------------------------\n";
	std::cout<<error<<"\n";
	std::cout<<msg<<"\n";
	std::cout<<"--------------------------------\n";
}

/*
void PaletteTransform::set(uchar* _from,uchar* _to,int _n0,int _sz,int* _event)
{
	n0 = _n0;
	sz = _sz;
	event = _event;
	if(!_from)
		memset(from + 3*n0,0,3*sz);
	else
		memcpy(from + 3*n0,_from + 3*n0,3*sz);
	if(!_to)
		memset(to + 3*n0,0,3*sz);
	else
		memcpy(to + 3*n0,_to + 3*n0,3*sz);
	memset(delta + 3*n0,0,3*sz);
	int i,j;
	for(i = 0,j = 3*n0;i < 3*sz;i++,j++){
		if(to[j] > from[j]) delta[j] = 2 + (((int)to[j] - (int)from[j]) >> 5);
		else if(to[j] < from[j]) delta[j] = -2 - (((int)from[j] - (int)to[j]) >> 5);
		}
	inProgress = 1;
}

void PaletteTransform::quant(void)
{
	if(!inProgress) return;

	uchar* pf = from + 3*n0;
	uchar* pt = to + 3*n0;
	char* pd = delta + 3*n0;
	int max = 3*sz;
	int log = 0;
	while(max--){
		if(*pd){
			log = 1;
			if(abs((int)*pf - (int)*pt) <= abs(*pd))
				*pf = *pt, *pd = 0;
			else
				*pf += *pd;
			}
		pf++; pt++; pd++;
		}
	if(!log){ inProgress = 0; if(event) *event = 0; }
	XGR_SetPal(from,n0,sz);
}
*/

void ShowImageRTO::Init(int id)
{
#ifdef SHOW_IMAGES
	int i;
	short sx,sy;

	XStream fh(0);
	XBuffer* XBuf = new XBuffer(1024);

	unsigned char* buf;
	char* pname;

	//NEED SEE
	set_key_nadlers(&ShowImageKeyPress, NULL);
	XGR_MouseSetPressHandler(XGM_LEFT_BUTTON, ShowImageMousePress);
	XGR_MouseSetPressHandler(XGM_RIGHT_BUTTON, ShowImageMousePress);

	XBuf -> init();
	if(!(Flags[curFile] & IMG_RTO_NO_IMAGE)){
		if(Flags[curFile] & IMG_RTO_CD_IMAGE){
			*XBuf < iVideoPathDefault < fileNames[curFile];

			if(!fh.open(XBuf -> address(),XS_IN)){
				XBuf -> init();
				*XBuf < iVideoPath < fileNames[curFile];
				if(!fh.open(XBuf -> address(),XS_IN)){
					Flags[curFile] |= IMG_RTO_NO_IMAGE;
				}
				else
					fh.close();
			}
			else
				fh.close();
		}
		else
			*XBuf < fileNames[curFile];
	}

	pname = strdup(XBuf -> address());
	memcpy(pname + strlen(pname) - 3,"pal",3);

	char* pal = new char[768];
	char* tpal = new char[768];
	memset(tpal,0,768);

	XStream ff(pname,XS_IN);
	ff.read(pal,768);
	ff.close();

	XGR_SetPal(tpal,0,255);

	if(!(Flags[curFile] & IMG_RTO_NO_IMAGE)){
		ff.open(XBuf -> address(),XS_IN);
		ff > sx > sy;

		if(!(Flags[curFile] & IMG_RTO_KEEP_IMAGE)){
			for(i = 0; i < sy; i ++){
				buf = XGR_GetVideoLine(i);
				ff.read(buf,sx);
			}
			imageData = NULL;
		}
		else {
			imageSX = sx;
			imageSY = sy;
			imageData = new char[sx * sy];
			ff.read(imageData,sx * sy);
			XGR_PutSpr(0,0,sx,sy,imageData,XGR_BLACK_FON);
		}
		ff.close();
	}
	else {
		XGR_Fill(0);
	}
	XGR_Flush(0,0,XGR_MAXX,XGR_MAXY);

	PalEvidence(tpal,pal);

	count = CLOCK();

	delete[] pal;
	delete[] tpal;
	free(pname);

	delete XBuf;

	ShowImageMouseFlag = 0;
	ShowImageKeyFlag = 0;

	switch(GameOverTrigger){
		case GAME_OVER_EXPLOSION:
			LastStartWTRACK(ST_GAMEOVER);
			break;
		case GAME_OVER_INFERNAL_LOCKED:
			LastStartWTRACK(ST_THEEND);
			break;
		case GAME_OVER_SPOBS_LOCKED:
			LastStartWTRACK(ST_THEEND);
			break;
		case GAME_OVER_ALL_LOCKED:
			LastStartWTRACK(ST_THEEND_DOUBLE);
			break;
		case GAME_OVER_LUCKY:
			LastStartWTRACK(ST_INTRO);
			break;
		case GAME_OVER_LAMPASSO:
			LastStartWTRACK(ST_GAMEOVER);
			break;
		case GAME_OVER_ABORT:
			LastStartWTRACK(ST_INTRO);
			break;
		case GAME_OVER_NETWORK:
			break;
		}
	GameOverTrigger = -1;
#endif
_MEM_STATISTIC_("AFTER SHOW IMAGE RTO INIT -> ");
}


void ShowAviRTO::Init(int id)
{
#ifdef SHOW_LOGOS



/*
	int x,y,sx,sy;

	XBuffer* XBuf = new XBuffer(1024);
	char* avi_pal,*pal,*tpal;

	if(!(Flags[curFile] & AVI_RTO_HICOLOR)){
		pal = new char[768];
		tpal = new char[768];
		memset(tpal,0,768);
	}

	set_key_nadlers(ShowImageKeyPress,NULL);
	XGR_MouseSetPressHandler(XGM_LEFT_BUTTON,ShowImageMousePress);
	XGR_MouseSetPressHandler(XGM_RIGHT_BUTTON,ShowImageMousePress);

	XBuf -> init();
	*XBuf < iVideoPathDefault < fileNames[curFile];

//	OLD XTOOL
//	if(!AVIopen(XBuf -> address(),AVI_NOTIMER | AVI_NODRAW | AVI_NOPALETTE | AVI_NO_SOUND,0,&aviBuf)){
	if(!AVIopen(XBuf -> address(),AVI_NOTIMER | AVI_NODRAW | AVI_NOPALETTE,0,&aviBuf)){
		XBuf -> init();
		*XBuf < iVideoPath < fileNames[curFile];
//		OLD XTOOL
//		if(!AVIopen(XBuf -> address(),AVI_NOTIMER | AVI_NODRAW | AVI_NOPALETTE | AVI_NO_SOUND,0,&aviBuf)){
		if(!AVIopen(XBuf -> address(),AVI_NOTIMER | AVI_NODRAW | AVI_NOPALETTE,0,&aviBuf)){
			aviBuf = NULL;
		}
	}

	if(aviBuf){
		if(!(Flags[curFile] & AVI_RTO_HICOLOR)){
			if(XGR_Obj.flags & XGR_HICOLOR)
				XGR_ReInit(800,600,emode);

			XGR_Fill(0);
			XGR_SetPal(tpal,0,255);
			XGR_Flush(0,0,XGR_MAXX,XGR_MAXY);
		}
		else {
			if(!(XGR_Obj.flags & XGR_HICOLOR)){
				if(XGR_ReInit(800,600,emode | XGR_HICOLOR)){
					XGR_ReInit(800,600,emode);
					aviBuf = NULL;
				}
			}
			else {
//				OLD XTOOL
//				XGR_Fill16RGB(0,0,0);
				XGR_Fill16(0);
				XGR_Flush(0,0,XGR_MAXX - 1,XGR_MAXY - 1);
			}
		}

		if(aviBuf){
			sx = AVIwidth(aviBuf);
			sy = AVIheight(aviBuf);

			x = (XGR_MAXX - sx) / 2;
			y = (XGR_MAXY - sy) / 2;

			if(!(Flags[curFile] & AVI_RTO_HICOLOR)){
				avi_pal = (char*)AVIGetPalette(aviBuf);
				memcpy(pal,avi_pal,768);

				AVIplay(aviBuf,x,y);
				AVIdraw(aviBuf);
				AVIstop(aviBuf);
				AVIclose(aviBuf);

				PalEvidence(tpal,pal);

				if(!AVIopen(XBuf -> address(),0,0,&aviBuf))
					ErrH.Abort(AVInotFoundMSS);
			}
			else {
				AVIstop(aviBuf);
				AVIclose(aviBuf);
//				OLD XTOOL
//				if(!AVIopen(XBuf -> address(),AVI_HICOLOR,0,&aviBuf))
				if(!AVIopen(XBuf -> address(),AVI_LOOPING,0,&aviBuf))
					ErrH.Abort(AVInotFoundMSS);
			}
			AVIplay(aviBuf,x,y);
		}
	}
	count = CLOCK();

	ShowImageMouseFlag = 0;
	ShowImageKeyFlag = 0;

	if(!(Flags[curFile] & AVI_RTO_HICOLOR)){
		delete pal;
		delete tpal;
	}
	delete XBuf;
*/
#endif
_MEM_STATISTIC_("AFTER SHOW IMAGE RTO INIT -> ");
}

int ShowImageRTO::Quant(void)
{
	int ret_flag = 1;
#ifdef SHOW_IMAGES
	LastSoundQuant();
	if(Flags[curFile] & IMG_RTO_SHOW_TEXT){
		ret_flag = aciTextQuant();
	}
	else {
		if(!(Flags[curFile & IMG_RTO_CDTRACK_TIME])){
			if((count + 10000) > CLOCK()){
				ret_flag = 0;
			}
		}
		else
			ret_flag = 0;

		if(ShowImageKeyFlag || ShowImageMouseFlag){
			ret_flag = 1;
		}
	}
#endif
	if(ret_flag){
		curFile ++;
		if(curFile >= numFiles)
			return NextID;
	}
	else {
		return 0;
	}

	return ID;
}

int ShowAviRTO::Quant(void)
{
#ifdef SHOW_LOGOS
/*
	int ret = 0;
	if(aviBuf){
		if(ShowImageKeyFlag || ShowImageMouseFlag){
			ret = 1;
		}
//		OLD XTOOL
//		if(AVIactive(aviBuf)){
			count= CLOCK();
//		}
//		else {
//			if((count + 100) < CLOCK())
//				ret = 1;
//		}

		if(!ret) return 0;
	}
*/
#endif
	curFile ++;
	if(curFile >= numFiles){
		return NextID;
	}

	return ID;
}

void ShowImageRTO::Finit(void)
{
#ifdef SHOW_IMAGES
	char* pal;
	pal = new char[768];
	i_slake_pal((unsigned char*)pal,32);
	delete[] pal;

	if(imageData) delete imageData;
#endif
_MEM_STATISTIC_("AFTER SHOW IMAGE RTO 4 FINIT -> ");
}

void ShowAviRTO::Finit(void)
{
#ifdef SHOW_LOGOS
/*
	char* pal;

	if(aviBuf){
		AVIstop(aviBuf);
		AVIclose(aviBuf);

		if(XGR_Obj.flags & XGR_HICOLOR && curFile >= numFiles) XGR_ReInit(800,600,emode);

		if(!(Flags[curFile - 1] & AVI_RTO_HICOLOR)){
			pal = new char[768];
			i_slake_pal((unsigned char*)pal,32);
			delete pal;
		}
	}
	else {
		if(XGR_Obj.flags & XGR_HICOLOR && curFile >= numFiles) XGR_ReInit(800,600,emode);
	}
*/
#endif
_MEM_STATISTIC_("AFTER SHOW IMAGE RTO 4 FINIT -> ");
}

void set_screen(int Dx,int Dy,int mode,int xcenter,int ycenter)
{
	curGMap -> change(Dx,Dy,mode,xcenter,ycenter);
	Redraw = 1;
}

#ifdef SCREENSHOT
void shotFlush(void)
{
	static int curShotNumber;

	XBuffer buf, out_buf;
	buf < "Vangers";
	if(curShotNumber < 1000)
		buf < "0";
	if(curShotNumber < 100)
		buf < "0";
	if(curShotNumber < 10)
		buf < "0";
	buf <= curShotNumber;
	
	while(true) {
		XBuffer buf2;
		buf2 = buf2 < buf.GetBuf() < ".bmp";
		std::fstream filestr(buf2.GetBuf(), std::ios::in);
		if (!filestr.is_open()) {
			out_buf = out_buf < buf2.GetBuf();
			break;
		}
		filestr.close();
		buf = buf < "v";
	}
	std::cout<<"ScreenShot name:"<<out_buf.GetBuf()<<std::endl;
	SDL_SaveBMP(XGR_Obj.XGR_ScreenSurface, out_buf.GetBuf());
	curShotNumber++;
}
#endif

#ifdef MEMORY_STATISTICS
//const MEMPTR_MAX = 1 << 15;
const int MEMPTR_MAX = 1 << 23;
void* memPtr[MEMPTR_MAX];
int memSize[MEMPTR_MAX];
int memId[MEMPTR_MAX];
int memNcnt,memDcnt;

void* operator new(unsigned int sz)
{
	if(!sz) return NULL;
	void* p = malloc(sz);
	if(memStart){
		memPtr[memIndex] = p;
		memSize[memIndex] = sz;
		memId[memIndex] = memNcnt;
		if(++memIndex == MEMPTR_MAX) ErrH.Abort("memStat: overflow");
		memQ += sz;
//		fmem < "new:\t\t" <= sz < "\t\ttotal:\t\t" <= memQ < "\t\t" <= memNcnt < "\r\n";
		memNcnt++;
		}
	return p;
}

void operator delete(void* p)
{
	int i;
	if(memStart){
		for(i = 0;i < memIndex;i++)
			if(memPtr[i] == p){
				memQ -= memSize[i];
				break;
				}
		if(i < memIndex){
//			fmem < "delete:\t\t" <= memSize[i] < "\t\ttotal:\t\t" <= memQ < "\t\t" <= memDcnt < "\r\n";
			memDcnt++;
			memPtr[i] = NULL;
			memSize[i] = 0;
			}
		}
	free(p);
}

void memstatInit(void)
{
//	static int log = 1;
//	if(log){ log--; return; }
	memset(memPtr,0,MEMPTR_MAX*sizeof(void*));
	memset(memSize,0,MEMPTR_MAX*sizeof(int));
	memIndex = 0;
	memStart = 1;
	memNcnt = memDcnt = 0;
}

void memstatDumpLeak(void)
{
	if(!memStart) return;
	for(int i = 0;i < memIndex;i++)
		if(memPtr[i]){
			fmem < "leak:\t\t" <= memId[i] < "\t\tsize: " <= memSize[i] < "\r\n";
			}
	memStart = 0;
}
#endif

void LoadingMessage(int flush)
{
	if(flush){
		char col255[3] = { 60,40,0 }; XGR_SetPal(col255,255,1);
		char col0[3] = { 0,0,0 }; XGR_SetPal(col0,0,1);
		XGR_Fill(0);
	}
	char* str = getRC();
	sysfont.draw(8,XGR_MAXY - 35,(unsigned char*)str,255,-1);
	XBuffer buf;
#ifdef RUSSIAN_VERSION
#	ifdef _ISCREEN_GERMAN_
	if(XGR_MAXX == 800)
		buf < "(C)1997-2014. Alle Rechte vorbehalten. Release Version 1.3. Gleich geht's weiter...";
	else
		buf < "Spiel wird geladen. Gleich geht's weiter...";
#	else
	buf < Convert(zSTR_LOADING_800);

#	endif
#else
	buf < "(C)1997-2014 All Rights Reserved. Release Version 1.3 Loading. Please wait...";
#endif
	int len = MAX(strlen(str),strlen(buf.GetBuf()));
	sysfont.draw(8,XGR_MAXY - 20,(unsigned char*)(buf.GetBuf()),255,-1);
	XGR_LineTo(8,XGR_MAXY - 45,len*8 + 5 + 16,2,255);
	XGR_LineTo(8 + len*8 + 11,XGR_MAXY - 45 - 7,2*7,3,255);
	//if(flush){
	//	XGR_Flush(0,0,XGR_MAXX,XGR_MAXY);
	//}
}

void theEND(void)
{
	EffectsOff();
	//if(XGR_MAXX != 800)
	//	if(XGR_ReInit(800,600,emode)) ErrH.Abort(ErrorVideoMss);
#ifdef ISCREEN
//	if(Dead) ShowImage("resource\\iscreen\\bitmap\\corpse.bmp");
//	ShowImage("resource\\iscreen\\bitmap\\email.bmp");
#endif
}

void sqFont::init(void* d)
{
	char* p = (char*)d;
	memcpy(this,p,8);
	data = new void*[num];
	for(int i = 0;i < num;i++)
		data[i] = p + 8 + sy*i;
}

void sqFont::draw(int x,int y,unsigned char* s,int fore,int back)
{
	while(*s){
		drawchar(x,y,*s,fore,back);
		s++;
		x += sx;
		}
}

void sqFont::drawtext(int x,int y,char* s,int fore,int back)
{
	char c;
	int i = x;
	while((c = *s) != 0){
		switch(c){
			default:
				drawchar(i,y,*s,fore,back);
				i += sx;
				break;
			case '\n':
				y += sy;
				i = x;
				break;
			case '\t':
				i += 4*sx;
				break;
				}
		s++;
		}
}


void sqFont::drawchar(int x,int y,int ch,int fore,int back)
{
    if(x < 0 || y < 0 || x + sx >= XGR_MAXX || y + sy >= XGR_MAXY) return;
	unsigned char* p = (unsigned char*)data[ch];
	int i,j,m;
	for(j = 0;j < sy;j++)
		for(i = 0,m = 1 << sx;i < sx;i++,m >>= 1){
            if(p[j] & m) 
				XGR_SetPixel(x + i,y + j,fore);
			else
                if(back != -1) 
					XGR_SetPixel(x + i,y + j,back);
			}
}

void rtoSetNextID(int id,int next)
{
	BaseRTO* p = (BaseRTO*)xtGetRuntimeObject(id);
	if(p) p -> SetNext(next);
}

void rtoSetFlag(int id,int fl)
{
	BaseRTO* p = (BaseRTO*)xtGetRuntimeObject(id);
	if(p) p -> SetFlag(fl);
}

void rtoClearFlag(int id,int fl)
{
	BaseRTO* p = (BaseRTO*)xtGetRuntimeObject(id);
	if(p) p -> ClearFlag(fl);
}

ShowImageRTO::ShowImageRTO(void)
{
	int i;
	ID = RTO_SHOW_IMAGE_ID;
	Timer = RTO_IMAGE_TIMER;
	numFiles = 0;

	imageData = NULL;

	Flags = new int[RTO_SHOW_IMAGE_MAX];
	memset(Flags,0,sizeof(int) * RTO_SHOW_IMAGE_MAX);

	fileNames = new char*[RTO_SHOW_IMAGE_MAX];
	for(i = 0; i < RTO_SHOW_IMAGE_MAX; i ++)
		fileNames[i] = new char[256];
}

ShowAviRTO::ShowAviRTO(void)
{
	int i;
	ID = RTO_SHOW_AVI_ID;
	Timer = RTO_IMAGE_TIMER;
}

void SetupPath(void)
{
#if defined(BETA_TESTING) || defined(CDCHECK)
	char* path = getVideoPath();
#if defined(RUSSIAN_VERSION) && !defined(GERMAN_VERSION)
	if(!path) ErrH.Abort("");
#else
	if(!path) ErrH.Abort("Software is NOT properly installed. Please, reinstall the Game!");
#endif
	extern char* iVideoPath;
	strcat(path,"\\");
	iVideoPath = path;
#endif
}

char* getRC(void)
{
#ifdef _DEMO_
	return "VANGERS Cover Demo (c)1997-98 All Rights Reserved";
#else
#ifdef BETA_TESTING
	static char* str = getOwner();
	if(!str) ErrH.Abort("Software is NOT properly registered. Please, address to developers!");
	return str;
#else
		static XBuffer b(128);
		b.init();
#ifdef RUSSIAN_VERSION
		const char* sub[6] = { "The Vandals and the Heroes of the Bouillon of Spawn...",
					    "The Fitters of the Emmet Cults...",
					     "The Mechanix of Own Fate...",
					     "The Hearse-Riders of the Cursed Crossroads...",
					     "Exodus to Otherworld...",
					     "One for the Road..."
					};
		b < ConvertUTF8("Vangers: ");
		b < ConvertUTF8(sub[realRND(5)]);
#else
		const char* sub[6] = { "The Vandals and the Heroes of the Bouillon of Spawn...",
					    "The Fitters of the Emmet Cults...",
					     "The Mechanix of Own Fate...",
					     "The Hearse-Riders of the Cursed Crossroads...",
					     "Exodus to Otherworld...",
					     "One for the Road..."
					};
		b < "VANGERS: ";
		b < sub[realRND(6)];
#endif
		return b.GetBuf();

#endif
#endif
}

void GameTimerON_OFF(void)
{
	GameQuantRTO* p = (GameQuantRTO*)xtGetRuntimeObject(RTO_GAME_QUANT_ID);
	if(!p) return;
	if(!p -> Timer) p -> SetTimer(RTO_GAME_QUANT_TIMER);
	else p -> SetTimer(0);
}

void ShowImageMousePress(int fl, int x, int y)
{
	ShowImageMouseFlag = 1;
}

void ShowImageKeyPress(SDL_Event *k)
{
	ShowImageKeyFlag = 1;
}
