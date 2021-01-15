#ifndef __ISCREEN_H__
#define __ISCREEN_H__

#include "../lang.h"
#include "hfont.h"
#include "iscreen_options.h"

//#define iMOVE_MOUSE_OBJECTS

const int  iJOYSTICK_MASK		= ~0xFF;

const int  I_PAL_START		= 128;
const int  I_PAL_OFFS		= I_PAL_START * 3;
const int  I_PAL_SIZE		= 255 - I_PAL_START;

const int  I_RES_X			= 800;
const int  I_RES_Y			= 600;

const int  I_MAP_RES_X		= 1 << 11;

const int  I_MEM_HEAP_SIZE		= I_RES_X * I_RES_Y;
const int  I_LEVEL			= 20;
const int  I_UNSELECT_SCALE		= -255;
const int  I_DEFAULT_SPACE		= 5;
const int  I_SHADOW_SIZE		= 20;

const int  iS_STR_COL		= 155;
const int  iS_STR_UNSELECT_COL	= 0;
const int  iS_STR_SEG_START_COL	= 144;

//const int  iS_STR_COL		  = 80;
//const int  iS_STR_UNSELECT_COL	  = 0;
//const int  iS_STR_SEG_START_COL	  = 80;

const int  iSTR_LEN			= 50;
const int  iS_STR_LEN		= 512;

const int  LEFT_MOUSE		= -1;
const int  RIGHT_MOUSE		= -2;

enum EvObjTypes {
	I_ABSENT      = -1,
	I_COMMAND,		// 0
	I_EVENT,		// 1
	I_ELEMENT,		// 2
	I_SCROLLER,		// 3
	I_TERR_ELEMENT, 	// 4
	I_OBJECT,		// 5
	I_TRIGGER,		// 6
	I_SCREEN		// 7
};

const int  I_MAX_SMOOTH_INDEX	= 4;
const int  I_MIN_SMOOTH_INDEX	= 0;

// Directions...
const int 	I_LEFT		= 0;
const int 	I_UP		= 1;
const int 	I_RIGHT 	= 2;
const int 	I_DOWN		= 3;

struct iListElement
{
	std::string ID_ptr;
	int name_len;

	iListElement* prev;
	iListElement* next;

	void init_id(const char* p);

	iListElement(void);
	~iListElement(void);
};

struct iList
{
	int Size;
	int index;
	iListElement* first;
	iListElement* last;

	void connect(iListElement* p);
	void dconnect(iListElement* p);
	void init_list(void){ Size = index = 0; first = last = NULL; }
	void swap(iListElement* p,iListElement* p1);

	iList(void);
};

struct iListElementPtr : public iListElement
{
	iListElement* ptr;
};

const int 	BMP_FLAG		= 0x01;
const int 	BG_FLAG 		= 0x02;
const int 	BML_FRAMES_LOADED	= 0x04;
const int 	BML_NO_OFFSETS		= 0x08;

struct bmlObject : public iListElement
{
	int ID;

	int flags;

	short SizeX;
	short SizeY;
	short Size;

	short OffsX;
	short OffsY;

	char* name;
	unsigned char* frames;

	void init_name(char* p);
	void load(const char* fname = NULL,int bmp = 0);
	void load2mem(unsigned char* ptr,char* fname = NULL);
	void show(int frame = 0);
	void offs_show(int x,int y,int frame = 0);
	void free(void);

	void change_color(int from,int to);

	bmlObject(void);
	~bmlObject(void);
};

struct ibsObject : public iListElement
{
	int ID;

	int PosX;
	int PosY;

	int CenterX;
	int CenterY;

	int bSizeX;
	int bSizeY;

	int SizeX;
	int SizeY;

	int SideX;
	int SideY;

	int fontID;

	char indPosX[4];
	char indPosY[4];

	bmlObject* back;
	int backObjID;

	char* name;
	int ImageSize;
	unsigned char* image;

	void load(char* fname = NULL);
	void show(void);
	void show_bground(void);
	void free(void);

	void set_name(char* p);

	ibsObject(void);
	~ibsObject(void);
};

struct iScanCode : public iListElement
{
	int code;
};

struct iObjID : public iListElement
{
	char* ID;
	int numID;

	iListElement* obj;

	int compare(const char* p);
	void init(const char* p,int num,iListElement* ptr = NULL);

	iObjID(void);
	~iObjID(void);
};

// Event flags...
const int 	EC_NO_OBJECT		= 0x01;
const int 	EV_ACTIVE		= 0x01;
const int 	EV_IF_SELECTED		= 0x02;
const int 	EV_IF_NOT_SELECTED	= 0x04;
const int 	EC_ELEMENT_LEVEL	= 0x08;
const int 	EC_LIST 		= 0x10;
const int 	EV_MULTI_LINE		= 0x20;
const int 	EV_IF_LOCKED		= 0x40;

// Event types...
enum EventTypes
{
	EV_RISE_OBJECT = 1,	// 1
	EV_HIDE_OBJECT, 	// 2
	EV_LOW_OBJECT,		// 3
	EV_RISE_SCREEN, 	// 4
	EV_HIDE_SCREEN, 	// 5
	EV_OBJECT_JUMP, 	// 6
	EV_SCREEN_JUMP, 	// 7
	EV_EXIT,		// 8
	EV_SCALE_OBJECT,	// 9
	EV_SMOOTH_OBJECT,	// 10
	EV_CHANGE_SCROLLER,	// 11
	EV_CHANGE_TRIGGER,	// 12
	EV_CHANGE_AVI,		// 13
	EV_LIST_CHANGE_AVI,	// 14
	EV_NEXT_AVI,		// 15
	EV_PREV_AVI,		// 16
	EV_EXT_SCREEN_JUMP,	// 17
	EV_INPUT_STRING,	// 18

	EV_MOVE_SCREEN, 	// 19
	EV_MOVE2SCREEN, 	// 20

	EV_SEND2ACTINT, 	// 21
	EV_SEND_DATA2ACTINT,	// 22

	EV_SET_TRIGGER, 	// 23

	EV_SET_LINE_ID, 	// 24
	EV_END_EVENT,		// 25

	EV_STOP_AVI,		// 26
	EV_PLAY_AVI,		// 27

	EV_EXIT_EVENT,		// 28
	EV_CHECK_CREDITS,	// 29

	EV_SEND2ML_DISP,	// 30
	EV_ML_EVENT,		// 31

	EV_RESET_TEXTS, 	// 32
	EV_NEXT_TEXT,		// 33
	EV_PREV_TEXT,		// 34

	EV_START_DEMO,		// 35

	EV_EXT_EVENT,		// 36
	EV_LOCK_OBJ,		// 37
	EV_UNLOCK_OBJ,		// 38

	EV_PREV_TRIGGER,	// 39

	EV_MAX_EVENT
};

struct iScreenEventCommand : public iListElement
{
	short objID;
	char* tmp_objID;
	short objType;

	iListElement* obj;

	short EvType;
	short flags;
	short value;

	short data0;
	short data1;

	char* pvalue;

	short time;
	short start_time;

	int LineID;

	void copy(iScreenEventCommand* dest);

	void init_objID(const char* ptr);
	void init_pvalue(char* ptr);

	iScreenEventCommand(void);
	~iScreenEventCommand(void);
};

struct iScreenEvent : public iListElement
{
	iList* codes;		// Scancodes...
	short flags;
	short time;
	int cur_time;

	iList* CommSeq;

	void copy(iScreenEvent* dest);
	void add_scancode(int cd);
	void add_command(iScreenEventCommand* cm){ CommSeq -> connect((iListElement*)cm); };

	iScreenEvent(void);
	~iScreenEvent(void);
};

enum AlignTypes
{
	ALIGN_CENTER = 1,

	ALIGN_L,
	ALIGN_U,
	ALIGN_R,
	ALIGN_D
};

// Types of iScreenElement..
enum iElementTypes
{
	I_STRING_ELEM,
	I_BITMAP_ELEM,
	I_SCROLLER_ELEM,
	I_TERRAIN_ELEM,
	I_S_STRING_ELEM,
	I_AVI_ELEM,
	I_AVI_BORDER_ELEM
};

const int 	EL_DATA_LOADED	= 0x01;
const int 	EL_TEXT_STRING	= 0x02;
const int 	EL_NO_REDRAW	= 0x04;
const int 	EL_HIDE 	= 0x08;
const int 	EL_PICTURE	= 0x10;
const int 	EL_BORDER_INIT	= 0x20;
const int 	EL_PAL_CHANGE	= 0x40;
const int 	EL_SCALE_NULL	= 0x80;
const int 	AVI_STOPPED	= 0x100;
const int 	EL_HIDDEN_FON	= 0x200;
const int 	EL_NO_SCALE	= 0x400;
const int 	EL_NONE_BMP	= 0x800;
const int 	EL_KEY_NAME	= 0x1000;
const int 	EL_NUMBER	= 0x2000;
const int 	EL_JOYSTICK_KEY = 0x4000;

struct iScreenElement : public iListElement
{
	short ID;
	short type;

	short lX;		// Local
	short lY;		// coords...

	short nlX;
	short nlY;

	short SizeX;
	short SizeY;

	int scale;
	int scale_delta;

	int terrainNum;

	int align_x;
	int align_y;
	int align_x_offs;
	int align_y_offs;

	int flags;

	int null_level;

	iListElement* owner;

	void load_data(void);
	void free_data(void);
	void redraw(int x,int y,int sc,int sm,int hide_mode);

	void scale_quant(void);
	int check_xy(int x,int y);

	void copy(iScreenElement* dest);

	void init_align(void);

	void change_terrainNum(int terr);

	iScreenElement(void);
	~iScreenElement(void);
};

const int 	AVI_DEF_BORDER_SIZE	= 5;

// Border types...
const int 	AVI_FLAT_BORDER 	= 0x00;
const int 	AVI_SHAPED_BORDER	= 0x01;

struct iAVIElement : public iScreenElement
{
	char* avi_name;
	void* data;

	iList* avi_ids;

	int border_type;
	int border_level;
	int border_null_level;
	int border_size;

	char* border_shape_file;
	char* border_shape;

	ibsObject* ibs;

	unsigned char* palBuf;

	short ShSizeX;
	short ShSizeY;

	void load(void);
	void load_shape(void);
	void free_shape(void);
	void free(void);
	void change_avi(char* fname);
	void init_size(void);

	int check_visible(void);

	void init_name(char* p);
	void add_name(const char* ptr);

	void next_avi(void);
	void prev_avi(void);

	void free_mem(void);

	iAVIElement(void);
};

struct iAVIBorderElement : public iScreenElement
{
	int border_type;
	int border_level;
	int border_null_level;
	int border_size;
	int dir;

	char* border_shape_file;
	char* shape;

	int ShapeX;
	int ShapeY;

	void free_mem(void);

	iAVIBorderElement(void);
};

struct iStringElement : public iScreenElement
{
	int font;
	int space;
	char* string;

	void init_string(const char* p);
	void init_size(void);
	void free_mem(void);

	iStringElement(void);
};

struct iS_StringElement : public iScreenElement
{
	int font;
	int space;
	char* string;

	void init_string(char* p);
	void init_size(void);
	void free_mem(void);

	iS_StringElement(void);
};

struct iBitmapElement : public iScreenElement
{
	short bSizeX;
	short bSizeY;

	int bmp_null_level;

	iList* bmp_names;
	iList* pal_names;

	ibsObject* ibs;

	char* fname;
	char* palname;

	char* fdata;

	char* border_shape_file;
	char* border_shape;

	short ShSizeX;
	short ShSizeY;

	void load_shape(void);
	void free_shape(void);

	void init_name(const char* ptr);
	void init_palname(char* ptr);

	void alloc_mem(void);

	void free(void);
	void load(void);
	void set_pal(void);
	void init_size(void);

	void free_mem(void);

	iBitmapElement(void);
};

struct iScrollerElement : public iBitmapElement
{
	int dir;

	int Value;
	int maxValue;
	int prevValue;

	int scale;
	int scale_delta;

	int space;

	void change_quant(void);
	void change_val(int x,int y);
	void scroller_init(void);

	iScrollerElement(void);
};

struct iTerrainElement : public iScreenElement
{
	int scale_strength;

	int def_null_level;
	int level_delta;

	int pX;
	int pY;

	void level_quant(void);

	void free_mem(void);

	iTerrainElement(void);
};

// Object flags...
const int 	OBJ_SELECTED		= 0x08;
const int 	OBJ_SELECTABLE		= 0x02;
const int 	OBJ_AUTO_SIZE		= 0x04;
const int 	OBJ_HIDE		= 0x01;
const int 	OBJ_REDRAW		= 0x10;
const int 	OBJ_TERRAIN		= 0x20;
const int 	OBJ_TRIGGER		= 0x40;
const int 	TRG_MEM_ALLOC		= 0x80;
const int 	OBJ_AVI_PRESENT 	= 0x100;
const int 	OBJ_AVI_BORDER		= 0x200;
const int 	AVI_BORDER_INIT 	= 0x400;
const int 	OBJ_ALIGN		= 0x800;
const int 	OBJ_REINIT		= 0x1000;
const int 	OBJ_PIC_PRESENT 	= 0x2000;
const int 	OBJ_FON_DATA		= 0x4000;
const int 	OBJ_SET_COORDS		= 0x8000;
const int 	OBJ_MUST_REDRAW 	= 0x10000;
const int 	OBJ_AVI_STOPPED 	= 0x20000;
const int 	OBJ_CLEAR_FON		= 0x40000;
const int 	OBJ_STORE_TERRAIN	= 0x80000;
const int 	OBJ_LOCKED		= 0x100000;
const int 	OBJ_NOT_UNLOCK		= 0x200000;
const int 	OBJ_NO_FON		= 0x400000;

struct iScreenObject : public iListElement
{
	short ID;

	int PosX;
	int PosY;

	int nPosX;
	int nPosY;

	int SizeX;
	int SizeY;

	int ShadowSize;

	int FlushX;
	int FlushY;
	int FlushSX;
	int FlushSY;

	int align_x;
	int align_y;
	int align_x_offs;
	int align_y_offs;

	int flags;

	iList* EventList;
	iList* ElementList;
	iListElement* owner;

	unsigned char* fonData;
	unsigned char* terr_fonData;

	int promptID;
	char* promptData;

	int curHeightScale;
	int HeightScaleDelta;

	int SmoothLevel;
	int SmoothDelta;
	int SmoothTime;
	int SmoothCount;

	void redraw(int mode = 0);
	void flush(void);
	void ScaleQuant(void);
	void SmoothQuant(void);

	void getfon(int del = 1);
	void putfon(void);
	void free_fon(void);

	void add_element(iListElement* p);
	void remove_element(iListElement* p);
	void add_event(iListElement* p);

	void load_data(void);
	void free_data(void);

	void copy(iScreenObject* dest);

	void init(void);
	int CheckXY(int x,int y);

	iScreenObject(void);
	~iScreenObject(void);
};

struct iTriggerObject : public iScreenObject
{
	int num_state;
	int state;
	int prev_state;
	void (*callback)(int);

	iListElement** StateElem;

	void change_state(int time,int st = -1);
	void init_state(void);

	void trigger_init(void);
	void alloc_state(void);

	iTriggerObject(void);
};


// iScreen flags...
#define iSCR_BLOCK_GLOBAL_OBJ		0x01

struct iScreen : public iListElement
{
	short ID;

	iScreenObject* curObj;
	iScreenObject* dcurObj;
	char* t_objID;

	int flags;
	int ScreenOffs;

	iList* objList;
	iList* EventList;
	iList* TerrainList;
	iList* ObjIDs;

	char* pal_name;
	char* palette_buf;

	XGR_MousePromptScreen* promptData;

	void CheckScanCode(int sc);
	void HandleEvent(iScreenEvent* ev);
	void redraw(int mode = 0);
	void prepare(void);
	void init(void);
	void init_prompt(void);
	void init_ids(void);
	void init_avi_border(void);

	void load_palette(void);
	void free_palette(void);

	void show_avi(void);
	void hide_avi(void);
	void hide_avi_place(void);

	void load_data(void);
	void free_data(void);
	void getfon(void);

	void prepare_avi(void);
	void close_avi(void);

	void copy(iScreen* dest);
	void add_object(iScreenObject* p);
	void add_terr_object(iScreenObject* p);
	void add_event(iListElement* p);

	iListElement* get_object(const char* p);
	int check_id(char* p);

	iScreen(void);
	~iScreen(void);
};

#define iTEXT_EOF		0x01
#define iTEXT_DATA_LOADED	0x02
struct iTextData : public iListElement
{
	int ID;
	char* fname;
	char* objName;

	int flags;

	int curLine;
	int numLines;
	char** data;

	int heap_size;
	char* mem_heap;

	void load(void);
	void free(void);

	void copy(iScreenObject* p);
	void read_str(char* p);
	void reset(void);

	int eof(void){ return (flags & iTEXT_EOF); }

	iTextData(void);
	~iTextData(void);
};

const int 	SD_PAL_DELTA	= 10;

// Screen dispatcher flags...
const int 	SD_EXIT 	= 0x01;
const int 	MS_LEFT_PRESS	= 0x02;
const int 	MS_RIGHT_PRESS	= 0x04;
const int 	MS_MOVED	= 0x08;
const int 	SD_INPUT_STRING = 0x10;
const int 	SD_FINISH_INPUT = 0x20;
const int 	SD_PAL_CHANGE	= 0x40;
const int 	SD_GETFON	= 0x80;
const int 	SD_END_GETFON	= 0x100;
const int 	SD_LOCK_EVENTS	= 0x200;

struct iScreenDispatcher : public iList
{
	int flags;
	iScreen* curScr;
	char* t_scrID;

	unsigned char* curPal;
	unsigned char* trgPal;

	int ret_val;
	int cur_max_input;

	iScreenEvent* ActiveEv;
	iScreenElement* ActiveEl;
	iList* ObjHeap;

	iList* drwObjHeap;
	iList* ScreenIDs;
	iList* ExtScreens;

	iTextData* curText;
	iList* texts;

	void init_texts(void);
	void finit_texts(void);
	void next_text(void);
	void prev_text(void);
	int copy_text_next(iScreen* p,int mode);
	int copy_text_prev(iScreen* p,int mode);

	iTextData* get_text(int id);

	void key_trap(int sc);
	void CheckScanCode(int sc);
	void HandleEvent(iScreenEvent* ev);
	void EventQuant(void);
	void ProcessEvent(iScreenEvent* p);
	void end_event(void);

	void input_string_quant(void);
	void init_input_string(iScreenElement* p);

	void prepare(void);
	void redraw(int mode = 0);
	void redraw_quant(void);
	void flush_quant(void);
	void set_scr_redraw(iScreen* p);
	void set_obj_redraw(iScreenObject* p);

	void hide_screen(iScreen* p);
	void show_screen(iScreen* p);

	void alloc_mem(void);
	void free_mem(void);

	int check_id(char* p);
	iListElement* get_object(const char* p);

	void move_screen(int val,int time);
	void aci_move_screen(int val,int time);
	void move2screen(iScreen* p,int time);

	void lock_events(void);
	void unlock_events(void);

	void add_ext_screen(iScreen* p);
	void remove_ext_screen(iScreen* p);
	iScreen* get_ext_screen(const char* p);

	void pal_quant(void);

	void save_data(XStream* fh);
	void load_data(XStream* fh);

	iScreenDispatcher(void);
};

#define I_STR_SPACE		2
#define I_FONT_SIZE		1

struct iScreenFont
{
	int SizeX;
	int SizeY;

	char* data;
	char* LeftOffs;
	char* RightOffs;

	iScreenFont(char* p,int sz);
};



#define iOPTION_VALUE_CUR	0
#define iOPTION_VALUE_MAX	1

// iScreenOption flags...
#define iOPTION_NO_SAVE 	0x01

struct iScreenOption
{
	int flags;

	int ObjectType;
	int ValueType;

	iListElement* objPtr;

	XList* pLinks;

	int GetValueINT(void);
	void SetValueINT(int val);

	char* GetValueCHR(void);
	void SetValueCHR(const char* p);

	void save(XStream* fh);
	void load(XStream* fh);

	void update(void);

	void add_link(iScreenOption* p);

	iScreenOption(int tp,int vtp,const char* scr,const char* obj);
};

struct iScreenOptionPtr : XListElement
{
	iScreenOption* optionPtr;
};

void iPutS_Str(int x,int y,int font,int color,unsigned char* str,int bsx,int bsy,unsigned char* buf,int space = I_STR_SPACE);
int iS_StrLen(unsigned char* str,int font,int space = I_STR_SPACE);


iListElement* iGetOptionObj(int id);

void i_preExtQuant(void);
void i_postExtQuant(void);

void ParseScript(const char* fname,const char* bname = NULL);
void iInit(void);
int iQuant(void);

iScreen* iCreateScreen(const char* id,iScreen* tpl = NULL);
iScreenObject* iCreateScreenObject(const char* id,iScreenObject* tpl = NULL);
iTriggerObject* iCreateTriggerObject(const char* id,iScreenObject* tpl = NULL);
iScreenElement* iCreateScreenElement(int tp,const char* id = NULL,iScreenElement* tpl = NULL);
iScreenEvent* iCreateScreenEvent(iScreenEvent* tpl = NULL);
iScreenEventCommand* iCreateEventCommand(iScreenEventCommand* tpl = NULL);
void iInitText(iScreenObject* obj,char* text,int text_len,int font,int space,int null_level = I_LEVEL);
void iInitS_Text(iScreenObject* obj,char* text,int text_len,int font,int space,int null_level = I_LEVEL);

extern int iScreenOffs;
extern int iScreenOffsDelta;
extern int iScreenTargetOffs;

extern const char* iVideoPath;
extern const char* iVideoPathDefault;

//#define SCREENSHOT

extern int logShot;
void shotMake(int pal);

extern HFont** HFntTable;
extern int iNumFonts;

extern iScreenFont** iScrFontTable;
extern iScreenEventCommand* CurEvComm;
extern int EventFlag;
extern int iAbortGameFlag;
extern int iAbortGameMode;
extern iScreenDispatcher* iScrDisp;

#define iScrExitCode	iScrDisp -> ret_val

void iInit(void);
void iQuantFirst(void);
void iQuantPrepare(void);
int iQuantSecond(void);
void iFinitQuant(void);
void i_change_res(void);

void aciShowLocation(void);

void iOutEscaveInit(void);
int iOutEscaveQuant(void);
void iOutEscaveFinit(void);

void iKeyClear(void);

void iHandleExtEvent(int code,int data = 0);

const char* iGetKeyNameText(int vkey, Language lang = ENGLISH, bool scan = false);
const char* iGetJoyBtnNameText(int vkey, Language lang = ENGLISH);

void i_slake_pal(unsigned char* p,int delta);
int iGetKeyName(int vkey,int shift,int lng_flag);

#endif
