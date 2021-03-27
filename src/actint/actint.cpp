/* ---------------------------- INCLUDE SECTION ----------------------------- */

#include "../global.h"

#include "../runtime.h"

#include "../iscreen/hfont.h"
#include "../iscreen/iscreen.h"
#include "item_api.h"
#include "actint.h"
#include "aci_scr.h"
#include "aci_str.h"
#include "aci_evnt.h"
#include "a_consts.h"
#include "credits.h"
#include "../iscreen/ikeys.h"
#include "../iscreen/controls.h"

#include "mlconsts.h"
#include "mlstruct.h"

#include "../units/uvsapi.h"

#include "../uvs/univang.h"
#include "../uvs/diagen.h"

#include "../sound/hsound.h"
#include "layout.h"
/* ----------------------------- STRUCT SECTION ----------------------------- */
/* ----------------------------- EXTERN SECTION ----------------------------- */

template <> void layout(invMatrix* view, int width, int height){
	unsigned int anchor = view->anchor;

	if(anchor & WIDGET_ANCHOR_INITIALIZED){
		std::cout<<"  WARNING: layout is already done"<<std::endl;
		return;
	}

	view->anchor |= WIDGET_ANCHOR_INITIALIZED;

	if(anchor & WIDGET_ANCHOR_RIGHT){
		view->ScreenX = width - view->ScreenX - view->ScreenSizeX;
	}

	if(anchor & WIDGET_ANCHOR_BOTTOM){
		view->ScreenY = height - view->ScreenY - view->ScreenSizeY;
	}
}

template <> void layout(bmlObject* view, int width, int height){
	unsigned int anchor = view->anchor;

	if(anchor & WIDGET_ANCHOR_INITIALIZED){
		std::cout<<"  WARNING: layout is already done"<<std::endl;
		return;
	}

	view->anchor |= WIDGET_ANCHOR_INITIALIZED;

	if(anchor & WIDGET_ANCHOR_RIGHT){
		view->OffsX = width - view->OffsX - view->SizeX;
	}

	if(anchor & WIDGET_ANCHOR_BOTTOM){
		view->OffsY = height - view->OffsY - view->SizeY;
	}
}

extern int uvsTabuTaskFlag;

extern bmlObject* aIndArrowBML;
extern bmlObject* aIndDataBML;
extern bmlObject* aIndBackBML;

extern int iChatExit;

extern int NetworkON;

extern int iMouseX;
extern int iMouseY;

extern int iMouseLPressFlag;
extern int iMouseRPressFlag;

extern int iMousePrevX;
extern int iMousePrevY;

extern aciFont** aScrFonts32;

#ifdef _ACI_BML_FONTS_
extern aciFont** aScrFonts;
#else
extern iScreenFont** aScrFonts;
#endif

extern int aciKeyboardLocked;
extern int aciML_ToolzerRadius;

extern unsigned char* iscrPal;
extern iScreenDispatcher* iScrDisp;

extern int CurrentWorld;
extern int GameQuantReturnValue;

extern int* aSinTable;
extern int* aCosTable;

extern int uvsCurrentWorldUnable;
extern int iScreenLog;
extern int* AVI_index;

extern actIntDispatcher* aScrDisp;
extern int mechosCameraOffsetX;
extern int PalIterLock;
extern int aciShopMenuLog;
extern int ExclusiveLog;

extern int aciWorldIndex;

extern int Pause;

extern char* aciSTR_OFF;
extern char* aciSTR_DAY;
extern char* aciSTR_UNDEFINED_PRICE;
extern char* aciSTR_PRICE;
extern char* aciSTR_EMPTY_SLOT;
extern char* aciSTR_UNNAMED_SAVE;
extern char* aciSTR_AUTOSAVE;
extern char* aciSTR_KILLS;
extern char* aciSTR_DEATHS;
extern char* aciSTR_LUCK;
extern char* aciSTR_DOMINANCE;
extern char* aciSTR_BROKEN;
extern char* aciSTR_ENERGY_SHIELD;
extern char* aciSTR_RESTORING_SPEED;
extern char* aciSTR_MECHANIC_ARMOR;
extern char* aciSTR_VELOCITY;
extern char* aciSTR_SPIRAL_CAPACITY;
extern char* aciSTR_AIR_RESERVE;
extern char* aciSTR_DAMAGE;
extern char* aciSTR_LOAD;
extern char* aciSTR_SHOTS;
extern char* aciSTR_BURST;
extern char* aciSTR_WORKING_TIME;
extern char* aciSTR_SECONDS;
extern char* aciSTR_IN_PACK;
extern char* aciSTR_NO_CASH;
extern char* aciSTR_PICKUP_ITEMS_OFF;
extern char* aciSTR_PICKUP_WEAPONS_OFF;
extern char* aciSTR_PutThis;

extern int aciItmTextQueueSize;

/* --------------------------- PROTOTYPE SECTION ---------------------------- */

void aciWorldExploredInfo(void);
void LoadingMessage(int flush = 0);
void acsHandleExtEvent(int code,int data0 = 0,int data1 = 0,int data2 = 0);

void aciShowLocationPicture(void);
void aciPrepareScreenText(char* ptr,int sz,int max_len);

void aOutText32(int x,int y,int color,void* text,int font,int hspace,int vspace);
void aOutText32clip(int x,int y,int color,void* text,int font,int hspace,int vspace);
int aTextWidth32(void* text,int font,int hspace);
int aTextHeight32(void* text,int font,int vspace);

void aciInitItmTextQueue(void);
void aciReadItmText(void);
char* dgGetThreallPhrase(void);
void aciShowRacingPlace(void);

void aciML_Pause(void);
void aciML_Resume(void);

int aciGetCurCycle(void);

void ipal_init(unsigned char* p);

void aciPutBufSpr(int x,int y,int sx,int sy,int bsx,void* src,void* dest,int col,int col_sz);

iListElement* iGetObject(const char* scr,const char* obj);
int aciGetCurMatrixPrice(void);

void aciChangeWorld(int id);

void iChatInit(void);
void iChatQuant(int flush = 0);
void iChatFinit(void);
void iChatKeyQuant(SDL_Event *k);
void iChatMouseQuant(int x,int y,int bt);

void LoadResourceSOUND(const char *path_name, int surface);
void SoundEscaveOff(void);

void aci_setMatrixBorder(void);
void aci_setScMatrixBorder(void);
void aci_LocationQuant(void);
void aciAutoMoveItems(void);
void aciSetMechName(void);
void aciSell_Item(invItem* p);
void aciSell_Matrix(invMatrix* p);
void aciSellMoveItem(void);
void aciInitInfoPanels(void);
int aciCheckSellItem(void);
void aciSetTimePanel(void);
void aciInitEvLineID(void);
void aciChangeList(void);
void aciInitShopItems(void);
void aciSetShopItem(int id);
void aciShowFrags(void);
void aciInitAviObject(void);

void aciPutTreasureItem(void);

void aciCHandler(int k);

void aciHandleCameraEvent(int code,int data);
void aciPrepareWorldsMenu(void);

void aciSet_aMouse(void);
void aciSet_iMouse(void);

void aciNextShopItem(void);
void aciPrevShopItem(void);
void aciInitShopAvi(void);
void aciChangeAviIndex(void);
void aciBuyItem(void);

void aciInitShopButtons(void);

void aciGetEleech(void);
void aciGetRubox(void);
void aciInitMechosInfo(invMatrix* p,InfoPanel* ip);

void aciInitShopList(void);
void aciChooseShopItem(void);

void aciNextPhrase(void);
void aciStartSpeech(void);
void aciInitCurMatrixPtr(void);

void aciHandleSubmenuEvent(int id,fncMenuItem* p);

char* aciGetPrice(iListElement* p,int mode,int sell_mode);
char* aciGetItemLoad(invItem* p,int mode);

void aciSwapMatrices(void);
void aciCancelMatrix(void);
void aciShowScMatrix(void);

void aciNextPhrase(void);
void aciBuildQuestList(void);
void aciAskQuestion(void);
void aciShowBlockPhrase(void);

void aciPay4Item(invItem* p);
int aciCheckItemCredits(void);

void iScrQuant(void);
void i_evince_pal(unsigned char* p,int delta);

int aciGetScreenMechos(int x,int y);

void redraw_fnc(int x,int y,int sx,int sy,int fnc);

char* GetCompasTarget(void);

void swap_buf_col(int src,int dest,int sx,int sy,unsigned char* buf);

void restore_mouse_cursor(void);
void set_mouse_cursor(char* p,int sx,int sy);
void set_screen(int Dx,int Dy,int mode,int xcenter,int ycenter);

void put_fon(int x,int y,int sx,int sy,unsigned char* buf);
void put_attr_fon(int x,int y,int sx,int sy,unsigned char* buf);
void iregRender(int LowX,int LowY,int HiX,int HiY);

void show_map(int x,int y,int sx,int sy);
void a_buf_line(int x1,int y1,int x2,int y2,int col,int sx,int sy,unsigned char* buf);
void a_buf_line2(int x1,int y1,int x2,int y2,int col,int sx,int sy,unsigned char* buf);
void a_buf_setpixel(int x,int y,int sx,int sy,int col,unsigned char* buf);

void ibsout(int x,int y,void* ptr);

void change_screen(int scr);

void mem_putspr(int x,int y,int sx,int sy,int bsx,int bsy,unsigned char* src,unsigned char* dest);
void mem_putspr_h(int x,int y,int sx,int sy,int bsx,int bsy,unsigned char* src,unsigned char* dest);
void mem_rectangle(int x,int y,int sx,int sy,int bsx,int col_in,int col_out,int mode,void* buf);

void build_cell_frame(void);
void free_cell_frame(void);
void aMS_init(void);

#if defined(_GENERATE_MATRIX_SHAPES_) || defined(_GENERATE_iMATRIX_SHAPES_)
void smooth_shape_quant(int sx,int sy,unsigned char* buf,int src,int dest,int cnd);
#endif

void put_buf2col(int x,int y,int sx,int sy,unsigned char* buf,int null_lev,int n_mode);
void put_map(int x,int y,int sx,int sy);

#ifdef _DEBUG
void aciResizeItem(double delta);
#endif

int acsQuant(void);

/* --------------------------- DEFINITION SECTION --------------------------- */

const int 	aciTEXT_COLOR	= 165 + (2 << 16);

// Ind colors...
#define ACI_IND_SPIRAL_COL	195
#define ACI_IND_SPIRAL_SIZE	3

#define ACI_IND_JUMP_COL	160
#define ACI_IND_JUMP_SIZE	1

#define ACI_IND_DVC_COL 	160
#define ACI_IND_DVC_SIZE	1

#define ACI_IND_NRG0_COL	224
#define ACI_IND_NRG0_SIZE	1
#define ACI_IND_NRG1_COL	128
#define ACI_IND_NRG1_SIZE	1

#define ACI_IND_LOAD_COL	224
#define ACI_IND_LOAD_SIZE	1

#define ACI_IND_FON_START	74
#define ACI_IND_FON_SIZE	3

#define ACI_IND_ARROW_START	72
#define ACI_IND_ARROW_SIZE	1

#if defined(_GENERATE_MATRIX_SHAPES_) || defined(_GENERATE_iMATRIX_SHAPES_)
//#define _FACET_FLOOR_
#ifdef _GENERATE_MATRIX_SHAPES_
#define SHAPE_DEPTH	6
#else
#define SHAPE_DEPTH	8
#endif
#endif

const int 	MAX_ITEM_ID		= 9;
const int 	MAX_MATRIX_ID		= 3;

#define AS_NUMRES		2

#define FULLSCR_MODE		1
#define INVSCR_MODE		2

#define FM_OFFSET		3
#define ACI_ARROW_OFFS_UP	1
#define ACI_ARROW_OFFS_DOWN	5

#define IND_DATA_OFFS		5

int aCellSize		= 20;
int iCellSize		= 20;
int aci_CellSize	= 20;

int aciCurCredits01 = 0;

int curItemID = 1;
int curMatrixID = 0;

unsigned char* aCellFrame;

int aciTextColBright = 0;
int aciTextColBrightDelta = 1;

int aci_tmp = 0;
int aci_tmp_max = 64;

int aciCurLuck = 0;
int aciPrevLuck = -1;
int aciCurDominance = 0;
int aciPrevDominance = -1;

int aciCurEnergy = 63;
int aciMaxEnergy = 64;

int aciCurArmor = 63;
int aciMaxArmor = 64;

int aciCurLoad = 0;
int aciMaxLoad = 1;

int aciCurFrame = 0;

int aciTextEndFlag = 0;

int aciCurWpnResource = 0;
int aciMaxWpnResource = 1;

int aciCurDvcResource = 0;
int aciMaxDvcResource = 1;

int aciCurJumpCount = 3;
int aciMaxJumpCount = 5;
int aciPrevJumpCount = -1;

//int aciCurCredits = 11000;
int aciCurTime = 0;

int aciCurJumpImpulse = 0;
int aciMaxJumpImpulse = 1;

int aciNumColSchemes = 0;
int aciColSchemeLen = 0;
unsigned char** aciColorSchemes = NULL;
unsigned char* aciCurColorScheme;
unsigned char* actIntPal;

const char* aci_curLocationName = "";
int aci_curLocationID = 0;
int aci_SecondMatrixID = 0;

char* aci_ivMapName = NULL;
char* aci_iScreenID = NULL;
unsigned char aci_iscrPal[768];

invMatrix* backupMatrix;

int prevWorld;

int aciPickupItems = 1;
int aciPickupDevices = 1;

int aciBufRedrawFlag = 0;

int aciProtractorEvent = 0;
int aciMechMessiahEvent = 0;
int aciTeleportEvent = 0;

#ifdef _DEBUG
int aciShotCount = 0;
int aciCurIND = 0;
int aciIndMove = 0;
#endif

aciBitmapMenuItem::aciBitmapMenuItem(void)
{
	ID = 0;
	PosX = PosY = 0;
	SizeX = SizeY = 0;

	numStates = curState = 0;
	flags = 0;

	fname = NULL;
}

aciBitmapMenuItem::~aciBitmapMenuItem(void)
{
	if(fname) delete[] fname;
	if(flags & BMI_DATA_LOADED) delete[] data;
}

aciBitmapMenu::aciBitmapMenu(void)
{
	ID = 0;
	PosX = PosY = 0;
	SizeX = SizeY = 0;

	activeCount = 0;
	curCount = 0;

	flags = BM_REBUILD;
	upMenu = NULL;

	items = new iList;
	pictBuf = NULL;
}

aciBitmapMenu::~aciBitmapMenu(void)
{
	aciBitmapMenuItem* p,*p1;
	p = (aciBitmapMenuItem*)items -> last;
	while(p){
		p1 = (aciBitmapMenuItem*)p -> prev;
		delete p;
		p = p1;
	}
	delete items;
	if(pictBuf) delete[] pictBuf;
}

void aciBitmapMenuItem::load(void)
{
	XStream fh;
	short sx,sy,s;
	if(!fname) return;

	if(!(flags & BMI_DATA_LOADED)){
		fh.open(fname,XS_IN);
		fh > sx > sy > s;

		SizeX = sx;
		SizeY = sy;
		numStates = s;

		data = new unsigned char[SizeX * SizeY * numStates];
		fh.read(data,SizeX * SizeY * numStates);
		fh.close();

		flags |= BMI_DATA_LOADED;
		switch(ID){
			case BMENU_ITEM_ROT:
				curState = iGetOptionValue(iCAMERA_TURN);
				break;
			case BMENU_ITEM_ZOOM:
				curState = iGetOptionValue(iCAMERA_SCALE);
				break;
			case BMENU_ITEM_PERSP:
				curState = iGetOptionValue(iCAMERA_SLOPE);
				break;
		}
	}
}

void aciBitmapMenuItem::free(void)
{
	if(flags & BMI_DATA_LOADED){
		delete[] data;
		flags ^= BMI_DATA_LOADED;
	}
}

void aciBitmapMenuItem::init_name(char* p)
{
	int sz = strlen(p) + 1;
	if(fname) delete fname;

	fname = new char[sz];
	strcpy(fname,p);
}

void aciBitmapMenu::add_item(aciBitmapMenuItem* p)
{
	items -> connect((iListElement*)p);
}

void aciBitmapMenuItem::redraw(int bsx,int bsy,unsigned char* buf)
{
	mem_putspr(PosX,PosY,SizeX,SizeY,bsx,bsy,data + SizeX * SizeY * curState,buf);
}

int aciBitmapMenuItem::check_xy(int x,int y)
{
	if(x >= PosX && y >= PosY && x < PosX + SizeX && y < PosY + SizeY) return 1;
	return 0;
}

void aciBitmapMenu::init(void)
{
	aciBitmapMenuItem* p = (aciBitmapMenuItem*)items -> last;
	while(p){
		p -> load();
		p = (aciBitmapMenuItem*)p -> prev;
	}
	alloc_mem();
	remap_coords();
}

void aciBitmapMenu::finit(void)
{
	aciBitmapMenuItem* p = (aciBitmapMenuItem*)items -> last;
	while(p){
		p -> free();
		p = (aciBitmapMenuItem*)p -> prev;
	}
	free_mem();
}

void aciBitmapMenu::alloc_mem(void)
{
	pictBuf = new unsigned char[SizeX * SizeY];
	flags |= BM_REBUILD;
}

void aciBitmapMenu::free_mem(void)
{
	delete[] pictBuf;
	pictBuf = NULL;
}

void aciBitmapMenu::build_pict(void)
{
	aciBitmapMenuItem* p = (aciBitmapMenuItem*)items -> last;
	memset(pictBuf,aciCurColorScheme[ACI_BACK_COL],SizeX * SizeY);
	while(p){
		p -> redraw(SizeX,SizeY,pictBuf);
		p = (aciBitmapMenuItem*)p -> prev;
	}
	flags &= ~BM_REBUILD;
}

void aciBitmapMenu::redraw(int x,int y,int sx,int sy)
{
	int dx,dy;

	if(!pictBuf) return;

	dx = (sx - SizeX)/2;
	dy = (sy - SizeY)/2;

	if(flags & BM_REBUILD) build_pict();
	XGR_PutSpr(x + dx,y + dy,SizeX,SizeY,pictBuf,XGR_BLACK_FON);
}

#ifdef _ACI_BML_FONTS_
aciFont::aciFont(void)
{
	flags = 0;
	SizeX = SizeY = Size = 0;
	data = NULL;
	LeftOffs = RightOffs = NULL;
}

aciFont::~aciFont(void)
{
	if(data) delete data;
	if(LeftOffs) delete LeftOffs;
	if(RightOffs) delete RightOffs;
}

void aciFont::load(char* fname)
{
	short sx,sy,sz;
	XStream fh(fname,XS_IN);
	fh > sx > sy > sz;
	SizeX = sx;
	SizeY = sy;
	Size = sz;

	data = new unsigned char[SizeX * SizeY * Size];
	fh.read(data,SizeX * SizeY * Size);
	fh.close();
}

void aciFont::calc_offs(void)
{
	int i,j,s,offs = 0;
	int align_left,align_right,fl,fr;
	unsigned char* p = data;

	LeftOffs = new char[Size];
	RightOffs = new char[Size];

	for(s = 0; s < Size; s ++){
		offs = SizeX * SizeY * s;
		align_left = align_right = 3;
		for(i = 0; i < SizeY; i ++){
			fl = fr = 0;
			for(j = 0; j < SizeX - 1; j ++){
				if(flags & ACI_RANGE_FONT){
					if(!fl && p[offs + j] && j < align_left){
						align_left = j;
						fl = 1;
					}
					if(!fr && p[offs + SizeX - j - 1] && j < align_right){
						align_right = j;
						fr = 1;
					}
				}
				else {
					if(!fl && p[offs + j] == 1 && j < align_left){
						align_left = j;
						fl = 1;
					}
					if(!fr && p[offs + SizeX - j - 1] == 1 && j < align_right){
						align_right = j;
						fr = 1;
					}
				}
			}
			offs += SizeX;
		}
		if(s != ' '){
			LeftOffs[s] = align_left;
			RightOffs[s] = align_right;
		}
		else {
			if(SizeX < 8){
				LeftOffs[s] = 1;
				RightOffs[s] = 1;
			}
			else {
				LeftOffs[s] = 3;
				RightOffs[s] = 3;
			}
		}
	}
}
#endif

aciWorldMap::aciWorldMap(void)
{
	int i;
	redraw_buf = NULL;
	ShapeSizeX = NULL;
	ShapeSizeY = NULL;

	shapes = NULL;

	flags = WMAP_REBUILD_PIC;

	world_list = new iList;

	shape_files = new char*[3];
	for(i = 0; i < 3; i ++) {
		shape_files[i] = NULL;
	}

	ShapeOffsX = new short[3];
	ShapeOffsY = new short[3];

	world_ids = new signed char[AS_MAX_WORLD];

	for(i = 0; i < 3; i ++)
		ShapeOffsX[i] = ShapeOffsY[i] = 0;

	world_ptr = new aciWorldInfo*[AS_MAX_WORLD];
}

aciWorldMap::~aciWorldMap(void)
{
	int i;
	aciWorldInfo* p,*p1;

	delete[] ShapeOffsX;
	delete[] ShapeOffsY;

	if(redraw_buf) {
		delete[] redraw_buf;
	}

	delete[] world_ids;

	if(shapes){
		for(i = 0; i < AS_NUM_WORLD_SHAPES; i ++)
			delete[] shapes[i];
		delete[] shapes;
	}
	if(ShapeSizeX){
		delete[] ShapeSizeX;
	}
	if(ShapeSizeY){
		delete[] ShapeSizeY;
	}

	for(i = 0; i < 3; i ++) {
		if(shape_files[i]) { 
			delete[] shape_files[i]; 
		}
	}
	delete[] shape_files;

	p = (aciWorldInfo*)world_list -> last;
	while(p){
		p1 = (aciWorldInfo*)p -> prev;
		delete p;
		p = p1;
	}
	delete world_list;

	delete[] world_ptr;
}

int aciWorldMap::world_key(int id)
{
	int i;
	aciWorldInfo* p,*p1;

	p1 = NULL;
	p = (aciWorldInfo*)world_list -> last;
	while(p){
		if(p -> links[id]) return 1;
		if(p -> ID == id) p1 = p;
		p = (aciWorldInfo*)p -> prev;
	}
	if(p1){
		for(i = 0; i < AS_MAX_WORLD; i ++)
			if(p1 -> links[i]) return 1;
	}
	return 0;
}

aciWorldInfo::aciWorldInfo(void)
{
	ID = 0;
	uvsID = -1;
	flags = 0;

	font = 0;

	PosX = PosY = 0;

	name = NULL;

	letter = 0;
	shape_id = 0;

	links = new char[AS_MAX_WORLD];
	memset(links,0,AS_MAX_WORLD);
}

aciWorldInfo::~aciWorldInfo(void)
{
	if(name) {
		delete[] name;
	}

	delete[] links;
}

void aciWorldInfo::add_link(int id)
{
	links[id] = 1;
}

void aciWorldInfo::delete_link(int id)
{
	links[id] = 0;
}

aciLocationInfo::aciLocationInfo(void)
{
	int i;

	ID = 0;
	WorldID = 0;
	SaveScreenID = 3;

	numIndex = new char[ACI_LOCATION_INDEX_SIZE];
	memset(numIndex,-1,ACI_LOCATION_INDEX_SIZE);

	mapData = new iList;

	numColorScheme = SCH_ISCREEN;
	mapName = NULL;
	palName = NULL;
	screenID = NULL;

	nameID = NULL;
	nameID2 = NULL;

	numMatrixShutters1 = 0;
	MatrixShutters1 = NULL;

	numMatrixShutters2 = 0;
	MatrixShutters2 = NULL;

	numGateShutters = 0;
	GateShutters = NULL;

	ExcludeItems = new char[ACI_MAX_TYPE];
	memset(ExcludeItems,0,ACI_MAX_TYPE);

	objIDs = new char*[ACI_NUM_OBJECTS];
	objList = new iListElement*[ACI_NUM_OBJECTS];

	s_objIDs = new char*[ACI_NUM_S_OBJECTS];
	s_objList = new iListElement*[ACI_NUM_S_OBJECTS];

	soundResPath = NULL;

	for(i = 0; i < ACI_NUM_OBJECTS; i ++)
		objIDs[i] = NULL;

	for(i = 0; i < ACI_NUM_S_OBJECTS; i ++)
		s_objIDs[i] = NULL;
}

aciLocationInfo::~aciLocationInfo(void)
{
	int i;
	iListElement* p,*p1;
	if(nameID) delete[] nameID;
	if(nameID2) delete[] nameID2;
	if(mapName) delete[] mapName;
	if(palName) delete[] palName;
	if(screenID) delete[] screenID;

	p = mapData -> last;
	while(p){
		p1 = p -> prev;
		delete p;
		p = p1;
	}
	delete mapData;

	if(soundResPath)
		delete[] soundResPath;

	delete[] numIndex;

	for(i = 0; i < numMatrixShutters1; i ++)
		delete MatrixShutters1[i];
	delete[] MatrixShutters1;

	for(i = 0; i < numMatrixShutters2; i ++)
		delete MatrixShutters2[i];
	delete[] MatrixShutters2;

	for(i = 0; i < numGateShutters; i ++)
		if(GateShutters[i]) delete GateShutters[i];
	delete[] GateShutters;

	for(i = 0; i < ACI_NUM_OBJECTS; i ++){
		if(objIDs[i])
			delete[] objIDs[i];
	}
	delete[] objIDs;
	for(i = 0; i < ACI_NUM_S_OBJECTS; i ++){
		if(s_objIDs[i])
			delete[] s_objIDs[i];
	}
	delete[] s_objIDs;
	delete[] ExcludeItems;

	delete[] objList;
	delete[] s_objList;
}

void aciLocationInfo::prepare(void)
{
	int i;
	iScreen* scr;
	if(!objIDs[ACI_SHOP_SCREEN_ID]){
		for(i = 0; i < ACI_NUM_OBJECTS; i ++)
			objList[i] = NULL;
	}
	else {
		scr = (iScreen*)iScrDisp -> get_object(objIDs[ACI_SHOP_SCREEN_ID]);
		for(i = 0; i < ACI_NUM_OBJECTS; i ++){
			if(objIDs[i]){
				objList[i] = scr -> get_object(objIDs[i]);
			}
			else {
				objList[i] = NULL;
			}
		}
	}
	scr = (iScreen*)iScrDisp -> get_object(screenID);
	if(!scr) ErrH.Abort("Escave screen not found...");
	for(i = 0; i < ACI_NUM_S_OBJECTS; i ++){
		if(s_objIDs[i]){
			s_objList[i] = scr -> get_object(s_objIDs[i]);
		}
		else {
			s_objList[i] = NULL;
		}
	}
}

void bmlObject::change_color(int from,int to)
{
	int i,j,fr,index = 0;
	for(fr = 0; fr < Size; fr ++){
		for(i = 0; i < SizeY; i ++){
			for(j = 0; j < SizeX; j ++){
				if(frames[index] == from) frames[index] = to;
				index ++;
			}
		}
	}
}

void aciLocationInfo::init_map_data(void)
{
	unsigned short x,y,sx,sy,num;
	unsigned char* data;

	XStream fh;

	iListElement* p = mapData -> last;

	while(p){
		if(p -> ID_ptr.size()){
			fh.open(p->ID_ptr.c_str(),XS_IN);
			fh > sx > sy > num > x > y;
			data = new unsigned char[sx * sy * num];
			fh.read(data,sx * sy * num);
			fh.close();

			put_fon(x,y,sx,sy,data);
			put_attr_fon(x,y,sx,sy,data + sx * sy);
//			  iregRender(x,y,x + sx,y + sy);

			delete[] data;
		}
		p = p -> prev;
	}
}

iScreen* aciLocationInfo::get_shop_scr(void)
{
	iScreen* scr;
	if(!objIDs[ACI_SHOP_SCREEN_ID]) return NULL;
	scr = (iScreen*)iScrDisp -> get_object(objIDs[ACI_SHOP_SCREEN_ID]);

	return scr;
}

aIndData::aIndData(void)
{
	ID = 0;

	type = IND_CLOCK;

	NumVals = 1;

	CornerNum = 0;

	promptData = NULL;

	flags = 0;

	SizeX = SizeY = 0;

	pict_buf = NULL;
	bml = NULL;

	colors = NULL;
	check_points = NULL;
	value = NULL;
	value_ptr = NULL;
	max_value = NULL;

	PosX = PosY = 0;

	alloc_mem();
}

aIndData::~aIndData(void)
{
	if(pict_buf)
		delete[] pict_buf;

	delete[] check_points;
	delete[] value;
	delete[] value_ptr;
	delete[] max_value;
	delete[] colors;

	if(promptData)
		delete[] promptData;

	if(bml)
		delete bml;
}

bmlObject::bmlObject(void)
{
	ID = 0;
	anchor = 0;
	flags = 0;
	SizeX = SizeY = Size = 0;
	OffsX = OffsY = 0;
	frames = NULL;
	name = NULL;
}

bmlObject::~bmlObject(void)
{
	if(frames) delete[] frames;
	if(name) delete[] name;
}

void bmlObject::init_name(char* p)
{
	int sz = strlen(p);
	name = new char[sz + 1];
	strcpy(name,p);
}

void bmlObject::free(void)
{
	if(frames) {
		delete[] frames;
	}
	frames = NULL;
}

void bmlObject::load(const char* fname,int bmp)
{
	if(!fname)
		fname = name;

	if(!fname) return;

	if(flags & BML_FRAMES_LOADED) free();

	XStream fh(fname,XS_IN);
	if(bmp)
		flags |= BMP_FLAG;

	if(!(flags & BMP_FLAG) && !(flags & BG_FLAG)){
		if(!(flags & BML_NO_OFFSETS))
			fh > SizeX > SizeY > Size > OffsX > OffsY;
		else
			fh > SizeX > SizeY > Size;
	}
	else {
		if(flags & BMP_FLAG){
			fh > SizeX > SizeY;
			Size = 1;
		}
		if(flags & BG_FLAG){
			Size = 1;
		}
	}
	frames = new unsigned char[SizeX * SizeY * Size];
	fh.read(frames,SizeX * SizeY * Size);
	fh.close();

	flags |= BML_FRAMES_LOADED;
}

void bmlObject::load2mem(unsigned char* ptr,char* fname)
{
	if(!fname)
		fname = name;

	if(!fname) return;

	//std::cout<<"bmlObject::load2mem "<<fname<<std::endl;
	XStream fh(fname,XS_IN);

	if(!(flags & BMP_FLAG) && !(flags & BG_FLAG)){
		fh > SizeX > SizeY > Size > OffsX > OffsY;
	}
	else {
		if(flags & BMP_FLAG){
			fh > SizeX > SizeY;
			Size = 1;
		}
		if(flags & BG_FLAG){
			Size = 1;
		}
	}
	fh.read(ptr,SizeX * SizeY * Size);
	fh.close();
}

void bmlObject::show(int frame)
{
	unsigned char* frame_ptr = frames + SizeX * SizeY * frame;

	XGR_PutSpr(OffsX,OffsY,SizeX,SizeY,frame_ptr,XGR_HIDDEN_FON);
}

void bmlObject::offs_show(int x,int y,int frame)
{
	unsigned char* frame_ptr = frames + SizeX * SizeY * frame;
	XGR_PutSpr(x,y,SizeX,SizeY,frame_ptr,XGR_HIDDEN_FON);
}

ibsObject::ibsObject(void)
{
	ID = 0;
	image = NULL;
	name = NULL;

	fontID = 0;

	memset(indPosX,0,4);
	memset(indPosY,0,4);

	ImageSize = 0;
}

ibsObject::~ibsObject(void)
{
	if(image) {
		delete[] image;
	}
	if(name) {
		delete[] name;
	}
}

void ibsObject::free(void)
{
	if(image) {
		delete[] image;
	}
	image = NULL;
}

void ibsObject::set_name(char* p)
{
	int sz = strlen(p);
	name = new char[sz + 1];
	strcpy(name,p);
}

void ibsObject::load(char* fname)
{
	if(!fname) fname = name;

	if(!fname) ErrH.Abort("Bad XBM name");

	XStream fh;
	XBuffer XBuf;
	XBuf < fname < ".xbm";
	fh.open(XBuf.GetBuf(),XS_IN);
	fh > PosX > PosY > bSizeX > bSizeY > SizeX > SizeY > ImageSize;
	image = new unsigned char[ImageSize];
	fh.read(image,ImageSize);
	fh.close();

	SideX = SizeX/2;
	SideY = SizeY/2;

	CenterX = PosX + SideX;
	CenterY = PosY + SideY;
}

void ibsObject::show(void)
{
	ibsout(PosX,PosY,image);
}

void ibsObject::show_bground(void)
{
	for(int i = 0; i < backs.size(); i++){
		backs[i] -> show();
	}
}

actIntDispatcher::actIntDispatcher(void)
{
	int i;
	flags = 0;

	curIbsID = 0;

	iP = NULL;

	wMap = NULL;
	qMenu = NULL;

	curPrompt = new aciPromptData;

	ScrTextData = new aciScreenText;

	invPrompt = new XGR_MousePromptScreen;
	infPrompt = new XGR_MousePromptScreen;

	freeItemList = new iList;
	freeMatrixList = new iList;
	freeMenuItemList = new iList;

	b_menuList = new iList;

	map_names = new char*[MAX_NUM_MAP];
	for(i = 0; i < MAX_NUM_MAP; i ++)
		map_names[i] = NULL;

	i_infoPanels = new iList;
	infoPanels = new iList;

	curScrMode = prevScrMode = FULLSCR_MODE;

	ModeObj = new aKeyObj;
	InvObj = new aKeyObj;
	InvObj -> add_key(SDL_SCANCODE_RETURN);
	InfoObj = new aKeyObj;

	locationList = new iList;

	menuList = new iList;
	itemList = new iList;
	matrixList = new iList;

	i_matrixList = new iList;
	i_Counters = new iList;
	i_itemList = new iList;
	i_menuList = new iList;

	iscr_iP = NULL;
	ascr_iP = NULL;

	indList = new iList;

	intButtons = new iList;
	invButtons = new iList;
	infButtons = new iList;

	intCounters = new iList;
	invCounters = new iList;
	infCounters = new iList;

	curItem = NULL;

	events = new actEventHeap;

	curMode = AS_INFO_MODE;
	mechosCameraOffsetX = AS_INF_CAMERA_OFFSET;

	mapObj = new bmlObject;
	mapObj -> flags |= BMP_FLAG;

	ibsList = new iList;
	backList = new iList;

	curIbs = NULL;
	curMatrix = NULL;
}

actIntDispatcher::~actIntDispatcher(void)
{
	int i;
	fncMenu* p,*p1;
	invMatrix* m,*m1;
	invItem* it,*it1;
	aButton* bt,*bt1;
	aIndData* ind,*ind1;
	CounterPanel* cp,*cp1;
	aciLocationInfo* lP,*lP1;
	InfoPanel* pl,*pl1;
	fncMenuItem* f,*f1;
	bmlObject* bml,*bml1;
	ibsObject* ibs,*ibs1;

	aciBitmapMenu* bm,*bm1;

	delete curPrompt;

	delete invPrompt;
	delete infPrompt;

	XGR_MouseSetPromptData(NULL);

	if(iP) delete iP;
	if(iscr_iP) delete iscr_iP;

	if(wMap) delete wMap;

	if(qMenu) delete qMenu;

	delete ScrTextData;

	if(aciColorSchemes){
		for(i = 0;  i < aciNumColSchemes; i ++)
			if(aciColorSchemes[i])
				delete[] aciColorSchemes[i];
		delete[] aciColorSchemes;
		aciNumColSchemes = 0;
	}

	delete mapObj;
	for(i = 0; i < MAX_NUM_MAP; i ++)
		if(map_names[i]) delete[] map_names[i];
	delete[] map_names;

	m = (invMatrix*)freeMatrixList -> last;
	while(m){
		m1 = (invMatrix*)m -> prev;
		delete m;
		m = m1;
	}
	delete freeMatrixList;

	f = (fncMenuItem*)freeMenuItemList -> last;
	while(f){
		f1 = (fncMenuItem*)f -> prev;
		delete f;
		f = f1;
	}
	delete freeMenuItemList;

	it = (invItem*)freeItemList -> last;
	while(it){
		it1 = (invItem*)it -> prev;
		delete it;
		it = it1;
	}
	delete freeItemList;

	pl = (InfoPanel*)i_infoPanels -> last;
	while(pl){
		pl1 = (InfoPanel*)pl -> prev;
		delete pl;
		pl = pl1;
	}
	delete i_infoPanels;

	pl = (InfoPanel*)infoPanels -> last;
	while(pl){
		pl1 = (InfoPanel*)pl -> prev;
		delete pl;
		pl = pl1;
	}
	delete infoPanels;

	delete ModeObj;
	delete InvObj;
	delete InfoObj;

	lP = (aciLocationInfo*)locationList -> last;
	while(lP){
		lP1 = (aciLocationInfo*)lP -> prev;
		delete lP;
		lP = lP1;
	}
	delete locationList;

	it = (invItem*)itemList -> last;
	while(it){
		it1 = (invItem*)it -> prev;
		delete it;
		it = it1;
	}
	delete itemList;

	it = (invItem*)i_itemList -> last;
	while(it){
		it1 = (invItem*)it -> prev;
		delete it;
		it = it1;
	}
	delete i_itemList;

	p = (fncMenu*)menuList -> last;
	while(p){
		p1 = (fncMenu*)p -> prev;
		if(!(p -> flags & FM_ITEM_MENU))
			delete p;
		p = p1;
	}
	delete menuList;

	bm = (aciBitmapMenu*)b_menuList -> last;
	while(bm){
		bm1 = (aciBitmapMenu*)bm -> prev;
		delete bm;
		bm = bm1;
	}
	delete b_menuList;

	p = (fncMenu*)i_menuList -> last;
	while(p){
		p1 = (fncMenu*)p -> prev;
		if(!(p -> flags & FM_ITEM_MENU))
			delete p;
		p = p1;
	}
	delete i_menuList;

	m = (invMatrix*)matrixList -> last;
	while(m){
		m1 = (invMatrix*)m -> prev;
		delete m;
		m = m1;
	}
	delete matrixList;

	m = (invMatrix*)i_matrixList -> last;
	while(m){
		m1 = (invMatrix*)m -> prev;
		delete m;
		m = m1;
	}
	delete i_matrixList;

	bt = (aButton*)intButtons -> last;
	while(bt){
		bt1 = (aButton*)bt -> prev;
		delete bt;
		bt = bt1;
	}
	delete intButtons;

	bt = (aButton*)invButtons -> last;
	while(bt){
		bt1 = (aButton*)bt -> prev;
		delete bt;
		bt = bt1;
	}
	delete invButtons;

	bt = (aButton*)infButtons -> last;
	while(bt){
		bt1 = (aButton*)bt -> prev;
		delete bt;
		bt = bt1;
	}
	delete infButtons;

	ind = (aIndData*)indList -> last;
	while(ind){
		ind1 = (aIndData*)ind -> prev;
		delete ind;
		ind = ind1;
	}
	delete indList;

	cp = (CounterPanel*)intCounters -> last;
	while(cp){
		cp1 = (CounterPanel*)cp -> prev;
		delete cp;
		cp = cp1;
	}
	delete intCounters;

	cp = (CounterPanel*)infCounters -> last;
	while(cp){
		cp1 = (CounterPanel*)cp -> prev;
		delete cp;
		cp = cp1;
	}
	delete infCounters;

	cp = (CounterPanel*)invCounters -> last;
	while(cp){
		cp1 = (CounterPanel*)cp -> prev;
		delete cp;
		cp = cp1;
	}
	delete invCounters;

	cp = (CounterPanel*)i_Counters -> last;
	while(cp){
		cp1 = (CounterPanel*)cp -> prev;
		delete cp;
		cp = cp1;
	}
	delete i_Counters;

	bml = (bmlObject*)backList -> last;
	while(bml){
		bml1 = (bmlObject*)bml -> prev;
		delete bml;
		bml = bml1;
	}
	delete backList;

	ibs = (ibsObject*)ibsList -> last;
	while(ibs){
		ibs1 = (ibsObject*)ibs -> prev;
		delete ibs;
		ibs = ibs1;
	}
	delete ibsList;

	delete events;
	flags = 0;
}

invItem::invItem(void)
{
	ID = 0;

	slotType = -1;

	EvCode = 0;

	flags = 0;

	NumIndex = 0;

	classID = AS_ITEM_CLASS;

	ScreenSizeX = ScreenSizeY = 0;

	ShapeSizeX = ShapeSizeY = 0;
	ShapeCenterX = ShapeCenterY = 0;

	MatrixX = MatrixY = 0;

	pData = NULL;
	promptData = NULL;
	pTemplate = NULL;

	dropCount = 0;

	numComments = 0;
	comments = NULL;

	partData = NULL;

	ShapeLen = 0;
	ShapeX = NULL;
	ShapeY = NULL;

	fname = NULL;
	menu = NULL;

	uvsDataPtr = NULL;
	item_ptr = NULL;

	numAviIDs = 0;
	avi_ids = NULL;
}

invItem::~invItem(void)
{
	int i;
	ScreenSizeX = ScreenSizeY = 0;

	if(flags & INV_CLONE){
		ID_ptr.clear();
	}
	else {
		if(partData) delete partData;

		if(fname) {
			delete[] fname;
		}

		if(flags & AS_ITEM_DATA_LOADED)
			delete[] frame;

		if(pData)
			delete[] pData;

		if(pTemplate)
			delete[] pTemplate;

		if(promptData)
			delete[] promptData;

		if(numAviIDs){
			for(i = 0; i < numAviIDs; i ++){
				if(avi_ids[i]) delete[] avi_ids[i];
			}
			delete[] avi_ids;
		}

		if(ShapeX) delete[] ShapeX;
		if(ShapeY) delete[] ShapeY;

		if(numComments){
			for(i = 0; i < numComments; i ++)
				delete[] comments[i];
			delete[] comments;
		}
	}
}

void invItem::init(void)
{
	fncMenu* m;
	load_frame();
	if(menu){
		m = (fncMenu*)menu;
		m -> init();
		m -> curItem = m -> get_fnc_item(m -> curFunction);
		if(m -> VItems < m -> items -> Size)
			m -> firstItem = m -> curItem;
		else
			m -> firstItem = (fncMenuItem*)m -> items -> first;
	}

	calc_shape_center();
}

void invItem::finit(void)
{
	free_frame();
}

//TODO need fast malloc
void CounterPanel::redraw(void)
{
	int i,x,y,digits,ox,dx = aCellSize + 1,v = *value_ptr,col,bcol = aciCurColorScheme[ACI_BACK_COL],col_sz = 0;
	char* ptr;
	unsigned char* buf = NULL, *p = NULL;
	iScreenObject* obj;

	if(ID == CREDITS_COUNTER) v = aciGetCurCredits();

#ifdef _ACI_BML_FONTS_
	if(flags & CP_RANGE_FONT){
		col = aciCurColorScheme[FM_SELECT_START];
		col_sz = aciCurColorScheme[FM_SELECT_SIZE];
	}
	else {
		col = (aciCurColorScheme[COUNTER_BORDER_COL] << 8) | aciCurColorScheme[COUNTER_COL];
	}
#else
	col = aciCurColorScheme[COUNTER_COL];
#endif

	unsigned char* frm = aCellFrame;

	last_value = v;

	if(flags & CP_RANGE_FONT){
		y = (dx - aScrFonts32[font] -> SizeY) >> 1;
		ox = (dx - aScrFonts32[font] -> SizeX) >> 1;
	}
	else {
		y = (dx - aScrFonts[font] -> SizeY) >> 1;
		ox = (dx - aScrFonts[font] -> SizeX) >> 1;
	}

	if(!iScreenOwner){
		buf = new unsigned char[SizeX * SizeY];
	} else {
		if(!(iScreenOwner -> flags & EL_DATA_LOADED))
			return;
		obj = (iScreenObject*)iScreenOwner -> owner;
		obj -> flags |= OBJ_MUST_REDRAW;
		buf = (unsigned char*)iScreenOwner -> fdata;
	}
	p = new unsigned char[dx * dx];

	memset(buf,0,SizeX * SizeY);
	memcpy(p,frm,dx * dx);

	xconv -> init();
	*xconv <= v;

	ptr = xconv -> address();
	digits = strlen(ptr);

	x = 0;
	swap_buf_col(1,bcol,dx,dx,p);
	swap_buf_col(2,bcol,dx,dx,p);
	for(i = 0; i < MaxLen; i ++){
		mem_putspr(x,0,dx,dx,SizeX,SizeY,p,buf);
		x += dx - 1;
	}

	x = (MaxLen - digits) * dx;
	for(i = 0; i < digits; i ++){
		if(flags & CP_RANGE_FONT)
			aPutChar32(x + ox,y,font,col,col_sz,ptr[i],SizeX,buf);
		else
			aPutChar(x + ox,y,font,col,ptr[i],SizeX,SizeY,buf);
		x += dx - 1;
	}
	if(!iScreenOwner){
		XGR_PutSpr(PosX,PosY,SizeX,SizeY,buf,XGR_HIDDEN_FON);
		if(ibs) ibs -> show();
	}

	flags &= ~CP_REDRAW;
	if(!iScreenOwner) {
		delete[] buf;
	}
	delete[] p;
}

void invItem::redraw(int x,int y,int drop_mode,int flush_mode)
{
	int dx,dy,col = 0,mx,my;
	unsigned char* buf;

	mx = MatrixX * aCellSize + (aCellSize >> 1);
	if(MatrixY & 0x01) mx += (aCellSize >> 1);

	my = MatrixY * (aCellSize - (aCellSize >> 2)) + (aCellSize >> 1);

	dx = mx - ShapeCenterX + (ShapeSizeX >> 1) - (ScreenSizeX >> 1);
	dy = my - ShapeCenterY + (ShapeSizeY >> 1) - (ScreenSizeY >> 1);

	if(aciBufRedrawFlag){
		put_buf2col(800 + x + dx,y + dy,ScreenSizeX,ScreenSizeY,(unsigned char*)frame,300,0);
//		  put_map(800 + x + dx,y + dy,ScreenSizeX,ScreenSizeY);
	}
	else {
		XGR_PutSpr(x + dx,y + dy,ScreenSizeX,ScreenSizeY,frame,XGR_HIDDEN_FON);

		if(flush_mode)
			XGR_Flush(x + dx,y + dy,ScreenSizeX,ScreenSizeY);
	}

	if(drop_mode){
		switch(slotType){
			case -1:
				col = aciCurColorScheme[INV_NUM_ITM_COL];
				break;
			case AS_WEAPON_SLOT:
				col = aciCurColorScheme[INV_NUM_WPN_COL];
				break;
			case AS_TWEAPON_SLOT:
				col = aciCurColorScheme[INV_NUM_TWPN_COL];
				break;
			case AS_DEVICE_SLOT:
				col = aciCurColorScheme[INV_NUM_DVC_COL];
				break;
		}
		dx = mx + ShapeX[NumIndex] * aCellSize - (aCellSize >> 1);
		dy = my + ShapeY[NumIndex] * (aCellSize - (aCellSize >> 2)) - (aCellSize >> 1);

		if(ShapeY[NumIndex] & 0x01) dx += (aCellSize >> 1);

		buf = new unsigned char[aCellSize * aCellSize];
		memset(buf,0,aCellSize * aCellSize);
		aPutNum(aCellSize/2 - aScrFonts[INV_FONT] -> SizeX/2,aCellSize/2 - aScrFonts[INV_FONT] -> SizeY/2,INV_FONT,col,dropCount,aCellSize,buf,1);

		if(aciBufRedrawFlag){
			put_buf2col(800 + x + dx,y + dy,aCellSize,aCellSize,buf,300,0);
//			  put_map(800 + x + dx,y + dy,aCellSize,aCellSize);
		}
		else {
			XGR_PutSpr(x + dx,y + dy,aCellSize,aCellSize,buf,XGR_HIDDEN_FON);

			if(flush_mode)
				XGR_Flush(x + dx,y + dy,aCellSize,aCellSize);
		}

		delete[] buf;
	}
	if(aciBufRedrawFlag && flush_mode){
		dx = mx - ShapeCenterX + (ShapeSizeX >> 1) - (ScreenSizeX >> 1);
		dy = my - ShapeCenterY + (ShapeSizeY >> 1) - (ScreenSizeY >> 1);
		put_map(800 + x + dx,y + dy,ScreenSizeX,ScreenSizeY);
	}
#ifdef _ACI_BOUND_TEST_
	col = aciCurColorScheme[INV_NUM_ITM_COL];

	dx = mx - ShapeCenterX;
	dy = my - ShapeCenterY;
	XGR_Rectangle(x + dx,y + dy,ShapeSizeX,ShapeSizeY,col,col,XGR_OUTLINED);
	XGR_Rectangle(x + dx + ShapeCenterX - 1,y + dy + ShapeCenterY - 1,3,3,col,col,XGR_FILLED);
#endif
}

void invItem::get_coords(int px,int py,int& x,int& y,int& sx,int& sy)
{
	int dx,dy,mx,my;

	mx = MatrixX * aCellSize + (aCellSize >> 1);
	if(MatrixY & 0x01) mx += (aCellSize >> 1);

	my = MatrixY * (aCellSize - (aCellSize >> 2)) + (aCellSize >> 1);

	dx = mx - ShapeCenterX + (ShapeSizeX >> 1) - (ScreenSizeX >> 1);
	dy = my - ShapeCenterY + (ShapeSizeY >> 1) - (ScreenSizeY >> 1);

	x = px + dx;
	y = py + dy;

	sx = ScreenSizeX;
	sy = ScreenSizeY;
}

void invItem::init_fname(char* p)
{
	int sz = strlen(p) + 1;
	fname = new char[sz + 1];
	strcpy(fname, p);
}

void invItem::clone(invItem* p)
{
	p -> menu = menu;
	p -> pData = pData;
	p -> promptData = promptData;
	p -> pTemplate = pTemplate;

	p -> ID_ptr = ID_ptr;
	p -> classID = classID;

	p -> ID = ID;

	p -> slotType = slotType;

	p -> EvCode = EvCode;

	p -> flags = flags;

	p -> ScreenSizeX = ScreenSizeX;
	p -> ScreenSizeY = ScreenSizeY;

	p -> fname = fname;
	p -> frame = frame;

	p -> ShapeLen = ShapeLen;
	p -> ShapeX = ShapeX;
	p -> ShapeY = ShapeY;

	p -> ShapeSizeX = ShapeSizeX;
	p -> ShapeSizeY = ShapeSizeY;

	p -> ShapeCenterX = ShapeCenterX;
	p -> ShapeCenterY = ShapeCenterY;

	p -> NumIndex = NumIndex;

	p -> numComments = numComments;
	p -> comments = comments;

	p -> numAviIDs = numAviIDs;
	p -> avi_ids = avi_ids;

	p -> partData = partData;

	p -> flags |= INV_CLONE;
}

void invItem::init_name(char* p)
{
	init_id(p);
}

void invItem::load_frame(void)
{
	XStream fh;

	short sx,sy;
	if(!(flags & AS_ITEM_DATA_LOADED) && fname){
		flags |= AS_ITEM_DATA_LOADED;
		fh.open(fname,XS_IN);

		fh > sx > sy;
		ScreenSizeX = sx;
		ScreenSizeY = sy;
		frame = new char[ScreenSizeX * ScreenSizeY];
		fh.read(frame,ScreenSizeX * ScreenSizeY);
	}
}

void invItem::free_frame(void)
{
	if(flags & AS_ITEM_DATA_LOADED){
		flags &= ~AS_ITEM_DATA_LOADED;
		delete[] frame;
	}
}

invMatrixCell::invMatrixCell(void)
{
	type = AS_NO_CELL;
	flags = 0;

	slotType = slotNumber = -1;

	item = NULL;
}

void invMatrixCell::init_mem(void)
{
	type = AS_NO_CELL;
	flags = 0;

	slotType = slotNumber = -1;

	item = NULL;
}

invMatrix::invMatrix(void)
{
	anchor = 0;
	type = 0;
	flags = 0;

	internalID = 0;

	maxLoad = 1;

	SizeX = SizeY = 0;
	ScreenX = ScreenY = 0;
	ScreenSizeX = ScreenSizeY = 0;

	pData = NULL;
	back = NULL;

	items = new iList;

	matrix = NULL;
	mech_name = NULL;

	numAviIDs = 0;
	avi_ids = NULL;

	uvsDataPtr = NULL;
}

invMatrix::invMatrix(int sx,int sy)
{
	type = 0;
	flags = 0;

	SizeX = sx;
	SizeY = sy;

	ScreenX = ScreenY = 0;
	ScreenSizeX = ScreenSizeY = 0;

	back = NULL;
	mech_name = NULL;
	pData = NULL;

	items = new iList;
	alloc_matrix();

	numAviIDs = 0;
	avi_ids = NULL;

	uvsDataPtr = NULL;
}

invMatrix::~invMatrix(void)
{
	int i;
	invItem* p,*p1;

	p = (invItem*)items -> last;
	while(p){
		p1 = (invItem*)p -> prev;
		delete p;
		p = p1;
	}
	delete items;

	if(!(flags & IM_CLONE)){
		if(back)
			delete back;
		if(mech_name)
			delete[] mech_name;
		if(pData)
			delete[] pData;

		if(numAviIDs){
			for(i = 0; i < numAviIDs; i ++)
				if(avi_ids[i]) delete[] avi_ids[i];
			delete[] avi_ids;
		}
	}
	free_matrix();

	type = 0;
	flags = 0;

	SizeX = SizeY = 0;
	ScreenX = ScreenY = 0;
	ScreenSizeX = ScreenSizeY = 0;
}

void invMatrix::alloc_prm(void)
{
	pData = new char[IM_NUM_PARAMS * ACI_MAX_PRM_LEN];
	memset(pData,0,IM_NUM_PARAMS * ACI_MAX_PRM_LEN);
}

void invItem::alloc_prm(void)
{
	pData = new char[INV_ITEM_NUM_PARAMS * ACI_MAX_PRM_LEN];
	memset(pData,0,INV_ITEM_NUM_PARAMS * ACI_MAX_PRM_LEN);
}

void invMatrix::alloc_matrix(void)
{
	int i,sz = SizeX * SizeY;
	matrix = new invMatrixCell*[sz];
	matrix_heap = new char[sz * sizeof(invMatrixCell)];
	for(i = 0; i < sz; i ++){
		matrix[i] = (invMatrixCell*)(matrix_heap + i * sizeof(invMatrixCell));
		matrix[i] -> init_mem();
	}
}

int invMatrix::slot_exist(int id)
{
	int i,sz = SizeX * SizeY;
	for(i = 0; i < sz; i ++){
		if(matrix[i] -> slotType == id) return 1;
	}
	return 0;
}

void invMatrix::free_matrix(void)
{
	delete[] matrix;
	delete[] matrix_heap;
}

int invMatrix::get_item_slot(invItem* p)
{
	return matrix[p -> MatrixX + p -> MatrixY * SizeX] -> slotNumber;
}

int invMatrix::check_fit(int x,int y,invItem* p)
{
	int i,offs = 0;

	int ix = x,iy = y,dl = p -> ShapeLen,tp = p -> slotType;
	int* dx = p -> ShapeX;
	int* dy = p -> ShapeY;

	for(i = 0; i < dl; i ++){
		ix = x + dx[i];
		iy = y + dy[i];

		if((y & 0x01) && (dy[i] & 0x01))
			ix ++;

		if(ix < 0 || iy < 0 || ix >= SizeX || iy >= SizeY)
			return 0;

		offs = ix + iy * SizeX;

		if(matrix[offs] -> type == AS_NO_CELL || (tp != matrix[offs] -> slotType && matrix[offs] -> slotType != -1) || (matrix[offs] -> flags & AS_BUSY_CELL))
			return 0;
	}
	return 1;
}

int invMatrix::auto_put_item(invItem* p)
{
	int x,y,offs = 0;
	for(y = 0; y < SizeY; y ++){
		for(x = 0; x < SizeX; x ++){
			if(!(p -> flags & INV_ITEM_NO_ACTIVATE) || p -> slotType != matrix[offs] -> slotType){
				if(put_item(x,y,p))
					return 1;
			}
			offs ++;
		}
	}
	return 0;
}

int invMatrix::put_item(int x,int y,invItem* p,int mode)
{
	int i,offs = 0,ix,iy;
	if(!check_fit(x,y,p))
		return 0;

	p -> MatrixX = x;
	p -> MatrixY = y;

	ix = x;
	iy = y;

	for(i = 0; i < p -> ShapeLen; i ++){
		ix = x + p -> ShapeX[i];
		iy = y + p -> ShapeY[i];

		if((y & 0x01) && (p -> ShapeY[i] & 0x01))
			ix ++;

		offs = ix + iy * SizeX;
		matrix[offs] -> put_item(p);
	}
	
	items -> connect((iListElement*)p);
	if(!mode)
		p -> dropCount = items -> Size;
	if(p -> slotType != -1)
		fill(p);
	return 1;
}

int invMatrix::put_item_shadow(int x,int y,invItem* p)
{
	int i,offs = 0,ix,iy;

	ix = x;
	iy = y;

	for(i = 0; i < p -> ShapeLen; i ++){
		ix = x + p -> ShapeX[i];
		iy = y + p -> ShapeY[i];

		if((y & 0x01) && (p -> ShapeY[i] & 0x01))
			ix ++;

		offs = ix + iy * SizeX;
		matrix[offs] -> flags |= AS_IN_SHADOW;
	}
	return 1;
}

void invMatrix::fill(invItem* p)
{
	int x,y,flag = 1,index;
	invMatrixCell* ic,*ic1;

	while(flag){
		index = 0;
		flag = 0;
		for(y = 0; y < SizeY; y ++){
			for(x = 0; x < SizeX; x ++){
				ic = matrix[index];
				if(ic -> flags & AS_BUSY_CELL && ic -> item == p && ic -> slotType != -1){
					if(x){
						ic1 = matrix[index - 1];
						if(ic1 -> slotNumber == ic -> slotNumber && !(ic1 -> flags & AS_BUSY_CELL)){
							flag = 1;
							ic1 -> put_item(p);
						}
					}
					if(x < SizeX - 1){
						ic1 = matrix[index + 1];
						if(ic1 -> slotNumber == ic -> slotNumber && !(ic1 -> flags & AS_BUSY_CELL)){
							flag = 1;
							ic1 -> put_item(p);
						}
					}
					if(y){
						ic1 = matrix[index - SizeX];
						if(ic1 -> slotNumber == ic -> slotNumber && !(ic1 -> flags & AS_BUSY_CELL)){
							flag = 1;
							ic1 -> put_item(p);
						}
					}
					if(y < SizeY - SizeX){
						ic1 = matrix[index + SizeX];
						if(ic1 -> slotNumber == ic -> slotNumber && !(ic1 -> flags & AS_BUSY_CELL)){
							flag = 1;
							ic1 -> put_item(p);
						}
					}
				}
				index ++;
			}
		}
	}
}

void invMatrix::remove_item(invItem* p,int mode)
{
	int i, j, index = 0;
	invItem* itm;

	for(i = 0; i < SizeY; i ++){
		for(j = 0; j < SizeX; j ++){
			if(matrix[index] -> flags & AS_BUSY_CELL && matrix[index] -> item == p)
				matrix[index] -> remove_item();
			index ++;
		}
	}
	items -> dconnect((iListElement*)p);

	if(!mode){
		itm = (invItem*)items -> last;
		while(itm){
			if(itm -> dropCount > p -> dropCount)
				itm -> dropCount --;
			itm = (invItem*)itm -> prev;
		}
	}
}

invItem* invMatrix::get_item(int x,int y)
{
	int index = x + y * SizeX;
	if(matrix[index] -> flags & AS_BUSY_CELL)
		return matrix[index] -> item;
	else
		return NULL;
}

invItem* invMatrix::get_area_item(int x,int y,invItem* it)
{
	int i,index,ix,iy;
	invItem* p = NULL;

	for(i = 0; i < it -> ShapeLen; i ++){
		ix = x + it -> ShapeX[i];
		iy = y + it -> ShapeY[i];

		if((y & 0x01) && (it -> ShapeY[i] & 0x01))
			ix ++;

		if(ix < 0 || ix >= SizeX || iy < 0 || iy >= SizeY)
			return NULL;

		index = ix + iy * SizeX;
		if(matrix[index] -> flags & AS_BUSY_CELL){
			if(!p){
				p = matrix[index] -> item;
			}
			else {
				if(matrix[index] -> item != p){
					return NULL;
				}
			}
		}
	}
	return p;
}

void invMatrixCell::put_item(invItem* p)
{
	item = p;
	flags |= AS_BUSY_CELL;
}

void invMatrixCell::remove_item(void)
{
	item = NULL;
	flags &= ~AS_BUSY_CELL;
}

aButton::aButton(void)
{
	anchor = 0;
	fname = NULL;
	frameSeq = NULL;

	ID = 0;
	ControlID = 0;

	type = flags = 0;
	PosX = PosY = 0;
	SizeX = SizeY = 0;

	promptData = NULL;

	curCount = activeCount = 0;

	eventCode = 0;
	eventData = 0;

	numFrames = 0;
	cur_frame = 0;

	scankey = new aKeyObj;
}

aButton::~aButton(void)
{
	delete scankey;

	if(fname) {
		delete[] fname;
	}

	if(frameSeq) {
		delete[] frameSeq;
	}

	if(promptData) {
		delete[] promptData;
	}

	type = flags = 0;
	PosX = PosY = 0;
	SizeX = SizeY = 0;

	numFrames = 0;
	cur_frame = 0;
}

void aButton::press(void)
{
	set_redraw();
	if(!(flags & B_ACTIVE)){
		curCount = 0;
		flags |= B_ACTIVE;
	}
	if(flags & B_PRESSED){
		cur_frame --;
		if(cur_frame <= 0){
			cur_frame = 0;
			flags ^= B_ACTIVE;
			flags ^= B_PRESSED;
		}
	}
	else {
		cur_frame ++;
		if(cur_frame >= numFrames){
			cur_frame = numFrames - 1;
			if(!(flags & B_UNPRESS))
				flags ^= B_ACTIVE;
			flags ^= B_PRESSED;
		}
	}
}

void aButton::set_fname(char* p)
{
	int sz = strlen(p) + 1;
	if(fname)
		delete fname;
	fname = new char[sz + 1];
	strcpy(fname,p);
}

void aButton::init(void)
{
	int i,key;
	load_frames();
	if(ControlID){
		scankey -> free_list();
		for(i = 0; i < iKEY_OBJECT_SIZE; i ++){
			key = iGetControlCode(ControlID,i);
			if(key) scankey -> add_key(key);
		}
	}
}

void aButton::load_frames(void)
{
	int sz;
	short sx,sy,s;
	XStream fh;

	if(!(flags & B_FRAMES_LOADED)){
		fh.open(fname,XS_IN);
		fh > sx > sy > s;
		sz = sx * sy * s;

		SizeX = sx;
		SizeY = sy;

		numFrames = s;

		frameSeq = new char[sz];
		fh.read(frameSeq,sz);
		fh.close();

		flags |= B_FRAMES_LOADED;

		layout(this, XGR_MAXX, XGR_MAXY);
	}
}

void aButton::free_frames(void)
{
	if(flags & B_FRAMES_LOADED){
		delete[] frameSeq;
		flags ^= B_FRAMES_LOADED;
	}
}

void aButton::finit(void)
{
	free_frames();
}

void actIntDispatcher::add_locdata(aciLocationInfo* p)
{
	locationList -> connect(p);
}

void actIntDispatcher::add_menu(fncMenu* p)
{
	menuList -> connect((iListElement*)p);
}

void actIntDispatcher::add_imenu(fncMenu* p)
{
	i_menuList -> connect((iListElement*)p);
}

void actIntDispatcher::add_ind(aIndData* p)
{
	indList -> connect((iListElement*)p);
}

void actIntDispatcher::add_item(invItem* p)
{
	itemList -> connect((iListElement*)p);
}

void actIntDispatcher::add_iitem(invItem* p)
{
	i_itemList -> connect((iListElement*)p);
}

void actIntDispatcher::add_matrix(invMatrix* m)
{
	matrixList -> connect((iListElement*)m);
}

void actIntDispatcher::add_imatrix(invMatrix* m)
{
	i_matrixList -> connect((iListElement*)m);
}

fncMenuItem::fncMenuItem(void)
{
	flags = 0;
	fnc_code = -1;
	scankey = new aKeyObj;

	PosX = PosY = 0;
	SizeX = SizeY = 0;
	font = 0;

	space = I_STR_SPACE;
	submenuID = -1;

	name = NULL;
	name_len = 0;

	eventPtr = NULL;
}

fncMenuItem::~fncMenuItem(void)
{
	if(!(flags & FM_CLONE)){
		delete scankey;

		if(eventPtr)
			delete eventPtr;

		if(name)
			delete[] name;
	}
}

void fncMenuItem::add_key(int key)
{
	scankey -> add_key(key);
}

void aButton::add_key(int key)
{
	scankey -> add_key(key);
}

void fncMenuItem::init_name(const char* p)
{
	int sz = strlen(p) + 1;

	if(!name_len || sz > name_len){
		if(name_len) delete name;
		name = new char[sz];
		name_len = sz;
	}
	strcpy(name,p);
}

fncMenu::fncMenu(void)
{
	anchor = 0;
	ibs = NULL;
	bml = NULL;
	bml_name = NULL;
	ibs_name = NULL;

	upMenu = subMenu = NULL;
	upMenuItem = NULL;

	fncCode = -1;

	prefix = NULL;
	PrefixX = PrefixY = 0;
	PrefixDelta = 0;

	postfix = NULL;

	iScreenOwner = NULL;

	flags = 0;
	vSpace = I_STR_SPACE;

	bCol = -1;

	VItems = FM_NUM_V_ITEMS;
	itemY = 0;

	activeCount = 0;
	curCount = 0;

	type = 0;
	curFunction = 0;

	scankey = new aKeyObj;
	up_key = new aKeyObj;
	down_key = new aKeyObj;

	up_obj = new fncMenuItem;
	down_obj = new fncMenuItem;

	up_obj -> init_name("\x1E");
	down_obj -> init_name("\x1F");

	trigger = NULL;
	trigger_code = -1;

	items = new iList;
	inactive_items = new iList;

	PosX = PosY = 0;
	SizeX = SizeY = 0;

	firstItem = NULL;
	curItem = NULL;
}

fncMenu::~fncMenu(void)
{
	fncMenuItem* p,*p1;

	if(!(flags & FM_SUBMENU)){
		if(ibs) delete ibs;
		if(bml) delete bml;
		if(bml_name) delete[] bml_name;
		if(ibs_name) delete[] ibs_name;
	}

	if(upMenuItem) delete upMenuItem;

	delete scankey;
	delete up_key;
	delete down_key;

	delete up_obj;
	delete down_obj;

	p = (fncMenuItem*)items -> last;
	while(p){
		p1 = (fncMenuItem*)p -> prev;
		if(!(p -> flags & FM_NO_DELETE))
			delete p;
		p = p1;
	}
	delete items;
	p = (fncMenuItem*)inactive_items -> last;
	while(p){
		p1 = (fncMenuItem*)p -> prev;
		if(!(p -> flags & FM_NO_DELETE))
			delete p;
		p = p1;
	}
	delete inactive_items;
	flags = 0;
}

InfoPanel::InfoPanel(void)
{
	anchor = 0;
	ibs = NULL;
	bml = NULL;
	bml_name = NULL;
	ibs_name = NULL;

	bCol = -1;

	iScreenOwner = NULL;

	interf_type = INF_PANEL;

	MaxStr = 3;

	SizeX = SizeY = 0;
	OffsX = OffsY = 0;

	flags = 0;
	vSpace = I_STR_SPACE;
	hSpace = I_STR_SPACE;

	font = 0;
	type = 0;

	items = new iList;
	item_pool = new iList;
}

CounterPanel::CounterPanel(void)
{
	anchor = 0;
	ibs = NULL;
	ibs_name = NULL;

	ID = CREDITS_COUNTER;

	iScreenOwner = NULL;

	color = 30;

	SizeX = SizeY = 0;

	MaxLen = 6;

	last_value = -1;

	PosX = PosY = 0;
	SizeX = SizeY = 0;

	flags = 0;
	hSpace = I_STR_SPACE;

	font = 0;

	xconv = new XBuffer;

	type = CP_INT;
}

InfoPanel::~InfoPanel(void)
{
	if(ibs) delete ibs;
	if(bml) delete bml;
	if(bml_name) delete bml_name;
	if(ibs_name) delete[] ibs_name;

	InfoPanelItem* p,*p1;

	p = (InfoPanelItem*)items -> last;
	while(p){
		p1 = (InfoPanelItem*)p -> prev;
		delete p;
		p = p1;
	}
	delete items;

	p = (InfoPanelItem*)item_pool -> last;
	while(p){
		p1 = (InfoPanelItem*)p -> prev;
		delete p;
		p = p1;
	}
	delete item_pool;

	flags = 0;
}

CounterPanel::~CounterPanel(void)
{
	if(ibs) delete ibs;
	if(ibs_name) delete[] ibs_name;

	delete xconv;

	flags = 0;
}

void InfoPanel::add_items(int num,char** ptr,int fnt,int col)
{
	int i;
	for(i = 0; i < num; i ++){
		add_item(ptr[i],fnt,col);
	}
}

void InfoPanel::add_item(const char* ptr,int fnt,int col)
{
	InfoPanelItem* p;
	if(item_pool -> Size){
		p = (InfoPanelItem*)item_pool -> last;
		item_pool -> dconnect((iListElement*)p);
	}
	else {
		p = new InfoPanelItem;
	}
	p -> init_id(ptr);
	p -> font = fnt;
	p -> color = col;
	items -> connect((iListElement*)p);
}

void fncMenu::add_key(int key)
{
	scankey -> add_key(key);
}

void fncMenu::add_item(fncMenuItem* p)
{
	items -> connect((iListElement*)p);
	if(flags & FM_RANGE_FONT) p -> flags |= FM_RANGE_FONT;
}

void invMatrix::redraw_matrix(void)
{
	int i,j,x0,y0,x = 0,y = 0,col = 0,col1 = 0,index = 0;

	int dx = aCellSize;
	int dx2 = aCellSize >> 1;
	int dx4 = aCellSize >> 2;

	int ssx = ScreenSizeX + aCellSize;
	int ssy = ScreenSizeY + aCellSize;
	int ssz = ssx * ssy;

	unsigned char* p,*scr_buf;
	p = new unsigned char[(dx + 1) * (dx + 1)];

	if(aciBufRedrawFlag){
		scr_buf = new unsigned char[ssz];
		memset(scr_buf,0,ssz);
	}

	for(i = 0; i < SizeY; i ++){
		if(i & 0x01){
			x += dx2;
		}
		for(j = 0; j < SizeX; j ++){
			if(matrix[index] -> type && !(matrix[index] -> flags & AS_BUSY_CELL)){
				memcpy(p,aCellFrame,(dx + 1) * (dx + 1));

				if(matrix[index] -> slotType == -1){
					col = aciCurColorScheme[INV_FLOOR_COL_ITM];
					col1 = aciCurColorScheme[INV_CELL_COL_ITM];
				}
				else {
					col = aciCurColorScheme[matrix[index] -> slotType * 4 + INV_FLOOR_COL_ITM];
					col1 = aciCurColorScheme[matrix[index] -> slotType * 4 + INV_CELL_COL_ITM];
				}

				swap_buf_col(1,col1,dx + 1,dx + 1,p);
				swap_buf_col(2,col,dx + 1,dx + 1,p);

				x0 = x + ScreenX;
				y0 = y + ScreenY;

				if(aciBufRedrawFlag)
					mem_putspr_h(x,y,dx + 1,dx + 1,ssx,ssy,p,scr_buf);
				else
					XGR_PutSpr(x0,y0,dx + 1,dx + 1,p,XGR_HIDDEN_FON);
			}
			x += aCellSize;
			index ++;
		}
		x = 0;
		y += aCellSize - dx4;
	}

	x = 0;
	y = 0;
	index = 0;

	for(i = 0; i < SizeY; i ++){
		if(i & 0x01){
			x += dx2;
		}
		for(j = 0; j < SizeX; j ++){
			if(matrix[index] -> type && (matrix[index] -> flags & AS_BUSY_CELL)){
				memcpy(p,aCellFrame,(dx + 1) * (dx + 1));

				if(matrix[index] -> slotType == -1){
					col = aciCurColorScheme[INV_BFLOOR_COL_ITM];
					col1 = aciCurColorScheme[INV_BCELL_COL_ITM];
				}
				else {
					col = aciCurColorScheme[matrix[index] -> slotType * 4 + INV_BFLOOR_COL_ITM];
					col1 = aciCurColorScheme[matrix[index] -> slotType * 4 + INV_BCELL_COL_ITM];
				}

				swap_buf_col(1,col1,dx + 1,dx + 1,p);
				swap_buf_col(2,col,dx + 1,dx + 1,p);

				x0 = x + ScreenX;
				y0 = y + ScreenY;

				if(aciBufRedrawFlag)
					mem_putspr_h(x,y,dx + 1,dx + 1,ssx,ssy,p,scr_buf);
				else
					XGR_PutSpr(x0,y0,dx + 1,dx + 1,p,XGR_HIDDEN_FON);
			}
			x += aCellSize;
			index ++;
		}
		x = 0;
		y += aCellSize - dx4;
	}

	delete[] p;
	if(aciBufRedrawFlag){
		put_buf2col(800 + ScreenX,ScreenY,ssx,ssy,scr_buf,300,0);
		delete[] scr_buf;
	}
}

#define SHADOW_COL	103
void invMatrix::redraw_shadow_cells(void)
{
	int i,j,x0,y0,x = 0,y = 0,col = 0,col1 = 0,index = 0;

	int dx = aCellSize;
	int dx2 = aCellSize >> 1;
	int dx4 = aCellSize >> 2;

	unsigned char* p;
	p = new unsigned char[(dx + 1) * (dx + 1)];

	for(i = 0; i < SizeY; i ++){
		if(i & 0x01){
			x += dx2;
		}
		for(j = 0; j < SizeX; j ++){
			if(matrix[index] -> type && !(matrix[index] -> flags & AS_IN_SHADOW) && (matrix[index] -> flags & AS_REDRAW_CELL) && !(matrix[index] -> flags & AS_BUSY_CELL)){
				memcpy(p,aCellFrame,(dx + 1) * (dx + 1));

				if(matrix[index] -> slotType == -1){
					col = aciCurColorScheme[INV_FLOOR_COL_ITM];
					col1 = aciCurColorScheme[INV_CELL_COL_ITM];
				}
				else {
					col = aciCurColorScheme[matrix[index] -> slotType * 4 + INV_FLOOR_COL_ITM];
					col1 = aciCurColorScheme[matrix[index] -> slotType * 4 + INV_CELL_COL_ITM];
				}

				swap_buf_col(1,col1,dx + 1,dx + 1,p);
				swap_buf_col(2,col,dx + 1,dx + 1,p);

				x0 = x + ScreenX;
				y0 = y + ScreenY;

				if(aciBufRedrawFlag)
					put_buf2col(800 + x0,y0,dx + 1,dx + 1,p,300,0);
				else
					XGR_PutSpr(x0,y0,dx + 1,dx + 1,p,XGR_HIDDEN_FON);
			}
			x += aCellSize;
			index ++;
		}
		x = 0;
		y += aCellSize - dx4;
	}

	x = 0;
	y = 0;
	index = 0;

	for(i = 0; i < SizeY; i ++){
		if(i & 0x01){
			x += dx2;
		}
		for(j = 0; j < SizeX; j ++){
			if(matrix[index] -> type && !(matrix[index] -> flags & AS_IN_SHADOW) && (matrix[index] -> flags & AS_REDRAW_CELL) && (matrix[index] -> flags & AS_BUSY_CELL)){
				memcpy(p,aCellFrame,(dx + 1) * (dx + 1));

				if(matrix[index] -> slotType == -1){
					col = aciCurColorScheme[INV_BFLOOR_COL_ITM];
					col1 = aciCurColorScheme[INV_BCELL_COL_ITM];
				}
				else {
					col = aciCurColorScheme[matrix[index] -> slotType * 4 + INV_BFLOOR_COL_ITM];
					col1 = aciCurColorScheme[matrix[index] -> slotType * 4 + INV_BCELL_COL_ITM];
				}

				swap_buf_col(1,col1,dx + 1,dx + 1,p);
				swap_buf_col(2,col,dx + 1,dx + 1,p);

				x0 = x + ScreenX;
				y0 = y + ScreenY;

				matrix[index] -> item -> flags |= INV_ITEM_REDRAW;

				if(aciBufRedrawFlag)
					put_buf2col(800 + x0,y0,dx + 1,dx + 1,p,300,0);
				else
					XGR_PutSpr(x0,y0,dx + 1,dx + 1,p,XGR_HIDDEN_FON);
			}
			x += aCellSize;
			index ++;
		}
		x = 0;
		y += aCellSize - dx4;
	}

	x = 0;
	y = 0;
	index = 0;

	for(i = 0; i < SizeY; i ++){
		if(i & 0x01){
			x += dx2;
		}
		for(j = 0; j < SizeX; j ++){
			if(matrix[index] -> type && (matrix[index] -> flags & AS_IN_SHADOW) && !(matrix[index] -> flags & AS_BUSY_CELL)){
				matrix[index] -> flags &= ~AS_REDRAW_CELL;
				memcpy(p,aCellFrame,(dx + 1) * (dx + 1));

				if(matrix[index] -> slotType == -1){
					col = aciCurColorScheme[INV_FLOOR_COL_ITM];
				}
				else {
					col = aciCurColorScheme[matrix[index] -> slotType * 4 + INV_FLOOR_COL_ITM];
				}
				col1 = aciCurColorScheme[INV_CELL_SELECT_COL];

				swap_buf_col(1,col1,dx + 1,dx + 1,p);
				swap_buf_col(2,col,dx + 1,dx + 1,p);

				x0 = x + ScreenX;
				y0 = y + ScreenY;

				if(matrix[index] -> flags & AS_BUSY_CELL)
					matrix[index] -> item -> flags |= INV_ITEM_REDRAW;

				if(aciBufRedrawFlag)
					put_buf2col(800 + x0,y0,dx + 1,dx + 1,p,300,0);
				else
					XGR_PutSpr(x0,y0,dx + 1,dx + 1,p,XGR_HIDDEN_FON);
			}
			x += aCellSize;
			index ++;
		}
		x = 0;
		y += aCellSize - dx4;
	}

	x = 0;
	y = 0;
	index = 0;

	for(i = 0; i < SizeY; i ++){
		if(i & 0x01){
			x += dx2;
		}
		for(j = 0; j < SizeX; j ++){
			if(matrix[index] -> type && (matrix[index] -> flags & AS_IN_SHADOW) && (matrix[index] -> flags & AS_BUSY_CELL)){
				matrix[index] -> flags &= ~AS_REDRAW_CELL;
				memcpy(p,aCellFrame,(dx + 1) * (dx + 1));

				if(matrix[index] -> slotType == -1){
					col = aciCurColorScheme[INV_BFLOOR_COL_ITM];
				}
				else {
					col = aciCurColorScheme[matrix[index] -> slotType * 4 + INV_BFLOOR_COL_ITM];
				}
				col1 = aciCurColorScheme[INV_CELL_SELECT_COL];

				swap_buf_col(1,col1,dx + 1,dx + 1,p);
				swap_buf_col(2,col,dx + 1,dx + 1,p);

				x0 = x + ScreenX;
				y0 = y + ScreenY;

				if(matrix[index] -> flags & AS_BUSY_CELL)
					matrix[index] -> item -> flags |= INV_ITEM_REDRAW;

				if(aciBufRedrawFlag)
					put_buf2col(800 + x0,y0,dx + 1,dx + 1,p,300,0);
				else
					XGR_PutSpr(x0,y0,dx + 1,dx + 1,p,XGR_HIDDEN_FON);
			}
			x += aCellSize;
			index ++;
		}
		x = 0;
		y += aCellSize - dx4;
	}

	x = 0;
	y = 0;
	index = 0;

	for(i = 0; i < SizeY; i ++){
		if(i & 0x01){
			x += dx2;
		}
		for(j = 0; j < SizeX; j ++){
			if(matrix[index] -> type && ((matrix[index] -> flags & AS_IN_SHADOW) || (matrix[index] -> flags & AS_REDRAW_CELL))){
				matrix[index] -> flags &= ~AS_REDRAW_CELL;

				x0 = x + ScreenX;
				y0 = y + ScreenY;

				if(aciBufRedrawFlag)
					put_map(800 + x0,y0,dx + 1,dx + 1);
				else
					XGR_Flush(x0,y0,dx + 1,dx + 1);
			}
			x += aCellSize;
			index ++;
		}
		x = 0;
		y += aCellSize - dx4;
	}

	delete[] p;

	invItem* itm = (invItem*)items -> last;
	while(itm){
		if(itm -> flags & INV_ITEM_REDRAW){
			itm -> redraw(ScreenX,ScreenY,1,1);
			itm -> flags &= ~INV_ITEM_REDRAW;
		}
		itm = (invItem*)itm -> prev;
	}
}

void invMatrix::clear_shadow_cells(void)
{
	int i,j,index = 0;
	for(i = 0; i < SizeY; i ++){
		for(j = 0; j < SizeX; j ++){
			if(matrix[index] -> type && (matrix[index] -> flags & AS_IN_SHADOW)){
				matrix[index] -> flags ^= AS_IN_SHADOW;
				matrix[index] -> flags |= AS_REDRAW_CELL;
			}
			index ++;
		}
	}
}

void invMatrix::redraw_items(void)
{
	invItem* p = (invItem*)items -> last;
	
	while(p){
		p -> redraw(ScreenX,ScreenY,1);
		p = (invItem*)p -> prev;
	}
}

void invMatrix::get_item_coords(invItem* p,int& x,int &y,int &sx,int &sy)
{
	int _x,_y,_sx,_sy;
	p -> get_coords(ScreenX,ScreenY,_x,_y,_sx,_sy);

	x = _x;
	y = _y;
	sx = _sx;
	sy = _sy;
}

void invMatrix::redraw(void)
{
	redraw_matrix();
	redraw_items();

	if(aciBufRedrawFlag)
		put_map(800 + ScreenX,ScreenY,ScreenSizeX,ScreenSizeY);

	flags &= ~IM_REDRAW;
}

void CounterPanel::flush(void)
{
	if(iScreenOwner) return;

	if(ibs)
		XGR_Flush(ibs -> PosX,ibs -> PosY,ibs -> SizeX,ibs -> SizeY);
	else
		XGR_Flush(PosX,PosY,SizeX,SizeY);

	flags &= ~CP_FLUSH;
}

void invMatrix::flush(void)
{
	int x;
#ifdef _ACI_BOUND_TEST_
	XGR_Rectangle(ScreenX,ScreenY,ScreenSizeX,ScreenSizeY,aciCurColorScheme[INV_NUM_ITM_COL],aciCurColorScheme[INV_NUM_ITM_COL],XGR_OUTLINED);
#endif
	if(aciBufRedrawFlag){
		x = ScreenX - iScreenOffs + 800;
		XGR_Flush(x,ScreenY,ScreenSizeX,ScreenSizeY);
	}
	else
		XGR_Flush(ScreenX,ScreenY,ScreenSizeX,ScreenSizeY);

	flags &= ~IM_FLUSH;
}

void aIndData::flush(int dx,int dy)
{
	int x,y;
	if(dx == -1){
		XGR_Flush(PosX,PosY,SizeX,SizeY);
	}
	else {
		switch(CornerNum){
			case IND_UP_LEFT:
				x = dx;
				y = dy;
				break;
			case IND_UP_RIGHT:
				x = XGR_MAXX - dx - SizeX;
				y = dy;
				break;
			case IND_DN_RIGHT:
				x = XGR_MAXX - dx - SizeX;
				y = XGR_MAXY - dy - SizeY;
				break;
			case IND_DN_LEFT:
				x = dx;
				y = XGR_MAXY - dy - SizeY;
				break;
		}
		XGR_Flush(x,y,SizeX,SizeY);
	}
	flags &= ~IND_FLUSH;
}

void invMatrix::init(void)
{
	int i,sz = SizeX * SizeY;
	if(!ScreenSizeX) ScreenSizeX = SizeX * aCellSize + (aCellSize >> 1) + 1;
	if(!ScreenSizeY) ScreenSizeY = SizeY * aCellSize - ((SizeY - 1) * (aCellSize >> 2)) + 1;
	maxLoad = 0;
	for(i = 0; i < sz; i ++){
		if(matrix[i] -> type != AS_NO_CELL) maxLoad ++;
	}
}

void aButton::redraw(void)
{
	int index = cur_frame * SizeX * SizeY;
	char* ptr = frameSeq + index;

	XGR_PutSpr(PosX,PosY,SizeX,SizeY,ptr,XGR_HIDDEN_FON);
	flags &= ~B_REDRAW;
}

void actIntDispatcher::ind_redraw(void)
{
	aIndData* p;

	if(flags & AS_FULLSCR)
		return;

	p = (aIndData*)indList -> last;
	while(p){
		p -> redraw();
		p = (aIndData*)p -> prev;
	}
}

void actIntDispatcher::redraw(void)
{
	fncMenu* p;
	aButton* b;
	invMatrix* m;
	CounterPanel* cp;
	InfoPanel* iPl;

	if(flags & AS_FULLSCR)
		return;

	aci_tmp ++;
	if(aci_tmp > 64) aci_tmp = 0;

	iPl = (InfoPanel*)infoPanels -> last;
	while(iPl){
		if(!(iPl -> flags & IP_NO_REDRAW)){
			if(iPl -> interf_type == INT_PANEL){
				iPl -> set_redraw();
			}
			else {
				switch(curMode){
					case AS_INFO_MODE:
						if(iPl -> interf_type == INF_PANEL){
							iPl -> set_redraw();
						}
						break;
					case AS_INV_MODE:
						if(iPl -> interf_type == INV_PANEL){
							iPl -> set_redraw();
						}
						break;
				}
			}
		}
		iPl = (InfoPanel*)iPl -> prev;
	}
	if(curMode == AS_INFO_MODE){
		p = (fncMenu*)menuList -> last;
		while(p){
			if(!(p -> flags & FM_ACTIVE) && p -> curFunction && !(p -> flags & FM_SUBMENU) && !(p -> flags & FM_HIDDEN))
				p -> set_redraw();

			if((p -> flags & FM_ACTIVE) && !p -> trigger){
				p -> curCount --;
				if(p -> curCount <= 0){
					if(!(p -> flags & FM_SUBMENU)){
						p -> flags &= ~FM_ACTIVE;
						p -> set_redraw();
						init_menus();
						init_submenu(p);
						p -> curCount = p -> activeCount;
						SOUND_SELECT();
					}
					else {
						if(p -> activeCount)
							p -> go2upmenu();
					}
				}
			}

			p = (fncMenu*)p -> prev;
		}
	}

	cp = (CounterPanel*)intCounters -> last;
	while(cp){
		if(*cp -> value_ptr != cp -> last_value)
			cp -> set_redraw();
		cp = (CounterPanel*)cp -> prev;
	}
	switch(curMode){
		case AS_INFO_MODE:
			cp = (CounterPanel*)infCounters -> last;
			while(cp){
				if(*cp -> value_ptr != cp -> last_value)
					cp -> set_redraw();
				cp = (CounterPanel*)cp -> prev;
			}
			break;
		case AS_INV_MODE:
			cp = (CounterPanel*)invCounters -> last;
			while(cp){
				if(*cp -> value_ptr != cp -> last_value)
					cp -> set_redraw();
				cp = (CounterPanel*)cp -> prev;
			}
			break;
	}
	if(!(flags & AS_FULL_REDRAW)){
		XGR_MouseObj.flags &= ~XGM_PROMPT_ACTIVE;
		XGR_Obj.fill(0);
		for(int i = 0; i < curIbs -> backs.size(); i++) {
			curIbs -> backs[i] -> show(0);
		}
		if(curMode == AS_INV_MODE && curMatrix && curMatrix -> back){
			curMatrix -> back -> show();
		}
		flags |= AS_FULL_FLUSH;
		cp = (CounterPanel*)intCounters -> last;
		while(cp){
			cp -> redraw();
			cp = (CounterPanel*)cp -> prev;
		}
		b = (aButton*)intButtons -> last;
		while(b){
			b -> redraw();
			b = (aButton*)b -> prev;
		}
		switch(curMode){
			case AS_INV_MODE:
				b = (aButton*)invButtons -> last;
				while(b){
					b -> redraw();
					b = (aButton*)b -> prev;
				}
				if(curMatrix){
					curMatrix -> redraw();
				}
				if(iP){
					iP -> redraw();
				}
				cp = (CounterPanel*)invCounters -> last;
				while(cp){
					cp -> redraw();
					cp = (CounterPanel*)cp -> prev;
				}
				break;
			case AS_INFO_MODE:
				b = (aButton*)infButtons -> last;
				while(b){
					b -> redraw();
					b = (aButton*)b -> prev;
				}
				p = (fncMenu*)menuList -> last;
				while(p){
					if(!(p -> flags & FM_SUBMENU) || (p -> flags & FM_ACTIVE))
						p -> redraw();
					else
						p -> flags &= ~FM_REDRAW;

					p = (fncMenu*)p -> prev;
				}
				cp = (CounterPanel*)infCounters -> last;
				while(cp){
					cp -> redraw();
					cp = (CounterPanel*)cp -> prev;
				}
				break;
		}
		flags |= AS_FULL_REDRAW;
	}
	b = (aButton*)intButtons -> last;
	while(b){
		if(b -> flags & B_REDRAW){
			b -> redraw();
			b -> set_flush();
		}
		b = (aButton*)b -> prev;
	}
	if(curMode == AS_INV_MODE){
		b = (aButton*)invButtons -> last;
		while(b){
			if(b -> flags & B_REDRAW){
				b -> redraw();
				b -> set_flush();
			}
			b = (aButton*)b -> prev;
		}
	}

	if(curMode == AS_INFO_MODE){
		b = (aButton*)infButtons -> last;
		while(b){
			if(b -> flags & B_REDRAW){
				b -> redraw();
				b -> set_flush();
			}
			b = (aButton*)b -> prev;
		}
	}

	if(curMode == AS_INV_MODE && curMatrix){
		m = curMatrix;
		if(m -> flags & IM_REDRAW){
			m -> redraw();
			m -> set_flush();
		}
	}

	if(curMode == AS_INFO_MODE){
		p = (fncMenu*)menuList -> last;
		while(p){
			if(p -> flags & FM_REDRAW){
				p -> redraw();
				p -> set_flush();
			}
			p = (fncMenu*)p -> prev;
		}
	}
	if(iP && curMode == AS_INV_MODE){
		if(iP -> flags & IP_REDRAW){
			iP -> redraw();
			iP -> set_flush();
		}
	}
	cp = (CounterPanel*)intCounters -> last;
	while(cp){
		if(cp -> flags & CP_REDRAW){
			cp -> redraw();
			cp -> set_flush();
		}
		cp = (CounterPanel*)cp -> prev;
	}
	if(curMode == AS_INFO_MODE){
		cp = (CounterPanel*)infCounters -> last;
		while(cp){
			if(cp -> flags & CP_REDRAW){
				cp -> redraw();
				cp -> set_flush();
			}
			cp = (CounterPanel*)cp -> prev;
		}
	}
	if(curMode == AS_INV_MODE){
		cp = (CounterPanel*)invCounters -> last;
		while(cp){
			if(cp -> flags & CP_REDRAW){
				cp -> redraw();
				cp -> set_flush();
			}
			cp = (CounterPanel*)cp -> prev;
		}
	}

	iPl = (InfoPanel*)infoPanels -> last;
	while(iPl){
		if(iPl -> flags & IP_REDRAW){
			iPl -> redraw();
			iPl -> set_flush();
		}
		iPl = (InfoPanel*)iPl -> prev;
	}
}

void actIntDispatcher::text_redraw(){
	if(Pause > 1 && NetworkON){
		if(GameQuantReturnValue || acsQuant()){
			Pause = 0;
		}
		flags |= AS_FULL_FLUSH;
	}

	if(flags & AS_TEXT_MODE){
		ScrTextData -> redraw();
		ScrTextData -> Quant();
	}
	if(curPrompt -> NumStr){
		curPrompt -> redraw(0,0,XGR_MAXX,XGR_MAXY);
		curPrompt -> quant();
	}
	if(flags & AS_CHAT_MODE){
		iChatQuant();
	}
	else {
		if(aciRacingFlag){
			aciShowRacingPlace();
		}
	}
	if(Pause > 1 && NetworkON){
		if(GameQuantReturnValue || acsQuant()){
			Pause = 0;
		}
	}


}

void actIntDispatcher::i_redraw(void)
{
	fncMenu* p;
	CounterPanel* cp;
	InfoPanel* ip;

	if(flags & AS_ISCREEN_INV_MODE){
		cp = (CounterPanel*)i_Counters -> last;
		while(cp){
			if(*cp -> value_ptr != cp -> last_value)
				cp -> set_redraw();
			cp = (CounterPanel*)cp -> prev;
		}
	}

	if(!(flags & AS_FULL_REDRAW)){
		if(flags & AS_ISCREEN_INV_MODE){
			cp = (CounterPanel*)i_Counters -> last;
			while(cp){
				cp -> redraw();
				cp = (CounterPanel*)cp -> prev;
			}
			if(curMatrix){
				curMatrix -> redraw();
			}
			if(secondMatrix){
				secondMatrix -> redraw();
			}
			if(iP){
				iP -> redraw();
			}
			p = (fncMenu*)i_menuList -> last;
			while(p){
				if(!(p -> flags & FM_LOCATION_MENU) && p -> flags & FM_ACTIVE)
					p -> redraw();
				p = (fncMenu*)p -> prev;
			}
		}
		else {
			if(qMenu) qMenu -> redraw();
			p = (fncMenu*)i_menuList -> last;
			while(p){
				if(p -> flags & FM_LOCATION_MENU && p -> flags & FM_ACTIVE)
					p -> redraw();
				p = (fncMenu*)p -> prev;
			}
		}
		ip = (InfoPanel*)i_infoPanels -> last;
		while(ip){
			ip -> redraw();
			ip = (InfoPanel*)ip -> prev;
		}
		flags |= AS_FULL_REDRAW;
		flags |= AS_FULL_FLUSH;
	}

	if(flags & AS_ISCREEN_INV_MODE){
		if(curMatrix && curMatrix -> flags & IM_REDRAW){
			curMatrix -> redraw();
			curMatrix -> set_flush();
		}

		if(secondMatrix && secondMatrix -> flags & IM_REDRAW){
			secondMatrix -> redraw();
			secondMatrix -> set_flush();
		}

		if(iP){
			if(iP -> flags & IP_REDRAW){
				iP -> redraw();
				iP -> set_flush();
			}
		}
		cp = (CounterPanel*)i_Counters -> last;
		while(cp){
			if(cp -> flags & CP_REDRAW){
				cp -> redraw();
				cp -> set_flush();
			}
			cp = (CounterPanel*)cp -> prev;
		}
		p = (fncMenu*)i_menuList -> last;
		while(p){
			if(!(p -> flags & FM_LOCATION_MENU) && p -> flags & FM_ACTIVE){
				p -> redraw();
				p -> set_flush();
			}
			p = (fncMenu*)p -> prev;
		}
	}
	else {
		if(qMenu && qMenu -> flags & FMC_REDRAW) qMenu -> redraw();
		p = (fncMenu*)i_menuList -> last;
		while(p){
			if(p -> flags & FM_LOCATION_MENU && p -> flags & FM_ACTIVE){
				p -> redraw();
				p -> set_flush();
			}
			p = (fncMenu*)p -> prev;
		}
	}

	ip = (InfoPanel*)i_infoPanels -> last;
	while(ip){
		if(ip -> flags & IP_REDRAW){
			ip -> redraw();
			ip -> set_flush();
		}
		ip = (InfoPanel*)ip -> prev;
	}
}

void actIntDispatcher::flush(void)
{
	fncMenu* p;
	aButton* b;
	invMatrix* m;
	aIndData* ind;
	CounterPanel* cp;
	InfoPanel* iPl;
	int emode = ExclusiveLog ? XGR_EXCLUSIVE : 0;

	if(iKeyPressed(iKEY_EXIT)){
		i_slake_pal(actIntPal,32);
		acsHandleExtEvent(ACS_GLOBAL_EXIT);
		aciPrepareEndImage();
		GameQuantReturnValue = RTO_SHOW_IMAGE_ID;
		XGR_MouseHide();
		/*if(XGR_MAXX != 800){
			if(XGR_ReInit(800,600,emode)) ErrH.Abort("Error video initialization");
		}*/
		XGR_Flip();
		return;
	}

	if(flags & AS_FULLSCR){
		ind = (aIndData*)indList -> last;
		while(ind){
			ind -> redraw(IND_DATA_OFFS,IND_DATA_OFFS);
			ind -> flush(IND_DATA_OFFS,IND_DATA_OFFS);
			ind = (aIndData*)ind -> prev;
		}
		if(flags & AS_EVINCE_PALETTE){
			i_evince_pal(actIntPal,16);
			PalIterLock = 0;
			flags ^= AS_EVINCE_PALETTE;
		}
		return;
	}

	ind = (aIndData*)indList -> last;
	while(ind){
		ind -> redraw();
		if(!(flags & AS_FULL_FLUSH)) ind -> flush();
		ind = (aIndData*)ind -> prev;
	}


	if(flags & AS_FULL_FLUSH){
		flags &= ~AS_FULL_FLUSH;
		XGR_Flush(0,0,XGR_MAXX,XGR_MAXY);
	}

	cp = (CounterPanel*)intCounters -> last;
	while(cp){
		if(cp -> flags & CP_FLUSH){
			cp -> flush();
		}
		cp = (CounterPanel*)cp -> prev;
	}
	cp = (CounterPanel*)infCounters -> last;
	while(cp){
		if(cp -> flags & CP_FLUSH){
			cp -> flush();
		}
		cp = (CounterPanel*)cp -> prev;
	}
	cp = (CounterPanel*)invCounters -> last;
	while(cp){
		if(cp -> flags & CP_FLUSH){
			cp -> flush();
		}
		cp = (CounterPanel*)cp -> prev;
	}
	if(curMatrix){
		m = curMatrix;
		if(m -> flags & IM_FLUSH){
			m -> flush();
		}
		if(m -> flags & IM_REDRAW_SHADOW){
			m -> redraw_shadow_cells();
			m -> flags &= ~IM_REDRAW_SHADOW;
		}
	}

	p = (fncMenu*)menuList -> last;
	while(p){
		if(p -> flags & FM_FLUSH){
			p -> flush();
		}
		p = (fncMenu*)p -> prev;
	}

	b = (aButton*)intButtons -> last;
	while(b){
		if(b -> flags & B_FLUSH){
			b -> flush();
		}
		if(b -> flags & B_ACTIVE){
			b -> press();
		}
		if(b -> activeCount && b -> flags & B_PRESSED){
			if(b -> curCount >= b -> activeCount){
				b -> press();
				send_event(b -> eventCode,b -> eventData);
			}
			else
				b -> curCount ++;
		}
		b = (aButton*)b -> prev;
	}
	b = (aButton*)invButtons -> last;
	while(b){
		if(b -> flags & B_FLUSH){
			b -> flush();
		}
		if(b -> flags & B_ACTIVE){
			b -> press();
		}
		if(curMode == AS_INV_MODE){
			if(b -> activeCount && b -> flags & B_PRESSED){
				if(b -> curCount >= b -> activeCount){
					b -> press();
					send_event(b -> eventCode,b -> eventData);
				}
				else
					b -> curCount ++;
			}
		}
		b = (aButton*)b -> prev;
	}
	b = (aButton*)infButtons -> last;
	while(b){
		if(b -> flags & B_FLUSH){
			b -> flush();
		}
		if(b -> flags & B_ACTIVE){
			b -> press();
		}
		if(curMode == AS_INFO_MODE){
			if(b -> activeCount && b -> flags & B_PRESSED){
				if(b -> curCount >= b -> activeCount){
					b -> press();
					send_event(b -> eventCode,b -> eventData);
				}
				else
					b -> curCount ++;
			}
		}
		b = (aButton*)b -> prev;
	}
	if(iP && iP -> flags & IP_FLUSH)
		iP -> flush();

	iPl = (InfoPanel*)infoPanels -> last;
	while(iPl){
		if(iPl -> flags & IP_FLUSH){
			iPl -> flush();
		}
		iPl = (InfoPanel*)iPl -> prev;
	}

	if(flags & AS_EVINCE_PALETTE){
		i_evince_pal(actIntPal,16);
		PalIterLock = 0;
		flags ^= AS_EVINCE_PALETTE;
	}
}

void actIntDispatcher::pal_flush() {
	if(flags & AS_EVINCE_PALETTE){
		i_evince_pal(actIntPal,16);
		PalIterLock = 0;
		flags ^= AS_EVINCE_PALETTE;
	}
}

void actIntDispatcher::i_flush(void)
{
	invMatrix* m;
	CounterPanel* cp;

	if(!(flags & AS_ISCREEN_INV_MODE))
		return;

	if(flags & AS_FULL_FLUSH){
		flags &= ~AS_FULL_FLUSH;
		XGR_Flush(0,0,XGR_MAXX,XGR_MAXY);
	}

	cp = (CounterPanel*)i_Counters -> last;
	while(cp){
		if(cp -> flags & CP_FLUSH){
			cp -> flush();
		}
		cp = (CounterPanel*)cp -> prev;
	}

	if(curMatrix){
		m = curMatrix;
		if(m -> flags & IM_FLUSH){
			m -> flush();
		}
		if(m -> flags & IM_REDRAW_SHADOW){
			m -> redraw_shadow_cells();
			m -> flush();
			m -> flags &= ~IM_REDRAW_SHADOW;
		}
	}

	if(secondMatrix){
		m = secondMatrix;
		if(m -> flags & IM_FLUSH){
			m -> flush();
		}
		if(m -> flags & IM_REDRAW_SHADOW){
			m -> redraw_shadow_cells();
			m -> flush();
			m -> flags &= ~IM_REDRAW_SHADOW;
		}
	}
/*
	m = (invMatrix*)i_matrixList -> last;
	while(m){
		if(m -> flags & IM_FLUSH){
			m -> flush();
		}
		if(m -> flags & IM_REDRAW_SHADOW){
			m -> redraw_shadow_cells();
		}
		m = (invMatrix*)m -> prev;
	}
*/
	if(iP && iP -> flags & IP_FLUSH)
		iP -> flush();
}

invMatrix* actIntDispatcher::get_matrix(int tp)
{
	invMatrix* p = (invMatrix*)matrixList -> last;
	while(p){
		if(p -> type == tp)
			return p;
		p = (invMatrix*)p -> prev;
	}
	return NULL;
}

invMatrix* actIntDispatcher::get_imatrix(int tp)
{
	invMatrix* p = (invMatrix*)i_matrixList -> last;
	while(p){
		if(p -> type == tp)
			return p;
		p = (invMatrix*)p -> prev;
	}
	return NULL;
}

invItem* actIntDispatcher::get_item(int id)
{
	invItem* p = (invItem*)itemList -> last;
	while(p){
		if(p -> ID == id)
			return p;
		p = (invItem*)p -> prev;
	}
	return NULL;
}

aciLocationInfo* actIntDispatcher::get_locdata(int id)
{
	aciLocationInfo* p = (aciLocationInfo*)locationList -> last;
	while(p){
		if(p -> ID == id)
			return p;
		p = (aciLocationInfo*)p -> prev;
	}
	return NULL;
}

invItem* actIntDispatcher::get_iitem(int id)
{
	invItem* p = (invItem*)i_itemList -> last;
	while(p){
		if(p -> ID == id)
			return p;
		p = (invItem*)p -> prev;
	}
	return NULL;
}

aIndData* actIntDispatcher::get_ind(int id)
{
	aIndData* p = (aIndData*)indList -> last;
	while(p){
		if(p -> ID == id)
			return p;
		p = (aIndData*)p -> prev;
	}
	return NULL;
}

void actIntDispatcher::init(void)
{
	fncMenu* it;
	invItem* itm;
	invMatrix* p;
	aButton* b;
	aIndData* ind;
	CounterPanel* cp;
	InfoPanel* ip;
	aciBitmapMenu* bm;

	ibsObject* ibs;
	bmlObject* bml;

	aciCurColorScheme = aciColorSchemes[SCH_DEFAULT];
	if(iP) {
		layout(iP, XGR_MAXX, XGR_MAXY);
		iP -> init();
	}

	ip = (InfoPanel*)infoPanels -> last;
	while(ip){
		layout(ip, XGR_MAXX, XGR_MAXY);
		ip -> init();
		ip = (InfoPanel*)ip -> prev;
	}

	bm = (aciBitmapMenu*)b_menuList -> last;
	while(bm){
		bm -> init();
		bm = (aciBitmapMenu*)bm -> prev;
	}

	if(wMap) wMap -> init();

	aciSet_aMouse();

	ind = (aIndData*)indList -> last;
	while(ind){
		ind -> init();
		ind = (aIndData*)ind -> prev;
	}

	cp = (CounterPanel*)intCounters -> last;
	while(cp){
        layout(cp, XGR_MAXX, XGR_MAXY);
		cp -> init();
		cp = (CounterPanel*)cp -> prev;
	}
	cp = (CounterPanel*)infCounters -> last;
	while(cp){
        layout(cp, XGR_MAXX, XGR_MAXY);
		cp -> init();
		cp = (CounterPanel*)cp -> prev;
	}
	cp = (CounterPanel*)invCounters -> last;
	while(cp){
        layout(cp, XGR_MAXX, XGR_MAXY);
		cp -> init();
		cp = (CounterPanel*)cp -> prev;
	}

	p = (invMatrix*)matrixList -> last;
	while(p){
		p -> init();
		layout(p, XGR_MAXX, XGR_MAXY);
		p -> back -> load();
		// TODO: move to resource file
		p -> back -> OffsX = 10;
		p -> back -> anchor = WIDGET_ANCHOR_RIGHT;

		layout(p -> back, XGR_MAXX, XGR_MAXY);
		p = (invMatrix*)p -> prev;
	}

#ifdef _ACI_SKIP_SHOP_
	if(!curMatrix){
		curMatrix = alloc_matrix(curMatrixID);
	}
#endif

	ibs = (ibsObject*)ibsList -> last;
	while(ibs){
		for(int i = 0; i < ibs -> backObjIDs.size(); i++) {
			ibs -> backs.push_back(get_back_bml(ibs -> backObjIDs[i]));
			ibs -> backs[i] -> load(NULL, 1);
			layout(ibs -> backs[i], XGR_MAXX, XGR_MAXY);
		}
		ibs = (ibsObject*)ibs -> prev;
	}
	curIbs = get_ibs(curIbsID);
	load_ibs();

	if(wMap -> world_ids[CurrentWorld] != -1 && map_names[wMap -> world_ids[CurrentWorld]]){
		mapObj -> free();
		mapObj -> load(map_names[wMap -> world_ids[CurrentWorld]],1);
		// TODO: the code below is quick hack
		//  for blitting XGR_Obj 2d surface onto main, with color=0 as colorkey
		//  This should be solved in the future with actIntDispatcher refactoring
		int size = mapObj->Size * mapObj->SizeX * mapObj->SizeY;
		for (int i = 0; i < size; ++i) {
			if(mapObj->frames[i] == 0){
				mapObj->frames[i] = 1; // Should be a close to black color
			}
		}
	}
	else {
		ErrH.Abort("Map BMP not found...");
	}

	itm = (invItem*)itemList -> last;
	while(itm){
		itm -> init();
		itm = (invItem*)itm -> prev;
	}

	it = (fncMenu*)menuList -> last;
	while(it){
		it -> init();
		layout(it, XGR_MAXX, XGR_MAXY);
		it = (fncMenu*)it -> prev;
	}

	b = (aButton*)intButtons -> last;
	while(b){
		b -> init();
		layout(b, XGR_MAXX, XGR_MAXY);
		b = (aButton*)b -> prev;
	}
	b = (aButton*)invButtons -> last;
	while(b){
		b -> init();
		layout(b, XGR_MAXX, XGR_MAXY);
		b = (aButton*)b -> prev;
	}
	b = (aButton*)infButtons -> last;
	while(b){
		b -> init();
		layout(b, XGR_MAXX, XGR_MAXY);
		b = (aButton*)b -> prev;
	}
	init_menus();

	build_cell_frame();

	if(flags & AS_FULLSCR){
		XGR_MouseSetPromptData(NULL);
		XGR_MouseHide();
	}
	else {
		switch(curMode){
			case AS_INV_MODE:
				XGR_MouseSetPromptData(invPrompt);
				break;
			case AS_INFO_MODE:
				XGR_MouseSetPromptData(infPrompt);
				break;
		}
	}
	XGR_MouseObj.PromptColor = 160 | (1 << 16);
	aciPrevJumpCount = -1;
}

void actIntDispatcher::i_init(void)
{
	int i;

	invMatrix* p;
	CounterPanel* cp;
	invItem* itm;
	aciLocationInfo* locData;
	fncMenu* m;

	XGR_GetPal(actIntPal);
	i_slake_pal(actIntPal,32);

	int emode = ExclusiveLog ? XGR_EXCLUSIVE : 0;

	/*if(XGR_MAXX != 800){
		if(XGR_ReInit(800,600,emode)) ErrH.Abort("Error video initialization");
	}*/
	XGR_Flip();
	LoadingMessage(1);

	aci_curLocationID = get_locdata_id(aci_curLocationName);
	locData = get_locdata(aci_curLocationID);

	if(!locData)
		ErrH.Abort("Location info not found...",XERR_USER,aci_curLocationID);

	aci_ivMapName = locData -> mapName;
	aci_iScreenID = locData -> screenID;

	curLocData = locData;
	locData -> prepare();

	memcpy(aci_iscrPal,iscrPal,768);
	XStream fh(locData -> palName,XS_IN);
	fh.read(iscrPal,768);
	fh.close();

	for(i = 0; i < 16; i ++){
		iscrPal[i * 3] = i * 4;
		iscrPal[i * 3 + 1] = i * 4;
		iscrPal[i * 3 + 2] = i * 4;
	}

	ipal_init(iscrPal);
	aciShowLocationPicture();

	finit();

	aciSet_iMouse();

	aci_CellSize = aCellSize;
	aCellSize = iCellSize;

	secondMatrix = NULL;
	flags &= ~AS_ISCREEN_INV_MODE;

	if(iscr_iP) iscr_iP -> init();

	m = (fncMenu*)i_menuList -> last;
	while(m){
		m -> init();
		m = (fncMenu*)m -> prev;
	}

	m = (fncMenu*)menuList -> last;
	while(m){
		if(m -> flags & FM_LOCATION_MENU)
			m -> flags &= ~FM_ACTIVE;
		m = (fncMenu*)m -> prev;
	}

	cp = (CounterPanel*)i_Counters -> last;
	while(cp){
		cp -> init();
		cp = (CounterPanel*)cp -> prev;
	}

	p = (invMatrix*)i_matrixList -> last;
	while(p){
		p -> init();
		p = (invMatrix*)p -> prev;
	}

	itm = (invItem*)i_itemList -> last;
	while(itm){
		itm -> init();
		itm = (invItem*)itm -> prev;
	}

	if(!curMatrix && GGamerMechos){
		p = get_matrix(GGamerMechos -> type);
		curMatrix = alloc_matrix(GGamerMechos -> type);
		p -> clone(curMatrix);
	}

	if(curMatrix){
		if(flags & AS_INV_MOVE_ITEM){
			flags ^= AS_INV_MOVE_ITEM;
			if(!curMatrix -> auto_put_item(curItem)){
				aciSendEvent2itmdsp(ACI_DROP_ITEM,curItem -> item_ptr);
				restore_mouse_cursor();
				free_item(curItem);
			}
		}
		i_curMatrix = alloc_matrix(curMatrix -> type,1);
		trade_items(i_curMatrix,1);
		
		free_matrix(curMatrix);
		curMatrix = i_curMatrix;
		
	}
	else {
		a_curMatrix = NULL;
		i_curMatrix = NULL;
	}

	qMenu = new fncMenuSet;

	ascr_iP = iP;
	iP = iscr_iP;

	build_cell_frame();

	aciInitInfoPanels();
	aciInitEvLineID();
	aciInitShopItems();

	aciCurColorScheme = aciColorSchemes[SCH_ISCREEN];

	flags |= AS_ISCREEN;
	flags &= ~AS_FULL_REDRAW;

	prevWorld = CurrentWorld;
	CurrentWorld = -1;

	aciBufRedrawFlag = 1;
	PalIterLock = 1;

	dgD -> startSession(aci_curLocationName);

	LoadResourceSOUND(curLocData -> soundResPath,0);

	aciML_D -> init_data_set(aci_curLocationID);
	aciML_D -> init();
	aciPutTreasureItem();
	aciML_D -> startup();
}

void actIntDispatcher::finit(void)
{
	fncMenu* it;
	invItem* itm;
	aButton* b;
	aIndData* ind;
	CounterPanel* cp;
	InfoPanel* ip;
	aciBitmapMenu* bm;

	XGR_MouseSetPromptData(NULL);

	if(iP) iP -> finit();

	ip = (InfoPanel*)infoPanels -> last;
	while(ip){
		ip -> finit();
		ip = (InfoPanel*)ip -> prev;
	}

	bm = (aciBitmapMenu*)b_menuList -> last;
	while(bm){
		bm -> finit();
		bm = (aciBitmapMenu*)bm -> prev;
	}

	ind = (aIndData*)indList -> last;
	while(ind){
		ind -> finit();
		ind = (aIndData*)ind -> prev;
	}

	cp = (CounterPanel*)intCounters -> last;
	while(cp){
		cp -> finit();
		cp = (CounterPanel*)cp -> prev;
	}
	cp = (CounterPanel*)infCounters -> last;
	while(cp){
		cp -> finit();
		cp = (CounterPanel*)cp -> prev;
	}
	cp = (CounterPanel*)invCounters -> last;
	while(cp){
		cp -> finit();
		cp = (CounterPanel*)cp -> prev;
	}

	free_ibs();
	mapObj -> free();

	itm = (invItem*)itemList -> last;
	while(itm){
		itm -> finit();
		itm = (invItem*)itm -> prev;
	}

	it = (fncMenu*)menuList -> last;
	while(it){
		it -> finit();
		it = (fncMenu*)it -> prev;
	}

	b = (aButton*)intButtons -> last;
	while(b){
		b -> finit();
		b = (aButton*)b -> prev;
	}
	b = (aButton*)invButtons -> last;
	while(b){
		b -> finit();
		b = (aButton*)b -> prev;
	}
	b = (aButton*)infButtons -> last;
	while(b){
		b -> finit();
		b = (aButton*)b -> prev;
	}

	free_cell_frame();
}

void actIntDispatcher::i_finit(void)
{
	CounterPanel* cp;
	invItem* itm;
	fncMenu* m;

	aciBufRedrawFlag = 0;
	aCellSize = aci_CellSize;
	CurrentWorld = prevWorld;

	aciChangeWorld(CurrentWorld);
	aciPrepareWorldsMenu();

	flags &= ~AS_ISCREEN;
	if(iscr_iP) iscr_iP -> finit();

	aciML_D -> finit();

	aciSet_aMouse();
	restore_mouse_cursor();

	if(!(flags & AS_FULLSCR))
		XGR_MouseShow();

	dgD -> endSession();
	SoundEscaveOff();

	delete qMenu;
	qMenu = NULL;

	if(flags & AS_INV_MOVE_ITEM){
		aciSell_Item(curItem);
		restore_mouse_cursor();

		curItem = NULL;
		flags ^= AS_INV_MOVE_ITEM;
	}

	m = (fncMenu*)i_menuList -> last;
	while(m){
		m -> finit();
		m = (fncMenu*)m -> prev;
	}

	cp = (CounterPanel*)i_Counters -> last;
	while(cp){
		cp -> finit();
		cp = (CounterPanel*)cp -> prev;
	}

	itm = (invItem*)i_itemList -> last;
	while(itm){
		itm -> finit();
		itm = (invItem*)itm -> prev;
	}

	free_cell_frame();

	iscr_iP = iP;
	iP = ascr_iP;

	init();
	aMS_init();

	if(curMatrix){
		a_curMatrix = alloc_matrix(curMatrix -> type);
		trade_items(a_curMatrix,0);
		
		free_matrix(curMatrix);
		curMatrix = a_curMatrix;

		clean_menus();
		prepare_menus();

		aciInitItmTextQueue();
	}
	aciPrevJumpCount = -1;
	iKeyClear();
}


void fncMenuItem::init(void)
{
	if(flags & FM_RANGE_FONT)
		SizeX = aStrLen32((unsigned char*)name,font,space);
	else
		SizeX = aStrLen((unsigned char*)name,font,space);
	SizeY = aScrFonts[font] -> SizeY;
}

void InfoPanel::init(void)
{
    if(bml_name){
        if(!bml) bml = new bmlObject;
        bml -> load(bml_name);
        bml -> OffsX = (short)PosX;
        bml -> OffsX = (short)PosY;

    }
    if(ibs_name){
        if(!ibs) ibs = new ibsObject;
        ibs -> load(ibs_name);
        ibs -> PosX = (short)PosX;
        ibs -> PosY = (short)PosY;
        // TODO:
//        ibs -> recalc_geometry();
//        SideX = SizeX / 2;
//        SideY = SizeY / 2;
//
//        CenterX = PosX + SideX;
//        CenterY = PosY + SideY;
    }
}

void InfoPanel::finit(void)
{
	if(bml_name)
		bml -> free();
	if(ibs_name)
		ibs -> free();
}

void InfoPanel::free_list(void)
{
	InfoPanelItem* p,*p1;

	p = (InfoPanelItem*)items -> last;
	while(p){
		p1 = (InfoPanelItem*)p -> prev;
		items -> dconnect((iListElement*)p);
		item_pool -> connect((iListElement*)p);
		p = p1;
	}
	items -> init_list();
}

void fncMenu::init(void)
{
	if(!(flags & FM_SUBMENU)){
		if(bml_name){
			if(!bml) bml = new bmlObject;
			bml -> load(bml_name);
			bml -> OffsX = (short)PosX;
			bml -> OffsY = (short)PosY;
			bml -> change_color(0, aciCurColorScheme[ACI_BACK_COL]);
		}
		if(ibs_name){
			if(!ibs) ibs = new ibsObject;
			ibs -> load(ibs_name);
			ibs -> PosX = (short)PosX;
			ibs -> PosY = (short)PosY;
			ibs -> CenterX = ibs -> PosX + ibs -> SideX;
			ibs -> CenterY = ibs -> PosY + ibs -> SideY;
		}
	}
	init_objects();
}

void fncMenu::init_objects(void)
{
	int i,sx = 0;
	fncMenuItem* p;

	p = (fncMenuItem*)items -> first;
	for(i = 0; i < items -> Size; i ++){
		p -> init();
		p -> PosX = (SizeX - p -> SizeX)/2;
		if(flags & FM_NO_ALIGN && sx < p -> SizeX) sx = p -> SizeX;
		p = (fncMenuItem*)p -> next;
	}

	if(flags & FM_NO_ALIGN){
		SizeX = sx;
		p = (fncMenuItem*)items -> first;
		for(i = 0; i < items -> Size; i ++){
			p -> PosX = (SizeX - p -> SizeX)/2;
			p = (fncMenuItem*)p -> next;
		}
	}

	up_obj -> init();
	up_obj -> PosX = (SizeX - up_obj -> SizeX)/2;
	up_obj -> PosY = FM_OFFSET;

	down_obj -> init();
	down_obj -> PosX = (SizeX - down_obj -> SizeX)/2;
	down_obj -> PosY = SizeY - down_obj -> SizeY - FM_OFFSET;
}

void fncMenu::finit(void)
{
	if(!(flags & FM_SUBMENU)){
		if(bml_name){
			bml -> free();
		}
		if(ibs_name){
			ibs -> free();
		}
	}
}

void fncMenuItem::redraw(int bsx,int bsy,unsigned char* buf,int x,int y)
{
	int col,col_sz;
#ifdef _ACI_BML_FONTS_
	if(!(flags & FM_RANGE_FONT)){
		if(flags & FM_SELECTED)
			col = (aciCurColorScheme[FM_SELECT_BORDER_COL] << 8) | aciCurColorScheme[FM_SELECT_COL];
		else
			col = (aciCurColorScheme[FM_UNSELECT_BORDER_COL] << 8) | aciCurColorScheme[FM_UNSELECT_COL];
	}
	else {
		if(flags & FM_SELECTED){
			col = aciCurColorScheme[FM_SELECT_START];
			col_sz = aciCurColorScheme[FM_SELECT_SIZE];
		}
		else {
			col = (aciCurColorScheme[FM_SELECT_START] << 16) | aciCurColorScheme[FM_UNSELECT_START];
			col_sz = (aciCurColorScheme[FM_SELECT_SIZE] << 16) | aciCurColorScheme[FM_UNSELECT_SIZE];
		}
	}
#else
	col = (flags & FM_SELECTED) ? aciCurColorScheme[FM_SELECT_COL] : aciCurColorScheme[FM_UNSELECT_COL];
#endif
	if(flags & FM_RANGE_FONT)
		aPutStr32(x,y,font,col,col_sz,name,bsx,buf,space);
	else
		aPutStr(x,y,font,col,(unsigned char*)name,bsx,buf,space);
}

void fncMenuItem::redraw_str(int bsx,int bsy,unsigned char* buf,int x,int y,unsigned char* str)
{
	int col;
#ifdef _ACI_BML_FONTS_
	col = (aciCurColorScheme[FM_SELECT_BORDER_COL] << 8) | aciCurColorScheme[FM_SELECT_COL];
#else
	col = aciCurColorScheme[FM_SELECT_COL];
#endif
	aPutStr(x,y,font,col,str,bsx,buf,space);
}

void fncMenu::redraw(void)
{
	int i,b_col,sx,sy;
	unsigned char* buf;
	fncMenuItem* p = firstItem;
	iScreenObject* obj;

	flags &= ~FM_REDRAW;
	if(!(flags & FM_ACTIVE)){
		if(!curFunction && bml){
			bml -> show();
		}
		else
			redraw_fnc(PosX,PosY,SizeX,SizeY,curFunction);
	}
	else {
		init_redraw();

		if(!iScreenOwner){
			buf = new unsigned char[SizeX * SizeY];
		}
		else {
			if(!(iScreenOwner -> flags & EL_DATA_LOADED))
				return;
			obj = (iScreenObject*)iScreenOwner -> owner;
			obj -> flags |= OBJ_MUST_REDRAW;
			if(!bCol)
				obj -> flags |= OBJ_CLEAR_FON;
			buf = (unsigned char*)iScreenOwner -> fdata;

			sx = iScreenOwner -> SizeX;
			sy = iScreenOwner -> SizeY;
		}

		if(!(flags & FM_NO_ALIGN)){
			b_col = (bCol == -1) ? aciCurColorScheme[ACI_BACK_COL] : bCol;
			memset(buf,b_col,SizeX * SizeY);
		}

		if(items -> Size){
			if(prefix){
				curItem -> redraw_str(SizeX,SizeY,buf,PrefixX,PrefixY,prefix);
				if(postfix)
					curItem -> redraw_str(SizeX,SizeY,buf,PrefixX + PrefixSX + curItem -> SizeX + FM_PREFIX_DELTA * 2,PrefixY,postfix);
			}
			if(VItems < items -> Size){
				if(flags & FM_RANGE_FONT){
					up_obj -> flags |= FM_RANGE_FONT;
					down_obj -> flags |= FM_RANGE_FONT;
				}
				if(!(flags & FM_NO_ALIGN)){
					up_obj -> redraw(SizeX,SizeY,buf,up_obj -> PosX + PrefixDelta,up_obj -> PosY);
					down_obj -> redraw(SizeX,SizeY,buf,down_obj -> PosX + PrefixDelta,down_obj -> PosY);
				}
				else {
					up_obj -> redraw(sx,sy,buf,up_obj -> PosX + PrefixDelta + PosX,up_obj -> PosY + PosY);
					down_obj -> redraw(sx,sy,buf,down_obj -> PosX + PrefixDelta + PosX,down_obj -> PosY + PosY);
				}
			}
			if(!(flags & FM_NO_ALIGN)){
				for(i = 0; i < VItems; i ++){
					if(p == curItem)
						p -> flags |= FM_SELECTED;
					else
						p -> flags &= ~FM_SELECTED;

					p -> redraw(SizeX,SizeY,buf,p -> PosX + PrefixDelta,p -> PosY);
					p = (fncMenuItem*)p -> next;
					if(p == firstItem) i = VItems;
				}
			}
			else {
				for(i = 0; i < VItems; i ++){
					if(p == curItem)
						p -> flags |= FM_SELECTED;
					else
						p -> flags &= ~FM_SELECTED;

					p -> redraw(sx,sy,buf,p -> PosX + PrefixDelta + PosX,p -> PosY + PosY);
					p = (fncMenuItem*)p -> next;
					if(p == firstItem) i = VItems;
				}
			}
			if(!iScreenOwner){
				XGR_PutSpr(PosX,PosY,SizeX,SizeY,buf,XGR_BLACK_FON);
				delete[] buf;
			}
		}
	}
	if(ibs) ibs -> show();

	if(flags & FM_OFF){
		flags &= ~(FM_ACTIVE | FM_OFF);
	}
}

void InfoPanel::redraw(void)
{
	int i,x,y,fnt,col,col1 = 0,col_sz,tmp1,tmp2;
	unsigned char* buf;
	iScreenObject* obj;
	InfoPanelItem* p = (InfoPanelItem*)items -> first;

	if(!iScreenOwner){
		buf = new unsigned char[SizeX * SizeY];
	}
	else {
		if(!(iScreenOwner -> flags & EL_DATA_LOADED))
			return;
		obj = (iScreenObject*)iScreenOwner -> owner;
		obj -> flags |= OBJ_MUST_REDRAW;
		if(!bCol)
			obj -> flags |= OBJ_CLEAR_FON;
		buf = (unsigned char*)iScreenOwner -> fdata;
	}

	int b_col = (bCol == -1) ? aciCurColorScheme[ACI_BACK_COL] : bCol;
	memset(buf,b_col,SizeX * SizeY);

	if(!(flags & IP_RANGE_FONT))
		y = OffsY + ((SizeY - items -> Size * (aScrFonts[font] -> SizeY + vSpace)) >> 1);
	else
		y = OffsY + ((SizeY - items -> Size * (aScrFonts32[font] -> SizeY + vSpace)) >> 1);

	for(i = 0; i < items -> Size; i ++){
		if(p -> font == -1)
			fnt = font;
		else
			fnt = p -> font;

#ifdef _ACI_BML_FONTS_
		col1 = (aciCurColorScheme[FM_SELECT_BORDER_COL] << 8) | aciCurColorScheme[FM_SELECT_COL];
		if(p -> color == -1){
			if(!(flags & IP_RANGE_FONT)){
				col = (aciCurColorScheme[FM_UNSELECT_BORDER_COL] << 8) | aciCurColorScheme[FM_UNSELECT_COL];
				col |= (col1 << 16);
			}
			else {
				col = (aciCurColorScheme[FM_SELECT_START] << 16) | aciCurColorScheme[FM_UNSELECT_START];
				col_sz = (aciCurColorScheme[FM_SELECT_SIZE] << 16) | aciCurColorScheme[FM_UNSELECT_SIZE];
			}
		}
		else {
			if(!(flags & IP_RANGE_FONT)){
				col = col1;
			}
			else {
				if(p -> color & (~0xFFFF)){
					tmp1 = p -> color & 0xFF;
					tmp2 = (p -> color >> 8) & 0xFF;
					col = (tmp2 << 16) | tmp1;

					tmp1 = (p -> color >> 16) & 0xFF;
					tmp2 = (p -> color >> 24) & 0xFF;
					col_sz = (tmp2 << 16) | tmp1;
				}
				else {
					col = (aciCurColorScheme[FM_SELECT_START] << 16) | aciCurColorScheme[FM_SELECT_START];
					col_sz = (aciCurColorScheme[FM_SELECT_SIZE] << 16) | aciCurColorScheme[FM_SELECT_SIZE];
				}
			}
		}
#else
		if(p -> color == -1)
			col = aciCurColorScheme[FM_UNSELECT_COL];
		else
			col = aciCurColorScheme[FM_SELECT_COL];
#endif

		if(flags & IP_NO_ALIGN){
			x = OffsX;
		}
		else {
			if(!(flags & IP_RANGE_FONT))
				x = OffsX + ((SizeX - aStrLen((unsigned char*)p->ID_ptr.c_str(), font,hSpace)) >> 1);
			else
				x = OffsX + ((SizeX - aStrLen32((void*)p->ID_ptr.c_str(), font,hSpace)) >> 1);
		}
		if(y < 0) ErrH.Abort("InfoPanel overflow...");
		if(!(flags & IP_RANGE_FONT)){
			aPutStr(x,y,fnt,col,(unsigned char*)p->ID_ptr.c_str(),SizeX,buf,hSpace);
		}
		else {
			aPutStr32(x,y,fnt,col,col_sz, (void*)p->ID_ptr.c_str(),SizeX,buf,hSpace);
		}

		if(!(flags & IP_RANGE_FONT))
			y += aScrFonts[font] -> SizeY + vSpace;
		else
			y += aScrFonts32[font] -> SizeY + vSpace;

		p = (InfoPanelItem*)p -> next;
	}
	if(!iScreenOwner){
		XGR_PutSpr(PosX,PosY,SizeX,SizeY,buf,XGR_BLACK_FON);
		delete[] buf;
	}

	if(ibs) ibs -> show();
	flags &= ~IP_REDRAW;
}

void fncMenu::flush(void)
{
	if(iScreenOwner) return;

	if(bml)
		XGR_Flush(bml -> OffsX,bml -> OffsY,bml -> SizeX,bml -> SizeY);
	else
		XGR_Flush(PosX,PosY,SizeX,SizeY);
	flags &= ~FM_FLUSH;
}

void InfoPanel::flush(void)
{
	if(iScreenOwner) return;

	if(bml)
		XGR_Flush(bml -> OffsX,bml -> OffsY,bml -> SizeX,bml -> SizeY);
	else
		XGR_Flush(PosX,PosY,SizeX,SizeY);
	flags &= ~IP_FLUSH;
}

void aButton::flush(void)
{
	XGR_Flush(PosX,PosY,SizeX,SizeY);
	flags &= ~B_FLUSH;
}

aKeyObj::aKeyObj(void)
{
	codes = new iList;
}

aKeyObj::~aKeyObj(void)
{
	iScanCode* p,*p1;
	p = (iScanCode*)codes -> last;
	while(p){
		p1 = (iScanCode*)p -> prev;
		delete p;
		p = p1;
	}
	delete codes;
}

void aKeyObj::free_list(void)
{
	iScanCode* p,*p1;
	p = (iScanCode*)codes -> last;
	while(p){
		p1 = (iScanCode*)p -> prev;
		delete p;
		p = p1;
	}
	codes -> init_list();
}

int aKeyObj::check(int k)
{
	iScanCode* p = (iScanCode*)codes -> last;
	while(p){
		if(p -> code == k)
			return 1;
		p = (iScanCode*)p -> prev;
	}
	return 0;
}

void aKeyObj::add_key(int k)
{
	iScanCode* p = new iScanCode;
	p -> code = k;
	codes -> connect((iListElement*)p);
}

void actIntDispatcher::KeyQuant(void)
{
	int k;
	aButton* bt;
	fncMenu* m;

	aciBitmapMenu* bm;

#ifdef _DEBUG
	int i;
	invMatrix* temp;
	invItem* temp_item;

	XBuffer* shotBufBMP;
	XBuffer* shotBufPAL;
#endif

	if(aciKeyboardLocked && !(flags & AS_CHAT_MODE)) return;

	if(flags & AS_LOCKED){
		if(flags & AS_CHAT_MODE){
			while(KeyBuf -> size) iChatKeyQuant(KeyBuf -> get());
			if(flags & aMS_LEFT_PRESS || flags & aMS_RIGHT_PRESS){
				iChatMouseQuant(XGR_MouseObj.PosX + XGR_MouseObj.SpotX,XGR_MouseObj.PosY + XGR_MouseObj.SpotY,1);
			}
		}
		flags &= ~aMS_PRESS;
		KeyBuf -> clear();
		return;
	}

	if(flags & AS_FULLSCR)
		flags &= ~aMS_PRESS;

	inv_mouse_move_quant();

	if(flags & aMS_MOVED){
		flags &= ~aMS_MOVED;
		if(curMode == AS_INFO_MODE){
			m = (fncMenu*)menuList -> last;
			while(m){
				if(!(m -> flags & FM_HIDDEN) && m -> check_xy(iMouseX,iMouseY)){
					if(m -> flags & FM_ACTIVE){
						m -> change(iMouseX,iMouseY,1);
					}
				}
				m = (fncMenu*)m -> prev;
			}
		}
	}

	if(flags & aMS_PRESS){
		if(flags & aMS_LEFT_PRESS){
			flags &= ~aMS_LEFT_PRESS;

			if(!(flags & AS_INV_MOVE_ITEM)){
				bt = (aButton*)intButtons -> last;
				while(bt){
					if(!(bt -> flags & B_ACTIVE)){
						if(bt -> check_xy(iMouseX,iMouseY)){
							bt -> press();
							SOUND_SELECT();
							send_event(bt -> eventCode,bt -> eventData);
						}
					}
					bt = (aButton*)bt -> prev;
				}
			}
			switch(curMode){
				case AS_INV_MODE:
					if(!(flags & AS_INV_MOVE_ITEM)){
						bt = (aButton*)invButtons -> last;
						while(bt){
							if(!(bt -> flags & B_ACTIVE)){
								if(bt -> check_xy(iMouseX,iMouseY)){
									bt -> press();
									SOUND_SELECT();
									send_event(bt -> eventCode,bt -> eventData);
								}
							}
							bt = (aButton*)bt -> prev;
						}
					}
					inv_mouse_quant_l();
					break;
				case AS_INFO_MODE:
					bt = (aButton*)infButtons -> last;
					while(bt){
						if(!(bt -> flags & B_ACTIVE)){
							if(bt -> check_xy(iMouseX,iMouseY)){
								bt -> press();
								SOUND_SELECT();
								send_event(bt -> eventCode,bt -> eventData);
							}
						}
						bt = (aButton*)bt -> prev;
					}
					m = (fncMenu*)menuList -> last;
					while(m){
						if(!(m -> flags & FM_HIDDEN) && !(m -> flags & FM_LOCK) && m -> check_xy(iMouseX,iMouseY)){
							if(m -> flags & FM_ACTIVE){
								if(m -> change(iMouseX,iMouseY)){
									init_menus();
									if(!(m -> flags & FM_SUBMENU)){
										init_submenu(m);
										m -> curCount = m -> activeCount;
									}
									SOUND_SELECT();
								}
							}
							else {
								if(m -> curFunction == FMENU_CAMERAS){
									bm = get_bmenu(BMENU_CAMERAS_MENU);
									if(!bm) return;
									bm -> change(iMouseX - m -> PosX,iMouseY - m -> PosY);
								}
								else {
									if(!m -> trigger && !(m -> flags & FM_SUBMENU)){
										if(!(m -> flags & FM_LOCK)){
											m -> flags |= FM_ACTIVE;
											m -> curCount = m -> activeCount;
											m -> set_redraw();
											SOUND_SELECT();
										}
										else {
											m -> flags &= ~FM_LOCK;
										}
									}
								}
							}
						}
						m = (fncMenu*)m -> prev;
					}
					break;
			}
		}
		if(flags & aMS_RIGHT_PRESS){
			flags &= ~aMS_RIGHT_PRESS;
			switch(curMode){
				case AS_INV_MODE:
					inv_mouse_quant_r();
					break;
				case AS_INFO_MODE:
					break;
			}
		}
	}

	while(KeyBuf -> size){
		k = sdlEventToCode(KeyBuf->get());
		if(flags & AS_TEXT_MODE){
			if(iCheckKeyID(iKEY_SKIP_TEXT,k)){
				if(!ScrTextData -> NextPage()){
					aciTextEndFlag = 1;
					send_event(EV_LEAVE_TEXT_MODE);
				}
			}
		}

		if(iCheckKeyID(iKEY_FULLSCREEN,k)){
			send_event(EV_FULLSCR_CHANGE);
		}

		if(NetworkON){
			if(iCheckKeyID(iKEY_FRAG_INFO,k)){
				aciShowFrags();
			}
			if(iCheckKeyID(iKEY_CHAT,k)){
				send_event(EV_ENTER_CHAT);
			}
		}

		if(!(flags & AS_FULLSCR)){
			if(iCheckKeyID(iKEY_REDUCE_VIEW,k)){
				if(curMode != AS_INV_MODE){
					send_event(EV_CHANGE_SCREEN,1);
				}
			}
			if(iCheckKeyID(iKEY_ENLARGE_VIEW,k)){
				if(curMode != AS_INV_MODE){
					send_event(EV_CHANGE_SCREEN,3);
				}
			}
		}
		else {
			if(iCheckKeyID(iKEY_INVENTORY,k)){
				if(curMode != AS_INV_MODE)
					send_event(EV_SET_MODE,AS_INV_MODE);
				send_event(EV_FULLSCR_CHANGE);
			}
		}

		if(!NetworkON) 
			aciCHandler(k);

		if(!(flags & AS_FULLSCR)){
			bt = (aButton*)intButtons -> last;
			while(bt){
				if(!(bt -> flags & B_ACTIVE)){
					if(bt -> scankey -> check(k)){
						bt -> press();
						SOUND_SELECT();
						send_event(bt -> eventCode,bt -> eventData);
					}
				}
				bt = (aButton*)bt -> prev;
			}
			switch(curMode){
				case AS_INV_MODE:
					bt = (aButton*)invButtons -> last;
					while(bt){
						if(!(bt -> flags & B_ACTIVE)){
							if(bt -> scankey -> check(k)){
								bt -> press();
								SOUND_SELECT();
								send_event(bt -> eventCode,bt -> eventData);
							}
						}
						bt = (aButton*)bt -> prev;
					}
					break;
				case AS_INFO_MODE:
					bt = (aButton*)infButtons -> last;
					while(bt){
						if(!(bt -> flags & B_ACTIVE)){
							if(bt -> scankey -> check(k)){
								bt -> press();
								SOUND_SELECT();
								send_event(bt -> eventCode,bt -> eventData);
							}
						}
						bt = (aButton*)bt -> prev;
					}
					m = (fncMenu*)menuList -> last;
					while(m){
						if(m -> flags & FM_ACTIVE && m -> up_key -> check(k)){
							m -> step_up();
							m -> set_redraw();
							SOUND_SELECT();
						}
						if(m -> flags & FM_ACTIVE && m -> down_key -> check(k)){
							m -> step_down();
							m -> set_redraw();
							SOUND_SELECT();
						}
						m = (fncMenu*)m -> prev;
					}
					break;
			}
		}
	}
	m = (fncMenu*)menuList -> last;
	while(m){
		m -> flags &= ~FM_LOCK;
		m = (fncMenu*)m -> prev;
	}
}

void actIntDispatcher::iKeyQuant(void)
{
	fncMenu* m;
	if(flags & AS_ISCREEN_INV_MODE){
		inv_mouse_imove_quant();
		if(flags & aMS_MOVED){
			flags &= ~aMS_MOVED;
			m = (fncMenu*)i_menuList -> last;
			while(m){
				if(!(m -> flags & FM_HIDDEN) && m -> check_xy(iMouseX,iMouseY)){
					if(m -> flags & FM_ACTIVE){
						m -> change(iMouseX,iMouseY,1);
						if(m -> type == SHOP_ITEMS_MENU_ID)
							aciSetShopItem(m -> curFunction);
					}
				}
				m = (fncMenu*)m -> prev;
			}
		}
		if(flags & aMS_PRESS){
			if(flags & aMS_LEFT_PRESS){
				flags &= ~aMS_LEFT_PRESS;
				inv_mouse_quant_l();
			}
			if(flags & aMS_RIGHT_PRESS){
				flags &= ~aMS_RIGHT_PRESS;
				inv_mouse_quant_r();
			}
		}
	}
	else {
		if(flags & aMS_MOVED){
			flags &= ~aMS_MOVED;
			m = (fncMenu*)menuList -> last;
			while(m){
				if(!(m -> flags & FM_HIDDEN) && m -> check_xy(iMouseX,iMouseY)){
					if(m -> flags & FM_ACTIVE){
						m -> change(iMouseX,iMouseY,1);
					}
				}
				m = (fncMenu*)m -> prev;
			}
			if(iMouseLPressFlag){
				if(qMenu && qMenu -> flags & FMC_DATA_INIT && qMenu -> check_xy(iMouseX,iMouseY) && qMenu -> check_xy(iMousePrevX,iMousePrevY)){
					qMenu -> shift((iMouseX - iMousePrevX) * 4);
				}
			}
		}
		if(flags & aMS_PRESS){
			if(flags & aMS_LEFT_PRESS){
				if(qMenu && qMenu -> flags & FMC_DATA_INIT){
					qMenu -> key_trap(iMouseX,iMouseY);
				}
				m = (fncMenu*)i_menuList -> last;
				while(m){
					if(m -> check_xy(iMouseX,iMouseY)){
						if(m -> flags & FM_ACTIVE){
							m -> change(iMouseX,iMouseY);
							SOUND_SELECT();
						}
					}
					m = (fncMenu*)m -> prev;
				}
				flags &= ~aMS_LEFT_PRESS;
			}
		}
	}
}

void actIntDispatcher::iKeyTrap(int cd)
{
	fncMenu* m;
	if(!NetworkON) aciCHandler(cd);
	if(!(flags & AS_ISCREEN_INV_MODE)){
		m = (fncMenu*)i_menuList -> last;
		while(m){
			if(m -> flags & FM_LOCATION_MENU && m -> flags & FM_ACTIVE && m -> up_key -> check(cd)){
				m -> step_up();
				m -> set_redraw();
				SOUND_SELECT();
			}
			if(m -> flags & FM_ACTIVE && m -> down_key -> check(cd)){
				m -> step_down();
				m -> set_redraw();
				SOUND_SELECT();
			}
			m = (fncMenu*)m -> prev;
		}
	}
}

void actIntDispatcher::send_event(int cd,int dt,actintItemData* p)
{
	if(cd == EV_CHANGE_MODE || cd == EV_SET_MODE){
		if(!(flags & AS_CHANGE_MODE)){
			flags |= AS_CHANGE_MODE;
			events -> put(cd,dt,p);
		}
	}
	else
		events -> put(cd,dt,p);
}

void actIntDispatcher::EventQuant(void)
{
	actEvent* p;
	fncMenu* m;
	calc_load();
	aciSetTimePanel();
	aciCurFrame ++;

//	  aciUpdateCurCredits(aciCurCredits);

	if(aciItmTextQueueSize && !(flags & AS_TEXT_MODE)) aciReadItmText();

	if(iChatExit){
		iChatExit = 0;
		send_event(EV_LEAVE_CHAT);
	}

	aciTextColBright += aciTextColBrightDelta;
	if(aciTextColBright < 0){
		aciTextColBright = 0;
		aciTextColBrightDelta = 1;
	}
	if(aciTextColBright > 64){
		aciTextColBright = 64;
		aciTextColBrightDelta = -1;
	}

	if(wMap){
		wMap -> quant();
	}

	while(events -> size){
		p = events -> get();
		switch(p -> code){
			case EV_NEXT_PHRASE:
				aciNextPhrase();
				break;
			case EV_CHANGE_MODE:
				change_mode();
				break;
			case EV_SET_MODE:
				if(p -> data != curMode)
					change_mode();
				break;
			case EV_ACTIVATE_MENU:
				if(curMode == AS_INFO_MODE){
					m = get_menu(p -> data);
					if(!m)
						ErrH.Abort("Bad event data...");
					if(m -> trigger){
						if(m -> trigger -> flags & B_PRESSED)
							m -> flags &= ~FM_ACTIVE;
						else
							m -> flags |= FM_ACTIVE;
					}
					else
						m -> flags ^= FM_ACTIVE;
					m -> curItem = m -> get_fnc_item(m -> curFunction);
					if(m -> VItems < m -> items -> Size)
						m -> firstItem = m -> curItem;
					else
						m -> firstItem = (fncMenuItem*)m -> items -> first;

					m -> set_redraw();
				}
				break;
			case ACI_PUT_ITEM:
				put_item(p -> ptr);
				break;
			case ACI_DROP_ITEM:
				remove_item(p -> ptr);
				break;
			case EV_CHANGE_SCREEN:
				if(curScrMode != p -> data){
					prevScrMode = curScrMode;
					curScrMode = p -> data;
					change_screen(p -> data);
				}
				break;
			case EV_FULLSCR_CHANGE:
				flags ^= AS_FULLSCR;
				if(flags & AS_FULLSCR){
					set_fullscreen(true);
				}
				else {
					flags &= ~AS_TEXT_MODE;
					set_fullscreen(false);
				}
				break;
			case EV_ACTIVATE_IINV:
				flags |= AS_ISCREEN_INV_MODE;
				break;
			case EV_DEACTIVATE_IINV:
				flags &= ~AS_ISCREEN_INV_MODE;
				break;
			case EV_ACTIVATE_MATRIX:
				secondMatrix = alloc_matrix(aci_SecondMatrixID,1);
				secondMatrix -> ScreenX += 400;
				if(curMatrix){
					backupMatrix = alloc_matrix(curMatrix -> type,1);
					curMatrix -> backup(backupMatrix);
				}
				aciShowScMatrix();
				break;
			case EV_DEACTIVATE_MATRIX:
				aciBuyItem();
				aciSwapMatrices();
				aciInitShopAvi();
				break;
			case EV_EVINCE_PALETTE:
				flags |= AS_EVINCE_PALETTE;
				break;
			case EV_INIT_MATRIX_OBJ:
				aci_setMatrixBorder();
				aci_setScMatrixBorder();
				break;
			case EV_INIT_SC_MATRIX_OBJ:
				aci_setScMatrixBorder();
				break;
			case EV_REDRAW:
				flags &= ~AS_FULL_REDRAW;
				break;
			case EV_CANCEL_MATRIX:
				aciCancelMatrix();
				break;
			case EV_AUTO_MOVE_ITEMS:
				aciAutoMoveItems();
				break;
			case EV_SET_MECH_NAME:
				aciSetMechName();
				break;
			case EV_NEXT_SHOP_AVI:
				aciNextShopItem();
				aciInitShopAvi();
				break;
			case EV_PREV_SHOP_AVI:
				aciPrevShopItem();
				aciInitShopAvi();
				break;
			case EV_CHANGE_AVI_LIST:
				aciChangeList();
				aciInitShopAvi();
				break;
			case EV_BUY_ITEM:
				aciBuyItem();
				aciInitShopAvi();
				break;
			case EV_SET_ITM_PICKUP:
				aciPickupItems ^= 1;
				break;
			case EV_SET_WPN_PICKUP:
				aciPickupDevices ^= 1;
				break;
			case EV_ACTIVATE_SHOP_MENU:
				aciInitShopList();
				break;
			case EV_CHOOSE_SHOP_ITEM:
				aciChooseShopItem();
				break;
			case EV_SHOW_QUESTS:
				aciBuildQuestList();
				break;
			case EV_ASK_QUEST:
				aciAskQuestion();
				break;
			case EV_GET_BLOCK_PHRASE:
				aciShowBlockPhrase();
				break;
			case EV_TRY_2_ENTER:
				aciActionEvent();
				break;
			case EV_GET_CIRT:
				aciSendEvent2itmdsp(ACI_ACTIVATE_ITEM,p -> ptr);
				break;
			case ACI_PUT_IN_SLOT:
				put_in_slot(p -> ptr);
				break;
			case EV_GET_ELEECH:
				aciGetEleech();
				break;
			case EV_GET_RUBOX:
				aciGetRubox();
				break;
			case EV_LOCK_ISCREEN:
				iScrDisp -> lock_events();
				break;
			case EV_UNLOCK_ISCREEN:
				iScrDisp -> unlock_events();
				break;
			case EV_ISCR_KEYTRAP:
				iScrDisp -> key_trap(p -> data);
				break;
			case EV_SELL_MOVE_ITEM:
				aciSellMoveItem();
				break;
			case EV_CHANGE_AVI_INDEX:
				aciChangeAviIndex();
				aciInitShopAvi();
				break;
			case EV_START_SPEECH:
//				  aciStartSpeech();
				break;
			case EV_TELEPORT:
				if(wMap){
					aciWorldIndex = wMap -> world_ptr[p -> data] -> uvsID;
					aciPrevJumpCount = -1;
					if(!(flags & AS_FULLSCR)){
						flags &= ~AS_TEXT_MODE;
						set_fullscreen(true);
					}
					lock();
				}
				break;
			case EV_INIT_BUTTONS:
				aciInitShopButtons();
				break;
			case EV_ENTER_TEXT_MODE:
			case ACI_SHOW_TEXT:
				if(!(flags & AS_FULLSCR)){
					set_fullscreen(true);
				}
				ScrTextData -> color = aciTEXT_COLOR;
				flags |= AS_TEXT_MODE;
				break;
			case EV_LEAVE_TEXT_MODE:
			case ACI_HIDE_TEXT:
				if(flags & AS_FULLSCR){
					set_fullscreen(false);
				}
				ScrTextData -> clear();
				flags &= ~AS_TEXT_MODE;
				break;
			case ACI_LOCK_INTERFACE:
				if(!(flags & AS_FULLSCR)){
					flags &= ~AS_TEXT_MODE;
					set_fullscreen(true);
				}
				lock();
				break;
			case ACI_UNLOCK_INTERFACE:
				if(flags & AS_FULLSCR){
					set_fullscreen(false);
				}
				unlock(); // logical, functional blocking
				break;
			case EV_PROTRACTOR_EVENT:
				aciProtractorEvent = p -> data;
				break;
			case EV_MECH_MESSIAH_EVENT:
				aciMechMessiahEvent = p -> data;
				break;
			case EV_PAUSE_AML:
				aciML_Pause();
				break;
			case EV_RESUME_AML:
				aciML_Resume();
				break;
			case EV_ENTER_CHAT:
				if(!(flags & AS_FULLSCR)){
					flags &= ~AS_TEXT_MODE;
					set_fullscreen(true);
				}
				lock();
				iChatInit();
				flags |= AS_CHAT_MODE;
				break;
			case EV_ITEM_TEXT:
				aciSendEvent2itmdsp(ACI_SHOW_ITEM_TEXT,p -> ptr);
				break;
			case EV_LEAVE_CHAT:
				if(flags & AS_FULLSCR){
					set_fullscreen(false);
				}
				unlock();
				iChatFinit();
				flags &= ~AS_CHAT_MODE;
				break;
			case EV_INIT_AVI_OBJECT:
				aciInitAviObject();
				break;
		}
	}
}

void actIntDispatcher::set_fullscreen(bool isEnabled) {
	if(isEnabled){
		flags |= AS_FULLSCR;
		set_screen(XGR_MAXX/2 - 0,XGR_MAXY/2 - 0,0,XGR_MAXX/2,XGR_MAXY/2);
		XGR_Obj.clear_2d_surface();
		XGR_MouseSetPromptData(NULL);
		XGR_MouseHide();
	} else {
		flags &= ~AS_FULL_REDRAW;
		flags &= ~AS_FULLSCR;
		set_screen(XGR_MAXX/2, XGR_MAXY/2,0, XGR_MAXX/2, XGR_MAXY/2);
		XGR_MouseShow();
		if(curMode == AS_INV_MODE){
			mechosCameraOffsetX = AS_INV_CAMERA_OFFSET;
			XGR_MouseSetPromptData(invPrompt);
		}
		else {
			if(curMode == AS_INFO_MODE){
				mechosCameraOffsetX = AS_INF_CAMERA_OFFSET;
				XGR_MouseSetPromptData(infPrompt);
			}
		}
	}
}

void actIntDispatcher::change_mode(void)
{
	//std::cout<<"actIntDispatcher::change_mode"<<std::endl;
	aButton* b;
	fncMenu* p;
	switch(curMode){
		case AS_INV_MODE:
			if(flags & AS_INV_MOVE_ITEM) break;
			curMode = AS_INFO_MODE;
			XGR_MouseSetPromptData(infPrompt);
			change_screen(prevScrMode);
			curScrMode = prevScrMode;

			p = (fncMenu*)menuList -> last;
			while(p){
				p -> set_redraw();
				p -> set_flush();
				p = (fncMenu*)p -> prev;
			}
			b = (aButton*)infButtons -> last;
			while(b){
				b -> set_redraw();
				b -> set_flush();
				b = (aButton*)b -> prev;
			}
			mechosCameraOffsetX = AS_INF_CAMERA_OFFSET;
			break;
		case AS_INFO_MODE:
			curMode = AS_INV_MODE;
			XGR_MouseSetPromptData(invPrompt);
			prevScrMode = curScrMode;
			curScrMode = INVSCR_MODE;
			change_screen(INVSCR_MODE);

			curMatrix -> set_redraw();
			curMatrix -> set_flush();
			b = (aButton*)invButtons -> last;

			while(b){
				b -> set_redraw();
				b -> set_flush();
				b = (aButton*)b -> prev;
			}
			mechosCameraOffsetX = AS_INV_CAMERA_OFFSET;
			break;
	}
	flags &= ~AS_CHANGE_MODE;
}

void CounterPanel::set_flush(void)
{
	flags |= CP_FLUSH;
}

void invMatrix::set_flush(void)
{
	flags |= IM_FLUSH;
}

void fncMenu::set_flush(void)
{
	flags |= FM_FLUSH;
}

void InfoPanel::set_flush(void)
{
	flags |= IP_FLUSH;
}

void aButton::set_flush(void)
{
	flags |= B_FLUSH;
}

void aIndData::set_flush(void)
{
	flags |= IND_FLUSH;
}

void CounterPanel::set_redraw(void)
{
	flags |= CP_REDRAW;
}

void invMatrix::set_redraw(void)
{
	flags |= IM_REDRAW;
}

void fncMenu::set_redraw(void)
{
	flags |= FM_REDRAW;
}

void InfoPanel::set_redraw(void)
{
	flags |= IP_REDRAW;
}

void aButton::set_redraw(void)
{
	flags |= B_REDRAW;
}

void aIndData::set_redraw(void)
{
	flags |= IND_REDRAW;
}

actEventHeap::actEventHeap(void)
{
	int i;
	table = new actEvent*[AS_MAX_EVENT];
	mem_heap = new char[AS_MAX_EVENT * sizeof(actEvent)];
	for(i = 0; i < AS_MAX_EVENT; i ++){
		table[i] = (actEvent*)(mem_heap + sizeof(actEvent) * i);
	}
	clear();
}

actEventHeap::~actEventHeap(void)
{
	delete[] table;
	delete[] mem_heap;
}

void actEventHeap::put(int cd,int dt,actintItemData* p)
{
	if(size < AS_MAX_EVENT){
		table[last_index] -> code = cd;
		table[last_index] -> data = dt;
		table[last_index] -> ptr = p;

		last_index ++;
		if(last_index >= AS_MAX_EVENT)
			last_index = 0;
		size ++;
	}
	else
		ErrH.Abort("Event buffer overflow...");
}

actEvent* actEventHeap::get(void)
{
	actEvent* p;
	if(size){
		p = table[first_index];

		first_index ++;
		if(first_index >= AS_MAX_EVENT)
			first_index = 0;

		size --;
		return p;
	}
	return NULL;
}

void actEventHeap::clear(void)
{
	size = 0;
	first_index = 0;
	last_index = 0;
}

fncMenu* actIntDispatcher::get_menu(int mn)
{
	fncMenu* m = (fncMenu*)menuList -> last;
	while(m){
		if(m -> type == mn)
			return m;
		m = (fncMenu*)m -> prev;
	}
	return NULL;
}

fncMenu* actIntDispatcher::get_imenu(int mn)
{
	fncMenu* m = (fncMenu*)i_menuList -> last;
	while(m){
		if(m -> type == mn)
			return m;
		m = (fncMenu*)m -> prev;
	}
	return NULL;
}

void actIntDispatcher::init_menus(void)
{
	fncMenu* m,*m1;
	fncMenuItem* p,*p1;
	m = (fncMenu*)menuList -> last;
	while(m){
		if(!(m -> flags & FM_SUBMENU)){
			p = (fncMenuItem*)m -> inactive_items -> last;
			while(p){
				p1 = (fncMenuItem*)p -> prev;
				m -> inactive_items -> dconnect((iListElement*)p);
				m -> items -> connect((iListElement*)p);
				p = p1;
			}
			m -> curItem = m -> get_fnc_item(m -> curFunction);
			if(m -> VItems < m -> items -> Size)
				m -> firstItem = m -> curItem;
			else
				m -> firstItem = (fncMenuItem*)m -> items -> first;
			m -> trigger = get_button(m -> trigger_code);
		}
		m = (fncMenu*)m -> prev;
	}

	m = (fncMenu*)menuList -> last;
	while(m){
		if(!(m -> flags & FM_SUBMENU)){
			p = (fncMenuItem*)m -> items -> last;
			while(p){
				p1 = (fncMenuItem*)p -> prev;
				m1 = (fncMenu*)menuList -> last;
				while(m1){
					if(!(m1 -> flags & FM_SUBMENU)){
						if(m1 != m && m1 -> curFunction && m1 -> curFunction == p -> fnc_code){
							if(m -> curItem == p){
								m -> curItem = (fncMenuItem*)p -> next;
								m -> curFunction = m -> curItem -> fnc_code;
							}
							m -> items -> dconnect((iListElement*)p);
							m -> inactive_items -> connect((iListElement*)p);
							m1 = NULL;
						}
						else
							m1 = (fncMenu*)m1 -> prev;
					}
					else
						m1 = (fncMenu*)m1 -> prev;
				}
				p = p1;
			}
			m -> sort();
			m -> curItem = m -> get_fnc_item(m -> curFunction);
			if(m -> VItems < m -> items -> Size)
				m -> firstItem = m -> curItem;
			else
				m -> firstItem = (fncMenuItem*)m -> items -> first;

			if(m -> flags & FM_ACTIVE) m -> set_redraw();
		}
		m = (fncMenu*)m -> prev;
	}
}

fncMenuItem* fncMenu::get_fnc_item(int fnc)
{
	fncMenuItem* p = (fncMenuItem*)items -> last;
	while(p){
		if(p -> fnc_code == fnc)
			return p;
		p = (fncMenuItem*)p -> prev;
	}
	return NULL;
}

int aButton::check_xy(int x,int y)
{
	if(x >= PosX && y >= PosY && x <= PosX + SizeX && y <= PosY + SizeY)
		return 1;
	return 0;
}

int fncMenu::check_xy(int x,int y)
{
	if(x >= PosX && y >= PosY && x <= PosX + SizeX && y <= PosY + SizeY)
		return 1;
	return 0;
}

int fncMenuItem::check_y(int y)
{
	if(y >= PosY && y <= PosY + SizeY)
		return 1;
	return 0;
}

void fncMenu::step_up(void)
{
	if(curItem -> prev){
		curItem = (fncMenuItem*)curItem -> prev;
	}
	else {
		curItem = (fncMenuItem*)items -> last;
	}

	init_redraw();
	while(!(curItem -> flags & FM_VISIBLE)){
		firstItem = (fncMenuItem*)firstItem -> prev;
		if(!firstItem) firstItem = (fncMenuItem*)items -> last;
		init_redraw();
	}
	if(trigger)
		trigger -> curCount = 0;

	curFunction = curItem -> fnc_code;
}

void fncMenu::step_down(void)
{
	curItem = (fncMenuItem*)curItem -> next;
	init_redraw();
	while(!(curItem -> flags & FM_VISIBLE)){
		firstItem = (fncMenuItem*)firstItem -> next;
		init_redraw();
	}
	if(trigger)
		trigger -> curCount = 0;

	curFunction = curItem -> fnc_code;
}

aButton* actIntDispatcher::get_button(int id)
{
	aButton* p;
	p = (aButton*)intButtons -> last;
	while(p){
		if(p -> ID == id)
			return p;
		p = (aButton*)p -> prev;
	}
	p = (aButton*)invButtons -> last;
	while(p){
		if(p -> ID == id)
			return p;
		p = (aButton*)p -> prev;
	}
	p = (aButton*)infButtons -> last;
	while(p){
		if(p -> ID == id)
			return p;
		p = (aButton*)p -> prev;
	}

	return NULL;
}

void fncMenu::init_redraw(void)
{
	int i,dy = 0,min_y,max_y,max_sy;
	fncMenuItem* p;

	p = (fncMenuItem*)items -> last;
	if(!p) return;
	while(p){
		p -> flags &= ~FM_VISIBLE;
		p = (fncMenuItem*)p -> prev;
	}

	if(!prefix){
		p = firstItem;
		for(i = 0; i < VItems; i ++){
			p -> flags |= FM_VISIBLE;
			p -> PosY = dy;
			dy += p -> SizeY + p -> space;
			p = (fncMenuItem*)p -> next;
			if(p == firstItem) i = VItems;
		}
		itemY = (SizeY - dy) / 2;

		p = firstItem;
		for(i = 0; i < VItems; i ++){
			p -> PosY += itemY;
			p = (fncMenuItem*)p -> next;
			if(p == firstItem) i = VItems;
		}
	}
	else {
		curItem -> PosY = PrefixY;
		curItem -> flags |= FM_VISIBLE;
		if(items -> Size > 2){
			if(curItem -> prev)
				p = (fncMenuItem*)curItem -> prev;
			else
				p = (fncMenuItem*)items -> last;

			p -> PosY = curItem -> PosY - (p -> space + p -> SizeY);
			p -> flags |= FM_VISIBLE;

			firstItem = p;
			p = (fncMenuItem*)curItem -> next;
			p -> PosY = curItem -> PosY + (curItem -> space + curItem -> SizeY);
			p -> flags |= FM_VISIBLE;
		}
		else {
			if(items -> Size == 2){
				firstItem = curItem;
				p = (fncMenuItem*)curItem -> next;
				p -> PosY = curItem -> PosY + (curItem -> space + curItem -> SizeY);
				p -> flags |= FM_VISIBLE;
			}
		}
	}
	p = firstItem;
	min_y = max_y = p -> PosY;
	max_sy = p -> SizeY;
	for(i = 0; i < VItems; i ++){
		if(min_y > p -> PosY)
			min_y = p -> PosY;
		if(max_y < p -> PosY){
			max_y = p -> PosY;
			max_sy = p -> SizeY;
		}
		p = (fncMenuItem*)p -> next;
		if(p == firstItem) i = VItems;
	}
	up_obj -> PosY = min_y - ACI_ARROW_OFFS_UP - up_obj -> SizeY;
	down_obj -> PosY = max_y + ACI_ARROW_OFFS_DOWN + max_sy;
}
 
int fncMenu::change(int x,int y,int mode)
{
	fncMenuItem* p;

	x -= PosX;
	y -= PosY;

	up_obj -> flags &= ~FM_SELECTED;
	down_obj -> flags &= ~FM_SELECTED;

	init_redraw();
	if(items -> Size > VItems){
		if(x >= 0 && x < SizeX && up_obj -> check_y(y)){
			if(!mode)
				step_up();
			up_obj -> flags |= FM_SELECTED;
			set_redraw();
			if(prefix) set_prefix(prefix);
			init_redraw();
			curCount = activeCount;
			return 0;
		}
		if(x >= 0 && x < SizeX && down_obj -> check_y(y)){
			if(!mode)
				step_down();
			down_obj -> flags |= FM_SELECTED;
			set_redraw();
			if(prefix) set_prefix(prefix);
			init_redraw();
			curCount = activeCount;
			return 0;
		}
	}
	p = (fncMenuItem*)items -> last;
	while(p){
		if(p -> flags & FM_VISIBLE && p -> check_y(y)){
			curItem = p;
			curFunction = p -> fnc_code;
			if(!mode){
				if(!(flags & FM_NO_DEACTIVATE) && !(flags & FM_SUBMENU))
					flags &= ~FM_ACTIVE;
				if(trigger)
					trigger -> press();
			}
			if(prefix) set_prefix(prefix);
			set_redraw();
			init_redraw();

			if(flags & FM_SUBMENU){
				if(!mode){
					aciHandleSubmenuEvent(type,curItem);
					if(flags & FM_ITEM_MENU || curFunction == FMENU_OFF){
						go2upmenu();
						return 1;
					}
				}
				curCount = activeCount;
			}
			return 1;
		}
		p = (fncMenuItem*)p -> prev;
	}

	if(!mode){
		if(flags & FM_SUBMENU){
			go2upmenu();
		}
	}
	return 0;
}

void actIntDispatcher::init_submenu(fncMenu* m)
{
	char* ptr;

	fncMenu* p = NULL;
	aciBitmapMenu* bm = NULL;

	if(!m -> curItem) return;

	if(m -> curItem -> flags & FM_SUBMENU_ITEM){
		p = get_menu(m -> curItem -> submenuID);
		if(!p) ErrH.Abort("init_submenu() failed...");

		if(m -> curItem -> submenuID == FMENU_TELEPORT_MENU)
			p -> curFunction = FMENU_OFF;

		m -> init_submenu(p);
		p -> init_curItem();
		p -> curCount = p -> activeCount;
		p -> flags |= FM_LOCK;

		if(p -> VItems < p -> items -> Size)
			p -> firstItem = p -> curItem;
		else
			p -> firstItem = (fncMenuItem*)p -> items -> first;
	}
	else {
		if(m -> curItem -> flags & FM_BSUBMENU_ITEM){
			bm = get_bmenu(m -> curItem -> submenuID);
			bm -> upMenu = (iListElement*)m;
			if(bm -> activeCount)
				bm -> curCount = bm -> activeCount;
		}
	}

	switch(m -> curFunction){
		case FMENU_TARGETS:
			if(!p) return;
			ptr = GetCompasTarget();
			if(!ptr)
				p -> curItem = p -> get_obj(aciSTR_OFF);
			else
				p -> curItem = p -> get_obj(ptr);

			if(p -> VItems < p -> items -> Size)
				p -> firstItem = p -> curItem;
			else
				p -> firstItem = (fncMenuItem*)p -> items -> first;
			break;
	}
}

void fncMenu::init_submenu(fncMenu* p)
{
	p -> PosX = PosX;
	p -> PosY = PosY;

	p -> SizeX = SizeX;
	p -> SizeY = SizeY;

	p -> ibs = ibs;
	p -> bml = bml;

	p -> flags |= FM_ACTIVE;
	p -> set_redraw();

	p -> upMenu = this;
	subMenu = p;

	flags |= FM_HIDDEN;
}

void fncMenu::sort(void)
{
	int flag = 1;
	fncMenuItem* n,*p,*tmp;
	while(flag){
		flag = 0;
		n = (fncMenuItem*)items -> last;
		p = (fncMenuItem*)n -> prev;
		while(n && p){
			if(n -> fnc_code < p -> fnc_code){
				items -> swap((iListElement*)n,(iListElement*)p);
				tmp = p;
				p = n;
				n = tmp;
				flag = 1;
			}
			n = p;
			if(n)
				p = (fncMenuItem*)n -> prev;
		}
	}
}

int actIntDispatcher::put_item(actintItemData* d)
{
	int id = d -> type;
	invItem* p = get_item(id),*p1;
	if(!p){
		std::cout<<"actIntDispatcher::put_item Item data not present... "<<XERR_USER<<" id:"<<id<<std::endl;
	}

	p1 = alloc_item();
	p -> clone(p1);

	if(!curMatrix -> auto_put_item(p1)){
		aciSendEvent2itmdsp(ACI_DROP_ITEM,d);
		free_item(p1);
		std::cout<<"actIntDispatcher::put_item !curMatrix -> auto_put_item(p1) id:"<<id<<std::endl;
		return 0;
	}
	p1 -> item_ptr = d;
	d -> actintOwner = p1;

	if(!(flags & AS_ISCREEN)){
		if(p1 -> slotType != -1){
			id = curMatrix -> get_item_slot(p1);
			if(id != -1)
				aciSendEvent2itmdsp(ACI_ACTIVATE_ITEM,d,id);
			else
				aciSendEvent2itmdsp(ACI_DEACTIVATE_ITEM,d);
		}
		if(p1 -> menu){
			add_menu_item((fncMenu*)p1 -> menu);
		}
	}
	if(curMode == AS_INV_MODE)
		curMatrix -> set_redraw();

	return 1;
}

int actIntDispatcher::put_item_auto(invItem* p1)
{
	if(!curMatrix -> auto_put_item(p1)){
		return 0;
	}
	p1 -> item_ptr = NULL;

	if(!(flags & AS_ISCREEN)){
		if(p1 -> menu){
			add_menu_item((fncMenu*)p1 -> menu);
		}
	}
	else {
		if(p1 -> uvsDataPtr){
			((uvsActInt*)p1 -> uvsDataPtr) -> pos_x = p1 -> MatrixX;
			((uvsActInt*)p1 -> uvsDataPtr) -> pos_y = p1 -> MatrixY;
		}
	}
	if(curMode == AS_INV_MODE)
		curMatrix -> set_redraw();

	return 1;
}

int actIntDispatcher::put_item_xy(invItem* p,int x,int y,int sflag)
{
	int id = 0;
	actintItemData* d;
	invMatrix* m = (sflag) ? secondMatrix : curMatrix;
	if(m -> check_fit(x,y,p)){
		m -> put_item(x,y,p);
		m -> clear_shadow_cells();
		m -> set_redraw();
		if(flags & AS_INV_MOVE_ITEM)
			restore_mouse_cursor();
		flags &= ~AS_INV_MOVE_ITEM;
		if(p -> item_ptr && p -> slotType != -1){
			id = m -> get_item_slot(p);
			d = p -> item_ptr;
			if(!(flags & AS_ISCREEN)){
				if(id != -1)
					aciSendEvent2itmdsp(ACI_ACTIVATE_ITEM,d,id);
				else
					aciSendEvent2itmdsp(ACI_DEACTIVATE_ITEM,d);
			}
		}
		if(!(flags & AS_ISCREEN) && p -> menu){
			add_menu_item((fncMenu*)p -> menu);
		}
		if(flags & AS_ISCREEN){
			if(p -> uvsDataPtr){
				((uvsActInt*)p -> uvsDataPtr) -> pos_x = p -> MatrixX;
				((uvsActInt*)p -> uvsDataPtr) -> pos_y = p -> MatrixY;
			}
		}
		return 1;
	}
	return 0;
}

void actIntDispatcher::remove_item(actintItemData* d)
{
	invItem* p = (invItem*)d -> actintOwner;
	curMatrix -> remove_item(p);

	if(p -> menu){
		remove_menu_item((fncMenu*)p -> menu);
	}
	free_item(p);

	if(curMode == AS_INV_MODE)
		curMatrix -> set_redraw();
}

void actIntDispatcher::inv_mouse_quant_l(void)
{
	int x,y,index;

	int mx = iMouseX;
	int my = iMouseY;

	if(flags & AS_ISCREEN) mx += iScreenOffs - 800;

	if(curMatrix && curMatrix -> check_xy(mx,my)){
		if(flags & AS_INV_MOVE_ITEM){
			x = iMouseX - curMatrix -> ScreenX - (curItem -> ShapeSizeX >> 1) + curItem -> ShapeCenterX;
			y = iMouseY - curMatrix -> ScreenY - (curItem -> ShapeSizeY >> 1) + curItem -> ShapeCenterY;

			if(flags & AS_ISCREEN)
				x += iScreenOffs - 800;

			y /= aCellSize - (aCellSize >> 2);

			if(y & 0x01) x -= (aCellSize >> 1);
			x /= aCellSize;
		}
		else {
			x = iMouseX - curMatrix -> ScreenX;
			y = iMouseY - curMatrix -> ScreenY;

			if(flags & AS_ISCREEN)
				x += iScreenOffs - 800;

			y /= aCellSize - (aCellSize >> 2);

			if(y & 0x01) x -= (aCellSize >> 1);
			x /= aCellSize;
		}
		index = x + y * curMatrix -> SizeX;

		if(x >= 0 && x < curMatrix -> SizeX && y >= 0 && y < curMatrix -> SizeY){
			if(flags & AS_INV_MOVE_ITEM){
				if(curMatrix -> check_fit(x,y,curItem)){
					if(flags & AS_ISCREEN && curItem -> flags & INV_ITEM_NO_PAY){
						if(!aciCheckItemCredits()) return;
						aciPay4Item(curItem);
					}
					put_item_xy(curItem,x,y);
					SOUND_DROP();
				}
				else {
					change_items(x,y);
					SOUND_PICK();
				}
			}
			else {
				if(curMatrix -> matrix[index] -> flags & AS_BUSY_CELL){
					set_move_item(index);
					SOUND_PICK();
				}
			}
		}
	}
	else {
		if(!(flags & AS_ISCREEN)){
			if(flags & AS_INV_MOVE_ITEM){
				aciSendEvent2itmdsp(ACI_DROP_ITEM,curItem -> item_ptr);
				restore_mouse_cursor();
				if(curItem -> menu){
					remove_menu_item((fncMenu*)curItem -> menu);
				}
				free_item(curItem);
				flags &= ~AS_INV_MOVE_ITEM;
			}
		}
		else {
			if(secondMatrix && secondMatrix -> check_xy(mx,my)){
				if(flags & AS_INV_MOVE_ITEM){
					x = iMouseX - secondMatrix -> ScreenX - (curItem -> ShapeSizeX >> 1) + curItem -> ShapeCenterX;
					y = iMouseY - secondMatrix -> ScreenY - (curItem -> ShapeSizeY >> 1) + curItem -> ShapeCenterY;

					x += iScreenOffs - 800;

					y /= aCellSize - (aCellSize >> 2);

					if(y & 0x01) x -= (aCellSize >> 1);
					x /= aCellSize;
				}
				else {
					x = iMouseX - secondMatrix -> ScreenX;
					y = iMouseY - secondMatrix -> ScreenY;

					x += iScreenOffs - 800;

					y /= aCellSize - (aCellSize >> 2);

					if(y & 0x01) x -= (aCellSize >> 1);
					x /= aCellSize;
				}
				index = x + y * secondMatrix -> SizeX;

				if(x >= 0 && x < secondMatrix -> SizeX && y >= 0 && y < secondMatrix -> SizeY){
					if(flags & AS_INV_MOVE_ITEM){
						if(secondMatrix -> check_fit(x,y,curItem)){
							put_item_xy(curItem,x,y,1);
							SOUND_DROP();
						}
						else {
							change_items(x,y,1);
							SOUND_PICK();
						}
					}
					else {
						if(secondMatrix -> matrix[index] -> flags & AS_BUSY_CELL){
							set_move_item(index,1);
							SOUND_PICK();
						}
					}
				}
			}
		}
	}
}

void actIntDispatcher::inv_mouse_quant_r(void)
{
	int x,y,index;
	invItem* p = NULL;

	int mx = iMouseX;
	int my = iMouseY;

	if(flags & AS_ISCREEN) mx += iScreenOffs - 800;

	if(curMatrix && curMatrix -> check_xy(mx,my)){
		if(flags & AS_INV_MOVE_ITEM){
			x = iMouseX - curMatrix -> ScreenX - (curItem -> ShapeSizeX >> 1) + curItem -> ShapeCenterX;
			y = iMouseY - curMatrix -> ScreenY - (curItem -> ShapeSizeY >> 1) + curItem -> ShapeCenterY;

			if(flags & AS_ISCREEN)
				x += iScreenOffs - 800;

			y /= aCellSize - (aCellSize >> 2);

			if(y & 0x01) x -= (aCellSize >> 1);
			x /= aCellSize;
		}
		else {
			x = iMouseX - curMatrix -> ScreenX;
			y = iMouseY - curMatrix -> ScreenY;

			if(flags & AS_ISCREEN)
				x += iScreenOffs - 800;

			y /= aCellSize - (aCellSize >> 2);

			if(y & 0x01) x -= (aCellSize >> 1);
			x /= aCellSize;
		}
		index = x + y * curMatrix -> SizeX;

		if(x >= 0 && x < curMatrix -> SizeX && y >= 0 && y < curMatrix -> SizeY){
			if(flags & AS_INV_MOVE_ITEM){
				if(curMatrix -> check_fit(x,y,curItem)){
					if(flags & AS_ISCREEN && curItem -> flags & INV_ITEM_NO_PAY){
						if(!aciCheckItemCredits()) return;
						aciPay4Item(curItem);
					}
					put_item_xy(curItem,x,y);
					SOUND_DROP();
				}
			}
			else {
				if(!(flags & AS_ISCREEN)){
					if(curMatrix -> matrix[index] -> flags & AS_BUSY_CELL){
						p = curMatrix -> matrix[index] -> item;
						if(p -> EvCode){
							send_event(p -> EvCode,0,p -> item_ptr);
							SOUND_DROP();
						}
					}
				}
			}
		}
	}
	else {
		if(!(flags & AS_ISCREEN)){
			if(flags & AS_INV_MOVE_ITEM){
				aciSendEvent2itmdsp(ACI_DROP_ITEM,curItem -> item_ptr);
				restore_mouse_cursor();
				if(curItem -> menu){
					remove_menu_item((fncMenu*)curItem -> menu);
				}
				free_item(curItem);
				flags &= ~AS_INV_MOVE_ITEM;
			}
		}
		else {
			if(secondMatrix){
				if(flags & AS_INV_MOVE_ITEM){
					x = iMouseX - secondMatrix -> ScreenX - (curItem -> ShapeSizeX >> 1) + curItem -> ShapeCenterX;
					y = iMouseY - secondMatrix -> ScreenY - (curItem -> ShapeSizeY >> 1) + curItem -> ShapeCenterY;

					x += iScreenOffs - 800;

					y /= aCellSize - (aCellSize >> 2);

					if(y & 0x01) x -= (aCellSize >> 1);
					x /= aCellSize;
				}
				else {
					x = iMouseX - secondMatrix -> ScreenX;
					y = iMouseY - secondMatrix -> ScreenY;

					x += iScreenOffs - 800;

					y /= aCellSize - (aCellSize >> 2);

					if(y & 0x01) x -= (aCellSize >> 1);
					x /= aCellSize;
				}
				index = x + y * secondMatrix -> SizeX;

				if(x >= 0 && x < secondMatrix -> SizeX && y >= 0 && y < secondMatrix -> SizeY){
					if(flags & AS_INV_MOVE_ITEM){
						if(secondMatrix -> check_fit(x,y,curItem)){
							put_item_xy(curItem,x,y,1);
						}
						SOUND_DROP();
					}
					else {
						if(secondMatrix -> matrix[index] -> flags & AS_BUSY_CELL){
							p = secondMatrix -> matrix[index] -> item;
						}
					}
				}
			}
		}
	}
}

bmlObject* actIntDispatcher::get_back_bml(int id)
{
	bmlObject* p = (bmlObject*)backList -> last;
	while(p){
		if(p -> ID == id)
			return p;
		p = (bmlObject*)p -> prev;
	}
	return NULL;
}

ibsObject* actIntDispatcher::get_ibs(int id)
{
	ibsObject* p = (ibsObject*)ibsList -> last;
	while(p){
		if(p -> ID == id)
			return p;
		p = (ibsObject*)p -> prev;
	}
	return NULL;
}

void actIntDispatcher::change_ibs(int id)
{
	if(curIbs){
		curIbs -> free();
//		  if(curIbs -> back) curIbs -> back -> free();
	}
	curIbs = get_ibs(id);
	if(!curIbs) ErrH.Abort("IBS object not present",XERR_USER,id);
	load_ibs();
}

void actIntDispatcher::next_ibs(void)
{
	if(curIbs){
		curIbs -> free();
		for(int i = 0; i < curIbs -> backs.size(); i++){
			curIbs -> backs[i] -> free();
		}
	}
	curIbs = (ibsObject*)curIbs -> next;
	if(!curIbs) ErrH.Abort("IBS object not present");
	load_ibs();
}

void actIntDispatcher::load_ibs(void)
{
	if(!curIbs)
		ErrH.Abort("curIBS object not present");

	curIbsID = curIbs -> ID;
	curIbs -> load();
	init_inds();
}

void actIntDispatcher::free_ibs(void)
{
	if(!curIbs)
		ErrH.Abort("curIBS object not present");

	curIbs -> free();
}

void actIntDispatcher::set_move_item(int index,int sflag)
{
	
	invMatrix* m = (sflag) ? secondMatrix : curMatrix;

	invItem* p = m -> matrix[index] -> item;
	
	m -> remove_item(p);

	m -> put_item_shadow(p -> MatrixX,p -> MatrixY,p);

	m -> set_redraw();
	curItem = p;
	set_mouse_cursor(p -> frame,p -> ScreenSizeX,p -> ScreenSizeY);
	if(!(flags & AS_ISCREEN)){
		if(p -> slotType != -1)
			aciSendEvent2itmdsp(ACI_DEACTIVATE_ITEM,p -> item_ptr);
	}
	else {
		if(p -> uvsDataPtr){
			((uvsActInt*)p -> uvsDataPtr) -> pos_x = -1;
			((uvsActInt*)p -> uvsDataPtr) -> pos_y = -1;

			p -> MatrixX = p -> MatrixY = -1;
		}
	}
	if(iP){
		iP -> free_list();
		iP -> add_item(curItem->ID_ptr.c_str(),-1,aciCurColorScheme[FM_SELECT_COL]);
		if(!(flags & AS_ISCREEN)){
			iP -> add_items(curItem -> numComments,curItem -> comments);
			if(curItem -> pTemplate)
				iP -> add_item(aciGetItemLoad(curItem,0));
		}
		else {
			iP -> add_items(curItem -> numComments,curItem -> comments);
			if(curItem -> pTemplate)
				iP -> add_item(aciGetItemLoad(curItem,1));
			if(!uvsCurrentWorldUnable)
				iP -> add_item(aciGetPrice((iListElement*)curItem,ITEMS_MODE,1));
		}
		iP -> set_redraw();
	}
	flags |= AS_INV_MOVE_ITEM;
}

void actIntDispatcher::change_items(int x,int y,int sflag)
{
	actintItemData* d;
	int id;

	invMatrix* m = (sflag) ? secondMatrix : curMatrix;

	invItem* p = m -> get_area_item(x,y,curItem);
	if(!p) return;

	m -> remove_item(p);
	if(!m -> check_fit(x,y,curItem)){
		m -> put_item(p -> MatrixX,p -> MatrixY,p);
		return;
	}
	if(flags & AS_ISCREEN && curItem -> flags & INV_ITEM_NO_PAY){
		if(!aciCheckItemCredits()){
			m -> put_item(p -> MatrixX,p -> MatrixY,p);
			return;
		}
		aciPay4Item(curItem);
	}
	m -> put_item(x,y,curItem);
	m -> set_redraw();

	if(!(flags & AS_ISCREEN)){
		if(p -> slotType != -1)
			aciSendEvent2itmdsp(ACI_DEACTIVATE_ITEM,p -> item_ptr);
	}

	if(curItem -> slotType != -1){
		if(!(flags & AS_ISCREEN)){
			id = m -> get_item_slot(curItem);
			d = curItem -> item_ptr;
			if(id != -1)
				aciSendEvent2itmdsp(ACI_ACTIVATE_ITEM,d,id);
			else
				aciSendEvent2itmdsp(ACI_DEACTIVATE_ITEM,d);
		}
	}

	m -> clear_shadow_cells();

	flags |= aMS_MOVED;

	if(!(flags & AS_ISCREEN)){
		if(p -> menu)
			remove_menu_item((fncMenu*)p -> menu);
		if(curItem -> menu)
			add_menu_item((fncMenu*)curItem -> menu);
	}
	else {
		if(curItem -> uvsDataPtr){
			((uvsActInt*)curItem -> uvsDataPtr) -> pos_x = curItem -> MatrixX;
			((uvsActInt*)curItem -> uvsDataPtr) -> pos_y = curItem -> MatrixY;
		}
		if(p -> uvsDataPtr){
			((uvsActInt*)p -> uvsDataPtr) -> pos_x = -1;
			((uvsActInt*)p -> uvsDataPtr) -> pos_y = -1;

			p -> MatrixX = p -> MatrixY = -1;
		}
	}

	curItem = p;

	if(iP){
		iP -> free_list();
		iP -> add_item(curItem->ID_ptr.c_str(),-1,aciCurColorScheme[FM_SELECT_COL]);
		if(!(flags & AS_ISCREEN)){
			iP -> add_items(curItem -> numComments,curItem -> comments);
			if(curItem -> pTemplate)
				iP -> add_item(aciGetItemLoad(curItem,0));
		}
		else {
			iP -> add_items(curItem -> numComments,curItem -> comments);
			if(curItem -> pTemplate)
				iP -> add_item(aciGetItemLoad(curItem,1));
			if(!uvsCurrentWorldUnable)
				iP -> add_item(aciGetPrice((iListElement*)curItem,ITEMS_MODE,1));
		}
		iP -> set_redraw();
	}

	set_mouse_cursor(p -> frame,p -> ScreenSizeX,p -> ScreenSizeY);
//	  inv_mouse_move_quant();
}

void aIndData::alloc_mem(void)
{
	int i;

	if(check_points) {
		delete[] check_points;
	}
	if(value) {
		delete[] value;
	}
	if(value_ptr) {
		delete[] value_ptr;
	}
	if(max_value) {
		delete[] max_value;
	}
	if(colors) {
		delete[] colors;
	}

	check_points = new int[NumVals];
	value = new int[NumVals];
	value_ptr = new int*[NumVals];
	max_value = new int*[NumVals];
	colors = new int[NumVals];

	check_points[0] = 0;

	for(i = 0; i < NumVals; i ++){
		value[i] = -1;
		colors[i] = 0;
	}
}

void aIndData::init(void)
{
	int i;
	SizeX = aIndArrowBML -> SizeX;
	SizeY = aIndArrowBML -> SizeY;

	if(pict_buf) {
		delete[] pict_buf;
	}
	pict_buf = new char[SizeX * SizeY];

	if(bml) bml -> load();

	for(i = 0; i < NumVals; i ++)
		value[i] = -1;

	switch(ID){
		case IND_N0:
		case IND_N1:
			value_ptr[0] = &aci_tmp;
			max_value[0] = &aci_tmp_max;
			colors[0] = aciCurColorScheme[IND_N0_COL];
			break;
		case IND_SPIRAL:
			value_ptr[0] = &aciCurJumpCount;
			max_value[0] = &aciMaxJumpCount;
			colors[0] = ACI_IND_SPIRAL_COL | (ACI_IND_SPIRAL_SIZE << 16);
			flags |= IND_SHOW_VALUES;
			break;
		case IND_NRG:
			value_ptr[1] = &aciCurEnergy;
			max_value[1] = &aciMaxEnergy;

			value_ptr[0] = &aciCurArmor;
			max_value[0] = &aciMaxArmor;

			check_points[0] = 0;
			check_points[1] = 64;

			colors[0] = ACI_IND_NRG0_COL | (ACI_IND_NRG0_SIZE << 16);
			colors[1] = ACI_IND_NRG1_COL | (ACI_IND_NRG1_SIZE << 16);
			break;
		case IND_JUMP:
			value_ptr[0] = &aciCurJumpImpulse;
			max_value[0] = &aciMaxJumpImpulse;
			colors[0] = ACI_IND_JUMP_COL | (ACI_IND_JUMP_SIZE << 16);
			break;
		case IND_LOAD:
			value_ptr[0] = &aciCurLoad;
			max_value[0] = &aciMaxLoad;
			colors[0] = ACI_IND_LOAD_COL | (ACI_IND_LOAD_SIZE << 16);
			break;
		case IND_WPN:
			value_ptr[0] = &aciCurWpnResource;
			max_value[0] = &aciMaxWpnResource;
			colors[0] = aciCurColorScheme[IND_WPN_COL];
			break;
		case IND_DVC:
			value_ptr[0] = &aciCurDvcResource;
			max_value[0] = &aciMaxDvcResource;
			colors[0] = ACI_IND_DVC_COL | (ACI_IND_DVC_SIZE << 16);
			break;
	}
}

void aIndData::finit(void)
{
	if(pict_buf){
		delete[] pict_buf;
		pict_buf = NULL;
	}
	if(bml) bml -> free();
}

void aciWorldMap::init(void)
{
	int i;
	for(i = 0; i < AS_MAX_WORLD; i ++){
		world_ptr[i] = get_world(i);
	}
	if(!redraw_buf) redraw_buf = new unsigned char[SizeX * SizeY];
	calc_shapes();
	remap_coords();
}

aciWorldInfo* aciWorldMap::get_world(int id)
{
	aciWorldInfo* p = (aciWorldInfo*)world_list -> last;
	while(p){
		if(p -> ID == id) return p;
		p = (aciWorldInfo*)p -> prev;
	}
	return NULL;
}

aciWorldInfo* aciWorldMap::get_world_p(char* ptr)
{
	aciWorldInfo* p = (aciWorldInfo*)world_list -> last;
	while(p){
		if(p -> name && !strcmp(p -> name,ptr)) return p;
		p = (aciWorldInfo*)p -> prev;
	}
	return NULL;
}

void aciWorldMap::quant(void)
{
	if(aciPrevJumpCount != aciCurJumpCount){
		aciWorldLinksOFF();
		if(aciCurJumpCount)
			aciWorldLinksON();
	}
	aciPrevJumpCount = aciCurJumpCount;

}

void aciWorldMap::redraw(int x,int y)
{
	quant();
	if(flags & WMAP_REBUILD_PIC)
		build_map();
//	  show_jump_count();
	XGR_PutSpr(x,y,SizeX,SizeY,redraw_buf,XGR_BLACK_FON);
}

void aciWorldMap::build_map(void)
{
	int i;
	aciWorldInfo* p;

	flags &= ~WMAP_REBUILD_PIC;
	memset(redraw_buf,aciCurColorScheme[ACI_BACK_COL],SizeX * SizeY);

	aciWorldExploredInfo();

	p = (aciWorldInfo*)world_list -> last;
	while(p){
		for(i = 0; i < AS_MAX_WORLD; i ++){
			if(i != p -> ID && p -> links[i]) show_link(p -> ID,i);
		}
		p = (aciWorldInfo*)p -> prev;
	}

	p = (aciWorldInfo*)world_list -> last;
	while(p){
		p -> redraw(SizeX,SizeY,redraw_buf);
		p = (aciWorldInfo*)p -> prev;
	}
}

void aIndData::build_pict(void)
{
	int ind,v,val,p1,p2,dx,dy,col,col_sz;

	memset(pict_buf,0,SizeX * SizeY);

	if(flags & IND_DISABLED){
		dx = (SizeX - aIndBackBML -> SizeX) >> 1;
		dy = (SizeY - aIndBackBML -> SizeY) >> 1;
		aciPutBufSpr(dx,dy,aIndBackBML -> SizeX,aIndBackBML -> SizeY,SizeX,aIndBackBML -> frames + aIndBackBML -> SizeX * aIndBackBML -> SizeY * (aIndBackBML -> Size - 1),pict_buf,ACI_IND_ARROW_START,ACI_IND_ARROW_SIZE);
	}
	else {
		for(ind = NumVals - 1; ind >= 0; ind --){
			p1 = check_points[ind];
			p2 = (ind == NumVals - 1) ? 256 : check_points[ind + 1];

			p1 = (p1 * (aIndDataBML -> Size - 1)) >> 8;
			p2 = (p2 * (aIndDataBML -> Size - 1)) >> 8;

			value[ind] = *value_ptr[ind];
			if(value[ind] >= *max_value[ind]) value[ind] = (*max_value[ind]) - 1;

			if((*max_value[ind]) > 1)
				val = (value[ind] * (p2 - p1)) / ((*max_value[ind]) - 1) + p1;
			else
				val = 0;

			col = colors[ind] & 0xFFFF;
			col_sz = (colors[ind] >> 16) & 0xFFFF;

			if(val >= aIndDataBML -> Size) val = aIndDataBML -> Size - 1;
			if(val < 0) val = 0;

			if(p2 < 0) p2 = 0;
			if(p2 >= aIndDataBML -> Size) p2 = aIndDataBML -> Size - 1;

			dx = (SizeX - aIndDataBML -> SizeX) >> 1;
			dy = (SizeY - aIndDataBML -> SizeY) >> 1;

			aciPutBufSpr(dx,dy,aIndDataBML -> SizeX,aIndDataBML -> SizeY,SizeX,aIndDataBML -> frames + aIndDataBML -> SizeX * aIndDataBML -> SizeY * p2,pict_buf,ACI_IND_FON_START,ACI_IND_FON_SIZE);
			aciPutBufSpr(dx,dy,aIndDataBML -> SizeX,aIndDataBML -> SizeY,SizeX,aIndDataBML -> frames + aIndDataBML -> SizeX * aIndDataBML -> SizeY * val,pict_buf,col,col_sz);

			aciPutBufSpr(0,0,aIndArrowBML -> SizeX,aIndArrowBML -> SizeY,SizeX,aIndArrowBML -> frames + aIndArrowBML -> SizeX * aIndArrowBML -> SizeY * p2,pict_buf,ACI_IND_ARROW_START,ACI_IND_ARROW_SIZE);
		}
		aciPutBufSpr(0,0,aIndArrowBML -> SizeX,aIndArrowBML -> SizeY,SizeX,aIndArrowBML -> frames,pict_buf,ACI_IND_ARROW_START,ACI_IND_ARROW_SIZE);

		if(flags & IND_SHOW_VALUES){
			for(ind = 0; ind < NumVals; ind ++){
				for(v = 0; v < *max_value[ind]; v ++){
					if((*max_value[ind]) > 1)
						val = (v * aIndArrowBML -> Size) / ((*max_value[ind]) - 1);
					else
						val = 0;

					if(val < 0) val = 0;
					if(val >= aIndArrowBML -> Size) val = aIndArrowBML -> Size - 1;
					aciPutBufSpr(0,0,aIndArrowBML -> SizeX,aIndArrowBML -> SizeY,SizeX,aIndArrowBML -> frames + aIndArrowBML -> SizeX * aIndArrowBML -> SizeY * val,pict_buf,ACI_IND_ARROW_START,ACI_IND_ARROW_SIZE);
				}
			}
		}
	}
}

void aIndData::redraw(int dx,int dy)
{
	int i,x,y;
	for(i = 0; i < NumVals; i ++){
		if(*value_ptr[i] != value[i]){
			build_pict();
			i = NumVals;
		}
	}
	if(dx == -1){
		XGR_PutSpr(PosX,PosY,SizeX,SizeY,pict_buf,XGR_HIDDEN_FON);
		if(bml) bml -> show();
	}
	else {
		switch(CornerNum){
			case IND_UP_LEFT:
				x = dx;
				y = dy;
				break;
			case IND_UP_RIGHT:
				x = XGR_MAXX - dx - SizeX;
				y = dy;
				break;
			case IND_DN_RIGHT:
				x = XGR_MAXX - dx - SizeX;
				y = XGR_MAXY - dy - SizeY;
				break;
			case IND_DN_LEFT:
				x = dx;
				y = XGR_MAXY - dy - SizeY;
				break;
		}
		XGR_PutSpr(x,y,SizeX,SizeY,pict_buf,XGR_HIDDEN_FON);
	}
}

void actIntDispatcher::init_inds(void)
{
	int lx,ly,rx,ry,cx,cy;
	ibsObject* ibs = curIbs;
	XGR_MousePromptData* pr;

	lx = 0;
	ly = 0;

	rx = XGR_MAXX;
	ry = XGR_MAXY;

	aIndData* p = (aIndData*)indList -> last;
	while(p){
		switch(p -> CornerNum){
			case IND_UP_LEFT:
				p -> PosX = lx + ibs -> indPosX[0];
				p -> PosY = ly + ibs -> indPosY[0];
				break;
			case IND_UP_RIGHT:
				p -> PosX = rx - ibs -> indPosX[1] - p -> SizeX;
				p -> PosY = ly + ibs -> indPosY[1];
				break;
			case IND_DN_LEFT:
				p -> PosX = lx + ibs -> indPosX[2];
				p -> PosY = ry - ibs -> indPosY[2] - p -> SizeY;
				break;
			case IND_DN_RIGHT:
				p -> PosX = rx - ibs -> indPosX[3] - p -> SizeX;
				p -> PosY = ry - ibs -> indPosY[3] - p -> SizeY;
				break;
		}
		if(p -> bml){
			cx = p -> PosX + (p -> SizeX >> 1);
			cy = p -> PosY + (p -> SizeY >> 1);
			p -> bml -> OffsX = cx - (p -> bml -> SizeX >> 1);
			p -> bml -> OffsY = cy - (p -> bml -> SizeY >> 1);
		}
		if(p -> ID == IND_DVC){
			if(curMatrix && !curMatrix -> slot_exist(3)){
				p -> flags |= IND_DISABLED;
			}
			else {
				p -> flags &= ~IND_DISABLED;
			}
		}
		pr = invPrompt -> getData(p -> ID);
		if(pr){
			if(p -> flags & IND_DISABLED){
				pr -> StartX = 0;
				pr -> StartY = 0;

				pr -> SizeX = 0;
				pr -> SizeY = 0;
			}
			else {
				pr -> StartX = p -> PosX;
				pr -> StartY = p -> PosY;

				pr -> SizeX = p -> SizeX;
				pr -> SizeY = p -> SizeY;
			}
		}
		pr = infPrompt -> getData(p -> ID);
		if(pr){
			if(p -> flags & IND_DISABLED){
				pr -> StartX = 0;
				pr -> StartY = 0;

				pr -> SizeX = 0;
				pr -> SizeY = 0;
			}
			else {
				pr -> StartX = p -> PosX;
				pr -> StartY = p -> PosY;

				pr -> SizeX = p -> SizeX;
				pr -> SizeY = p -> SizeY;
			}
		}
		p = (aIndData*)p -> prev;
	}
}

void invItem::calc_shape_center(void)
{
	int i,x,y,min_x,min_y,max_x,max_y;
	int cell_sz,cell_sz2,cell_sz4;

	int* tmpShapeX;
	int* tmpShapeY;

	x = y = 0;
	min_x = min_y = max_x = max_y = 0;

	cell_sz = aCellSize;
	cell_sz2 = aCellSize/2;
	cell_sz4 = aCellSize/4;

	if(ShapeLen){
		tmpShapeX = new int[ShapeLen];
		tmpShapeY = new int[ShapeLen];

		for(i = 0; i < ShapeLen; i ++){
			tmpShapeX[i] = ShapeX[i] * 2;
			tmpShapeY[i] = ShapeY[i];

			if(ShapeY[i] & 1)
				tmpShapeX[i] ++;

			x = tmpShapeX[i];
			y = tmpShapeY[i];

			if(x < min_x){
				min_x = x;
			}
			if(y < min_y){
				min_y = y;
			}

			if(x > max_x){
				max_x = x;
			}
			if(y > max_y){
				max_y = y;
			}
		}
		for(i = 0; i < ShapeLen; i ++){
			tmpShapeX[i] -= min_x;
			tmpShapeY[i] -= min_y;
		}
	}

	ShapeSizeX = cell_sz + (max_x - min_x) * cell_sz2;
	ShapeSizeY = cell_sz + (max_y - min_y) * (cell_sz - cell_sz4);

	if(ShapeLen){
        ShapeCenterX = (tmpShapeX[0] + 1) * cell_sz2;
        ShapeCenterY = tmpShapeY[0] * (cell_sz - cell_sz4) + cell_sz2;
        
		delete[] tmpShapeX;
		delete[] tmpShapeY;
	}
}

/*
void invItem::calc_shape_center(void)
{
	int i,x,y,min_x,min_y,min_index_x,min_index_y,max_x,max_y,max_index_x,max_index_y,sy;

	x = y = 0;
	min_x = min_y = max_x = max_y = 0;
	min_index_x = min_index_y = max_index_x = max_index_y = 0;

	for(i = 0; i < ShapeLen; i ++){
		x = ShapeX[i];
		y = ShapeY[i];

		if(x < min_x){
			min_x = x;
			min_index_x = i;
		}
		if(y < min_y){
			min_y = y;
			min_index_y = i;
		}

		if(x > max_x){
			max_x = x;
			max_index_x = i;
		}
		if(y > max_y){
			max_y = y;
			max_index_y = i;
		}
	}

	sy = max_y - min_y + 1;

	ShapeSizeX = (max_x - min_x + 1) * aCellSize;
	ShapeSizeY = aCellSize + (sy - 1) * (aCellSize - (aCellSize >> 2));

	ShapeCenterX = -min_x * aCellSize + (aCellSize >> 1);
	if(ShapeY[min_index_x] & 0x01) ShapeCenterX -= (aCellSize >> 1);
	ShapeCenterY = -min_y * (aCellSize - (aCellSize >> 2)) + (aCellSize >> 1);
}
*/

#if defined(_GENERATE_MATRIX_SHAPES_) || defined(_GENERATE_iMATRIX_SHAPES_)
void invMatrix::generate_shape(void)
{
	int i,j,x0,y0,sz = SizeX * SizeY,x = 0,y = 0,col = 0,col1 = 0,index = 0;
	int ssx,ssy,ssz;

	unsigned char pal_buf[768];

	XStream fh;
	XBuffer XBuf;
	unsigned char* buf;

	init();

	ssx = ScreenSizeX + aCellSize;
	ssy = ScreenSizeY + aCellSize;
	ssz = ssx * ssy;

	XBuf < "actint/temp/m";
	if(internalID < 10) XBuf < "0";
	XBuf <= internalID < ".bmp";

	for(i = 0; i < 64; i ++){
		pal_buf[i * 3] = pal_buf[i * 3 + 1] = pal_buf[i * 3 + 2] = i;
	}

	buf = new unsigned char[ssz];
	memset(buf,0,ssz);

	int dx = aCellSize;
	int dx2 = aCellSize >> 1;
	int dx4 = aCellSize >> 2;

	unsigned char* p;
	p = new unsigned char[(dx + 1) * (dx + 1)];

	for(i = 0; i < SizeY; i ++){
		if(i & 0x01){
			x += dx2;
		}
		for(j = 0; j < SizeX; j ++){
			if(matrix[index] -> type){
				memcpy(p,aCellFrame,(dx + 1) * (dx + 1));

				x0 = x + dx2;
				y0 = y + dx2;

				mem_putspr(x0,y0,dx + 1,dx + 1,ssx,ssy,p,buf);
			}
			x += aCellSize;
			index ++;
		}
		x = 0;
		y += aCellSize - dx4;
	}

	delete[] p;

	smooth_shape_quant(ssx,ssy,buf,0,SHAPE_DEPTH + 83,1);
	for(i = SHAPE_DEPTH - 1; i > 0; i --)
		smooth_shape_quant(ssx,ssy,buf,0,i + 83,i + 1 + 83);

	swap_buf_col(1,0,ssx,ssy,buf);
	swap_buf_col(2,0,ssx,ssy,buf);

	for(i = SHAPE_DEPTH; i > 0; i --)
		swap_buf_col(i + 83,SHAPE_DEPTH,ssx,ssy,buf);

	fh.open(XBuf,XS_OUT);
	fh < (short)ssx < (short)ssy;
	fh.write(buf,ssz);
	fh.close();

	XBuf.init();
	XBuf < "actint/temp/m";
	if(internalID < 10) XBuf < "0";
	XBuf <= internalID < ".shp";

	int scr_sx = (actintLowResFlag) ? 640 : 800;
	int scr_sy = (actintLowResFlag) ? 480 : 600;

	fh.open(XBuf,XS_OUT);
	fh < "X0: " <= (512 - scr_sx/2 + ScreenX + ScreenSizeX/2) < "\r\n";
	fh < "Y0: " <= (512 - scr_sy/2 + ScreenY + ScreenSizeY/2) < "\r\n\r\n";

	fh < "X1: " <= (1536 - scr_sx/2 + ScreenX + ScreenSizeX/2) < "\r\n";
	fh < "Y1: " <= (512 - scr_sy/2 + ScreenY + ScreenSizeY/2) < "\r\n\r\n";

	fh < "X2: " <= (512 - scr_sx/2 + ScreenX + ScreenSizeX/2) < "\r\n";
	fh < "Y2: " <= (1536 - scr_sy/2 + ScreenY + ScreenSizeY/2) < "\r\n\r\n";

	fh < "X3: " <= (1536 - scr_sx/2 + ScreenX + ScreenSizeX/2) < "\r\n";
	fh < "Y3: " <= (1536 - scr_sy/2 + ScreenY + ScreenSizeY/2) < "\r\n\r\n";

	fh < "iX: " <= (1200 - 800/2 + ScreenX + ScreenSizeX/2) < "\r\n";
	fh < "iY: " <= (300 - 600/2 + ScreenY + ScreenSizeY/2) < "\r\n\r\n";
	fh.close();

	XBuf.init();
	XBuf < "actint/temp/m";
	if(internalID < 10) XBuf < "0";
	XBuf <= internalID < ".pal";

	fh.open(XBuf,XS_OUT);
	fh.write(pal_buf,768);
	fh.close();

	delete buf;
}

void invMatrix::generate_floor(void)
{
	int i,j,x0,y0,sz = SizeX * SizeY,x = 0,y = 0,col = 0,col1 = 0,index = 0;
	int ssx,ssy,ssz;

	unsigned char pal_buf[768];

	XStream fh;
	XBuffer XBuf;
	unsigned char* buf;

	init();

	ssx = ScreenSizeX + aCellSize;
	ssy = ScreenSizeY + aCellSize;
	ssz = ssx * ssy;

	XBuf < "actint/temp/f";
	if(internalID < 10) XBuf < "0";
	XBuf <= internalID < ".bmp";

	for(i = 0; i < 64; i ++){
		pal_buf[i * 3] = pal_buf[i * 3 + 1] = pal_buf[i * 3 + 2] = i;
	}

	buf = new unsigned char[ssz];
	memset(buf,0,ssz);

	int dx = aCellSize;
	int dx2 = aCellSize >> 1;
	int dx4 = aCellSize >> 2;

	unsigned char* p;
	p = new unsigned char[(dx + 1) * (dx + 1)];

	for(i = 0; i < SizeY; i ++){
		if(i & 0x01){
			x += dx2;
		}
		for(j = 0; j < SizeX; j ++){
			if(matrix[index] -> type){
				memcpy(p,aCellFrame,(dx + 1) * (dx + 1));

				x0 = x + dx2;
				y0 = y + dx2;

				mem_putspr(x0,y0,dx + 1,dx + 1,ssx,ssy,p,buf);
			}
			x += aCellSize;
			index ++;
		}
		x = 0;
		y += aCellSize - dx4;
	}

#ifdef _FACET_FLOOR_
//	swap_buf_col(1,0,ssx,ssy,buf);
	swap_buf_col(2,0,ssx,ssy,buf);
#else
	swap_buf_col(2,1,ssx,ssy,buf);
#endif

	delete[] p;

	fh.open(XBuf,XS_OUT);
	fh < (short)ssx < (short)ssy;
	fh.write(buf,ssz);
	fh.close();

	XBuf.init();
	XBuf < "actint/temp/f";
	if(internalID < 10) XBuf < "0";
	XBuf <= internalID < ".shp";

	int scr_sx = (actintLowResFlag) ? 640 : 800;
	int scr_sy = (actintLowResFlag) ? 480 : 600;

	fh.open(XBuf,XS_OUT);
	fh < "X0: " <= (512 - scr_sx/2 + ScreenX + ScreenSizeX/2) < "\r\n";
	fh < "Y0: " <= (512 - scr_sy/2 + ScreenY + ScreenSizeY/2) < "\r\n\r\n";

	fh < "X1: " <= (1536 - scr_sx/2 + ScreenX + ScreenSizeX/2) < "\r\n";
	fh < "Y1: " <= (512 - scr_sy/2 + ScreenY + ScreenSizeY/2) < "\r\n\r\n";

	fh < "X2: " <= (512 - scr_sx/2 + ScreenX + ScreenSizeX/2) < "\r\n";
	fh < "Y2: " <= (1536 - scr_sy/2 + ScreenY + ScreenSizeY/2) < "\r\n\r\n";

	fh < "X3: " <= (1536 - scr_sx/2 + ScreenX + ScreenSizeX/2) < "\r\n";
	fh < "Y3: " <= (1536 - scr_sy/2 + ScreenY + ScreenSizeY/2) < "\r\n\r\n";

	fh < "iX: " <= (1200 - 800/2 + ScreenX + ScreenSizeX/2) < "\r\n";
	fh < "iY: " <= (300 - 600/2 + ScreenY + ScreenSizeY/2) < "\r\n\r\n";
	fh.close();

	XBuf.init();
	XBuf < "actint/temp/f";
	if(internalID < 10) XBuf < "0";
	XBuf <= internalID < ".pal";

	fh.open(XBuf,XS_OUT);
	fh.write(pal_buf,768);
	fh.close();

	delete buf;
}
#endif

void actIntDispatcher::inv_mouse_move_quant(void)
{
	int x,y,ix,iy,isx,isy,id = -1;
	invItem* p = NULL;
	XBuffer* XBuf;
	char* ptr;

	aButton* bt;

	XGR_MousePromptData* pr = NULL,*bt_pr0,*bt_pr1;
	if(XGR_MouseObj.promptData){
		pr = XGR_MouseObj.promptData -> getData(ACI_ITEM_PROMPT);
	}
	if(pr) pr -> StartX = pr -> StartY = pr -> SizeX = pr -> SizeY = 0;

	if(curMode != AS_INV_MODE)
		return;

	bt = get_button(ACI_WPN_PICKUP_BUTTON);
	if(bt){
		if(XGR_MouseObj.promptData){
			bt_pr0 = XGR_MouseObj.promptData -> getData(ACI_PICKUP_WPN_ON);
			bt_pr1 = XGR_MouseObj.promptData -> getData(ACI_PICKUP_WPN_OFF);
			if(!bt_pr1){
				bt_pr1 = new XGR_MousePromptData;
				bt_pr1 -> set_text(aciSTR_PICKUP_WEAPONS_OFF);
				bt_pr1 -> ID = ACI_PICKUP_WPN_OFF;
				XGR_MouseObj.promptData -> AddElement((XListElement*)bt_pr1);
			}
			if(bt -> flags & B_PRESSED){
				if(bt_pr0) bt_pr0 -> StartX = bt_pr0 -> StartY = bt_pr0 -> SizeX = bt_pr0 -> SizeY = 0;
				bt_pr1 -> StartX = bt -> PosX;
				bt_pr1 -> StartY = bt -> PosY;

				bt_pr1 -> SizeX = bt -> SizeX;
				bt_pr1 -> SizeY = bt -> SizeY;
			}
			else {
				if(bt_pr1) bt_pr1 -> StartX = bt_pr1 -> StartY = bt_pr1 -> SizeX = bt_pr1 -> SizeY = 0;
				if(bt_pr0){
					bt_pr0 -> StartX = bt -> PosX;
					bt_pr0 -> StartY = bt -> PosY;

					bt_pr0 -> SizeX = bt -> SizeX;
					bt_pr0 -> SizeY = bt -> SizeY;
				}
			}
		}
	}
	bt = get_button(ACI_ITM_PICKUP_BUTTON);
	if(bt){
		if(XGR_MouseObj.promptData){
			bt_pr0 = XGR_MouseObj.promptData -> getData(ACI_PICKUP_ITM_ON);
			bt_pr1 = XGR_MouseObj.promptData -> getData(ACI_PICKUP_ITM_OFF);
			if(!bt_pr1){
				bt_pr1 = new XGR_MousePromptData;
				bt_pr1 -> set_text(aciSTR_PICKUP_ITEMS_OFF);
				bt_pr1 -> ID = ACI_PICKUP_ITM_OFF;
				XGR_MouseObj.promptData -> AddElement((XListElement*)bt_pr1);
			}
			if(bt -> flags & B_PRESSED){
				if(bt_pr0) bt_pr0 -> StartX = bt_pr0 -> StartY = bt_pr0 -> SizeX = bt_pr0 -> SizeY = 0;
				bt_pr1 -> StartX = bt -> PosX;
				bt_pr1 -> StartY = bt -> PosY;

				bt_pr1 -> SizeX = bt -> SizeX;
				bt_pr1 -> SizeY = bt -> SizeY;
			}
			else {
				if(bt_pr1) bt_pr1 -> StartX = bt_pr1 -> StartY = bt_pr1 -> SizeX = bt_pr1 -> SizeY = 0;
				if(bt_pr0){
					bt_pr0 -> StartX = bt -> PosX;
					bt_pr0 -> StartY = bt -> PosY;

					bt_pr0 -> SizeX = bt -> SizeX;
					bt_pr0 -> SizeY = bt -> SizeY;
				}
			}
		}
	}

	if(flags & aMS_MOVED)
		curMatrix -> clear_shadow_cells();

	x = iMouseX;
	y = iMouseY;

	ix = 0;
	iy = 0;

	isx = ix + XGR_MAXX;
	isy = iy + XGR_MAXY;

	if(x >= ix && x < isx && y >= iy && y < isy){
		id = aciGetScreenItem(x,y);
		if(id != -1){
			p = get_item(id);
			if(iP){
				if(p){
					iP -> free_list();
					iP -> add_item(p->ID_ptr.c_str(),-1,aciCurColorScheme[FM_SELECT_COL]);
					iP -> add_items(p -> numComments,p -> comments);
					iP -> set_redraw();
				}
				else {
					XBuf = new XBuffer;
					*XBuf < "Undefined item, ID = " <= id;
					ptr = XBuf -> address();
					iP -> free_list();
					iP -> add_item(ptr,-1,aciCurColorScheme[FM_SELECT_COL]);
					iP -> set_redraw();
					delete XBuf;
				}
			}
		}
		if(id == -1){
			if(aciGetScreenMechos(x,y)){
				if(iP -> items -> Size){
					iP -> free_list();
					iP -> set_redraw();
				}
				if(curMatrix && curMatrix -> mech_name)
					iP -> add_item(curMatrix -> mech_name,-1,aciCurColorScheme[FM_SELECT_COL]);
			}
			else {
				if(iP -> items -> Size){
					iP -> free_list();
					iP -> set_redraw();
				}
			}
		}
	}
	else {
		if(curMatrix && curMatrix -> check_xy(iMouseX,iMouseY)){
			if(flags & AS_INV_MOVE_ITEM){
				x = iMouseX - curMatrix -> ScreenX - (curItem -> ShapeSizeX >> 1) + curItem -> ShapeCenterX;
				y = iMouseY - curMatrix -> ScreenY - (curItem -> ShapeSizeY >> 1) + curItem -> ShapeCenterY;

				y /= aCellSize - (aCellSize >> 2);

				if(y & 0x01) x -= (aCellSize >> 1);
				x /= aCellSize;
			}
			else {
				x = iMouseX - curMatrix -> ScreenX;
				y = iMouseY - curMatrix -> ScreenY;

				y /= aCellSize - (aCellSize >> 2);

				if(y & 0x01) x -= (aCellSize >> 1);
				x /= aCellSize;
			}

			if(x >= 0 && x < curMatrix -> SizeX && y >= 0 && y < curMatrix -> SizeY){
				if(flags & AS_INV_MOVE_ITEM){
					if(curMatrix -> check_fit(x,y,curItem)){
						curMatrix -> put_item_shadow(x,y,curItem);
					}
					else {
						p = curMatrix -> get_area_item(x,y,curItem);
						if(p){
							curMatrix -> remove_item(p,1);
							if(curMatrix -> check_fit(x,y,curItem))
								curMatrix -> put_item_shadow(x,y,curItem);
							curMatrix -> put_item(p -> MatrixX,p -> MatrixY,p,1);
						}
					}
				}
				else {
					if(iP){
						p = curMatrix -> get_item(x,y);
						if(p){
							if(iP -> items -> Size){
								iP -> free_list();
								iP -> set_redraw();
							}
							iP -> add_item(p->ID_ptr.c_str(),-1,aciCurColorScheme[FM_SELECT_COL]);
							iP -> add_items(p -> numComments,p -> comments);
							if(p -> pTemplate)
								iP -> add_item(aciGetItemLoad(p,0));
							if(p -> promptData && XGR_MouseObj.promptData){
								if(!pr){
									pr = new XGR_MousePromptData;
									pr -> ID = ACI_ITEM_PROMPT;
									XGR_MouseObj.promptData -> AddElement((XListElement*)pr);
								}
								pr -> set_text(p -> promptData);
								curMatrix -> get_item_coords(p,pr -> StartX,pr -> StartY,pr -> SizeX,pr -> SizeY);
							}
						}
						else {
							if(!(flags & AS_INV_MOVE_ITEM)){
								if(iP -> items -> Size){
									iP -> free_list();
									iP -> set_redraw();
								}
							}
						}
					}
				}
			}
		}
		else {
			if(!(flags & AS_INV_MOVE_ITEM)){
				if(iP){
					if(iP -> items -> Size){
						iP -> free_list();
						iP -> set_redraw();
					}
				}
			}
		}
	}
	if(flags & aMS_MOVED){
		if(curMode == AS_INV_MODE && curMatrix && curMatrix -> check_redraw()){
			curMatrix -> flags |= IM_REDRAW_SHADOW;
		}
	}
}

void actIntDispatcher::inv_mouse_imove_quant(void)
{
	int mx,my,x,y;
	invItem* p = NULL;
	invItem* p1 = NULL;
	iScreenObject* obj;

	int cr = aciGetCurCredits();

	XGR_MousePromptData* pr = NULL,*pr0 = NULL,*pr1,*avi_pr1,*avi_pr2;
	if(XGR_MouseObj.promptData){
		pr = XGR_MouseObj.promptData -> getData(ACI_NO_CASH_PROMPT);
		pr0 = XGR_MouseObj.promptData -> getData(ACI_NO_CASH_PROMPT2);
		pr1 = XGR_MouseObj.promptData -> getData(ACI_BT15_PROMPT);

		avi_pr1 = XGR_MouseObj.promptData -> getData(ACI_AVI_PROMPT1);
		if(avi_pr1){
			avi_pr2 = XGR_MouseObj.promptData -> getData(ACI_AVI_PROMPT2);
			if(!avi_pr2){
				avi_pr2 = new XGR_MousePromptData;
				avi_pr2 -> ID = ACI_AVI_PROMPT2;
				XGR_MouseObj.promptData -> AddElement((XListElement*)avi_pr2);
				avi_pr2 -> set_text(aciSTR_PutThis);
			}
			obj = (iScreenObject*)curLocData -> objList[ACI_AVI_OBJ_ID];
			if(flags & AS_INV_MOVE_ITEM){
				avi_pr2 -> StartX = obj -> PosX;
				avi_pr2 -> StartY = obj -> PosY;

				avi_pr2 -> SizeX = obj -> SizeX;
				avi_pr2 -> SizeY = obj -> SizeY;

				avi_pr1 -> StartX = avi_pr1 -> StartY = 0;
				avi_pr1 -> SizeX = avi_pr1 -> SizeY = 0;
			}
			else {
				avi_pr1 -> StartX = obj -> PosX;
				avi_pr1 -> StartY = obj -> PosY;

				avi_pr1 -> SizeX = obj -> SizeX;
				avi_pr1 -> SizeY = obj -> SizeY;

				avi_pr2 -> StartX = avi_pr2 -> StartY = 0;
				avi_pr2 -> SizeX = avi_pr2 -> SizeY = 0;
			}
		}
	}
	if(pr) pr -> StartX = pr -> StartY = pr -> SizeX = pr -> SizeY = 0;

	if(!pr0){
		pr0 = new XGR_MousePromptData;
		pr0 -> ID = ACI_NO_CASH_PROMPT2;
		XGR_MouseObj.promptData -> AddElement((XListElement*)pr0);
		pr0 -> set_text(aciSTR_NO_CASH);
	}

	if(secondMatrix){
		obj = (iScreenObject*)iGetObject(iScrDisp->curScr->ID_ptr.c_str(),"Button15");
		if(((uvsActInt*)secondMatrix -> uvsDataPtr) -> price > cr + aciGetCurMatrixPrice()){
			if(pr0){
				pr0 -> StartX = obj -> PosX;
				pr0 -> StartY = obj -> PosY;

				pr0 -> SizeX = obj -> SizeX;
				pr0 -> SizeY = obj -> SizeY;
			}
			if(pr1){
				pr1 -> StartX = 0;
				pr1 -> StartY = 0;

				pr1 -> SizeX = 0;
				pr1 -> SizeY = 0;
			}
		}
		else {
			if(pr1){
				pr1 -> StartX = obj -> PosX;
				pr1 -> StartY = obj -> PosY;

				pr1 -> SizeX = obj -> SizeX;
				pr1 -> SizeY = obj -> SizeY;
			}
			if(pr0){
				pr0 -> StartX = 0;
				pr0 -> StartY = 0;

				pr0 -> SizeX = 0;
				pr0 -> SizeY = 0;
			}
		}
	}

	if(flags & aMS_MOVED){
		if(curMatrix) curMatrix -> clear_shadow_cells();
		if(secondMatrix) secondMatrix -> clear_shadow_cells();
	}

	mx = iMouseX;
	my = iMouseY;

	if(flags & AS_ISCREEN)
		mx += iScreenOffs - 800;

	if(flags & AS_INV_MOVE_ITEM){
		if(curItem -> flags & INV_ITEM_NO_PAY && ((uvsActInt*)curItem -> uvsDataPtr) -> price > cr){
			if(!pr){
				pr = new XGR_MousePromptData;
				pr -> ID = ACI_NO_CASH_PROMPT;
				XGR_MouseObj.promptData -> AddElement((XListElement*)pr);
				pr -> set_text(aciSTR_NO_CASH);
			}
			pr -> StartX = curMatrix -> ScreenX + iScreenOffs;
			pr -> StartY = curMatrix -> ScreenY;

			pr -> SizeX = curMatrix -> ScreenSizeX;
			pr -> SizeY = curMatrix -> ScreenSizeY;
		}
	}

	if(curMatrix && curMatrix -> check_xy(mx,my)){
		if(flags & AS_INV_MOVE_ITEM){
			x = iMouseX - curMatrix -> ScreenX - (curItem -> ShapeSizeX >> 1) + curItem -> ShapeCenterX;
			y = iMouseY - curMatrix -> ScreenY - (curItem -> ShapeSizeY >> 1) + curItem -> ShapeCenterY;

			x += iScreenOffs - 800;

			y /= aCellSize - (aCellSize >> 2);

			if(y & 0x01) x -= (aCellSize >> 1);
			x /= aCellSize;
		}
		else {
			x = iMouseX - curMatrix -> ScreenX;
			y = iMouseY - curMatrix -> ScreenY;

			x += iScreenOffs - 800;

			y /= aCellSize - (aCellSize >> 2);

			if(y & 0x01) x -= (aCellSize >> 1);
			x /= aCellSize;
		}

		if(x >= 0 && x < curMatrix -> SizeX && y >= 0 && y < curMatrix -> SizeY){
			if(flags & AS_INV_MOVE_ITEM){
				if(curMatrix -> check_fit(x,y,curItem)){
					curMatrix -> put_item_shadow(x,y,curItem);
				}
				else {
					p = curMatrix -> get_area_item(x,y,curItem);
					if(p){
						curMatrix -> remove_item(p,1);
						if(curMatrix -> check_fit(x,y,curItem))
							curMatrix -> put_item_shadow(x,y,curItem);
						curMatrix -> put_item(p -> MatrixX,p -> MatrixY,p,1);
					}
				}
			}
			else {
				if(iP){
					p = curMatrix -> get_item(x,y);
					if(iP -> items -> Size){
						iP -> free_list();
						iP -> set_redraw();
					}
					if(p){
						p1 = get_iitem(p -> ID);
						iP -> add_item(p1->ID_ptr.c_str(),-1,aciCurColorScheme[FM_SELECT_COL]);
						iP -> add_items(p1 -> numComments,p1 -> comments);
						if(p1 -> pTemplate)
							iP -> add_item(aciGetItemLoad(p,1));
						if(!uvsCurrentWorldUnable)
							iP -> add_item(aciGetPrice((iListElement*)p,ITEMS_MODE,1));
					}
				}
			}
		}
	}
	else {
		if(secondMatrix && secondMatrix -> check_xy(mx,my)){
			if(flags & AS_INV_MOVE_ITEM){
				x = iMouseX - secondMatrix -> ScreenX - (curItem -> ShapeSizeX >> 1) + curItem -> ShapeCenterX;
				y = iMouseY - secondMatrix -> ScreenY - (curItem -> ShapeSizeY >> 1) + curItem -> ShapeCenterY;

				x += iScreenOffs - 800;

				y /= aCellSize - (aCellSize >> 2);

				if(y & 0x01) x -= (aCellSize >> 1);
				x /= aCellSize;
			}
			else {
				x = iMouseX - secondMatrix -> ScreenX;
				y = iMouseY - secondMatrix -> ScreenY;

				x += iScreenOffs - 800;

				y /= aCellSize - (aCellSize >> 2);

				if(y & 0x01) x -= (aCellSize >> 1);
				x /= aCellSize;
			}

			if(x >= 0 && x < secondMatrix -> SizeX && y >= 0 && y < secondMatrix -> SizeY){
				if(flags & AS_INV_MOVE_ITEM){
					if(secondMatrix -> check_fit(x,y,curItem)){
						secondMatrix -> put_item_shadow(x,y,curItem);
					}
					else {
						p = secondMatrix -> get_area_item(x,y,curItem);
						if(p){
							secondMatrix -> remove_item(p,1);
							if(secondMatrix -> check_fit(x,y,curItem))
								secondMatrix -> put_item_shadow(x,y,curItem);
							secondMatrix -> put_item(p -> MatrixX,p -> MatrixY,p,1);
						}
					}
				}
				else {
					if(iP){
						p = secondMatrix -> get_item(x,y);
						if(iP -> items -> Size){
							iP -> free_list();
							iP -> set_redraw();
						}
						if(p){
							iP -> add_item(p->ID_ptr.c_str(),-1,aciCurColorScheme[FM_SELECT_COL]);
							iP -> add_items(p -> numComments,p -> comments);
							if(p -> pTemplate)
								iP -> add_item(aciGetItemLoad(p,1));
							if(!uvsCurrentWorldUnable)
								iP -> add_item(aciGetPrice((iListElement*)p,ITEMS_MODE,1));
						}
					}
				}
			}
		}
		else {
			if(!(flags & AS_INV_MOVE_ITEM)){
				if(iP){
					if(iP -> items -> Size){
						iP -> free_list();
						iP -> set_redraw();
					}
				}
			}
		}
	}

	if(flags & aMS_MOVED){
		if(curMatrix && curMatrix -> check_redraw()){
			curMatrix -> flags |= IM_REDRAW_SHADOW;
		}
		if(secondMatrix && secondMatrix -> check_redraw()){
			secondMatrix -> flags |= IM_REDRAW_SHADOW;
		}
	}
	if(iP){
		if(!iP -> items -> Size){
			aciInitMechosInfo(curMatrix,iP);
		}
	}
}

int invMatrix::check_redraw(void)
{
	int i,j,index = 0;
	for(i = 0; i < SizeY; i ++){
		for(j = 0; j < SizeX; j ++){
			if(matrix[index] -> flags & (AS_IN_SHADOW | AS_REDRAW_CELL))
				return 1;
			index ++;
		}
	}
	return 0;
}

void CounterPanel::init(void)
{
	if(ibs_name){
		if(!ibs) ibs = new ibsObject;
		ibs -> load(ibs_name);
		ibs -> PosX = this->PosX;
		ibs -> PosY = this->PosY;
	}
	SizeX = MaxLen * (aCellSize + 1);
	SizeY = aCellSize + 1;

	switch(ID){
		case CREDITS_COUNTER:
			value_ptr = &aciCurCredits00;
			break;
		case TIME_COUNTER:
			value_ptr = &aciCurTime;
			break;
	}
}

void CounterPanel::finit(void)
{
	if(ibs_name){
		ibs -> free();
	}
}

void actIntDispatcher::trade_items(invMatrix* p,int imode)
{
	int cnt = 1,x,y,fl;
	invItem* it,*it1,*itm;

	it = (invItem*)curMatrix -> items -> last;
	while(it){
		it1 = (invItem*)it -> prev;

		cnt = it -> dropCount;
		x = it -> MatrixX;
		y = it -> MatrixY;
		fl = it -> flags;

		curMatrix -> remove_item(it,1);

		if(imode)
			itm = get_iitem(it -> ID);
		else
			itm = get_item(it -> ID);

		if(!itm)
			ErrH.Abort("Bad item ptr...");

		itm -> clone(it);
		if(!p -> put_item(x,y,it,1))
			ErrH.Abort("Bad trade item...");

		it -> dropCount = cnt;
		it -> flags = fl;
		it = it1;
	}
}

void actIntDispatcher::reinit_items(void)
{
	int id;
	invItem* p = (invItem*)curMatrix -> items -> last;
	actintItemData* d;

	while(p){
		if(p -> slotType != -1){
			id = curMatrix -> get_item_slot(p);
			d = p -> item_ptr;
			if(id != -1)
				aciSendEvent2itmdsp(ACI_ACTIVATE_ITEM,d,id);
		}
		p = (invItem*)p -> prev;
	}
}

void actIntDispatcher::deactivate_items(void)
{
	int id;
	if(!curMatrix) return;
	invItem* p = (invItem*)curMatrix -> items -> last;
	actintItemData* d;

	while(p){
		if(p -> slotType != -1){
			id = curMatrix -> get_item_slot(p);
			d = p -> item_ptr;
			if(id != -1)
				aciSendEvent2itmdsp(ACI_DEACTIVATE_ITEM,d,id);
		}
		p = (invItem*)p -> prev;
	}
}

int invMatrix::check_xy(int x,int y)
{
	if(x >= ScreenX && x < ScreenX + ScreenSizeX && y >= ScreenY && y < ScreenY + ScreenSizeY)
		return 1;
	return 0;
}

void CounterPanel::move(int delta)
{
	PosX -= delta;
	if(ibs){
		ibs -> CenterX -= delta;
		ibs -> PosX -= delta;
	}
}

void InfoPanel::move(int delta)
{
	PosX -= delta;
	if(ibs){
		ibs -> CenterX -= delta;
		ibs -> PosX -= delta;
	}
	if(bml) bml -> OffsX -= delta;
}

void invMatrix::clone(invMatrix* p)
{
	int i;

	p -> pData = pData;
	p -> internalID = internalID;

	p -> type = type;
	p -> flags = flags;

	p -> SizeX = SizeX;
	p -> SizeY = SizeY;

	p -> ScreenX = ScreenX;
	p -> ScreenY = ScreenY;

	p -> ScreenSizeX = ScreenSizeX;
	p -> ScreenSizeY = ScreenSizeY;

	p -> back = back;
	p -> mech_name = mech_name;

	p -> maxLoad = maxLoad;

	for(i = 0; i < SizeX * SizeY; i ++){
		p -> matrix[i] -> type = matrix[i] -> type;
		p -> matrix[i] -> flags = 0;

		p -> matrix[i] -> slotType = matrix[i] -> slotType;
		p -> matrix[i] -> slotNumber = matrix[i] -> slotNumber;
		p -> matrix[i] -> item = NULL;
	}

	p -> uvsDataPtr = uvsDataPtr;
	p -> numAviIDs = numAviIDs;
	p -> avi_ids = avi_ids;

	p -> flags |= IM_CLONE;
}

void invMatrix::backup(invMatrix* p)
{
	invItem* itm,*t;
	clone(p);

	itm = (invItem*)items -> last;
	while(itm){
		t = aScrDisp -> alloc_item();
		itm -> clone(t);
		t -> dropCount = itm -> dropCount;
		t -> uvsDataPtr = itm -> uvsDataPtr;
		p -> put_item(itm -> MatrixX,itm -> MatrixY,t,1);
		itm = (invItem*)itm -> prev;
	}
}

invItem* actIntDispatcher::alloc_item(void)
{
	invItem* p = new invItem;
/*
	if(!freeItemList -> Size){
		p = new invItem;
	}
	else {
		p = (invItem*)freeItemList -> last;
		freeItemList -> dconnect((iListElement*)p);
	}
*/
	return p;
}

invMatrix* actIntDispatcher::alloc_matrix(int type,int imode)
{
	invMatrix* t,*p;
	t = (imode) ? get_imatrix(type) : get_matrix(type);
	if(!t) return NULL;
/*
	p = (invMatrix*)freeMatrixList -> last;
	while(p){
		if(p -> SizeX == t -> SizeX && p -> SizeY == t -> SizeY){
			t -> clone(p);
			freeMatrixList -> dconnect(p);
			return p;
		}
		p = (invMatrix*)p -> prev;
	}
*/
	p = new invMatrix(t -> SizeX,t -> SizeY);
	t -> clone(p);
	return p;
}

void actIntDispatcher::free_item(invItem* p)
{
	delete p;
//	  freeItemList -> connect((iListElement*)p);
}

void actIntDispatcher::free_matrix(invMatrix* p)
{
	invItem* itm,*itm1;
	itm = (invItem*)p -> items -> last;
	while(itm){
		itm1 = (invItem*)itm -> prev;
		p -> items -> dconnect((iListElement*)itm);
		free_item(itm);
		itm = itm1;
	}
	delete p;
//	  freeMatrixList -> connect((iListElement*)p);
}

invItem* actIntDispatcher::get_item_ptr(int id)
{
	if(!curMatrix) return NULL;
	invItem* p = (invItem*)curMatrix -> items -> last;
	while(p){
		if(p -> ID == id && !p -> item_ptr)
			return p;
		p = (invItem*)p -> prev;
	}
	return NULL;
}

invItem* actIntDispatcher::get_item_ptr_xy(int id,int x,int y)
{
	if(!curMatrix) return NULL;
	invItem* p = (invItem*)curMatrix -> items -> last;
	
	while(p){
		if(p -> ID == id && !p -> item_ptr && p -> MatrixX == x && p -> MatrixY == y)
			return p;
		p = (invItem*)p -> prev;
	}
	return NULL;
}

void actIntDispatcher::calc_load(void)
{
	int i,sz;
	if(!curMatrix) return;
	sz = curMatrix -> SizeX * curMatrix -> SizeY;
	aciCurLoad = 0;
	for(i = 0; i < sz; i ++){
		if(curMatrix -> matrix[i] -> flags & AS_BUSY_CELL) aciCurLoad ++;
	}
	aciMaxLoad = curMatrix -> maxLoad;
}

InfoPanel* actIntDispatcher::get_info_panel(int id)
{
	InfoPanel* p = (InfoPanel*)i_infoPanels -> last;
	while(p){
		if(p -> type == id)
			return p;
		p = (InfoPanel*)p -> prev;
	}
	return NULL;
}

InfoPanel* actIntDispatcher::get_aci_info_panel(int id)
{
	InfoPanel* p = (InfoPanel*)infoPanels -> last;
	while(p){
		if(p -> type == id)
			return p;
		p = (InfoPanel*)p -> prev;
	}
	return NULL;
}

void aciWorldMap::calc_shapes(void)
{
	int i;
	XStream fh;
	if(!ShapeSizeX) ShapeSizeX = new short[3];
	if(!ShapeSizeY) ShapeSizeY = new short[3];
	if(!shapes){
		shapes = new unsigned char*[3];
		for(i = 0; i < 3; i ++){
			if(!shape_files[i]) ErrH.Abort("Map shape file not found...");

			fh.open(shape_files[i]);
			fh > ShapeSizeX[i] > ShapeSizeY[i];
			shapes[i] = new unsigned char[ShapeSizeX[i] * ShapeSizeY[i]];
			fh.read(shapes[i],ShapeSizeX[i] * ShapeSizeY[i]);
			fh.close();
		}
	}
}

void aciWorldMap::show_link(int w1,int w2)
{
	int col;
	if(!world_ptr[w1] || !world_ptr[w2]) return;

	if(world_ptr[w1] -> flags & AS_ACCESSIBLE_WORLD && world_ptr[w2] -> flags & AS_CURRENT_WORLD){
		col = aciCurColorScheme[WORLD_ACC_LINK_COL];
	}
	else {
		if(world_ptr[w1] -> flags & AS_CURRENT_WORLD && world_ptr[w2] -> flags & AS_ACCESSIBLE_WORLD){
			col = aciCurColorScheme[WORLD_ACC_LINK_COL];
		}
		else {
			col = aciCurColorScheme[WORLD_LINK_COL];
		}
	}
	a_buf_line2(world_ptr[w1] -> PosX,world_ptr[w1] -> PosY,world_ptr[w2] -> PosX,world_ptr[w2] -> PosY,col,SizeX,SizeY,redraw_buf);
}

#define WORLD_COORD_MAX       1000
void aciWorldMap::remap_coords(void)
{
	int x,y;
	aciWorldInfo* p = (aciWorldInfo*)world_list -> last;
	if(flags & WMAP_REMAP_COORDS)
		return;
	while(p){
		x = (p -> PosX * SizeX) / WORLD_COORD_MAX;
		y = (p -> PosY * SizeY) / WORLD_COORD_MAX;

		p -> PosX = x;
		p -> PosY = y;

		p = (aciWorldInfo*)p -> prev;
	}
	JumpCountSizeX = (JUMP_SIZE_X * SizeX) / WORLD_COORD_MAX;
	JumpCountSizeY = (JUMP_SIZE_Y * SizeY) / WORLD_COORD_MAX;

	JumpCountX = (SizeX - JumpCountSizeX) / 2;
	JumpCountY = (JUMP_Y * SizeY) / WORLD_COORD_MAX;

	flags |= WMAP_REMAP_COORDS;
}

//TODO need fast malloc
void aciWorldInfo::redraw(int bsx,int bsy,unsigned char* buf)
{
	int x,y,sx,sy,ox,oy;
	int border_col = 0,floor_col = 0;

	unsigned char* shape,*shape1;
	aciWorldMap* p = (aciWorldMap*)owner;

	if(flags & AS_SECRET_WORLD && !(flags & AS_VISIBLE_WORLD))
		return;

	floor_col = aciCurColorScheme[ACI_BACK_COL];

	if(flags & AS_VISIBLE_WORLD){
		border_col = aciCurColorScheme[WORLD_BORDER_VIS_COL];
		floor_col = aciCurColorScheme[WORLD_VISIBLE_COL];
	}
	if(flags & AS_SPECIAL_WORLD){
		border_col = aciCurColorScheme[WORLD_BORDER_SPC_COL];
		floor_col = aciCurColorScheme[WORLD_SPECIAL_COL];
	}
	if(flags & AS_EXPLORED_WORLD){
		border_col = aciCurColorScheme[WORLD_BORDER_EXP_COL];
		floor_col = aciCurColorScheme[WORLD_EXPLORED_COL];
	}
	if(flags & AS_CURRENT_WORLD){
		border_col = aciCurColorScheme[WORLD_BORDER_CUR_COL];
		floor_col = aciCurColorScheme[WORLD_CURRENT_COL];
	}
//	  if(flags & AS_ACCESSIBLE_WORLD){
//		  border_col = aciCurColorScheme[WORLD_BORDER_ACC_COL];
//		  floor_col = aciCurColorScheme[WORLD_ACCESSIBLE_COL];
//	  }

	sx = p -> ShapeSizeX[shape_id];
	sy = p -> ShapeSizeY[shape_id];
	shape = p -> shapes[shape_id];

	ox = p -> ShapeOffsX[shape_id];
	oy = p -> ShapeOffsY[shape_id];

	shape1 = new unsigned char[sx * sy];
	memcpy(shape1,shape,sx * sy);

	x = PosX - sx/2;
	y = PosY - sy/2;

	swap_buf_col(2,border_col,sx,sy,shape1);
	swap_buf_col(1,floor_col,sx,sy,shape1);
	mem_putspr(x,y,sx,sy,bsx,bsy,shape1,buf);

	x = PosX - (aScrFonts[font] -> SizeX >> 1) + ox;
	y = PosY - (aScrFonts[font] -> SizeY >> 1) + oy;

	aPutChar(x,y,font,border_col,letter,bsx,bsy,buf);
	delete[] shape1;
}

fncMenuItem* actIntDispatcher::alloc_menu_item(void)
{
	fncMenuItem* p = new fncMenuItem;
/*
	if(!freeMenuItemList -> Size){
		p = new fncMenuItem;
	}
	else {
		p = (fncMenuItem*)freeMenuItemList -> last;
		freeMenuItemList -> dconnect((iListElement*)p);
	}
*/
	return p;
}

void actIntDispatcher::free_menu_item(fncMenuItem* p)
{
	delete p;
//	  freeMenuItemList -> connect((iListElement*)p);
}

CounterPanel* actIntDispatcher::get_counter(int id)
{
	CounterPanel* p;
	p = (CounterPanel*)intCounters -> last;
	while(p){
		if(p -> ID == id)
			return p;
		p = (CounterPanel*)p -> prev;
	}
	p = (CounterPanel*)invCounters -> last;
	while(p){
		if(p -> ID == id)
			return p;
		p = (CounterPanel*)p -> prev;
	}
	p = (CounterPanel*)infCounters -> last;
	while(p){
		if(p -> ID == id)
			return p;
		p = (CounterPanel*)p -> prev;
	}
	return NULL;
}

CounterPanel* actIntDispatcher::get_icounter(int id)
{
	CounterPanel* p;
	p = (CounterPanel*)i_Counters -> last;
	while(p){
		if(p -> ID == id)
			return p;
		p = (CounterPanel*)p -> prev;
	}
	return NULL;
}

void actIntDispatcher::swap_matrices(void)
{
	int v;
	if(curMatrix){
		aciSell_Matrix(curMatrix);
		if(((uvsActInt*)secondMatrix -> uvsDataPtr) -> price <= 1){
			v = aciGetCurCredits();
			v -= ((uvsActInt*)curMatrix -> uvsDataPtr) -> sell_price;
			aciUpdateCurCredits(v);
		}
		secondMatrix -> ScreenX = curMatrix -> ScreenX;
		free_matrix(curMatrix);
		free_matrix(backupMatrix);
	}
	else {
		secondMatrix -> ScreenX -= 400;
	}
	curMatrix = secondMatrix;
	aciInitCurMatrixPtr();
	secondMatrix = NULL;
}

void actIntDispatcher::cancel_matrix(void)
{
	invItem* p;
	uvsActInt* u;
	if(flags & AS_INV_MOVE_ITEM){
		flags ^= AS_INV_MOVE_ITEM;
		free_item(curItem);
		restore_mouse_cursor();
		curItem = NULL;
	}
	free_matrix(secondMatrix);
	if(curMatrix){
		backupMatrix -> ScreenX = curMatrix -> ScreenX;
		free_matrix(curMatrix);
		curMatrix = alloc_matrix(backupMatrix -> type,1);
		backupMatrix -> backup(curMatrix);
		p = (invItem*)curMatrix -> items -> last;
		while(p){
			if(p -> uvsDataPtr){
				u = (uvsActInt*)p -> uvsDataPtr;

				u -> pos_x = p -> MatrixX;
				u -> pos_y = p -> MatrixY;
			}
			p = (invItem*)p -> prev;
		}
	}

	secondMatrix = NULL;
}

void actIntDispatcher::clear_menu(fncMenu* mn)
{
	fncMenuItem* p,*p1;

	p = (fncMenuItem*)mn -> items -> last;
	while(p){
		p1 = (fncMenuItem*)p -> prev;
		mn -> items -> dconnect((iListElement*)p);
		free_menu_item(p);
		p = p1;
	}
	p = (fncMenuItem*)mn -> inactive_items -> last;
	while(p){
		p1 = (fncMenuItem*)p -> prev;
		mn -> inactive_items -> dconnect((iListElement*)p);
		free_menu_item(p);
		p = p1;
	}
	mn -> prefix = NULL;
	mn -> postfix = NULL;
}

void fncMenu::set_prefix(unsigned char* p)
{
	int psx = 0;
	prefix = p;

	if(postfix) psx = aStrLen(postfix,curItem -> font,curItem -> space) + FM_PREFIX_DELTA;

	PrefixSX = aStrLen(p,curItem -> font,curItem -> space);
	PrefixSY = aScrFonts[curItem -> font] -> SizeY;

	PrefixX = (SizeX - (PrefixSX + curItem -> SizeX + FM_PREFIX_DELTA + psx))/2;
	PrefixDelta = PrefixX + PrefixSX + FM_PREFIX_DELTA - curItem -> PosX;

	PrefixY = (SizeY - PrefixSY)/2;
}

void fncMenu::set_postfix(unsigned char* p)
{
	postfix = p;
}

int actIntDispatcher::get_locdata_id(const char* name)
{
	aciLocationInfo* p = (aciLocationInfo*)locationList -> last;
	while(p){
		if(!strcmp(p -> nameID,name))
			return p -> ID;
		p = (aciLocationInfo*)p -> prev;
	}
	ErrH.Abort("Bad Location name...");
	return -1;
}

/*TODO*/
void actIntDispatcher::put_in_slot(actintItemData* d)
{
	int x,y,index = 0,px,py,id;
	int ms_flag = 0;
	invItem* dvc;
	invItem* p = (invItem*)d -> actintOwner;

	//std::cout<<"actIntDispatcher::put_in_slot "<<p->fname<<std::endl;
	
	if(flags & AS_INV_MOVE_ITEM && curItem == p){
		ms_flag = 1;
	}
	else {
		id = curMatrix -> get_item_slot(p);
		if(id == AS_DEVICE_SLOT) return;
	}

	for(y = 0; y < curMatrix -> SizeY; y ++){
		for(x = 0; x < curMatrix -> SizeX; x ++){
			if(curMatrix -> matrix[index] -> slotType == AS_DEVICE_SLOT){
				if(curMatrix -> check_fit(x,y,p)){
					if(!ms_flag){
						curMatrix -> remove_item(p);
					}
					else {
						flags ^= AS_INV_MOVE_ITEM;
						restore_mouse_cursor();
						curMatrix -> clear_shadow_cells();
						curItem = NULL;
						if(iP){
							iP -> free_list();
							iP -> set_redraw();
						}
					}
					curMatrix -> put_item(x,y,p);
					SOUND_DROP();
					id = curMatrix -> get_item_slot(p);
					aciSendEvent2itmdsp(ACI_ACTIVATE_ITEM,d,id);
					if(curMode == AS_INV_MODE)
						curMatrix -> set_redraw();
					return;
				}
				else {
					dvc = curMatrix -> get_area_item(x,y,p);
					if(dvc){
						curMatrix -> remove_item(dvc);
						if(!curMatrix -> check_fit(x,y,p)){
							curMatrix -> put_item(dvc -> MatrixX,dvc -> MatrixY,dvc);
							return;
						}
						if(!curMatrix -> check_fit(x,y,p)) ErrH.Abort("put_in_slot() failed...");
						if(!ms_flag){
							px = p -> MatrixX;
							py = p -> MatrixY;
							curMatrix -> remove_item(p);
							curMatrix -> put_item(dvc -> MatrixX,dvc -> MatrixY,p);
							curMatrix -> put_item(px,py,dvc);
							SOUND_DROP();
						}
						else {
							curMatrix -> put_item(dvc -> MatrixX,dvc -> MatrixY,p);
							SOUND_DROP();
							curItem = dvc;
							if(iP){
								iP -> free_list();
								iP -> add_item(curItem->ID_ptr.c_str(),-1,aciCurColorScheme[FM_SELECT_COL]);
								iP -> add_items(curItem -> numComments,curItem -> comments);
								if(curItem -> pTemplate)
									iP -> add_item(aciGetItemLoad(curItem,0));
								iP -> set_redraw();
							}
							set_mouse_cursor(dvc -> frame,dvc -> ScreenSizeX,dvc -> ScreenSizeY);
						}

						aciSendEvent2itmdsp(ACI_DEACTIVATE_ITEM,dvc -> item_ptr);

						id = curMatrix -> get_item_slot(p);
						aciSendEvent2itmdsp(ACI_ACTIVATE_ITEM,d,id);

						if(curMode == AS_INV_MODE)
							curMatrix -> set_redraw();
						return;
					}
				}
			}
			index ++;
		}
	}
}

void fncMenu::add_obj(fncMenuItem* p)
{
	items -> connect((iListElement*)p);
}

void fncMenu::delete_obj(fncMenuItem* p)
{
	items -> dconnect((iListElement*)p);
}

fncMenuItem* fncMenu::get_obj(const char* ptr)
{
	fncMenuItem* p = (fncMenuItem*)items -> last;
	while(p){
		if(!strcmp(p -> name,ptr)) return p;
		p = (fncMenuItem*)p -> prev;
	}
	return NULL;
}

void fncMenu::go2upmenu(int mode)
{
	if(flags & FM_SUBMENU){
		upMenu -> flags &= ~(FM_HIDDEN | FM_ACTIVE);
		upMenu -> flags |= FM_LOCK;
		upMenu -> curFunction = FMENU_OFF;
		upMenu -> curItem = upMenu -> get_fnc_item(FMENU_OFF);

		if(mode == AS_INFO_MODE || mode == -1)
			upMenu -> set_redraw();
		flags &= ~(FM_ACTIVE | FM_REDRAW);
	}
}

aciBitmapMenu* actIntDispatcher::get_bmenu(int mn)
{
	aciBitmapMenu* p = (aciBitmapMenu*)b_menuList -> last;
	while(p){
		if(p -> ID == mn) return p;
		p = (aciBitmapMenu*)p -> prev;
	}
	return NULL;
}

void actIntDispatcher::add_bmenu(aciBitmapMenu* p)
{
	b_menuList -> connect((iListElement*)p);
}

void aciBitmapMenu::go2upmenu(int mode)
{
	fncMenu* p = (fncMenu*)upMenu;
	if(!p) return;
	p -> flags &= ~(FM_HIDDEN | FM_ACTIVE);
	p -> flags |= FM_LOCK;
	p -> curFunction = FMENU_OFF;
	p -> curItem = p -> get_fnc_item(FMENU_OFF);
	if(mode == AS_INFO_MODE || mode == -1)
		p -> set_redraw();
}

void aciBitmapMenu::change(int x,int y)
{
	aciBitmapMenuItem* p = (aciBitmapMenuItem*)items -> last;

	while(p){
		if(p -> check_xy(x,y)){
			p -> change();
			aciHandleCameraEvent(p -> ID,p -> curState);
			curCount = activeCount;
			flags |= BM_REBUILD;
			return;
		}
		p = (aciBitmapMenuItem*)p -> prev;
	}
	go2upmenu();
}

void aciBitmapMenuItem::change(void)
{
	curState ++;
	if(curState >= numStates) curState = 0;
}

void aciBitmapMenu::remap_coords(void)
{
	int x,y;
	aciBitmapMenuItem* p;
	if(flags & BM_REMAP_COORDS) return;
	p = (aciBitmapMenuItem*)items -> last;
	while(p){
		x = (p -> PosX * SizeX) / BM_GLOBAL_SIZE;
		y = (p -> PosY * SizeY) / BM_GLOBAL_SIZE;

		p -> PosX = x - (p -> SizeX)/2;
		p -> PosY = y - (p -> SizeY)/2;

		p = (aciBitmapMenuItem*)p -> prev;
	}

	flags |= BM_REMAP_COORDS;
}

int invMatrix::check_slot(int type)
{
	int i,j,index = 0;
	for(i = 0; i < SizeX; i ++){
		for(j = 0; j < SizeY; j ++){
			if(matrix[index] -> slotType == type) return 1;
			index ++;
		}
	}
	return 0;
}

const int aciSaveVersion = 1;

void actIntDispatcher::save_data(XStream* fh)
{
	invItem* p;
	aciWorldInfo* w;

	int cr = aciGetCurCredits();

	*fh < aciSaveVersion;

	if(curMatrix){
		*fh < curMatrix -> type < curMatrix -> items -> Size;
		p = (invItem*)curMatrix -> items -> last;
		while(p){
			*fh < p -> ID < p -> MatrixX < p -> MatrixY < p -> dropCount;
			p = (invItem*)p -> prev;
		}
	}
	else {
		*fh < (int)(-1);
	}
	*fh < cr;
	if(wMap){
		w = (aciWorldInfo*)wMap -> world_list -> last;
		while(w){
			*fh < w -> flags;
			w = (aciWorldInfo*)w -> prev;
		}
		w = (aciWorldInfo*)wMap -> world_list -> last;
		while(w){
			fh -> write(w -> links,AS_MAX_WORLD);
			w = (aciWorldInfo*)w -> prev;
		}
	}
}

void actIntDispatcher::load_data(XStream* fh)
{
	int i,n,x,y,d,itm_size,ver,cr;

	*fh > ver;

	if(ver < 0 || ver > aciSaveVersion) ErrH.Abort("Bad save version...");

	aciWorldInfo* w;
	invItem* p,*p1;
	invMatrix* m;
	if(curMatrix)
		free_matrix(curMatrix);

	*fh > n > itm_size;
	if(n != -1){
		curMatrix = alloc_matrix(n);
		if(flags & AS_ISCREEN)
			m = get_imatrix(n);
		else
			m = get_matrix(n);
		
		m -> clone(curMatrix);
		for(i = 0; i < itm_size; i ++){
			*fh > n > x > y > d;
			if(flags & AS_ISCREEN)
				p = get_iitem(n);
			else
				p = get_item(n);

			p1 = alloc_item();
			p -> clone(p1);
			curMatrix -> put_item(x,y,p1,1);
			p1 -> dropCount = d;
		}
	}
	else {
		curMatrix = NULL;
	}
	*fh > cr;
	aciUpdateCurCredits(cr);

	if(wMap){
		w = (aciWorldInfo*)wMap -> world_list -> last;
		while(w){
			*fh > w -> flags;
			w = (aciWorldInfo*)w -> prev;
		}
		w = (aciWorldInfo*)wMap -> world_list -> last;
		while(w){
			fh -> read(w -> links,AS_MAX_WORLD);
			w = (aciWorldInfo*)w -> prev;
		}
	}
}

void actIntDispatcher::aml_check_redraw(int x,int y)
{
	int x1,y1,dx,dy;
	InfoPanel* pl;

	y += 16;

	pl = (InfoPanel*)i_infoPanels -> last;
	while(pl){
		if(pl -> iScreenOwner && pl -> iScreenOwner -> flags & EL_DATA_LOADED){
			x1 = pl -> PosX + pl -> SizeX/2;
			y1 = pl -> PosY + pl -> SizeY/2;

			dx = pl -> SizeX/2 + aciML_ToolzerRadius * 2;
			dy = pl -> SizeY/2 + aciML_ToolzerRadius * 2;

			if(abs(x - x1) <= dx && abs(y - y1) <= dy) pl -> set_redraw();
		}
		pl = (InfoPanel*)pl -> prev;
	}
	if(!(flags & AS_ISCREEN_INV_MODE) && qMenu && qMenu -> flags & FMC_DATA_INIT){
			x1 = qMenu -> ScreenX + qMenu -> ScreenSizeX/2;
			y1 = qMenu -> ScreenY + qMenu -> ScreenSizeY/2;

			dx = qMenu -> ScreenSizeX/2 + aciML_ToolzerRadius;
			dy = qMenu -> ScreenSizeY/2 + aciML_ToolzerRadius;

			if(abs(x - x1) <= dx && abs(y - y1) <= dy) qMenu -> set_redraw();
	}
}

fncMenuSet::fncMenuSet(void)
{
	PosX = 0;
	items = new iList;
	flags = 0;
	SizeX = SizeY = 0;

	space = 5;
	font = 2;

	redraw_data = NULL;
	data = NULL;

	qMenu = NULL;
}

fncMenuSet::~fncMenuSet(void)
{
	iListElement* p,*p1;
	iScreenElement* el = (iScreenElement*)redraw_data;
	fncMenu* m;
	if (items) {
		p = items -> last;
		while(p) {
			p1 = p -> prev;
			m = (fncMenu*)p;
			delete m;
			p = p1;
		}
		delete items;
		items = NULL;
	}
	if(el) delete el;
}

void fncMenuSet::init(void)
{
	if(!redraw_data)
		redraw_data = new iBitmapElement;
	else
		redraw_data -> free();

	redraw_data -> SizeY = redraw_data -> bSizeY = data -> SizeY;
	redraw_data -> owner = data -> owner;

	flags |= FMC_DATA_INIT;
}

void fncMenuSet::finit(void)
{
	fncMenu* m,*m1;
	if(redraw_data)
		redraw_data -> free();

	m = (fncMenu*)items -> last;
	while(m){
		m1 = (fncMenu*)m -> prev;
		delete m;
		m = m1;
	}
	items -> init_list();

	flags &= ~FMC_DATA_INIT;
}

void fncMenuSet::init_redraw(void)
{
	int i,sz = 0;
	fncMenu* m;

	m = (fncMenu*)items -> first;
	for(i = 0; i < items -> Size; i ++){
		m -> PosX = sz;
		sz += m -> SizeX + space;
		m -> iScreenOwner = redraw_data;
		m = (fncMenu*)m -> next;
	}

	redraw_data -> SizeX = redraw_data -> bSizeX = sz + space;
	redraw_data -> SizeY = redraw_data -> bSizeY = data -> SizeY;
	redraw_data -> alloc_mem();
	redraw_data -> owner = data -> owner;

	PosX = (data -> SizeX - redraw_data -> SizeX) / 2;
}

void fncMenuSet::build(void)
{
	int i,x = 0,dx;
	fncMenu* m;

	m = (fncMenu*)items -> first;
	for(i = 0; i < items -> Size; i ++){
		m -> PosX = x - m -> curItem -> PosX;
		x += m -> curItem -> SizeX + space;
		m = (fncMenu*)m -> next;
	}
	dx = (redraw_data -> SizeX - x)/2;
	if(dx){
		m = (fncMenu*)items -> first;
		for(i = 0; i < items -> Size; i ++){
			m -> PosX += dx;
			m = (fncMenu*)m -> next;
		}
	}
}

void fncMenuSet::set_redraw(void)
{
	flags |= FMC_REDRAW;
}

void fncMenuSet::redraw_owner(void)
{
	iScreenObject* obj;

	if(!(data -> flags & EL_DATA_LOADED))
		return;

	obj = (iScreenObject*)data -> owner;

	obj -> redraw();
	obj -> flush();
}

void fncMenuSet::redraw(void)
{
	int i,y,sx,dx,rdx,index,r_index;

	fncMenu* m;
	iScreenObject* obj;

	if(!(data -> flags & EL_DATA_LOADED))
		return;

	obj = (iScreenObject*)data -> owner;
	obj -> flags |= (OBJ_MUST_REDRAW | OBJ_CLEAR_FON);

	if(!redraw_data || !(redraw_data -> flags & EL_DATA_LOADED)){
		memset(data -> fdata,0,data -> SizeX * data -> SizeY);
		return;
	}

//	  memset(redraw_data -> fdata,4,redraw_data -> SizeX * redraw_data -> SizeY);
	memset(redraw_data -> fdata,0,redraw_data -> SizeX * redraw_data -> SizeY);
	m = (fncMenu*)items -> first;
	for(i = 0; i < items -> Size; i ++){
		m -> redraw();
		m = (fncMenu*)m -> next;
	}
	if(data -> SizeX > redraw_data -> SizeX){
		sx = redraw_data -> SizeX;
		dx = (data -> SizeX - sx)/2;
		rdx = 0;
	}
	else {
		sx = data -> SizeX;
		dx = 0;
		rdx = -PosX;
//		  rdx = (redraw_data -> SizeX - sx)/2 - PosX;
	}

	index = dx;
	r_index = rdx;
	for(y = 0; y < data -> SizeY; y ++){
		memcpy(data -> fdata + index,redraw_data -> fdata + r_index,sx);
		index += data -> SizeX;
		r_index += redraw_data -> SizeX;
	}

	flags &= ~FMC_REDRAW;
}

void fncMenuSet::add(fncMenu* p,int mode)
{
	items -> connect((iListElement*)p);
	p -> SizeY = data -> SizeY;

	p -> flags |= (FM_ACTIVE | FM_NO_ALIGN | FM_NO_DEACTIVATE);

	p -> VItems = 1;
	p -> curFunction = 0;
	p -> init_curItem();
	p -> init_objects();

	p -> iScreenOwner = redraw_data;

	if(mode) qMenu = p;
}

void fncMenu::init_curItem(void)
{
	curItem = get_fnc_item(curFunction);
	if(!curItem) ErrH.Abort("init_curItem() failed...");
	if(VItems < items -> Size)
		firstItem = curItem;
	else
		firstItem = (fncMenuItem*)items -> first;
}

int fncMenuSet::check_xy(int x,int y)
{
	if(x >= ScreenX && x < ScreenX + ScreenSizeX && y >= ScreenY && y < ScreenY + ScreenSizeY)
		return 1;
	return 0;
}

void fncMenuSet::key_trap(int x,int y)
{
	int flag = 0,fnc;
	fncMenu* m;
	if(!check_xy(x,y)) return;

	x -= ScreenX + PosX;
	y -= ScreenY;

	m = (fncMenu*)items -> last;
	while(m){
		if(m -> items -> Size > 1){
			fnc = m -> curFunction;
			m -> change(x,y);
			if(fnc != m -> curFunction)
				flag = 1;
		}
		m = (fncMenu*)m -> prev;
	}
	if(flag){
		build();
		set_redraw();

		SOUND_SELECT();
	}
}

char* fncMenuSet::get(void)
{
	if(qMenu){
		return qMenu -> curItem -> name;
	}
	return NULL;
}

void fncMenuSet::shift(int dx)
{
	if(redraw_data -> SizeX <= data -> SizeX) return;
	PosX += dx;
	limitPosX();
}

void fncMenuSet::limitPosX(void)
{
	int max_x,min_x;
	if(redraw_data -> SizeX <= data -> SizeX) return;

	max_x = 0;
	min_x = - redraw_data -> SizeX + data -> SizeX;

	if(PosX > max_x) PosX = max_x;
	if(PosX < min_x) PosX = min_x;
}

void invItem::init_avi_id(char* p,int num)
{
	int sz = strlen(p) + 1;
	avi_ids[num] = new char[sz];
	strcpy(avi_ids[num],p);
}

void invMatrix::init_avi_id(char* p,int num)
{
	int sz = strlen(p) + 1;
	avi_ids[num] = new char[sz];
	strcpy(avi_ids[num],p);
}

int fncMenu::remove_menu(int code)
{
	fncMenuItem* p = (fncMenuItem*)items -> last;
	while(p){
		if(p -> fnc_code == code){
			if(code == curFunction){
				curItem = (fncMenuItem*)p -> next;
				curFunction = curItem -> fnc_code;
			}
			items -> dconnect((iListElement*)p);
			init_curItem();

			delete p;
			return 1;
		}
		p = (fncMenuItem*)p -> prev;
	}
	p = (fncMenuItem*)inactive_items -> last;
	while(p){
		if(p -> fnc_code == code){
			inactive_items -> dconnect((iListElement*)p);
			init_curItem();
			delete p;
			return 1;
		}
		p = (fncMenuItem*)p -> prev;
	}
	return 0;
}

void fncMenu::clean(void)
{
	fncMenuItem* p = (fncMenuItem*)items -> last;
	fncMenuItem* p1;
	while(p){
		p1 = (fncMenuItem*)p -> prev;
		if(p -> flags & FM_CLONE){
			if(p -> fnc_code == curFunction){
				curItem = (fncMenuItem*)p -> next;
				curFunction = curItem -> fnc_code;
			}
			items -> dconnect((iListElement*)p);
			init_curItem();

			delete p;
		}
		p = p1;
	}
	p = (fncMenuItem*)inactive_items -> last;
	while(p){
		p1 = (fncMenuItem*)p -> prev;
		if(p -> fnc_code == curFunction){
			inactive_items -> dconnect((iListElement*)p);
			delete p;
		}
		p = p1;
	}
}

int fncMenu::check_menu(int code)
{
	fncMenuItem* p = (fncMenuItem*)items -> last;
	while(p){
		if(p -> fnc_code == code){
			return 1;
		}
		p = (fncMenuItem*)p -> prev;
	}
	p = (fncMenuItem*)inactive_items -> last;
	while(p){
		if(p -> fnc_code == code){
			return 1;
		}
		p = (fncMenuItem*)p -> prev;
	}
	return 0;
}

void fncMenu::add_menu(fncMenu* p)
{
	fncMenuItem * itm;
	if(!p -> upMenuItem || check_menu(p -> upMenuItem -> fnc_code)) return;
	itm = new fncMenuItem;
	p -> upMenuItem -> clone(itm);
	items -> connect((iListElement*)itm);

	if(flags & FM_RANGE_FONT)
		itm -> flags |= FM_RANGE_FONT;

	init_objects();
	init_curItem();
}

void actIntDispatcher::clean_menus(void)
{
	fncMenu* m = (fncMenu*)menuList -> last;
	while(m){
		if(m -> flags & FM_MAIN_MENU){
			m -> clean();
		}
		m = (fncMenu*)m -> prev;
	}
}

void actIntDispatcher::prepare_menus(void)
{
	invItem* p;
	if(!curMatrix) return;
	p = (invItem*)curMatrix -> items -> last;
	while(p){
		if(p -> menu)
			add_menu_item((fncMenu*)p -> menu);
		p = (invItem*)p -> prev;
	}
}

void actIntDispatcher::add_menu_item(fncMenu* p)
{
	fncMenu* m = (fncMenu*)menuList -> last;
	while(m){
		if(m -> flags & FM_MAIN_MENU){
			m -> add_menu(p);
			if(curMode == AS_INFO_MODE && m -> flags & FM_ACTIVE) m -> set_redraw();
		}
		m = (fncMenu*)m -> prev;
	}
}

void actIntDispatcher::remove_menu_item(fncMenu* p)
{
	int code = 0;
	fncMenu* m = (fncMenu*)menuList -> last;

	if(!p -> upMenuItem) return;
	code = p -> upMenuItem -> fnc_code;
	while(m){
		if(m -> flags & FM_MAIN_MENU){
			m -> remove_menu(code);
			if(curMode == AS_INFO_MODE && m -> flags & FM_ACTIVE) m -> set_redraw();
		}
		m = (fncMenu*)m -> prev;
	}
	if(p -> flags & FM_ACTIVE) p -> go2upmenu(curMode);
}

void fncMenuItem::clone(fncMenuItem* p)
{
	p -> PosX = PosX;
	p -> PosY = PosY;

	p -> SizeX = SizeX;
	p -> SizeY = SizeY;

	p -> fnc_code = fnc_code;
	p -> submenuID = submenuID;

	p -> font = font;
	p -> space = space;
	p -> scankey = scankey;

	p -> eventPtr = eventPtr;

	p -> name = name;
	p -> name_len = name_len;

	p -> flags |= (flags | FM_CLONE) & (~FM_NO_DELETE);
}

aciLocationShutterInfo::~aciLocationShutterInfo(void)
{
	if(data) delete data;
	if(name) delete[] name;
}

void aciLocationShutterInfo::prepare(int m)
{
	mode = m;
	switch(mode){
		case ACI_SHUTTER_OPEN:
			curPos.X = Pos[0].X;
			curPos.Y = Pos[0].Y;
			trgPos.X = Pos[1].X;
			trgPos.Y = Pos[1].Y;
			break;
		case ACI_SHUTTER_CLOSE:
			curPos.X = Pos[2].X;
			curPos.Y = Pos[2].Y;
			trgPos.X = Pos[3].X;
			trgPos.Y = Pos[3].Y;
			break;
	}
	curDelta.X = (curPos.X < trgPos.X) ? Delta.X : -Delta.X;
	curDelta.Y = (curPos.Y < trgPos.Y) ? Delta.Y : -Delta.Y;
}

void aciLocationShutterInfo::init_size(void)
{
	short sx,sy;
	XStream fh(name,XS_IN);
	fh > sx > sy;
	fh.close();

	Size.X = sx;
	Size.Y = sy;
}

void aciLocationShutterInfo::loadData(void)
{
	short sx,sy;
	XStream fh(name,XS_IN);
	fh > sx > sy;
	if(sx != Size.X || sy != Size.Y) ErrH.Abort("Bad shutter BMP size...");
	data = new char[Size.X * Size.Y];
	fh.read(data,Size.X * Size.Y);
	fh.close();
}

void aciLocationShutterInfo::freeData(void)
{
	if(data){
		delete []data;
		data = NULL;
	}
}

void aciLocationShutterInfo::Redraw(void)
{
	XGR_PutSpr(curPos.X,curPos.Y,Size.X,Size.Y,data,XGR_CLIPPED | XGR_HIDDEN_FON);
}

int aciLocationShutterInfo::Quant(void)
{
	if(curDelta.X){
		if(curDelta.X < 0){
			if(curPos.X > trgPos.X){
				curPos.X += curDelta.X;
				if(curPos.X <= trgPos.X)
					curPos.X = trgPos.X;
			}
		}
		else {
			if(curPos.X < trgPos.X){
				curPos.X += curDelta.X;
				if(curPos.X >= trgPos.X)
					curPos.X = trgPos.X;
			}
		}
	}
	if(curDelta.Y){
		if(curDelta.Y < 0){
			if(curPos.Y > trgPos.Y){
				curPos.Y += curDelta.Y;
				if(curPos.Y <= trgPos.Y)
					curPos.Y = trgPos.Y;
			}
		}
		else {
			if(curPos.Y < trgPos.Y){
				curPos.Y += curDelta.Y;
				if(curPos.Y >= trgPos.Y)
					curPos.Y = trgPos.Y;
			}
		}
	}
	if(curPos.X == trgPos.X && curPos.Y == trgPos.Y) return 1;
	return 0;
}

void aciLocationShutterInfo::init(void)
{
	int i;
	init_size();
	for(i = 0; i < 4; i ++){
		if(Pos[i].X < 0){
			switch(Pos[i].X){
				case ACI_SHUTTER_LEFT:
					Pos[i].X = -Size.X;
					break;
				case ACI_SHUTTER_RIGHT:
					Pos[i].X = 800 - Size.X;
					break;
				case ACI_SHUTTER_CENTER:
					Pos[i].X = (800 - Size.X)/2;
					break;
			}
		}
		if(Pos[i].Y < 0){
			switch(Pos[i].Y){
				case ACI_SHUTTER_LEFT:
					Pos[i].Y = -Size.Y;
					break;
				case ACI_SHUTTER_RIGHT:
					Pos[i].Y = 600 - Size.Y;
					break;
				case ACI_SHUTTER_CENTER:
					Pos[i].Y = (600 - Size.Y)/2;
					break;
			}
		}
	}
}

void aciLocationInfo::alloc_gate_shutters(int num)
{
	int i;
	numGateShutters = num;
	GateShutters = new aciLocationShutterInfo*[num];
	for(i = 0; i < num; i ++)
		GateShutters[i] = NULL;
}

void aciLocationInfo::alloc_matrix_shutters(int id,int num)
{
	int i;
	switch(id){
		case 1:
			numMatrixShutters1 = num;
			MatrixShutters1 = new aciLocationShutterInfo*[num];
			for(i = 0; i < num; i ++)
				MatrixShutters1[i] = NULL;
			break;
		case 2:
			numMatrixShutters2 = num;
			MatrixShutters2 = new aciLocationShutterInfo*[num];
			for(i = 0; i < num; i ++)
				MatrixShutters2[i] = NULL;
			break;
	}
}

void fncMenu::prepare_items(void)
{
	fncMenuItem* p,*p1;

	p = (fncMenuItem*)inactive_items -> last;
	while(p){
		p1 = (fncMenuItem*)p -> prev;
		inactive_items -> dconnect(p);
		items -> connect(p);
		p = p1;
	}
	inactive_items -> init_list();
}

void fncMenu::deactivate(char* p)
{
	fncMenuItem* itm,*itm1;
	itm = (fncMenuItem*)items -> last;
	while(itm){
		itm1 = (fncMenuItem*)itm -> prev;
		if(!strcmp(p,itm -> name)){
			if(itm == curItem){
				curFunction = 0;
				init_curItem();
			}
			items -> dconnect(itm);
			inactive_items -> connect(itm);
		}
		itm = itm1;
	}
}

void fncMenu::deactivate_item(int code)
{
	fncMenuItem* itm,*itm1;
	itm = (fncMenuItem*)items -> last;
	while(itm){
		itm1 = (fncMenuItem*)itm -> prev;
		if(itm -> fnc_code == code){
			if(itm == curItem){
				curFunction = 0;
				init_curItem();
			}
			items -> dconnect(itm);
			inactive_items -> connect(itm);
		}
		itm = itm1;
	}
}

void fncMenu::delete_item(int code)
{
	fncMenuItem* itm,*itm1;
	itm = (fncMenuItem*)items -> last;
	while(itm){
		itm1 = (fncMenuItem*)itm -> prev;
		if(itm -> fnc_code == code){
			if(itm == curItem){
				curFunction = 0;
				init_curItem();
			}
			items -> dconnect(itm);
			delete itm;
		}
		itm = itm1;
	}
}

void fncMenu::activate_item(int code)
{
	fncMenuItem* itm,*itm1;
	itm = (fncMenuItem*)inactive_items -> last;
	while(itm){
		itm1 = (fncMenuItem*)itm -> prev;
		if(itm -> fnc_code == code){
			if(!items -> Size){
				curItem = itm;
				curFunction = curItem -> fnc_code;
			}
			inactive_items -> dconnect(itm);
			items -> connect(itm);
		}
		itm = itm1;
	}
}

void aciWorldMap::show_jump_count(void)
{
	int i,x,sx0,col_in0,col_in1,col_out,index0,index1;

	col_in0 = aciCurColorScheme[JUMP_EMPTY_COL];
	col_in1 = aciCurColorScheme[JUMP_FILLED_COL];
	col_out = aciCurColorScheme[JUMP_BORDER_COL];

	index0 = aciCurJumpCount;
	index1 = aciMaxJumpCount;

	x = 0;
	sx0 = JumpCountSizeX / index1;
	JumpCountX = (SizeX - sx0 * index1) / 2;
	for(i = 0; i < index0; i ++){
		mem_rectangle(x + JumpCountX,JumpCountY,sx0,JumpCountSizeY,SizeX,col_in1,col_out,1,redraw_buf);
		x += sx0;
	}
	for(i = index0; i < index1; i ++){
		mem_rectangle(x + JumpCountX,JumpCountY,sx0,JumpCountSizeY,SizeX,col_in0,col_out,1,redraw_buf);
		x += sx0;
	}
}

void actIntDispatcher::init_prompt(void)
{
	aButton* b;
	aIndData* ind;

	XGR_MousePromptData* p;

	ind = (aIndData*)indList -> last;
	while(ind){
		if(ind -> promptData){
			p = new XGR_MousePromptData;
			p -> ID = ind -> ID;
			p -> set_text(ind -> promptData);

			invPrompt -> AddElement((XListElement*)p);

			p = new XGR_MousePromptData;
			p -> ID = ind -> ID;
			p -> set_text(ind -> promptData);

			infPrompt -> AddElement((XListElement*)p);
		}
		ind = (aIndData*)ind -> prev;
	}
	b = (aButton*)intButtons -> last;
	while(b){
		b -> init();
		if(b -> promptData){
			p = new XGR_MousePromptData(b -> PosX,b -> PosY,b -> SizeX,b -> SizeY,0);
			p -> set_text(b -> promptData);

			invPrompt -> AddElement((XListElement*)p);

			p = new XGR_MousePromptData(b -> PosX,b -> PosY,b -> SizeX,b -> SizeY,0);
			p -> set_text(b -> promptData);

			infPrompt -> AddElement((XListElement*)p);
		}
		b = (aButton*)b -> prev;
	}
	b = (aButton*)invButtons -> last;
	while(b){
		b -> init();
		if(b -> promptData){
			p = new XGR_MousePromptData(b -> PosX,b -> PosY,b -> SizeX,b -> SizeY,0);
			p -> set_text(b -> promptData);

			switch(b -> ID){
				case ACI_WPN_PICKUP_BUTTON:
					p -> ID = ACI_PICKUP_WPN_ON;
					break;
				case ACI_ITM_PICKUP_BUTTON:
					p -> ID = ACI_PICKUP_ITM_ON;
					break;
			}

			invPrompt -> AddElement((XListElement*)p);
		}
		b = (aButton*)b -> prev;
	}
	b = (aButton*)infButtons -> last;
	while(b){
		b -> init();
		if(b -> promptData){
			p = new XGR_MousePromptData(b -> PosX,b -> PosY,b -> SizeX,b -> SizeY,0);
			p -> set_text(b -> promptData);

			infPrompt -> AddElement((XListElement*)p);
		}
		b = (aButton*)b -> prev;
	}
}

aciScreenTextPage::aciScreenTextPage(void)
{
	StrTable = NULL;
	NumStr = StartStr = 0;
}

aciScreenTextPage::~aciScreenTextPage(void)
{
	if(StrTable) delete StrTable;
}

aciScreenText::aciScreenText(void)
{
	data = NULL;
	dataSize = 0;

	Timer = 0;
	StrTimer = 1;

	flags = 0;

	DeltaY = 2; 

	font = 0;
	color = 255;
	MaxStrLen = 60;
	MaxPageStr = ACI_TEXT_PAGE_SIZE;

	PageTable = new iList;
	StrTable = new iList;

	CurPage = CurStr = 0;

	NextPageKey = new aKeyObj;
	NextPageKey -> add_key(SDL_SCANCODE_SPACE);
	NextPageKey -> add_key(SDL_SCANCODE_TAB);

	convBuf = new char[ACI_MAX_STRING_LEN];
	memset(convBuf,0,ACI_MAX_STRING_LEN);
}

aciScreenText::~aciScreenText(void)
{
	free();
	delete NextPageKey;
	delete PageTable;
	delete StrTable;
}

void aciScreenText::free(void)
{
	aciScreenTextString* s,*s1;
	aciScreenTextPage* p,*p1;

	CurStr = CurPage = 0;

	if(data) delete data;
	data = NULL;
	dataSize = 0;

	s = (aciScreenTextString*)StrTable -> last;
	while(s){
		s1 = (aciScreenTextString*)s -> prev;
		delete s;
		s = s1;
	}
	StrTable -> init_list();

	p = (aciScreenTextPage*)PageTable -> last;
	while(p){
		p1 = (aciScreenTextPage*)p -> prev;
		delete p;
		p = p1;
	}
	PageTable -> init_list();

	flags &= ~ACI_TEXT_INIT;
}

void aciScreenText::alloc(char* ptr,int sz)
{
	free();
	data = new char[sz + 1];
	memcpy(data,ptr,sz);
	data[sz] = '\n';
	dataSize = sz + 1;

	aciPrepareScreenText(data,dataSize,MaxStrLen);

	prepare();
	CurPageData = (aciScreenTextPage*)PageTable -> first;
	flags |= ACI_TEXT_INIT;
	flags &= ~ACI_TEXT_END;
}

void aciScreenText::prepare(void)
{
	int i,j,cur_str,start_str,cur_text_str;
	char* strStart;
	aciScreenTextString* s;
	aciScreenTextPage* p;

	strStart = data;
	start_str = cur_str = cur_text_str = 0;
	for(i = 0; i < dataSize; i ++){
		if(data[i] == '\\' && i < (dataSize - 1) && data[i + 1] == '\\'){
			cur_text_str = MaxPageStr;
			data[i] = 0;
			data[i + 1] = 0;
			while(i < (dataSize - 1) && data[i] != '\n') i ++;
		}
		if(data[i] == '\n'){
			data[i] = 0;
			s = new aciScreenTextString;
			s -> data = strStart;
			StrTable -> connect((iListElement*)s);
			if(i < (dataSize - 1)) strStart = data + i + 1;

			cur_text_str ++;
			cur_str ++;
		}
		if(cur_text_str >= MaxPageStr){
			p = new aciScreenTextPage;
			p -> StartStr = start_str;
			p -> NumStr = cur_str - start_str + 1;
			if(p -> NumStr)
				p -> StrTable = new char*[p -> NumStr];
			PageTable -> connect((iListElement*)p);

			start_str = cur_str;
			cur_text_str = 0;
		}
	}
	if(cur_text_str){
		p = new aciScreenTextPage;
		p -> StartStr = start_str;
		p -> NumStr = cur_str - start_str + 1;
		if(p -> NumStr)
			p -> StrTable = new char*[p -> NumStr];
		PageTable -> connect((iListElement*)p);

		start_str = cur_str + 1;
		cur_text_str = 0;
	}
	p = (aciScreenTextPage*)PageTable -> first;
	for(i = 0; i < PageTable -> Size; i ++){
		s = (aciScreenTextString*)StrTable -> first;
		for(j = 0; j < p -> StartStr; j ++){
			s = (aciScreenTextString*)s -> next;
		}
		for(j = 0; j < p -> NumStr; j ++){
			p -> StrTable[j] = s -> data;
			s = (aciScreenTextString*)s -> next;
		}
		p = (aciScreenTextPage*)p -> next;
	}
	for(i = 0; i < dataSize; i ++)
		if(data[i] == '\r') data[i] = 0;
}

void aciScreenText::redraw(void)
{
	int i,x,y;
	aciScreenTextPage* p;
	char* str;

	if(flags & ACI_TEXT_END) return;

	p = (aciScreenTextPage*)PageTable -> first;
	for(i = 0; i < CurPage; i ++){
		p = (aciScreenTextPage*)p -> next;
	}

	y = (I_RES_Y - (DeltaY + aTextHeight32((void *)"",font,1)) * p -> NumStr)/2;
	for(i = 0; i < CurStr; i ++){
		str = p -> StrTable[i];
		x = (I_RES_X - aTextWidth32(str,font,1))/2;

		aOutText32(x,y,color,str,font,1,1);
		y += DeltaY + aTextHeight32(str,font,1);
	}
	CurPageData = p;
}

void aciScreenText::Quant(void)
{
	Timer ++;
	flags &= ~ACI_TEXT_REDRAW;
	if(Timer >= StrTimer){
		if(CurStr < (CurPageData -> NumStr - 1)){
			CurStr ++;
		}
		else
			flags |= ACI_TEXT_REDRAW;

		Timer = 0;
	}
}

int aciScreenText::NextPage(void)
{
	if(CurPage < (PageTable -> Size - 1)){
		CurPage ++;
		CurStr = 0;
		return 1;
	}
	flags |= ACI_TEXT_END;
	return 0;
}

#ifdef _DEBUG
void actIntDispatcher::save_items(void)
{
	int i;
	invItem* p;
	XStream fh;

	char* ptr;
	char str[256];

	fh.open("actint/temp/items.bat",XS_OUT);
	fh < "@echo off\r\n";
	for(i = 1; i < ACI_MAX_TYPE; i ++){
		p = get_item(i);
		if(p){
			p -> init();
			strcpy(str,p -> fname);
			ptr = str + strlen(str) - 9;
			str[strlen(str) - 3] = 0;
			fh < "call r1.bat " < "tga/" < ptr < "tga " <= p -> ScreenSizeX < " " <= p -> ScreenSizeY < "\r\n";
		}
	}
	fh < "call g1.bat\r\n";
	fh.close();

	fh.open("actint/temp/i_items.bat",XS_OUT);
	fh < "@echo off\r\n";
	for(i = 1; i < ACI_MAX_TYPE; i ++){
		p = get_iitem(i);
		if(p){
			p -> init();
			strcpy(str,p -> fname);
			ptr = str + strlen(str) - 9;
			str[strlen(str) - 3] = 0;
			fh < "call r.bat " < "tga/" < ptr < "tga " <= p -> ScreenSizeX < " " <= p -> ScreenSizeY < "\r\n";
		}
	}
	fh < "call g.bat\r\n";
	fh.close();
}
#endif

void invItem::set_template(char* p)
{
	int sz = strlen(p) + 1;
	pTemplate = new char[sz];
	strcpy(pTemplate,p);
}
