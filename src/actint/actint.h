
#define _ACI_PACK_SAVES_

/* ----------------------------- Debug options ------------------------------ */

#define _ACI_CHECK_DIALOGS_
//#define _ACI_SKIP_MAINMENU_

#ifdef _DEBUG
//#define _ACTINT_MEMSTAT_

//#define _ACI_STARTUP_LOAD_GAME_

//#define _FULLSCREEN_MODE_
#endif

//#define _ACI_SKIP_SHOP_
//#define _ACI_NO_SHOP_ANIMATION_

//#define _ACI_ESCAVE_DEBUG_

/* -------------------------------------------------------------------------- */

#define _ACI_BML_FONTS_
#ifdef _DEBUG
//#define _GENERATE_MATRIX_SHAPES_
//#define _GENERATE_iMATRIX_SHAPES_
//#define _GENERATE_ITEM_DATA_
//#define _ACI_BOUND_TEST_
//#define _ACI_LOGFILE_

#ifdef _ACTINT_MEMSTAT_
#define aMEMSTAT(a) aMemStatFile < "\r\n------->>>>>> " < a < " ->\t\t\t" <= memQ < "...";
#define aMEMSTAT_AUTO aMemStatFile < "\r\n" < __FILE__ < ", " <= __LINE__ < " ->\t" <= memQ;
#else
#define aMEMSTAT(a)
#endif
#endif

const int 	ACI_STARTUP_CREDITS	= 500;

const int 	AS_NUM_FONTS		= 8;
const int 	AS_NUM_FONTS_32 	= 4;

const int 	MAX_NUM_MAP		= 13;

struct actEvent;

#ifdef _ACI_BML_FONTS_

// aciFont flags...
#define ACI_RANGE_FONT		0x01

struct aciFont
{
	int flags;

	int SizeX;
	int SizeY;
	int Size;

	unsigned char* data;
	char* LeftOffs;
	char* RightOffs;

	void load(char* fname);
	void calc_offs(void);

	aciFont(void);
	~aciFont(void);
};
#endif

#define BM_GLOBAL_SIZE		1000

// flags...
#define BMI_DATA_LOADED 	0x01

struct aciBitmapMenuItem : public iListElement
{
	int ID;

	int PosX;
	int PosY;

	int SizeX;
	int SizeY;

	int numStates;
	int curState;

	int flags;

	char* fname;
	unsigned char* data;

	void init_name(char* p);
	void load(void);
	void free(void);

	int check_xy(int x,int y);
	void change(void);

	void redraw(int bsx,int bsy,unsigned char* buf);

	aciBitmapMenuItem(void);
	~aciBitmapMenuItem(void);
};

// flags...
#define BM_REBUILD		0x01
#define BM_REMAP_COORDS 	0x02

struct aciBitmapMenu : public iListElement
{
	int ID;

	int SizeX;
	int SizeY;

	int PosX;
	int PosY;

	int flags;

	int activeCount;
	int curCount;

	iList* items;
	unsigned char* pictBuf;

	iListElement* upMenu;

	void alloc_mem(void);
	void free_mem(void);

	void init(void);
	void finit(void);

	void go2upmenu(int mode = -1);

	void change(int x,int y);
	int check_xy(int x,int y);

	void remap_coords(void);

	void add_item(aciBitmapMenuItem* p);

	void build_pict(void);
	void redraw(int x,int y,int sx,int sy);

	aciBitmapMenu(void);
	~aciBitmapMenu(void);
};

struct aKeyObj
{
	iList* codes;

	int check(int k);
	void add_key(int k);
	void free_list(void);

	aKeyObj(void);
	~aKeyObj(void);
};

struct aciWorldInfo : public iListElement
{
	int ID;
	int uvsID;
	int flags;

	char* name;

	int PosX;
	int PosY;

	int letter;
	int shape_id;

	int font;

	char* links;

	iListElement* owner;

	void add_link(int id);
	void delete_link(int id);

	void redraw(int bsx,int bsy,unsigned char* buf);

	aciWorldInfo(void);
	~aciWorldInfo(void);
};

// aciWorldMap flags...
#define WMAP_REBUILD_PIC	1
#define WMAP_REMAP_COORDS	2

struct aciWorldMap : public iListElement
{
	int SizeX;
	int SizeY;

	int flags;

	int JumpCountSizeX;
	int JumpCountSizeY;

	int JumpCountX;
	int JumpCountY;

	unsigned char* redraw_buf;

	iList* world_list;
	aciWorldInfo** world_ptr;
	signed char* world_ids;

	short* ShapeSizeX;
	short* ShapeSizeY;

	short* ShapeOffsX;
	short* ShapeOffsY;

	char** shape_files;
	unsigned char** shapes;

	void quant(void);
	void init(void);
	void calc_shapes(void);
	void show_link(int w1,int w2);
	void build_map(void);
	void redraw(int x,int y);

	void show_jump_count(void);

	void remap_coords(void);

	int world_key(int id);

	aciWorldInfo* get_world(int id);
	aciWorldInfo* get_world_p(char* ptr);

	aciWorldMap(void);
	~aciWorldMap(void);
};

// aciLocationShutterInfo::mode values...
#define ACI_SHUTTER_OPEN	0
#define ACI_SHUTTER_CLOSE	1

struct aciPos
{
	int X;
	int Y;

	aciPos(void){ X = Y = 0; }
};

struct aciLocationShutterInfo
{
	aciPos curPos;
	aciPos trgPos;

	aciPos Pos[4];

	aciPos Delta;
	aciPos curDelta;

	aciPos Size;

	int mode;

	char* name;
	char* data;

	void init(void);
	void init_size(void);
	void prepare(int m);
	void loadData(void);
	void freeData(void);

	int Quant(void);
	void Redraw(void);

	aciLocationShutterInfo(void){ name = data = NULL; mode = 0; }
	~aciLocationShutterInfo(void);
};

struct aciLocationInfo : public iListElement
{
	int ID;
	char* nameID;
	char* nameID2;

	int numColorScheme;
	char* mapName;
	char* palName;
	char* screenID;

	char* soundResPath;

	char* ExcludeItems;

	int SaveScreenID;

	int numGateShutters;
	aciLocationShutterInfo** GateShutters;

	int numMatrixShutters1;
	aciLocationShutterInfo** MatrixShutters1;

	int numMatrixShutters2;
	aciLocationShutterInfo** MatrixShutters2;

	int WorldID;

	char* numIndex;

	char** objIDs;
	iListElement** objList;

	char** s_objIDs;
	iListElement** s_objList;

	iList* mapData;

	void prepare(void);
	void init_map_data(void);
	void alloc_gate_shutters(int num);
	void alloc_matrix_shutters(int id,int num);

	iScreen* get_shop_scr(void);

	aciLocationInfo(void);
	~aciLocationInfo(void);
};

struct aciMechosPartInfo
{
	int baseID[2];
	int targetID[2];

	aciMechosPartInfo(void) {
		baseID[0] = baseID[1] = -1; 
		targetID[0] = targetID[1] = -1; 
	}
};

const int 	AS_MAX_AVI_ID		= 0x06;

// invItem flags...
const int  AS_ITEM_DATA_LOADED	= 0x01;
const int  INV_CLONE 		= 0x04;
const int  INV_ITEM_REDRAW		= 0x08;
const int  INV_ITEM_SHOW_LOAD	= 0x10;
const int  INV_ITEM_NO_PAY		= 0x20;
const int  INV_ITEM_NO_ACTIVATE	= 0x40;
const int  INV_ITEM_SHOW_ESCAVE	= 0x80;
const int  INV_ITEM_NEW		= 0x100;

const int  INV_FONT	= 1;
const int  INV_COL	= 30;

// Slot types...
const int 	AS_WEAPON_SLOT	= 1;
const int 	AS_TWEAPON_SLOT = 2;
const int 	AS_DEVICE_SLOT	= 3;

// classID values
const int 	AS_ITEM_CLASS		= 0;
const int 	AS_WEAPON_CLASS 	= 1;

const int 	INV_ITEM_NUM_PARAMS	= 5;
const int 	ACI_MAX_PRM_LEN 	= 10;

struct invItem : public iListElement
{
	int ID;
	int classID;

	int flags;

	int slotType;
	int dropCount;

	int ScreenSizeX;
	int ScreenSizeY;

	int MatrixX;
	int MatrixY;

	int ShapeLen;
	int* ShapeX;
	int* ShapeY;

	int EvCode;

	int numComments;
	char** comments;

	char* pData;
	char* promptData;
	char* pTemplate;

	int numAviIDs;
	char** avi_ids;

	int ShapeCenterX;
	int ShapeCenterY;

	int ShapeSizeX;
	int ShapeSizeY;

	int NumIndex;

	actintItemData* item_ptr;
	void* uvsDataPtr;

	char* fname;
	char* frame;

	iListElement* menu;

	aciMechosPartInfo* partData;

	void init_fname(char* p);
	void init_name(char* p);
	void init_avi_id(char* p,int num);

	void clone(invItem* p);

	void init(void);
	void finit(void);
	void load_frame(void);
	void free_frame(void);

	void get_coords(int px,int py,int& x,int& y,int& sx,int& sy);

	void alloc_prm(void);
	void set_template(char* p);
	void calc_shape_center(void);

	void redraw(int x,int y,int drop_mode = 0,int flush_mode = 0);

	invItem(void);
	~invItem(void);
};

enum CellTypes
{
	AS_NO_CELL,		// 0
	AS_ITEM_CELL,		// 1
	AS_WEAPON_CELL, 	// 2

	AS_MAX_CELL
};

// invMatrixCell flags...
const int 	AS_BUSY_CELL	= 0x01;
const int 	AS_IN_SHADOW	= 0x02;
const int 	AS_REDRAW_CELL	= 0x04;

struct invMatrixCell : public iListElement
{
	int type;
	int flags;

	int slotType;
	int slotNumber;

	invItem* item;

	void put_item(invItem* p);
	void remove_item(void);

	void init_mem(void);

	invMatrixCell(void);
};

// invMatrix flags...
const int 	IM_REDRAW		= 0x01;
const int 	IM_FLUSH		= 0x02;
const int 	IM_CLONE		= 0x04;
const int 	IM_REDRAW_SHADOW	= 0x08;
const int 	IM_NOT_COMPLETE 	= 0x10;
const int 	IM_RAFFA		= 0x20;

const int 	IM_NUM_PARAMS		= 7;

struct invMatrix : public iListElement
{
	int internalID;

	int type;
	int flags;

	int SizeX;
	int SizeY;

	int ScreenX;
	int ScreenY;

	unsigned int anchor;

	int ScreenSizeX;
	int ScreenSizeY;

	int maxLoad;

	char* mech_name;
	invMatrixCell** matrix;
	char* matrix_heap;

	iList* items;
	bmlObject* back;

	char* pData;

	int numAviIDs;
	char** avi_ids;
	void* uvsDataPtr;

	int put_item(int x,int y,invItem* p,int mode = 0);
	int put_item_shadow(int x,int y,invItem* p);
	int auto_put_item(invItem* p);
	void remove_item(invItem* p,int mode = 0);

	int check_slot(int type);

	void clone(invMatrix* p);
	void backup(invMatrix* p);

	invItem* get_item(int x,int y);
	invItem* get_area_item(int x,int y,invItem* it);

	int check_fit(int x,int y,invItem* p);
	int get_item_slot(invItem* p);

	int check_xy(int x,int y);
	void get_item_coords(invItem* p,int& x,int &y,int &sx,int &sy);

	void alloc_matrix(void);
	void free_matrix(void);

	void alloc_prm(void);

	void init(void);
	void init_avi_id(char* p,int num);

	void redraw(void);
	void redraw_matrix(void);
	void redraw_items(void);
	void flush(void);

	void clear_shadow_cells(void);
	void redraw_shadow_cells(void);
	int check_redraw(void);

	void set_flush(void);
	void set_redraw(void);

	void fill(invItem* p);

#if defined(_GENERATE_MATRIX_SHAPES_) || defined(_GENERATE_iMATRIX_SHAPES_)
	void generate_shape(void);
	void generate_floor(void);
#endif

	int slot_exist(int id);

	invMatrix(void);
	invMatrix(int sx,int sy);

	~invMatrix(void);
};

// aIndData flags...
const int 	IND_REDRAW	= 0x01;
const int 	IND_FLUSH	= 0x02;
const int 	IND_SHOW_VALUES = 0x04;
const int 	IND_DISABLED	= 0x08;

// aIndData types...
#define IND_CLOCK	1
#define IND_BARREL	2

// aIndData::CornerNum values...
#define IND_NONE	0
#define IND_UP_LEFT	1
#define IND_UP_RIGHT	2
#define IND_DN_LEFT	3
#define IND_DN_RIGHT	4

// IDs...
#define IND_N0		0
#define IND_N1		1
#define IND_NRG 	2
#define IND_LOAD	3
#define IND_JUMP	4
#define IND_WPN 	5
#define IND_DVC 	6
#define IND_SPIRAL	7

struct aIndData : public iListElement
{
	int ID;

	int flags;

	int NumVals;

	int PosX;
	int PosY;

	int dX;
	int dY;

	int type;

	int SizeX;
	int SizeY;

	int CornerNum;
	char* pict_buf;

	int* colors;
	int* check_points;
	int* value;
	int** value_ptr;
	int** max_value;

	char* promptData;

	bmlObject* bml;

	void init(void);
	void finit(void);

	void build_pict(void);
	void change_val(int val);

	void redraw(int dx = -1,int dy = -1);
	void flush(int dx = -1,int dy = -1);

	void set_flush(void);
	void set_redraw(void);

	void alloc_mem(void);

	aIndData(void);
	~aIndData(void);
};

// aButton flags...
const int 	B_REDRAW	= 0x01;
const int 	B_FLUSH 	= 0x02;
const int 	B_ACTIVE	= 0x08;
const int 	B_PRESSED	= 0x10;
const int 	B_UNPRESS	= 0x20;
const int 	B_FRAMES_LOADED = 0x40;

// Button types...
const int 	INTERF_BUTTON		= 0x00;
const int 	INV_BUTTON		= 0x01;
const int 	INFO_BUTTON		= 0x02;

struct aButton : public iListElement
{
	int ID;
	int ControlID;

	int PosX;
	int PosY;

	unsigned int anchor;

	int type;
	int flags;

	int activeCount;
	int curCount;

	int eventCode;
	int eventData;

	int SizeX;
	int SizeY;

	int numFrames;

	char* fname;

	int cur_frame;
	char* frameSeq;

	char* promptData;

	aKeyObj* scankey;

	void set_fname(char* p);
	void load_frames(void);
	void free_frames(void);

	void press(void);

	void redraw(void);
	void flush(void);

	void init(void);
	void finit(void);

	void set_flush(void);
	void set_redraw(void);

	int check_xy(int x,int y);

	void add_key(int key);

	aButton(void);
	~aButton(void);
};

// fncMenuItem flags...
const int 	FM_SELECTED		= 0x01;
const int 	FM_VISIBLE		= 0x02;
const int 	FM_SUBMENU_ITEM 	= 0x04;
const int 	FM_BSUBMENU_ITEM	= 0x08;
const int 	FM_NO_DELETE		= 0x10;
const int 	FM_CLONE		= 0x20;

struct fncMenuItem : public iListElement
{
	int PosX;
	int PosY;

	int SizeX;
	int SizeY;

	int flags;
	int fnc_code;
	int submenuID;

	int font;
	int space;

	aKeyObj* scankey;
	char* name;
	int name_len;

	actEvent* eventPtr;

	void add_key(int key);
	void init_name(const char* p);
	void init(void);

	int check_y(int y);

	void redraw(int bsx,int bsy,unsigned char* buf,int x,int y);
	void redraw_str(int bsx,int bsy,unsigned char* buf,int x,int y,unsigned char* str);

	void clone(fncMenuItem* p);

	fncMenuItem(void);
	~fncMenuItem(void);
};

const int 	FM_NUM_V_ITEMS	= 10;
const int 	FM_PREFIX_DELTA = 10;
const int 	FM_PREFIX_LEN	= 512;

// fncMenu flags...
const int 	FM_REDRAW		= 0x01;
const int 	FM_FLUSH		= 0x02;
const int 	FM_ACTIVE		= 0x08;
const int 	FM_ITEM_MENU		= 0x10;
const int 	FM_OFF			= 0x20;
const int 	FM_LOCATION_MENU	= 0x40;
const int 	FM_ISCREEN_MENU 	= 0x80;
const int 	FM_NO_DEACTIVATE	= 0x100;
const int 	FM_SUBMENU		= 0x200;
const int 	FM_HIDDEN		= 0x400;
const int 	FM_LOCK 		= 0x800;
const int 	FM_NO_ALIGN		= 0x1000;
const int 	FM_MAIN_MENU		= 0x2000;
const int 	FM_RANGE_FONT		= 0x4000;

struct fncMenu : public iListElement
{
	int type;

	int PosX;
	int PosY;
	unsigned int anchor;

	int SizeX;
	int SizeY;

	int VItems;
	int itemY;

	int curFunction;
	int fncCode;
	int activeCount;
	int curCount;

	int flags;

	int vSpace;

	int bCol;

	unsigned char* prefix;
	unsigned char* postfix;

	int PrefixX;
	int PrefixY;
	int PrefixSX;
	int PrefixSY;
	int PrefixDelta;

	aKeyObj* scankey;
	aKeyObj* up_key;
	aKeyObj* down_key;

	ibsObject* ibs;
	bmlObject* bml;
	char* bml_name;
	char* ibs_name;

	aButton* trigger;
	int trigger_code;

	iList* items;
	iList* inactive_items;

	fncMenuItem* firstItem;
	fncMenuItem* curItem;

	fncMenuItem* up_obj;
	fncMenuItem* down_obj;
	fncMenuItem* upMenuItem;

	fncMenu* upMenu;
	fncMenu* subMenu;

	iBitmapElement* iScreenOwner;

	void add_key(int key);
	void add_item(fncMenuItem* p);
	void sort(void);

	void set_prefix(unsigned char* p);
	void set_postfix(unsigned char* p);

	void step_up(void);
	void step_down(void);

	void init(void);
	void finit(void);

	void init_objects(void);
	void init_submenu(fncMenu* p);
	void clean(void);

	void go2upmenu(int mode = -1);

	int check_xy(int x,int y);
	int change(int x,int y,int mode = 0);

	void redraw(void);
	void flush(void);

	void set_flush(void);
	void set_redraw(void);

	void add_obj(fncMenuItem* p);
	void delete_obj(fncMenuItem* p);
	fncMenuItem* get_obj(const char* ptr);

	void init_curItem(void);
	void init_redraw(void);

	void add_menu(fncMenu* p);
	int remove_menu(int code);
	int check_menu(int code);

	void prepare_items(void);
	void deactivate(char* p);

	void activate_item(int code);
	void deactivate_item(int code);
	void delete_item(int code);

	fncMenuItem* get_fnc_item(int fnc);

	fncMenu(void);
	~fncMenu(void);
};

// fncMenuSet flags...
const int 	FMC_REDRAW	= 0x01;
const int 	FMC_DATA_INIT	= 0x02;

struct fncMenuSet
{
	int PosX;
	int flags;

	int SizeX;
	int SizeY;

	int ScreenX;
	int ScreenY;
	int ScreenSizeX;
	int ScreenSizeY;

	int font;
	int space;

	iBitmapElement* data;
	iBitmapElement* redraw_data;
	iList* items;

	fncMenu* qMenu;

	void init(void);
	void finit(void);
	void init_redraw(void);
	void build(void);

	char* get(void);

	void add(fncMenu* p,int mode = 0);

	void redraw(void);
	void redraw_owner(void);
	void set_redraw(void);

	void shift(int dx);
	void limitPosX(void);

	int check_xy(int x,int y);
	void key_trap(int x,int y);

	fncMenuSet(void);
	~fncMenuSet(void);
};

// CounterPanel types...
const int 	CP_INT		= 0x00;
const int 	CP_INF		= 0x01;
const int 	CP_INV		= 0x02;

// CounterPanel flags...
const int 	CP_REDRAW	= 0x01;
const int 	CP_FLUSH	= 0x02;
const int 	CP_RANGE_FONT	= 0x04;

struct CounterPanel : public iListElement
{
	int ID;
	int type;

	int PosX;
	int PosY;

	unsigned int anchor;

	int SizeX;
	int SizeY;

	int MaxLen;

	int flags;
	int font;
	int color;

	int hSpace;
	int* value_ptr;
	int last_value;

	ibsObject* ibs;
	char* ibs_name;

	XBuffer* xconv;

	iBitmapElement* iScreenOwner;

	void move(int delta);

	void init(void);
	void finit(void);

	void redraw(void);
	void flush(void);

	void set_flush(void);
	void set_redraw(void);

	CounterPanel(void);
	~CounterPanel(void);
};

// InfoPanel flags...
const int 	IP_REDRAW	= 0x01;
const int 	IP_FLUSH	= 0x02;
const int 	IP_NO_ALIGN	= 0x04;
const int 	IP_NO_REDRAW	= 0x08;
const int 	IP_RANGE_FONT	= 0x10;

struct InfoPanelItem : public iListElement
{
	int color;
	int font;
};

struct InfoPanel : public iListElement
{
	int type;
	int interf_type;

	int PosX;
	int PosY;

	unsigned int anchor;

	int OffsX;
	int OffsY;

	int SizeX;
	int SizeY;

	int MaxStr;
	int bCol;

	int flags;
	int font;

	int vSpace;
	int hSpace;

	ibsObject* ibs;
	bmlObject* bml;
	char* bml_name;
	char* ibs_name;

	iBitmapElement* iScreenOwner;

	iList* items;
	iList* item_pool;

	void move(int delta);

	void add_item(const char* ptr,int fnt = -1,int col = -1);
	void add_items(int num,char** ptr,int fnt = -1,int col = -1);
	void free_list(void);

	void init(void);
	void finit(void);

	void redraw(void);
	void flush(void);

	void set_flush(void);
	void set_redraw(void);

	InfoPanel(void);
	~InfoPanel(void);
};

// Event codes...
enum actEventCodes {
	EV_CHANGE_MODE = ACI_MAX_EVENT + 1,
	EV_SET_MODE,
	EV_ACTIVATE_MENU,
	EV_CHANGE_SCREEN,
	EV_FULLSCR_CHANGE,
	EV_ACTIVATE_IINV,
	EV_DEACTIVATE_IINV,
	EV_ACTIVATE_MATRIX,
	EV_DEACTIVATE_MATRIX,
	EV_EVINCE_PALETTE,
	EV_INIT_MATRIX_OBJ,
	EV_INIT_SC_MATRIX_OBJ,
	EV_REDRAW,
	EV_CANCEL_MATRIX,
	EV_AUTO_MOVE_ITEMS,
	EV_SET_MECH_NAME,

	EV_NEXT_SHOP_AVI,
	EV_PREV_SHOP_AVI,

	EV_CHANGE_AVI_LIST,

	EV_BUY_ITEM,

	EV_SET_ITM_PICKUP,
	EV_SET_WPN_PICKUP,

	EV_ACTIVATE_SHOP_MENU,
	EV_CHOOSE_SHOP_ITEM,

	EV_NEXT_PHRASE,
	EV_START_SPEECH,
	EV_SHOW_QUESTS,
	EV_ASK_QUEST,

	EV_TRY_2_ENTER,
	EV_GET_CIRT,

	EV_TAKE_ELEECH,
	EV_GET_ELEECH,
	EV_ISCR_KEYTRAP,

	EV_LOCK_ISCREEN,
	EV_UNLOCK_ISCREEN,

	EV_SELL_MOVE_ITEM,
	EV_CHANGE_AVI_INDEX,

	EV_TELEPORT,

	EV_INIT_BUTTONS,

	EV_ENTER_TEXT_MODE,
	EV_LEAVE_TEXT_MODE,

	EV_PROTRACTOR_EVENT,
	EV_MECH_MESSIAH_EVENT,

	EV_GET_BLOCK_PHRASE,

	EV_PAUSE_AML,
	EV_RESUME_AML,

	EV_ENTER_CHAT,
	EV_LEAVE_CHAT,

	EV_ITEM_TEXT,

	EV_GET_RUBOX,
	EV_INIT_AVI_OBJECT,

	EV_MAX_CODE
};

struct actEvent
{
	int code;
	int data;

	actintItemData* ptr;
};

const int  AS_MAX_EVENT	= 50;

struct actEventHeap
{
	int size;
	int first_index;
	int last_index;
	int flags;

	actEvent** table;
	char* mem_heap;

	void clear(void);
	void put(int cd,int dt = 0,actintItemData* p = NULL);
	actEvent* get(void);

	actEventHeap(void);
	~actEventHeap(void);
};

#define ACI_TEXT_PAGE_SIZE	16
#define ACI_MAX_STRING_LEN	256

struct aciScreenTextString : iListElement
{
	char* data;
};

struct aciScreenTextPage : iListElement
{
	int StartStr;
	int NumStr;

	char** StrTable;

	aciScreenTextPage(void);
	~aciScreenTextPage(void);
};

// aciScreenText::flags...
#define ACI_TEXT_INIT		0x01
#define ACI_TEXT_END		0x02
#define ACI_TEXT_REDRAW 	0x04

struct aciScreenText
{
	int flags;

	int font;
	int color;
	int MaxStrLen;
	int MaxPageStr;

	int DeltaY;

	int Timer;
	int StrTimer;

	int CurPage;
	int CurStr;
	aciScreenTextPage* CurPageData;

	int dataSize;
	char* data;
	char* convBuf;

	iList* StrTable;
	iList* PageTable;

	aKeyObj* NextPageKey;

	void redraw(void);
	void Quant(void);
	int NextPage(void);

	void prepare(void);
	void alloc(char* ptr,int sz);
	void free(void);
	void clear(void){ CurPage = CurStr = 0; }

	aciScreenText(void);
	~aciScreenText(void);
};

// actInt modes...
const int 	AS_INV_MODE	= 0x00;
const int 	AS_INFO_MODE	= 0x01;

// flags...
const int 	AS_FULL_REDRAW		= 0x01;
const int 	aMS_LEFT_PRESS		= 0x02;
const int 	aMS_RIGHT_PRESS 	= 0x04;
const int 	aMS_MOVED		= 0x08;
const int 	aMS_PRESS		= aMS_LEFT_PRESS | aMS_RIGHT_PRESS | aMS_MOVED;
const int 	AS_CHANGE_MODE		= 0x10;
const int 	AS_INV_MOVE_ITEM	= 0x20;
const int 	AS_INV_SET_DROP 	= 0x40;
const int 	AS_FULL_FLUSH		= 0x80;
const int 	AS_FULLSCR		= 0x100;
const int 	AS_ISCREEN		= 0x200;
const int 	AS_ISCREEN_INV_MODE	= 0x400;
const int 	AS_EVINCE_PALETTE	= 0x800;
const int 	AS_LOCKED		= 0x1000;
const int 	AS_TEXT_MODE		= 0x2000;
const int 	AS_CHAT_MODE		= 0x4000;
const int 	AS_WORLDS_INIT		= 0x8000;

// actInt camera offsets
const int AS_INF_CAMERA_OFFSET = 0;
const int AS_INV_CAMERA_OFFSET = 200;

struct actIntDispatcher
{
	int flags;
	int curMode;

	int curScrMode;
	int prevScrMode;

	iList* menuList;
	iList* b_menuList;
	iList* itemList;
	iList* matrixList;

	aciPromptData* curPrompt;

	iList* locationList;
	aciLocationInfo* curLocData;

	iList* i_infoPanels;
	iList* infoPanels;

	iList* i_matrixList;
	iList* i_Counters;
	iList* i_itemList;
	iList* i_menuList;

	iList* intButtons;
	iList* invButtons;
	iList* infButtons;

	iList* intCounters;
	iList* invCounters;
	iList* infCounters;

	iList* indList;

	XGR_MousePromptScreen* invPrompt;
	XGR_MousePromptScreen* infPrompt;

	aKeyObj* ModeObj;
	aKeyObj* InvObj;
	aKeyObj* InfoObj;

	invMatrix* curMatrix;
	invMatrix* secondMatrix;
	invMatrix* i_curMatrix;
	invMatrix* a_curMatrix;

	invItem* curItem;

	InfoPanel* iP;
	InfoPanel* iscr_iP;
	InfoPanel* ascr_iP;

	aciWorldMap* wMap;

	bmlObject* mapObj;
	char** map_names;

	fncMenuSet* qMenu;

	iList* ibsList;
	iList* backList;
	ibsObject* curIbs;
	int curIbsID;

	iList* freeItemList;
	iList* freeMatrixList;
	iList* freeMenuItemList;

	aciScreenText* ScrTextData;

#ifdef _ACI_LOGFILE_
	XStream logFile;
#endif

	actEventHeap* events;

	void add_matrix(invMatrix* m);
	void add_imatrix(invMatrix* m);
	void add_menu(fncMenu* p);
	void add_imenu(fncMenu* p);
	void add_item(invItem* p);
	void add_iitem(invItem* p);
	void add_ind(aIndData* p);
	void add_locdata(aciLocationInfo* p);
	void add_bmenu(aciBitmapMenu* p);

	void send_event(int cd,int dt = 0,actintItemData* p = NULL);

	invMatrix* get_matrix(int tp);
	invMatrix* get_imatrix(int tp);

	invItem* get_item(int id);
	invItem* get_iitem(int id);
	fncMenu* get_menu(int mn);
	fncMenu* get_imenu(int mn);
	aciBitmapMenu* get_bmenu(int mn);
	aButton* get_button(int id);
	aIndData* get_ind(int id);
	aciLocationInfo* get_locdata(int id);
	int get_locdata_id(const char* name);
	InfoPanel* get_info_panel(int id);
	InfoPanel* get_aci_info_panel(int id);

	CounterPanel* get_counter(int id);
	CounterPanel* get_icounter(int id);

	bmlObject* get_back_bml(int id);
	ibsObject* get_ibs(int id);

	void init(void);
	void finit(void);

	void init_prompt(void);

	void i_init(void);
	void i_finit(void);

	void redraw(void);
	void ind_redraw(void);
	void text_redraw();
	void flush(void);
	void pal_flush();

	void i_redraw(void);
	void i_flush(void);

	void init_menus(void);
	void init_inds(void);
	void calc_load(void);

	void init_submenu(fncMenu* m);

	void change_ibs(int id);
	void next_ibs(void);
	void load_ibs(void);
	void free_ibs(void);

	void lock(void){ flags |= AS_LOCKED; }
	void unlock(void){ flags &= ~AS_LOCKED; }

	void KeyQuant(void);
	void EventQuant(void);

	void iKeyQuant(void);
	void iKeyTrap(int cd);

	invItem* get_item_ptr(int id);
	invItem* get_item_ptr_xy(int id,int x,int y);

	invItem* alloc_item(void);
	fncMenuItem* alloc_menu_item(void);
	invMatrix* alloc_matrix(int type,int imode = 0);

	void free_item(invItem* p);
	void free_menu_item(fncMenuItem* p);
	void clear_menu(fncMenu* p);
	void free_matrix(invMatrix* p);

	void change_mode(void);
	void set_fullscreen(bool isEnabled);
	void inv_mouse_move_quant(void);
	void inv_mouse_imove_quant(void);
	void inv_mouse_quant_l(void);
	void inv_mouse_quant_r(void);

	void aml_check_redraw(int x,int y);

	int put_item(actintItemData* d);
	void put_in_slot(actintItemData* d);
	int put_item_xy(invItem* p,int x,int y,int sflag = 0);
	int put_item_auto(invItem* p);

	void change_items(int x,int y,int sflag = 0);
	void remove_item(actintItemData* d);
	void set_move_item(int index,int sflag = 0);

	void add_menu_item(fncMenu* p);
	void remove_menu_item(fncMenu* p);
	void prepare_menus(void);
	void clean_menus(void);

	void reinit_items(void);
	void deactivate_items(void);
	void trade_items(invMatrix* p,int imode = 0);
	void swap_matrices(void);
	void cancel_matrix(void);

	void save_data(XStream* fh);
	void load_data(XStream* fh);

#ifdef _DEBUG
	void save_items(void);
#endif

	actIntDispatcher(void);
	~actIntDispatcher(void);
};

void aPutStr(int x,int y,int font,int color,unsigned char* str,int bsx,unsigned char* buf,int space = I_STR_SPACE);
void aOutStr(int x,int y,int font,int color,unsigned char* str,int space = I_STR_SPACE);
void aPutStr32(int x,int y,int font,int color,int color_size,void* str,int bsx,void* buf,int space);
void aPutChar(int x,int y,int font,int color,int str,int bsx,int bsy,unsigned char* buf);
void aPutChar32(int x,int y,int font,int color,int color_size,int str,int bsx,void* buf);
void aPutNum(int x,int y,int font,int color,int str,int bsx,unsigned char* buf,int space = I_STR_SPACE);
int aStrLen(unsigned char* str,int font,int space = I_STR_SPACE);
int aStrLen32(void* str,int font,int space = I_STR_SPACE);
int aStrHeight(int font);
int aStrHeight32(int font);

extern int actintLowResFlag;

#ifdef _ACTINT_MEMSTAT_
extern int memQ;
extern XStream aMemStatFile;
#endif

