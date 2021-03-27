/* ---------------------------- INCLUDE SECTION ----------------------------- */

#include "../global.h"
#include "../lang.h"
#include <zlib.h>

#if defined(__APPLE__) || __GNUC__ < 9
#include <sys/stat.h>
#else
#include <filesystem>
#endif

#include "../runtime.h"

#include "../network.h"

#include "../terra/world.h"

#include "../iscreen/hfont.h"
#include "../iscreen/iscreen.h"
#include "../iscreen/iscript.h"
#include "../iscreen/iml.h"

#include "item_api.h"
#include "../units/uvsapi.h"

#include "../uvs/univang.h"
#include "../uvs/diagen.h"

#include "a_consts.h"

#include "actint.h"
#include "credits.h"
#include "aci_str.h"

#include "mlconsts.h"
#include "mlstruct.h"

#include "../iscreen/ikeys.h"
#include "../iscreen/zip.h"

#include "../iscreen/ext_evnt.h"

#include "../sound/hsound.h"

#include "aci_evnt.h"
#include "acsconst.h"
#include "aci_scr.h"
#include "chtree.h"

#if defined(__APPLE__) || __GNUC__ < 9
#else
namespace fs = std::filesystem;
#endif

/* ----------------------------- STRUCT SECTION ----------------------------- */
/* ----------------------------- EXTERN SECTION ----------------------------- */

#ifdef _DEMO_
extern int aciCompleteGameFlag;
#endif

extern int aciPrevJumpCount;
extern PlayerData** iPlayers;

extern int dgMood;
extern int iMultiFlag;
extern int aciCurLuck;
extern int aciPrevLuck;
extern const char* iVideoPath;

extern int aciCurDominance;
extern int aciPrevDominance;

extern char* acsBackupStr;
extern unsigned char* palbuf;

extern int RAM16;

extern int aciPickupItems;
extern int aciPickupDevices;

extern int iNumPlayers;

extern int prevWorld;

extern uvsActInt* GGamer;
extern uvsActInt* GCharItem;

extern unsigned char* iscrPal;


extern int iScreenActive;
extern int iSlotNumber;

extern int ShowImageKeyFlag;
extern int ShowImageMouseFlag;

extern uvsContimer ConTimer;

extern int GameQuantReturnValue;

extern int aci_SecondMatrixID;
extern int curMatrixID;

extern actIntDispatcher* aScrDisp;
extern char* aImageBmp;

extern unsigned char* aCellFrame;
extern unsigned char* actIntPal;

extern int beebos;
extern int uvsCurrentWorldUnable;

extern int aCellSize;
extern int iCellSize;
extern int aci_CellSize;

extern int iMouseX;
extern int iMouseY;

extern int iEvLineID;

extern int curMatrix;

extern int ViewX;
extern int ViewY;

extern short iMouseSizeX;
extern short iMouseSizeY;
extern short iMouseSize;
extern unsigned char* iMouseFrames;

extern short aMouseSizeX;
extern short aMouseSizeY;
extern short aMouseSize;
extern unsigned char* aMouseFrames;

extern short MouseSizeX;
extern short MouseSizeY;
extern short MouseSize;
extern unsigned char* MouseFrames;

extern unsigned char* aciCurColorScheme;
extern int aciTeleportEvent;

extern uvsVanger* Gamer;

extern int MouseSize2;

extern int camera_moving_xy_enable;
extern int camera_moving_z_enable;

extern int camera_slope_enable;

extern int camera_rotate_enable;

#ifdef _ACTINT_MEMSTAT_
extern void* memPtr[];
extern int memIndex;
#endif

extern int iCurMultiGame;

/* --------------------------- PROTOTYPE SECTION ---------------------------- */

void uvsChangeCycle(void);
void iSortPlayers(int mode = 0);

void aciInitEndGame(int id);
int aciTextQuant(void);

void aciAddTabutask(int num);
void aciCompleteTabutask(void);
void aciDisableParametersMenu(void);

void uvsTabuComplete(int i);
void uvsTabuCreate(uvsActInt* pa, int num);

void aciReadItmText(void);

char* uvsGetNameByID(int type, int& ID);
char* aciGetItemEscave(int p2);

void acsChangeStrState(int id);
void acsSetStrState(int id,int state);
int acsGetStrState(int id);

int uvsReturnWorldGamerVisit(int where);

void uvsChangeLuck(void);
void uvsChangeDominance(void);

void iSoundQuant(int value = 0);

void i_slake_pal(unsigned char* p,int delta);
void i_evince_pal(unsigned char* p,int delta);

void aWriteHelpString(int count, int* x, int* y, int font, unsigned char **pstr, int timer, int color, int space, unsigned int *pcolor);

void EffectsOff(void);

void iSaveData(void);
void iLoadData(void);

void acsSaveData(void);
void acsLoadData(void);
void acsCompressData(char* p);
void acsDecompressData(char* p);

void aciShowFrags(void);

void aciRedrawParamsPanel(int x,int y,int sx,int sy);

void aciSaveUVSList(XStream& fh,uvsActInt* p);
void aciLoadUVSList(XStream& fh,uvsActInt** p,int list_type = 0);
void aciFreeUVSList(uvsActInt* p);

void aciFreeUVS(void);

void aciPutTreasureItem(void);
void aciStealItem(int type);

void acsPrepareSlotName(int id,int slot_num);
void acsPrepareSlotNameInput(int id,int slot_num);

int acsQuant(void);
void acsHandleExtEvent(int code,int data0 = 0,int data1 = 0,int data2 = 0);

void GeneralSystemLoad(XStream& in);
void GeneralSystemSave(XStream& out);

void load_map_bmp(int num);
void set_screen(int Dx,int Dy,int mode,int xcenter,int ycenter);
void set_mouse_cursor(char* p,int sx,int sy);
void restore_mouse_cursor(void);

void aciInitAviObject(void);
void aciFreeAll(void);
void loadMouseFrames(void);

void build_cell_frame(void);
void free_cell_frame(void);

void swap_buf_col(int src,int dest,int sx,int sy,unsigned char* buf);
void mem_putspr(int x,int y,int sx,int sy,int bsx,int bsy,unsigned char* src,unsigned char* dest);
void mem_rectangle(int x,int y,int sx,int sy,int bsx,int col_in,int col_out,int mode,void* buf);
void aParseScript(const char* fname,const char* bname = NULL);
void acsParseScript(const char* fname,const char* bname = NULL);

void aciChangeWorld(int w_id);
void aciInitWorldIDs(void);
void aciPrepareWorldsMenu(void);

void aciInitPanel(InfoPanel* p,unsigned char* str);
void aciClearQuestMenu(void);

void aciInitShopButtons(void);

void aciSet_aMouse(void);
void aciSet_iMouse(void);

void aInit(void);
void aRedraw(void);
void aLoadFonts(void);
void aFreeFonts(void);

void aKeyInit(void);
void aKeyTrap(SDL_Event *k);
void aKeyQuant(void);
void actIntQuant(void);
void aci_iScreenQuant(void);

void aciSetCameraMenu(void);
void aciHandleCameraEvent(int code,int data);

void aci_setMatrixBorder(void);
void aci_setScMatrixBorder(void);
void aciAutoMoveItems(void);
void aciSetMechName(void);
void aciSellMoveItem(void);
void aciRemoveUvsItem(uvsActInt* ptr);
void aciSell_Item(invItem* p);
void aciSell_Matrix(invMatrix* p);
void aciInitInfoPanels(void);
int aciCheckSellItem(void);
void aciSetTimePanel(void);
void aciInitEvLineID(void);
void aciChangeList(void);
void aciChangeLineID(void);
void aciInitShopItems(void);

void aciCloseShopMenu(void);
void aciShopMenuUp(void);
void aciShopMenuDown(void);
void aciBuildShopList(void);
void aciInitShopList(void);
void aciChooseShopItem(void);
void aciSetShopItem(int id);

void aciNextPhrase(void);
void aciStartSpeech(void);

void aciEscaveKickOut(void);

void aciInitMechosInfo(invMatrix* p,InfoPanel* ip);

void aciOfferEleech(void);
void aciWaitNextPeriod(void);
void aciOfferWeezyk(void);
void aciGetEleech(void);

void aciNextShopItem(void);
void aciPrevShopItem(void);
void aciInitShopAvi(void);
void aciBuyItem(void);

void aciOpenGate(void);
void aciCloseGate(void);

void SelectCompasTarget(const char * n);
char* GetCompasTarget(void);

int aciCheckCredits(void);

void aciShowCamerasMenu(int x,int y,int sx,int sy);
void aciHandleSubmenuEvent(int id,fncMenuItem* p);

int aciGetCurCycle(void);

void aciInitAviID(void);
void aciChangeAviIndex(void);

void aciBuildQuestList(void);

char* aciGetItemLoad(invItem* p,int mode);
char* aciGetPrice(iListElement* p,int mode,int sell_mode);
void aciInitPricePanel(InfoPanel* ip,iListElement* p,int mode,int sell_mode);
int aciGetCurMatrixPrice(void);

void aciCHandler(int key);

int aciCheckSlot(void);

void iScrQuantPrepare(void);
void iScrQuantFinit(void);

void show_map(int x,int y,int sx,int sy);

void set_mouse(void);

void a_calc_tables(void);
void a_buf_line(int x1,int y1,int x2,int y2,int col,int sx,int sy,unsigned char* buf);

void aMS_init(void);

void aci_SendEvent(int cd,int dt = 0);

void aciPackFile(char* fname);
void aciUnPackFile(char* fname);

void aMS_LeftPress(int, int, int);
void aMS_LeftUnpress(int, int, int);
void aMS_RightPress(int, int, int);
void aMS_RightUnpress(int, int, int);
void aMS_Movement(int, int, int);

void aciShowRacingPlace(void);

#ifdef _GENERATE_IND_SHAPE_
void smooth_shape_quant(int sx,int sy,unsigned char* buf,int src,int dest,int cnd);
#endif

#ifdef _ACTINT_MEMSTAT_
void aciDetectLeaks(void);
#endif

void aciPutBufSpr(int x,int y,int sx,int sy,int bsx,void* src,void* dest,int col,int col_sz);
void aciPutSpr32(int x,int y,int sx,int sy,void* buf,int col,int col_sz);
void aciPutSpr32clip(int x,int y,int sx,int sy,void* buf,int col,int col_sz);
void aOutText32(int x,int y,int color,void* text,int font,int hspace,int vspace);
void aOutText32clip(int x,int y,int color,void* text,int font,int hspace,int vspace);
int aTextWidth32(void* text,int font,int hspace);
int aTextHeight32(void* text,int font,int vspace);

void aciInitStrings(void);

void aciInitItmTextQueue(void);

/* --------------------------- DEFINITION SECTION --------------------------- */

#define ACI_ITEM_QUEUE_MAX	5

//#define _ACI_CHECK_PHRASES_
#define _GENERATE_EXIT_
//#define _ACI_MAP_TESTING_

aciFont** aScrFonts32 = NULL;

#ifdef _ACI_BML_FONTS_
aciFont** aScrFonts = NULL;
#else
iScreenFont** aScrFonts = NULL;
#endif

#ifdef _ACI_LOGFILE_
char* aciLogFileName = "actint/temp/actint.log";
#endif


int aciCurCredits02 = 0;

int aciRacingFlag = 0;
char* aciCurRaceType = NULL;
char* aciCurRaceInfo = NULL;

int aciMouseFlagL = 0;
int aciMouseFlagR = 0;

int aInitFlag = 0;
int actintLowResFlag = 0;
int actintActiveFlag = 0;
int actIntLog = 0;

int aciGamerColor = 0;

int aciCurMechosID = 0;

int* aSinTable = NULL;
int* aCosTable = NULL;

bmlObject* aIndArrowBML = NULL;
bmlObject* aIndDataBML = NULL;
bmlObject* aIndBackBML = NULL;

iListElement* iShopItem;

XBuffer* aciXConv;

unsigned char* aciDgPrefix = NULL;
unsigned char* aciDgPostfix = NULL;

aciScreenText* aciImgText = NULL;

int aciCurAviIndex = 0;
int aciCurAviIndexDelta = 0;
int aciLoadLog = 0;
int aciShopMenuLog = 0;
int aciWorldIndex = -1;

int acsScreenID = 1;

int acsCurrentSlotID = ACS_SAVE_SLOT0;
int acsCurrentSlotNum = 0;

#ifdef _ACTINT_MEMSTAT_
XStream aMemStatFile("actint/temp/memstat.log",XS_OUT);
int aMemStartIndex = 0;
int aMemEndIndex = 0;
#endif

int acsAllocFlag = 0;

int aciSpeechStarted = 0;
int aciSpeechBegLocked = 0;
int aciSpeechEndLocked = 0;
int aciEndSpeech = 0;
int aciSpeechMode = 2;
int aciBreathFlag = 0;
int aciEscaveDead = 0;
int aciEscaveEmpty = 0;
aciML_EventSeq* aciSpeechSeq = NULL;
aciCHTree* aciTreeData = NULL;

int aciKeyboardLocked = 0;
int aciAutoRun = 0;

#ifdef _ACI_ESCAVE_DEBUG_
int aci_dgMoodNext = -1;
#endif

int aciAutoSaveFlag = 0;

const char* aciSTR_ON = aciSTR_ON1;
const char* aciSTR_OFF = aciSTR_OFF1;
const char* aciSTR_DAY = aciSTR_DAY1;
const char* aciSTR_UNDEFINED_PRICE = aciSTR_UNDEFINED_PRICE1;
const char* aciSTR_PRICE = aciSTR_PRICE1;
const char* aciSTR_EMPTY_SLOT = aciSTR_EMPTY_SLOT1;
const char* aciSTR_UNNAMED_SAVE = aciSTR_UNNAMED_SAVE1;
const char* aciSTR_AUTOSAVE = aciSTR_AUTOSAVE1;
const char* aciSTR_KILLS = aciSTR_KILLS1;
const char* aciSTR_DEATHS = aciSTR_DEATHS1;
const char* aciSTR_LUCK = aciSTR_LUCK1;
const char* aciSTR_DOMINANCE = aciSTR_DOMINANCE1;
const char* aciSTR_BROKEN = aciSTR_BROKEN1;
const char* aciSTR_ENERGY_SHIELD = aciSTR_ENERGY_SHIELD1;
const char* aciSTR_RESTORING_SPEED = aciSTR_RESTORING_SPEED1;
const char* aciSTR_MECHANIC_ARMOR = aciSTR_MECHANIC_ARMOR1;
const char* aciSTR_VELOCITY = aciSTR_VELOCITY1;
const char* aciSTR_SPIRAL_CAPACITY = aciSTR_SPIRAL_CAPACITY1;
const char* aciSTR_AIR_RESERVE = aciSTR_AIR_RESERVE1;
const char* aciSTR_DAMAGE = aciSTR_DAMAGE1;
const char* aciSTR_LOAD = aciSTR_LOAD1;
const char* aciSTR_SHOTS = aciSTR_SHOTS1;
const char* aciSTR_BURST = aciSTR_BURST1;
const char* aciSTR_WORKING_TIME = aciSTR_WORKING_TIME1;
const char* aciSTR_SECONDS = aciSTR_SECONDS1;
const char* aciSTR_IN_PACK = aciSTR_IN_PACK1;
const char* aciSTR_NO_CASH = aciSTR_NO_CASH1;
const char* aciSTR_PICKUP_ITEMS_OFF = aciSTR_PICKUP_ITEMS_OFF1;
const char* aciSTR_PICKUP_WEAPONS_OFF = aciSTR_PICKUP_WEAPONS_OFF1;
const char* aciSTR_PutThis = aciSTR_PutThis1;
const char* aciSTR_Ware1 = aciSTR_Ware11;
const char* aciSTR_Ware2 = aciSTR_Ware21;
const char* aciSTR_Checkpoints = aciSTR_Checkpoints1;

int aciItmTextQueueSize = 0;
int aciItmTextQueueCur = 0;
invItem** aciItmTextQueue = NULL;

#ifdef _ACI_CHECK_PHRASES_
int aciPhraseStrLen = 0;
int aciPhraseNumStr = 0;
#endif

extern iScreenOption** iScrOpt;

void aInit(void)
{
	GameOverID = 0;
	aciInitStrings();

	if(!aIndArrowBML){
		aIndArrowBML = new bmlObject;
		aIndArrowBML -> flags |= BML_NO_OFFSETS;
		if(actintLowResFlag)
			aIndArrowBML -> load("resource/actint/640x480/ind_data/arrow.bml");
		else
			aIndArrowBML -> load("resource/actint/800x600/ind_data/arrow.bml");
	}
	if(!aIndDataBML){
		aIndDataBML = new bmlObject;
		aIndDataBML -> flags |= BML_NO_OFFSETS;
		if(actintLowResFlag)
			aIndDataBML -> load("resource/actint/640x480/ind_data/data.bml");
		else
			aIndDataBML -> load("resource/actint/800x600/ind_data/data.bml");
	}
	if(!aIndBackBML){
		aIndBackBML = new bmlObject;
		aIndBackBML -> flags |= BMP_FLAG;
		if(actintLowResFlag)
			aIndBackBML -> load("resource/actint/640x480/ind_data/back.bmp");
		else
			aIndBackBML -> load("resource/actint/800x600/ind_data/back.bmp");
	}
	if(!aciSpeechSeq){
		aciSpeechSeq = new aciML_EventSeq;
		aciSpeechSeq -> alloc_mem(256);
	}

	if(!aciCurRaceType)
			aciCurRaceType = new char[10];

	if(!aciCurRaceInfo)
			aciCurRaceInfo = new char[10];

	if(!aciTreeData){
		aciTreeData = new aciCHTree;
		aciTreeData -> load("resource/iscreen/ldata/l7/border.xbm");
	}

#ifdef _BINARY_SCRIPT_
	if(actintLowResFlag){
		if(lang() == RUSSIAN){
			aParseScript("resource/actint/aci_low2.scb");
			acsParseScript("resource/actint/acs_low2.scb");
		}
		else {
			aParseScript("resource/actint/aci_low.scb");
			acsParseScript("resource/actint/acs_low.scb");
		}
	}
	else {
		if(lang() == RUSSIAN){
			aParseScript("resource/actint/aci_hi2.scb");
			acsParseScript("resource/actint/acs_low2.scb");
		}
		else {
			aParseScript("resource/actint/aci_hi.scb");
			acsParseScript("resource/actint/acs_low.scb");
		}
	}
#else
	if(actintLowResFlag){
		if(lang() == RUSSIAN){
			aParseScript("actint/aci_low2.scr","resource/actint/aci_low2.scb");
			acsParseScript("actint/acs_low2.scr","resource/actint/acs_low2.scb");
		}
		else {
			aParseScript("actint/aci_low.scr","resource/actint/aci_low.scb");
			acsParseScript("actint/acs_low.scr","resource/actint/acs_low.scb");
		}
	}
	else {
		if(lang() == RUSSIAN){
			aParseScript("actint/aci_hi2.scr","resource/actint/aci_hi2.scb");
			acsParseScript("actint/acs_low2.scr","resource/actint/acs_low2.scb");
		}
		else {
			aParseScript("actint/aci_hi.scr","resource/actint/aci_hi.scb");
			acsParseScript("actint/acs_low.scr","resource/actint/acs_low.scb");
		}
	}
#endif
	a_calc_tables();

	if(!NetworkON){
		aciUpdateCurCredits(ACI_STARTUP_CREDITS + beebos);
	}
	else {
		aciDisableParametersMenu();
	}

#ifdef _ACI_LOGFILE_
	aScrDisp -> logFile.open(aciLogFileName,XS_OUT);
	aScrDisp -> logFile < "actIntDispatcher Log File...\r\n";
	aScrDisp -> logFile.close();
#endif
	curMatrixID = 0;

	aciXConv = new XBuffer;

	aciInitWorldIDs();
	aLoadFonts();
	aScrDisp -> init_prompt();
	aScrDisp -> init();

	aInitFlag = 1;
	aMS_init();

#ifdef _GENERATE_MATRIX_SHAPES_
	invMatrix* p = (invMatrix*)aScrDisp -> matrixList -> last;
	while(p){
		p -> generate_shape();
		p -> generate_floor();
		p = (invMatrix*)p -> prev;
	}
#ifdef _GENERATE_EXIT_
	ErrH.Abort("Done...");
#endif
#endif
#ifdef _GENERATE_iMATRIX_SHAPES_
	aci_CellSize = aCellSize;
	aCellSize = iCellSize;
	build_cell_frame();
	invMatrix* p = (invMatrix*)aScrDisp -> i_matrixList -> last;
	while(p){
		p -> generate_shape();
		p -> generate_floor();
		p = (invMatrix*)p -> prev;
	}
	aCellSize = aci_CellSize;
	build_cell_frame();
#ifdef _GENERATE_EXIT_
	ErrH.Abort("Done...");
#endif
#endif
	actIntPal = new unsigned char[768];
	if(!aciItmTextQueue) aciItmTextQueue = new invItem*[ACI_ITEM_QUEUE_MAX];

#ifdef _FULLSCREEN_MODE_
	aScrDisp -> send_event(EV_FULLSCR_CHANGE);
#endif

/*
#ifdef _GENERATE_ITEM_DATA_
	aScrDisp -> save_items();
#ifdef _GENERATE_EXIT_
	ErrH.Abort("Done...");
#endif
#endif
*/

#ifdef _ACTINT_MEMSTAT_
	aciDetectLeaks();
	aMemStatFile.close();
#endif
}

void aRedraw(void)
{
	aScrDisp -> flags &= ~AS_FULL_REDRAW;
	aScrDisp -> redraw();
}

void actIntQuant(void)
{
	actintActiveFlag = 1;
	aScrDisp -> KeyQuant();
	aScrDisp -> EventQuant();
}

void aLoadFonts(void)
{
	int i;
	XBuffer XBuf;
	XStream fh;

#ifdef _ACI_BML_FONTS_
	if(!aScrFonts)
		aScrFonts = new aciFont*[AS_NUM_FONTS];
#else
	int sz;
	char* buf;
	if(!aScrFonts)
		aScrFonts = new iScreenFont*[AS_NUM_FONTS];
#endif

	for(i = 0; i < AS_NUM_FONTS; i ++){
		XBuf.init();
		XBuf < "resource/actint/fonts/sfont";
		if(i < 10)
			XBuf < "0";

#ifdef _ACI_BML_FONTS_
		XBuf <= i < ".bml";
		aScrFonts[i] = new aciFont;
		aScrFonts[i] -> load(XBuf.GetBuf());
		aScrFonts[i] -> calc_offs();
#else
		XBuf <= i < ".fnt";
		fh.open(XBuf,XS_IN);
		sz = fh.size();

		buf = new char[sz];
		fh.read(buf,sz);
		fh.close();

		aScrFonts[i] = new iScreenFont(buf,sz);
#endif

	}
}

void aLoadFonts32(void)
{
	int i;
	XBuffer XBuf;
	XStream fh;

	if(!aScrFonts32)
		aScrFonts32 = new aciFont*[AS_NUM_FONTS_32];

	for(i = 0; i < AS_NUM_FONTS_32; i ++){
		XBuf.init();
		XBuf < "resource/actint/fonts/cfont";
		if(i < 10)
			XBuf < "0";

		XBuf <= i < ".bml";
		aScrFonts32[i] = new aciFont;
		aScrFonts32[i] -> flags |= ACI_RANGE_FONT;

		aScrFonts32[i] -> load(XBuf.GetBuf());
		aScrFonts32[i] -> calc_offs();

	}
	XGR_SetTextOutFnc(aOutText32);
	XGR_SetTextXFnc(aTextWidth32);
	XGR_SetTextYFnc(aTextHeight32);
}

void aFreeFonts(void)
{
	int i;
	for(i = 0; i < AS_NUM_FONTS; i ++){
		delete aScrFonts[i];
	}
	delete aScrFonts;
	aScrFonts = NULL;
/*
	for(i = 0; i < AS_NUM_FONTS_32; i ++){
		delete aScrFonts32[i];
	}
	delete aScrFonts32;
	aScrFonts32 = NULL;
*/
}

void aPutNum(int x,int y,int font,int color,int str,int bsx,unsigned char* buf,int space)
{
	char* p;
	static XBuffer XBuf;

	XBuf.init();
	XBuf <= str;
	p = XBuf.GetBuf();

	aPutStr(x,y,font,color,(unsigned char*)p,bsx,buf,space);
}

int aStrLen(unsigned char* str,int font,int space)
{
	int s,sz = strlen((char*)str),len = 0;
	for(s = 0; s < sz; s ++)
		len += aScrFonts[font] -> SizeX - (aScrFonts[font] -> LeftOffs[str[s]] + aScrFonts[font] -> RightOffs[str[s]]) + space;
	return len;
}

int aStrLen32(void* str,int font,int space)
{
	int s,sz,len = 0;

	unsigned char* str_buf = (unsigned char*)str;
	sz = strlen((char*)str_buf);
	for(s = 0; s < sz; s ++)
		len += aScrFonts32[font] -> SizeX - (aScrFonts32[font] -> LeftOffs[str_buf[s]] + aScrFonts32[font] -> RightOffs[str_buf[s]]) + space;
	//printf("aStrLen32: %s %d %d %d -> %d\n", str_buf, sz, font, space, len);
	return len;
}

int aStrHeight(int font)
{
	return aScrFonts[font] -> SizeY;
}

int aStrHeight32(int font)
{
	return aScrFonts32[font] -> SizeY;
}

#ifdef _ACI_BML_FONTS_
#define ACI_COLOR_MASK		0xFF
void aPutStr32(int x,int y,int font,int color,int color_size,void* str,int bsx,void* buf,int space)
{
	int s,xs,ys,offs = 0,sz = strlen((char*)str),X = 0,col,col_sz,col1,col2,col_sz1,col_sz2;
	int buf_offs = 0;

	unsigned char* str_buf;

	if(aScrFonts32[font] == NULL)
		ErrH.Abort("MISSING FONT...");

	col1 = color & 0xFF;
	col2 = (color >> 16) & 0xFF;

	col_sz1 = color_size & 0xFF;
	col_sz2 = (color_size >> 16) & 0xFF;

	unsigned char* p = aScrFonts32[font] -> data;
	xs = aScrFonts32[font] -> SizeX;
	ys = aScrFonts32[font] -> SizeY;

	str_buf = (unsigned char*)str;

	X = x;

	for(s = 0; s < sz; s ++){
		buf_offs = 0;
		X -= aScrFonts32[font] -> LeftOffs[str_buf[s]];

		offs = xs * ys * str_buf[s];
		if(col2){
			if((str_buf[s] >= 48 && str_buf[s] <= 57) || str_buf[s] == 36){
				col = col2;
				col_sz = col_sz2;
			}
			else {
				col = col1;
				col_sz = col_sz1;
			}
		}
		else {
			col = col1;
			col_sz = col_sz1;
		}
		aciPutBufSpr(X,y,xs,ys,bsx,p + offs,buf,col,col_sz);
		X += xs + space - aScrFonts32[font] -> RightOffs[str_buf[s]];
	}
}

void aPutStr(int x,int y,int font,int color,unsigned char* str,int bsx,unsigned char* buf,int space)
{
	int i,j,s,xs,ys,offs = 0,sz = strlen((char*)str),X = 0,col1,col2,col3,col4;
	int buf_index = 0,buf_offs = 0;

	col1 = color & ACI_COLOR_MASK;
	col2 = (color >> 8) & ACI_COLOR_MASK;

	col3 = (color >> 16) & ACI_COLOR_MASK;
	col4 = (color >> 24) & ACI_COLOR_MASK;

	if(aScrFonts[font] == NULL)
		ErrH.Abort("MISSING FONT...");

	unsigned char* p = aScrFonts[font] -> data;
	xs = aScrFonts[font] -> SizeX;
	ys = aScrFonts[font] -> SizeY;

	buf_index = x + y * bsx;

	if(col2){
		for(s = 0; s < sz; s ++){
			buf_offs = 0;
			offs = ys * str[s];

			X -= aScrFonts[font] -> LeftOffs[str[s]];

			offs = xs * ys * str[s];
			if((col3 || col4) && ((str[s] >= 48 && str[s] <= 57) || str[s] == 36)){
				for(i = 0; i < ys; i ++){
					for(j = 0; j < xs; j ++){
						switch(p[offs + j]){
							case 1:
								buf[X + j + buf_index + buf_offs] = col3;
								break;
							case 2:
								buf[X + j + buf_index + buf_offs] = col4;
								break;
						}
					}
					offs += xs;
					buf_offs += bsx;
				}
			}
			else {
				for(i = 0; i < ys; i ++){
					for(j = 0; j < xs; j ++){
						switch(p[offs + j]){
							case 1:
								buf[X + j + buf_index + buf_offs] = col1;
								break;
							case 2:
								buf[X + j + buf_index + buf_offs] = col2;
								break;
						}
					}
					offs += xs;
					buf_offs += bsx;
				}
			}
			X += xs + space - aScrFonts[font] -> RightOffs[str[s]];
		}
	}
	else {
		for(s = 0; s < sz; s ++){
			buf_offs = 0;
			offs = ys * str[s];

			X -= aScrFonts[font] -> LeftOffs[str[s]];

			offs = xs * ys * str[s];
			if(col3 && ((str[s] >= 48 && str[s] <= 57) || str[s] == 36)){
				for(i = 0; i < ys; i ++){
					for(j = 0; j < xs; j ++){
						switch(p[offs + j]){
							case 1:
								buf[X + j + buf_index + buf_offs] = col3;
								break;
						}
					}
					offs += xs;
					buf_offs += bsx;
				}
			}
			else {
				for(i = 0; i < ys; i ++){
					for(j = 0; j < xs; j ++){
						switch(p[offs + j]){
							case 1:
								buf[X + j + buf_index + buf_offs] = col1;
								break;
						}
					}
					offs += xs;
					buf_offs += bsx;
				}
			}
			X += xs + space - aScrFonts[font] -> RightOffs[str[s]];
		}
	}
}

void aOutStr(int x,int y,int font,int color,unsigned char* str,int space)
{
	int i,j,s,xs,ys,offs = 0,sz = strlen((char*)str),X = 0,Y = 0,col1,col2;

	unsigned char* palBuf = palbuf;

	col1 = color & ACI_COLOR_MASK;
	col2 = (color >> 8) & ACI_COLOR_MASK;

	if(aScrFonts[font] == NULL)
		ErrH.Abort("MISSING FONT...");

	unsigned char* p = aScrFonts[font] -> data;
	xs = aScrFonts[font] -> SizeX;
	ys = aScrFonts[font] -> SizeY;

	X = x;

	if(col2){
		for(s = 0; s < sz; s ++){
			offs = ys * str[s];

			X -= aScrFonts[font] -> LeftOffs[str[s]];

			offs = xs * ys * str[s];
			for(i = 0; i < ys; i ++){
				Y = y + i;
				for(j = 0; j < xs; j ++){
					switch(p[offs + j]){
						case 1:
							XGR_SetPixel(X + j,Y,col1);
							break;
						case 2:
							XGR_SetPixel(X + j,Y,col2);
							break;
					}
				}
				offs += xs;
			}
			X += xs + space - aScrFonts[font] -> RightOffs[str[s]];
		}
	}
	else {
		for(s = 0; s < sz; s ++){
			offs = ys * str[s];

			X -= aScrFonts[font] -> LeftOffs[str[s]];

			offs = xs * ys * str[s];
			for(i = 0; i < ys; i ++){
				Y = y + i;
				for(j = 0; j < xs; j ++){
					switch(p[offs + j]){
						case 1:
							XGR_SetPixel(X + j,Y,col1);
							break;
					}
				}
				offs += xs;
			}
			X += xs + space - aScrFonts[font] -> RightOffs[str[s]];
		}
	}
}

void aPutChar(int x,int y,int font,int color,int str,int bsx,int bsy,unsigned char* buf)
{
	int i,j,xs,ys,offs = 0,X = 0,col1,col2;
	int buf_index = 0,buf_offs = 0;

	col1 = color & ACI_COLOR_MASK;
	col2 = (color >> 8) & ACI_COLOR_MASK;

	if(aScrFonts[font] == NULL)
		ErrH.Abort("MISSING FONT...");

	unsigned char* p = aScrFonts[font] -> data;
	xs = aScrFonts[font] -> SizeX;
	ys = aScrFonts[font] -> SizeY;

	buf_index = x + y * bsx;

	buf_offs = 0;
	offs = xs * ys * str;

	if(col2){
		for(i = 0; i < ys; i ++){
			for(j = 0; j < xs; j ++){
				switch(p[offs + j]){
					case 1:
						buf[X + j + buf_index + buf_offs] = col1;
						break;
					case 2:
						buf[X + j + buf_index + buf_offs] = col2;
						break;
				}
			}
			offs += xs;
			buf_offs += bsx;
		}
	}
	else {
		for(i = 0; i < ys; i ++){
			for(j = 0; j < xs; j ++){
				switch(p[offs + j]){
					case 1:
						buf[X + j + buf_index + buf_offs] = col1;
						break;
				}
			}
			offs += xs;
			buf_offs += bsx;
		}
	}
}
#else
void aPutStr(int x,int y,int font,int color,unsigned char* str,int bsx,int bsy,unsigned char* buf,int space)
{
	int i,j,s,xs,ys,offs = 0,sz = strlen((char*)str),X = 0,col = color;
	int buf_index = 0,buf_offs = 0;

	if(aScrFonts[font] == NULL)
		ErrH.Abort("MISSING FONT...");

	char* p = aScrFonts[font] -> data;
	xs = aScrFonts[font] -> SizeX;
	ys = aScrFonts[font] -> SizeY;

	buf_index = x + y * bsx;

	for(s = 0; s < sz; s ++){
		buf_offs = 0;
		offs = ys * str[s];

		col = color;

		X -= aScrFonts[font] -> LeftOffs[str[s]];

		offs = ys * str[s];
		for(i = 0; i < ys; i ++){
			for(j = 0; j < xs; j ++){
				if(p[offs] & (1 << (7 - j)))
					buf[X + j + buf_index + buf_offs] = col;
			}
			offs ++;
			buf_offs += bsx;
		}
		X += xs + space - aScrFonts[font] -> RightOffs[str[s]];
	}
}

void aPutChar(int x,int y,int font,int color,int str,int bsx,int bsy,unsigned char* buf)
{
	int i,j,xs,ys,offs = 0,X = 0,col = color;
	int buf_index = 0,buf_offs = 0;

	if(aScrFonts[font] == NULL)
		ErrH.Abort("MISSING FONT...");

	char* p = aScrFonts[font] -> data;
	xs = aScrFonts[font] -> SizeX;
	ys = aScrFonts[font] -> SizeY;

	buf_index = x + y * bsx;

	buf_offs = 0;
	offs = ys * str;

	col = color;

	offs = ys * str;
	for(i = 0; i < ys; i ++){
		for(j = 0; j < xs; j ++){
			if(p[offs] & (1 << (7 - j)))
				buf[X + j + buf_index + buf_offs] = col;
		}
		offs ++;
		buf_offs += bsx;
	}
}
#endif

void aKeyTrap(SDL_Event *k)
{
	if(aInitFlag){
		KeyBuf -> put(k,CUR_KEY_PRESSED);
	}
}

void aKeyQuant(void)
{
	aScrDisp -> KeyQuant();
}

void aKeyInit(void)
{
	loadMouseFrames();
	KeyBuf = new KeyBuffer;
	set_mouse();
}

void aMS_init(void)
{
	XGR_MouseSetPressHandler(XGM_LEFT_BUTTON,aMS_LeftPress);
	XGR_MouseSetUnPressHandler(XGM_LEFT_BUTTON,aMS_LeftUnpress);

	XGR_MouseSetPressHandler(XGM_RIGHT_BUTTON,aMS_RightPress);
	XGR_MouseSetUnPressHandler(XGM_RIGHT_BUTTON,aMS_RightUnpress);

	XGR_MouseSetMoveHandler(aMS_Movement);

	aciMouseFlagL = aciMouseFlagR = 0;
}

void aMS_LeftPress(int fl, int x, int y)
{
	if (aScrDisp==NULL) {
		return;
	}
	aScrDisp -> flags |= aMS_LEFT_PRESS;
	iMouseX = x;
	iMouseY = y;

	aciMouseFlagL = 1;

	XGR_MouseSetSeq(0,1,XGM_PLAY_ONCE,1);
}

void aMS_Movement(int fl, int x, int y)
{
	if (aScrDisp==NULL) {
		return;
	}
	aScrDisp -> flags |= aMS_MOVED;
	iMouseX = x;
	iMouseY = y;
}

void aMS_LeftUnpress(int fl, int x, int y)
{
	iMouseX = x;
	iMouseY = y;

	aciMouseFlagL = 0;

	XGR_MouseSetSeq(XGR_MouseCurFrame,XGR_MouseNumFrames - 1,XGM_PLAY_ONCE,1);
}

void aMS_RightPress(int fl, int x, int y)
{
	if (aScrDisp==NULL) {
		return;
	}
	aScrDisp -> flags |= aMS_RIGHT_PRESS;
	iMouseX = x;
	iMouseY = y;

	aciMouseFlagR = 1;

	XGR_MouseSetSeq(0,1,XGM_PLAY_ONCE,1);
}

void aMS_RightUnpress(int fl, int x, int y)
{
	iMouseX = x;
	iMouseY = y;

	aciMouseFlagR = 0;

	XGR_MouseSetSeq(XGR_MouseCurFrame,XGR_MouseNumFrames - 1,XGM_PLAY_ONCE,1);
}

#define MAP_COLOR	200
#define MAP_COLOR1	166
#define MAP_COLOR2	143
#define MAP_COLOR3	111

#define MAP_PTR_X	3
#define MAP_PTR_X2	1
#define MAP_PTR_XL	6
#define MAP_PTR_XL2	3

#define MAP_CUR_VANGER	0
#define MAP_VANGER	1
#define MAP_INSECT	2

static int aciMapVngColors[11] = { 254, 143, 156, 175, 200, 200, 239, 230, 224, 254, 156 };

void show_map(int x,int y,int sx,int sy)
{
	int i,index,m_index,cx,cy,y0,x_index = 0,_x = 0,_y = 0,vy,sx2,sy2;
	int scr_x,scr_y,dx,scr_x0;

	unsigned char* ptr;
	bmlObject* p = aScrDisp -> mapObj;

	cx = (ViewX * p -> SizeX) >> MAP_POWER_X;
	cy = (ViewY * p -> SizeY) >> MAP_POWER_Y;

	sx2 = sx / 2;
	sy2 = sy / 2;

	vy = sy * ((1 << MAP_POWER_Y) / p -> SizeY) / 2;

	y0 = cy - sy2;
	if(y0 < 0) y0 += p -> SizeY;
	if(y0 >= p -> SizeY) y0 -= p -> SizeY;

	ptr = new unsigned char[sx * sy];
	memset(ptr,0,sx * sy);

	if(cx < sx2){
		dx = sx2 - cx;

		_y = y0;
		index = 0;
		m_index = y0 * p -> SizeX + p -> SizeX - dx;
		for(i = 0; i < sy; i ++){
			memcpy(ptr + index,p -> frames + m_index,dx);

			index += sx;
			m_index += p -> SizeX;

			_y ++;
			if(_y >= p -> SizeY){
				m_index = p -> SizeX - dx;
				_y = 0;
			}
		}

		_y = y0;
		index = dx;
		m_index = y0 * p -> SizeX;
		for(i = 0; i < sy; i ++){
			memcpy(ptr + index,p -> frames + m_index,sx - dx);

			index += sx;
			m_index += p -> SizeX;

			_y ++;
			if(_y >= p -> SizeY){
				m_index = 0;
				_y = 0;
			}
		}
	}
	else {
		if(p -> SizeX - cx < sx2){
			dx = sx2 - (p -> SizeX - cx);

			_y = y0;
			index = 0;
			m_index = y0 * p -> SizeX + p -> SizeX - (sx - dx);
			for(i = 0; i < sy; i ++){
				memcpy(ptr + index,p -> frames + m_index,sx - dx);

				index += sx;
				m_index += p -> SizeX;

				_y ++;
				if(_y >= p -> SizeY){
					m_index = p -> SizeX - (sx - dx);
					_y = 0;
				}
			}

			_y = y0;
			index = sx - dx;
			m_index = y0 * p -> SizeX;
			for(i = 0; i < sy; i ++){
				memcpy(ptr + index,p -> frames + m_index,dx);

				index += sx;
				m_index += p -> SizeX;

				_y ++;
				if(_y >= p -> SizeY){
					m_index = 0;
					_y = 0;
				}
			}
		}
		else {
			_y = y0;
			index = 0;
			m_index = y0 * p -> SizeX + cx - sx2;
			int max_frames_size = p->SizeX * p->SizeY * p->Size;
			for(i = 0; i < sy; i ++){
				if (m_index + sx >= max_frames_size) {
					memcpy(ptr + index, p -> frames + m_index, max_frames_size - m_index);
				} else {
					memcpy(ptr + index, p -> frames + m_index, sx);
				}

				index += sx;
				m_index += p -> SizeX;

				_y ++;
				if(_y >= p -> SizeY){
					m_index = cx - sx2;
					_y = 0;
				}
			}
		}
	}
	XGR_PutSpr(x,y,sx,sy,ptr,XGR_BLACK_FON);
	delete[] ptr;

	_y = 0;

	scr_x = sx2;
	scr_y = sy2;

/*
	if(scr_x > MAP_PTR_X && scr_x < sx - MAP_PTR_X && scr_y > MAP_PTR_X && scr_y < sy - MAP_PTR_X)
		XGR_Rectangle(x + scr_x - MAP_PTR_X2,y + scr_y - MAP_PTR_X2,MAP_PTR_X,MAP_PTR_X,aciMapVngColors[aciGamerColor],aciMapVngColors[aciGamerColor],XGR_FILLED);
*/

#ifdef _ACI_MAP_TESTING_
	index = 2;
	for(_y = 0; _y < (1 << MAP_POWER_Y); _y += 256){
		for(_x = 0; _x < (1 << MAP_POWER_X); _x += 256){
			scr_x = -1;
			scr_x0 = ((_x * p -> SizeX) >> MAP_POWER_X);

			if(cx < sx2){
				if(p -> SizeX - scr_x0 < dx)
					scr_x = scr_x0 - (p -> SizeX - dx);
				else {
					if(scr_x0 < sx - dx)
						scr_x = scr_x0 + dx;
					else
						scr_x = -1;
				}
			}
			else {
				if(p -> SizeX - cx < sx2){
					if(p -> SizeX - scr_x0 < sx - dx)
						scr_x = scr_x0 - (p -> SizeX - (sx - dx));
					else {
						if(scr_x0 < dx)
							scr_x = scr_x0 + (sx - dx);
						else
							scr_x = -1;
					}
				}
				else {
					scr_x = scr_x0 - (cx - sx2);
				}
			}
			scr_y = ((_y * p -> SizeY) >> MAP_POWER_Y) - y0;
			if(scr_y < 0)
				scr_y += p -> SizeY;

			if(scr_x > MAP_PTR_X && scr_x < sx - MAP_PTR_X && scr_y > MAP_PTR_X && scr_y < sy - MAP_PTR_X){
				switch(index){
					case 1:
						XGR_Rectangle(x + scr_x - MAP_PTR_X2,y + scr_y - MAP_PTR_X2,MAP_PTR_X,MAP_PTR_X,MAP_COLOR1,MAP_COLOR1,XGR_FILLED);
						break;
					case 2:
						XGR_SetPixel(x + scr_x,y + scr_y,(_y >> 8) & 255);
						break;
					case 3:
						XGR_Rectangle(x + scr_x - MAP_PTR_X2,y + scr_y - MAP_PTR_X2,MAP_PTR_X,MAP_PTR_X,MAP_COLOR3,MAP_COLOR3,XGR_FILLED);
						break;
				}
			}
		}
	}
#else
	index = getObjectPosition(_x,_y);
	while(index != -1){
		if(index){
			scr_x = -1;
			scr_x0 = ((_x * p -> SizeX) >> MAP_POWER_X);

			if(cx < sx2){
				if(p -> SizeX - scr_x0 < dx)
					scr_x = scr_x0 - (p -> SizeX - dx);
				else {
					if(scr_x0 < sx - dx)
						scr_x = scr_x0 + dx;
					else
						scr_x = -1;
				}
			}
			else {
				if(p -> SizeX - cx < sx2){
					if(p -> SizeX - scr_x0 < sx - dx)
						scr_x = scr_x0 - (p -> SizeX - (sx - dx));
					else {
						if(scr_x0 < dx)
							scr_x = scr_x0 + (sx - dx);
						else
							scr_x = -1;
					}
				}
				else {
					scr_x = scr_x0 - (cx - sx2);
				}
			}
			scr_y = ((_y * p -> SizeY) >> MAP_POWER_Y) - y0;
			if(scr_y < 0)
				scr_y += p -> SizeY;

			if(scr_x > MAP_PTR_X && scr_x < sx - MAP_PTR_X && scr_y > MAP_PTR_X && scr_y < sy - MAP_PTR_X){
				switch(index){
					case 1:
						XGR_Rectangle(x + scr_x - MAP_PTR_X2,y + scr_y - MAP_PTR_X2,MAP_PTR_X,MAP_PTR_X,MAP_COLOR1,MAP_COLOR1,XGR_FILLED);
						break;
#ifdef _ACI_SHOW_BUGS_ON_MAP_
					case 2:
						XGR_SetPixel(x + scr_x,y + scr_y,MAP_COLOR2);
						break;
#endif
					case 3:
						XGR_Rectangle(x + scr_x - MAP_PTR_X2,y + scr_y - MAP_PTR_X2,MAP_PTR_X,MAP_PTR_X,MAP_COLOR3,MAP_COLOR3,XGR_FILLED);
						break;
					case 4:
						XGR_Rectangle(x + scr_x - MAP_PTR_X2,y + scr_y - MAP_PTR_X2,MAP_PTR_X,MAP_PTR_X,aciMapVngColors[0],aciMapVngColors[0],XGR_FILLED);
						break;
					case 5:
						XGR_Rectangle(x + scr_x - MAP_PTR_X2,y + scr_y - MAP_PTR_X2,MAP_PTR_X,MAP_PTR_X,aciMapVngColors[1],aciMapVngColors[1],XGR_FILLED);
						break;
					case 6:
						XGR_Rectangle(x + scr_x - MAP_PTR_X2,y + scr_y - MAP_PTR_X2,MAP_PTR_X,MAP_PTR_X,aciMapVngColors[2],aciMapVngColors[2],XGR_FILLED);
						break;
					case 7:
						XGR_Rectangle(x + scr_x - MAP_PTR_X2,y + scr_y - MAP_PTR_X2,MAP_PTR_X,MAP_PTR_X,aciMapVngColors[3],aciMapVngColors[3],XGR_FILLED);
						break;
					case 8:
						if(scr_x > MAP_PTR_XL && scr_x < sx - MAP_PTR_XL && scr_y > MAP_PTR_XL && scr_y < sy - MAP_PTR_XL)
							XGR_Rectangle(x + scr_x - MAP_PTR_XL2,y + scr_y - MAP_PTR_XL2,MAP_PTR_XL,MAP_PTR_XL,aciMapVngColors[3],aciMapVngColors[0],XGR_FILLED);
						break;
					case 9:
						if(scr_x > MAP_PTR_XL && scr_x < sx - MAP_PTR_XL && scr_y > MAP_PTR_XL && scr_y < sy - MAP_PTR_XL)
							XGR_Rectangle(x + scr_x - MAP_PTR_XL2,y + scr_y - MAP_PTR_XL2,MAP_PTR_XL,MAP_PTR_XL,aciMapVngColors[3],aciMapVngColors[1],XGR_FILLED);
						break;
					case 10:
						if(scr_x > MAP_PTR_XL && scr_x < sx - MAP_PTR_XL && scr_y > MAP_PTR_XL && scr_y < sy - MAP_PTR_XL)
							XGR_Rectangle(x + scr_x - MAP_PTR_XL2,y + scr_y - MAP_PTR_XL2,MAP_PTR_XL,MAP_PTR_XL,aciMapVngColors[3],aciMapVngColors[2],XGR_FILLED);
						break;
					case 11:
						XGR_Rectangle(x + scr_x - MAP_PTR_X2,y + scr_y - MAP_PTR_X2,MAP_PTR_X,MAP_PTR_X,aciMapVngColors[5],aciMapVngColors[5],XGR_FILLED);
						break;
					case 12:
						XGR_Rectangle(x + scr_x - MAP_PTR_X2,y + scr_y - MAP_PTR_X2,MAP_PTR_X,MAP_PTR_X,aciMapVngColors[6],aciMapVngColors[6],XGR_FILLED);
						break;
					case 13:
						XGR_Rectangle(x + scr_x - MAP_PTR_X2,y + scr_y - MAP_PTR_X2,MAP_PTR_X,MAP_PTR_X,aciMapVngColors[7],aciMapVngColors[7],XGR_FILLED);
						break;
					case 14:
						XGR_Rectangle(x + scr_x - MAP_PTR_X2,y + scr_y - MAP_PTR_X2,MAP_PTR_X,MAP_PTR_X,aciMapVngColors[8],aciMapVngColors[8],XGR_FILLED);
						break;
					case 15:
						XGR_Rectangle(x + scr_x - MAP_PTR_X2,y + scr_y - MAP_PTR_X2,MAP_PTR_X,MAP_PTR_X,aciMapVngColors[9],aciMapVngColors[9],XGR_FILLED);
						break;
					case 16:
						XGR_Rectangle(x + scr_x - MAP_PTR_X2,y + scr_y - MAP_PTR_X2,MAP_PTR_X,MAP_PTR_X,aciMapVngColors[10],aciMapVngColors[10],XGR_FILLED);
						break;
				}
			}
		}
		index = getObjectPosition(_x,_y);
	}
#endif
}

void load_map_bmp(int num)
{
	aScrDisp -> mapObj -> free();
	if(aScrDisp -> wMap -> world_ids[num] == -1 || !(aScrDisp -> map_names[aScrDisp -> wMap -> world_ids[num]])) ErrH.Abort("Map BMP not present...");
	aScrDisp -> mapObj -> load(aScrDisp -> map_names[aScrDisp -> wMap -> world_ids[num]],1);
	aScrDisp -> unlock();
	aciPrepareWorldsMenu();
	aciChangeWorld(num);
}

void change_screen(int mode)
{
	aScrDisp -> change_ibs(mode);
	aScrDisp -> flags &= ~AS_FULL_REDRAW;
//	set_screen(XGR_MAXX/2, XGR_MAXY/2, 0, XGR_MAXX/2, XGR_MAXY/2);
}

void aciSendEvent2actint(int code,actintItemData* p,int data)
{
	aScrDisp -> send_event(code,data,p);
}

actintItemData* aciGetLast(void)
{
	invItem* itm;
	invItem* p = (invItem*)aScrDisp -> curMatrix -> items -> last;
	itm = p;
	while(p){
		if(p -> dropCount > itm -> dropCount)
			itm = p;

		p = (invItem*)p -> prev;
	}
	if(itm)
		return itm -> item_ptr;
	return NULL;
}

const double _PI128	= 3.14/128.0;
void a_calc_tables(void)
{
	int i;
	double phi;
	if(!aSinTable && !aCosTable){
		aSinTable = new int[256];
		aCosTable = new int[256];

		for(i = 0; i < 256; i ++){
			phi = ((double)i) * _PI128;
			aSinTable[i] = (int)(sin(phi) * 256.0);
			aCosTable[i] = (int)(cos(phi) * 256.0);
		}
	}
}

void a_buf_setpixel(int x,int y,int sx,int sy,int col,unsigned char* buf)
{
	buf[x + y * sx] = col;
}

void a_buf_line(int x1,int y1,int x2,int y2,int col,int sx,int sy,unsigned char* buf)
{
	int dx,dy,xinc,yinc,err,cnt,index,i_xinc,i_yinc;

	dx = x2 - x1;
	dy = y2 - y1;

	xinc = (dx > 0) ? 1 : (dx < 0) ? (-1) : 0;
	yinc = (dy > 0) ? 1 : (dy < 0) ? (-1) : 0;

	i_xinc = xinc;
	i_yinc = (dy > 0) ? sx : (dy < 0) ? (-sx) : 0;

	dx = abs(dx);
	dy = abs(dy);
	index = x1 + y1 * sx;
	if(dx > dy){
		err = dx/2;
		cnt = dx + 1;
		while(cnt--){
			buf[index] = col;
			x1 += xinc;
			index += i_xinc;

			err += dy;
			if(err >= dx){
				if(cnt)
					buf[index] = col;
				err -= dx;
				y1 += yinc;
				index += i_yinc;
			}
		}
	}
	else {
		err = dy / 2;
		cnt = dy + 1;
		while(cnt--){
			buf[index] = col;
			y1 += yinc;
			index += i_yinc;
			err += dx;
			if(err >= dy){
				if(cnt)
					buf[index] = col;
				err -= dy;
				x1 += xinc;
				index += i_xinc;
			}
		}
	}
}

void a_buf_line2(int x1,int y1,int x2,int y2,int col,int sx,int sy,unsigned char* buf)
{
	int dx,dy,xinc,yinc,err,cnt,index,i_xinc,i_yinc;

	dx = x2 - x1;
	dy = y2 - y1;

	xinc = (dx > 0) ? 1 : (dx < 0) ? (-1) : 0;
	yinc = (dy > 0) ? 1 : (dy < 0) ? (-1) : 0;

	i_xinc = xinc;
	i_yinc = (dy > 0) ? sx : (dy < 0) ? (-sx) : 0;

	dx = abs(dx);
	dy = abs(dy);
	index = x1 + y1 * sx;
	if(dx > dy){
		err = dx/2;
		cnt = dx + 1;
		while(cnt--){
			buf[index] = col;
			x1 += xinc;
			index += i_xinc;

			err += dy;
			if(err >= dx){
				err -= dx;
				y1 += yinc;
				index += i_yinc;
			}
		}
	}
	else {
		err = dy / 2;
		cnt = dy + 1;
		while(cnt--){
			buf[index] = col;
			y1 += yinc;
			index += i_yinc;
			err += dx;
			if(err >= dy){
				err -= dy;
				x1 += xinc;
				index += i_xinc;
			}
		}
	}
}

void fill_buf(int fx,int fy,int src_col,int dest_col,int sx,int sy,unsigned char* buf)
{
	int x,y,flag = 1,index;

	buf[fx + fy * sx] = dest_col;

	while(flag){
		index = 0;
		flag = 0;
		for(y = 0; y < sy; y ++){
			for(x = 0; x < sx; x ++){
				if(buf[index] == src_col){
					if(x && buf[index - 1] == dest_col){
						flag = 1;
						buf[index] = dest_col;
					}
					if(x < sx - 1 && buf[index + 1] == dest_col){
						flag = 1;
						buf[index] = dest_col;
					}
					if(y && buf[index - sx] == dest_col){
						flag = 1;
						buf[index] = dest_col;
					}
					if(y < sy - 1 && buf[index + sx] == dest_col){
						flag = 1;
						buf[index] = dest_col;
					}
				}
				index ++;
			}
		}
	}
}

void free_cell_frame(void)
{
	delete[] aCellFrame;
}

void build_cell_frame(void)
{
	int dx = aCellSize;
	int dxx = dx * dx;
	int dx2 = dx >> 1;
	int dx4 = dx >> 2;

	unsigned char* cell = new unsigned char[(dx + 1) * (dx + 1)];
	memset(cell,0,(dx + 1) * (dx + 1));
/*
	a_buf_line(dx2,0,dx,dx4,1,dx + 1,dx + 1,cell);
	a_buf_line(dx,dx4,dx,dx2 + dx4,1,dx + 1,dx + 1,cell);
	a_buf_line(dx,dx2 + dx4,dx2,dx,1,dx + 1,dx + 1,cell);
	a_buf_line(dx2,dx,0,dx2 + dx4,1,dx + 1,dx + 1,cell);
	a_buf_line(0,dx2 + dx4,0,dx4,1,dx + 1,dx + 1,cell);
	a_buf_line(0,dx4,dx2,0,1,dx + 1,dx + 1,cell);
*/
	a_buf_line(dx2,0,dx,dx4,1,dx + 1,dx + 1,cell);
	a_buf_line(0,dx - dx4,dx2,dx,1,dx + 1,dx + 1,cell);

	a_buf_line(dx,dx4,dx,dx - dx4,1,dx + 1,dx + 1,cell);
	a_buf_line(0,dx - dx4,0,dx4,1,dx + 1,dx + 1,cell);

	a_buf_line(dx,dx - dx4,dx2,dx,1,dx + 1,dx + 1,cell);
	a_buf_line(dx2,0,0,dx4,1,dx + 1,dx + 1,cell);

	fill_buf(dx2,dx2,0,2,dx + 1,dx + 1,cell);

	aCellFrame = cell;
}

void swap_buf_col(int src,int dest,int sx,int sy,unsigned char* buf)
{
	int i,j,index = 0;
	for(i = 0; i < sy; i ++){
		for(j = 0; j < sx; j ++){
			if(buf[index] == src) buf[index] = dest;
			index ++;
		}
	}
}

void mem_putspr(int x,int y,int sx,int sy,int bsx,int bsy,unsigned char* src,unsigned char* dest)
{
	int i,j,index = 0,b_index = 0;

	index = x + y * bsx;
	for(i = 0; i < sy; i ++){
		for(j = 0; j < sx; j ++){
			if(src[b_index])
				dest[index + j] = src[b_index];
			b_index ++;
		}
		index += bsx;
	}
}

void mem_putspr_h(int x,int y,int sx,int sy,int bsx,int bsy,unsigned char* src,unsigned char* dest)
{
	int i,j,index = x + y * bsx,b_index = 0;
	int h,h1;

	for(i = 0; i < sy; i ++){
		for(j = 0; j < sx; j ++){
			if(src[b_index]){
				h1 = dest[index + j];
				h = src[b_index];
				if(h > h1)
					dest[index + j] = h;
			}
			b_index ++;
		}
		index += bsx;
	}
}

void aci_iScreenQuant(void)
{
	aScrDisp -> iKeyQuant();
	aScrDisp -> EventQuant();

	aciML_D -> quant();

	aScrDisp -> i_redraw();
	aScrDisp -> i_flush();
}

void aci_SendEvent(int cd,int dt)
{
	aScrDisp -> send_event(cd,dt);
}

void aci_LocationQuantPrepare(void)
{
	aciSpeechBegLocked = aciSpeechEndLocked = 0;
	aciSpeechStarted = aciEndSpeech = 0;
	aciEscaveDead = aciEscaveEmpty = 0;
	aciBreathFlag = 0;

	aScrDisp -> i_init();
	iScrQuantPrepare();
}

void aci_LocationQuantFinit(void)
{
	iScrQuantFinit();
	aScrDisp -> i_finit();
	aciKillLinks();
	aScrDisp -> flags &= ~AS_FULL_REDRAW;
	aciAutoRun = iGetOptionValue(iAUTO_ACCELERATION);
}

void loadMouseFrames(void)
{
	XStream fh;
	if(actintLowResFlag)
		fh.open("resource/actint/640x480/mouse.bml",XS_IN);
	else
		fh.open("resource/actint/800x600/mouse.bml",XS_IN);
	fh > aMouseSizeX > aMouseSizeY > aMouseSize;
	aMouseFrames = new unsigned char[aMouseSizeX * aMouseSizeY * aMouseSize];
	fh.read(aMouseFrames,aMouseSizeX * aMouseSizeY * aMouseSize);
	fh.close();

	fh.open("resource/iscreen/mouse.bml");
	fh > iMouseSizeX > iMouseSizeY > iMouseSize;
	iMouseFrames = new unsigned char[iMouseSizeX * iMouseSizeY * iMouseSize];
	fh.read(iMouseFrames,iMouseSizeX * iMouseSizeY * iMouseSize);
	fh.close();

	MouseSize2 = iMouseSize/2;
}

int aciReInitItem(actintItemData* p)
{
	int id;
	invItem* it = aScrDisp -> get_item_ptr(p -> type);
	if(!it) {
		std::cout<<"aciReInitItem !it p:"<<p<<std::endl;
		return 0;
	}
	it -> item_ptr = p;
	p -> actintOwner = it;

	if(it -> slotType != -1){
		id = aScrDisp -> curMatrix -> get_item_slot(it);
		if(id != -1)
			aciSendEvent2itmdsp(ACI_ACTIVATE_ITEM,p,id);
	}
	return 1;
}

int aciReInitItemXY(actintItemData* p,int x,int y)
{
	int id;
	invItem* it = aScrDisp->get_item_ptr_xy(p->type, x, y);
	if(!it) {
		std::cout<<"aciReInitItemXY !it p:"<<p<<std::endl;
		return 0;
	}
	it -> item_ptr = p;
	p -> actintOwner = it;

	if(it -> slotType != -1){
		id = aScrDisp -> curMatrix -> get_item_slot(it);
		if(id != -1)
			aciSendEvent2itmdsp(ACI_ACTIVATE_ITEM,p,id);
	}
	return 1;
}

void aciKillLinks(void)
{
	if(!aScrDisp -> curMatrix) return;
	invItem* p = (invItem*)aScrDisp -> curMatrix -> items -> last;
	while(p){
		p -> item_ptr = NULL;
		p = (invItem*)p -> prev;
	}
}

void aciAutoMoveItems(void)
{
	int i,x,y,sz;
	invItem* p,*p1;
	if(!aScrDisp -> curMatrix || !aScrDisp -> secondMatrix)
		return;

	p = (invItem*)aScrDisp -> curMatrix -> items -> last;
	sz = aScrDisp -> curMatrix -> items -> Size;
	for(i = 0; i < sz; i ++){
		p1 = (invItem*)p -> prev;
		x = p -> MatrixX;
		y = p -> MatrixY;
		aScrDisp -> curMatrix -> remove_item(p);
		if(!aScrDisp -> secondMatrix -> auto_put_item(p)){
			aScrDisp -> curMatrix -> put_item(x,y,p);
		}
		else {
			((uvsActInt*)p -> uvsDataPtr) -> pos_x = p -> MatrixX;
			((uvsActInt*)p -> uvsDataPtr) -> pos_y = p -> MatrixY;
		}
		p = p1;
	}
	SOUND_DROP();
	aScrDisp -> flags &= ~AS_FULL_REDRAW;
}

void aciSetMechName(void)
{
	iScreenObject* obj;
	iStringElement* str;
	if(!aScrDisp -> secondMatrix) return;

	obj = (iScreenObject*)aScrDisp -> curLocData -> objList[ACI_MECHOS_ID_OBJ];
	str = (iStringElement*)aScrDisp -> curLocData -> objList[ACI_MECHOS_ID_STR];
	if(!obj || !str) return;

	if(aScrDisp -> secondMatrix -> mech_name)
		strcpy(str -> string,aScrDisp -> secondMatrix -> mech_name);
	else
		strcpy(str -> string," ");

	str -> SizeX = str -> SizeY = 0;
	str -> lX = str -> lY = 0;
	obj -> init();
}

uvsActInt* aciGetMechos(int id)
{
	uvsActInt* p = GMechos;
	
	while(p){
		if(p -> type == id)
			return p;
		p = (uvsActInt*)p -> next;
	}
	return NULL;
}

uvsActInt* aciGetGamerItem(int id)
{
	uvsActInt* p = GGamer;

	while(p){
		if(p -> type == id)
			return p;
		p = (uvsActInt*)p -> next;
	}
	return NULL;
}

uvsActInt* aciGetShopItem(int id)
{
	uvsActInt* p = GItem;

	while(p){
		if(p -> type == id)
			return p;
		p = (uvsActInt*)p -> next;
	}
	return NULL;
}

void aciSell_Item(invItem* p)
{
	int cr;
	invItem* it = aScrDisp -> get_iitem(p -> ID);
	uvsActInt* ptr = (uvsActInt*)p -> uvsDataPtr;
	listElem* el = (listElem*)GItem;

	if(!ptr) ErrH.Abort("Bad sell item...");

	ptr -> delink(GGamer);
	ptr -> link(el);
	GItem = (uvsActInt*)el;

	if(!(p -> flags & INV_ITEM_NO_PAY))
		uvsActIntSell(ptr);

	if(!it -> uvsDataPtr && (!aScrDisp -> curLocData -> ExcludeItems[p -> ID] || (p -> flags & INV_ITEM_NO_PAY))){
		it -> uvsDataPtr = ptr;
	}

	if(!(p -> flags & INV_ITEM_NO_PAY)){
		cr = aciGetCurCredits();
		cr += ptr -> sell_price;
		aciUpdateCurCredits(cr);
	}

	aScrDisp -> free_item(p);
}

void aciSell_Matrix(invMatrix* p)
{
	int cr;
	invItem* itm = (invItem*)p -> items -> last,*itm1;
	uvsActInt* ptr = (uvsActInt*)p -> uvsDataPtr;
	if(!ptr) ErrH.Abort("Bad sell mechos...");
	while(itm){
		itm1 = (invItem*)itm -> prev;
		p -> items -> dconnect((iListElement*)itm);
		aciSell_Item(itm);
		itm = itm1;
	}
	cr = aciGetCurCredits();
	cr += ptr -> sell_price;
	aciUpdateCurCredits(cr);
}

int aciCheckSellItem(void)
{
	int x = iMouseX;
	iScreenObject* p;
	iAVIElement* el = (iAVIElement*)aScrDisp -> curLocData -> objList[ACI_MECHOS_AVI_ID];
	if(!el) return 0;
	p = (iScreenObject*)el -> owner;
	if(p -> owner) x += ((iScreen*)p -> owner) -> ScreenOffs;
	if(p -> CheckXY(x,iMouseY)) return 1;
	return 0;
}

void aciInitInfoPanels(void)
{
	InfoPanel* pl;
	CounterPanel* cp;
	iBitmapElement* el;
	fncMenu* mn;
	iScreenObject* obj;
	iScreen* scr;
	iAVIElement* avi;

	el = (iBitmapElement*)aScrDisp -> curLocData -> s_objList[ACI_QUEST_PANEL_ID];
	pl = aScrDisp -> get_info_panel(ACI_QUEST_PANEL);
	if(el && pl){
		obj = (iScreenObject*)el -> owner;
		scr = (iScreen*)obj -> owner;

		pl -> PosX = obj -> PosX + el -> lX - scr -> ScreenOffs;
		pl -> PosY = obj -> PosY + el -> lY;

		pl -> iScreenOwner = el;
		pl -> SizeX = el -> bSizeX;
		pl -> SizeY = el -> bSizeY;

		pl -> free_list();
	}
	el = (iBitmapElement*)aScrDisp -> curLocData -> s_objList[ACI_ANSWER_PANEL_ID];
	pl = aScrDisp -> get_info_panel(ACI_ANSWER_PANEL);
	if(el && pl){
		obj = (iScreenObject*)el -> owner;
		scr = (iScreen*)obj -> owner;

		pl -> PosX = obj -> PosX + el -> lX - scr -> ScreenOffs;
		pl -> PosY = obj -> PosY + el -> lY;

		pl -> iScreenOwner = el;
		pl -> SizeX = el -> bSizeX;
		pl -> SizeY = el -> bSizeY;

		pl -> free_list();
		pl -> set_redraw();
	}
	el = (iBitmapElement*)aScrDisp -> curLocData -> s_objList[ACI_TIME_PANEL_ID];
	pl = aScrDisp -> get_info_panel(ACI_SHOP_TIME_PANEL);
	if(el && pl){
		obj = (iScreenObject*)el -> owner;
		scr = (iScreen*)obj -> owner;

		pl -> PosX = obj -> PosX + el -> lX - scr -> ScreenOffs;
		pl -> PosY = obj -> PosY + el -> lY;

		pl -> iScreenOwner = el;
		pl -> SizeX = el -> bSizeX;
		pl -> SizeY = el -> bSizeY;

		pl -> free_list();
		pl -> set_redraw();
	}
	el = (iBitmapElement*)aScrDisp -> curLocData -> objList[ACI_PRICE_PANEL_ID];
	pl = aScrDisp -> get_info_panel(ACI_PRICE_PANEL);
	if(el && pl){
		obj = (iScreenObject*)el -> owner;
		scr = (iScreen*)obj -> owner;

		pl -> PosX = obj -> PosX + el -> lX - scr -> ScreenOffs;
		pl -> PosY = obj -> PosY + el -> lY;

		pl -> iScreenOwner = el;
		pl -> SizeX = el -> bSizeX;
		pl -> SizeY = el -> bSizeY;

		pl -> free_list();
		pl -> set_redraw();
	}
	el = (iBitmapElement*)aScrDisp -> curLocData -> objList[ACI_INFO_PANEL_ID];
	pl = aScrDisp -> iscr_iP;
	if(el && pl){
		obj = (iScreenObject*)el -> owner;
		scr = (iScreen*)obj -> owner;

		pl -> PosX = obj -> PosX + el -> lX - scr -> ScreenOffs;
		pl -> PosY = obj -> PosY + el -> lY;

		pl -> iScreenOwner = el;
		pl -> SizeX = el -> bSizeX;
		pl -> SizeY = el -> bSizeY;

		pl -> free_list();
		pl -> set_redraw();
	}
	el = (iBitmapElement*)aScrDisp -> curLocData -> objList[ACI_CREDITS_PANEL_ID];
	cp = aScrDisp -> get_icounter(ACI_CREDITS_PANEL);
	if(el && cp){
		obj = (iScreenObject*)el -> owner;
		scr = (iScreen*)obj -> owner;

		cp -> PosX = obj -> PosX + el -> lX - scr -> ScreenOffs;
		cp -> PosY = obj -> PosY + el -> lY;

		cp -> iScreenOwner = el;
		cp -> SizeX = el -> bSizeX;
		cp -> SizeY = el -> bSizeY;
	}
	el = (iBitmapElement*)aScrDisp -> curLocData -> s_objList[ACI_QUEST_PANEL_ID];
	mn = aScrDisp -> get_imenu(ACI_QUEST_MENU);
	if(aScrDisp -> qMenu && el){
		obj = (iScreenObject*)el -> owner;
		scr = (iScreen*)obj -> owner;

		aScrDisp -> qMenu -> data = el;

		aScrDisp -> qMenu -> ScreenX = obj -> PosX + el -> lX - scr -> ScreenOffs;
		aScrDisp -> qMenu -> ScreenY = obj -> PosY + el -> lY;

		aScrDisp -> qMenu -> ScreenSizeX = obj -> SizeX;
		aScrDisp -> qMenu -> ScreenSizeY = obj -> SizeY;
	}
	el = (iBitmapElement*)aScrDisp -> curLocData -> objList[ACI_ITEMS_MENU_ID];
	mn = aScrDisp -> get_imenu(SHOP_ITEMS_MENU_ID);
	if(mn && el){
		obj = (iScreenObject*)el -> owner;
		scr = (iScreen*)obj -> owner;

		mn -> iScreenOwner = el;

		mn -> SizeX = el -> bSizeX;
		mn -> SizeY = el -> bSizeY;

		mn -> PosX = obj -> PosX + el -> lX - scr -> ScreenOffs;
		mn -> PosY = obj -> PosY + el -> lY;

		aScrDisp -> clear_menu(mn);
	}
	avi = (iAVIElement*)aScrDisp -> curLocData -> objList[ACI_MECHOS_AVI_ID];
	if(avi){
		obj = (iScreenObject*)avi -> owner;
		if(obj -> flags & OBJ_TRIGGER){
			((iTriggerObject*)obj) -> state = 0;
			((iTriggerObject*)obj) -> trigger_init();
		}
	}
}

void aciInitAviObject(void)
{
	fncMenu* mn;
	iScreenObject* obj;
	iAVIElement* avi;

	avi = (iAVIElement*)aScrDisp -> curLocData -> objList[ACI_MECHOS_AVI_ID];
	if(avi){
		obj = (iScreenObject*)avi -> owner;
		if(obj -> flags & OBJ_TRIGGER){
			((iTriggerObject*)obj) -> state = 0;
			((iTriggerObject*)obj) -> trigger_init();
		}
	}
	mn = aScrDisp -> get_imenu(SHOP_ITEMS_MENU_ID);
	if(mn) mn -> flags &= ~FM_ACTIVE;

	switch(iEvLineID){
		case MECHOS_LIST_MODE:
			iEvLineID = MECHOS_MODE;
			break;
		case ITEMS_LIST_MODE:
			iEvLineID = ITEMS_MODE;
			break;
		case WEAPONS_LIST_MODE:
			iEvLineID = WEAPONS_MODE;
			break;
	}
	aciCurAviIndex = ACI_PICTURE_AVI_ENG;
}

void redraw_fnc(int x,int y,int sx,int sy,int fnc)
{
	switch(fnc){
		case FMENU_MAP:
			show_map(x,y,sx,sy);
			break;
		case FMENU_WMAP:
			if(aScrDisp -> wMap){
				aScrDisp -> wMap -> redraw(x + ((sx - aScrDisp -> wMap -> SizeX) >> 1),y + ((sy - aScrDisp -> wMap -> SizeY) >> 1));
			}
			else {
				XGR_Rectangle(x,y,sx,sy,0,0,XGR_FILLED);
			}
			break;
		case FMENU_TARGETS:
			break;
		case FMENU_CAMERAS:
			aciShowCamerasMenu(x,y,sx,sy);
			break;
		case FMENU_PARAMETERS:
			aciRedrawParamsPanel(x,y,sx,sy);
			break;
		default:
			XGR_Rectangle(x,y,sx,sy,0,0,XGR_FILLED);
			break;
	}
}

void aciSetTimePanel(void)
{
	char* p;
	int d,h,m,s;

	InfoPanel* iPl;
	if(aScrDisp -> flags & AS_ISCREEN)
		iPl = aScrDisp -> get_info_panel(ACI_SHOP_TIME_PANEL);
	else
		iPl = aScrDisp -> get_aci_info_panel(ACI_TIME_PANEL);

	if(!iPl) return;

	ConTimer.sTime.init();

	if(NetworkON){
		GetNetworkGameTime(d,h,m,s);
	}
	else {
		d = ConTimer.day;
		h = ConTimer.hour;
		m = ConTimer.min;
		s = ConTimer.sec;
	}

	if(d < 100) ConTimer.sTime < " ";
	if(d < 10) ConTimer.sTime < " ";

	ConTimer.sTime <= d < " ";
	if(h < 10) ConTimer.sTime < "0";
	ConTimer.sTime <= h < ":";
	if(m < 10) ConTimer.sTime < "0";
	ConTimer.sTime <= m < ":";
	if(s < 10) ConTimer.sTime < "0";
	ConTimer.sTime <= s;

	p = ConTimer.sTime.GetBuf();

	iPl -> free_list();
	iPl -> add_item(aciSTR_DAY);
	iPl -> add_item(p,-1,(aciCurColorScheme[FM_SELECT_BORDER_COL] << 8) | aciCurColorScheme[FM_SELECT_COL]);

	if(aScrDisp -> flags & AS_ISCREEN)
		iPl -> set_redraw();
}

void aciInitEvLineID(void)
{
	fncMenu* mn;
	iTriggerObject* p = (iTriggerObject*)aScrDisp -> curLocData -> objList[ACI_TRIGGER_ID];
	if(!p) return;

	if(GMechos){
		p -> state = 2;
		p -> init_state();
	}
	else {
		p -> state = 0;
		p -> init_state();
	}
	iEvLineID = p -> state + 1;

	p = (iTriggerObject*)aScrDisp -> curLocData -> objList[ACI_AVI_OBJ_ID];
	if(p){
		mn = aScrDisp -> get_imenu(SHOP_ITEMS_MENU_ID);
		if(mn) mn -> flags &= ~FM_ACTIVE;
		p -> state = 0;
		p -> init_state();
	}
	switch(uvsBunchNumber){
		case 2:
			aciCurAviIndexDelta = 0;
			break;
		case 0:
			aciCurAviIndexDelta = ACI_PICTURE_AVI_ENG2;
			break;
		case 1:
			aciCurAviIndexDelta = ACI_PICTURE_AVI_ENG3;
			break;
	}
}

void aciInitWorldIDs(void)
{
	int i;
	signed char* ids;
	uvsWorld* wrld;
	aciWorldInfo* a_wrld;
	if(!aScrDisp -> wMap) return;
	ids = aScrDisp -> wMap -> world_ids;

	memset(ids,-1,AS_MAX_WORLD);

	for(i = 0; i < WORLD_MAX; i ++){
		wrld = WorldTable[i];
		a_wrld = aScrDisp -> wMap -> get_world_p(wrld -> name);
		if(a_wrld){
			ids[i] = a_wrld -> ID;
			a_wrld -> uvsID = i;
		}
	}
	aScrDisp -> flags |= AS_WORLDS_INIT;
//	  aciPrepareWorldsMenu();
}

void aciWorldLinksON(void)
{
	int i,cur_id;
	aciWorldInfo* p,*p1;

	if(CurrentWorld == -1) return;

	cur_id = aScrDisp -> wMap -> world_ids[CurrentWorld];
	if(cur_id == -1) return;

	p = aScrDisp -> wMap -> world_ptr[cur_id];
	if(!p) return;

	for(i = 0; i < AS_MAX_WORLD; i ++){
		p1 = aScrDisp -> wMap -> world_ptr[i];
		if(i != cur_id && p1){
			if(p1 -> links[cur_id] || p -> links[i]){
				p1 -> flags |= AS_ACCESSIBLE_WORLD;
			}
		}
		if(p1 && uvsReturnWorldGamerVisit(p1 -> uvsID)){
			p1 -> flags |= AS_EXPLORED_WORLD;
		}
	}
	aScrDisp -> wMap -> flags |= WMAP_REBUILD_PIC;
}

void aciWorldExploredInfo(void)
{
	int i;
	aciWorldInfo* p1;

	for(i = 0; i < AS_MAX_WORLD; i ++){
		p1 = aScrDisp -> wMap -> world_ptr[i];
		if(p1 && uvsReturnWorldGamerVisit(p1 -> uvsID)){
			p1 -> flags |= AS_EXPLORED_WORLD;
		}
	}
}

void aciWorldLinksOFF(void)
{
	int i;
	aciWorldInfo* p;

	for(i = 0; i < AS_MAX_WORLD; i ++){
		p = aScrDisp -> wMap -> world_ptr[i];
		if(p){
			p -> flags &= ~AS_ACCESSIBLE_WORLD;
		}
	}
	aScrDisp -> wMap -> flags |= WMAP_REBUILD_PIC;
}

void aciOpenWorldLink(int w1,int w2)
{
	int id1,id2;
	aciWorldInfo* p1,*p2;

	id1 = aScrDisp -> wMap -> world_ids[w1];
	id2 = aScrDisp -> wMap -> world_ids[w2];

	if(id1 == -1 || id2 == -1) ErrH.Abort("Bad world ID...");

	p1 = aScrDisp -> wMap -> world_ptr[id1];
	p2 = aScrDisp -> wMap -> world_ptr[id2];

	if(!p1 || !p2) ErrH.Abort("Bad world ptr...");

	p1 -> links[id2] = 1;
	if(id1 != id2 && p2 -> links[id1]) p2 -> links[id1] = 0;
	aScrDisp -> wMap -> flags |= WMAP_REBUILD_PIC;
	aciPrevJumpCount = -1;
}

int aciWorldLinkExist(int w1,int w2,int sp_flag)
{
	int id1,id2;
	aciWorldInfo* p1,*p2;

	id1 = aScrDisp -> wMap -> world_ids[w1];
	id2 = aScrDisp -> wMap -> world_ids[w2];

	if(id1 == -1 || id2 == -1) ErrH.Abort("Bad world ID...");

	p1 = aScrDisp -> wMap -> world_ptr[id1];
	p2 = aScrDisp -> wMap -> world_ptr[id2];

	if(!p1 || !p2) ErrH.Abort("Bad world ptr...");

	if(p1 -> links[id2] || p2 -> links[id1]){
		if(!sp_flag || (p1 -> flags & AS_ACCESSIBLE_WORLD || p2 -> flags & AS_ACCESSIBLE_WORLD)){
			return 1;
		}
	}

	return 0;
}

void aciInitAviID(void)
{
	invItem* p;
	invMatrix* m;
	iAVIElement* avi;

	InfoPanel* ip = aScrDisp -> get_info_panel(ACI_PRICE_PANEL);

	avi = (iAVIElement*)aScrDisp -> curLocData -> objList[ACI_MECHOS_AVI_ID];
	if(!avi) return;

	if(ip){
		ip -> free_list();
		ip -> set_redraw();
	}

	if(!iShopItem){
		aciXConv -> init();
		*aciXConv < iVideoPath < "empty.avi";
		strcpy(avi -> avi_name,aciXConv -> address());
		iEvLineID = EMPTY_MODE;
		return;
	}

	if(iEvLineID == MECHOS_MODE || iEvLineID == MECHOS_LIST_MODE || iEvLineID == CHANGE_2_MECHOS_MODE){
		m = (invMatrix*)iShopItem;
		aci_SecondMatrixID = m -> type;
		if(!aciCurAviIndex){
			if(m -> avi_ids && m -> avi_ids[aciCurAviIndex + aciCurAviIndexDelta]){
				strcpy(avi -> avi_name,m -> avi_ids[aciCurAviIndex + aciCurAviIndexDelta]);
			}
		}
		else {
			if(m -> avi_ids && m -> avi_ids[aciCurAviIndex]){
				strcpy(avi -> avi_name,m -> avi_ids[aciCurAviIndex]);
			}
		}
		if(ip && m -> mech_name){
			aciInitPricePanel(ip,(iListElement*)m,MECHOS_MODE,0);

//			  ip -> add_item(m -> mech_name,-1,aciCurColorScheme[FM_SELECT_COL]);
//			  ip -> add_item(aciGetPrice((iListElement*)m,MECHOS_MODE,0),-1,aciCurColorScheme[FM_SELECT_COL]);
		}
	}
	else {
		p = (invItem*)iShopItem;
		if(p -> avi_ids && p -> avi_ids[aciCurAviIndex]){
			strcpy(avi -> avi_name,p -> avi_ids[aciCurAviIndex]);
		}
		if(ip){
			aciInitPricePanel(ip,(iListElement*)p,ITEMS_MODE,0);
		}
	}
}

void aciInitShopAvi(void)
{
	iAVIElement* avi;

	avi = (iAVIElement*)aScrDisp -> curLocData -> objList[ACI_MECHOS_AVI_ID];
	if(!avi) return;

	avi -> change_avi(avi -> avi_name);
	avi -> init_size();
	avi -> init_align();
}

void aciNextShopItem(void)
{
	invItem* p,*fp;
	invMatrix* m,*fm;
	if(!iShopItem){
		aciInitEvLineID();
		aciChangeList();
		return;
	}

	aciShopMenuDown();

	switch(iEvLineID){
		case MECHOS_MODE:
		case MECHOS_LIST_MODE:
			fm = (invMatrix*)iShopItem;
			m = (invMatrix*)fm -> next;
			if(m != fm){
				while(!m -> uvsDataPtr){
					m = (invMatrix*)m -> next;
					if(m == fm) break;
				}
				if(!m -> uvsDataPtr){
					iShopItem = NULL;
				}
				else {
					iShopItem = (iListElement*)m;
					aciCurMechosID = m -> type;
				}
			}
			break;
		case ITEMS_MODE:
		case ITEMS_LIST_MODE:
		case CHANGE_2_ITEMS_MODE:
			fp = (invItem*)iShopItem;
			p = (invItem*)fp -> next;
			if(p != fp){
				while(!p -> uvsDataPtr || p -> classID != AS_ITEM_CLASS){
					p = (invItem*)p -> next;
					if(p == fp) break;
				}
				if(!p -> uvsDataPtr || p -> classID != AS_ITEM_CLASS)
					iShopItem = NULL;
				else
					iShopItem = (iListElement*)p;
			}
			break;
		case WEAPONS_MODE:
		case WEAPONS_LIST_MODE:
		case CHANGE_2_WEAPONS_MODE:
			fp = (invItem*)iShopItem;
			p = (invItem*)fp -> next;
			if(p != fp){
				while(!p -> uvsDataPtr || p -> classID != AS_WEAPON_CLASS){
					p = (invItem*)p -> next;
					if(p == fp) break;
				}
				if(!p -> uvsDataPtr || p -> classID != AS_WEAPON_CLASS)
					iShopItem = NULL;
				else
					iShopItem = (iListElement*)p;
			}
			break;
	}
	aciInitAviID();
}

void aciSetShopItem(int id)
{
	invItem* p;
	invMatrix* m;
	if(!iShopItem){
		aciInitEvLineID();
		aciChangeList();
		return;
	}

	switch(iEvLineID){
		case MECHOS_MODE:
		case MECHOS_LIST_MODE:
		case CHANGE_2_MECHOS_MODE:
			m = (invMatrix*)aScrDisp -> i_matrixList -> last;
			while(m){
				if(m -> type == id){
					iShopItem = (iListElement*)m;
					aciCurMechosID = m -> type;
					break;
				}
				m = (invMatrix*)m -> prev;
			}
			if(!m) iShopItem = NULL;
			break;
		case ITEMS_MODE:
		case ITEMS_LIST_MODE:
		case CHANGE_2_ITEMS_MODE:
		case WEAPONS_MODE:
		case WEAPONS_LIST_MODE:
		case CHANGE_2_WEAPONS_MODE:
			p = (invItem*)aScrDisp -> i_itemList -> last;
			while(p){
				if(p -> ID == id){
					iShopItem = (iListElement*)p;
					break;
				}
				p = (invItem*)p -> prev;
			}
			if(!p) iShopItem = NULL;
			break;
	}
	aciInitAviID();
}

void aciPrevShopItem(void)
{
	invItem* p,*fp;
	invMatrix* m,*fm;

	if(!iShopItem){
		aciInitEvLineID();
		aciChangeList();
		return;
	}

	aciShopMenuUp();

	switch(iEvLineID){
		case MECHOS_MODE:
		case MECHOS_LIST_MODE:
			fm = (invMatrix*)iShopItem;
			m = (invMatrix*)fm -> prev;
			if(!m) m = (invMatrix*)aScrDisp -> i_matrixList -> last;
			while(!m -> uvsDataPtr){
				m = (invMatrix*)m -> prev;
				if(!m) m = (invMatrix*)aScrDisp -> i_matrixList -> last;
				if(m == fm) break;
			}
			if(!m -> uvsDataPtr){
				iShopItem = NULL;
			}
			else {
				iShopItem = (iListElement*)m;
				aciCurMechosID = m -> type;
			}
			break;
		case ITEMS_MODE:
		case ITEMS_LIST_MODE:
		case CHANGE_2_ITEMS_MODE:
			fp = (invItem*)iShopItem;
			p = (invItem*)fp -> prev;
			if(!p) p = (invItem*)aScrDisp -> i_itemList -> last;
			if(p != fp){
				while(!p -> uvsDataPtr || p -> classID != AS_ITEM_CLASS){
					p = (invItem*)p -> prev;
					if(!p) p = (invItem*)aScrDisp -> i_itemList -> last;
					if(p == fp) break;
				}
				if(!p -> uvsDataPtr || p -> classID != AS_ITEM_CLASS)
					iShopItem = NULL;
				else
					iShopItem = (iListElement*)p;
			}
			break;
		case WEAPONS_MODE:
		case WEAPONS_LIST_MODE:
		case CHANGE_2_WEAPONS_MODE:
			fp = (invItem*)iShopItem;
			p = (invItem*)fp -> prev;
			if(!p) p = (invItem*)aScrDisp -> i_itemList -> last;
			if(p != fp){
				while(!p -> uvsDataPtr || p -> classID != AS_WEAPON_CLASS){
					p = (invItem*)p -> prev;
					if(!p) p = (invItem*)aScrDisp -> i_itemList -> last;
					if(p == fp) break;
				}
				if(!p -> uvsDataPtr || p -> classID != AS_WEAPON_CLASS)
					iShopItem = NULL;
				else
					iShopItem = (iListElement*)p;
			}
			break;
	}
	aciInitAviID();
}

void aciChangeList(void)
{
	invItem* p,*fp;
	invMatrix* m,*fm;

	switch(iEvLineID){
		case MECHOS_MODE:
		case MECHOS_LIST_MODE:
			fm = m = (invMatrix*)aScrDisp -> i_matrixList -> first;
			while(!m -> uvsDataPtr){
				m = (invMatrix*)m -> next;
				if(m == fm) break;
			}
			if(!m -> uvsDataPtr){
				iShopItem = NULL;
			}
			else {
				iShopItem = (iListElement*)m;
				aciCurMechosID = m -> type;
			}
			break;
		case ITEMS_MODE:
		case ITEMS_LIST_MODE:
			fp = p = (invItem*)aScrDisp -> i_itemList -> first;
			while(!p -> uvsDataPtr || p -> classID != AS_ITEM_CLASS){
				p = (invItem*)p -> next;
				if(p == fp) break;
			}
			if(!p -> uvsDataPtr || p -> classID != AS_ITEM_CLASS)
				iShopItem = NULL;
			else
				iShopItem = (iListElement*)p;
			break;
		case WEAPONS_MODE:
		case WEAPONS_LIST_MODE:
			fp = p = (invItem*)aScrDisp -> i_itemList -> first;
			while(!p -> uvsDataPtr || p -> classID != AS_WEAPON_CLASS){
				p = (invItem*)p -> next;
				if(p == fp) break;
			}
			if(!p -> uvsDataPtr || p -> classID != AS_WEAPON_CLASS)
				iShopItem = NULL;
			else
				iShopItem = (iListElement*)p;
			break;
	}
	aciInitAviID();
	if(iEvLineID == MECHOS_LIST_MODE || iEvLineID == ITEMS_LIST_MODE || iEvLineID == WEAPONS_LIST_MODE)
		aciBuildShopList();
}

void aciInitShopItems(void)
{
	invItem* p;
	invMatrix* m;
	uvsActInt* u;

#ifdef _ACI_LOGFILE_
	aScrDisp -> logFile.open(aciLogFileName,XS_OUT | XS_APPEND | XS_NOREPLACE);
	aScrDisp -> logFile < "\r\nLocation: " < aci_curLocationName;
#endif

	p = (invItem*)aScrDisp -> i_itemList -> last;
	while(p){
		p -> uvsDataPtr = NULL;
		p = (invItem*)p -> prev;
	}

	m = (invMatrix*)aScrDisp -> i_matrixList -> last;
	while(m){
		m -> uvsDataPtr = NULL;
		m = (invMatrix*)m -> prev;
	}

	if(aScrDisp -> curMatrix){
		p = (invItem*)aScrDisp -> curMatrix -> items -> last;
		while(p){
			p -> uvsDataPtr = NULL;
			p = (invItem*)p -> prev;
		}
	}

#ifdef _ACI_LOGFILE_
		aScrDisp -> logFile < "\r\n\r\nShopItems :\r\n";
#endif

	u = GMechos;
	while(u){
		m = aScrDisp -> get_imatrix(u -> type);
		if(m && !m -> uvsDataPtr)
			m -> uvsDataPtr = u;
#ifdef _ACI_LOGFILE_
		aScrDisp -> logFile < "\r\nMechos \"" < m -> mech_name < "\"";
#endif

		u = (uvsActInt*)u -> next;
	}
	u = GItem;
	while(u){
		p = aScrDisp -> get_iitem(u -> type);
		if(p && !p -> uvsDataPtr)
			p -> uvsDataPtr = u;

#ifdef _ACI_LOGFILE_
		aScrDisp -> logFile < "\r\nItem \"" < p -> ID_ptr < "\"";
#endif

		u = (uvsActInt*)u -> next;
	}

	if(aScrDisp -> curMatrix){

#ifdef _ACI_LOGFILE_
		aScrDisp -> logFile < "\r\n\r\nGamerItems :\r\n";
#endif
		p = (invItem*)aScrDisp -> curMatrix -> items -> last;
		while(p){
			p -> uvsDataPtr = NULL;
#ifdef _ACI_LOGFILE_
			aScrDisp -> logFile < "\r\n" < p -> ID_ptr;
#endif
			p = (invItem*)p -> prev;
		}
		u = GGamer;
		while(u){
			p = (invItem*)aScrDisp -> curMatrix -> items -> last;
			while(p){
				if(!p -> uvsDataPtr && p -> ID == u -> type && p -> MatrixX == u -> pos_x && p -> MatrixY == u -> pos_y){
					p -> uvsDataPtr = u;
					break;
				}
				p = (invItem*)p -> prev;
			}
			u = (uvsActInt*)u -> next;
		}
		p = (invItem*)aScrDisp -> curMatrix -> items -> last;
		while(p){
			if(!p -> uvsDataPtr)
				ErrH.Abort("aciInitShopItems failed...");
			p = (invItem*)p -> prev;
		}
		aScrDisp -> curMatrix -> uvsDataPtr = GGamerMechos;
	}
	aciChangeList();
#ifdef _ACI_LOGFILE_
	aScrDisp -> logFile.close();
#endif
}

void aciChangeLineID(void)
{
	switch(iEvLineID){
		case CHANGE_2_MECHOS_MODE:
			iEvLineID = MECHOS_MODE;
			break;
		case CHANGE_2_WEAPONS_MODE:
			iEvLineID = WEAPONS_MODE;
			break;
		case CHANGE_2_ITEMS_MODE:
			iEvLineID = ITEMS_MODE;
			break;
		case MECHOS_LIST_MODE:
			iEvLineID = MECHOS_MODE;
			break;
		case ITEMS_LIST_MODE:
			iEvLineID = ITEMS_MODE;
			break;
		case WEAPONS_LIST_MODE:
			iEvLineID = WEAPONS_MODE;
			break;
	}
}

void aciBuyItem(void)
{
	int cr;
	invItem* p,*p1;
	invMatrix* m,*m1;
	uvsActInt* u,*u1;
	listElem* el;

	if(!(aScrDisp -> flags & AS_INV_MOVE_ITEM)){
		if(iShopItem){
			if(iEvLineID == MECHOS_MODE || iEvLineID == MECHOS_LIST_MODE){
				m = (invMatrix*)iShopItem;
				u = (uvsActInt*)m -> uvsDataPtr;

				if(!u) ErrH.Abort("Bad shop item...");
				u -> delink(GMechos);
				if(aScrDisp -> curMatrix){
					if(GGamerMechos){
						el = GMechos;
						//std::cout<<"GGamerMechos -> link(el)"<<std::endl;
						GGamerMechos -> link(el);
						GMechos = (uvsActInt*)el;
					}

					m1 = aScrDisp -> get_imatrix(aScrDisp -> curMatrix -> type);
					if(!m1 -> uvsDataPtr) m1 -> uvsDataPtr = GGamerMechos;

					iShopItem = (iListElement*)m1;
				}
				else {
					aciNextShopItem();
				}
				//std::cout<<"aciBuyItem GGamerMechos = u"<<std::endl;
				GGamerMechos = u;

				cr = aciGetCurCredits();
				cr -= u -> price;
				aciUpdateCurCredits(cr);

				u1 = aciGetMechos(m -> type);
				if(u1)
					m -> uvsDataPtr = u1;
				else
					m -> uvsDataPtr = NULL;

			}
			else {
				if(aScrDisp -> flags & AS_INV_MOVE_ITEM) return;

				p = (invItem*)iShopItem;
				p1 = aScrDisp -> alloc_item();
				p -> clone(p1);

				aScrDisp -> curItem = p1;
				set_mouse_cursor(p1 -> frame,p1 -> ScreenSizeX,p1 -> ScreenSizeY);

				aScrDisp -> flags |= AS_INV_MOVE_ITEM;

				el = (listElem*)GGamer;
				u = (uvsActInt*)p -> uvsDataPtr;
				if(!u) ErrH.Abort("Null uvsDataPtr...");
				u -> delink(GItem);
				u -> link(el);
				GGamer = (uvsActInt*)el;
				p1 -> uvsDataPtr = u;
				p1 -> flags |= INV_ITEM_NO_PAY;
				p1 -> flags |= INV_ITEM_NEW;

				u -> pos_x = u -> pos_y = -1;
				p1 -> MatrixX = p1 -> MatrixY = -1;

				if(aScrDisp -> iscr_iP){
					aciInitPricePanel(aScrDisp -> iscr_iP,(iListElement*)p1,ITEMS_MODE,1);
					aScrDisp -> iscr_iP -> set_redraw();
				}

				u1 = aciGetShopItem(u -> type);
				if(u1 && !aScrDisp -> curLocData -> ExcludeItems[u1 -> type]){
					p -> uvsDataPtr = u1;
				}
				else {
					p -> uvsDataPtr = NULL;
					aciNextShopItem();
				}
			}
		}
		aciInitAviID();
		if(iEvLineID == MECHOS_LIST_MODE || iEvLineID == ITEMS_LIST_MODE || iEvLineID == WEAPONS_LIST_MODE)
			aciBuildShopList();
	}
	else {
		aciSellMoveItem();
	}
}

void aciSellMoveItem(void)
{
	int i,id,fl = 0,index,exclude = 1;
	invItem* p,*p1;
	invMatrix* m,*m1;
	uvsActInt* u,*pu;

	iListElement* prev_item = iShopItem;

	int change_mode = 0,prev_mode = iEvLineID;

	if(!(aScrDisp -> flags & AS_INV_MOVE_ITEM)) return;

	p = aScrDisp -> curItem;
	p1 = aScrDisp -> get_iitem(p -> ID);
	pu = (uvsActInt*)p -> uvsDataPtr;

	if(!uvsCurrentWorldUnable && p -> partData){
		index = -1;
		m = (invMatrix*)aScrDisp -> i_matrixList -> last;
		while(m){
			for(i = 0; i < 2; i ++){
				if(m -> uvsDataPtr && m -> type == p -> partData -> baseID[i])
					index = i;
			}
			m = (invMatrix*)m -> prev;
		}
		if(index != -1)
			fl = 1;
	}
	if(!fl){
		id = p -> ID;
		if(p -> flags & INV_ITEM_NO_PAY) exclude = 0;
		if(p -> classID == AS_ITEM_CLASS && prev_mode != ITEMS_MODE && prev_mode != ITEMS_LIST_MODE){
			change_mode = CHANGE_2_ITEMS_MODE;
			if(iEvLineID != ITEMS_MODE && iEvLineID != ITEMS_LIST_MODE)
				iEvLineID = change_mode;
		}
		if(p -> classID == AS_WEAPON_CLASS && prev_mode != WEAPONS_MODE && prev_mode != WEAPONS_LIST_MODE){
			change_mode = CHANGE_2_WEAPONS_MODE;
			if(iEvLineID != WEAPONS_MODE && iEvLineID != WEAPONS_LIST_MODE)
				iEvLineID = change_mode;
		}

		aciSell_Item(p);
		aScrDisp -> flags ^= AS_INV_MOVE_ITEM;

		if(!exclude || !aScrDisp -> curLocData -> ExcludeItems[id]){
			iShopItem = p1;
		}
		else {
			aciRemoveUvsItem(pu);
			iShopItem = prev_item;
			switch(prev_mode){
				case ITEMS_MODE:
				case ITEMS_LIST_MODE:
					change_mode = CHANGE_2_ITEMS_MODE;
					break;
				case WEAPONS_MODE:
				case WEAPONS_LIST_MODE:
					change_mode = CHANGE_2_WEAPONS_MODE;
					break;
				case MECHOS_MODE:
				case MECHOS_LIST_MODE:
					change_mode = CHANGE_2_MECHOS_MODE;
					break;
			}
			iEvLineID = change_mode;
		}
	}
	else {
		m = aScrDisp -> get_imatrix(p -> partData -> baseID[index]);
		m1 = aScrDisp -> get_imatrix(p -> partData -> targetID[index]);
		if(!m || !m1)
			ErrH.Abort("Bad constructor mechos ID...");
		if(m1 -> uvsDataPtr)
			ErrH.Abort("Duplicate constructor mechos...");

		u = (uvsActInt*)m -> uvsDataPtr;

		u -> type = m1 -> type;
		m1 -> uvsDataPtr = u;

		m -> uvsDataPtr = NULL;

		iShopItem = (iListElement*)m1;

		u = (uvsActInt*)p -> uvsDataPtr;
		u -> delink(GGamer);
		delete u;

		p -> uvsDataPtr = NULL;

		if(m1 -> flags & IM_NOT_COMPLETE){
			SOUND_UNICUM_1();
		}
		else {
			SOUND_UNICUM_2();
		}

		iEvLineID = CHANGE_2_MECHOS_MODE;
		aScrDisp -> flags ^= AS_INV_MOVE_ITEM;
	}

	aciInitAviID();
	restore_mouse_cursor();

	if(aScrDisp -> iscr_iP){
		aScrDisp -> iscr_iP -> free_list();
		aScrDisp -> iscr_iP -> set_redraw();
	}

	if(fl) iEvLineID = CHANGE_2_MECHOS_MODE;

	aciInitShopAvi();
}

int uvs_aciKillItem(int id,int id1)
{
	int ret = 0;
	uvsActInt* u;
	invItem* p,*p1,*it;
	if(!aScrDisp -> curMatrix) return ret;
	p = (invItem*)aScrDisp -> curMatrix -> items -> last;

	if(id1 != -1){
		it = aScrDisp -> get_iitem(id1);
	}

	while(p){
		p1 = (invItem*)p -> prev;
		if(p -> ID == id){
			if(id1 == -1){
				aScrDisp -> curMatrix -> remove_item(p);
				aScrDisp -> free_item(p);
			}
			else {
				it -> clone(p);
				u = (uvsActInt*)p -> uvsDataPtr;
				u -> type = it -> ID;
			}
			ret = 1;
		}
		p = p1;
	}
	if(aScrDisp -> flags & AS_INV_MOVE_ITEM){
		if(aScrDisp -> curItem -> ID == id){
			if(id1 == -1){
				aScrDisp -> free_item(aScrDisp -> curItem);
				aScrDisp -> curItem = NULL;
				aScrDisp -> flags ^= AS_INV_MOVE_ITEM;
				restore_mouse_cursor();
			}
			else {
				it -> clone(aScrDisp -> curItem);
				u = (uvsActInt*)aScrDisp -> curItem -> uvsDataPtr;
				u -> type = it -> ID;
				set_mouse_cursor(aScrDisp -> curItem -> frame,aScrDisp -> curItem -> ScreenSizeX,aScrDisp -> curItem -> ScreenSizeY);
			}
			ret = 1;
		}
	}
	aScrDisp -> curMatrix -> set_redraw();
	if((aScrDisp -> flags & AS_ISCREEN) && GCharItem && GCharItem -> type == id){
		aciML_D -> key_trap(AML_KEY,AML_KILL_ELEECH_CODE);
	}
	return ret;
}

int uvs_aciKillOneItem(int id,int x,int y)
{
	int ret = 0;
	invItem* p,*p1;
	if(!aScrDisp -> curMatrix) return ret;
	p = (invItem*)aScrDisp -> curMatrix -> items -> last;

	while(p){
		p1 = (invItem*)p -> prev;
		if(p -> ID == id && p -> MatrixX == x && p -> MatrixY == y){
			aScrDisp -> curMatrix -> remove_item(p);
			aScrDisp -> free_item(p);
			return 1;
		}
		p = p1;
	}
	return 0;
}

void uvs_aciChangeItem(int id_from,int id_to)
{
	invItem* p,*p1,*it;
	if(!aScrDisp -> curMatrix) return;
	p = (invItem*)aScrDisp -> curMatrix -> items -> last;
	if(!(aScrDisp -> flags & AS_ISCREEN))
		it = aScrDisp -> get_item(id_to);
	else
		it = aScrDisp -> get_iitem(id_to);

	while(p){
		p1 = (invItem*)p -> prev;
		if(p -> ID == id_from){
			it -> clone(p);
		}
		p = p1;
	}
	aScrDisp -> curMatrix -> set_redraw();

	if(aScrDisp -> flags & AS_INV_MOVE_ITEM){
		if(aScrDisp -> curItem -> ID == id_from){
				it -> clone(aScrDisp -> curItem);
				set_mouse_cursor(aScrDisp -> curItem -> frame,aScrDisp -> curItem -> ScreenSizeX,aScrDisp -> curItem -> ScreenSizeY);
		}
	}

	if(aScrDisp -> flags & AS_ISCREEN){
		if(!aScrDisp -> secondMatrix) return;
		p = (invItem*)aScrDisp -> secondMatrix -> items -> last;
		while(p){
			p1 = (invItem*)p -> prev;
			if(p -> ID == id_from){
				it -> clone(p);
			}
			p = p1;
		}
		aScrDisp -> secondMatrix -> set_redraw();
	}
}

void uvs_aciChangeOneItem(int id_from,int id_to,int x,int y)
{
	invItem* p,*p1,*it;
	if(!aScrDisp -> curMatrix) return;
	p = (invItem*)aScrDisp -> curMatrix -> items -> last;

	if(!(aScrDisp -> flags & AS_ISCREEN))
		it = aScrDisp -> get_item(id_to);
	else
		it = aScrDisp -> get_iitem(id_to);

	while(p){
		p1 = (invItem*)p -> prev;
		if(p -> ID == id_from && p -> MatrixX == x && p -> MatrixY == y){
			it -> clone(p);
//			  if(p -> uvsDataPtr) ((uvsActInt*)p -> uvsDataPtr) -> type = id_to;
			return;
		}
		p = p1;
	}
	aScrDisp -> curMatrix -> set_redraw();

	if(aScrDisp -> flags & AS_INV_MOVE_ITEM){
		if(aScrDisp -> curItem -> ID == id_from && aScrDisp -> curItem -> MatrixX == x && aScrDisp -> curItem -> MatrixY == y){
				it -> clone(aScrDisp -> curItem);
				set_mouse_cursor(aScrDisp -> curItem -> frame,aScrDisp -> curItem -> ScreenSizeX,aScrDisp -> curItem -> ScreenSizeY);
				return;
		}
	}

	if(aScrDisp -> flags & AS_ISCREEN){
		if(!aScrDisp -> secondMatrix) return;
		p = (invItem*)aScrDisp -> secondMatrix -> items -> last;
		while(p){
			p1 = (invItem*)p -> prev;
			if(p -> ID == id_from && p -> MatrixX == x && p -> MatrixY == y){
				it -> clone(p);
			}
			p = p1;
		}
		aScrDisp -> secondMatrix -> set_redraw();
	}
}

void uvs_aciChangeOneItem(int id_from,int id_to,uvsActInt* ptr)
{
	invItem* p,*p1,*it;
	if(!aScrDisp -> curMatrix) return;
	p = (invItem*)aScrDisp -> curMatrix -> items -> last;

	it = aScrDisp -> get_iitem(id_to);

	while(p){
		p1 = (invItem*)p -> prev;
		if(p -> ID == id_from && p -> uvsDataPtr == ptr){
			it -> clone(p);
			return;
		}
		p = p1;
	}
	aScrDisp -> curMatrix -> set_redraw();

	if(aScrDisp -> flags & AS_INV_MOVE_ITEM){
		if(aScrDisp -> curItem -> ID == id_from && aScrDisp -> curItem -> uvsDataPtr == ptr){
				it -> clone(aScrDisp -> curItem);
				set_mouse_cursor(aScrDisp -> curItem -> frame,aScrDisp -> curItem -> ScreenSizeX,aScrDisp -> curItem -> ScreenSizeY);
				return;
		}
	}
	if(aScrDisp -> flags & AS_ISCREEN){
		if(!aScrDisp -> secondMatrix) return;
		p = (invItem*)aScrDisp -> secondMatrix -> items -> last;
		while(p){
			p1 = (invItem*)p -> prev;
			if(p -> ID == id_from && p -> uvsDataPtr == ptr){
				it -> clone(p);
			}
			p = p1;
		}
		aScrDisp -> secondMatrix -> set_redraw();
	}
}

void aciChangeItem(actintItemData* p)
{
	invItem* p1,*it;
	if(!aScrDisp -> curMatrix) return;

	if(!(aScrDisp -> flags & AS_ISCREEN))
		it = aScrDisp -> get_item(p -> type);
	else
		it = aScrDisp -> get_iitem(p -> type);

	p1 = (invItem*)p -> actintOwner;
	it -> clone(p1);

	if(!(aScrDisp -> flags & AS_ISCREEN) && aScrDisp -> curMode == AS_INV_MODE)
		aScrDisp -> curMatrix -> set_redraw();
}

void aciInitShopList(void)
{
	switch(iEvLineID){
		case MECHOS_LIST_MODE:
			iEvLineID = MECHOS_MODE;
			aciCloseShopMenu();
			break;
		case ITEMS_LIST_MODE:
			iEvLineID = ITEMS_MODE;
			aciCloseShopMenu();
			break;
		case WEAPONS_LIST_MODE:
			iEvLineID = WEAPONS_MODE;
			aciCloseShopMenu();
			break;
		case MECHOS_MODE:
		case ITEMS_MODE:
		case WEAPONS_MODE:
			aciBuildShopList();
			break;
	}
}

void aciCloseShopMenu(void)
{
	fncMenu* mn = aScrDisp -> get_imenu(SHOP_ITEMS_MENU_ID);
	if(mn)
		mn -> flags &= ~FM_ACTIVE;
}

void aciShopMenuUp(void)
{
	fncMenu* mn = aScrDisp -> get_imenu(SHOP_ITEMS_MENU_ID);
	if(mn && mn -> flags & FM_ACTIVE)
		mn -> step_up();
}

void aciShopMenuDown(void)
{
	fncMenu* mn = aScrDisp -> get_imenu(SHOP_ITEMS_MENU_ID);
	if(mn && mn -> flags & FM_ACTIVE)
		mn -> step_down();
}

void aciBuildShopList(void)
{
	int i;

	invItem* p;
	fncMenu* mn;
	invMatrix* m;
	fncMenuItem* itm;

	if(!iShopItem) return;

	mn = aScrDisp -> get_imenu(SHOP_ITEMS_MENU_ID);
	if(!mn) return;

	aScrDisp -> clear_menu(mn);

	switch(iEvLineID){
		case MECHOS_MODE:
		case MECHOS_LIST_MODE:
			m = (invMatrix*)aScrDisp -> i_matrixList -> first;
			for(i = 0; i < aScrDisp -> i_matrixList -> Size; i ++){
				if(m -> uvsDataPtr){
					if(m -> mech_name){
						itm = aScrDisp -> alloc_menu_item();
						itm -> init_name(m -> mech_name);
						itm -> fnc_code = m -> type;
						itm -> init();
						mn -> add_item(itm);
					}
				}
				m = (invMatrix*)m -> next;
			}
			mn -> curFunction = ((invMatrix*)iShopItem) -> type;
			iEvLineID = MECHOS_LIST_MODE;
			break;
		case ITEMS_MODE:
		case ITEMS_LIST_MODE:
			p = (invItem*)aScrDisp -> i_itemList -> first;
			for(i = 0; i < aScrDisp -> i_itemList -> Size; i ++){
				if(p -> uvsDataPtr && p -> classID == AS_ITEM_CLASS){
					if(p->ID_ptr.size()){
						itm = aScrDisp -> alloc_menu_item();
						itm -> init_name(p->ID_ptr.c_str());
						itm -> fnc_code = p -> ID;
						itm -> init();
						mn -> add_item(itm);
					}
				}
				p = (invItem*)p -> next;
			}
			mn -> curFunction = ((invItem*)iShopItem) -> ID;
			iEvLineID = ITEMS_LIST_MODE;
			break;
		case WEAPONS_MODE:
		case WEAPONS_LIST_MODE:
			p = (invItem*)aScrDisp -> i_itemList -> first;
			for(i = 0; i < aScrDisp -> i_itemList -> Size; i ++){
				if(p -> uvsDataPtr && p -> classID == AS_WEAPON_CLASS){
					if(p->ID_ptr.size()){
						itm = aScrDisp -> alloc_menu_item();
						itm -> init_name(p->ID_ptr.c_str());
						itm -> fnc_code = p -> ID;
						itm -> init();
						mn -> add_item(itm);
					}
				}
				p = (invItem*)p -> next;
			}
			mn -> curFunction = ((invItem*)iShopItem) -> ID;
			iEvLineID = WEAPONS_LIST_MODE;
			break;
	}
	mn -> set_redraw();
	mn -> flags |= FM_ACTIVE;

	mn -> init_objects();

	mn -> curItem = mn -> get_fnc_item(mn -> curFunction);
	if(!mn -> curItem)
		ErrH.Abort("Bad shop menu list...");
	if(mn -> VItems < mn -> items -> Size)
		mn -> firstItem = mn -> curItem;
	else
		mn -> firstItem = (fncMenuItem*)mn -> items -> first;
}

void aciChooseShopItem(void)
{
	int x,y,id;

	invItem* p;
	invMatrix* m;

	fncMenu* mn;
	fncMenuItem* itm;

	mn = aScrDisp -> get_imenu(SHOP_ITEMS_MENU_ID);
	if(!mn) return;

	itm = mn -> curItem;

	x = iMouseX;
	y = iMouseY;

	if(!mn -> check_xy(x,y)) return;

	if(mn -> change(x,y)){
		switch(iEvLineID){
			case MECHOS_LIST_MODE:
				m = aScrDisp -> get_imatrix(mn -> curFunction);
				iShopItem = (iListElement*)m;
				aciInitAviID();
				aciInitShopAvi();
				mn -> flags &= ~FM_ACTIVE;
				break;
			case ITEMS_LIST_MODE:
				p = aScrDisp -> get_iitem(mn -> curFunction);
				iShopItem = (iListElement*)p;
				aciInitAviID();
				aciInitShopAvi();
				mn -> flags &= ~FM_ACTIVE;
				break;
			case WEAPONS_LIST_MODE:
				p = aScrDisp -> get_iitem(mn -> curFunction);
				iShopItem = (iListElement*)p;
				aciInitAviID();
				aciInitShopAvi();
				mn -> flags &= ~FM_ACTIVE;
				break;
		}
	}
	else {
		if(iEvLineID == MECHOS_LIST_MODE || iEvLineID == MECHOS_MODE){
			id = ((invMatrix*)iShopItem) -> type;
			if(id != mn -> curFunction){
				aciSetShopItem(mn -> curFunction);
				iScrDisp -> end_event();
			}
		}
		else {
			id = ((invItem*)iShopItem) -> ID;
			if(id != mn -> curFunction){
				aciSetShopItem(mn -> curFunction);
				iScrDisp -> end_event();
			}
		}
		aciInitAviID();
		aciInitShopAvi();
		if(itm == mn -> curItem)
			mn -> flags &= ~FM_ACTIVE;
	}
}

#ifdef _ACI_CHECK_DIALOGS_
int aciBadPhraseFlag = 0;
#endif

//TODO need fast malloc and need rewrite!
void aciInitPanel(InfoPanel* p,unsigned char* str)
{
	int i = 0,quit_log = 0,sz = strlen((char*)str),str_len,len,col;
	unsigned char* dest_str,*ptr,*tmp_ptr;

	dest_str = new unsigned char[sz + 1];
	ptr = new unsigned char[sz + 1];
	tmp_ptr = ptr;

	str_len = sz;
	strcpy((char*)ptr,(char*)str);
	col = (aciCurColorScheme[FM_SELECT_BORDER_COL] << 8) | aciCurColorScheme[FM_SELECT_BORDER_COL];

	while(!quit_log){
		strcpy((char*)dest_str,(char*)tmp_ptr);
		
		if(p -> flags & IP_RANGE_FONT)
			len = aStrLen32(dest_str,ACI_PHRASE_FONT,p->hSpace);
		else
			len = aStrLen(dest_str,ACI_PHRASE_FONT,p->hSpace);
		if(len < p->SizeX){
			if(p -> items -> Size >= p -> MaxStr){
				std::cout<<"Dialog phrase too long..."<<std::endl;
				quit_log = 1;
			} else {
				p->add_item((char*)dest_str,-1,col);
				sz -= strlen((char*)dest_str);
				strcpy((char*)ptr, (char*)str);
				if(sz <= 0) {
					quit_log = 1;
				} else {
					tmp_ptr = ptr + (str_len - sz);
				}
			}
		} else {
			i = sz;
			while(dest_str[i] != ' '){
				i --;
				if(i <= 0){
					quit_log = 1;
					break;
				}
			}
			if(!quit_log)
				tmp_ptr[i] = 0;
		}
	}

	delete[] dest_str;
	delete[] ptr;
}

void aciStartSpeech(void)
{
	aciML_EventSeq* seq;
	int i,unlock_id,prepare2_id,prepare_id,speech_id,end_id,end2_id,lock_id,speech_sz,dropLev = 0;

	InfoPanel* p = aScrDisp -> get_info_panel(ACI_ANSWER_PANEL);
	if(!p) return;

	aciSpeechMode = dgMood;

#ifdef _ACI_ESCAVE_DEBUG_
	if(aci_dgMoodNext != -1){
		aciSpeechMode = aci_dgMoodNext;
		aci_dgMoodNext = -1;
	}
#endif
	unlock_id = AML_UNLOCK0_SEQ + aciSpeechMode;
	prepare2_id = AML_PREPARE0_SEQ2 + aciSpeechMode;
	prepare_id = AML_PREPARE0_SEQ + aciSpeechMode;
	speech_id = AML_SPEECH0_SEQ + aciSpeechMode;
	end_id = AML_END0_SEQ + aciSpeechMode;
	end2_id = AML_END0_SEQ2 + aciSpeechMode;
	lock_id = AML_LOCK0_SEQ + aciSpeechMode;

	speech_sz = p -> items -> Size;

	aciSpeechSeq -> size = 0;

	for(i = 0; i < AML_MAX_DROP_EVENT; i ++)
		aciSpeechSeq -> dropLevel[i] = -1;

	if(aciSpeechBegLocked || !aciSpeechStarted){
		seq = aciML_D -> getEventSeq(unlock_id);
		if(seq){
			aciSpeechSeq -> mergeSeq(seq);
			aciSpeechSeq -> dropLevel[dropLev] = seq -> dropLevel[0];
			dropLev ++;
		}
		aciSpeechStarted = 1;
	}
	seq = aciML_D -> getEventSeq(prepare2_id);
	if(seq){
		aciSpeechSeq -> mergeSeq(seq);
		aciSpeechSeq -> dropLevel[dropLev] = seq -> dropLevel[0];
		dropLev ++;
	}
	seq = aciML_D -> getEventSeq(prepare_id);
	if(seq){
		aciSpeechSeq -> mergeSeq(seq);
		aciSpeechSeq -> dropLevel[dropLev] = seq -> dropLevel[0];
		dropLev ++;
	}
	seq = aciML_D -> getEventSeq(speech_id);
	if(seq){
		for(i = 0; i < speech_sz; i ++)
			aciSpeechSeq -> mergeSeq(seq);
	}

	seq = aciML_D -> getEventSeq(end_id);
	if(seq){
		aciSpeechSeq -> mergeSeq(seq);
		aciSpeechSeq -> dropLevel[dropLev] = seq -> dropLevel[0];
		dropLev ++;
	}
	seq = aciML_D -> getEventSeq(end2_id);
	if(seq){
		aciSpeechSeq -> mergeSeq(seq);
		aciSpeechSeq -> dropLevel[dropLev] = seq -> dropLevel[0];
		dropLev ++;
	}
	if(aciSpeechEndLocked){
		seq = aciML_D -> getEventSeq(lock_id);
		if(seq){
			aciSpeechSeq -> mergeSeq(seq);
			aciSpeechSeq -> dropLevel[dropLev] = seq -> dropLevel[0];
			dropLev ++;
		}
	}

	aciSpeechSeq -> ChannelID = aciML_D -> curDataSet -> SpeechChannel;
	aciML_D -> SeqList[0] -> putSeq(aciSpeechSeq);

	aciSpeechBegLocked = aciSpeechEndLocked;
}

void aciNextPhrase(void)
{
	unsigned char* str;
	iScreenObject* obj;
	InfoPanel* p = aScrDisp -> get_info_panel(ACI_ANSWER_PANEL);
	if(!p || !p -> iScreenOwner) return;

	if(aciEscaveDead){
		aciClearQuestMenu();
		obj = (iScreenObject*)p -> iScreenOwner -> owner;
		p -> free_list();

		str = (unsigned char*)dgD -> getQdead();
		if(str) aciInitPanel(p,str);
		p -> set_redraw();
		return;
	}

	if(!aciSpeechStarted && dgD -> isEnd())
		aciSpeechEndLocked = aciSpeechBegLocked = aciEndSpeech = 1;

	aciClearQuestMenu();
	obj = (iScreenObject*)p -> iScreenOwner -> owner;

	p -> free_list();

	str = (unsigned char*)dgD -> getNextPhrase();

	if(aciEndSpeech){
		if(!dgD -> isEnd()){
			aciEndSpeech = 0;
		}
		else {
			p -> set_redraw();
			return;
		}
	}

	if(str)
		aciInitPanel(p,str);

	aciSpeechEndLocked = dgD -> isEnd();

	if(str && !aciEscaveEmpty)
		aciStartSpeech();
	p -> set_redraw();
}

int aciCheckCredits(void)
{
	invItem* p;
	invMatrix* m;
	uvsActInt* u;

	int cr = aciGetCurCredits();

	if(iEvLineID == MECHOS_MODE || iEvLineID == MECHOS_LIST_MODE){
		m = (invMatrix*)iShopItem;
		u = (uvsActInt*)m -> uvsDataPtr;

		if(m -> flags & IM_NOT_COMPLETE) return 0;
	}

	if(aScrDisp -> flags & AS_INV_MOVE_ITEM){
		if(aScrDisp -> curItem -> partData) return 1;
		if(!((uvsActInt*)aScrDisp -> curItem -> uvsDataPtr) -> sell_price) return 0;
		return 1;
	}

	if(iEvLineID == EMPTY_MODE)
		return 0;

	if(iEvLineID == MECHOS_MODE || iEvLineID == MECHOS_LIST_MODE){
		if(u -> price > cr + aciGetCurMatrixPrice()) return 0;
	}
	else {
		p = (invItem*)iShopItem;
		u = (uvsActInt*)p -> uvsDataPtr;
		if(u -> price > cr) return 0;
	}
	return 1;
}

int aciCheckItemCredits(void)
{
	int cr = aciGetCurCredits();
	if(aScrDisp -> flags & AS_INV_MOVE_ITEM){
		if(((uvsActInt*)aScrDisp -> curItem -> uvsDataPtr) -> price > cr) return 0;
		return 1;
	}
	return 0;
}

char* aciGetPrice(iListElement* p,int mode,int sell_mode)
{
	char* ptr;
	invItem* it;
	invMatrix* m;
	uvsActInt* u;

	if(mode == MECHOS_MODE){
		m = (invMatrix*)p;
		u = (uvsActInt*)m -> uvsDataPtr;
	}
	else {
		it = (invItem*)p;
		u = (uvsActInt*)it -> uvsDataPtr;
	}

	aciXConv -> init();

	if(!u){
		*aciXConv < aciSTR_UNDEFINED_PRICE;
		return aciXConv -> address();
	}

	*aciXConv < aciSTR_PRICE;

	if(sell_mode)
		*aciXConv <= u -> sell_price < " $";
	else
		*aciXConv <= u -> price < " $";
	ptr = aciXConv -> address();

	return ptr;
}

void aciInitCurMatrixPtr(void)
{
	aScrDisp -> curMatrix -> uvsDataPtr = GGamerMechos;
}

int aciGetCurMatrixPrice(void)
{
	int price = 0;

//	  invItem* p;
	uvsActInt* u;

	if(!aScrDisp -> curMatrix) return 0;
	u = (uvsActInt*)aScrDisp -> curMatrix -> uvsDataPtr;
	if(!u) ErrH.Abort("Bad CurMatrix...");
	price += u -> sell_price;
/*
	p = (invItem*)aScrDisp -> curMatrix -> items -> last;
	while(p){
		u = (uvsActInt*)p -> uvsDataPtr;
		if(!u) ErrH.Abort("Bad CurMatrix item...");
		price += u -> price;
		p = (invItem*)p -> prev;
	}
*/
	return price;
}

void aciBuildQuestList(void)
{
	int cd = 0;
	char* p;
	if(!aScrDisp -> qMenu) return;

	if(aciEscaveDead || aciEscaveEmpty) return;

	fncMenuItem* itm;
	fncMenu* m;

	p = dgD -> findQfirst();
	if(!p) return;

	aScrDisp -> qMenu -> finit();
	aScrDisp -> qMenu -> init();

	p = dgD -> getQprefix();
	if(p){
		m = new fncMenu;
		m -> flags |= FM_RANGE_FONT;
		itm = aScrDisp -> alloc_menu_item();
		itm -> init_name(p);
		itm -> fnc_code = 0;

		itm -> font = ACI_PHRASE_FONT;
		itm -> space = 1;

		itm -> init();
		m -> add_item(itm);

		aScrDisp -> qMenu -> add(m);
	}

	p = dgD -> findQfirst();
	if(p){
		m = new fncMenu;
		m -> flags |= FM_RANGE_FONT;
		while(p){
			itm = aScrDisp -> alloc_menu_item();
			itm -> init_name(p);
			itm -> fnc_code = cd;

			itm -> font = ACI_PHRASE_FONT;
			itm -> space = 1;

			itm -> init();
			m -> add_item(itm);

			cd ++;

			p = dgD -> findQnext();
		}
		aScrDisp -> qMenu -> add(m,1);
	}

	p = dgD -> getQpostfix();
	if(p){
		m = new fncMenu;
		m -> flags |= FM_RANGE_FONT;
		itm = aScrDisp -> alloc_menu_item();
		itm -> init_name(p);
		itm -> fnc_code = 0;

		itm -> font = ACI_PHRASE_FONT;
		itm -> space = 1;

		itm -> init();
		m -> add_item(itm);

		aScrDisp -> qMenu -> add(m);
	}
	aScrDisp -> qMenu -> init_redraw();
	aScrDisp -> qMenu -> build();
	aScrDisp -> qMenu -> set_redraw();
}

void aciShowBlockPhrase(void)
{
	char* str;
	InfoPanel* p = aScrDisp -> get_info_panel(ACI_ANSWER_PANEL);
	if(!p) return;

	str = dgD -> getQblock();

	if(str){
		p -> free_list();
		aciInitPanel(p,(unsigned char*)str);
		p -> set_redraw();
	}
}

void aciAskQuestion(void)
{
	char* str,*subj;
	fncMenuSet* m = aScrDisp -> qMenu;
	InfoPanel* p = aScrDisp -> get_info_panel(ACI_ANSWER_PANEL);
	if(!p || !m) return;

	if(aciEscaveDead || aciEscaveEmpty) return;

	if(!(m -> flags & FMC_DATA_INIT)){
		str = dgD -> getQempty();
	}
	else {
		subj = m -> get();
		str = dgD -> getAnswer(subj);
	}

	if(str){
		p -> free_list();
		aciInitPanel(p,(unsigned char*)str);
		p -> set_redraw();
	}
	aciStartSpeech();
}

void aciEscaveKickOut(void)
{
	char* str;
	InfoPanel* p = aScrDisp -> get_info_panel(ACI_ANSWER_PANEL);
	if(!p) return;

	str = dgD -> getQout();

	if(str){
		p -> free_list();
		aciInitPanel(p,(unsigned char*)str);
		p -> set_redraw();
	}
	aciStartSpeech();
}

int aciCheckItemPickUp(actintItemData* p)
{
	invItem* itm = aScrDisp -> get_item(p -> type);
	if((itm -> classID == AS_ITEM_CLASS && aciPickupItems) || (itm -> classID == AS_WEAPON_CLASS && aciPickupDevices))
		return 1;
	return 0;
}

void aciSet_aMouse(void)
{
	MouseSizeX = aMouseSizeX;
	MouseSizeY = aMouseSizeY;
	MouseSize = aMouseSize;
	MouseFrames = aMouseFrames;
}

void aciSet_iMouse(void)
{
	MouseSizeX = iMouseSizeX;
	MouseSizeY = iMouseSizeY;
	MouseSize = iMouseSize;
	MouseFrames = iMouseFrames;
}

void aciClearQuestMenu(void)
{
	fncMenuSet* m;
	m = aScrDisp -> qMenu;

	if(m && m -> flags & FMC_DATA_INIT){
		m -> finit();
		m -> redraw();
		m -> redraw_owner();
	}
}

void aciOfferEleech(void)
{
	int w_id = aScrDisp -> wMap -> world_ptr[aScrDisp -> curLocData -> WorldID] -> uvsID;
	aciML_SendEvent(AML_KEY_TRAP,AML_KEY,AML_ELEECH_CODE);
	WaitGameInTown(w_id);
}

void aciWaitNextPeriod(void)
{
	int w_id = aScrDisp -> wMap -> world_ptr[aScrDisp -> curLocData -> WorldID] -> uvsID;
	WaitGameInTown(w_id);
}

void aciOfferWeezyk(void)
{
	aciML_SendEvent(AML_KEY_TRAP,AML_KEY,AML_ELEECH_CODE);
}

void aciGetEleech(void)
{
	listElem* el;
	uvsActInt* u;
	invItem* p,*p1;
	if(!GCharItem) return;
	u = GCharItem;

	p = aScrDisp -> get_iitem(u -> type);
	p1 = aScrDisp -> alloc_item();
	p -> clone(p1);

	if(aScrDisp -> flags & AS_INV_MOVE_ITEM){
		aScrDisp -> flags ^= AS_INV_MOVE_ITEM;
		aciSell_Item(aScrDisp -> curItem);
		aScrDisp -> curItem = NULL;
	}

	aScrDisp -> flags |= AS_INV_MOVE_ITEM;
	aScrDisp -> curItem = p1;
	set_mouse_cursor(p1 -> frame,p1 -> ScreenSizeX,p1 -> ScreenSizeY);

	el = (listElem*)GGamer;
	u -> link(el);
	GGamer = (uvsActInt*)el;
	p1 -> uvsDataPtr = u;

	GCharItem = NULL;
}

char* aciGetItemLoad(invItem* p,int mode)
{
	char* ptr;
	aciXConv -> init();

	if(p -> pTemplate){
		switch(mode){
			case 0:
				if(p -> flags & INV_ITEM_SHOW_ESCAVE){
					if(p -> item_ptr)
						sprintf(aciXConv -> address(),p -> pTemplate,aciGetItemEscave(p -> item_ptr -> data1));
				}
				else {
					if(p -> item_ptr)
						sprintf(aciXConv -> address(),p -> pTemplate,p -> item_ptr -> data1);
				}
				break;
			case 1:
				if(p -> flags & INV_ITEM_SHOW_ESCAVE){
					if(p -> uvsDataPtr)
						sprintf(aciXConv -> address(),p -> pTemplate,aciGetItemEscave(((uvsActInt*)p -> uvsDataPtr) -> param2));
				}
				else {
					if(p -> uvsDataPtr)
						sprintf(aciXConv -> address(),p -> pTemplate,((uvsActInt*)p -> uvsDataPtr) -> param2);
				}
				break;
		}
	}
	ptr = aciXConv -> address();

	return ptr;
}

void aciAddTeleportMenuItem(int id,int fnc_id)
{
	const char* ptr;
	fncMenu* mn;
	fncMenuItem* itm;
	invMatrix* m;

	mn = aScrDisp -> get_menu(FMENU_TELEPORT_MENU);
	if(!mn) return;

	if(id == -1){
		ptr = aci_curLocationName;
	}
	else {
		m = aScrDisp -> get_matrix(id);
		ptr = m -> mech_name;
	}

	itm = aScrDisp -> alloc_menu_item();
	itm -> init_name(ptr);
	itm -> font = FMENU_TARGETS_FONT;
	itm -> space = FMENU_TARGETS_SPACE;
	itm -> fnc_code = fnc_id;
	itm -> flags |= FM_RANGE_FONT;
	itm -> init();
	mn -> add_item(itm);
	mn -> init_objects();

	if(mn -> flags & FM_ACTIVE && aScrDisp -> curMode == AS_INFO_MODE)
		mn -> set_redraw();
}

void aciDeleteTeleportMenuItem(int fnc_id)
{
	fncMenu* m = aScrDisp -> get_menu(FMENU_TELEPORT_MENU);
	if(!m) return;

	m -> deactivate_item(fnc_id);

	if(m -> flags & FM_ACTIVE && aScrDisp -> curMode == AS_INFO_MODE)
		m -> set_redraw();
}

void aciAdd2Targets(char* p)
{
	char* ptr;

	fncMenu* mn;
	fncMenuItem* itm;

	mn = aScrDisp -> get_menu(FMENU_TARGETS_MENU);
	if(!mn) return;

	itm = aScrDisp -> alloc_menu_item();
	itm -> init_name(p);
	itm -> font = FMENU_TARGETS_FONT;
	itm -> space = FMENU_TARGETS_SPACE;
	itm -> fnc_code = FMENU_OFF;
	itm -> init();
	mn -> add_item(itm);

	// rebuild...
	ptr = GetCompasTarget();

	if(ptr)
		mn -> curItem = mn -> get_obj(ptr);
	else
		mn -> curItem = mn -> get_obj(aciSTR_OFF);

	mn -> firstItem = mn -> curItem;
	mn -> init_objects();

	if(mn -> flags & FM_ACTIVE && aScrDisp -> curMode == AS_INFO_MODE)
		mn -> set_redraw();
}

void aciRefreshTargetsMenu(void)
{
	char* ptr;
	fncMenu* mn;

	mn = aScrDisp -> get_menu(FMENU_TARGETS_MENU);
	if(!mn) return;

	ptr = GetCompasTarget();

	if(ptr)
		mn -> curItem = mn -> get_obj(ptr);
	else
		mn -> curItem = mn -> get_obj(aciSTR_OFF);

	mn -> init_objects();

	if(mn -> flags & FM_ACTIVE && aScrDisp -> curMode == AS_INFO_MODE)
		mn -> set_redraw();
}

void aciDeleteFromTargets(char* p)
{
	char* ptr;
	fncMenu* mn;
	fncMenuItem* itm;

	mn = aScrDisp -> get_menu(FMENU_TARGETS_MENU);
	if(!mn) return;

	itm = mn -> get_obj(p);

	if(itm == mn -> curItem)
		SelectCompasTarget(NULL);

	if(itm){
		mn -> delete_obj(itm);
		aScrDisp -> free_menu_item(itm);
	}

	// rebuild...
	ptr = GetCompasTarget();

	if(ptr)
		mn -> curItem = mn -> get_obj(ptr);
	else
		mn -> curItem = mn -> get_obj(aciSTR_OFF);

	mn -> firstItem = mn -> curItem;
	mn -> init_objects();

	if(mn -> flags & FM_ACTIVE && aScrDisp -> curMode == AS_INFO_MODE)
		mn -> set_redraw();
}

void aciChangeAviIndex(void)
{
	switch(aciCurAviIndex){
		case ACI_PICTURE_AVI_ENG:
			aciCurAviIndex = (lang() == RUSSIAN) ? ACI_TEXT_AVI_RUS : ACI_TEXT_AVI_ENG;
			break;
		case ACI_TEXT_AVI_RUS:
		case ACI_TEXT_AVI_ENG:
			aciCurAviIndex = ACI_PICTURE_AVI_ENG;
			break;
	}
	aciInitAviID();
}

void aciShowCamerasMenu(int x,int y,int sx,int sy)
{
	aciBitmapMenu* p = aScrDisp -> get_bmenu(BMENU_CAMERAS_MENU);
	if(!p) return;

	if(p -> activeCount){
		p -> curCount --;
		if(p -> curCount <= 0){
			p -> go2upmenu();
			return;
		}
	}
	p -> redraw(x,y,sx,sy);
}

void aciHandleCameraEvent(int code,int data)
{
	aciBitmapMenu* m;
	aciBitmapMenuItem* p;
	switch(code){
		case BMENU_ITEM_ROT:
			camera_rotate_enable = data;
			iScrOpt[iCAMERA_TURN]->SetValueINT(data);
			if(RAM16 && data){
				camera_moving_xy_enable = camera_moving_z_enable = 0;
				camera_slope_enable = 0;
			}
			break;
		case BMENU_ITEM_ZOOM:
			camera_moving_xy_enable = camera_moving_z_enable = data;
			iScrOpt[iCAMERA_SCALE]->SetValueINT(data);
			if(RAM16 && data){
				camera_rotate_enable = 0;
				camera_slope_enable = 0;
			}
			break;
		case BMENU_ITEM_PERSP:
			camera_slope_enable = data;
			iScrOpt[iCAMERA_SLOPE]->SetValueINT(data);
			if(RAM16 && data){
				camera_moving_xy_enable = camera_moving_z_enable = 0;
				camera_rotate_enable = 0;
			}
			break;
	}
	if(RAM16 && data){
		m = aScrDisp -> get_bmenu(BMENU_CAMERAS_MENU);
		if(m){
			p = (aciBitmapMenuItem*)m -> items -> last;
			while(p){
				if(p -> ID != code) p -> curState = 0;
				p = (aciBitmapMenuItem*)p -> prev;
			}
		}
	}
	iSaveData();
	
}

void aciSetCameraMenu(void)
{
	aciBitmapMenu* m;
	aciBitmapMenuItem* p;

	m = aScrDisp -> get_bmenu(BMENU_CAMERAS_MENU);
	if(m){
		p = (aciBitmapMenuItem*)m -> items -> last;
		while(p){
			switch(p -> ID){
				case BMENU_ITEM_ROT:
					p -> curState = camera_rotate_enable;
					iScrOpt[iCAMERA_TURN]->SetValueINT(camera_rotate_enable);
					break;
				case BMENU_ITEM_ZOOM:
					p -> curState = camera_moving_xy_enable;
					iScrOpt[iCAMERA_SCALE]->SetValueINT(camera_moving_xy_enable);
					break;
				case BMENU_ITEM_PERSP:
					p -> curState = camera_slope_enable;
					iScrOpt[iCAMERA_SLOPE]->SetValueINT(camera_slope_enable);
					break;
			}
			p = (aciBitmapMenuItem*)p -> prev;
		}
	}
	m -> flags |= BM_REBUILD;
	iSaveData();
}

void aciPay4Item(invItem* p)
{
	int cr = aciGetCurCredits();
	uvsActInt* u = (uvsActInt*)p -> uvsDataPtr;
	cr -= u -> price;
	aciUpdateCurCredits(cr);
	p -> flags &= ~INV_ITEM_NO_PAY;
}

void aciHandleSubmenuEvent(int id,fncMenuItem* p)
{
	if(p -> eventPtr){
		aci_SendEvent(p -> eventPtr -> code,p -> eventPtr -> data);
		return;
	}
	switch(id){
		case FMENU_TARGETS_MENU:
			if(strcmp(p -> name,aciSTR_OFF)){
				SelectCompasTarget(p -> name);
			}
			else {
				SelectCompasTarget(NULL);
			}
			break;
		case FMENU_TELEPORT_MENU:
			aciTeleportEvent = p -> fnc_code;
			break;
	}
}

int aciGetCurCycle(void)
{
#ifdef _ACI_ESCAVE_DEBUG_
	return 1;
#else
	return uvsCurrentCycle;
#endif
}

#ifdef _DEBUG
void aciChangeMouseItem(void)
{
	invItem* temp_item;

	if(aScrDisp -> flags & AS_ISCREEN){
		temp_item = aScrDisp -> get_iitem(aScrDisp -> curItem -> ID + 1);
	}
	else {
		temp_item = aScrDisp -> get_item(aScrDisp -> curItem -> ID + 1);
	}
	if(!temp_item){
		if(aScrDisp -> flags & AS_ISCREEN){
			temp_item = aScrDisp -> get_iitem(1);
		}
		else {
			temp_item = aScrDisp -> get_item(1);
		}
	}
	temp_item -> clone(aScrDisp -> curItem);
	set_mouse_cursor(aScrDisp -> curItem -> frame,aScrDisp -> curItem -> ScreenSizeX,aScrDisp -> curItem -> ScreenSizeY);
}
#endif

void aciInitMechosInfo(invMatrix* p,InfoPanel* ip)
{
	ip -> free_list();
	if(p){
		ip -> add_item(p -> mech_name,-1,aciCurColorScheme[FM_SELECT_COL]);
		if(!uvsCurrentWorldUnable){
			if(GMechos)
				ip -> add_item(aciGetPrice((iListElement*)p,MECHOS_MODE,1));
		}
		ip -> set_redraw();
	}
}

int aciCheckSlot(void)
{
	if(!aScrDisp -> curMatrix) return 0;
	return aScrDisp -> curMatrix -> check_slot(AS_DEVICE_SLOT);
}

void aciSaveData(void)
{
	acsSaveData();
}

void aciLoadData(void)
{
	acsLoadData();
}

void aciPackFile(char* fname)
{
	int sz, sz1;
	char* p, *p1;

	XStream fh(fname,XS_IN);

	sz1 = sz = fh.size();
	sz1+=12;
	p = new char[sz];
	p1 = new char[sz1];
	fh.read(p,sz);
	fh.close();

	//sz1 = ZIP_compress(p1,sz,p,sz);
	/* ZLIB realization (stalkerg)*/
	p1[0] = (char)(8 & 0xFF); //8 it is DEFLATE method
	p1[1] = (char)((8 >> 8) & 0xFF);
	*(unsigned int*)(p1 + 2) = (unsigned int)sz;
	sz1-=2+4;
	int stat = compress((Bytef*)(p1+2+4),(uLongf*)&(sz1),(Bytef*)p,sz);
	sz1+=2+4;
	switch(stat){
		//case Z_OK: std::cout<<"Compress ok."<<std::endl; break;
		case Z_MEM_ERROR: std::cout<<"not enough memory."<<std::endl; break;
		case Z_BUF_ERROR: std::cout<<"not enough room in the output buffer."<<std::endl; break;
	};

	fh.open(fname,XS_OUT);
	fh.write(p1,sz1);
	fh.close();

	delete[] p;
	delete[] p1;
}

void aciUnPackFile(char* fname)
{
	int sz,sz1;
	char* p,*p1;

	XStream fh(fname,XS_IN);

	sz = fh.size();
	p = new char[sz];
	fh.read(p,sz);
	fh.close();

	sz1 = *(unsigned int*)(p + 2) + 12;//ZIP_GetExpandedSize(p);
	p1 = new char[sz1];

	//ZIP_expand(p1,sz1,p,sz);
	/* ZLIB realization (stalkerg)*/
	if(*(short*)(p)) { //if label = 0 not compress
		std::cout<<"aciUnPackFile DeCompress "<<fname<<" file."<<std::endl;
		int stat = uncompress((Bytef*)p1,(uLongf*)&sz1,(Bytef*)(p+2+4),sz-2-4);
		switch(stat){
			//case Z_OK: std::cout<<"DeCompress ok."<<std::endl; break;
			case Z_MEM_ERROR: std::cout<<"DeCompress not enough memory."<<std::endl; break;
			case Z_BUF_ERROR: std::cout<<"DeCompress not enough room in the output buffer."<<std::endl; break;
			case Z_DATA_ERROR: std::cout<<"DeCompress error data."<<std::endl; break;
		};
	} else {
		memcpy(p1, p + 2 + 4,(unsigned)(sz - 2 - 4));
	}
	fh.open(fname,XS_OUT);
	fh.write(p1,sz1);
	fh.close();

	delete[] p;
	delete[] p1;
}

#ifdef _ACTINT_MEMSTAT_
void aciDetectLeaks(void)
{
	int i,start,end;
	char* ptr;

	start = aMemStartIndex;
	end = aMemEndIndex - 1;

	aMemStatFile < "\r\n\r\n --------------- Leaks... --------------- \r\n";

	for(i = start; i < end; i ++){
		ptr = (char*)memPtr[i];
		if(ptr)
			aMemStatFile < "\r\nIndex -> " <= i < "...";
	}

	aMemStatFile < "\r\n\r\n ---------------------------------------- \r\n";
}
#endif

void ibsout(int x,int y,void* ptr)
{
	int _x,_y,cnt;
	int* offs_table = XGR_Obj.yOffsTable;

	unsigned char* vbuf = XGR_VIDEOBUF;
	unsigned char* pbuf = (unsigned char*)ptr;

	cnt = *((int*)pbuf);
	pbuf += 4;
	while(cnt){
		_x = *((int*)pbuf);
		pbuf += 4;
		_y = *((int*)pbuf);
		pbuf += 4;

		memcpy(vbuf + offs_table[_y + y] + _x + x,pbuf,cnt);
		pbuf += cnt;

		cnt = *((int*)pbuf);
		pbuf += 4;
	}
}

void aciPrepareMenus(void)
{

	aScrDisp -> clean_menus();
	aScrDisp -> prepare_menus();
	aciPrepareWorldsMenu();
}

void aciPrepareWorldsMenu(void)
{
	int i;
	aciWorldInfo* p1;
	if(CurrentWorld == -1) return;
	fncMenu* p = aScrDisp -> get_menu(FMENU_WORLDS_MENU);
	if(p){
		p -> prepare_items();
		for(i = 0; i < AS_MAX_WORLD; i ++){
			if(!aScrDisp -> wMap -> world_key(i)){
				p -> deactivate_item(1000 + i);
			}
		}
		if(aScrDisp -> flags & AS_WORLDS_INIT){
			p1 = (aciWorldInfo*)aScrDisp -> wMap -> world_list -> last;
			while(p1){
				if(p1 -> uvsID == CurrentWorld) p -> deactivate_item(1000 + p1 -> ID);
				p1 = (aciWorldInfo*)p1 -> prev;
			}
		}
	}
}

void aciChangeWorld(int w_id)
{
	int i,cur_id,fl = 0;
	aciWorldInfo* p,*p1;

	if(w_id == -1) return;
	cur_id = aScrDisp -> wMap -> world_ids[w_id];
	if(cur_id == -1) return;

	p = aScrDisp -> wMap -> world_ptr[cur_id];
	if(!p) return;

	for(i = 0; i < AS_MAX_WORLD; i ++){
		p1 = aScrDisp -> wMap -> world_ptr[i];
		if(p1){
			if(p1 -> flags & AS_ACCESSIBLE_WORLD){
				p1 -> flags &= ~AS_ACCESSIBLE_WORLD;
				fl = 1;
			}
			if(p1 -> flags & AS_CURRENT_WORLD){
				p1 -> flags &= ~AS_CURRENT_WORLD;
				p1 -> flags |= AS_VISIBLE_WORLD;
			}
		}
	}
	if(fl){
		for(i = 0; i < AS_MAX_WORLD; i ++){
			p1 = aScrDisp -> wMap -> world_ptr[i];
			if(i != cur_id && p1){
				if(p1 -> links[cur_id] || p -> links[i]){
					p1 -> flags |= AS_ACCESSIBLE_WORLD;
				}
			}
		}
	}
	p -> flags |= AS_CURRENT_WORLD;
	aScrDisp -> wMap -> flags |= WMAP_REBUILD_PIC;
}

void aciInitShopButtons(void)
{
	iScreenObject* obj;
	iScreenElement* p,*p1,*p2,*p3,*p4;

	fncMenu* mn = aScrDisp -> get_imenu(SHOP_ITEMS_MENU_ID);

	p = NULL;
	p1 = (iScreenElement*)aScrDisp -> curLocData -> objList[ACI_WEAPONS_BUTTON_ID];
	p2 = (iScreenElement*)aScrDisp -> curLocData -> objList[ACI_MECHOS_BUTTON_ID];
	p3 = (iScreenElement*)aScrDisp -> curLocData -> objList[ACI_ITEMS_BUTTON_ID];
	p4 = (iScreenElement*)aScrDisp -> curLocData -> objList[ACI_MENU_BUTTON_ID];

	if(p1){
		obj = (iScreenObject*)p1 -> owner;
		p1 -> change_terrainNum(aScrDisp -> curLocData -> numIndex[ACI_BUTTON_OFF_TYPE]);
		obj -> flags |= OBJ_MUST_REDRAW;
	}
	if(p2){
		obj = (iScreenObject*)p2 -> owner;
		p2 -> change_terrainNum(aScrDisp -> curLocData -> numIndex[ACI_BUTTON_OFF_TYPE]);
		obj -> flags |= OBJ_MUST_REDRAW;
	}
	if(p3){
		obj = (iScreenObject*)p3 -> owner;
		p3 -> change_terrainNum(aScrDisp -> curLocData -> numIndex[ACI_BUTTON_OFF_TYPE]);
		obj -> flags |= OBJ_MUST_REDRAW;
	}
	if(p4){
		obj = (iScreenObject*)p4 -> owner;
		obj -> flags |= OBJ_MUST_REDRAW;

		if(mn && mn -> flags & FM_ACTIVE)
			p4 -> change_terrainNum(aScrDisp -> curLocData -> numIndex[ACI_BUTTON_ON_TYPE]);
		else
			p4 -> change_terrainNum(aScrDisp -> curLocData -> numIndex[ACI_BUTTON_OFF_TYPE]);
	}

	switch(iEvLineID){
		case MECHOS_MODE:
		case MECHOS_LIST_MODE:
		case CHANGE_2_MECHOS_MODE:
			p = p2;
			break;
		case WEAPONS_MODE:
		case WEAPONS_LIST_MODE:
		case CHANGE_2_WEAPONS_MODE:
			p = p1;
			break;
		case ITEMS_MODE:
		case ITEMS_LIST_MODE:
		case CHANGE_2_ITEMS_MODE:
			p = p3;
			break;
	}
	if(p){
		p -> change_terrainNum(aScrDisp -> curLocData -> numIndex[ACI_BUTTON_ON_TYPE]);
	}
}

void mem_rectangle(int x,int y,int sx,int sy,int bsx,int col_in,int col_out,int mode,void* buf)
{
	int i,index,_sx,_sy;
	unsigned char* p = (unsigned char*)buf;

	index = x + y * bsx;
	memset(p + index,col_out,sx);
	for(i = 0; i < sy; i ++){
		p[index] = p[index + sx - 1] = col_out;
		index += bsx;
	}
	memset(p + index - bsx,col_out,sx);

	if(mode){
		_sx = sx - 2;
		_sy = sy - 2;

		if(_sx <= 0 || _sy <= 0) return;
		index = (x + 1) + (y + 1) * bsx;
		for(i = 0; i < _sy; i ++){
			memset(p + index,col_in,_sx);
			index += bsx;
		}
	}
}

void aciGetItemCoords(actintItemData* p,int& x,int& y)
{
	invItem* itm = (invItem*)p -> actintOwner;

	x = itm -> MatrixX;
	y = itm -> MatrixY;
}

void aciGetItemCoords(int id,int& x,int& y)
{
	invItem* itm;
	if(!aScrDisp -> curMatrix) return;
	itm = (invItem*)aScrDisp -> curMatrix -> items -> last;

	while(itm){
		if(itm -> ID == id){
			x = itm -> MatrixX;
			y = itm -> MatrixY;
			return;
		}
		itm = (invItem*)itm -> prev;
	}
}

int aciPutItem(int id,int x,int y)
{
	invItem* p,*p1;
	if(aScrDisp -> flags & AS_ISCREEN)
		p1 = aScrDisp -> get_iitem(id);
	else
		p1 = aScrDisp -> get_item(id);

	if(!p1) ErrH.Abort("Bad aciPutItem() argument...");
	p = aScrDisp -> alloc_item();
	
	p1 -> clone(p);
	p -> item_ptr = NULL;
	
	if(aScrDisp -> curMatrix){
		if(x != -1){
			if(!aScrDisp -> put_item_xy(p,x,y,0)){
				aScrDisp -> free_item(p);
				return 0;
			}
		}
		else {
			if(!aScrDisp -> put_item_auto(p)){
				aScrDisp -> free_item(p);
				return 0;
			}
		}
		return 1;
	}
	aScrDisp->free_item(p);
	return 0;
}

void aciRemoveItem(actintItemData* d)
{
	aScrDisp -> remove_item(d);
}

void aciNewMechos(int newID)
{
	if(aScrDisp -> curMatrix)
		aScrDisp -> free_matrix(aScrDisp -> curMatrix);

	if(newID != -1)
		aScrDisp -> curMatrix = aScrDisp -> alloc_matrix(newID);
	else
		aScrDisp -> curMatrix = NULL;
}

void aciPromptData::free_mem(void)
{
	int i;
	if(StrBuf){
		for(i = 0; i < NumStr; i ++){
			if(StrBuf[i])
				delete[] StrBuf[i];
		}
		delete[] StrBuf;
	}
	if(TimeBuf) {
		delete[] TimeBuf;
	}
	if(PosX) {
		delete[] PosX;
	}
	if(PosY) {
		delete[] PosY;
	}
	if(ColBuf) {
		delete[] ColBuf;
	}

	StrBuf = NULL;
	TimeBuf = NULL;

	PosX = NULL;
	PosY = NULL;

	ColBuf = NULL;

	NumStr = 0;
	fontID = -1;
}

aciPromptData::~aciPromptData(void)
{
	free_mem();
}

aciPromptData::aciPromptData(int size)
{
	NumStr = 0;
	StrBuf = NULL;
	TimeBuf = NULL;
	PosX = NULL;
	PosY = NULL;
	ColBuf = NULL;
	align_type = ACI_PROMPT_UP;
	CurTimer = 0;
	fontID = -1;

	alloc_mem(size);
}

void aciPromptData::alloc_mem(int size)
{
	int i;
	NumStr = size;
	StrBuf = new unsigned char*[size];
	TimeBuf = new int[size];
	PosX = new int[size];
	PosY = new int[size];
	ColBuf = new unsigned int[size];
	for(i = 0; i < size; i ++){
		StrBuf[i] = NULL;
		TimeBuf[i] = 0;
		ColBuf[i] = 0;
	}
	fontID = -1;
}

void aciPromptData::copy_data(aciPromptData* src)
{
	int i,sz;
	free_mem();
	alloc_mem(src -> NumStr);

	align_type = src -> align_type;

	if(src -> StrBuf){
		for(i = 0; i < NumStr; i ++){
			if(src -> StrBuf[i]){
				sz = strlen((char*)src -> StrBuf[i]) + 1;
				StrBuf[i] = new unsigned char[sz];
				strcpy((char*)StrBuf[i],(char*)src -> StrBuf[i]);
				TimeBuf[i] = src -> TimeBuf[i];
				ColBuf[i] = src -> ColBuf[i];
			}
		}
	}
	CurTimer = 0;
}

#define ACI_PROMPT_OFFSET	50

#define ACI_PROMPT_FONT 	7
#define ACI_PROMPT_COLOR	175

#define ACI_PROMPT_DELTA_X	1
#define ACI_PROMPT_DELTA_Y	1
void aciPromptData::redraw(int scr_x,int scr_y,int scr_sx,int scr_sy)
{
	int i,x,y,sy = 0,sx,sz;
	if(!StrBuf) return;

	int font = ACI_PROMPT_FONT;

	if(!(aScrDisp -> flags & AS_FULLSCR)){
		font = aScrDisp -> curIbs -> fontID;
	}
	if(fontID != -1) font = fontID;

	if(align_type == ACI_PROMPT_CENTER){
		for(i = 0; i < NumStr; i ++){
			if(StrBuf[i]){
				sy += aScrFonts[font] -> SizeY + ACI_PROMPT_DELTA_Y;
			}
		}
		y = scr_y + (scr_sy - sy) / 2;
	}
	else
		y = scr_y + ACI_PROMPT_OFFSET;

	for(i = 0; i < NumStr; i ++){
		if(StrBuf[i]){
			sx = aStrLen(StrBuf[i],font,ACI_PROMPT_DELTA_X);
			x = scr_x + (scr_sx - sx) / 2;
			PosX[i] = x;
			PosY[i] = y;

//			  if(x < 0) ErrH.Abort("Prompt string out of screen...");
			while(x < 0){
				sz = strlen((char*)StrBuf[i]) - 1;
				StrBuf[i][sz] = 0;
				sx = aStrLen(StrBuf[i],font,ACI_PROMPT_DELTA_X);
				x = scr_x + (scr_sx - sx) / 2;
				PosX[i] = x;
				PosY[i] = y;
			}

			y += aScrFonts[font] -> SizeY + ACI_PROMPT_DELTA_Y;
		}
	}
	aWriteHelpString(NumStr,PosX,PosY,font,StrBuf,CurTimer,ACI_PROMPT_COLOR,1,ColBuf);
}

void aciPromptData::add_str(int num,unsigned char* p)
{
	int sz;
	if(StrBuf[num]) delete[] StrBuf[num];
	sz = strlen((char*)p) + 1;
	StrBuf[num] = new unsigned char[sz];
	strcpy((char*)StrBuf[num],(char*)p);
}

void aciPromptData::quant(void)
{
	int i,sz = 0;

	if(!NumStr) return;

	if (NetworkON) {
		CurTimer += 2;
	} else {
		CurTimer ++;
	}
	for(i = 0; i < NumStr; i ++){
		if(StrBuf[i]){
			if(CurTimer > TimeBuf[i]){
				delete[] StrBuf[i];
				StrBuf[i] = NULL;
			}
			else
				sz ++;
		}
	}
	if(!sz) free_mem();
}

void aciSendPrompt(aciPromptData* p)
{
	aScrDisp -> curPrompt -> copy_data(p);
	aScrDisp -> curPrompt -> CurTimer = 0;
}

void aciActivateItemFunction(int itemID,int fncID)
{
	fncMenu* m = NULL;
	invItem* p = aScrDisp -> get_item(itemID);
	if(p && p -> menu){
		m = (fncMenu*)p -> menu;
		m -> activate_item(fncID + 2000);
	}

	if(m && m -> flags & FM_ACTIVE && aScrDisp -> curMode == AS_INFO_MODE)
		m -> set_redraw();
}

void aciDeactivateItemFunction(int itemID,int fncID)
{
	fncMenu* m = NULL;
	invItem* p = aScrDisp -> get_item(itemID);
	if(p && p -> menu){
		m = (fncMenu*)p -> menu;
		m -> deactivate_item(fncID + 2000);
	}

	if(m && m -> flags & FM_ACTIVE && aScrDisp -> curMode == AS_INFO_MODE)
		m -> set_redraw();
}

int acsQuant(void)
{
	int ret,k,firstQuant = 0;
	if(!acsAllocFlag){
		acsAllocFlag = 1;
		if(NetworkON) aciKeyboardLocked = 1;
		acsScrD -> curScrID = acsScreenID;
		acsScrD -> QuantCode = 0;
		XGR_MouseObj.DisablePrompt();
		XGR_MouseShow();
		acsScrD -> alloc_mem();

		if (!(aScrDisp -> flags & AS_ISCREEN)) { acsScrD -> curScr -> ChangeCoords((XGR_MAXX - 640)/2, (XGR_MAXY - 480)/2); }

		EffectsOff();
		firstQuant = 1;
		if(acsScrD -> curScrID >= 3){
			aciCloseGate();
			acsScrD -> Quant(0);
			aciOpenGate();
		}
		KeyBuf -> clear();
	}
	while(KeyBuf->size){
		SDL_Event *e = KeyBuf->get();
		if (e->type == SDL_KEYDOWN) {
			k = sdlEventToCode(e);
		}
		if (e->type == SDL_KEYDOWN || e->type == SDL_TEXTINPUT) {
			acsScrD->KeyTrap(0, e);
			if (e->key.keysym.scancode == SDL_SCANCODE_ESCAPE && !acsScrD -> QuantCode) {
				acsScrD -> QuantCode = 1;
			}
		}

	}
	if(aScrDisp -> flags & aMS_MOVED){
		acsScrD -> KeyTrap(iMOUSE_MOVE_CODE, nullptr);
		aScrDisp -> flags ^= aMS_MOVED;
		if(aciMouseFlagL){
			acsScrD -> KeyTrap(iMOUSE_LEFT_MOVE, nullptr);
		}
		if(aciMouseFlagR){
			acsScrD -> KeyTrap(iMOUSE_RIGHT_MOVE, nullptr);
		}
	}
	if(aScrDisp -> flags & aMS_LEFT_PRESS){
		acsScrD -> KeyTrap(iMOUSE_LEFT_PRESS_CODE, nullptr);
		aScrDisp -> flags ^= aMS_LEFT_PRESS;
	}
	if(aScrDisp -> flags & aMS_RIGHT_PRESS){
		acsScrD -> KeyTrap(iMOUSE_RIGHT_PRESS_CODE, nullptr);
		aScrDisp -> flags ^= aMS_RIGHT_PRESS;
	}
	if(NetworkON){
		acsScrD -> flags |= ACS_FORCED_REDRAW;
		ret = acsScrD -> Quant(actIntLog);
	}
	else
		ret = acsScrD -> Quant();

	if(firstQuant){
		acsPrepareSlotNameInput(acsCurrentSlotID,acsCurrentSlotNum);
		acsScrD -> PrepareInput(acsCurrentSlotID);
	}
	if(ret){
		acsAllocFlag = 0;
		if (!(aScrDisp -> flags & AS_ISCREEN)) { acsScrD -> curScr -> ChangeCoords(-(XGR_MAXX - 640)/2, -(XGR_MAXY - 480)/2); }
		acsScrD -> free_mem();
		if(aScrDisp -> flags & AS_FULLSCR && !(aScrDisp -> flags & AS_ISCREEN)){
			XGR_MouseHide();
		}
		else
			aScrDisp -> flags &= ~AS_FULL_REDRAW;

		iScrDisp -> flags &= ~MS_LEFT_PRESS;
		iScrDisp -> flags &= ~MS_RIGHT_PRESS;
		iScrDisp -> flags &= ~MS_MOVED;
		iHandleExtEvent(iEXT_UPDATE_TUTORIAL_MODE);
		iHandleExtEvent(iEXT_UPDATE_SOUND_MODE);
		iHandleExtEvent(iEXT_UPDATE_SOUND_VOLUME);

		if(NetworkON) aciKeyboardLocked = 0;

		acsScreenID = 1;
		iSaveData();
		KeyBuf -> clear();
		return 1;
	}
	return 0;
}

void acsHandleExtEvent(int code,int data0,int data1,int data2)
{
	int val,max_val,i_val,i_max_val;
	switch(code){
		case ACS_GLOBAL_EXIT:
			if(!(aScrDisp -> flags & AS_ISCREEN))
				GameQuantReturnValue = RTO_LOADING3_ID;
			else {
				iScrDisp -> flags |= SD_EXIT;
				iScrExitCode = 1;
				iAbortGameFlag = 1;
			}
			if(NetworkON) iMultiFlag = 1;
			acsScrD -> QuantCode = 1; // resume game pause
			if(!GameOverID)
				GameOverID = GAME_OVER_ABORT;
			break;
		case ACS_INIT_TUTORIAL_MODE:
			acsSetStrState(ACS_TUTORIAL_MODE,!iGetOptionValue(iTUTORIAL_ON));
			break;
		case ACS_SET_TUTORIAL_MODE:
			iSetOptionValue(iTUTORIAL_ON,!acsGetStrState(ACS_TUTORIAL_MODE));
			iHandleExtEvent(iEXT_UPDATE_TUTORIAL_MODE);
			break;
		case ACS_SET_AUTORUN_MODE:
			aciAutoRun = acsGetStrState(ACS_AUTORUN_MODE);
			iScrOpt[iAUTO_ACCELERATION]->SetValueINT(acsGetStrState(ACS_AUTORUN_MODE));
			break;
		case ACS_SET_SOUND_MODE:
			iSetOptionValue(iSOUND_ON,!acsGetStrState(ACS_SOUND_MODE));
			iHandleExtEvent(iEXT_UPDATE_SOUND_MODE);
			break;
		case ACS_SET_MUSIC_MODE:
			iSetOptionValue(iMUSIC_ON,!acsGetStrState(ACS_MUSIC_MODE));
			iHandleExtEvent(iEXT_UPDATE_MUSIC_MODE);
			break;
		case ACS_INIT_AUTORUN_MODE:
//			acsSetStrState(ACS_AUTORUN_MODE,aciAutoRun);
			aciAutoRun = iGetOptionValue(iAUTO_ACCELERATION);
			acsSetStrState(ACS_AUTORUN_MODE,iGetOptionValue(iAUTO_ACCELERATION));
			break;
		case ACS_INIT_SOUND_MODE:
			acsSetStrState(ACS_SOUND_MODE,!iGetOptionValue(iSOUND_ON));
			break;
		case ACS_INIT_SOUND_VOLUME:
			acsScrD -> SetScroller(ACS_SOUND_VOLUME,iGetOptionValue(iSOUND_VOLUME_CUR),iGetOptionValue(iSOUND_VOLUME_MAX));
			break;
		case ACS_SET_SOUND_VOLUME:
			val = acsScrD -> GetObjectValue(ACS_SOUND_VOLUME);
			max_val = acsScrD -> GetObjectValue(ACS_SOUND_VOLUME,1);
			i_max_val = iGetOptionValue(iSOUND_VOLUME_MAX);
			i_val = (val * i_max_val) / max_val;
			iSetOptionValue(iSOUND_VOLUME_CUR,i_val);
			iHandleExtEvent(iEXT_UPDATE_SOUND_VOLUME);
			break;
		case ACS_INIT_MUSIC_MODE:
			acsSetStrState(ACS_MUSIC_MODE,!iGetOptionValue(iMUSIC_ON));
			break;
		case ACS_INIT_MUSIC_VOLUME:
			acsScrD -> SetScroller(ACS_MUSIC_VOLUME,iGetOptionValue(iMUSIC_VOLUME_CUR),iGetOptionValue(iMUSIC_VOLUME_MAX));
			break;
		case ACS_SET_MUSIC_VOLUME:
			val = acsScrD -> GetObjectValue(ACS_MUSIC_VOLUME);
			max_val = acsScrD -> GetObjectValue(ACS_MUSIC_VOLUME,1);
			i_max_val = iGetOptionValue(iMUSIC_VOLUME_MAX);
			i_val = (val * i_max_val) / max_val;
			iSetOptionValue(iMUSIC_VOLUME_CUR,i_val);
			iHandleExtEvent(iEXT_UPDATE_MUSIC_VOLUME);
			break;
		case ACS_CHANGE_MUSIC_MODE:
			acsChangeStrState(ACS_MUSIC_MODE);
			iSetOptionValue(iMUSIC_ON,!acsGetStrState(ACS_MUSIC_MODE));
			iHandleExtEvent(iEXT_UPDATE_MUSIC_MODE);
			break;
		case ACS_CHANGE_AUTORUN_MODE:
			acsChangeStrState(ACS_AUTORUN_MODE);
			aciAutoRun = acsGetStrState(ACS_AUTORUN_MODE);
			iScrOpt[iAUTO_ACCELERATION]->SetValueINT(acsGetStrState(ACS_AUTORUN_MODE));
			break;
		case ACS_CHANGE_SOUND_MODE:
			acsChangeStrState(ACS_SOUND_MODE);
			iSetOptionValue(iSOUND_ON,!acsGetStrState(ACS_SOUND_MODE));
			iHandleExtEvent(iEXT_UPDATE_SOUND_MODE);
			break;
		case ACS_CHANGE_TUTORIAL_MODE:
			acsChangeStrState(ACS_TUTORIAL_MODE);
			iSetOptionValue(iTUTORIAL_ON,!acsGetStrState(ACS_TUTORIAL_MODE));
			iHandleExtEvent(iEXT_UPDATE_TUTORIAL_MODE);
			break;
		case ACS_INIT_SLOT_NAME:
			acsPrepareSlotName(data0,data1);
			break;
		case ACS_INIT_SLOT_NAME_INPUT:
			acsPrepareSlotNameInput(data0,data1);
			break;
		case ACS_SAVE_GAME:
			acsSaveData();
			break;
	}
}

void aciFreeAll(void)
{
	delete aScrDisp;
	aScrDisp = NULL;
	delete acsScrD;
	acsScrD = NULL;
	free_cell_frame();

	if(aIndArrowBML)
		delete aIndArrowBML;
	if(aIndDataBML)
		delete aIndDataBML;
	if(aIndBackBML)
		delete aIndBackBML;

	aIndArrowBML = NULL;
	aIndDataBML = NULL;
	aIndBackBML = NULL;

	aciLoadLog = 0;
}

void acsPrepareSlotName(int id,int slot_num)
{
	int len,time_len;
	XBuffer XBuf;
	XStream fh(0);

	aciScreenInputField* p = (aciScreenInputField*)acsScrD -> GetObject(id);
	if(!p) return;

	XBuf.init();
	XBuf < "savegame/save";
	if(slot_num < 10) XBuf < "0";
	XBuf <= slot_num < ".dat";

	if(fh.open(XBuf.address(),XS_IN)){
		fh > len;

		time_len = (len >> 16) & 0xFF;
		len &= 0xFF;

		memset(p -> string,0,p -> MaxStrLen + 2);
		fh.read(p -> string,len);
		fh.close();
	}
	else
		strcpy(p -> string,aciSTR_EMPTY_SLOT);
}

void acsPrepareSlotNameInput(int id,int slot_num)
{
	int sz;
	aciScreenInputField* p = (aciScreenInputField*)acsScrD -> GetObject(id);
	if(!p) return;
	if(acsScrD -> activeInput){
		acsScrD -> activeInput -> StopEvents();
		acsScrD -> CancelInput();
	}
	if(!strcmp(p -> string,aciSTR_EMPTY_SLOT) || !strcmp(p -> string,aciSTR_UNNAMED_SAVE)){
		sz = strlen(p -> string) + 1;
		acsBackupStr = new char[sz];
		strcpy(acsBackupStr,p -> string);
		memset(p -> string,0,p -> MaxStrLen + 2);
	}
	acsCurrentSlotID = id;
	acsCurrentSlotNum = slot_num;
}

#if defined(__APPLE__) || __GNUC__ < 9
void createDirIfNotExist(const char* dirName) {
	struct stat info;
	if (stat(dirName, &info) != 0) {
		std::cout<<"Directory "<<dirName<<" not found. Created it..."<< std::endl;
		const int dirr_err = mkdir(dirName, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);

		if (dirr_err == -1) {
			std::string subj = "Dir name: ";
			subj += dirName;
			ErrH.Abort("Can't create directory", XERR_USER, 0, subj.c_str());
			return;
		}
	}
}
#else
void createDirIfNotExist(const fs::path& dirName) {
	if (fs::exists(dirName)) {
		return;
	}

	std::cout<<"Directory "<<dirName<<" not found. Created it..."<<std::endl;
	fs::create_directory(dirName);
}
#endif

void acsSaveData(void)
{
	int i,null_flag = 1;
	int slot = acsCurrentSlotNum,len,time_len;
	XBuffer XBuf;
	XStream fh;

	const char* ptr = NULL;
	aciScreenInputField* p;

	if(aciAutoSaveFlag){
		ptr = aciSTR_AUTOSAVE;
		slot = ACS_NUM_SLOTS;
	}
	else {
		if(acsScrD -> activeInput) acsScrD -> DoneInput();
		p = (aciScreenInputField*)acsScrD -> GetObject(acsCurrentSlotID);
		ptr = p -> string;
	}
	createDirIfNotExist("savegame");
	if(slot != -1 && ptr){
		XBuf < "savegame/save";
		if(slot < 10) XBuf < "0";
		XBuf <= slot < ".dat";
		len = strlen(ptr);
		for(i = 0; i < len; i ++){
			if(ptr[i] && ptr[i] != ' ')
				null_flag = 0;
		}
		if(null_flag){
			ptr = aciSTR_UNNAMED_SAVE;
			len = strlen(ptr);
		}

		if(aScrDisp -> flags & AS_INV_MOVE_ITEM && aScrDisp -> curItem -> uvsDataPtr){
			((uvsActInt*)aScrDisp -> curItem -> uvsDataPtr) -> pos_x = aScrDisp -> curItem -> MatrixX = -1;
			((uvsActInt*)aScrDisp -> curItem -> uvsDataPtr) -> pos_y = aScrDisp -> curItem -> MatrixY = -1;
		}

		fh.open(XBuf.address(),XS_OUT);

		aciXConv -> init();
		*aciXConv < aScrDisp -> curLocData -> nameID2 < ", ";

		*aciXConv <= ConTimer.day < ":";
		if(ConTimer.hour < 10) *aciXConv < "0";
		*aciXConv <= ConTimer.hour < ":";
		if(ConTimer.min < 10) *aciXConv < "0";
		*aciXConv <= ConTimer.min < ":";
		if(ConTimer.sec < 10) *aciXConv < "0";
		*aciXConv <= ConTimer.sec;
		time_len = aciXConv -> tell();

		fh < (len | (time_len << 16));
		
		fh.write(ptr,len);
		
		fh.write(aciXConv -> address(),time_len);
		
		
		CurrentWorld = prevWorld;
		
		uniVangSave(fh);

		aciSaveUVSList(fh,GItem);
		aciSaveUVSList(fh,GMechos);
		aciSaveUVSList(fh,GGamer);
		aciSaveUVSList(fh,GGamerMechos);
		aciSaveUVSList(fh,GCharItem);
		aciSaveUVSList(fh,GTreasureItem);

		aScrDisp -> save_data(&fh);

		GeneralSystemSave(fh);
		dgD -> save(fh);
		CurrentWorld = -1;
		fh.close();

#ifdef _ACI_PACK_SAVES_
		acsCompressData(XBuf.address());
#endif
	}
	aciAutoSaveFlag = 0;
}

void acsLoadData(void)
{
	int slot = iSlotNumber,len,time_len;
	XBuffer XBuf;
	XStream fh;

	if(slot != -1){
		XBuf < "savegame/save";
		if(slot < 10) XBuf < "0";
		XBuf <= slot < ".dat";

#ifdef _ACI_PACK_SAVES_
		acsDecompressData(XBuf.address());
#endif
		std::cout<<"acsLoadData"<<std::endl;
		fh.open(XBuf.address(),XS_IN);
		fh > len;
		time_len = (len >> 16) & 0xFF;
		len &= 0xFF;
		fh.seek(len + sizeof(int) + time_len,XS_BEG);
		uniVangLoad(fh);

		aciLoadUVSList(fh,&GItem,1);
		aciLoadUVSList(fh,&GMechos);
		aciLoadUVSList(fh,&GGamer,1);
		aciLoadUVSList(fh,&GGamerMechos);
		aciLoadUVSList(fh,&GCharItem,1);
		aciLoadUVSList(fh,&GTreasureItem,1);

		aScrDisp->load_data(&fh);
		//stalkerg Попытемся проверить трюм магазина и трюм мехоса в мире
		//Эти трюмы должны совпадать по содержимому, а иначе игра будет умирать
		if(aScrDisp->curMatrix) { //Существует ли матрица
			listElem *el = NULL;
			listElem *el2 = NULL;
			uvsActInt *new_item = NULL;
			uvsActInt *iter_gamer = (uvsActInt*)GGamer;
			invItem* p,*p1;
			while (iter_gamer) {
				bool item_detect = false;
				int i, j, index = 0;
				for(i = 0; i < aScrDisp->curMatrix->SizeY; i++){
					for(j = 0; j < aScrDisp->curMatrix->SizeX; j++){
						invMatrixCell *cell = aScrDisp->curMatrix->matrix[index];
						/*std::cout<<"GGamer type:"<<iter_gamer->type
							<<" x:"<<iter_gamer->pos_x
							<<" y:"<<iter_gamer->pos_y
							<<" i:"<<i
							<<" j:"<<j
							<<" mcell flags:"<<cell->flags
							<<" mcell type:"<<cell->type
							<<" index:"<<index<<std::endl;*/
						if(cell->flags & AS_BUSY_CELL && cell->item) {
							/*std::cout<<"item_type:"<<cell->item->ID
								<<" MX:"<<cell->item->MatrixX
								<<" MY:"<<cell->item->MatrixY<<std::endl;*/
							if (cell->item->ID == iter_gamer->type &&
								cell->item->MatrixX == iter_gamer->pos_x &&
								cell->item->MatrixY == iter_gamer->pos_y
							) {
								item_detect = true;
								goto end_find;
							}
						}
						index++;
					}
				}
				std::cout<<"Can't find item GGamer type:"<<iter_gamer->type
						<<" pos_x:"<<iter_gamer->pos_x
						<<" pos_y:"<<iter_gamer->pos_y
						<<" and we make it"<<std::endl;
				if(aScrDisp->flags & AS_ISCREEN)
					p = aScrDisp->get_iitem(iter_gamer->type);
				else
					p = aScrDisp->get_item(iter_gamer->type);

				p1 = aScrDisp->alloc_item();
				p->clone(p1);
				aScrDisp->curMatrix->put_item(iter_gamer->pos_x, iter_gamer->pos_y, p1, 1);
				end_find:
				//Is it duplicate?
				if (el != NULL) {
					el2 = el;
					while (el2) {
						uvsActInt *item = dynamic_cast<uvsActInt*> (el2);
						if (item->pos_x == iter_gamer->pos_x && item->pos_y == iter_gamer->pos_y && item->type == iter_gamer->type) {
							std::cout<<"We find duplicate item GGamer type:"<<iter_gamer->type
								<<" pos_x:"<<iter_gamer->pos_x
								<<" pos_y:"<<iter_gamer->pos_y
								<<std::endl;
							goto next;
						}
						el2 = el2->next;
					}
				}
				new_item = new uvsActInt;
				new_item->pos_x = iter_gamer->pos_x;
				new_item->pos_y = iter_gamer->pos_y;
				new_item->type = iter_gamer->type;
				new_item->price = iter_gamer->price;
				new_item->sell_price = iter_gamer->sell_price;
				new_item->param1 = iter_gamer->param1;
				new_item->param2 = iter_gamer->param2;
				new_item->link(el);

				next:
				iter_gamer = (uvsActInt*)iter_gamer->next;
			}
			
			aciFreeUVSList(GGamer);
			GGamer = (uvsActInt*)el;
			
		}
		GeneralSystemLoad(fh);
		dgD -> load(fh);
		fh.close();

#ifdef _ACI_PACK_SAVES_
		acsCompressData(XBuf.address());
#endif
	}
}

void aciAcceptRubox(void)
{
	aciML_D -> key_trap(AML_RUBOX_CODE,AML_KEY);
}

void aciGetRubox(void)
{
	uvsActInt* p;
	invItem* itm;
	listElem* el = (listElem*)GGamer;
	if(!aciPutItem(ACI_RUBOX,-1,-1)) ErrH.Abort("aciPutItem() failed...");

	itm = (invItem*)aScrDisp -> curMatrix -> items -> last;
	aScrDisp -> curMatrix -> remove_item(itm);

	if(aScrDisp -> flags & AS_INV_MOVE_ITEM){
		aScrDisp -> flags ^= AS_INV_MOVE_ITEM;
		aciSell_Item(aScrDisp -> curItem);
		aScrDisp -> curItem = NULL;
	}

	aScrDisp -> flags |= AS_INV_MOVE_ITEM;
	aScrDisp -> curItem = itm;
	set_mouse_cursor(itm -> frame,itm -> ScreenSizeX,itm -> ScreenSizeY);

	p = new uvsActInt;
	p -> type = ACI_RUBOX;
	itm -> uvsDataPtr = p;

	p -> link(el);
	GGamer = (uvsActInt*)el;
}

void aciAddRubox(void)
{
	uvsActInt* p;
	invItem* itm;
	listElem* el = (listElem*)GGamer;
	if(!aciPutItem(ACI_RUBOX,-1,-1)) ErrH.Abort("aciPutItem() failed...");

	itm = (invItem*)aScrDisp -> curMatrix -> items -> last;

	p = new uvsActInt;
	p -> type = ACI_RUBOX;
	itm -> uvsDataPtr = p;

	p -> pos_x = itm -> MatrixX;
	p -> pos_y = itm -> MatrixY;

	p -> link(el);
	GGamer = (uvsActInt*)el;
}

void aciExpropriation(void)
{
	if(!aScrDisp -> curMatrix) return;
	invItem* itm = (invItem*)aScrDisp -> curMatrix -> items -> last,*itm1;
	uvsActInt* ptr = (uvsActInt*)aScrDisp -> curMatrix -> uvsDataPtr;
	if(!ptr) ErrH.Abort("Bad sell mechos...");
	while(itm){
		itm1 = (invItem*)itm -> prev;
		aScrDisp -> curMatrix -> remove_item(itm);
		itm -> flags |= INV_ITEM_NO_PAY;
		aciSell_Item(itm);
		itm = itm1;
	}
}

void aciChange2Raffa(void)
{
	invMatrix* m,*rf = NULL;
	uvsActInt* u,*u1;
	listElem* el;

	if(aScrDisp -> curMatrix) aciExpropriation();

	aScrDisp -> curMatrix = NULL;
	if(GGamerMechos){
		//std::cout<<"aciChange2Raffa if(GGamerMechos) "<<GGamerMechos<<std::endl;
		m = aScrDisp -> get_imatrix(GGamerMechos -> type);
		if(!m -> uvsDataPtr) {
			m -> uvsDataPtr = GGamerMechos;
		}
		//std::cout<<"aciChange2Raffa el = GMechos "<<GMechos<<std::endl;
		el = GMechos;
		GGamerMechos -> link(el);
		GMechos = (uvsActInt*)el;
		//std::cout<<"aciChange2Raffa GMechos = (uvsActInt*)el "<<GMechos<<std::endl;
	}
	GGamerMechos = NULL;

	m = (invMatrix*)aScrDisp -> i_matrixList -> last;
	while(m){
		//std::cout<<"aciChange2Raffa invMatrix type: "<<m->type<<std::endl;
		if(m -> flags & IM_RAFFA){
			if(m -> uvsDataPtr){
				u = (uvsActInt*)m -> uvsDataPtr;
				//std::cout<<"aciChange2Raffa GMechos "<<GMechos<<std::endl;
				u -> delink(GMechos);
				u->next = NULL;
				u->prev = NULL;
				if(((iListElement*)m) == iShopItem)
					aciNextShopItem();
				//std::cout<<"aciChange2Raffa GGamerMechos = u "<<u<<std::endl;
				GGamerMechos = u;

				u1 = aciGetMechos(m -> type);
				if(u1)
					m -> uvsDataPtr = u1;
				else
					m -> uvsDataPtr = NULL;
				aScrDisp -> curMatrix = aScrDisp -> alloc_matrix(m -> type,1);
				aScrDisp -> curMatrix -> uvsDataPtr = u;
				return;
			} else {
				rf = m;
			}
		}
		m = (invMatrix*)m -> prev;
	}
	if(!rf)
		ErrH.Abort("Free mechos not found...");

	m = rf;
	u = new uvsActInt;
	u -> type = m -> type;
	u -> price = u -> sell_price = 1;

	if(((iListElement*)m) == iShopItem)
		aciNextShopItem();

	//std::cout<<"aciChange2Raffa2 GGamerMechos = u"<<std::endl;
	GGamerMechos = u;

	u1 = aciGetMechos(m -> type);
	if(u1)
		m -> uvsDataPtr = u1;
	else
		m -> uvsDataPtr = NULL;

	aScrDisp -> curMatrix = aScrDisp -> alloc_matrix(m -> type,1);
	aScrDisp -> curMatrix -> uvsDataPtr = u;
}

char* aciGetPlayerName(void)
{
	return iGetOptionValueCHR(iPLAYER_NAME2);
}

char* aciGetPlayerPassword(void)
{
	return iGetOptionValueCHR(iPLAYER_PASSWORD);
}

int aciGetPlayerColor(void)
{
	return iGetOptionValue(iPLAYER_COLOR);
}

void aciClearTeleportMenu(void)
{
	fncMenu* mn;
	fncMenuItem* p,*p1;

	mn = aScrDisp -> get_menu(FMENU_TELEPORT_MENU);
	if(!mn) return;

	p = (fncMenuItem*)mn -> items -> last;
	while(p){
		p1 = (fncMenuItem*)p -> prev;
		if(p -> fnc_code != FMENU_OFF){
			mn -> items -> dconnect((iListElement*)p);
			aScrDisp -> free_menu_item(p);
		}
		p = p1;
	}
	p = (fncMenuItem*)mn -> inactive_items -> last;
	while(p){
		p1 = (fncMenuItem*)p -> prev;
		if(p -> fnc_code != FMENU_OFF){
			mn -> inactive_items -> dconnect((iListElement*)p);
			aScrDisp -> free_menu_item(p);
		}
		p = p1;
	}
	mn -> prefix = NULL;
	mn -> postfix = NULL;
}

void aciSaveUVSList(XStream& fh,uvsActInt* p)
{
	uvsActInt* ptr;
	int i,list_size = 0;

	ptr = p;
	while(ptr){
		list_size ++;
		ptr = (uvsActInt*)ptr -> next;
	}

	ptr = p;
	fh < list_size;
	for(i = 0; i < list_size; i ++){
		ptr -> save(fh);
		ptr = (uvsActInt*)ptr -> next;
	}
}

void aciFreeUVSList(uvsActInt* p)
{
	uvsActInt* ptr,*ptr1;
	ptr = p;
	while(ptr){
		ptr1 = (uvsActInt*)ptr -> next;
		//std::cout<<"aciFreeUVSList "<<ptr<<" type:"<<ptr->type<<std::endl;
		delete ptr;
		ptr = ptr1;
	}
}

void aciLoadUVSList(XStream& fh,uvsActInt** p,int list_type)
{
	int i,list_size = 0;
	uvsActInt* ptr;
	listElem* el = (listElem*)(*p);

	fh > list_size;
	for(i = 0; i < list_size; i ++){
		ptr = new uvsActInt;
		ptr -> load(fh,list_type);
		//std::cout<<"aciLoadUVSList new "<<ptr<<" type:"<<ptr->type<<" price:"<<ptr->price<<" param1:"<<ptr->param1<<" param2:"<<ptr->param2<<std::endl;
		ptr -> link(el);
	}
	*p = (uvsActInt*)el;
}

#define ACI_FRAG_TIMER		300
#define ACI_FRAG_FONT		0

const unsigned ACI_FRAG_COL0	 = (252 | (246 << 8));
const unsigned ACI_FRAG_COL1	 = (143 | (135 << 8));
const unsigned ACI_FRAG_COL2	 = (156 | (151 << 8));
const unsigned ACI_FRAG_COL3	 = (173 | (166 << 8));
const unsigned ACI_FRAG_COL4	 = (200 | (195 << 8));
const unsigned ACI_FRAG_COL5	 = (238 | (233 << 8));
const unsigned ACI_FRAG_COL6	 = (238 | (233 << 8));
const unsigned ACI_FRAG_COL7	 = (238 | (233 << 8));
const unsigned ACI_FRAG_COL8	 = (252 | (246 << 8));
const unsigned ACI_FRAG_COL9	 = (156 | (151 << 8));

void aciShowFrags(void)
{
	int i,num;
	XBuffer XBuf;
	PlayerData* p;
	char* world_name;

	static int fragColors[10] = { ACI_FRAG_COL0, ACI_FRAG_COL1, ACI_FRAG_COL2, ACI_FRAG_COL3, ACI_FRAG_COL4, ACI_FRAG_COL5, ACI_FRAG_COL6, ACI_FRAG_COL7, ACI_FRAG_COL8, ACI_FRAG_COL9 };

	if(!NetworkON || !players_list.size()) return;
	if(aScrDisp -> curPrompt -> NumStr){
		aScrDisp -> curPrompt -> free_mem();
		return;
	}

	iSortPlayers(1);

	num = iNumPlayers;
	aScrDisp -> curPrompt -> alloc_mem(num);
	aScrDisp -> curPrompt -> CurTimer = 0;

	aScrDisp -> curPrompt -> fontID = ACI_FRAG_FONT;

	switch(iCurMultiGame){
		case 0: // VAN-WAR...
			for(i = 0; i < num; i ++){
				p = iPlayers[i];
				XBuf.init();
				world_name = aScrDisp -> wMap -> world_ptr[aScrDisp -> wMap -> world_ids[p -> body.world]] -> name;
				XBuf < p -> name < " (" < world_name < ") : " <= p -> body.kills < " " < aciSTR_KILLS < " " <= p -> body.deaths < " " < aciSTR_DEATHS;
				aScrDisp -> curPrompt -> add_str(i,(unsigned char*)XBuf.address());
				aScrDisp -> curPrompt -> TimeBuf[i] = ACI_FRAG_TIMER;
				aScrDisp -> curPrompt -> ColBuf[i] = fragColors[p -> body.color];
			}
			break;
		case 1: // MECHOSOMA...
			for(i = 0; i < num; i ++){
				p = iPlayers[i];
				XBuf.init();
				world_name = aScrDisp -> wMap -> world_ptr[aScrDisp -> wMap -> world_ids[p -> body.world]] -> name;
				XBuf < p -> name < " (" < world_name < ") : " < aciSTR_Ware1 < " " <= p -> body.MechosomaStat.ItemCount1 < "/" <= my_server_data.Mechosoma.ProductQuantity1;
				XBuf < " " < aciSTR_Ware2 < " " <= p -> body.MechosomaStat.ItemCount2 < "/" <= my_server_data.Mechosoma.ProductQuantity2;
				aScrDisp -> curPrompt -> add_str(i,(unsigned char*)XBuf.address());
				aScrDisp -> curPrompt -> TimeBuf[i] = ACI_FRAG_TIMER;
				aScrDisp -> curPrompt -> ColBuf[i] = fragColors[p -> body.color];
			}
			break;
		case 2: // PASSEMBLOSS...
			for(i = 0; i < num; i ++){
				p = iPlayers[i];
				XBuf.init();
				world_name = aScrDisp -> wMap -> world_ptr[aScrDisp -> wMap -> world_ids[p -> body.world]] -> name;
				XBuf < p -> name < " (" < world_name < ") : " < aciSTR_Checkpoints < " " <= p -> body.PassemblossStat.CheckpointLighting < "/" <= my_server_data.Passembloss.CheckpointsNumber;
				aScrDisp -> curPrompt -> add_str(i,(unsigned char*)XBuf.address());
				aScrDisp -> curPrompt -> TimeBuf[i] = ACI_FRAG_TIMER;
				aScrDisp -> curPrompt -> ColBuf[i] = fragColors[p -> body.color];
			}
			break;
	}
}

#ifdef _DEBUG
void scale_bmp(int sx,int sy,int sx0,int sy0,void* dest,void* src)
{
	int x,y;
	unsigned char* dest_buf,*src_buf;

	double ix,iy,dx,dy;

	dest_buf = (unsigned char*)dest;
	src_buf = (unsigned char*)src;

	dx = (double)sx0/(double)sx;
	dy = (double)sy0/(double)sy;
	for(y = 0; y < sy; y ++){
		for(x = 0; x < sx; x ++){
			iy = y * dy;
			ix = x * dx;
			dest_buf[x + y * sx] = src_buf[round(ix) + (round(iy) * sx0)];
		}
	}
}

void aciResizeItem(double delta)
{
	int x,y;
	invItem* p,*p0;
	unsigned char* buf;

	double dx,dy;

	p0 = NULL;

	if(!(aScrDisp -> flags & AS_INV_MOVE_ITEM)) return;
	p = aScrDisp -> curItem;

	x = p -> ScreenSizeX;
	y = p -> ScreenSizeY;

	dy = (double)y * delta;
	dx = ((double)x * dy) / (double)y;
	x = round(dx);
	y = round(dy);

	if(x < 0 || y < 0) return;
	buf = new unsigned char[x * y];
	scale_bmp(x,y,p -> ScreenSizeX,p -> ScreenSizeY,buf,p -> frame);
	delete p -> frame;
	p -> frame = (char*)buf;
	p -> ScreenSizeX = x;
	p -> ScreenSizeY = y;
	set_mouse_cursor((char*)buf,x,y);

	if(aScrDisp -> flags & AS_ISCREEN)
		p0 = aScrDisp -> get_iitem(p -> ID);
	else
		p0 = aScrDisp -> get_item(p -> ID);

	if(p0){
		p0 -> ScreenSizeX = x;
		p0 -> ScreenSizeY = y;
		p0 -> frame = (char*)buf;
	}
}
#endif

void aciFreeUVS(void)
{
	//std::cout<<"aciFreeUVSList(GItem)"<<std::endl;
	if(GItem) aciFreeUVSList(GItem);
	//std::cout<<"aciFreeUVSList(GMechos)"<<std::endl;
	if(GMechos) aciFreeUVSList(GMechos);
	//std::cout<<"aciFreeUVSList(GGamer)"<<std::endl;
	if(GGamer) aciFreeUVSList(GGamer);
	//std::cout<<"aciFreeUVSList(GGamerMechos)"<<std::endl;
	if(GGamerMechos) aciFreeUVSList(GGamerMechos);
	//std::cout<<"aciFreeUVSList(GCharItem)"<<std::endl;
	if(GCharItem) aciFreeUVSList(GCharItem);
	//std::cout<<"aciFreeUVSList(GTreasureItem)"<<std::endl;
	if(GTreasureItem) aciFreeUVSList(GTreasureItem);

	GItem = NULL;
	GMechos = NULL;
	GGamer = NULL;
	GGamerMechos = NULL;
	GCharItem = NULL;
	GTreasureItem = NULL;
}

void aciExitShop(void)
{
	iScrDisp -> ret_val = 1;
	iScrDisp -> flags |= SD_EXIT;
}

void aciRedrawParamsPanel(int x,int y,int sx,int sy)
{
	int col0,col1;
	InfoPanel* pl = aScrDisp -> get_aci_info_panel(ACI_PARAMS_PANEL);
	if(!pl){
		XGR_Rectangle(x,y,sx,sy,0,0,XGR_FILLED);
		return;
	}

	col0 = (aciCurColorScheme[FM_SELECT_START2] << 8) | aciCurColorScheme[FM_UNSELECT_START];
	col1 = (aciCurColorScheme[FM_SELECT_SIZE2] << 8) | aciCurColorScheme[FM_UNSELECT_SIZE];

	if(aciCurLuck != aciPrevLuck || aciCurDominance != aciPrevDominance){
		pl -> free_list();

		pl -> add_item(aciSTR_LUCK);
		aciXConv -> init();
		*aciXConv <= aciCurLuck;
		if(aciCurLuck < 100)
			pl -> add_item(aciXConv -> address());
		else
			pl -> add_item(aciXConv -> address(),-1,col0 | (col1 << 16));

		pl -> add_item(" ");

		pl -> add_item(aciSTR_DOMINANCE);
		aciXConv -> init();
		*aciXConv <= aciCurDominance;
		if(aciCurDominance < 100)
			pl -> add_item(aciXConv -> address());
		else
			pl -> add_item(aciXConv -> address(),-1,col0 | (col1 << 16));
	}
	pl -> PosX = x;
	pl -> PosY = y;

	pl -> SizeX = sx;
	pl -> SizeY = sy;

	pl -> flags |= IP_NO_REDRAW;

	pl -> redraw();
}

void aciPutTreasureItem(void)
{
	uvsActInt* p;
	if(!GTreasureItem) return;
	p = GTreasureItem;
	while(p){
		aciML_D -> curDataSet -> put_item(p -> type);
		p = (uvsActInt*)p -> next;
	}
}

void aciStealItem(int type)
{
	listElem* el;
	uvsActInt* u;
	invItem* p,*p1;
	if(!GTreasureItem) return;
	u = GTreasureItem;
	while(u){
		if(u -> type == type) break;
		u = (uvsActInt*)u -> next;
	}
	if(!u) return;

	if(u -> type == ACI_BOORAWCHIK) aciEscaveEmpty = 1;

	p = aScrDisp -> get_iitem(u -> type);
	p1 = aScrDisp -> alloc_item();
	p -> clone(p1);

	if(aScrDisp -> flags & AS_INV_MOVE_ITEM){
		aScrDisp -> flags ^= AS_INV_MOVE_ITEM;
		aciSell_Item(aScrDisp -> curItem);
		aScrDisp -> curItem = NULL;
	}

	aScrDisp -> flags |= AS_INV_MOVE_ITEM;
	aScrDisp -> curItem = p1;
	set_mouse_cursor(p1 -> frame,p1 -> ScreenSizeX,p1 -> ScreenSizeY);

	u -> delink(GTreasureItem);

	el = (listElem*)GGamer;
	u -> link(el);
	GGamer = (uvsActInt*)el;
	p1 -> uvsDataPtr = u;
}

void aciInitPricePanel(InfoPanel* ip,iListElement* p,int mode,int sell_mode)
{
	invItem* it;
	invMatrix* m;
	uvsActInt* u;

	int col = -1;

	ip -> free_list();

	if(mode == MECHOS_MODE){
		m = (invMatrix*)p;
		u = (uvsActInt*)m -> uvsDataPtr;
		if(m -> mech_name) ip -> add_item(m -> mech_name,-1,aciCurColorScheme[FM_SELECT_COL]);

		if(m -> flags & IM_NOT_COMPLETE)
			ip -> add_item(aciSTR_BROKEN);
	}
	else {
		it = (invItem*)p;
		u = (uvsActInt*)it -> uvsDataPtr;
		if(it->ID_ptr.size()) {
			ip -> add_item(it->ID_ptr.c_str(),-1,aciCurColorScheme[FM_SELECT_COL]);
		}
	}

	if(!u)
		return;

	aciXConv -> init();
	if(!uvsCurrentWorldUnable){
		if(mode != MECHOS_MODE || !(m -> flags & IM_NOT_COMPLETE)){
			if(sell_mode)
				*aciXConv < aciSTR_PRICE <= u -> sell_price < " $";
			else
				*aciXConv < aciSTR_PRICE <= u -> price < " $";

			ip -> add_item(aciXConv -> address(),-1,col);
		}
	}

	if(mode == MECHOS_MODE){
		m = (invMatrix*)p;
		if(!sell_mode && m -> pData){
			aciXConv -> init();
			*aciXConv < aciSTR_ENERGY_SHIELD < " " < (m -> pData + ACI_MECHOS_ENERGY_SHIELD * ACI_MAX_PRM_LEN);
			ip -> add_item(aciXConv -> address(),-1,col);

			aciXConv -> init();
			*aciXConv < aciSTR_RESTORING_SPEED < " " < (m -> pData + ACI_MECHOS_RESTORING_SPEED * ACI_MAX_PRM_LEN);
			ip -> add_item(aciXConv -> address(),-1,col);

			aciXConv -> init();
			*aciXConv < aciSTR_MECHANIC_ARMOR < " " < (m -> pData + ACI_MECHOS_MECHANIC_ARMOR * ACI_MAX_PRM_LEN) < " " < aciSTR_VELOCITY < " " < (m -> pData + ACI_MECHOS_VELOCITY * ACI_MAX_PRM_LEN);
			ip -> add_item(aciXConv -> address(),-1,col);

			aciXConv -> init();
			*aciXConv < aciSTR_SPIRAL_CAPACITY < " " < (m -> pData + ACI_MECHOS_SPIRAL_CAPACITY * ACI_MAX_PRM_LEN);
			ip -> add_item(aciXConv -> address(),-1,col);

			aciXConv -> init();
			*aciXConv < aciSTR_AIR_RESERVE < " " < (m -> pData + ACI_MECHOS_AIR_RESERVE * ACI_MAX_PRM_LEN);
			ip -> add_item(aciXConv -> address(),-1,col);
		}
	}
	else {
		it = (invItem*)p;
		if(it -> pData && !sell_mode){
			if(it -> slotType == AS_WEAPON_SLOT || it -> slotType == AS_TWEAPON_SLOT){
				aciXConv -> init();
				*aciXConv < aciSTR_DAMAGE < " " < (it -> pData + ACI_WEAPON_DAMAGE * ACI_MAX_PRM_LEN);
				ip -> add_item(aciXConv -> address(),-1,col);

				if(!uvsCurrentWorldUnable){
					aciXConv -> init();
					*aciXConv < aciSTR_LOAD < " " < (it -> pData + ACI_WEAPON_LOAD * ACI_MAX_PRM_LEN);
					ip -> add_item(aciXConv -> address(),-1,col);
				}

				aciXConv -> init();
				*aciXConv < aciSTR_SHOTS < " " < (it -> pData + ACI_WEAPON_SHOTS_SEC * ACI_MAX_PRM_LEN);
				ip -> add_item(aciXConv -> address(),-1,col);

				aciXConv -> init();
				*aciXConv < aciSTR_BURST < " " < (it -> pData + ACI_WEAPON_RANGE * ACI_MAX_PRM_LEN);
				ip -> add_item(aciXConv -> address(),-1,col);
			}
			else {
				if(it -> slotType == AS_DEVICE_SLOT){
					if(!uvsCurrentWorldUnable){
						aciXConv -> init();
						*aciXConv < aciSTR_WORKING_TIME < " " < (it -> pData + ACI_DEVICE_WORKING_TIME * ACI_MAX_PRM_LEN) < aciSTR_SECONDS;
						ip -> add_item(aciXConv -> address(),-1,col);
					}
				}
				else {
					aciXConv -> init();
					*aciXConv < aciSTR_DAMAGE < " " < (it -> pData + ACI_AMMO_DAMAGE * ACI_MAX_PRM_LEN);
					ip -> add_item(aciXConv -> address(),-1,col);

					aciXConv -> init();
					*aciXConv < aciSTR_IN_PACK < " " < (it -> pData + ACI_AMMO_IN_PACK * ACI_MAX_PRM_LEN);
					ip -> add_item(aciXConv -> address(),-1,col);
				}
			}
		}
	}
}

#define ACI_RACING_X	70
#define ACI_RACING_Y	70

#define ACI_RACING2_X	70
#define ACI_RACING2_Y	70

#define ACI_RACING_X_H	80
#define ACI_RACING_Y_H	80

#define ACI_RACING2_X_H 80
#define ACI_RACING2_Y_H 80

#define ACI_RACING_FONT 7
#define ACI_RACING_COL	103
#define ACI_RACING_COL2 111

void aciShowRacingPlace(void)
{
	int x,y,dx;
	aciXConv -> init();
	*aciXConv < aciCurRaceType < " ";

	if(!(aScrDisp -> flags & AS_FULLSCR)){
		if(actintLowResFlag){
			x = aScrDisp -> curIbs -> PosX + ACI_RACING_X;
			y = aScrDisp -> curIbs -> PosY + ACI_RACING_Y;
		}
		else {
			x = aScrDisp -> curIbs -> PosX + ACI_RACING_X_H;
			y = aScrDisp -> curIbs -> PosY + ACI_RACING_Y_H;
		}

		y = XGR_MAXY - y;
	}
	else {
		if(actintLowResFlag){
			x = ACI_RACING2_X;
			y = ACI_RACING2_Y;
		}
		else {
			x = ACI_RACING2_X_H;
			y = ACI_RACING2_Y_H;
		}

		y = XGR_MAXY - y;
	}

	aOutStr(x,y,ACI_RACING_FONT,ACI_RACING_COL,(unsigned char*)aciXConv -> address(),1);

	dx = aStrLen((unsigned char*)aciXConv -> address(),ACI_RACING_FONT,1);
	aciXConv -> init();
	*aciXConv < aciCurRaceInfo;
	aOutStr(x + dx,y,ACI_RACING_FONT,ACI_RACING_COL2,(unsigned char*)aciXConv -> address(),1);
}

void aciPrepareText(char* ptr)
{
	int sz = strlen(ptr);
	aScrDisp -> ScrTextData -> alloc(ptr,sz);
	aScrDisp -> send_event(ACI_SHOW_TEXT);
}

void aciAutoSave(void)
{
	if(NetworkON || aScrDisp -> curLocData -> WorldID == XPLO_ID) return;

	aciAutoSaveFlag = 1;
	acsSaveData();
}

#ifdef _DEMO_
void aciTheEnd(void)
{
	GameQuantReturnValue = RTO_LOADING3_ID;
	aciCompleteGameFlag = 1;
}
#endif

void aciDropMoveItem(void)
{
	if(aScrDisp -> flags & AS_INV_MOVE_ITEM){
		aciSendEvent2itmdsp(ACI_DROP_ITEM,aScrDisp -> curItem -> item_ptr);
		aScrDisp -> flags &= ~AS_INV_MOVE_ITEM;
		aScrDisp -> free_item(aScrDisp -> curItem);
	}
}

//znfo key handler
void aciCHandler(int key)
{
	return;
	int code = 0,cr;
	if(NetworkON || key > 256 || key <= 0) return;

	if(aciTreeData) {
		code = aciTreeData -> quant(key);
	}
	switch(code << NetworkON){
		case ACI_ADD_BEEBOS:
			cr = aciGetCurCredits();
			cr += 50000;
			aciUpdateCurCredits(cr);
			if(!(aScrDisp -> flags & AS_ISCREEN))
				SetWorldBeebos(cr);
			aciTreeData -> reset();
			SOUND_DROP();
			break;
		case ACI_ADD_LUCK:
			uvsChangeLuck();
			aciTreeData -> reset();
			SOUND_DROP();
			break;
		case ACI_ADD_DOMINANCE:
			uvsChangeDominance();
			aciTreeData -> reset();
			SOUND_DROP();
			break;
		case ACI_ADD_KEYS:
			aciOpenWorldLink(1,0);
			aciOpenWorldLink(1,3);
			aciOpenWorldLink(1,2);
			aciOpenWorldLink(1,4);

			aciOpenWorldLink(0,6);

			aciOpenWorldLink(3,8);

			aciOpenWorldLink(2,9);
			aciOpenWorldLink(2,5);

			aciOpenWorldLink(7,7);
			aciPrepareWorldsMenu();
			aciTreeData -> reset();
			SOUND_DROP();
			break;
		case ACI_COMPLETE_TABUTASK:
			aciCompleteTabutask();
			aciTreeData -> reset();
			break;
		case ACI_ADD_TABUTASK01:
			aciAddTabutask(1);
			aciTreeData -> reset();
			break;
		case ACI_ADD_TABUTASK02:
			aciAddTabutask(2);
			aciTreeData -> reset();
			break;
		case ACI_ADD_TABUTASK03:
			aciAddTabutask(3);
			aciTreeData -> reset();
			break;
		case ACI_ADD_TABUTASK04:
			aciAddTabutask(4);
			aciTreeData -> reset();
			break;
		case ACI_ADD_TABUTASK05:
			aciAddTabutask(5);
			aciTreeData -> reset();
			break;
		case ACI_ADD_TABUTASK06:
			aciAddTabutask(6);
			aciTreeData -> reset();
			break;
		case ACI_ADD_TABUTASK07:
			aciAddTabutask(7);
			aciTreeData -> reset();
			break;
		case ACI_ADD_TABUTASK08:
			aciAddTabutask(8);
			aciTreeData -> reset();
			break;
		case ACI_ADD_TABUTASK09:
			aciAddTabutask(9);
			aciTreeData -> reset();
			break;
		case ACI_ADD_TABUTASK10:
			aciAddTabutask(10);
			aciTreeData -> reset();
			break;
		case ACI_ADD_TABUTASK11:
			aciAddTabutask(11);
			aciTreeData -> reset();
			break;
		case ACI_ADD_TABUTASK12:
			aciAddTabutask(12);
			aciTreeData -> reset();
			break;
		case ACI_ADD_TABUTASK13:
			aciAddTabutask(13);
			aciTreeData -> reset();
			break;
		case ACI_ADD_TABUTASK14:
			aciAddTabutask(14);
			aciTreeData -> reset();
			break;
		case ACI_ADD_TABUTASK15:
			aciAddTabutask(15);
			aciTreeData -> reset();
			break;
		case ACI_ADD_TABUTASK16:
			aciAddTabutask(16);
			aciTreeData -> reset();
			break;
		case ACI_ADD_TABUTASK17:
			aciAddTabutask(17);
			aciTreeData -> reset();
			break;
		case ACI_ADD_TABUTASK18:
			aciAddTabutask(18);
			aciTreeData -> reset();
			break;
		case ACI_ADD_TABUTASK19:
			aciAddTabutask(19);
			aciTreeData -> reset();
			break;
		case ACI_ADD_TABUTASK20:
			aciAddTabutask(20);
			aciTreeData -> reset();
			break;
		case ACI_TELEPORT_G:
			if(!actIntLog){
				SOUND_DROP();
				aci_SendEvent(EV_TELEPORT,0);
			}
			break;
		case ACI_TELEPORT_F:
			if(!actIntLog){
				SOUND_DROP();
				aci_SendEvent(EV_TELEPORT,1);
			}
			break;
		case ACI_TELEPORT_N:
			if(!actIntLog){
				SOUND_DROP();
				aci_SendEvent(EV_TELEPORT,2);
			}
			break;
		case ACI_TELEPORT_X:
			if(!actIntLog){
				SOUND_DROP();
				aci_SendEvent(EV_TELEPORT,3);
			}
			break;
		case ACI_TELEPORT_B:
			if(!actIntLog){
				SOUND_DROP();
				aci_SendEvent(EV_TELEPORT,4);
			}
			break;
		case ACI_TELEPORT_K:
			if(!actIntLog){
				SOUND_DROP();
				aci_SendEvent(EV_TELEPORT,6);
			}
			break;
		case ACI_TELEPORT_T:
			if(!actIntLog){
				SOUND_DROP();
				aci_SendEvent(EV_TELEPORT,7);
			}
			break;
		case ACI_TELEPORT_H:
			if(!actIntLog){
				SOUND_DROP();
				aci_SendEvent(EV_TELEPORT,8);
			}
			break;
		case ACI_TELEPORT_W:
			if(!actIntLog){
				SOUND_DROP();
				aci_SendEvent(EV_TELEPORT,11);
			}
			break;
		case ACI_TELEPORT_A:
			if(!actIntLog){
				SOUND_DROP();
				aci_SendEvent(EV_TELEPORT,12);
			}
			break;
		case ACI_END_GAME0:
			if(!actIntLog){
				SOUND_DROP();
				XpeditionOFF(GAME_OVER_INFERNAL_LOCKED);
			}
			break;
		case ACI_END_GAME1:
			if(!actIntLog){
				SOUND_DROP();
				XpeditionOFF(GAME_OVER_SPOBS_LOCKED);
			}
			break;
		case ACI_END_GAME2:
			if(!actIntLog){
				SOUND_DROP();
				XpeditionOFF(GAME_OVER_ALL_LOCKED);
			}
			break;
		case ACI_END_GAME3:
			if(!actIntLog){
				SOUND_DROP();
				XpeditionOFF(GAME_OVER_LUCKY);
			}
			break;
		case ACI_CHANGE_CYCLE:
			if(!actIntLog){
				SOUND_DROP();
				uvsChangeCycle();
			}
			break;
		case ACI_REPAIR:
			if(!actIntLog){
				SOUND_DROP();
				FakeOfMight();
			}
			break;
	}
}

void aciPutBufSpr(int x,int y,int sx,int sy,int bsx,void* src,void* dest,int col,int col_sz)
{
	int i,j,dest_index,src_index;
	unsigned src_col;
	unsigned char* src_buf,*dest_buf;

	dest_index = x + y * bsx;
	src_index = 0;

	src_buf = (unsigned char*)src;
	dest_buf = (unsigned char*)dest;

	if(!col_sz){
		for(i = 0; i < sy; i ++){
			for(j = 0; j < sx; j ++){
				src_col = src_buf[src_index + j];
				if(src_col)
					dest_buf[dest_index + j] = col + src_col;
			}
			dest_index += bsx;
			src_index += sx;
		}
	}
	else {
		for(i = 0; i < sy; i ++){
			for(j = 0; j < sx; j ++){
				src_col = src_buf[src_index + j];
				if(src_col)
					dest_buf[dest_index + j] = col + (src_col >> col_sz);
			}
			dest_index += bsx;
			src_index += sx;
		}
	}
}

void aPutChar32(int x,int y,int font,int color,int color_size,int str,int bsx,void* buf)
{
	aciXConv -> init();
	*aciXConv < (char)str < "\0";

	aPutStr32(x,y,font,color,color_size,aciXConv -> address(),bsx,buf,0);
}

void aOutText32(int x,int y,int color,void* text,int font,int hspace,int vspace)
{
	int s,xs,ys,offs = 0,sz = strlen((char*)text),X = 0,Y = 0;
	int color_size,chr;

	unsigned char* str_buf;

	if(aScrFonts32[font] == NULL)
		ErrH.Abort("MISSING FONT...");

	color_size = (color >> 16) & 0xFF;
	color &= 0xFFFF;

	unsigned char* p = aScrFonts32[font] -> data;
	xs = aScrFonts32[font] -> SizeX;
	ys = aScrFonts32[font] -> SizeY;

	str_buf = (unsigned char*)text;

	X = x;
	Y = y;

	for(s = 0; s < sz; s ++){
		chr = str_buf[s];
		if(chr != '\n'){
			if(chr != '\r'){
				X -= aScrFonts32[font] -> LeftOffs[chr];
				offs = xs * ys * chr;
				aciPutSpr32(X,Y,xs,ys,p + offs,color,color_size);
				X += xs + hspace - aScrFonts32[font] -> RightOffs[chr];
			}
		}
		else {
			X = x;
			Y += ys + vspace;
		}
	}
}

void aOutText32clip(int x,int y,int color,void* text,int font,int hspace,int vspace)
{
	int s,xs,ys,offs = 0,sz = strlen((char*)text),X = 0,Y = 0;
	int color_size,chr;

	unsigned char* str_buf;

	if(aScrFonts32[font] == NULL)
		ErrH.Abort("MISSING FONT...");

	color_size = (color >> 16) & 0xFF;
	color &= 0xFFFF;

	unsigned char* p = aScrFonts32[font] -> data;
	xs = aScrFonts32[font] -> SizeX;
	ys = aScrFonts32[font] -> SizeY;

	str_buf = (unsigned char*)text;

	X = x;
	Y = y;

	for(s = 0; s < sz; s ++){
		chr = str_buf[s];
		if(chr != '\n'){
			if(chr != '\r'){
				X -= aScrFonts32[font] -> LeftOffs[chr];
				offs = xs * ys * chr;
				aciPutSpr32clip(X,Y,xs,ys,p + offs,color,color_size);
				X += xs + hspace - aScrFonts32[font] -> RightOffs[chr];
			}
		}
		else {
			X = x;
			Y += ys + vspace;
		}
	}
}

int aTextWidth32(void* text,int font,int hspace)
{
	int s,xs,ys,sz = strlen((char*)text),chr;
	int len = 0,max_len = 0;

	unsigned char* str_buf;

	if(aScrFonts32[font] == NULL)
		ErrH.Abort("MISSING FONT...");

	xs = aScrFonts32[font] -> SizeX;
	ys = aScrFonts32[font] -> SizeY;

	str_buf = (unsigned char*)text;

	for(s = 0; s < sz; s ++){
		chr = str_buf[s];
		if(chr != '\n'){
			if(chr != '\r'){
				len += xs + hspace - aScrFonts32[font] -> RightOffs[chr] - aScrFonts32[font] -> LeftOffs[chr];
			}
		}
		else {
			if(len > max_len) max_len = len;
			len = 0;
		}
	}
	if(len > max_len) max_len = len;
	return max_len;
}

int aTextHeight32(void* text,int font,int vspace)
{
	int s,xs,ys,sz = strlen((char*)text),chr;
	int len = 0;

	unsigned char* str_buf;

	if(aScrFonts32[font] == NULL)
		ErrH.Abort("MISSING FONT...");

	xs = aScrFonts32[font] -> SizeX;
	ys = aScrFonts32[font] -> SizeY;

	str_buf = (unsigned char*)text;

	len = ys + vspace;

	for(s = 0; s < sz; s ++){
		chr = str_buf[s];
		if(chr == '\n'){
			len += ys + vspace;
		}
	}
	return len;
}

void aciPutSpr32(int x,int y,int sx,int sy,void* buf,int col,int col_sz)
{
	int i,j,index = 0;
	unsigned cl;
	unsigned char* p = (unsigned char*)buf;

	if(col_sz){
		for(i = 0; i < sy; i ++){
			for(j = 0; j < sx; j ++){
				cl = p[index];
				cl >>= col_sz;
				if(cl)
					XGR_SetPixelFast(x + j,y + i,col + cl);
				index ++;
			}
		}
	}
	else {
		for(i = 0; i < sy; i ++){
			for(j = 0; j < sx; j ++){
				cl = p[index];
				if(cl)
					XGR_SetPixelFast(x + j,y + i,col + cl);
				index ++;
			}
		}
	}
}

void aciPutSpr32clip(int x,int y,int sx,int sy,void* buf,int col,int col_sz)
{
	int i,j,index = 0;
	unsigned cl;
	unsigned char* p = (unsigned char*)buf;

	if(col_sz){
		for(i = 0; i < sy; i ++){
			for(j = 0; j < sx; j ++){
				cl = p[index];
				cl >>= col_sz;
				if(cl)
					XGR_SetPixel(x + j,y + i,col + cl);
				index ++;
			}
		}
	}
	else {
		for(i = 0; i < sy; i ++){
			for(j = 0; j < sx; j ++){
				cl = p[index];
				if(cl)
					XGR_SetPixel(x + j,y + i,col + cl);
				index ++;
			}
		}
	}
}

void acsCompressData(char* p)
{
	int len,time_len,flags,sz,data;
	unsigned long dest_sz, src_sz;
	XStream fh;

	char* header,*src_buf,*dest_buf;

	fh.open(p,XS_IN);
	sz = fh.size();
	fh > len;

	time_len = (len >> 16) & 0xFF;
	flags = (len >> 24) & 0xFF;
	len &= 0xFF;

	if(flags & ACI_SAVE_COMPRESSED){
		fh.close();
		return;
	}

	dest_sz = src_sz = sz - time_len - len - sizeof(int);
	dest_sz*=2;
	header = new char[len + time_len];
	src_buf = new char[src_sz];
	dest_buf = new char[dest_sz];

	fh.read(header,len + time_len);
	fh.read(src_buf,src_sz);
	fh.close();

	//std::cout<<"compress src_sz:"<<src_sz<<std::endl;
	//dest_sz = ZIP_compress(dest_buf,src_sz,src_buf,src_sz);
	/* ZLIB realization (stalkerg)*/
	dest_buf[0] = (char)(8 & 0xFF); //8 it is DEFLATE method
	dest_buf[1] = (char)((8 >> 8) & 0xFF);
	*(unsigned int*)(dest_buf + 2) = (unsigned int)src_sz;
	dest_sz-=2+4;
	int stat = compress((Bytef*)(dest_buf+2+4),(uLongf*)&(dest_sz),(Bytef*)src_buf,src_sz);
	dest_sz+=2+4;
	switch(stat){
		//case Z_OK: std::cout<<"Compress ok."<<std::endl; break;
		case Z_MEM_ERROR: std::cout<<"not enough memory."<<std::endl; break;
		case Z_BUF_ERROR: std::cout<<"not enough room in the output buffer."<<std::endl; break;
	};
	//std::cout<<"compress dest_sz:"<<dest_sz<<std::endl;
	flags |= ACI_SAVE_COMPRESSED;
	data = len | ((time_len | (flags << 8)) << 16);

	fh.open(p,XS_OUT);
	fh < data;

	fh.write(header,len + time_len);
	fh.write(dest_buf,dest_sz);
	fh.close();

	delete[] header;
	delete[] src_buf;
	delete[] dest_buf;
}

void acsDecompressData(char* p)
{
	int len,time_len,flags,sz,data;
	XStream fh;
	unsigned long dest_sz, src_sz;
	char* header,*src_buf,*dest_buf;

	fh.open(p,XS_IN);
	sz = fh.size();
	fh > len;

	time_len = (len >> 16) & 0xFF;
	flags = (len >> 24) & 0xFF;
	len &= 0xFF;

	if(!(flags & ACI_SAVE_COMPRESSED)){
		fh.close();
		return;
	}

	src_sz = sz - time_len - len - sizeof(int);
	header = new char[len + time_len];
	src_buf = new char[src_sz];

	fh.read(header,len + time_len);
	fh.read(src_buf,src_sz);
	fh.close();

	//dest_sz = ZIP_GetExpandedSize(src_buf);
	dest_sz = *(unsigned int*)(src_buf + 2) + 12; //+12 for size need for zlib
	dest_buf = new char[dest_sz];

	//ZIP_expand(dest_buf,dest_sz,src_buf,src_sz);
	/* ZLIB realisation (stalkerg)*/
	if(*(short*)(src_buf)) { //if label = 0 not compress
		//std::cout<<"acsDecompressData DeCompress "<<p<<" file."<<std::endl;
		int stat = uncompress((Bytef*)dest_buf,(uLongf*)&dest_sz,(Bytef*)(src_buf+2+4),src_sz-2-4);
		switch(stat){
			//case Z_OK: std::cout<<"DeCompress ok."<<std::endl; break;
			case Z_MEM_ERROR: std::cout<<"not enough memory."<<std::endl; break;
			case Z_BUF_ERROR: std::cout<<"not enough room in the output buffer."<<std::endl; break;
			case Z_DATA_ERROR: std::cout<<"error data."<<std::endl; break;
		};
	} else {
		memcpy(dest_buf,src_buf + 2 + 4,(unsigned)(src_sz - 2 - 4));
	}

	flags &= ~ACI_SAVE_COMPRESSED;
	data = len | ((time_len | (flags << 8)) << 16);

	fh.open(p,XS_OUT);
	fh < data;

	fh.write(header,len + time_len);
	fh.write(dest_buf,dest_sz);
	fh.close();

	delete[] header;
	delete[] src_buf;
	delete[] dest_buf;
}

void aciSetRedraw(void)
{
	aScrDisp -> flags &= ~AS_FULL_REDRAW;
	aciPrevJumpCount = -1;
}

void aciInitStrings(void)
{
	if(lang() != RUSSIAN){
		aciSTR_ON = aciSTR_ON1;
		aciSTR_OFF = aciSTR_OFF1;
		aciSTR_DAY = aciSTR_DAY1;
		aciSTR_UNDEFINED_PRICE = aciSTR_UNDEFINED_PRICE1;
		aciSTR_PRICE = aciSTR_PRICE1;
		aciSTR_EMPTY_SLOT = aciSTR_EMPTY_SLOT1;
		aciSTR_UNNAMED_SAVE = aciSTR_UNNAMED_SAVE1;
		aciSTR_AUTOSAVE = aciSTR_AUTOSAVE1;
		aciSTR_KILLS = aciSTR_KILLS1;
		aciSTR_DEATHS = aciSTR_DEATHS1;
		aciSTR_LUCK = aciSTR_LUCK1;
		aciSTR_DOMINANCE = aciSTR_DOMINANCE1;
		aciSTR_BROKEN = aciSTR_BROKEN1;
		aciSTR_ENERGY_SHIELD = aciSTR_ENERGY_SHIELD1;
		aciSTR_RESTORING_SPEED = aciSTR_RESTORING_SPEED1;
		aciSTR_MECHANIC_ARMOR = aciSTR_MECHANIC_ARMOR1;
		aciSTR_VELOCITY = aciSTR_VELOCITY1;
		aciSTR_SPIRAL_CAPACITY = aciSTR_SPIRAL_CAPACITY1;
		aciSTR_AIR_RESERVE = aciSTR_AIR_RESERVE1;
		aciSTR_DAMAGE = aciSTR_DAMAGE1;
		aciSTR_LOAD = aciSTR_LOAD1;
		aciSTR_SHOTS = aciSTR_SHOTS1;
		aciSTR_BURST = aciSTR_BURST1;
		aciSTR_WORKING_TIME = aciSTR_WORKING_TIME1;
		aciSTR_SECONDS = aciSTR_SECONDS1;
		aciSTR_IN_PACK = aciSTR_IN_PACK1;
		aciSTR_NO_CASH = aciSTR_NO_CASH1;
		aciSTR_PICKUP_ITEMS_OFF = aciSTR_PICKUP_ITEMS_OFF1;
		aciSTR_PICKUP_WEAPONS_OFF = aciSTR_PICKUP_WEAPONS_OFF1;
		aciSTR_PutThis = aciSTR_PutThis1;
		aciSTR_Ware1 = aciSTR_Ware11;
		aciSTR_Ware2 = aciSTR_Ware21;
		aciSTR_Checkpoints = aciSTR_Checkpoints1;
	}
	else {
		aciSTR_ON = aciSTR_ON2;
		aciSTR_OFF = aciSTR_OFF2;
		aciSTR_DAY = aciSTR_DAY2;
		aciSTR_UNDEFINED_PRICE = aciSTR_UNDEFINED_PRICE2;
		aciSTR_PRICE = aciSTR_PRICE2;
		aciSTR_EMPTY_SLOT = aciSTR_EMPTY_SLOT2;
		aciSTR_UNNAMED_SAVE = aciSTR_UNNAMED_SAVE2;
		aciSTR_AUTOSAVE = aciSTR_AUTOSAVE2;
		aciSTR_KILLS = aciSTR_KILLS2;
		aciSTR_DEATHS = aciSTR_DEATHS2;
		aciSTR_LUCK = aciSTR_LUCK2;
		aciSTR_DOMINANCE = aciSTR_DOMINANCE2;
		aciSTR_BROKEN = aciSTR_BROKEN2;
		aciSTR_ENERGY_SHIELD = aciSTR_ENERGY_SHIELD2;
		aciSTR_RESTORING_SPEED = aciSTR_RESTORING_SPEED2;
		aciSTR_MECHANIC_ARMOR = aciSTR_MECHANIC_ARMOR2;
		aciSTR_VELOCITY = aciSTR_VELOCITY2;
		aciSTR_SPIRAL_CAPACITY = aciSTR_SPIRAL_CAPACITY2;
		aciSTR_AIR_RESERVE = aciSTR_AIR_RESERVE2;
		aciSTR_DAMAGE = aciSTR_DAMAGE2;
		aciSTR_LOAD = aciSTR_LOAD2;
		aciSTR_SHOTS = aciSTR_SHOTS2;
		aciSTR_BURST = aciSTR_BURST2;
		aciSTR_WORKING_TIME = aciSTR_WORKING_TIME2;
		aciSTR_SECONDS = aciSTR_SECONDS2;
		aciSTR_IN_PACK = aciSTR_IN_PACK2;
		aciSTR_NO_CASH = aciSTR_NO_CASH2;
		aciSTR_PICKUP_ITEMS_OFF = aciSTR_PICKUP_ITEMS_OFF2;
		aciSTR_PICKUP_WEAPONS_OFF = aciSTR_PICKUP_WEAPONS_OFF2;
		aciSTR_PutThis = aciSTR_PutThis2;
		aciSTR_Ware1 = aciSTR_Ware12;
		aciSTR_Ware2 = aciSTR_Ware22;
		aciSTR_Checkpoints = aciSTR_Checkpoints2;
	}
}

char* aciGetItemEscave(int p2)
{
	int id;
	aciLocationInfo* locData;
	char* ptr = uvsGetNameByID(p2 & 0x0000FFFF,id);

	id = aScrDisp -> get_locdata_id(ptr);
	locData = aScrDisp -> get_locdata(id);
	if(locData)
		return locData -> nameID2;
	else
		return NULL;//NEED SEE!!! ""
}

void aciReadItmText(void)
{
	invItem* p;
	p = aciItmTextQueue[aciItmTextQueueCur];
	aciItmTextQueueCur ++;
	if(aciItmTextQueueCur >= aciItmTextQueueSize)
		aciItmTextQueueCur = aciItmTextQueueSize = 0;
	if(p && p -> EvCode)
		aScrDisp -> send_event(p -> EvCode,0,p -> item_ptr);
}

void aciInitItmTextQueue(void)
{
	int i;
	invItem* p;
	for(i = 0; i < ACI_ITEM_QUEUE_MAX; i ++){
		aciItmTextQueue[i] = NULL;
	}
	aciItmTextQueueSize = aciItmTextQueueCur = 0;

	p = (invItem*)aScrDisp -> curMatrix -> items -> last;
	while(p){
		if(aciItmTextQueueSize < ACI_ITEM_QUEUE_MAX && p -> ID == ACI_TABUTASK && (p -> flags & INV_ITEM_NEW)){
			aciItmTextQueue[aciItmTextQueueSize] = p;
			aciItmTextQueueSize ++;
		}
		p = (invItem*)p -> prev;
	}
	p = (invItem*)aScrDisp -> curMatrix -> items -> last;
	while(p){
		if(aciItmTextQueueSize < ACI_ITEM_QUEUE_MAX && (p -> ID == ACI_ELEEPOD || p -> ID == ACI_BEEBOORAT || p -> ID == ACI_ZEEX) && (p -> flags & INV_ITEM_NEW)){
			aciItmTextQueue[aciItmTextQueueSize] = p;
			aciItmTextQueueSize ++;
		}
		p = (invItem*)p -> prev;
	}
	p = (invItem*)aScrDisp -> curMatrix -> items -> last;
	while(p){
		p -> flags &= ~INV_ITEM_NEW;
		p = (invItem*)p -> prev;
	}
}

void aciAddTabutask(int num)
{
	invItem* p;
	uvsActInt* u;
	listElem* el = (listElem*)GItem;
	if(!(aScrDisp -> flags & AS_ISCREEN)) return;

	p = aScrDisp -> get_iitem(ACI_TABUTASK);
	if(p -> uvsDataPtr){
		u = (uvsActInt*)p -> uvsDataPtr;
	}
	else {
		u = new uvsActInt;
		u -> type = ACI_TABUTASK;
		p -> uvsDataPtr = u;

		u -> link(el);
		GItem = (uvsActInt*)el;
	}
	uvsTabuCreate(u,num);
	SOUND_DROP();
}

void aciCompleteTabutask(void)
{
	invItem* p;
	if(!aScrDisp -> curMatrix) return;
	p = (invItem*)aScrDisp -> curMatrix -> items -> last;
	while(p){
		if(p -> ID == ACI_TABUTASK){
			if(aScrDisp -> flags & AS_ISCREEN){
				if(p -> uvsDataPtr)
					uvsTabuComplete(((uvsActInt*)p -> uvsDataPtr) -> param2);
			}
			else {
				if(p -> item_ptr)
				uvsTabuComplete(p -> item_ptr -> data1);
			}
			SOUND_DROP();
			return;
		}
		p = (invItem*)p -> prev;
	}
}

void aciRemoveUvsItem(uvsActInt* ptr)
{
	switch(ptr -> type){
		case ACI_PROTRACTOR:
		case ACI_MECHANIC_MESSIAH:
		case ACI_FUNCTION83:
		case ACI_SPUMMY:
		case ACI_BOOT_SECTOR:
		case ACI_PEELOT:
		case ACI_LEEPURINGA:
		case ACI_SANDOLL_PART1:
		case ACI_SANDOLL_PART2:
		case ACI_LAST_MOGGY_PART1:
		case ACI_LAST_MOGGY_PART2:
		case ACI_QUEEN_FROG_PART1:
		case ACI_QUEEN_FROG_PART2:
		case ACI_LAWN_MOWER_PART1:
		case ACI_LAWN_MOWER_PART2:
		case ACI_WORMASTER_PART1:
		case ACI_WORMASTER_PART2:
		case ACI_PALOCHKA:
		case ACI_PIPKA:
		case ACI_NOBOOL:
		case ACI_BOORAWCHIK:
		case ACI_PEREPONKA:
		case ACI_ZEEFICK:
			return;
		default:
			ptr -> delink(GItem);
			delete ptr;
			break;
	}
}

void aciVMapPrepare(void)
{
	aScrDisp -> curLocData -> init_map_data();
}

void aciInitEndGame(int id)
{
	char* ptr;
	ShowImageRTO* p = (ShowImageRTO*)xtGetRuntimeObject(RTO_SHOW_IMAGE_ID);
	LoadingRTO3* lp = (LoadingRTO3*)xtGetRuntimeObject(RTO_LOADING3_ID);

//	int sz;
//	XStream fh;

	if(!aciImgText){
		aciImgText = new aciScreenText;
		aciImgText -> MaxStrLen = 55;
		aciImgText -> MaxPageStr = 19;
		aciImgText -> color = 239 | (1 << 16);
		aciImgText -> DeltaY = -2;
		aciImgText -> font = 3;
	}
	switch(GameOverID){
		case GAME_OVER_EXPLOSION:
			if(lang() == RUSSIAN)
				p -> SetName("img/img7.bmp");
			else
				p -> SetName("img/img0.bmp");

			p -> SetFlag(0,IMG_RTO_CDTRACK_TIME);
			break;
		case GAME_OVER_INFERNAL_LOCKED:
			p -> SetName("img/img1.bmp");
			ptr = dgD -> getInvText(DG_FINAL,3);
			if(ptr){
				aciImgText -> alloc(ptr,strlen(ptr));
				p -> SetFlag(0,IMG_RTO_SHOW_TEXT | IMG_RTO_KEEP_IMAGE);
			}
			p -> SetFlag(0,IMG_RTO_CDTRACK_TIME);
			break;
		case GAME_OVER_SPOBS_LOCKED:
			p -> SetName("img/img2.bmp");
			ptr = dgD -> getInvText(DG_FINAL,4);
			if(ptr){
				aciImgText -> alloc(ptr,strlen(ptr));
				p -> SetFlag(0,IMG_RTO_SHOW_TEXT | IMG_RTO_KEEP_IMAGE);
			}
			p -> SetFlag(0,IMG_RTO_CDTRACK_TIME);
			break;
		case GAME_OVER_ALL_LOCKED:
			p -> SetName("img/img3.bmp");
			ptr = dgD -> getInvText(DG_FINAL,5);
			if(ptr){
				aciImgText -> alloc(ptr,strlen(ptr));
				p -> SetFlag(0,IMG_RTO_SHOW_TEXT | IMG_RTO_KEEP_IMAGE);
			}
			p -> SetFlag(0,IMG_RTO_CDTRACK_TIME);
			break;
		case GAME_OVER_LUCKY:
			p -> SetName("img/img4.bmp");
			ptr = dgD -> getInvText(DG_FINAL,2);
			if(ptr){
				aciImgText -> alloc(ptr,strlen(ptr));
				p -> SetFlag(0,IMG_RTO_SHOW_TEXT | IMG_RTO_KEEP_IMAGE);
			}
			p -> SetFlag(0,IMG_RTO_CDTRACK_TIME);
			break;
		case GAME_OVER_LAMPASSO:
			p -> SetName("img/img5.bmp");
			ptr = dgD -> getInvText(DG_FINAL,1);
			if(ptr){
				aciImgText -> alloc(ptr,strlen(ptr));
				p -> SetFlag(0,IMG_RTO_SHOW_TEXT | IMG_RTO_KEEP_IMAGE);
			}
			p -> SetFlag(0,IMG_RTO_CDTRACK_TIME);
			break;
		case GAME_OVER_ABORT:
			lp -> SetNext(RTO_MAIN_MENU_ID);
			break;
		case GAME_OVER_NETWORK:
			iMultiFlag = 1;
			lp -> SetNext(RTO_MAIN_MENU_ID);
			break;
		default:
			lp -> SetNext(RTO_MAIN_MENU_ID);
			break;
	}
	p -> SetFlag(0,IMG_RTO_CD_IMAGE);
	GameOverID = 0;
}

int aciTextQuant(void)
{
	int ret = 0;
	ShowImageRTO* p = (ShowImageRTO*)xtGetRuntimeObject(RTO_SHOW_IMAGE_ID);

	if(aciImgText -> flags & ACI_TEXT_INIT){
		ret = 0;
		if(ShowImageMouseFlag || ShowImageKeyFlag){
			if(aciImgText -> flags & ACI_TEXT_END)
				ret = 1;
			else {
				aciImgText -> NextPage();
				if(aciImgText -> flags & ACI_TEXT_END){
					if(p -> imageData){
						XGR_PutSpr(0,0,p -> imageSX,p -> imageSY,p -> imageData,XGR_BLACK_FON);
					}
					else {
						XGR_Fill(0);
					}
					XGR_Flush(0,0,XGR_MAXX,XGR_MAXY);
				}
			}

			ShowImageMouseFlag = ShowImageKeyFlag = 0;
		}
		aciImgText -> Quant();
		if(!(aciImgText -> flags & ACI_TEXT_REDRAW)){
			if(p -> imageData){
				XGR_PutSpr(0,0,p -> imageSX,p -> imageSY,p -> imageData,XGR_BLACK_FON);
			}
			else {
				XGR_Fill(0);
			}
			aciImgText -> redraw();
			XGR_Flush(0,0,XGR_MAXX,XGR_MAXY);
		}
	}
	else {
		if(ShowImageMouseFlag || ShowImageKeyFlag)
				ret = 1;
	}
	return ret;
}

void aciPrepareScreenText(char* ptr,int sz,int max_len)
{
	int i,offs_start,offs_end,offs;

//	  XStream fh("text0.dat",XS_OUT);
//	  fh.write(ptr,sz);
//	  fh.close();

	offs_start = offs_end = 0;
	while(offs_start < sz - 1){
		while(ptr[offs_end] != '\n'){
			offs_end ++;
			if(offs_end >= sz){
				offs_end = sz - 1;
				break;
			}
		}
		if(offs_end - offs_start > max_len){
			offs = offs_start + max_len;
			for(i = 0; i < max_len; i ++){
				if(ptr[offs] == ' ' || ptr[offs] == '\t'){
					break;
				}
				offs --;
				if(offs <= offs_start){
					break;
				}
			}
			ptr[offs] = '\n';
			if(offs > offs_start){
				offs_start = offs;
				offs_end = offs + 1;
			}
			else {
				offs_start = offs_end;
				offs_end ++;
			}
		}
		else {
			offs_start = offs_end;
			offs_end ++;
		}
	}

//	  fh.open("text1.dat",XS_OUT);
//	  fh.write(ptr,sz);
//	  fh.close();
}

void aciPrepareEndImage(void)
{
	StopCDTRACK();
	ShowImageRTO* p;

	p = (ShowImageRTO*)xtGetRuntimeObject(RTO_SHOW_IMAGE_ID);
	p -> SetNumFiles(1);
	if(lang() == RUSSIAN)
		p -> SetName("img/img8.bmp",0);
	else
		p -> SetName("img/img6.bmp",0);
	p -> SetFlag(0,IMG_RTO_CD_IMAGE);
	p -> SetNext(XT_TERMINATE_ID);
}

char* aciLoadPackedFile(XStream& fh,int& out_sz) //Text decompress
{
	int sz;
	char *p,*ptr;

	sz = fh.size();
	p = new char[sz];
	fh.read(p,sz);
	
	out_sz = *(unsigned int*)(p + 2);
	long long_out_sz = out_sz;
	ptr = new char[out_sz];

	//ZIP_expand(ptr,out_sz,p,sz);
	/* ZLIB realisation (stalkerg)*/
	if(*(short*)(p)) { //if label = 0 not compress
		//std::cout<<"aciLoadPackedFile DeCompress "<<fh.file_name<<" file."<<std::endl;
		int stat = uncompress((Bytef*)ptr,(uLongf*)&long_out_sz,(Bytef*)(p+2+4),sz-2-4);
		switch(stat){
			//case Z_OK: std::cout<<"DeCompress ok."<<std::endl; break;
			case Z_MEM_ERROR: std::cout<<"DeCompress not enough memory."<<std::endl; break;
			case Z_BUF_ERROR: std::cout<<"DeCompress not enough room in the output buffer."<<std::endl; break;
			case Z_DATA_ERROR: std::cout<<"DeCompress error data."<<std::endl; break;
		};
	} else {
		memcpy(ptr, p + 2 + 4,(unsigned)(sz - 2 - 4));
	}
	
	delete[] p;
	
	return ptr;
}

void aciDisableParametersMenu(void)
{
	fncMenu* m = (fncMenu*)aScrDisp -> menuList -> last;

	while(m){
		if(m -> flags & FM_MAIN_MENU)
			m -> delete_item(FMENU_PARAMETERS);
		m = (fncMenu*)m -> prev;
	}
}

int sdlEventToCode(SDL_Event *event) {
	switch (event->type) {
		case SDL_KEYDOWN:
		case SDL_KEYUP:
			return event->key.keysym.scancode;
		case SDL_MOUSEBUTTONDOWN:
		case SDL_MOUSEBUTTONUP:
			switch (event->button.button) {
				case SDL_BUTTON_LEFT:
					return iMOUSE_LEFT_PRESS_CODE;
				case SDL_BUTTON_RIGHT:
					return iMOUSE_RIGHT_PRESS_CODE;
			}
		case SDL_MOUSEMOTION:
			if (event->motion.state & SDL_BUTTON_LMASK)
				return iMOUSE_LEFT_MOVE;
			else if (event->motion.state & SDL_BUTTON_RMASK)
				return iMOUSE_RIGHT_MOVE;
			else
				return iMOUSE_MOVE_CODE;
	}
	return -1;
}
