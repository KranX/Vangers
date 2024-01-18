/* ---------------------------- INCLUDE SECTION ----------------------------- */

#include "../global.h"

#include "../iscreen/hfont.h"
#include "../iscreen/iscreen.h"
#include "item_api.h"
#include "actint.h"
#include "a_consts.h"
#include "../iscreen/s_mem.h"

#include "../iscreen/iscript.h"

#include "mlstruct.h"
#include "layout.h"
/* ----------------------------- STRUCT SECTION ----------------------------- */
/* ----------------------------- EXTERN SECTION ----------------------------- */

extern ScriptFile* script;
extern int aCellSize;
extern int iCellSize;

extern int actintLowResFlag;

extern int aciColSchemeLen;
extern int aciNumColSchemes;
extern unsigned char** aciColorSchemes;

/* --------------------------- PROTOTYPE SECTION ---------------------------- */

void aParseScript(const char* fname);
int aGetOptionID(char* p);

void begin_block(void);
void end_block(void);

void handle_error(const char* prefix,const char* subj = NULL);

void load_matrix(void);
void load_slot_nums(void);
void load_slot_types(void);

void load_item_shape(void);

void end_block(void);

void init_event_code(int cd);

/* --------------------------- DEFINITION SECTION --------------------------- */

#define I_STACK_SIZE	16

enum aScriptModes
{
	AS_NONE,		// 0
	AS_INIT_MATRIX, 	// 1
	AS_INIT_BUTTON, 	// 2
	AS_INIT_ITEM,		// 3
	AS_INIT_MATRIX_EL,	// 4
	AS_INIT_MENU,		// 5
	AS_INIT_MENU_ITEM,	// 6
	AS_INIT_IBS,		// 7
	AS_INIT_BML,		// 8
	AS_INIT_IND,		// 9
	AS_INIT_SHAPE_OFFS,	// 10
	AS_INIT_INFO_PANEL,	// 11
	AS_INIT_COUNTER,	// 12
	AS_INIT_COLOR_SCHEME,	// 13
	AS_INIT_LOC_DATA,	// 14
	AS_INIT_WORLD_MAP,	// 15
	AS_INIT_WORLD_DATA,	// 16

	AML_INIT_EVENT_COMMAND, // 17
	AML_INIT_EVENT, 	// 18
	AML_INIT_DATA_SET,	// 19
	AML_INIT_DATA,		// 20
	AML_INIT_EVENT_SEQ,	// 21

	BM_INIT_MENU_ITEM,	// 22
	BM_INIT_MENU,		// 23

	AS_INIT_SHUTTER,	// 24


	AS_MAX_MODE
};

enum aOptions
{
	// Common section...
	I_END_BLOCK,		// 0
	I_END_STRING,		// 1

	INIT_MODE_KEY,		// 2
	INIT_INV_KEY,		// 3
	INIT_INFO_KEY,		// 4

	INIT_CELL_SIZE, 	// 5

	NEW_INV_MATRIX, 	// 6
	NEW_INV_ITEM,		// 7
	NEW_MENU,		// 8
	NEW_MENU_ITEM,		// 9
	NEW_BUTTON,		// 10

	INT_BUTTON_TYPE,	// 11
	INV_BUTTON_TYPE,	// 12
	INF_BUTTON_TYPE,	// 13

	INIT_X, 		// 14
	INIT_Y, 		// 15
	INIT_SX,		// 16
	INIT_SY,		// 17
	INIT_MSX,		// 18
	INIT_MSY,		// 19

	INIT_SCANCODE,		// 20

	INIT_CLASS,		// 21
	ITEM_BASE,		// 22
	ITEM_WEAPON,		// 23

	INIT_FNC_CODE,		// 24
	INIT_SPACE,		// 25
	INIT_FONT,		// 26
	INIT_STRING,		// 27

	INIT_CUR_FNC,		// 28

	INIT_FNAME,		// 29
	INIT_SEQ_NAME,		// 30

	INIT_EVENT_CODE,	// 31
	INIT_ACTIVE_TIME,	// 32

	INIT_MATRIX_EL, 	// 33

	INIT_MENU_TYPE, 	// 34

	INIT_UP_KEY,		// 35
	INIT_DOWN_KEY,		// 36

	INIT_TRIGGER,		// 37
	INIT_ID,		// 38

	SET_UNPRESS,		// 39

	SET_BML_NAME,		// 40
	SET_IBS_NAME,		// 41
	SET_MAP_NAME,		// 42
	SET_BCELL_NAME, 	// 43
	SET_WCELL_NAME, 	// 44

	SET_CURMATRIX,		// 45
	NEW_IBS,		// 46
	NEW_BML,		// 47

	INIT_FILE,		// 48
	INIT_BGROUND,		// 49

	INIT_CUR_IBS,		// 50

	INIT_SLOT_NUMS, 	// 51
	INIT_SLOT_TYPES,	// 52

	INIT_SLOT_NUM,		// 53
	INIT_SLOT_TYPE, 	// 54

	NEW_IND,		// 55
	INIT_RADIUS,		// 56
	INIT_MAX_VAL,		// 57
	INIT_CORNER,		// 58
	INIT_IND_TYPE,		// 59

	INIT_SHAPE_LEN, 	// 60
	INIT_SHAPE,		// 61
	INIT_NUM_INDEX, 	// 62

	INIT_BACK_BML,		// 63

	NEW_INFO_PANEL, 	// 64
	INIT_VSPACE,		// 65
	INIT_HSPACE,		// 66

	NEW_COUNTER,		// 67

	INT_COUNTER,		// 68
	INF_COUNTER,		// 69
	INV_COUNTER,		// 70

	INIT_NUMVALS,		// 71

	INIT_NUM_SCHEMES,	// 72
	INIT_SCHEME_LEN,	// 73
	NEW_COL_SCHEME, 	// 74
	INIT_COLOR,		// 75

	TOGGLE_ISCREEN_MODE,	// 76

	NEW_LOC_DATA,		// 77
	INIT_SCREEN_ID, 	// 78
	INIT_PAL_NAME,		// 79

	INIT_S_ID,		// 80

	NEW_WORLD_MAP,		// 81
	NEW_WORLD_DATA, 	// 82

	INIT_OFFS_X,		// 83
	INIT_OFFS_Y,		// 84

	ADD_FLAG,		// 85
	ADD_LINK,		// 86

	INIT_INFO_OFFS_X,	// 87
	INIT_INFO_OFFS_Y,	// 88

	SET_NO_ALIGN,		// 89

	SET_WORLD_NAME, 	// 90

	INIT_COMMENTS,		// 91

	INIT_AVI_ID,		// 92

	INIT_BACK_COL,		// 93
	SET_MAX_STR,		// 94

	SET_NO_DEACTIVATE_FLAG, // 95
	SET_LOCATION_MENU_FLAG, // 96

	SET_NUM_V_ITEMS,	// 97

	NEW_ML_EVENT_COMMAND,	// 98
	NEW_ML_EVENT,		// 99
	NEW_ML_DATA,		// 100

	INIT_ML_EVENT_CODE,	// 101

	INIT_ML_EVENT_STARTUP,	// 102
	INIT_ML_EVENT_SDATA,	// 103

	INIT_STARTUP_TIME,	// 104
	INIT_SHUTDOWN_TIME,	// 105

	INIT_START_TIMER,	// 106

	INIT_CHANNEL_ID,	// 107

	NEW_ML_DATA_SET,	// 108

	INIT_ML_EVENT_KEY_CODE, // 109
	SET_ML_EVENT_SEQUENCE,	// 110

	SET_SUBMENU_FLAG,	// 111
	SET_NUM_AVI_ID, 	// 112
	SET_SHOW_LOAD,		// 113
	SET_NO_SHOW_LOAD,	// 114

	NEW_BITMAP_MENU,	// 115
	NEW_BITMAP_MENU_ITEM,	// 116

	SET_EXCLUDE,		// 117

	NEW_ML_ITEM_DATA,	// 118

	SET_UPMENU_FLAG,	// 119
	SET_SUBMENU_ID, 	// 120
	SET_BSUBMENU_ID,	// 121

	SET_MAINMENU_FLAG,	// 122
	SET_NO_DELETE_FLAG,	// 123

	SET_DOOR_ID,		// 124
	SET_SHAPE_ID,		// 125

	INIT_PART_DATA, 	// 126

	NEW_GATE_SHUTTER,	// 127
	NEW_MATRIX_SHUTTER,	// 128

	INIT_SHUTTER_POS,	// 129
	INIT_SHUTTER_DELTA,	// 130

	INIT_NUM_GATE_SHUTTERS, 	// 131
	INIT_NUM_MATRIX_SHUTTERS,	// 132

	SET_NOT_COMPLETED_FLAG, 	// 133

	SET_PROMPT_TEXT,		// 134

	SET_SAVE_SCREEN_ID,		// 135
	SET_WORLD_ID,			// 136

	SET_RAFFA_FLAG, 		// 137

	SET_PRIORITY,			// 138
	SET_NOT_LOCKED_FLAG,		// 139
	SET_LOCKED_FLAG,		// 140

	SET_SOUND_PATH, 		// 141

	NEW_ML_EVENT_SEQ,		// 142

	SET_SPEECH_CHANNEL,		// 143
	SET_SPEECH_LEVEL,		// 144

	SET_RND_VALUE,			// 145
	SET_CONTROL_ID, 		// 146
	SET_FRAME_CHECK_FLAG,		// 147

	INIT_MECHOS_PRM,		// 148
	INIT_ITEM_PRM,			// 149

	SET_RANGE_FLAG, 		// 150
	SET_ACTIVATE_FLAG,		// 151
	SET_TEMPLATE,			// 152
	SET_ESCAVE_FLAG,		// 153

	ADD_MAP_INFO_FILE,		// 154
	ANCHOR_RIGHT,			//155
	ANCHOR_BOTTOM,			//156

	MAX_OPTION
};

static const char* aOptIDs[MAX_OPTION] =
{
	"}",            // 0
	"\n",           // 1

	"mode_key",     // 2
	"inv_key",      // 3
	"info_key",     // 4

	"cell_size",    // 5

	"invMatrix",    // 6
	"invItem",      // 7
	"fncMenu",      // 8
	"fncMenuItem",  // 9
	"Button",       // 10

	"INT_BUTTON",   // 11
	"INV_BUTTON",   // 12
	"INF_BUTTON",   // 13

	"x",            // 14
	"y",            // 15
	"size_x",       // 16
	"size_y",       // 17
	"msize_x",      // 18
	"msize_y",      // 19

	"key",          // 20

	"class",        // 21
	"ITEM",         // 22
	"WEAPON",       // 23

	"function",     // 24
	"space",        // 25
	"font",         // 26
	"string",       // 27

	"cur_function", // 28

	"bmp_file",     // 29
	"frame_seq",    // 30

	"event",        // 31
	"active_time",  // 32

	"matrix",       // 33
	"menu_type",    // 34

	"up_key",       // 35
	"down_key",     // 36

	"trig_button",  // 37
	"id",           // 38

	"unpress",      // 39
	"bml_name",     // 40
	"ibs_name",     // 41
	"map_name",     // 42
	"bcell_name",   // 43
	"wcell_name",   // 44
	"cur_matrix",   // 45

	"ibs_obj",      // 46
	"bml_obj",      // 47

	"file",         // 48
	"bground",      // 49
	"cur_ibs",      // 50

	"slot_nums",    // 51
	"slot_types",   // 52

	"slot_num",     // 53
	"slot_type",    // 54

	"ind_data",     // 55
	"radius",       // 56
	"max_val",      // 57
	"corner",       // 58
	"ind_type",     // 59

	"shape_len",    // 60
	"shape",        // 61
	"num_index",    // 62

	"back_bml",     // 63

	"info_panel",   // 64
	"vspace",       // 65
	"hspace",       // 66

	"counter",      // 67

	"INT_COUNTER",  // 68
	"INF_COUNTER",  // 69
	"INV_COUNTER",  // 70

	"num_values",           // 71

	"num_col_schemes",      // 72
	"col_scheme_len",       // 73
	"color_scheme",         // 74
	"color",                // 75
	"iscreen_section",      // 76

	"location_data",        // 77
	"screen_id",            // 78
	"palette",              // 79
	"s_id",                 // 80

	"world_map",            // 81
	"world_data",           // 82

	"offs_x",               // 83
	"offs_y",               // 84

	"flag",                 // 85
	"link",                 // 86

	"info_offs_x",          // 87
	"info_offs_y",          // 88

	"no_align",             // 89

	"name",                 // 90

	"comments",             // 91

	"avi_id",               // 92
	"back_col",             // 93

	"max_str",              // 94

	"no_deactivate",        // 95
	"location_menu",        // 96
	"visible_items",        // 97

	"ml_ev_comm",           // 98
	"ml_event",             // 99
	"ml_data",              // 100

	"code",                 // 101

	"startup",              // 102
	"startup_data",         // 103

	"startup_time",         // 104
	"shutdown_time",        // 105

	"start_time",           // 106

	"channel",              // 107

	"ml_data_set",          // 108

	"key_code",             // 109
	"sequence",             // 110

	"submenu",              // 111
	"num_avi_ids",          // 112
	"show_shots",           // 113
	"no_show_shots",        // 114

	"bitmap_menu",          // 115
	"bitmap_menu_item",     // 116

	"exclude_item",         // 117

	"ml_item",              // 118

	"up_menu_item",         // 119
	"submenu_id",           // 120
	"bsubmenu_id",          // 121

	"main_menu",            // 122
	"no_delete",            // 123

	"door_id",              // 124
	"shape_id",             // 125

	"part_data",            // 126

	"gate_shutter",         // 127
	"matrix_shutter",       // 128

	"pos",                  // 129
	"delta",                // 130

	"num_gate_shutters",    // 131
	"num_matrix_shutters",  // 132
	"not_complete",         // 133

	"prompt_text",          // 134

	"save_screen",          // 135
	"world",                // 136

	"raffa",                // 137

	"priority",             // 138
	"if_not_locked",        // 139
	"if_locked",            // 140

	"sounds",               // 141

	"ml_event_seq",         // 142

	"speech_channel",       // 143
	"speech_level",         // 144

	"rnd",                  // 145
	"control_id",           // 146

	"frame_check",          // 147

	"mech_prm",             // 148
	"item_prm",             // 149

	"range_font",           // 150
	"no_activate",          // 151
	"prm_template",         // 152
	"show_escave",          // 153

	"map_data",             // 154
	"anchor_right",         // 155
	"anchor_bottom"         // 156
};

int curMode = AS_NONE;

int curScheme = 0;

invMatrix* invMat;
invItem* invItm;

InfoPanel* iPl;

int curMatrix = 0;
int iScreenFlag = 0;

int upMenuFlag = 0;

ibsObject* ibsObj;
bmlObject* bmlObj;
fncMenu* fnMnu;
fncMenuItem* fnMnuItm;
aButton* aBt;
aIndData* aInd;
CounterPanel* cP;
aciLocationInfo* locData;
aciLocationShutterInfo* locSh;

aciWorldMap* wMap;
aciWorldInfo* wData;

actIntDispatcher* aScrDisp;

int aciCurCredits03 = 0;

aciML_EventCommand* mlEvComm;
aciML_Event* mlEv;
aciML_Data* mlData;
aciML_DataSet* mlDataSet;
aciML_ItemData* mlItm;
aciML_EventSeq* mlEvSeq;

aciBitmapMenuItem* aciBM_it;
aciBitmapMenu* aciBM;

void aParseScript(const char* fname,const char* bname)
{
	int i,id,t_id = 0,sz = 0,num;

	char* ptr;
	iListElement* elemPtr;

	invMatrix* mtx;
	invItem* itm;

	aScrDisp = new actIntDispatcher;
	aciML_D = new aciML_Dispatcher;

#ifndef _BINARY_SCRIPT_
	_sALLOC_HEAP_(3000000,char);
#endif
	script = new ScriptFile;
#ifndef _BINARY_SCRIPT_
	script -> set_bscript_name(bname);
#endif
	script -> load(fname);
	script -> prepare();

	while(!script -> EOF_Flag){
#ifndef _BINARY_SCRIPT_
		if(script -> curBlock && *script -> curBlock -> data){
#endif
			id = script -> read_option(0);

#ifndef _BINARY_SCRIPT_
			if(id == -1)
				handle_error("Unknown keyword",script -> prevStrBuf);
#else
			if(id == iSCRIPT_EOF) script -> EOF_Flag = 1;
			if(id == -1)
				handle_error("Unknown keyword",NULL);
#endif

			switch(id){
				case INIT_MECHOS_PRM:
					t_id = script -> read_idata();
					t_id = script -> read_idata();
					mtx = aScrDisp -> get_imatrix(t_id);
					if(mtx){
						t_id = script -> read_idata();
						if(!mtx -> pData) mtx -> alloc_prm();
						ptr = mtx -> pData + t_id * ACI_MAX_PRM_LEN;
						script -> read_pdata(&ptr);
					}
					else
						handle_error("Bad matrix ID");
					break;
				case INIT_ITEM_PRM:
					t_id = script -> read_idata();
					itm = aScrDisp -> get_iitem(t_id);
					if(itm){
						t_id = script -> read_idata();
						if(!itm -> pData) itm -> alloc_prm();
						ptr = itm -> pData + t_id * ACI_MAX_PRM_LEN;
						script -> read_pdata(&ptr);
					}
					else
						handle_error("Bad item ID");
					break;
				case SET_PROMPT_TEXT:
					if(curMode == AS_INIT_BUTTON){
						script -> read_pdata(&aBt -> promptData,1);
					}
					else {
						if(curMode == AS_INIT_IND){
							script -> read_pdata(&aInd -> promptData,1);
						}
						else {
							if(curMode == AS_INIT_ITEM){
								script -> read_pdata(&invItm -> promptData,1);
							}
							else
								handle_error("Misplaced option",aOptIDs[id]);
						}
					}
					break;
				case INIT_NUM_GATE_SHUTTERS:
					if(curMode == AS_INIT_LOC_DATA){
						t_id = script -> read_idata();
						locData -> alloc_gate_shutters(t_id);
					}
					else
						handle_error("Misplaced option",aOptIDs[id]);
					break;
				case ADD_MAP_INFO_FILE:
					if(curMode == AS_INIT_LOC_DATA){
						script -> prepare_pdata();
						elemPtr = new iListElement;
						elemPtr -> init_id(script -> get_conv_ptr());
						locData -> mapData -> connect(elemPtr);
					}
					else
						handle_error("Misplaced option",aOptIDs[id]);
					break;
				case SET_SOUND_PATH:
					if(curMode == AS_INIT_LOC_DATA){
						script -> read_pdata(&locData -> soundResPath,1);
					}
					else
						handle_error("Misplaced option",aOptIDs[id]);
					break;
				case INIT_NUM_MATRIX_SHUTTERS:
					if(curMode == AS_INIT_LOC_DATA){
						t_id = script -> read_idata();
						sz = script -> read_idata();
						locData -> alloc_matrix_shutters(t_id,sz);
					}
					else
						handle_error("Misplaced option",aOptIDs[id]);
					break;
				case INIT_SHUTTER_POS:
					if(curMode == AS_INIT_SHUTTER){
						t_id = script -> read_idata();

						locSh -> Pos[t_id].X = script -> read_idata();
						locSh -> Pos[t_id].Y = script -> read_idata();
					}
					else
						handle_error("Misplaced option",aOptIDs[id]);
					break;
				case INIT_SHUTTER_DELTA:
					if(curMode == AS_INIT_SHUTTER){
						locSh -> Delta.X = script -> read_idata();
						locSh -> Delta.Y = script -> read_idata();
					}
					else
						handle_error("Misplaced option",aOptIDs[id]);
					break;
				case NEW_GATE_SHUTTER:
					if(curMode == AS_INIT_LOC_DATA){
						curMode = AS_INIT_SHUTTER;
						locSh = new aciLocationShutterInfo;

						t_id = script -> read_idata();
						if(t_id < 0 || t_id >= locData -> numGateShutters) ErrH.Abort("Bad shutter index...");
						locData -> GateShutters[t_id] = locSh;
					}
					else
						handle_error("Misplaced option",aOptIDs[id]);
					break;
				case NEW_MATRIX_SHUTTER:
					if(curMode == AS_INIT_LOC_DATA){
						curMode = AS_INIT_SHUTTER;

						t_id = script -> read_idata();
						sz = script -> read_idata();
						locSh = new aciLocationShutterInfo;

						if(t_id == 1) num = locData -> numMatrixShutters1;
						else num = locData -> numMatrixShutters2;

						if(sz < 0 || sz >= num)
							ErrH.Abort("Bad shutter index...");

						if(t_id == 1)
							locData -> MatrixShutters1[sz] = locSh;
						else
							locData -> MatrixShutters2[sz] = locSh;
					}
					else
						handle_error("Misplaced option",aOptIDs[id]);
					break;
				case INIT_STARTUP_TIME:
					aciML_D -> startup_timer = script -> read_idata();
					break;
				case SET_UPMENU_FLAG:
					upMenuFlag = 1;
					break;
				case INIT_SHUTDOWN_TIME:
					aciML_D -> shutdown_timer = script -> read_idata();
					break;
				case NEW_ML_EVENT_COMMAND:
					if(curMode != AML_INIT_EVENT)
						handle_error("Misplaced option",aOptIDs[id]);
					mlEvComm = new aciML_EventCommand;
					curMode = AML_INIT_EVENT_COMMAND;
					break;
				case NEW_ML_EVENT:
					if(curMode != AML_INIT_DATA)
						handle_error("Misplaced option",aOptIDs[id]);
					mlEv = new aciML_Event;
					curMode = AML_INIT_EVENT;
					break;
				case NEW_ML_DATA:
					if(curMode != AML_INIT_DATA_SET)
						handle_error("Misplaced option",aOptIDs[id]);
					mlData = new aciML_Data;
					curMode = AML_INIT_DATA;
					break;
				case NEW_ML_EVENT_SEQ:
					if(curMode != AML_INIT_DATA_SET)
						handle_error("Misplaced option",aOptIDs[id]);
					mlEvSeq = new aciML_EventSeq;
					mlEvSeq -> ID = script -> read_idata();
					mlEvSeq -> size = script -> read_idata();
					mlEvSeq -> alloc_mem(mlEvSeq -> size);
					curMode = AML_INIT_EVENT_SEQ;
					break;
				case NEW_BITMAP_MENU:
					if(curMode != AS_NONE)
						handle_error("Misplaced option",aOptIDs[id]);
					aciBM = new aciBitmapMenu;
					curMode = BM_INIT_MENU;
					break;
				case NEW_BITMAP_MENU_ITEM:
					if(curMode != BM_INIT_MENU)
						handle_error("Misplaced option",aOptIDs[id]);
					aciBM_it = new aciBitmapMenuItem;
					curMode = BM_INIT_MENU_ITEM;
					break;
				case NEW_ML_DATA_SET:
					if(curMode != AS_NONE)
						handle_error("Misplaced option",aOptIDs[id]);
					mlDataSet = new aciML_DataSet;
					curMode = AML_INIT_DATA_SET;
					break;
				case NEW_ML_ITEM_DATA:
					if(curMode != AML_INIT_DATA_SET)
						handle_error("Misplaced option",aOptIDs[id]);
					mlItm = new aciML_ItemData;

					mlItm -> ItemID = script -> read_idata();
					mlItm -> NullLevel = script -> read_idata();
					script -> read_pdata(&mlItm -> frameName,1);
					mlDataSet -> add_item(mlItm);
					break;
				case INIT_ML_EVENT_KEY_CODE:
					if(curMode == AML_INIT_EVENT){
						mlEv -> keys -> add_key(script -> read_idata());
					}
					else
						handle_error("Misplaced option",aOptIDs[id]);
					break;
				case SET_RND_VALUE:
					if(curMode == AML_INIT_EVENT){
						mlEv -> rndValue = script -> read_idata();
					}
					else
						handle_error("Misplaced option",aOptIDs[id]);
					break;
				case SET_SPEECH_CHANNEL:
					if(curMode == AML_INIT_DATA_SET){
						mlDataSet -> SpeechChannel = script -> read_idata();
					}
					else
						handle_error("Misplaced option",aOptIDs[id]);
					break;
				case SET_FRAME_CHECK_FLAG:
					if(curMode == AML_INIT_DATA){
						mlData -> flags |= AML_FRAME_CHECK;
					}
					else
						handle_error("Misplaced option",aOptIDs[id]);
					break;
				case SET_SPEECH_LEVEL:
					if(curMode == AML_INIT_DATA_SET){
						t_id = script -> read_idata();
						mlDataSet -> SpeechPriority[t_id] = script -> read_idata();
					}
					else
						handle_error("Misplaced option",aOptIDs[id]);
					break;
				case SET_ML_EVENT_SEQUENCE:
					if(curMode == AML_INIT_EVENT){
						mlEv -> flags |= AML_SEQUENCE_EVENT;
					}
					else {
						if(curMode == AML_INIT_EVENT_SEQ){
							for(t_id = 0; t_id < mlEvSeq -> size; t_id ++){
								mlEvSeq -> SeqIDs[t_id] = script -> read_idata();
								mlEvSeq -> SeqModes[t_id] = script -> read_idata();
							}
						}
						else
							handle_error("Misplaced option",aOptIDs[id]);
					}
					break;
				case INIT_ML_EVENT_CODE:
					if(curMode == AML_INIT_EVENT_COMMAND){
						mlEvComm -> code = script -> read_idata();
						mlEvComm -> data0 = script -> read_idata();
						mlEvComm -> data1 = script -> read_idata();
					}
					else
						handle_error("Misplaced option",aOptIDs[id]);
					break;
				case INIT_ML_EVENT_STARTUP:
					if(curMode == AML_INIT_EVENT){
						mlEv -> startupType = script -> read_idata();
					}
					else
						handle_error("Misplaced option",aOptIDs[id]);
					break;
				case SET_PRIORITY:
					if(curMode == AML_INIT_EVENT){
						mlEv -> priority = script -> read_idata();
					}
					else {
						if(curMode == AML_INIT_EVENT_SEQ){
							mlEvSeq -> dropLevel[0] = script -> read_idata();
						}
						else
							handle_error("Misplaced option",aOptIDs[id]);
					}
					break;
				case SET_NOT_LOCKED_FLAG:
					if(curMode == AML_INIT_EVENT){
						mlEv -> flags |= AML_IF_NOT_LOCKED;
					}
					else
						handle_error("Misplaced option",aOptIDs[id]);
					break;
				case SET_LOCKED_FLAG:
					if(curMode == AML_INIT_EVENT){
						mlEv -> flags |= AML_IF_LOCKED;
					}
					else
						handle_error("Misplaced option",aOptIDs[id]);
					break;
				case INIT_START_TIMER:
					if(curMode == AML_INIT_EVENT_COMMAND){
						mlEvComm -> start_timer = script -> read_idata();
					}
					else
						handle_error("Misplaced option",aOptIDs[id]);
					break;
				case INIT_CHANNEL_ID:
					if(curMode == AML_INIT_EVENT){
						mlEv -> ChannelID = script -> read_idata();
					}
					else {
						if(curMode == AML_INIT_DATA){
							mlData -> ChannelID = script -> read_idata();
						}
						else {
							if(curMode == AML_INIT_EVENT_SEQ){
								mlEvSeq -> ChannelID = script -> read_idata();
							}
							else
								handle_error("Misplaced option",aOptIDs[id]);
						}
					}
					break;
				case INIT_ML_EVENT_SDATA:
					if(curMode == AML_INIT_EVENT){
						mlEv -> data = script -> read_idata();
					}
					else
						handle_error("Misplaced option",aOptIDs[id]);
					break;
				case INIT_OFFS_X:
					if(curMode == AS_INIT_WORLD_MAP){
						t_id = script -> read_idata();
						wMap -> ShapeOffsX[t_id] = script -> read_idata();
					}
					else {
						if(curMode == AS_INIT_IBS){
							t_id = script -> read_idata();
							ibsObj -> indPosX[t_id] = script -> read_idata();
						}
						else
							handle_error("Misplaced option",aOptIDs[id]);
					}
					break;
				case SET_WORLD_NAME:
					if(curMode == AS_INIT_WORLD_DATA){
						script -> read_pdata(&wData -> name,1);
					}
					else {
						if(curMode == AML_INIT_DATA){
							script -> read_pdata(&mlData -> name,1);
						}
						else {
							if(curMode == AS_INIT_LOC_DATA){
								script -> read_pdata(&locData -> nameID,1);
								script -> read_pdata(&locData -> nameID2,1);
							}
							else {
								if(curMode == AS_INIT_SHUTTER){
									script -> read_pdata(&locSh -> name,1);
								}
								else
									handle_error("Misplaced option",aOptIDs[id]);
							}
						}
					}
					break;
				case ADD_FLAG:
					if(curMode == AS_INIT_WORLD_DATA){
						t_id = script -> read_idata();
						wData -> flags |= t_id;
					}
					else
						handle_error("Misplaced option",aOptIDs[id]);
					break;
				case ADD_LINK:
					if(curMode == AS_INIT_WORLD_DATA){
						t_id = script -> read_idata();
						wData -> links[t_id] = 1;
					}
					else
						handle_error("Misplaced option",aOptIDs[id]);
					break;
				case SET_MAX_STR:
					if(curMode == AS_INIT_INFO_PANEL){
						iPl -> MaxStr = script -> read_idata();
					}
					else
						handle_error("Misplaced option",aOptIDs[id]);
					break;
				case INIT_INFO_OFFS_X:
					if(curMode == AS_INIT_INFO_PANEL){
						iPl -> OffsX = script -> read_idata();
					}
					else
						handle_error("Misplaced option",aOptIDs[id]);
					break;
				case INIT_BACK_COL:
					if(curMode == AS_INIT_INFO_PANEL){
						iPl -> bCol = script -> read_idata();
					}
					else {
						if(curMode == AS_INIT_MENU){
							fnMnu -> bCol = script -> read_idata();
						}
						else
							handle_error("Misplaced option",aOptIDs[id]);
					}
					break;
				case SET_RANGE_FLAG:
					if(curMode == AS_INIT_INFO_PANEL){
						iPl -> flags |= IP_RANGE_FONT;
					}
					else {
						if(curMode == AS_INIT_MENU){
							fnMnu -> flags |= FM_RANGE_FONT;
						}
						else {
							if(curMode == AS_INIT_COUNTER){
								cP -> flags |= CP_RANGE_FONT;
							}
							else
								handle_error("Misplaced option",aOptIDs[id]);
						}
					}
					break;
				case SET_SUBMENU_FLAG:
					if(curMode == AS_INIT_MENU){
						fnMnu -> flags |= FM_SUBMENU;
					}
					else
						handle_error("Misplaced option",aOptIDs[id]);
					break;
				case SET_MAINMENU_FLAG:
					if(curMode == AS_INIT_MENU){
						fnMnu -> flags |= FM_MAIN_MENU;
					}
					else
						handle_error("Misplaced option",aOptIDs[id]);
					break;
				case INIT_INFO_OFFS_Y:
					if(curMode == AS_INIT_INFO_PANEL){
						iPl -> OffsY = script -> read_idata();
					}
					else
						handle_error("Misplaced option",aOptIDs[id]);
					break;
				case SET_NO_ALIGN:
					if(curMode == AS_INIT_INFO_PANEL){
						iPl -> flags |= IP_NO_ALIGN;
					}
					else
						handle_error("Misplaced option",aOptIDs[id]);
					break;
				case INIT_OFFS_Y:
					if(curMode == AS_INIT_WORLD_MAP){
						t_id = script -> read_idata();
						wMap -> ShapeOffsY[t_id] = script -> read_idata();
					}
					else {
						if(curMode == AS_INIT_IBS){
							t_id = script -> read_idata();
							ibsObj -> indPosY[t_id] = script -> read_idata();
						}
						else
							handle_error("Misplaced option",aOptIDs[id]);
					}
					break;
				case NEW_WORLD_MAP:
					if(curMode != AS_NONE)
						handle_error("Misplaced option",aOptIDs[id]);

					wMap = new aciWorldMap;
					curMode = AS_INIT_WORLD_MAP;
					break;
				case NEW_WORLD_DATA:
					if(curMode != AS_INIT_WORLD_MAP)
						handle_error("Misplaced option",aOptIDs[id]);

					wData = new aciWorldInfo;
					wData -> ID = script -> read_idata();
					wData -> letter = script -> read_idata();
					wData -> shape_id = script -> read_idata();
					curMode = AS_INIT_WORLD_DATA;
					break;
				case TOGGLE_ISCREEN_MODE:
					iScreenFlag = 1;
					break;
				case I_END_BLOCK:
					end_block();
					break;
				case INIT_CELL_SIZE:
					if(iScreenFlag)
						iCellSize = script -> read_idata();
					else
						aCellSize = script -> read_idata();
					break;
				case NEW_INV_MATRIX:
					if(curMode != AS_NONE)
						handle_error("Misplaced option",aOptIDs[id]);
					invMat = new invMatrix;
					curMode = AS_INIT_MATRIX;

					invMat -> internalID = script -> read_idata();
					invMat -> type = script -> read_idata();
					break;
				case NEW_INFO_PANEL:
					if(curMode != AS_NONE)
						handle_error("Misplaced option",aOptIDs[id]);
					iPl = new InfoPanel;
					curMode = AS_INIT_INFO_PANEL;
					break;
				case NEW_IBS:
					if(curMode != AS_NONE)
						handle_error("Misplaced option",aOptIDs[id]);
					ibsObj = new ibsObject;
					curMode = AS_INIT_IBS;

					ibsObj -> ID = script -> read_idata();
					break;
				case NEW_IND:
					if(curMode != AS_NONE)
						handle_error("Misplaced option",aOptIDs[id]);
					aInd = new aIndData;
					curMode = AS_INIT_IND;

					aInd -> ID = script -> read_idata();
					break;
				case NEW_BML:
					if(curMode != AS_NONE)
						handle_error("Misplaced option",aOptIDs[id]);
					bmlObj = new bmlObject;
					bmlObj -> flags |= BMP_FLAG;
					curMode = AS_INIT_BML;

					bmlObj -> ID = script -> read_idata();
					break;
				case NEW_INV_ITEM:
					if(curMode != AS_NONE)
						handle_error("Misplaced option",aOptIDs[id]);
					invItm = new invItem;
					curMode = AS_INIT_ITEM;

					script -> prepare_pdata();
					invItm -> init_name(script -> get_conv_ptr());
					break;
				case NEW_COUNTER:
					if(curMode != AS_NONE)
						handle_error("Misplaced option",aOptIDs[id]);

					cP = new CounterPanel;
					cP -> type = script -> read_option(0) - NEW_COUNTER - 1;

					curMode = AS_INIT_COUNTER;
					break;
				case NEW_BUTTON:
					if(curMode != AS_NONE)
						handle_error("Misplaced option",aOptIDs[id]);
					aBt = new aButton;

					aBt -> type = script -> read_option(0) - NEW_BUTTON - 1;

					curMode = AS_INIT_BUTTON;
					break;
				case NEW_MENU:
					if(curMode != AS_NONE && curMode != AS_INIT_ITEM)
						handle_error("Misplaced option",aOptIDs[id]);
					fnMnu = new fncMenu;
					if(curMode == AS_INIT_ITEM)
						fnMnu -> flags |= FM_ITEM_MENU;
					curMode = AS_INIT_MENU;
					break;
				case NEW_MENU_ITEM:
					if(curMode != AS_INIT_MENU)
						handle_error("Misplaced option",aOptIDs[id]);
					fnMnuItm = new fncMenuItem;
					curMode = AS_INIT_MENU_ITEM;
					break;
				case INIT_NUMVALS:
					if(curMode == AS_INIT_IND){
						aInd -> NumVals = script -> read_idata();
						aInd -> alloc_mem();
					}
					else
						handle_error("Misplaced option",aOptIDs[id]);
					break;
				case INIT_X:
					if(curMode == AS_INIT_MATRIX){
						invMat -> ScreenX = script -> read_idata();
					}
					else {
						if(curMode == AS_INIT_MENU){
							fnMnu -> PosX = script -> read_idata();
						}
						else {
							if(curMode == AS_INIT_BUTTON){
								aBt -> PosX = script -> read_idata();
							}
							else {
								if(curMode == AS_INIT_IND){
									aInd -> PosX = script -> read_idata();
									aInd -> dX = aInd -> PosX;
								}
								else {
									if(curMode == AS_INIT_INFO_PANEL){
										iPl -> PosX = script -> read_idata();
									}
									else {
										if(curMode == AS_INIT_COUNTER){
											cP -> PosX = script -> read_idata();
										}
										else {
											if(curMode == AS_INIT_WORLD_DATA){
												wData -> PosX = script -> read_idata();
											}
											else {
												if(curMode == BM_INIT_MENU_ITEM){
													aciBM_it -> PosX = script -> read_idata();
												}
												else
													handle_error("Misplaced option",aOptIDs[id]);
											}
										}
									}
								}
							}
						}
					}
					break;
				case INIT_Y:
					if(curMode == AS_INIT_MATRIX){
						invMat -> ScreenY = script -> read_idata();
					}
					else {
						if(curMode == AS_INIT_MENU){
							fnMnu -> PosY = script -> read_idata();
						}
						else {
							if(curMode == AS_INIT_BUTTON){
								aBt -> PosY = script -> read_idata();
							}
							else {
								if(curMode == AS_INIT_IND){
									aInd -> PosY = script -> read_idata();
									aInd -> dY = aInd -> PosY;
								}
								else {
									if(curMode == AS_INIT_INFO_PANEL){
										iPl -> PosY = script -> read_idata();
									}
									else {
										if(curMode == AS_INIT_COUNTER){
											cP -> PosY = script -> read_idata();
										}
										else {
											if(curMode == AS_INIT_WORLD_DATA){
												wData -> PosY = script -> read_idata();
											}
											else {
												if(curMode == BM_INIT_MENU_ITEM){
													aciBM_it -> PosY = script -> read_idata();
												}
												else
													handle_error("Misplaced option",aOptIDs[id]);
											}
										}
									}
								}
							}
						}
					}
					break;
				case INIT_MSX:
					if(curMode == AS_INIT_MATRIX){
						invMat -> SizeX = script -> read_idata();
					}
					else {
						handle_error("Misplaced option",aOptIDs[id]);
					}
					break;
				case SET_RAFFA_FLAG:
					if(curMode == AS_INIT_MATRIX){
						invMat -> flags |= IM_RAFFA;
					}
					else {
						handle_error("Misplaced option",aOptIDs[id]);
					}
					break;
				case SET_NUM_AVI_ID:
					if(curMode == AS_INIT_MATRIX){
						invMat -> numAviIDs = script -> read_idata();
						if(!invMat -> numAviIDs) handle_error("Bad invMatrix::numAviIDs");
						invMat -> avi_ids = new char*[invMat -> numAviIDs];
						for(i = 0; i < invMat -> numAviIDs; i ++)
							invMat -> avi_ids[i] = NULL;
					}
					else {
						if(curMode == AS_INIT_ITEM){
							invItm -> numAviIDs = script -> read_idata();
							if(!invItm -> numAviIDs) handle_error("Bad invItem::numAviIDs");
							invItm -> avi_ids = new char*[invItm -> numAviIDs];
							for(i = 0; i < invItm -> numAviIDs; i ++)
								invItm -> avi_ids[i] = NULL;
						}
						else {
							handle_error("Misplaced option",aOptIDs[id]);
						}
					}
					break;
				case SET_ESCAVE_FLAG:
					if(curMode == AS_INIT_ITEM){
						invItm -> flags |= INV_ITEM_SHOW_ESCAVE;
					}
					else {
						handle_error("Misplaced option",aOptIDs[id]);
					}
					break;
				case SET_SHOW_LOAD:
					if(curMode == AS_INIT_ITEM){
						invItm -> flags |= INV_ITEM_SHOW_LOAD;
					}
					else {
						handle_error("Misplaced option",aOptIDs[id]);
					}
					break;
				case SET_TEMPLATE:
					if(curMode == AS_INIT_ITEM){
						script -> prepare_pdata();
						if(strcasecmp(script -> get_conv_ptr(),"NONE"))
							invItm -> set_template(script -> get_conv_ptr());
					}
					else {
						handle_error("Misplaced option",aOptIDs[id]);
					}
					break;
				case SET_ACTIVATE_FLAG:
					if(curMode == AS_INIT_ITEM){
						invItm -> flags |= INV_ITEM_NO_ACTIVATE;
					}
					else {
						handle_error("Misplaced option",aOptIDs[id]);
					}
					break;
				case INIT_PART_DATA:
					if(curMode == AS_INIT_ITEM){
						invItm -> partData = new aciMechosPartInfo;

						invItm -> partData -> baseID[0] = script -> read_idata();
						invItm -> partData -> targetID[0] = script -> read_idata();

						invItm -> partData -> baseID[1] = script -> read_idata();
						invItm -> partData -> targetID[1] = script -> read_idata();
					}
					else {
						handle_error("Misplaced option",aOptIDs[id]);
					}
					break;
				case SET_NO_SHOW_LOAD:
					if(curMode == AS_INIT_ITEM){
						invItm -> flags &= ~INV_ITEM_SHOW_LOAD;
					}
					else {
						handle_error("Misplaced option",aOptIDs[id]);
					}
					break;
				case SET_NOT_COMPLETED_FLAG:
					if(curMode == AS_INIT_MATRIX){
						invMat -> flags |= IM_NOT_COMPLETE;
					}
					else {
						handle_error("Misplaced option",aOptIDs[id]);
					}
					break;
				case INIT_MSY:
					if(curMode == AS_INIT_MATRIX){
						invMat -> SizeY = script -> read_idata();
					}
					else {
						handle_error("Misplaced option",aOptIDs[id]);
					}
					break;
				case INIT_RADIUS:
					if(curMode == AS_INIT_IND){
						script -> read_idata();
					}
					else {
						handle_error("Misplaced option",aOptIDs[id]);
					}
					break;
				case INIT_CORNER:
					if(curMode == AS_INIT_IND){
						aInd -> CornerNum = script -> read_idata();
					}
					else {
						handle_error("Misplaced option",aOptIDs[id]);
					}
					break;
				case INIT_IND_TYPE:
					if(curMode == AS_INIT_IND){
						aInd -> type = script -> read_idata();
					}
					else {
						handle_error("Misplaced option",aOptIDs[id]);
					}
					break;
				case INIT_SX:
					if(curMode == AS_INIT_MATRIX){
						invMat -> ScreenSizeX = script -> read_idata();
					}
					else {
						if(curMode == AS_INIT_ITEM){
							t_id = script -> read_idata();
						}
						else {
							if(curMode == AS_INIT_MENU){
								fnMnu -> SizeX = script -> read_idata();
							}
							else {
								if(curMode == AS_INIT_BML){
									bmlObj -> SizeX = script -> read_idata();
								}
								else {
									if(curMode == AS_INIT_INFO_PANEL){
										iPl -> SizeX = script -> read_idata();
									}
									else {
										if(curMode == AS_INIT_WORLD_MAP){
											wMap -> SizeX = script -> read_idata();
										}
										else {
											if(curMode == AS_INIT_COUNTER){
												cP -> SizeX = script -> read_idata();
											}
											else {
												if(curMode == BM_INIT_MENU){
													aciBM -> SizeX = script -> read_idata();
												}
												else
													handle_error("Misplaced option",aOptIDs[id]);
											}
										}
									}
								}
							}
						}
					}
					break;
				case INIT_SY:
					if(curMode == AS_INIT_MATRIX){
						invMat -> ScreenSizeY = script -> read_idata();
					}
					else {
						if(curMode == AS_INIT_ITEM){
							t_id = script -> read_idata();
						}
						else {
							if(curMode == AS_INIT_MENU){
								fnMnu -> SizeY = script -> read_idata();
							}
							else {
								if(curMode == AS_INIT_BML){
									bmlObj -> SizeY = script -> read_idata();
								}
								else {
									if(curMode == AS_INIT_INFO_PANEL){
										iPl -> SizeY = script -> read_idata();
									}
									else {
										if(curMode == AS_INIT_WORLD_MAP){
											wMap -> SizeY = script -> read_idata();
										}
										else {
											if(curMode == AS_INIT_COUNTER){
												cP -> SizeY = script -> read_idata();
											}
											else {
												if(curMode == BM_INIT_MENU){
													aciBM -> SizeY = script -> read_idata();
												}
												else
													handle_error("Misplaced option",aOptIDs[id]);
											}
										}
									}
								}
							}
						}
					}
					break;
				case INIT_SLOT_TYPE:
					if(curMode == AS_INIT_ITEM){
						invItm -> slotType = script -> read_idata();
					}
					else {
						handle_error("Misplaced option",aOptIDs[id]);
					}
					break;
				case INIT_AVI_ID:
					if(curMode == AS_INIT_ITEM){
						if(!invItm -> numAviIDs) handle_error("Null item numAviIDs...");
						t_id = script -> read_idata();
						if(t_id >= invItm -> numAviIDs || invItm -> avi_ids[t_id])
							handle_error("Bad AVI_ID number");
						script -> prepare_pdata();
						invItm -> init_avi_id(script -> get_pdata_ptr(),t_id);
					}
					else {
						if(curMode == AS_INIT_MATRIX){
							if(!invMat -> numAviIDs) handle_error("Null matrix numAviIDs...");
							t_id = script -> read_idata();
							if(t_id >= invMat -> numAviIDs || invMat -> avi_ids[t_id])
								handle_error("Bad AVI_ID number");
							script -> prepare_pdata();
							invMat -> init_avi_id(script -> get_pdata_ptr(),t_id);
						}
						else {
							handle_error("Misplaced option",aOptIDs[id]);
						}
					}
					break;
				case INIT_COMMENTS:
					if(curMode == AS_INIT_ITEM){
						invItm -> numComments = script -> read_idata();
						if(invItm -> numComments){
							invItm -> comments = new char*[invItm -> numComments];
							for(i = 0; i < invItm -> numComments; i ++){
								script -> read_pdata(&invItm -> comments[i],1);
							}
						}
					}
					else {
						handle_error("Misplaced option",aOptIDs[id]);
					}
					break;
				case INIT_NUM_INDEX:
					if(curMode == AS_INIT_ITEM){
						invItm -> NumIndex = script -> read_idata();
					}
					else {
						if(curMode == AS_INIT_LOC_DATA){
							t_id = script -> read_idata();
							if(t_id < 0 || t_id >= ACI_LOCATION_INDEX_SIZE)
								ErrH.Abort("Bad aciLocationInfo::numIndex ID...");
							locData -> numIndex[t_id] = script -> read_idata();
						}
						else
							handle_error("Misplaced option",aOptIDs[id]);
						}
					break;
				case INIT_SHAPE_LEN:
					if(curMode == AS_INIT_ITEM){
						invItm -> ShapeLen = script -> read_idata();
					}
					else {
						handle_error("Misplaced option",aOptIDs[id]);
					}
					break;
				case INIT_SHAPE:
					if(curMode == AS_INIT_ITEM){
						curMode = AS_INIT_SHAPE_OFFS;
						load_item_shape();
					}
					else {
						if(curMode == AS_INIT_WORLD_MAP){
							t_id = script -> read_idata();
							script -> read_pdata(&wMap -> shape_files[t_id],1);
						}
						else
							handle_error("Misplaced option",aOptIDs[id]);
					}
					break;
				case INIT_MATRIX_EL:
					if(curMode == AS_INIT_MATRIX){
						curMode = AS_INIT_MATRIX_EL;
						load_matrix();
					}
					else {
						handle_error("Misplaced option",aOptIDs[id]);
					}
					break;
				case INIT_SLOT_NUMS:
					if(curMode == AS_INIT_MATRIX){
						curMode = AS_INIT_MATRIX_EL;
						load_slot_nums();
					}
					else {
						handle_error("Misplaced option",aOptIDs[id]);
					}
					break;
				case INIT_SLOT_TYPES:
					if(curMode == AS_INIT_MATRIX){
						curMode = AS_INIT_MATRIX_EL;
						load_slot_types();
					}
					else {
						handle_error("Misplaced option",aOptIDs[id]);
					}
					break;
				case INIT_CLASS:
					if(curMode == AS_INIT_ITEM){
						t_id = script -> read_option(0) - INIT_CLASS - 1;
						invItm -> classID = t_id;
					}
					else
						handle_error("Misplaced option",aOptIDs[id]);
					break;
				case INIT_FNAME:
					if(curMode == AS_INIT_ITEM){
						script -> prepare_pdata();
						invItm -> init_fname(script -> get_conv_ptr());
					}
					else
						handle_error("Misplaced option",aOptIDs[id]);
					break;
				case INIT_FILE:
					script -> prepare_pdata();
					if(curMode == AS_INIT_IBS){
						ibsObj -> set_name(script -> get_conv_ptr());
					}
					else {
						if(curMode == AS_INIT_BML){
							bmlObj -> init_name(script -> get_conv_ptr());
						}
						else
							handle_error("Misplaced option",aOptIDs[id]);
					}
					break;
				case INIT_BGROUND:
					if(curMode == AS_INIT_IBS){
						ibsObj -> backObjIDs.push_back(script -> read_idata());
					}
					else
						handle_error("Misplaced option",aOptIDs[id]);
					break;
				case INIT_SEQ_NAME:
					script -> prepare_pdata();
					if(curMode == AS_INIT_BUTTON){
						aBt -> set_fname(script -> get_conv_ptr());
					}
					else
						handle_error("Misplaced option",aOptIDs[id]);
					break;
				case SET_CONTROL_ID:
					if(curMode == AS_INIT_BUTTON){
						aBt -> ControlID = script -> read_idata();
					}
					else
						handle_error("Misplaced option",aOptIDs[id]);
					break;
				case INIT_EVENT_CODE:
					if(curMode == AS_INIT_BUTTON){
						t_id = script -> read_idata();
						init_event_code(t_id);
					}
					else {
						if(curMode == AS_INIT_ITEM){
							invItm -> EvCode = script -> read_idata() + ACI_MAX_EVENT;
						}
						else {
							if(curMode == AS_INIT_MENU_ITEM){
								fnMnuItm -> eventPtr = new actEvent;
								fnMnuItm -> eventPtr -> code = script -> read_idata() + ACI_MAX_EVENT;
								fnMnuItm -> eventPtr -> data = script -> read_idata();
							}
							else
								handle_error("Misplaced option",aOptIDs[id]);
						}
					}
					break;
				case SET_UNPRESS:
					if(curMode == AS_INIT_BUTTON){
						aBt -> flags |= B_UNPRESS;
					}
					else
						handle_error("Misplaced option",aOptIDs[id]);
					break;
				case INIT_ACTIVE_TIME:
					if(curMode == AS_INIT_BUTTON){
						aBt -> activeCount = script -> read_idata();
					}
					else {
						if(curMode == AS_INIT_MENU){
							fnMnu -> activeCount = script -> read_idata();
						}
						else {
							if(curMode == AML_INIT_EVENT){
								mlEv -> active_time = script -> read_idata();
							}
							else {
								if(curMode == BM_INIT_MENU){
									aciBM -> activeCount = script -> read_idata();
								}
								else
									handle_error("Misplaced option",aOptIDs[id]);
							}
						}
					}
					break;
				case INIT_FNC_CODE:
					if(curMode == AS_INIT_MENU_ITEM){
						fnMnuItm -> fnc_code = script -> read_idata();
					}
					else
						handle_error("Misplaced option",aOptIDs[id]);
					break;
				case SET_NO_DELETE_FLAG:
					if(curMode == AS_INIT_MENU_ITEM){
						fnMnuItm -> flags |= FM_NO_DELETE;
					}
					else
						handle_error("Misplaced option",aOptIDs[id]);
					break;
				case SET_SUBMENU_ID:
					if(curMode == AS_INIT_MENU_ITEM){
						fnMnuItm -> submenuID = script -> read_idata();
						fnMnuItm -> flags |= FM_SUBMENU_ITEM;
					}
					else
						handle_error("Misplaced option",aOptIDs[id]);
					break;
				case SET_BSUBMENU_ID:
					if(curMode == AS_INIT_MENU_ITEM){
						fnMnuItm -> submenuID = script -> read_idata();
						fnMnuItm -> flags |= FM_BSUBMENU_ITEM;
					}
					else
						handle_error("Misplaced option",aOptIDs[id]);
					break;
				case SET_NUM_V_ITEMS:
					if(curMode == AS_INIT_MENU){
						fnMnu -> VItems = script -> read_idata();
					}
					else
						handle_error("Misplaced option",aOptIDs[id]);
					break;
				case INIT_SPACE:
					if(curMode == AS_INIT_MENU){
						fnMnu -> vSpace = script -> read_idata();
					}
					else {
						if(curMode == AS_INIT_MENU_ITEM){
							fnMnuItm -> space = script -> read_idata();
						}
						else
							handle_error("Misplaced option",aOptIDs[id]);
					}
					break;
				case SET_NO_DEACTIVATE_FLAG:
					if(curMode == AS_INIT_MENU){
						fnMnu -> flags |= FM_NO_DEACTIVATE;
					}
					else {
						if(curMode == AML_INIT_EVENT){
							mlEv -> flags |= AML_NO_DEACTIVATE;
						}
						else
							handle_error("Misplaced option",aOptIDs[id]);
					}
					break;
				case SET_LOCATION_MENU_FLAG:
					if(curMode == AS_INIT_MENU){
						fnMnu -> flags |= FM_LOCATION_MENU;
					}
					else {
						handle_error("Misplaced option",aOptIDs[id]);
					}
					break;
				case SET_BML_NAME:
					if(curMode == AS_INIT_MENU){
						script -> read_pdata(&fnMnu -> bml_name,1);
					}
					else {
						if(curMode == AS_INIT_IND){
							script -> prepare_pdata();

							if(aInd -> bml)
								handle_error("aIndData::bml already inited");

							aInd -> bml = new bmlObject;
							aInd -> bml -> init_name(script -> get_conv_ptr());
						}
						else {
							if(curMode == AS_INIT_INFO_PANEL){
								script -> read_pdata(&iPl -> bml_name,1);
							}
							else {
								if(curMode == BM_INIT_MENU_ITEM){
									script -> read_pdata(&aciBM_it -> fname,1);
								}
								else
									handle_error("Misplaced option",aOptIDs[id]);
							}
						}
					}
					break;
				case INIT_BACK_BML:
					if(curMode == AS_INIT_MATRIX){
						script -> prepare_pdata();

						if(invMat -> back)
							handle_error("invMatrix::back already inited");

						invMat -> back = new bmlObject;
						invMat -> back -> init_name(script -> get_conv_ptr());
					}
					else
						handle_error("Misplaced option",aOptIDs[id]);
					break;
				case SET_IBS_NAME:
					if(curMode == AS_INIT_MENU){
						script -> read_pdata(&fnMnu -> ibs_name,1);
					}
					else {
						if(curMode == AS_INIT_INFO_PANEL){
							script -> read_pdata(&iPl -> ibs_name,1);
						}
						else {
							if(curMode == AS_INIT_COUNTER){
								script -> read_pdata(&cP -> ibs_name,1);
							}
							else
								handle_error("Misplaced option",aOptIDs[id]);
						}
					}
					break;
				case SET_MAP_NAME:
					if(curMode == AS_INIT_LOC_DATA){
						script -> read_pdata(&locData -> mapName,1);
					}
					else {
						t_id = script -> read_idata();
						script -> read_pdata(&aScrDisp -> map_names[t_id],1);
					}
					break;
				case SET_SAVE_SCREEN_ID:
					if(curMode == AS_INIT_LOC_DATA){
						locData -> SaveScreenID = script -> read_idata();
					}
					else {
						handle_error("Misplaced option",aOptIDs[id]);
					}
					break;
				case SET_WORLD_ID:
					if(curMode == AS_INIT_LOC_DATA){
						locData -> WorldID = script -> read_idata();
					}
					else {
						handle_error("Misplaced option",aOptIDs[id]);
					}
					break;
				case SET_EXCLUDE:
					if(curMode == AS_INIT_LOC_DATA){
						t_id = script -> read_idata();
						locData -> ExcludeItems[t_id] = 1;
					}
					else {
						handle_error("Misplaced option",aOptIDs[id]);
					}
					break;
				case INIT_SCREEN_ID:
					if(curMode == AS_INIT_LOC_DATA){
						script -> read_pdata(&locData -> screenID,1);
					}
					else {
						handle_error("Misplaced option",aOptIDs[id]);
					}
					break;
				case INIT_PAL_NAME:
					if(curMode == AS_INIT_LOC_DATA){
						script -> read_pdata(&locData -> palName,1);
					}
					else {
						handle_error("Misplaced option",aOptIDs[id]);
					}
					break;
				case INIT_CUR_FNC:
					if(curMode == AS_INIT_MENU){
						fnMnu -> curFunction = script -> read_idata();
					}
					else {
						handle_error("Misplaced option",aOptIDs[id]);
					}
					break;
				case INIT_FONT:
					if(curMode == AS_INIT_MENU_ITEM){
						fnMnuItm -> font = script -> read_idata();
					}
					else {
						if(curMode == AS_INIT_INFO_PANEL){
							iPl -> font = script -> read_idata();
						}
						else {
							if(curMode == AS_INIT_WORLD_DATA){
								wData -> font = script -> read_idata();
							}
							else {
								if(curMode == AS_INIT_COUNTER){
									cP -> font = script -> read_idata();
								}
								else {
									if(curMode == AS_INIT_IBS){
										ibsObj -> fontID = script -> read_idata();
									}
									else
										handle_error("Misplaced option",aOptIDs[id]);
								}
							}
						}
					}
					break;
				case INIT_VSPACE:
					if(curMode == AS_INIT_INFO_PANEL){
						iPl -> vSpace = script -> read_idata();
					}
					else
						handle_error("Misplaced option",aOptIDs[id]);
					break;
				case INIT_HSPACE:
					if(curMode == AS_INIT_INFO_PANEL){
						iPl -> hSpace = script -> read_idata();
					}
					else
						handle_error("Misplaced option",aOptIDs[id]);
					break;
				case INIT_STRING:
					if(curMode == AS_INIT_MENU_ITEM){
						script -> prepare_pdata();
						fnMnuItm -> init_name(script -> get_conv_ptr());
					}
					else
						handle_error("Misplaced option",aOptIDs[id]);
					break;
				case INIT_SCANCODE:
					if(curMode == AS_INIT_MENU){
						fnMnu -> add_key(script -> read_key());
					}
					else {
						if(curMode == AS_INIT_MENU_ITEM){
							fnMnuItm -> add_key(script -> read_key());
						}
						else {
							if(curMode == AS_INIT_BUTTON){
								aBt -> add_key(script -> read_key());
							}
							else {
								if(curMode == AML_INIT_EVENT){
									mlEv -> add_key(script -> read_key());
								}
								else
									handle_error("Misplaced option",aOptIDs[id]);
							}
						}
					}
					break;
				case INIT_MENU_TYPE:
					if(curMode == AS_INIT_MENU){
						fnMnu -> type = script -> read_idata();
					}
					else {
						handle_error("Misplaced option",aOptIDs[id]);
					}
					break;
				case INIT_UP_KEY:
					if(curMode == AS_INIT_MENU){
						fnMnu -> up_key -> add_key(script -> read_key());
					}
					else {
						handle_error("Misplaced option",aOptIDs[id]);
					}
					break;
				case INIT_DOWN_KEY:
					if(curMode == AS_INIT_MENU){
						fnMnu -> down_key -> add_key(script -> read_key());
					}
					else {
						handle_error("Misplaced option",aOptIDs[id]);
					}
					break;
				case INIT_TRIGGER:
					if(curMode == AS_INIT_MENU){
						fnMnu -> trigger_code = script -> read_idata();
					}
					else {
						handle_error("Misplaced option",aOptIDs[id]);
					}
					break;
				case INIT_ID:
					if(curMode == AS_INIT_BUTTON){
						aBt -> ID = script -> read_idata();
					}
					else {
						if(curMode == AS_INIT_ITEM){
							invItm -> ID = script -> read_idata();
						}
						else {
							if(curMode == AS_INIT_LOC_DATA){
								t_id = script -> read_idata();
								script -> read_pdata(&locData -> objIDs[t_id],1);
							}
							else {
								if(curMode == AS_INIT_MATRIX){
									script -> read_pdata(&invMat -> mech_name,1);
								}
								else {
									if(curMode == AS_INIT_INFO_PANEL){
										iPl -> type = script -> read_idata();
									}
									else {
										if(curMode == AS_INIT_COUNTER){
											cP -> ID = script -> read_idata();
										}
										else {
											if(curMode == AML_INIT_DATA_SET){
												mlDataSet -> ID = script -> read_idata();
											}
											else {
												if(curMode == AML_INIT_DATA){
													mlData -> ID = script -> read_idata();
												}
												else {
													if(curMode == BM_INIT_MENU_ITEM){
														aciBM_it -> ID = script -> read_idata();
													}
													else {
														if(curMode == BM_INIT_MENU){
															aciBM -> ID = script -> read_idata();
														}
														else {
															if(curMode == AML_INIT_EVENT_SEQ){
																mlEvSeq -> add_id(script -> read_idata());
															}
															else
																handle_error("Misplaced option",aOptIDs[id]);
														}
													}
												}
											}
										}
									}
								}
							}
						}
					}
					break;
				case INIT_S_ID:
					if(curMode == AS_INIT_LOC_DATA){
						t_id = script -> read_idata();
						script -> read_pdata(&locData -> s_objIDs[t_id],1);
					}
					else {
						handle_error("Misplaced option",aOptIDs[id]);
					}
					break;
				case INIT_MODE_KEY:
					aScrDisp -> ModeObj -> add_key(script -> read_key());
					break;
				case INIT_INV_KEY:
					aScrDisp -> InvObj -> add_key(script -> read_key());
					break;
				case INIT_INFO_KEY:
					aScrDisp -> InfoObj -> add_key(script -> read_key());
					break;
				case SET_CURMATRIX:
					curMatrix = script -> read_idata();
					break;
				case INIT_CUR_IBS:
					aScrDisp -> curIbsID = script -> read_idata();
					break;
				case NEW_LOC_DATA:
					if(curMode != AS_NONE){
						handle_error("Misplaced option",aOptIDs[id]);
					}
					locData = new aciLocationInfo;
					locData -> ID = script -> read_idata();
					curMode = AS_INIT_LOC_DATA;
					break;
				case NEW_COL_SCHEME:
					if(curMode != AS_NONE){
						if(curMode == AS_INIT_LOC_DATA){
							locData -> numColorScheme = script -> read_idata();
						}
						else
							handle_error("Misplaced option",aOptIDs[id]);
					}
					curMode = AS_INIT_COLOR_SCHEME;

					t_id = script -> read_idata();
					curScheme = t_id;
					aciColorSchemes[t_id] = new unsigned char[aciColSchemeLen];
					memset(aciColorSchemes[t_id],0,aciColSchemeLen);
					break;
				case INIT_NUM_SCHEMES:
					aciNumColSchemes = script -> read_idata();
					aciColorSchemes = new unsigned char*[aciNumColSchemes];
					for(i = 0; i < aciNumColSchemes; i ++)
						aciColorSchemes[i] = NULL;
					break;
				case INIT_SCHEME_LEN:
					aciColSchemeLen = script -> read_idata();
					break;
				case INIT_COLOR:
					if(curMode == AS_INIT_COLOR_SCHEME){
						t_id = script -> read_idata();
						aciColorSchemes[curScheme][t_id] = script -> read_idata();
					}
					else {
						handle_error("Misplaced option",aOptIDs[id]);
					}
					break;
				case ANCHOR_RIGHT:
					switch (curMode) {
						case AS_INIT_MENU:
							fnMnu->anchor |= WIDGET_ANCHOR_RIGHT;
							break;
						case AS_INIT_INFO_PANEL:
							iPl->anchor |= WIDGET_ANCHOR_RIGHT;
							break;
						case AS_INIT_BUTTON:
							aBt->anchor |= WIDGET_ANCHOR_RIGHT;
							break;
						case AS_INIT_COUNTER:
							cP->anchor |= WIDGET_ANCHOR_RIGHT;
							break;
						case AS_INIT_MATRIX:
							invMat->anchor |= WIDGET_ANCHOR_RIGHT;
							break;
						case AS_INIT_BML:
							bmlObj->anchor |= WIDGET_ANCHOR_RIGHT;
							break;
						default:
							handle_error("Misplaced option", aOptIDs[id]);
					}

					break;
				case ANCHOR_BOTTOM:
					switch (curMode) {
						case AS_INIT_MENU:
							fnMnu->anchor |= WIDGET_ANCHOR_BOTTOM;
							break;
						case AS_INIT_INFO_PANEL:
							iPl->anchor |= WIDGET_ANCHOR_BOTTOM;
							break;
						case AS_INIT_BUTTON:
							aBt->anchor |= WIDGET_ANCHOR_BOTTOM;
							break;
						case AS_INIT_COUNTER:
							cP->anchor |= WIDGET_ANCHOR_BOTTOM;
							break;
						case AS_INIT_MATRIX:
							invMat->anchor |= WIDGET_ANCHOR_BOTTOM;
							break;
						case AS_INIT_BML:
							bmlObj->anchor |= WIDGET_ANCHOR_BOTTOM;
							break;
						default:
							handle_error("Misplaced option", aOptIDs[id]);
					}
					break;
			}
#ifndef _BINARY_SCRIPT_
		}
		else
			script -> next_ptr();
#endif
	}
#ifdef _SAVE_BINARY_SCRIPT_
	script -> close_bscript();
#endif
	delete script;
#ifndef _BINARY_SCRIPT_
	_sFREE_HEAP_(1500000,char);
#endif
}

int aGetOptionID(char* p)
{
	int i;
	if(!p) handle_error("Null ID pointer");
	for(i = 0; i < MAX_OPTION; i ++){
		if(!strcmp(p,aOptIDs[i]))
			return i;
	}
	return -1;
}

void load_matrix(void)
{
	int i,j,index = 0;
	invMat -> alloc_matrix();
	for(i = 0; i < invMat -> SizeY; i ++){
		for(j = 0; j < invMat -> SizeX; j ++){
			invMat -> matrix[index] -> type = script -> read_idata();
			index ++;
		}
	}
}

void load_slot_nums(void)
{
	int i,j,index = 0;
	for(i = 0; i < invMat -> SizeY; i ++){
		for(j = 0; j < invMat -> SizeX; j ++){
			invMat -> matrix[index] -> slotNumber = script -> read_idata() - 1;
			index ++;
		}
	}
}

void load_slot_types(void)
{
	int i,j,index = 0;
	for(i = 0; i < invMat -> SizeY; i ++){
		for(j = 0; j < invMat -> SizeX; j ++){
			invMat -> matrix[index] -> slotType = script -> read_idata();
			if(!invMat -> matrix[index] -> slotType)
				invMat -> matrix[index] -> slotType = -1;
			index ++;
		}
	}
}

void end_block(void)
{
	switch(curMode){
		case AS_NONE:
			if(!iScreenFlag)
				handle_error("Misplaced \"}\"");
			else
				iScreenFlag = 0;
			break;
		case AS_INIT_SHAPE_OFFS:
			curMode = AS_INIT_ITEM;
			break;
		case AS_INIT_ITEM:
			curMode = AS_NONE;
			if(iScreenFlag)
				aScrDisp -> add_iitem(invItm);
			else
				aScrDisp -> add_item(invItm);
			break;
		case AS_INIT_MATRIX:
			curMode = AS_NONE;
			if(iScreenFlag)
				aScrDisp -> add_imatrix(invMat);
			else
				aScrDisp -> add_matrix(invMat);
			break;
		case AS_INIT_MATRIX_EL:
			curMode = AS_INIT_MATRIX;
			break;
		case AS_INIT_MENU:
			if(!iScreenFlag){
				if(fnMnu -> flags & FM_ITEM_MENU){
					curMode = AS_INIT_ITEM;
					aScrDisp -> add_menu(fnMnu);
					invItm -> menu = (iListElement*)fnMnu;
				}
				else {
					curMode = AS_NONE;
					aScrDisp -> add_menu(fnMnu);
				}
			}
			else {
				if(fnMnu -> flags & FM_ITEM_MENU){
					curMode = AS_INIT_ITEM;
					invItm -> menu = (iListElement*)fnMnu;
				}
				else {
					curMode = AS_NONE;
					fnMnu -> flags |= FM_ISCREEN_MENU;
					aScrDisp -> add_imenu(fnMnu);
				}
			}
			break;
		case AS_INIT_MENU_ITEM:
			curMode = AS_INIT_MENU;
			if(upMenuFlag){
				fnMnu -> upMenuItem = fnMnuItm;
				upMenuFlag = 0;
			}
			else
				fnMnu -> add_item(fnMnuItm);
			break;
		case AS_INIT_BUTTON:
			curMode = AS_NONE;
			if(aBt -> type == INTERF_BUTTON)
				aScrDisp -> intButtons -> connect((iListElement*)aBt);
			if(aBt -> type == INV_BUTTON)
				aScrDisp -> invButtons -> connect((iListElement*)aBt);
			if(aBt -> type == INFO_BUTTON)
				aScrDisp -> infButtons -> connect((iListElement*)aBt);
			break;
		case AS_INIT_COUNTER:
			curMode = AS_NONE;
			if(iScreenFlag){
				aScrDisp -> i_Counters -> connect((iListElement*)cP);
			}
			else {
				if(cP -> type == CP_INT)
					aScrDisp -> intCounters -> connect((iListElement*)cP);
				if(cP -> type == CP_INV)
					aScrDisp -> invCounters -> connect((iListElement*)cP);
				if(cP -> type == CP_INF)
					aScrDisp -> infCounters -> connect((iListElement*)cP);
			}
			break;
		case AS_INIT_IBS:
			aScrDisp -> ibsList -> connect((iListElement*)ibsObj);
			curMode = AS_NONE;
			break;
		case AS_INIT_BML:
			aScrDisp -> backList -> connect((iListElement*)bmlObj);
			curMode = AS_NONE;
			break;
		case AS_INIT_IND:
			aScrDisp -> add_ind(aInd);
			curMode = AS_NONE;
			break;
		case AS_INIT_INFO_PANEL:
			if(iScreenFlag){
				if(!iPl -> type)
					aScrDisp -> iscr_iP = iPl;
				else {
					aScrDisp -> i_infoPanels -> connect((iListElement*)iPl);
				}
			}
			else {
				if(!iPl -> type)
					aScrDisp -> iP = iPl;
				else {
					aScrDisp -> infoPanels -> connect((iListElement*)iPl);
				}
			}
			curMode = AS_NONE;
			break;
		case AS_INIT_COLOR_SCHEME:
			curMode = AS_NONE;
			break;
		case AS_INIT_LOC_DATA:
			aScrDisp -> add_locdata(locData);
			curMode = AS_NONE;
			break;
		case AS_INIT_WORLD_MAP:
			aScrDisp -> wMap = wMap;
			curMode = AS_NONE;
			break;
		case AS_INIT_WORLD_DATA:
			wMap -> world_list -> connect((iListElement*)wData);
			wData -> owner = (iListElement*)wMap;
			curMode = AS_INIT_WORLD_MAP;
			break;
		case AML_INIT_DATA:
			mlDataSet -> add_data(mlData);
			curMode = AML_INIT_DATA_SET;
			break;
		case AML_INIT_DATA_SET:
			aciML_D -> add_data_set(mlDataSet);
			curMode = AS_NONE;
			break;
		case AML_INIT_EVENT:
			mlData -> add_event(mlEv);
			curMode = AML_INIT_DATA;
			break;
		case AML_INIT_EVENT_COMMAND:
			mlEv -> add_command(mlEvComm);
			curMode = AML_INIT_EVENT;
			break;
		case BM_INIT_MENU_ITEM:
			aciBM -> add_item(aciBM_it);
			curMode = BM_INIT_MENU;
			break;
		case BM_INIT_MENU:
			aScrDisp -> add_bmenu(aciBM);
			curMode = AS_NONE;
			break;
		case AS_INIT_SHUTTER:
			curMode = AS_INIT_LOC_DATA;
			locSh -> init();
			break;
		case AML_INIT_EVENT_SEQ:
			mlDataSet -> add_seq(mlEvSeq);
			curMode = AML_INIT_DATA_SET;
			break;
	}
}

void init_event_code(int cd)
{
	aBt -> eventCode = cd + ACI_MAX_EVENT;
	aBt -> eventData = script -> read_idata();
}

void load_item_shape(void)
{
	int i;
	invItm -> ShapeX = new int[invItm -> ShapeLen];
	invItm -> ShapeY = new int[invItm -> ShapeLen];
	for(i = 0; i < invItm -> ShapeLen; i ++){
		invItm -> ShapeX[i] = script -> read_idata();
		invItm -> ShapeY[i] = script -> read_idata();
	}
	invItm -> calc_shape_center();
}

