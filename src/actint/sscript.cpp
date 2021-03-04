
/* ---------------------------- INCLUDE SECTION ----------------------------- */

#include "../global.h"

#include "../iscreen/s_mem.h"
#include "../iscreen/iscript.h"

#include "aci_evnt.h"
#include "aci_scr.h"

/* ----------------------------- STRUCT SECTION ----------------------------- */
/* ----------------------------- EXTERN SECTION ----------------------------- */

extern ScriptFile* script;

/* --------------------------- PROTOTYPE SECTION ---------------------------- */

void acsParseScript(const char* fname,char* bname = NULL);
int acsGetOptionID(char* p);

void acsEndScriptBlock(void);

aciScreenResource* acsAllocResource(int type);
void acsFreeResource(aciScreenResource* p);

void handle_error(const char* prefix,const char* subj = NULL);

/* --------------------------- DEFINITION SECTION --------------------------- */

#define ACS_SCRIPT_SIZE 	400000

enum acsScriptModes
{
	ACS_INIT_RESOURCE = 1,
	ACS_INIT_COMMAND,
	ACS_INIT_EVENT,
	ACS_INIT_FRAME_SEQ,
	ACS_INIT_OBJECT,
	ACS_INIT_SCREEN,

	ACS_MAX_MODE
};

enum acsOptions
{
	ACS_END_BLOCK,
	ACS_END_STRING,

	ACS_NEW_RESOURCE,
	ACS_NEW_COMMAND,
	ACS_NEW_EVENT,
	ACS_NEW_FRAME_SEQ,
	ACS_NEW_OBJECT,
	ACS_NEW_SCREEN,

	// Resource options...
	ACS_SET_ID,
	ACS_SET_FNAME,
	ACS_SET_BUILD,

	ACS_SET_OFFS_X,
	ACS_SET_OFFS_Y,
	ACS_SET_SZ_X,
	ACS_SET_SZ_Y,

	// EventCommand options...
	ACS_SET_EVENT,
	ACS_SET_START_TIMER,

	// Event options...
	ACS_SET_MAX_TIMER,
	ACS_ADD_KEY,

	// FrameSequence options...
	ACS_SET_RESOURCE_ID,
	ACS_SET_SEQ,

	// Object options...
	ACS_SET_POS_X,
	ACS_SET_POS_Y,

	// Scroller options...
	ACS_SET_MAX_VALUE,
	ACS_SET_COLORS,
	ACS_SET_SECOND_RES,
	ACS_SET_ACTIVE_SX,
	ACS_SET_ACTIVE_SY,

	// InputField options...
	ACS_SET_STRING_LEN,
	ACS_SET_FONT,
	ACS_SET_IFONT,
	ACS_SET_SPACE,
	ACS_SET_CENTER,
	ACS_SET_COLOR,
	ACS_SET_STRING,

	// Screen options...
	ACS_SET_BACK_ID,


	MAX_OPTION
};

static const char* acsOptIDs[MAX_OPTION] =
{
	"}",
	"\n",

	"resource",
	"ev_comm",
	"event",
	"frame_seq",
	"object",
	"screen",

	// Resource options...
	"id",
	"name",
	"build",

	"offs_x",
	"offs_y",
	"size_x",
	"size_y",

	// EventCommand options...
	"command",
	"start_time",

	// Event options...
	"time",
	"key",

	// FrameSequence options...
	"resource_id",
	"seq",

	// Object options...
	"x",
	"y",

	// Scroller options...
	"max_value",
	"colors",
	"s_resource_id",
	"active_sx",
	"active_sy",

	// InputField options...
	"string_len",
	"font",
	"ifont",
	"space",
	"align_center",
	"color",
	"string",

	// Screen options...
	"background"
};

int acsScriptMode = ACS_NONE;
int acsEndBlockMode = ACS_NONE;

int aciCurCredits05 = 0;

aciScreenResource* acsRes;
aciScreenEventCommand* acsEvComm;
aciScreenEvent* acsEv;
aciScreenFrameSequence* acsFrameSeq;
aciScreenObject* acsObj;
aciScreen* acsScr;

void acsParseScript(const char* fname,const char* bname)
{
	int id,num = 0;

	acsScrD = new aciScreenDispatcher;

#ifndef _BINARY_SCRIPT_
	_sALLOC_HEAP_(ACS_SCRIPT_SIZE,char);
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
			id = script -> read_option(-1);

#ifndef _BINARY_SCRIPT_
			if(id == -1)
				handle_error("Unknown keyword",script -> prevStrBuf);
#else
			if(id == iSCRIPT_EOF) script -> EOF_Flag = 1;
			if(id == -1)
				handle_error("Unknown keyword",NULL);
#endif
			switch(id){
				case ACS_END_BLOCK:
					acsEndScriptBlock();
					break;
				case ACS_END_STRING:
					break;
				case ACS_NEW_RESOURCE:
					if(acsScriptMode != ACS_NONE && acsScriptMode != ACS_INIT_SCREEN)
						handle_error("Misplaced option",acsOptIDs[id]);
					acsEndBlockMode = acsScriptMode;
					acsScriptMode = ACS_INIT_RESOURCE;
					num = script -> read_idata();
					acsRes = acsAllocResource(num);
					if(!acsRes) ErrH.Abort("Bad resource type...");
					break;
				case ACS_NEW_COMMAND:
					if(acsScriptMode != ACS_INIT_EVENT)
						handle_error("Misplaced option",acsOptIDs[id]);
					acsScriptMode = ACS_INIT_COMMAND;
					acsEvComm = new aciScreenEventCommand;
					break;
				case ACS_NEW_EVENT:
					if(acsScriptMode != ACS_INIT_OBJECT && acsScriptMode != ACS_INIT_SCREEN)
						handle_error("Misplaced option",acsOptIDs[id]);
					acsEndBlockMode = acsScriptMode;
					acsScriptMode = ACS_INIT_EVENT;
					acsEv = new aciScreenEvent;
					break;
				case ACS_NEW_FRAME_SEQ:
					if(acsScriptMode != ACS_INIT_OBJECT)
						handle_error("Misplaced option",acsOptIDs[id]);
					acsScriptMode = ACS_INIT_FRAME_SEQ;
					acsFrameSeq = new aciScreenFrameSequence;
					break;
				case ACS_NEW_OBJECT:
					if(acsScriptMode != ACS_INIT_SCREEN)
						handle_error("Misplaced option",acsOptIDs[id]);
					acsScriptMode = ACS_INIT_OBJECT;
					acsObj = acsAllocObject(script -> read_idata());
					break;
				case ACS_NEW_SCREEN:
					if(acsScriptMode != ACS_NONE)
						handle_error("Misplaced option",acsOptIDs[id]);
					acsScriptMode = ACS_INIT_SCREEN;
					acsScr = new aciScreen;
					break;
				case ACS_SET_ID:
					if(acsScriptMode == ACS_INIT_RESOURCE){
						acsRes -> ID = script -> read_idata();
					}
					else {
						if(acsScriptMode == ACS_INIT_FRAME_SEQ){
							acsFrameSeq -> ID = script -> read_idata();
						}
						else {
							if(acsScriptMode == ACS_INIT_OBJECT){
								acsObj -> ID = script -> read_idata();
							}
							else {
								if(acsScriptMode == ACS_INIT_SCREEN){
									acsScr -> ID = script -> read_idata();
								}
								else {
									handle_error("Misplaced option",acsOptIDs[id]);
								}
							}
						}
					}
					break;
				case ACS_SET_FNAME:
					if(acsScriptMode == ACS_INIT_RESOURCE){
						script -> prepare_pdata();
						acsRes -> set_name(script -> get_conv_ptr());
					}
					else {
						handle_error("Misplaced option",acsOptIDs[id]);
					}
					break;
				case ACS_SET_BUILD:
					if(acsScriptMode == ACS_INIT_RESOURCE){
						acsRes -> flags |= ACS_BUILD_RESOURCE;
					}
					else {
						handle_error("Misplaced option",acsOptIDs[id]);
					}
					break;
				case ACS_SET_OFFS_X:
					if(acsScriptMode == ACS_INIT_RESOURCE && acsRes -> type == ACS_BMO){
						((aciScreenResourceBMO*)acsRes) -> OffsX = script -> read_idata();
					}
					else {
						handle_error("Misplaced option",acsOptIDs[id]);
					}
					break;
				case ACS_SET_OFFS_Y:
					if(acsScriptMode == ACS_INIT_RESOURCE && acsRes -> type == ACS_BMO){
						((aciScreenResourceBMO*)acsRes) -> OffsY = script -> read_idata();
					}
					else {
						handle_error("Misplaced option",acsOptIDs[id]);
					}
					break;
				case ACS_SET_SZ_X:
					if(acsScriptMode == ACS_INIT_RESOURCE && acsRes -> type == ACS_BMO){
						((aciScreenResourceBMO*)acsRes) -> SizeX = script -> read_idata();
					}
					else {
						if(acsScriptMode == ACS_INIT_OBJECT && acsObj -> type == ACS_SCROLLER_OBJ){
							((aciScreenScroller*)acsObj) -> SizeX = script -> read_idata();
						}
						else
							handle_error("Misplaced option",acsOptIDs[id]);
					}
					break;
				case ACS_SET_SZ_Y:
					if(acsScriptMode == ACS_INIT_RESOURCE && acsRes -> type == ACS_BMO){
						((aciScreenResourceBMO*)acsRes) -> SizeY = script -> read_idata();
					}
					else {
						if(acsScriptMode == ACS_INIT_OBJECT && acsObj -> type == ACS_SCROLLER_OBJ){
							((aciScreenScroller*)acsObj) -> SizeY = script -> read_idata();
						}
						else
							handle_error("Misplaced option",acsOptIDs[id]);
					}
					break;
				case ACS_SET_EVENT:
					if(acsScriptMode == ACS_INIT_COMMAND){
						acsEvComm -> code = script -> read_idata();
						for(num = 0; num < 4; num ++)
							acsEvComm -> data[num] = script -> read_idata();
					}
					else {
						handle_error("Misplaced option",acsOptIDs[id]);
					}
					break;
				case ACS_SET_START_TIMER:
					if(acsScriptMode == ACS_INIT_COMMAND){
						acsEvComm -> StartTimer = script -> read_idata();
					}
					else {
						handle_error("Misplaced option",acsOptIDs[id]);
					}
					break;
				case ACS_SET_MAX_TIMER:
					if(acsScriptMode == ACS_INIT_EVENT){
						acsEv -> MaxTimer = script -> read_idata();
					}
					else {
						handle_error("Misplaced option",acsOptIDs[id]);
					}
					break;
				case ACS_ADD_KEY:
					if(acsScriptMode == ACS_INIT_EVENT){
						acsEv -> AddKey(script -> read_key());
					}
					else {
						handle_error("Misplaced option",acsOptIDs[id]);
					}
					break;
				case ACS_SET_RESOURCE_ID:
					if(acsScriptMode == ACS_INIT_FRAME_SEQ){
						acsFrameSeq -> ResourceID = script -> read_idata();
					}
					else {
						if(acsScriptMode == ACS_INIT_OBJECT && acsObj -> type == ACS_SCROLLER_OBJ){
							((aciScreenScroller*)acsObj) -> ScrResID = script -> read_idata();
						}
						else {
							handle_error("Misplaced option",acsOptIDs[id]);
						}
					}
					break;
				case ACS_SET_SEQ:
					if(acsScriptMode == ACS_INIT_FRAME_SEQ){
						acsFrameSeq -> StartFrame = script -> read_idata();
						acsFrameSeq -> EndFrame = script -> read_idata();
						acsFrameSeq -> FrameDelta = script -> read_idata();
					}
					else {
						handle_error("Misplaced option",acsOptIDs[id]);
					}
					break;
				case ACS_SET_POS_X:
					if(acsScriptMode == ACS_INIT_OBJECT){
						acsObj -> PosX = script -> read_idata();
					}
					else {
						if(acsScriptMode == ACS_INIT_OBJECT && acsObj -> type == ACS_SCROLLER_OBJ){
							((aciScreenScroller*)acsObj) -> PosX = script -> read_idata();
						}
						else
							handle_error("Misplaced option",acsOptIDs[id]);
					}
					break;
				case ACS_SET_POS_Y:
					if(acsScriptMode == ACS_INIT_OBJECT){
						acsObj -> PosY = script -> read_idata();
					}
					else {
						if(acsScriptMode == ACS_INIT_OBJECT && acsObj -> type == ACS_SCROLLER_OBJ){
							((aciScreenScroller*)acsObj) -> PosY = script -> read_idata();
						}
						else
							handle_error("Misplaced option",acsOptIDs[id]);
					}
					break;
				case ACS_SET_MAX_VALUE:
					if(acsScriptMode == ACS_INIT_OBJECT && acsObj -> type == ACS_SCROLLER_OBJ){
						((aciScreenScroller*)acsObj) -> MaxValue = script -> read_idata();
					}
					else {
						handle_error("Misplaced option",acsOptIDs[id]);
					}
					break;
				case ACS_SET_ACTIVE_SX:
					if(acsScriptMode == ACS_INIT_OBJECT && acsObj -> type == ACS_SCROLLER_OBJ){
						((aciScreenScroller*)acsObj) -> ActiveSX = script -> read_idata();
					}
					else {
						handle_error("Misplaced option",acsOptIDs[id]);
					}
					break;
				case ACS_SET_ACTIVE_SY:
					if(acsScriptMode == ACS_INIT_OBJECT && acsObj -> type == ACS_SCROLLER_OBJ){
						((aciScreenScroller*)acsObj) -> ActiveSY = script -> read_idata();
					}
					else {
						handle_error("Misplaced option",acsOptIDs[id]);
					}
					break;
				case ACS_SET_SECOND_RES:
					if(acsScriptMode == ACS_INIT_OBJECT && acsObj -> type == ACS_SCROLLER_OBJ){
						((aciScreenScroller*)acsObj) -> sResID = script -> read_idata();
					}
					else {
						handle_error("Misplaced option",acsOptIDs[id]);
					}
					break;
				case ACS_SET_COLORS:
					if(acsScriptMode == ACS_INIT_OBJECT && acsObj -> type == ACS_SCROLLER_OBJ){
						((aciScreenScroller*)acsObj) -> Color = script -> read_idata();
						((aciScreenScroller*)acsObj) -> BackColor = script -> read_idata();
					}
					else {
						handle_error("Misplaced option",acsOptIDs[id]);
					}
					break;
				case ACS_SET_STRING_LEN:
					if(acsScriptMode == ACS_INIT_OBJECT && acsObj -> type == ACS_INPUT_FIELD_OBJ){
						((aciScreenInputField*)acsObj) -> MaxStrLen = script -> read_idata();
						((aciScreenInputField*)acsObj) -> alloc_str();
					}
					else {
						handle_error("Misplaced option",acsOptIDs[id]);
					}
					break;
				case ACS_SET_FONT:
					if(acsScriptMode == ACS_INIT_OBJECT && acsObj -> type == ACS_INPUT_FIELD_OBJ){
						((aciScreenInputField*)acsObj) -> font = script -> read_idata();
					}
					else {
						handle_error("Misplaced option",acsOptIDs[id]);
					}
					break;
				case ACS_SET_CENTER:
					if(acsScriptMode == ACS_INIT_OBJECT && acsObj -> type == ACS_INPUT_FIELD_OBJ){
						acsObj -> flags |= ACS_ALIGN_CENTER;
					}
					else {
						handle_error("Misplaced option",acsOptIDs[id]);
					}
					break;
				case ACS_SET_COLOR:
					if(acsScriptMode == ACS_INIT_OBJECT && acsObj -> type == ACS_INPUT_FIELD_OBJ){
						num = script -> read_idata();
						if(num == 0) ((aciScreenInputField*)acsObj) -> Color0 = script -> read_idata();
						if(num == 1) ((aciScreenInputField*)acsObj) -> Color1 = script -> read_idata();
						if(num == 2) ((aciScreenInputField*)acsObj) -> Color2 = script -> read_idata();
					}
					else {
						handle_error("Misplaced option",acsOptIDs[id]);
					}
					break;
				case ACS_SET_STRING:
					if(acsScriptMode == ACS_INIT_OBJECT && acsObj -> type == ACS_INPUT_FIELD_OBJ){
						script -> prepare_pdata();
						((aciScreenInputField*)acsObj) -> set_string(script -> get_conv_ptr());
					}
					else {
						handle_error("Misplaced option",acsOptIDs[id]);
					}
					break;
				case ACS_SET_IFONT:
					if(acsScriptMode == ACS_INIT_OBJECT && acsObj -> type == ACS_INPUT_FIELD_OBJ){
						((aciScreenInputField*)acsObj) -> font = script -> read_idata();
						((aciScreenInputField*)acsObj) -> flags |= ACS_ISCREEN_FONT;
					}
					else {
						handle_error("Misplaced option",acsOptIDs[id]);
					}
					break;
				case ACS_SET_SPACE:
					if(acsScriptMode == ACS_INIT_OBJECT && acsObj -> type == ACS_INPUT_FIELD_OBJ){
						((aciScreenInputField*)acsObj) -> Space = script -> read_idata();
					}
					else {
						handle_error("Misplaced option",acsOptIDs[id]);
					}
					break;
				case ACS_SET_BACK_ID:
					if(acsScriptMode == ACS_INIT_SCREEN){
						acsScr -> backResID = script -> read_idata();
					}
					else {
						if(acsScriptMode == ACS_INIT_OBJECT && acsObj -> type == ACS_SCROLLER_OBJ){
							((aciScreenScroller*)acsObj) -> flags |= ACS_BACK_RES;
						}
						else
							handle_error("Misplaced option",acsOptIDs[id]);
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
	_sFREE_HEAP_(ACS_SCRIPT_SIZE,char);
#endif
}

int acsGetOptionID(char* p)
{
	int i;
	if(!p) handle_error("Null ID pointer");
	for(i = 0; i < MAX_OPTION; i ++){
		if(!strcmp(p,acsOptIDs[i]))
			return i;
	}
	return -1;
}

void acsEndScriptBlock(void)
{
	switch(acsScriptMode){
		case ACS_INIT_RESOURCE:
			if(acsEndBlockMode == ACS_INIT_SCREEN)
				acsScr -> AddResource(acsRes);
			else
				acsScrD -> AddResource(acsRes);
			acsScriptMode = acsEndBlockMode;
			break;
		case ACS_INIT_COMMAND:
			acsEv -> AddCommand(acsEvComm);
			acsScriptMode = ACS_INIT_EVENT;
			break;
		case ACS_INIT_EVENT:
			if(acsEndBlockMode == ACS_INIT_SCREEN)
				acsScr -> AddEvent(acsEv);
			else
				acsObj -> AddEvent(acsEv);
			acsScriptMode = acsEndBlockMode;
			break;
		case ACS_INIT_FRAME_SEQ:
			acsObj -> AddSeq(acsFrameSeq);
			acsScriptMode = ACS_INIT_OBJECT;
			break;
		case ACS_INIT_OBJECT:
			acsScr -> AddObject(acsObj);
			acsScriptMode = ACS_INIT_SCREEN;
			break;
		case ACS_INIT_SCREEN:
			acsScrD -> AddScreen(acsScr);
			acsScriptMode = ACS_NONE;
			break;
	}
}

