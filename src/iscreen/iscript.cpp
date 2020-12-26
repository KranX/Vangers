/* ---------------------------- INCLUDE SECTION ----------------------------- */

#include "../global.h"

#include "hfont.h"
#include "iscreen.h"
#include "s_mem.h"

#include "iscript.h"

/* ----------------------------- STRUCT SECTION ----------------------------- */

struct iStackElement
{
	int objType;
	iListElement* obj;
};

struct iStack
{
	int curSize;
	char* mem_heap;
	iStackElement** heap;

	void init(void);
	void add(iListElement* p,int tp);
	iStackElement* get(void);

	iStack(void);
	~iStack(void);
};

/* ----------------------------- EXTERN SECTION ----------------------------- */

extern iScreenDispatcher* iScrDisp;
extern ScriptFile* script;

/* --------------------------- PROTOTYPE SECTION ---------------------------- */

char* aciLoadPackedFile(XStream& fh,int& sz);
void get_buf(int x,int y,int sx,int sy,unsigned char* buf);

int GetOptionID(char* p);

void addObjID(const char* p,iListElement* obj);
int getObjID(const char* p);

void handle_error(const char* prefix,const char* subj = NULL);

void link_object(void);
void new_object(int tp);
void new_element(void);
void init_command(int code);
void build_links(void);
void init_objects(void);

void load_text(char* fname);
void load_s_text(char* fname);

int aStrLen32(void* p,int f,int space);
int aStrHeight32(void* p,int f);
void init_hfonts(void);
void free_hfonts(void);

/* --------------------------- DEFINITION SECTION --------------------------- */

#define ABORT(str)	ErrH.Abort(str)
#define I_STACK_SIZE	16
#define I_MAX_ID	2048

enum Options
{
	// Common section...
	I_END_BLOCK,
	I_END_STRING,

	I_NEW_COMMAND,
	I_NEW_EVENT,
	I_NEW_ELEMENT,
	I_NEW_OBJECT,
	I_NEW_TRIGGER_OBJECT,
	I_NEW_SCREEN,
	I_SET_NUM_FONTS,

	// Event & EventCommand section...
	EV_CODE,
	EV_KEY,
	E_TIME,
	EC_START_TIME,
	EC_RISE_OBJ,
	EC_HIDE_OBJ,
	EC_LOW_OBJ,
	EC_RISE_SCR,
	EC_HIDE_SCR,
	EC_JUMP_OBJ,
	EC_JUMP_SCR,
	EC_EXIT,
	EC_SCALE_OBJ,
	EC_SMOOTH_OBJ,
	EC_SCROLLER_CHANGE,
	EC_TRIGGER_CHANGE,
	EC_AVI_CHANGE,
	EC_AVI_LIST_CHANGE,
	EC_NEXT_AVI,
	EC_PREV_AVI,
	EC_EXT_SCREEN_JUMP,
	EC_INPUT_STRING,
	EC_MOVE_SCREEN,
	EC_MOVE2SCREEN,
	EC_SEND2ACTINT,
	EC_SENDDATA2ACTINT,
	EC_SET_TRIGGER,
	EC_SET_LINE_ID,
	EC_END_EVENT,
	EC_STOP_AVI,
	EC_PLAY_AVI,

	EC_EXIT_EVENT,
	EC_CHECK_CREDITS,

	EC_SEND2ML_DISP,
	EC_ML_EVENT,

	EC_RESET_TEXTS,
	EC_NEXT_TEXT,
	EC_PREV_TEXT,

	EC_START_DEMO,
	EC_EXT_EVENT,
	EC_LOCK_OBJ,
	EC_UNLOCK_OBJ,

	EC_PREV_TRIGGER,

	EV_SELECTED,
	EV_NOT_SELECTED,
	EV_LOCKED,
	EV_SET_MULTILINE,
	EC_LINE_ID,

	// iScreenObject & iScreenObjectElement section...
	OBJ_INIT_X,
	OBJ_INIT_Y,
	OBJ_INIT_SX,
	OBJ_INIT_SY,
	OBJ_SET_SELECT,
	OBJ_SET_UNLOCK,
	OBJ_SET_SHADOW_SIZE,
	OBJ_SET_PROMPT_TEXT,
	OBJ_SET_PROMPT_ID,
	OBJ_SET_NO_FON,

	TRIG_NUM_STATE,
	TRIG_NEW_ELEMENT,

	EL_STRING,
	EL_BITMAP,
	EL_SCROLLER,
	EL_TERRAIN,
	EL_S_STRING,
	EL_AVI,
	EL_SET_ID,
	EL_INIT_LEVEL,
	EL_INIT_BMP_LEVEL,
	EL_INIT_FONT,
	EL_INIT_SPACE,
	EL_INIT_FNAME,
	EL_INIT_PALNAME,
	EL_INIT_AVINAME,
	EL_AVI_BORDER_TYPE,
	EL_AVI_BORDER_SIZE,
	EL_AVI_BORDER_LEVEL,
	EL_AVI_BORDER_NULL_LEVEL,
	EL_AVI_BORDER_NAME,
	EL_AVI_IBS_NAME,
	EL_AVI_FLAT_TYPE,
	EL_AVI_SHAPED_TYPE,
	EL_INIT_STRING,
	EL_INIT_TEXT,
	EL_INIT_S_TEXT,
	EL_SET_ALIGN_X,
	EL_SET_ALIGN_Y,
	EL_SET_ALIGN_OFFS_X,
	EL_SET_ALIGN_OFFS_Y,
	EL_INIT_DIR,
	EL_INIT_VALUE,
	EL_INIT_MAX_VALUE,
	EL_INIT_SCALE,
	EL_PICTURE_INIT,
	EL_SET_HIDDEN_FON,
	EL_SET_NO_SCALE,

	EL_ALIGN_CENTER,

	EL_ALIGN_L,
	EL_ALIGN_U,
	EL_ALIGN_R,
	EL_ALIGN_D,

	EL_SET_SCALE_NULL,
	EL_SET_TERRAIN,
	EL_SET_KEY_FLAG,
	EL_SET_NUMBER_FLAG,
	EL_SET_JOYSTICK_FLAG,

	// iScreen section...
	SCR_SET_DEFAULT_OBJ,
	SCR_SET_OFFS,
	SCR_SET_BLOCK_FLAG,

	// iScreenDispatcher section...
	SD_SET_DEFAULT_SCR,

	SD_NEW_TEXT,

	MAX_OPTION
};

static const char* OptIDs[MAX_OPTION] = {
	"}",
	"\n",

	"EvComm",
	"Event",
	"Element",
	"Object",
	"Trigger",
	"Screen",
	"num_fonts",

	"scancode",
	"key",
	"time",
	"start_time",
	"rise_obj",
	"hide_obj",
	"low_obj",
	"rise_scr",
	"hide_scr",
	"jump_obj",
	"jump_scr",
	"exit",
	"scale_obj",
	"smooth_obj",
	"change_scroller",
	"change_trigger",
	"change_avi",
	"list_change_avi",
	"next_avi",
	"prev_avi",
	"jump_ext_scr",
	"input_string",
	"move_screen",
	"move2screen",
	"send2actint",
	"send_aci_data",
	"set_trigger",
	"set_event_line",
	"end_event",
	"stop_avi",
	"play_avi",

	"exit_event",
	"check_credits",

	"send2mldisp",
	"ml_event",

	"reset_texts",
	"next_text",
	"prev_text",

	"start_demo",
	"ext_event",
	"lock_obj",
	"unlock_obj",

	"prev_trigger",

	"if_selected",
	"if_not_selected",
	"if_locked",
	"multi_event",
	"event_line",

	"x",
	"y",
	"size_x",
	"size_y",
	"selectable_obj",
	"not_unlock",
	"shadow_size",
	"prompt_text",
	"prompt_id",
	"no_fon",

	"num_state",
	"State",

	"STRING",
	"BITMAP",
	"SCROLLER",
	"ScaledTerrain",
	"S_STRING",
	"AVI",
	"id",
	"null_level",
	"bmp_null_level",
	"font",
	"space",
	"bmp_file",
	"pal_file",
	"avi_file",
	"border_type",
	"border_size",
	"border_level",
	"border_null_level",
	"shape_file",
	"ibs_file",
	"FLAT",
	"SHAPED",
	"string",
	"text",
	"s_text",
	"align_x",
	"align_y",
	"align_x_offs",
	"align_y_offs",
	"dir",
	"value",
	"max_value",
	"scale",
	"picture",
	"hidden_fon",
	"no_scale",

	"center",

	"left",
	"up",
	"right",
	"down",

	"scale_null",
	"terrain_num",
	"key_text",
	"number_text",
	"joystick_text",

	"default_obj",
	"screen_offs",
	"block_global_obj",

	"default_scr",

	"text_object"
};

iStack* iStk;

iScreenEventCommand* iEvComm;
iScreenEvent* iEv;
iScreenElement* iElem;
iStringElement* iElStr;
iS_StringElement* iElS_Str;
iAVIElement* iElAvi;
iBitmapElement* iElBmp;
iTerrainElement* iElTerr;
iScrollerElement* iElScr;
iTriggerObject* iTrig;
iScreenObject* iScrObj;
iScreen* iScr;

iListElement** ObjTable;
int ObjIDSize = 0;

iTextData* text_obj;

iList* curID_List;

int DefFont = 0;
int DefSpace = I_DEFAULT_SPACE;
int DefLevel = I_LEVEL;
int iCurObjType = I_ABSENT;
int iCurID = 0;

iStack::iStack(void)
{
	int i;
	heap = new iStackElement*[I_STACK_SIZE];
	mem_heap = new char[I_STACK_SIZE * sizeof(iStackElement)];

	for(i = 0; i < I_STACK_SIZE; i ++){
		heap[i] = (iStackElement*)(mem_heap + sizeof(iStackElement) * i);
	}
	curSize = 0;
}

iStack::~iStack(void)
{
	delete[] heap;
	delete[] mem_heap;
	curSize = 0;
}

void iStack::add(iListElement* p,int tp)
{
	heap[curSize] -> obj = p;
	heap[curSize] -> objType = tp;

	curSize ++;
	if(curSize >= I_STACK_SIZE)
		ErrH.Abort("iStack overflow...");
}

iStackElement* iStack::get(void)
{
	if(!curSize)
		return NULL;
	curSize --;
	return heap[curSize];
}

void ParseScript(const char* fname,char* bname)
{
	int id,t_id = 0,st = 0;

	iScanCode* cd;
	iScrDisp = new iScreenDispatcher;

	iStk = new iStack;

	init_hfonts();
	curID_List = iScrDisp -> ScreenIDs;
	ObjTable = new iListElement*[I_MAX_ID];

#ifndef _BINARY_SCRIPT_
	_sALLOC_HEAP_(iSCRIPT_HEAP_SIZE,char);
#endif
	script = new ScriptFile;
#ifndef _BINARY_SCRIPT_
	script -> set_bscript_name(bname);
#endif
	script -> load(fname);
	script -> prepare();

	addObjID("SELECTED_OBJ",(iListElement*)NULL);
	addObjID("TERRAIN_OBJ",(iListElement*)NULL);

	while(!script -> EOF_Flag){

#ifndef _BINARY_SCRIPT_
		if(script -> curBlock && *script -> curBlock -> data){
#endif
			id = script -> read_option();

#ifndef _BINARY_SCRIPT_
			if(id == -1)
				handle_error("Unknown keyword",script -> prevStrBuf);
#else
			if(id == iSCRIPT_EOF) script -> EOF_Flag = 1;
			if(id == -1)
				handle_error("Unknown keyword");
#endif

			switch(id){
				case EL_SET_SCALE_NULL:
					if(iCurObjType == I_ELEMENT){
						iElem -> flags |= EL_SCALE_NULL;
					}
					else
						handle_error("Misplaced option",OptIDs[id]);
					break;
				case EL_SET_TERRAIN:
					if(iCurObjType == I_ELEMENT){
						iElem -> terrainNum = script -> read_idata();
					}
					else
						handle_error("Misplaced option",OptIDs[id]);
					break;
				case EL_SET_KEY_FLAG:
					if(iCurObjType == I_ELEMENT){
						iElem -> flags |= EL_KEY_NAME;
					}
					else
						handle_error("Misplaced option",OptIDs[id]);
					break;
				case EL_SET_NUMBER_FLAG:
					if(iCurObjType == I_ELEMENT){
						iElem -> flags |= EL_NUMBER;
					}
					else
						handle_error("Misplaced option",OptIDs[id]);
					break;
				case EL_SET_JOYSTICK_FLAG:
					if(iCurObjType == I_ELEMENT){
						iElem -> flags |= EL_JOYSTICK_KEY;
					}
					else
						handle_error("Misplaced option",OptIDs[id]);
					break;
				case SD_NEW_TEXT:
					text_obj = new iTextData;
					text_obj -> ID = script -> read_idata();
					script -> read_pdata(&text_obj -> objName,1);
					script -> read_pdata(&text_obj -> fname,1);
					iScrDisp -> texts -> connect((iListElement*)text_obj);
					break;
				case EL_SET_HIDDEN_FON:
					if(iCurObjType == I_ELEMENT){
						iElem -> flags |= EL_HIDDEN_FON;
					}
					break;
				case EL_SET_NO_SCALE:
					if(iCurObjType == I_ELEMENT){
						iElem -> flags |= EL_NO_SCALE;
					}
					break;
				case I_END_BLOCK:
					link_object();
					break;
				case I_NEW_COMMAND:
					new_object(I_COMMAND);

					t_id = script -> read_option();

					init_command(t_id);
					break;
				case I_SET_NUM_FONTS:
					if(HFntTable)
						free_hfonts();

					iNumFonts = script -> read_idata();

					init_hfonts();
					break;
				case I_NEW_EVENT:
					new_object(I_EVENT);
					break;
				case I_NEW_ELEMENT:
					new_object(I_ELEMENT);
					break;
				case I_NEW_OBJECT:
					new_object(I_OBJECT);
					script -> prepare_pdata();
					addObjID(script -> get_conv_ptr(),(iListElement*)iScrObj);
					break;
				case I_NEW_TRIGGER_OBJECT:
					new_object(I_TRIGGER);
					script -> prepare_pdata();
					addObjID(script -> get_conv_ptr(),(iListElement*)iScrObj);
					break;
				case EL_TERRAIN:
					new_object(I_TERR_ELEMENT);
					break;
				case I_NEW_SCREEN:
					new_object(I_SCREEN);
					script -> prepare_pdata();
					addObjID(script -> get_conv_ptr(),(iListElement*)iScr);
					curID_List = iScr -> ObjIDs;
					break;
				case OBJ_INIT_X:
					if(iCurObjType == I_OBJECT){
						iScrObj -> PosX = script -> read_idata();
					}
					else {
						if(iCurObjType == I_ELEMENT || iCurObjType == I_TERR_ELEMENT){
							iElem -> lX = script -> read_idata();
						}
						else
							handle_error("Misplaced option",OptIDs[id]);
					}
					break;
				case OBJ_INIT_Y:
					if(iCurObjType == I_OBJECT){
						iScrObj -> PosY = script -> read_idata();
					}
					else {
						if(iCurObjType == I_ELEMENT || iCurObjType == I_TERR_ELEMENT){
							iElem -> lY = script -> read_idata();
						}
						else
							handle_error("Misplaced option",OptIDs[id]);
					}
					break;
				case OBJ_INIT_SX:
					if(iCurObjType == I_OBJECT){
						iScrObj -> SizeX = script -> read_idata();
					}
					else {
						if(iCurObjType == I_ELEMENT || iCurObjType == I_TERR_ELEMENT){
							iElem -> SizeX = script -> read_idata();
						}
						else
							handle_error("Misplaced option",OptIDs[id]);
					}
					break;
				case OBJ_INIT_SY:
					if(iCurObjType == I_OBJECT){
						iScrObj -> SizeY = script -> read_idata();
					}
					else {
						if(iCurObjType == I_ELEMENT || iCurObjType == I_TERR_ELEMENT){
							iElem -> SizeY = script -> read_idata();
						}
						else
							handle_error("Misplaced option",OptIDs[id]);
					}
					break;
				case OBJ_SET_SELECT:
					if(iCurObjType == I_OBJECT){
						iScrObj -> flags |= OBJ_SELECTABLE;
					}
					else
						handle_error("Misplaced option",OptIDs[id]);
					break;
				case OBJ_SET_UNLOCK:
					if(iCurObjType == I_OBJECT){
						iScrObj -> flags |= OBJ_NOT_UNLOCK;
					}
					else
						handle_error("Misplaced option",OptIDs[id]);
					break;
				case OBJ_SET_SHADOW_SIZE:
					if(iCurObjType == I_OBJECT){
						iScrObj -> ShadowSize = script -> read_idata();
					}
					else
						handle_error("Misplaced option",OptIDs[id]);
					break;
				case OBJ_SET_PROMPT_TEXT:
					if(iCurObjType == I_OBJECT){
						script -> read_pdata(&iScrObj -> promptData,1);
					}
					else
						handle_error("Misplaced option",OptIDs[id]);
					break;
				case OBJ_SET_PROMPT_ID:
					if(iCurObjType == I_OBJECT){
						iScrObj -> promptID = script -> read_idata();
					}
					else
						handle_error("Misplaced option",OptIDs[id]);
					break;
				case OBJ_SET_NO_FON:
					if(iCurObjType == I_OBJECT){
						iScrObj -> flags |= OBJ_NO_FON;
					}
					else
						handle_error("Misplaced option",OptIDs[id]);
					break;
				case EL_SET_ID:
					if(iCurObjType == I_ELEMENT || iCurObjType == I_TERR_ELEMENT){
						iElem -> ID = ObjIDSize;
						script -> prepare_pdata();
						addObjID(script -> get_conv_ptr(),(iListElement*)iElem);
					}
					else
						handle_error("Misplaced option",OptIDs[id]);
					break;
				case EL_INIT_FNAME:
					if(iCurObjType == I_ELEMENT){
						if(iElem -> type == I_BITMAP_ELEM){
							script -> prepare_pdata();
							iElBmp -> init_name(script -> get_conv_ptr());
						}
						else {
							if(iElem -> type == I_SCROLLER_ELEM){
								script -> prepare_pdata();
								iElScr -> init_name(script -> get_conv_ptr());
							}
							else
								handle_error("Misplaced option",OptIDs[id]);
						}
					}
					else
						handle_error("Misplaced option",OptIDs[id]);
					break;
				case EL_INIT_PALNAME:
					if(iCurObjType == I_ELEMENT){
						if(iElem -> type == I_BITMAP_ELEM){
							script -> prepare_pdata();
							iElBmp -> init_palname(script -> get_conv_ptr());
						}
						else
							handle_error("Misplaced option",OptIDs[id]);
					}
					else {
						if(iCurObjType == I_SCREEN){
							script -> read_pdata(&iScr -> pal_name,1);
						}
						else
							handle_error("Misplaced option",OptIDs[id]);
					}
					break;
				case EL_PICTURE_INIT:
					if(iCurObjType == I_ELEMENT){
						if(iElem -> type == I_BITMAP_ELEM)
							iElBmp -> flags |= EL_PICTURE;
						else
							handle_error("Misplaced option",OptIDs[id]);
					}
					else
						handle_error("Misplaced option",OptIDs[id]);
					break;
				case EL_INIT_AVINAME:
					if(iCurObjType == I_ELEMENT){
						script -> prepare_pdata();
						if(iElem -> type == I_AVI_ELEM){
							iElAvi -> add_name(script -> get_conv_ptr());
						}
						else
							handle_error("Misplaced option",OptIDs[id]);
					}
					else
						handle_error("Misplaced option",OptIDs[id]);
					break;
				case EL_AVI_BORDER_NAME:
					if(iCurObjType == I_ELEMENT){
						script -> prepare_pdata();
						if(iElem -> type == I_AVI_ELEM){
							script -> get_pdata(&iElAvi -> border_shape_file,1);
						}
						else {
							if(iElem -> type == I_BITMAP_ELEM){
								script -> get_pdata(&iElBmp -> border_shape_file,1);
							}
							else
								handle_error("Misplaced option",OptIDs[id]);
						}
					}
					else
						handle_error("Misplaced option",OptIDs[id]);
					break;
				case EL_AVI_IBS_NAME:
					if(iCurObjType == I_ELEMENT){
						if(iElem -> type == I_AVI_ELEM){
							script -> prepare_pdata();
							iElAvi -> ibs = new ibsObject;
							iElAvi -> ibs -> set_name(script -> get_conv_ptr());
						}
						else
							handle_error("Misplaced option",OptIDs[id]);
					}
					else
						handle_error("Misplaced option",OptIDs[id]);
					break;
				case EL_AVI_BORDER_TYPE:
					if(iCurObjType == I_ELEMENT){
						if(iElem -> type == I_AVI_ELEM){
							iElAvi -> border_type = script -> read_option() - EL_AVI_FLAT_TYPE;
						}
						else
							handle_error("Misplaced option",OptIDs[id]);
					}
					else
						handle_error("Misplaced option",OptIDs[id]);
					break;
				case EL_AVI_BORDER_SIZE:
					if(iCurObjType == I_ELEMENT){
						if(iElem -> type == I_AVI_ELEM){
							iElAvi -> border_size = script -> read_idata();
						}
						else
							handle_error("Misplaced option",OptIDs[id]);
					}
					else
						handle_error("Misplaced option",OptIDs[id]);
					break;
				case EL_AVI_BORDER_LEVEL:
					if(iCurObjType == I_ELEMENT){
						if(iElem -> type == I_AVI_ELEM){
							iElAvi -> border_level = script -> read_idata();
						}
						else
							handle_error("Misplaced option",OptIDs[id]);
					}
					else
						handle_error("Misplaced option",OptIDs[id]);
					break;
				case EL_AVI_BORDER_NULL_LEVEL:
					if(iCurObjType == I_ELEMENT){
						if(iElem -> type == I_AVI_ELEM){
							iElAvi -> border_null_level = script -> read_idata();
						}
						else
							handle_error("Misplaced option",OptIDs[id]);
					}
					else
						handle_error("Misplaced option",OptIDs[id]);
					break;
				case EL_INIT_SCALE:
					if(iCurObjType == I_TERR_ELEMENT){
						iElTerr -> scale_strength = script -> read_idata();
					}
					break;
				case EL_INIT_FONT:
					if(iCurObjType == I_ELEMENT){
						if(iElem -> type == I_STRING_ELEM){
							iElStr -> font = script -> read_idata();
						}
						else {
							if(iElem -> type == I_S_STRING_ELEM){
								iElS_Str -> font = script -> read_idata();
							}
							else
								DefFont = script -> read_idata();
						}
					}
					else
						DefFont = script -> read_idata();
					break;
				case EL_INIT_SPACE:
					if(iCurObjType == I_ELEMENT){
						if(iElem -> type == I_STRING_ELEM){
							iElStr -> space = script -> read_idata();
						}
						else {
							if(iElem -> type == I_SCROLLER_ELEM){
								iElScr -> space = script -> read_idata();
							}
							else {
								if(iElem -> type == I_S_STRING_ELEM){
									iElS_Str -> space = script -> read_idata();
								}
								else {
									DefSpace = script -> read_idata();
								}
							}
						}
					}
					else
						DefSpace = script -> read_idata();
					break;
				case EL_INIT_LEVEL:
					if(iCurObjType == I_ELEMENT || iCurObjType == I_TERR_ELEMENT){
						script -> prepare_idata();
						iElem -> null_level = script -> get_idata();
						if(iCurObjType == I_TERR_ELEMENT){
							iElTerr -> def_null_level = script -> get_idata();
						}
					}
					else
						DefLevel = script -> read_idata();
					break;
				case EL_INIT_BMP_LEVEL:
					if(iCurObjType == I_ELEMENT){
						if(iElem -> type == I_BITMAP_ELEM){
							iElBmp -> bmp_null_level = script -> read_idata();
						}
						else {
							if(iElem -> type == I_SCROLLER_ELEM){
								iElScr -> bmp_null_level = script -> read_idata();
							}
							else
								handle_error("Misplaced option",OptIDs[id]);
						}
					}
					else
						handle_error("Misplaced option",OptIDs[id]);
					break;
				case EL_INIT_STRING:
					if(iCurObjType == I_ELEMENT){
						script -> prepare_pdata();
						if(iElem -> type == I_STRING_ELEM){
							iElStr -> init_string(script -> get_conv_ptr());
						}
						else {
							if(iElem -> type == I_S_STRING_ELEM){
								iElS_Str -> init_string(script -> get_conv_ptr());
							}
							else {
								handle_error("Misplaced option",OptIDs[id]);
							}
						}
					}
					else
						handle_error("Misplaced option",OptIDs[id]);
					break;
				case EL_INIT_DIR:
					if(iCurObjType == I_ELEMENT && iElem -> type == I_SCROLLER_ELEM){
						iElScr -> dir = script -> read_idata();
						if(iElScr -> dir < 0)
							handle_error("Bad scroller direction");
					}
					else
						handle_error("Misplaced option",OptIDs[id]);
					break;
				case EL_INIT_VALUE:
					if(iCurObjType == I_ELEMENT && iElem -> type == I_SCROLLER_ELEM){
						iElScr -> Value = script -> read_idata();
						iElScr -> prevValue = iElScr -> Value;
					}
					else
						handle_error("Misplaced option",OptIDs[id]);
					break;
				case EL_INIT_MAX_VALUE:
					if(iCurObjType == I_ELEMENT && iElem -> type == I_SCROLLER_ELEM){
						iElScr -> maxValue = script -> read_idata();
					}
					else
						handle_error("Misplaced option",OptIDs[id]);
					break;
				case EL_INIT_TEXT:
					if(iCurObjType == I_OBJECT){
						script -> prepare_pdata();
						load_text(script -> get_conv_ptr());
					}
					else
						handle_error("Misplaced option",OptIDs[id]);
					break;
				case EL_INIT_S_TEXT:
					if(iCurObjType == I_OBJECT){
						script -> prepare_pdata();
						load_s_text(script -> get_conv_ptr());
					}
					else
						handle_error("Misplaced option",OptIDs[id]);
					break;
				case EV_CODE:
					if(iCurObjType == I_EVENT){
						cd = new iScanCode;
						cd -> code = script -> read_idata();
						iEv -> codes -> connect((iListElement*)cd);
					}
					else
						handle_error("Misplaced option",OptIDs[id]);
					break;
				case EV_SET_MULTILINE:
					if(iCurObjType == I_EVENT){
						iEv -> flags |= EV_MULTI_LINE;
					}
					else
						handle_error("Misplaced option",OptIDs[id]);
					break;
				case EV_KEY:
					if(iCurObjType == I_EVENT){
						cd = new iScanCode;
						cd -> code = script -> read_key();
						iEv -> codes -> connect((iListElement*)cd);
					}
					else
						handle_error("Misplaced option",OptIDs[id]);
					break;
				case E_TIME:
					if(iCurObjType == I_EVENT){
						iEv -> time = script -> read_idata();
					}
					else {
						if(iCurObjType == I_COMMAND){
							iEvComm -> time = script -> read_idata();
						}
						else
							handle_error("Misplaced option",OptIDs[id]);
					}
					break;
				case EV_SELECTED:
					if(iCurObjType == I_EVENT){
						iEv -> flags |= EV_IF_SELECTED;
					}
					else
						handle_error("Misplaced option",OptIDs[id]);
					break;
				case EV_NOT_SELECTED:
					if(iCurObjType == I_EVENT){
						iEv -> flags |= EV_IF_NOT_SELECTED;
					}
					else
						handle_error("Misplaced option",OptIDs[id]);
					break;
				case EV_LOCKED:
					if(iCurObjType == I_EVENT){
						iEv -> flags |= EV_IF_LOCKED;
					}
					else
						handle_error("Misplaced option",OptIDs[id]);
					break;
				case EC_START_TIME:
					if(iCurObjType == I_COMMAND){
						iEvComm -> start_time = script -> read_idata();
					}
					else
						handle_error("Misplaced option",OptIDs[id]);
					break;
				case EC_LINE_ID:
					if(iCurObjType == I_COMMAND){
						iEvComm -> LineID = script -> read_idata();
					}
					else
						handle_error("Misplaced option",OptIDs[id]);
					break;
				case SCR_SET_BLOCK_FLAG:
					if(iCurObjType == I_SCREEN){
						iScr -> flags |= iSCR_BLOCK_GLOBAL_OBJ;
					}
					else
						handle_error("Misplaced option",OptIDs[id]);
					break;
				case SCR_SET_DEFAULT_OBJ:
					if(iCurObjType == I_SCREEN){
						script -> read_pdata(&iScr -> t_objID,1);
					}
					else
						handle_error("Misplaced option",OptIDs[id]);
					break;
				case SCR_SET_OFFS:
					if(iCurObjType == I_SCREEN){
						iScr -> ScreenOffs = script -> read_idata();
					}
					else
						handle_error("Misplaced option",OptIDs[id]);
					break;
				case SD_SET_DEFAULT_SCR:
					script -> read_pdata(&iScrDisp -> t_scrID,1);
					break;
				case EL_SET_ALIGN_X:
					if(iCurObjType == I_OBJECT){
						iScrObj -> align_x = script -> read_option() - EL_ALIGN_CENTER + 1;
					}
					else {
						if(iCurObjType == I_ELEMENT || iCurObjType == I_TERR_ELEMENT){
							iElem -> align_x = script -> read_option() - EL_ALIGN_CENTER + 1;
						}
						else
							handle_error("Misplaced option",OptIDs[id]);
					}
					break;
				case EL_SET_ALIGN_OFFS_X:
					if(iCurObjType == I_OBJECT){
						iScrObj -> align_x_offs = script -> read_idata();
					}
					else {
						if(iCurObjType == I_ELEMENT || iCurObjType == I_TERR_ELEMENT){
							iElem -> align_x_offs = script -> read_idata();
						}
						else
							handle_error("Misplaced option",OptIDs[id]);
					}
					break;
				case EL_SET_ALIGN_Y:
					if(iCurObjType == I_OBJECT){
						iScrObj -> align_y = script -> read_option() - EL_ALIGN_CENTER + 1;
					}
					else {
						if(iCurObjType == I_ELEMENT || iCurObjType == I_TERR_ELEMENT){
							iElem -> align_y = script -> read_option() - EL_ALIGN_CENTER + 1;
						}
						else
							handle_error("Misplaced option",OptIDs[id]);
					}
					break;
				case EL_SET_ALIGN_OFFS_Y:
					if(iCurObjType == I_OBJECT){
						iScrObj -> align_y_offs = script -> read_idata();
					}
					else {
						if(iCurObjType == I_ELEMENT || iCurObjType == I_TERR_ELEMENT){
							iElem -> align_y_offs = script -> read_idata();
						}
						else
							handle_error("Misplaced option",OptIDs[id]);
					}
					break;
				case TRIG_NUM_STATE:
					if(iCurObjType == I_OBJECT && iScrObj -> flags & OBJ_TRIGGER){
						iTrig -> num_state = script -> read_idata();
						if(iTrig -> flags & TRG_MEM_ALLOC)
							handle_error("Trigger already inited");
						iTrig -> alloc_state();
					}
					else
						handle_error("Misplaced option",OptIDs[id]);
					break;
				case TRIG_NEW_ELEMENT:
					if(iCurObjType == I_OBJECT && iScrObj -> flags & OBJ_TRIGGER){
						st = script -> read_idata();
						if(st >= iTrig -> num_state)
							handle_error("Trigger state value out of range");

						new_object(I_ELEMENT);
						if(!(iTrig -> flags & TRG_MEM_ALLOC))
							iTrig -> alloc_state();
						iTrig -> StateElem[st] = (iListElement*)iElem;
						if(st != iTrig -> state)
							iElem -> flags |= EL_NO_REDRAW;
					}
					else
						handle_error("Misplaced option",OptIDs[id]);
					break;
			}
#ifndef _BINARY_SCRIPT_
		}
		else
			script -> next_ptr();
#endif
	}
	build_links();
	free_hfonts();

#ifdef _SAVE_BINARY_SCRIPT_
	script -> close_bscript();
#endif

	delete script;

	delete iStk;
#ifndef _BINARY_SCRIPT_
	_sFREE_HEAP_(iSCRIPT_HEAP_SIZE,char);
#endif
}

void new_object(int tp)
{
	iListElement* p = NULL;
	if(iCurObjType != I_ABSENT){
		switch(iCurObjType){
			case I_EVENT:
				p = (iListElement*)iEv;
				break;
			case I_ELEMENT:
				handle_error("Can't nest object");
				break;
			case I_OBJECT:
				p = (iListElement*)iScrObj;
				break;
			case I_SCREEN:
				p = (iListElement*)iScr;
				break;
		}
		iStk -> add(p,iCurObjType);
	}
	switch(tp){
		case I_COMMAND:
			iEvComm = new iScreenEventCommand;
			break;
		case I_EVENT:
			iEv = new iScreenEvent;
			break;
		case I_TERR_ELEMENT:
			iElTerr = new iTerrainElement;
			iElem = (iScreenElement*)iElTerr;
			break;
		case I_ELEMENT:
			new_element();
			break;
		case I_TRIGGER:
			iTrig = new iTriggerObject;
			iScrObj = (iScreenObject*)iTrig;
			iScrObj -> ID = ObjIDSize;
			iScrObj -> flags |= OBJ_TRIGGER;
			break;
		case I_OBJECT:
			iScrObj = new iScreenObject;
			iScrObj -> ID = ObjIDSize;
			break;
		case I_SCREEN:
			iScr = new iScreen;
			iScr -> ID = ObjIDSize;
			break;
	}
	iCurObjType = tp;
	if(iCurObjType == I_TRIGGER)
		iCurObjType = I_OBJECT;
}

void link_object(void)
{
	iStackElement* p = iStk -> get();
	if(p != NULL){
		if(p -> objType == iCurObjType)
			handle_error("Bad object link");
		switch(p -> objType){
			case I_EVENT:
				iEv = (iScreenEvent*)p -> obj;
				if(iCurObjType == I_COMMAND){
					iEv -> CommSeq -> connect((iListElement*)iEvComm);
				}
				else
					handle_error("Can't link object");
				break;
			case I_OBJECT:
				iScrObj = (iScreenObject*)p -> obj;
				if(iCurObjType == I_EVENT){
					iScrObj -> EventList -> connect((iListElement*)iEv);
				}
				else {
					if(iCurObjType == I_ELEMENT){
						iScrObj -> add_element((iListElement*)iElem);
					}
					else
						handle_error("Can't link object");
				}
				break;
			case I_SCREEN:
				iScr = (iScreen*)p -> obj;
				if(iCurObjType == I_OBJECT){
//					  iScr -> objList -> connect((iListElement*)iScrObj);
					curID_List = iScr -> ObjIDs;
					iScr -> add_object(iScrObj);
				}
				else {
					if(iCurObjType == I_EVENT){
						iScr -> EventList -> connect((iListElement*)iEv);
					}
					else {
						if(iCurObjType == I_TERR_ELEMENT){
							iScrObj = new iScreenObject;
							if(iElTerr -> ID)
								iScrObj -> ID = iElTerr -> ID;
							else
								iScrObj -> ID = 1;

							iScrObj -> PosX = iElTerr -> lX;
							iScrObj -> PosY = iElTerr -> lY;

							iScrObj -> align_x = iElTerr -> align_x;
							iScrObj -> align_y = iElTerr -> align_y;
							iScrObj -> align_x_offs = iElTerr -> align_x_offs;
							iScrObj -> align_y_offs = iElTerr -> align_y_offs;

							iElTerr -> pX = iElTerr -> lX;
							iElTerr -> pY = iElTerr -> lY;

							iElem -> lX = iElem -> lY = 0;

							iElTerr -> align_x = iElTerr -> align_y = 0;
							iElTerr -> align_x_offs = iElTerr -> align_y_offs = 0;

//							  iScrObj -> ElementList -> connect((iListElement*)iElTerr);
							iScrObj -> add_element((iListElement*)iElTerr);
							iScr -> add_terr_object(iScrObj);
//							  iScr -> TerrainList -> connect((iListElement*)iScrObj);
						}
						else
							handle_error("Can't link object");
					}
				}
				break;
		}
		iCurObjType = p -> objType;
	}
	else {
		if(iCurObjType == I_SCREEN){
			iScrDisp -> connect((iListElement*)iScr);
			curID_List = iScrDisp -> ScreenIDs;
		}
		else {
			if(iCurObjType == I_OBJECT){
				iScrDisp -> ObjHeap -> connect((iListElement*)iScrObj);
			}
			else
				handle_error("Can't link object");
		}
		iCurObjType = I_ABSENT;
	}
}

void addObjID(const char* p,iListElement* obj)
{
	iObjID* id;
	if(getObjID(p) != -1)
		handle_error("Duplicate object ID",p);

	id = new iObjID;
	id -> init(p,ObjIDSize);
	id -> obj = obj;

	curID_List -> connect((iListElement*)id);

	if(obj)
		obj -> init_id(id -> ID);

	ObjTable[ObjIDSize] = obj;
	ObjIDSize ++;
	if(ObjIDSize >= I_MAX_ID)
		handle_error("ID table overflow");
}

int getObjID(const char* p)
{
	iObjID* id = (iObjID*)curID_List -> last;
	while(id){
		if(id -> compare(p))
			return id -> numID;
		id = (iObjID*)id -> prev;
	}
	id = (iObjID*)iScrDisp -> ScreenIDs -> last;
	while(id){
		if(id -> compare(p))
			return id -> numID;
		id = (iObjID*)id -> prev;
	}
	return -1;
}

void init_command(int code)
{
	int v_flag = 0,num = 0,d0_flag = 0,d1_flag = 0;
	if(code == -1)
		handle_error("Bad command code");
	iEvComm -> EvType = code - EC_RISE_OBJ + 1;
	switch(iEvComm -> EvType){
		case EV_RISE_SCREEN:
		case EV_HIDE_SCREEN:
			iEvComm -> objType = I_ABSENT;
			break;
		case EV_HIDE_OBJECT:
		case EV_RISE_OBJECT:
		case EV_LOW_OBJECT:
			iEvComm -> objType = I_OBJECT;
			break;
		case EV_SCALE_OBJECT:
		case EV_SMOOTH_OBJECT:
			iEvComm -> objType = I_OBJECT;
			v_flag = 1;
			break;
		case EV_SCREEN_JUMP:
			iEvComm -> objType = I_SCREEN;
			break;
		case EV_EXT_SCREEN_JUMP:
			iEvComm -> objType = I_ABSENT;
			v_flag = 2;
			break;
		case EV_OBJECT_JUMP:
			iEvComm -> objType = I_OBJECT;
			break;
		case EV_EXIT:
			iEvComm -> objType = I_ABSENT;
			v_flag = 1;
			break;
		case EV_CHANGE_SCROLLER:
			iEvComm -> objType = I_SCROLLER;
			iEvComm -> flags |= EC_ELEMENT_LEVEL;
			break;
		case EV_CHANGE_TRIGGER:
		case EV_PREV_TRIGGER:
			iEvComm -> objType = I_OBJECT;
			break;
		case EV_CHANGE_AVI:
			iEvComm -> objType = I_ELEMENT;
			v_flag = 2;
			break;
		case EV_NEXT_AVI:
		case EV_PREV_AVI:
			iEvComm -> objType = I_ELEMENT;
			break;
		case EV_START_DEMO:
			iEvComm -> objType = I_ABSENT;
			break;
		case EV_EXT_EVENT:
			iEvComm -> objType = I_ABSENT;
			v_flag = 1;
			d0_flag = 1;
			break;
		case EV_INPUT_STRING:
			iEvComm -> objType = I_ELEMENT;
			v_flag = 1;
			break;
		case EV_MOVE_SCREEN:
			iEvComm -> objType = I_ABSENT;
			v_flag = 1;
			break;
		case EV_MOVE2SCREEN:
			iEvComm -> objType = I_SCREEN;
			break;
		case EV_SEND2ACTINT:
		case EV_SEND2ML_DISP:
			iEvComm -> objType = I_ABSENT;
			v_flag = 1;
			break;
		case EV_ML_EVENT:
			iEvComm -> objType = I_ABSENT;
			v_flag = 1;
			d0_flag = 1;
			d1_flag = 1;
			break;
		case EV_RESET_TEXTS:
			iEvComm -> objType = I_ABSENT;
			break;
		case EV_NEXT_TEXT:
		case EV_PREV_TEXT:
			iEvComm -> objType = I_SCREEN;
			v_flag = 1;
			break;
		case EV_SEND_DATA2ACTINT:
			iEvComm -> objType = I_ABSENT;
			v_flag = 3;
			break;
		case EV_SET_TRIGGER:
			iEvComm -> objType = I_OBJECT;
			v_flag = 1;
			break;
		case EV_LOCK_OBJ:
		case EV_UNLOCK_OBJ:
			iEvComm -> objType = I_OBJECT;
			break;
		case EV_SET_LINE_ID:
			iEvComm -> objType = I_ABSENT;
			v_flag = 1;
			break;
		case EV_END_EVENT:
		case EV_EXIT_EVENT:
		case EV_CHECK_CREDITS:
			iEvComm -> objType = I_ABSENT;
			break;
		case EV_STOP_AVI:
		case EV_PLAY_AVI:
			iEvComm -> objType = I_ELEMENT;
			break;
	}
	if(iEvComm -> objType != I_ABSENT && !(iEvComm -> flags & EC_LIST)){
		script -> read_pdata(&iEvComm -> tmp_objID,1);
	}
	if(v_flag){
		if(v_flag == 1){
			iEvComm -> value = script -> read_idata();
		}
		if(v_flag == 2){
			script -> read_pdata(&iEvComm -> pvalue,1);
		}
		if(v_flag == 3){
			iEvComm -> value = script -> read_idata();
			num = script -> read_idata();
			iEvComm -> value |= (num << 16);
		}
		if(d0_flag){
			iEvComm -> data0 = script -> read_idata();
		}
		if(d1_flag){
			iEvComm -> data1 = script -> read_idata();
		}
	}
}

void build_links(void)
{
	int id;

	iScreenEventCommand* p;
	iScreenEvent* ev;
	iScreenObject* obj;
	iScreen* scr = (iScreen*)iScrDisp -> last;

	curID_List = iScrDisp -> ScreenIDs;
	obj = (iScreenObject*)iScrDisp -> ObjHeap -> last;
	while(obj){
		ev = (iScreenEvent*)obj -> EventList -> last;
		while(ev){
			p = (iScreenEventCommand*)ev -> CommSeq -> last;
			while(p){
				if(p -> objType != I_ABSENT){
					id = getObjID(p -> tmp_objID);
					if(id == -1){
						handle_error("Object not found",p -> tmp_objID);
					}
					p -> obj = ObjTable[id];
					p -> objID = id;
				}
				else {
					p -> obj = NULL;
					p -> objID = -1;
				}
				p = (iScreenEventCommand*)p -> prev;
			}
			ev = (iScreenEvent*)ev -> prev;
		}
		obj = (iScreenObject*)obj -> prev;
	}

	while(scr){
		curID_List = scr -> ObjIDs;
		id = getObjID(scr -> t_objID);
		if(id == -1){
			handle_error("Object not found",scr -> t_objID);
		}
		scr -> dcurObj = (iScreenObject*)ObjTable[id];
		scr -> curObj = scr -> dcurObj;
		scr -> curObj -> flags |= OBJ_SELECTED;

		obj = (iScreenObject*)scr -> objList -> last;
		ev = (iScreenEvent*)scr -> EventList -> last;
		while(ev){
			p = (iScreenEventCommand*)ev -> CommSeq -> last;
			while(p){
				if(p -> objType != I_ABSENT){
					id = getObjID(p -> tmp_objID);
					if(id == -1){
						handle_error("Object not found",p -> tmp_objID);
					}
					p -> obj = ObjTable[id];
					p -> objID = id;
				}
				else {
					p -> obj = NULL;
					p -> objID = -1;
				}
				p = (iScreenEventCommand*)p -> prev;
			}
			ev = (iScreenEvent*)ev -> prev;
		}
		while(obj){
			ev = (iScreenEvent*)obj -> EventList -> last;
			while(ev){
				p = (iScreenEventCommand*)ev -> CommSeq -> last;
				while(p){
					if(p -> objType != I_ABSENT){
						id = getObjID(p -> tmp_objID);
						if(id == -1){
							handle_error("Object not found",p -> tmp_objID);
						}
						p -> obj = ObjTable[id];
						p -> objID = id;
					}
					else {
						p -> obj = NULL;
						p -> objID = -1;
					}
					p = (iScreenEventCommand*)p -> prev;
				}
				ev = (iScreenEvent*)ev -> prev;
			}
			obj = (iScreenObject*)obj -> prev;
		}
		scr = (iScreen*)scr -> prev;
	}

	id = getObjID(iScrDisp -> t_scrID);
	if(id == -1){
		handle_error("Object not found",iScrDisp -> t_scrID);
	}
	init_objects();
	iScrDisp -> curScr = (iScreen*)ObjTable[id];
}

void init_objects(void)
{
	iScreenObject* obj;
	iScreen* scr = (iScreen*)iScrDisp -> last;

	obj = (iScreenObject*)iScrDisp -> ObjHeap -> last;
	while(obj){
		obj -> init();
		obj = (iScreenObject*)obj -> prev;
	}

	while(scr){
		obj = (iScreenObject*)scr -> objList -> last;
		while(obj){
			obj -> init();
			obj = (iScreenObject*)obj -> prev;
		}
		obj = (iScreenObject*)scr -> TerrainList -> last;
		while(obj){
			obj -> init();
			obj = (iScreenObject*)obj -> prev;
		}
		scr -> init_prompt();
		scr = (iScreen*)scr -> prev;
	}
}

void load_text(char* fname)
{
	XStream fh;
	int i,sz,y = 0,t_sz;
	char* buf;
	iStringElement* p;
	if(iCurObjType == I_OBJECT){
		fh.open(fname,XS_IN);
//		buf = aciLoadPackedFile(fh,sz);
		sz = fh.size();
		buf = new char[sz];
		fh.read(buf,sz);
		fh.close();
		
		for(i = 0; i < sz; i ++){
			if(buf[i] == '\r' || buf[i] == '\n') buf[i] = 0;
		}
		i = 0;
		while(i < sz){
			while(!buf[i]){
				i ++;
				if(i >= sz) break;
			}
			if(i < sz){
				p = new iStringElement;
				t_sz = strlen(buf + i) + 1;
				if(t_sz){
					strcpy(p -> string,buf + i);
					p -> flags |= EL_TEXT_STRING;

					p -> font = DefFont;
					p -> null_level = DefLevel;
					p -> space = DefSpace;

					p -> lY = y;
					y += HFntTable[p -> font] -> SizeY + p -> space;
					iScrObj -> add_element((iListElement*)p);
				}
				while(buf[i]){
					i ++;
					if(i >= sz) break;
				}
			}
		}
	}
}

void load_s_text(char* fname)
{
	XStream fh;
	int i,sz,y = 0,t_sz;
	char* buf;
	iS_StringElement* p;
	if(iCurObjType == I_OBJECT){
		fh.open(fname,XS_IN);
//		buf = aciLoadPackedFile(fh,sz);
		sz = fh.size();
		buf = new char[sz];
		fh.read(buf,sz);
		fh.close();
		
		for(i = 0; i < sz; i ++){
			if(buf[i] == '\r' || buf[i] == '\n') buf[i] = 0;
		}
		i = 0;
		while(i < sz){
			while(!buf[i]){
				i ++;
				if(i >= sz) break;
			}
			if(i < sz){
				p = new iS_StringElement;
				t_sz = strlen(buf + i) + 1;
				if(t_sz){
					strcpy(p -> string,buf + i);
					p -> flags |= EL_TEXT_STRING;

					p -> font = DefFont;
					p -> null_level = DefLevel;
					p -> space = DefSpace;

					p -> lY = y;
					y += iScrFontTable[p -> font] -> SizeY + p -> space;
					iScrObj -> add_element((iListElement*)p);
				}
				while(buf[i]){
					i ++;
					if(i >= sz) break;
				}
			}
		}
	}
}

int GetOptionID(char* p)
{
	int i;
	if(!p) handle_error("Null ID pointer");
	for(i = 0; i < MAX_OPTION; i ++){
		if(!strcmp(p,OptIDs[i]))
			return i;
	}
	return -1;
}

void new_element(void)
{
	int id;

	id = script -> read_option();

	switch(id){
		case EL_STRING:
			iElStr = new iStringElement;
			iElem = (iScreenElement*)iElStr;
			break;
		case EL_S_STRING:
			iElS_Str = new iS_StringElement;
			iElem = (iScreenElement*)iElS_Str;
			break;
		case EL_BITMAP:
			iElBmp = new iBitmapElement;
			iElem = (iScreenElement*)iElBmp;
			break;
		case EL_SCROLLER:
			iElScr = new iScrollerElement;
			iElScr -> ID = ObjIDSize;
			iElem = (iScreenElement*)iElScr;
			script -> prepare_pdata();
			addObjID(script -> get_conv_ptr(),(iListElement*)iElScr);
			break;
		case EL_AVI:
			iElAvi = new iAVIElement;
			iElem = (iScreenElement*)iElAvi;
			iElem -> ID = ObjIDSize;

			script -> prepare_pdata();
			addObjID(script -> get_conv_ptr(),(iListElement*)iElAvi);
			break;
	}
}
