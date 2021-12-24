/* ---------------------------- INCLUDE SECTION ----------------------------- */


#include "../global.h"

#include "../network.h"
#include "controls.h"
#include "i_chat.h"
#include "i_str.h"
#include "hfont.h"
#include "iscreen.h"
#include "../actint/item_api.h"
#include "../actint/actint.h"

#include "../network.h"

#ifndef _WIN32
#include <arpa/inet.h> // ntohl() FIXME: remove
#endif

/* ----------------------------- EXTERN SECTION ----------------------------- */

extern XBuffer* iResBuf;
extern int aciKeyboardLocked;

extern int iScreenOffs;

extern char* iSTR_WORLD_NONE_CHAR;
extern char* iSTR_WORLD_FOSTRAL_CHAR;
extern char* iSTR_WORLD_GLORX_CHAR;
extern char* iSTR_WORLD_NECROSS_CHAR;
extern char* iSTR_WORLD_XPLO_CHAR;
extern char* iSTR_WORLD_KHOX_CHAR;
extern char* iSTR_WORLD_BOOZEENA_CHAR;
extern char* iSTR_WORLD_WEEXOW_CHAR;
extern char* iSTR_WORLD_HMOK_CHAR;
extern char* iSTR_WORLD_THREALL_CHAR;
extern char* iSTR_WORLD_ARK_A_ZNOY_CHAR;
extern char* iSTR_COLOR_GREEN_CHAR;
extern char* iSTR_COLOR_ORANGE_CHAR;
extern char* iSTR_COLOR_BLUE_CHAR;
extern char* iSTR_COLOR_YELLOW_CHAR;
extern char* iSTR_COLOR_RED_CHAR;
extern char* iSTR_COLOR_WHITE_CHAR;
extern char* iSTR_COLOR_GRAY_CHAR;
extern char* iSTR_COLOR_BLACK_CHAR;
extern char* iSTR_COLOR_CAMOUFLAGE_CHAR;
extern char* iSTR_COLOR_PATROL_CHAR;
extern char* iSTR_MUTE_ALL;
extern char* iSTR_CLEAR_LOG;
extern char* iSTR_BACKGROUND;
extern char* iSTR_LEAVE_CHAT;
extern char* iSTR_All;
extern char* iSTR_Yellow;
extern char* iSTR_Orange;
extern char* iSTR_Blue;
extern char* iSTR_Green;
extern char* iSTR_Red;
extern char* iSTR_White;
extern char* iSTR_Gray;
extern char* iSTR_Black;
extern char* iSTR_Camouflage;
extern char* iSTR_Patrol;

extern aciFont** aScrFonts32;

/* --------------------------- PROTOTYPE SECTION ---------------------------- */

int iGetKeyName(int vkey,int shift,int lng_flag);

void aOutText32(int x,int y,int color,void* text,int font,int hspace,int vspace);
int aTextWidth32(void* text,int font,int hspace);
int aTextHeight32(void* text,int font,int vspace);

void iChatInit(void);
void iChatQuant(int flush = 0);
void iChatFinit(void);
void iChatKeyQuant(SDL_Event *k);
void iChatMouseQuant(int x,int y,int bt);

void iInitChatButtons(void);
void iInitChatScreen(void);

void iChatInputChar(SDL_Event *code);
void iChatInputChar(unsigned char* input_char);
void iChatInputFlush(void);
void iChatInputBack(void);
void iChatInputEditing(SDL_Event *code);
void iChatInputDrawCursor(void);

void iChatMouseHandler(iChatButton* bt,int bt_id);
void iChatClearButtons(void);
void iChatClearLog(void);

iChatButton* iGetChatButton(int id);
iChatButton* iGetChatPlayerButton(int id);

void put_map(int x,int y,int sx,int sy);

unsigned char UTF8toCP866(unsigned short utf);
unsigned short CP866toUTF8(unsigned char cp866);

int getCharGroup(char chr);
/* --------------------------- DEFINITION SECTION --------------------------- */

#define ICS_INPUT_TIMER 	10

// Button IDs...
#define ICS_UP_BUTTON		0x01
#define ICS_DN_BUTTON		0x02

#define ICS_ALL_BUTTON		0x03
#define ICS_GREEN_BUTTON	0x04
#define ICS_ORANGE_BUTTON	0x05
#define ICS_BLUE_BUTTON 	0x06
#define ICS_YELLOW_BUTTON	0x07

#define ICS_MUTE_BUTTON 	0x08
#define ICS_CLEAR_BUTTON	0x09

#define ICS_PLAYER1_BUTTON	0x0A
#define ICS_PLAYER2_BUTTON	0x0B
#define ICS_PLAYER3_BUTTON	0x0C
#define ICS_PLAYER4_BUTTON	0x0D
#define ICS_PLAYER5_BUTTON	0x0E

#define ICS_EXIT_BUTTON 	0x0F

#define ICS_BACK_BUTTON 	0x10

#define ICS_RED_BUTTON	0x11
#define ICS_WHITE_BUTTON	0x12
#define ICS_GRAY_BUTTON	0x13
#define ICS_BLACK_BUTTON	0x14
#define ICS_CAMOUFLAGE_BUTTON	0x15
#define ICS_PATROL_BUTTON	0x16

// Chat Screen Colors...
const int  ICS_FON_COLOR	= 184;
const int  ICS_BORDER_COLOR	= 184;

const int  ICS_STRING_COL0	= 227 + (3 << 16);
const int  ICS_STRING_COL1	= 88 + (1 << 16);

const int  ICS_iFON_COLOR	= 80;
const int  ICS_iBORDER_COLOR = 80;

const int  ICS_iSTRING_COL0	= 2 + (3 << 16);
const int  ICS_iSTRING_COL1	= 144 + (1 << 16);

// Chat Screen Coordinates...
#define ICS_DELTA		10

#define ICS_HISTORY_SIZE_X	450
#define ICS_HISTORY_SIZE_Y	320

#define ICS_BUTTON_SIZE_X	170
#define ICS_BUTTON_SIZE_Y	20

XList* iChatButtons = NULL;
iChatHistoryScreen* iChatHistory = NULL;
iChatInputField* iChatInput = NULL;

int iChatON = 0;
int iChatExit = 0;
int iScreenChat = 0;

int iChatSkip = 0;

int iChatFilter = MESSAGE_FOR_ALL;
int iChatFilterID = 0;

int iChatMUTE = 0;
int iChatBackLog = 1;

bool iChatCursorFlag = false;
int iChatCursorTimer = 0;

static int aciChatColors0[10] =
{
	(214 | (2 << 16)),
	(230 | (2 << 16)),
	(244 | (2 << 16)),
	(150 | (2 << 16)),
	(177 | (2 << 16)),
	(6 | (2 << 16)),
	(6 | (2 << 16)),
	(6 | (2 << 16)),
	(214 | (2 << 16)),
	(244 | (2 << 16))
};

static int aciChatColors1[10] =
{
	(244 | (2 << 16)),
	(134 | (2 << 16)),
	(150 | (2 << 16)),
	(176 | (2 << 16)),
	(196 | (2 << 16)),
	(230 | (2 << 16)),
	(230 | (2 << 16)),
	(230 | (2 << 16)),
	(244 | (2 << 16)),
	(150 | (2 << 16))
};

int selectionColorI = (244 | (2 << 16)); //aciChatColors0[2] blue color for selection
int selectionColor = (150 | (2 << 16)); //aciChatColors1[2]

int cursorColorI = (6 | (2 << 16)); //aciChatColors0[5] white color for cursor
int cursorColor = (230 | (2 << 16)); //aciChatColors1[5]
int cursorWidth = 2;

static char* iChatWorlds[] = {
	iSTR_WORLD_NONE_CHAR,
	iSTR_WORLD_FOSTRAL_CHAR,
	iSTR_WORLD_GLORX_CHAR,
	iSTR_WORLD_NECROSS_CHAR,
	iSTR_WORLD_XPLO_CHAR,
	iSTR_WORLD_KHOX_CHAR,
	iSTR_WORLD_BOOZEENA_CHAR,
	iSTR_WORLD_WEEXOW_CHAR,
	iSTR_WORLD_HMOK_CHAR,
	iSTR_WORLD_THREALL_CHAR,
	iSTR_WORLD_ARK_A_ZNOY_CHAR
};

static char* iChatColors[] = {
	iSTR_COLOR_GREEN_CHAR,
	iSTR_COLOR_ORANGE_CHAR,
	iSTR_COLOR_BLUE_CHAR,
	iSTR_COLOR_YELLOW_CHAR,
	iSTR_COLOR_RED_CHAR,
	iSTR_COLOR_WHITE_CHAR,
	iSTR_COLOR_GRAY_CHAR,
	iSTR_COLOR_BLACK_CHAR,
	iSTR_COLOR_CAMOUFLAGE_CHAR,
	iSTR_COLOR_PATROL_CHAR
};

iChatScreenObject::iChatScreenObject(void)
{
	ID = 0;
	type = ICS_STRING;
	flags = 0;

	font = 0;

	PosX = PosY = 0;
	SizeX = SizeY = 0;
	fonColor = borderColor = 0;
}

iChatScreenObject::~iChatScreenObject(void)
{
}

void iChatScreenObject::redraw(void)
{
	if(iChatBackLog)
		XGR_Rectangle(PosX,PosY,SizeX,SizeY,borderColor,fonColor,XGR_FILLED);
	else
		XGR_Rectangle(PosX,PosY,SizeX,SizeY,borderColor,fonColor,XGR_OUTLINED);
}

void iChatScreenObject::flush(void)
{
	XGR_Flush(PosX,PosY,SizeX,SizeY);
}

iChatInputField::iChatInputField(void)
{
	string = std::string();
	
	XConv = new XBuffer(256);

	if(!iScreenChat)
		color = ICS_STRING_COL1;
	else
		color = ICS_iSTRING_COL1;
	
	position = 0;
	
	selectionPosition = 0;
}

iChatInputField::~iChatInputField(void)
{
}

void iChatInputField::redraw(void)
{
	int dy;
	iChatScreenObject::redraw();
	dy = (SizeY - aTextHeight32((char*)string.c_str(), font, 0)) / 2;
	
	this -> selectionRedraw();
	
	XConv -> init();
	*XConv < CurPlayerName < (char*)(string.c_str());

	aOutText32(PosX, PosY, color, XConv -> address(), font, 1, 0);
}

void iChatInputField::selectionRedraw(void) {
	int color;
	if (iScreenChat) {
		color = selectionColorI;
	}
	else {
		color = selectionColor;
	}
	
	int leftPosition = std::min(position, selectionPosition);
	int rightPosition = std::max(position, selectionPosition);
	
	char* ptr;
	
	XConv -> init();
	*XConv < CurPlayerName < (char*)(string.substr(0, leftPosition).c_str());
	ptr = iChatInput -> XConv -> address();
	int left_x = aTextWidth32(ptr, iChatInput -> font, 1);
	
	XConv -> init();
	*XConv < CurPlayerName < (char*)(string.substr(0, rightPosition).c_str());
	ptr = iChatInput -> XConv -> address();
	int right_x = aTextWidth32(ptr, iChatInput -> font, 1);
	
	XGR_Rectangle(PosX + left_x, PosY, right_x - left_x, SizeY, color, color, XGR_FILLED);
}

iChatHistoryScreen::iChatHistoryScreen(void)
{
	int i;
	NumStr = ICS_MAX_HISTORY_OBJ;

	data = new char*[NumStr];
	ColorData = new int[NumStr];
	for(i = 0; i < NumStr; i ++){
		data[i] = new char[ISC_MAX_STRING_LEN];
		memset(data[i],0,ISC_MAX_STRING_LEN);

		if(!iScreenChat)
			ColorData[i] = ICS_STRING_COL0;
		else
			ColorData[i] = ICS_iSTRING_COL0;
	}
}

iChatHistoryScreen::~iChatHistoryScreen(void)
{
	int i;
	for(i = 0; i < NumStr; i ++){
		delete[] data[i];
	}
	delete[] data;
	delete[] ColorData;
}

void iChatHistoryScreen::redraw(void)
{
	int i,y = 0;
	iChatScreenObject::redraw();
	for(i = 0; i < NumStr; i ++){
		if(strlen(data[i]))
			aOutText32(PosX,PosY + y,ColorData[i],data[i],font,1,0);
		y += aTextHeight32(data[i],font,0) + 2;
	}
}

void iChatHistoryScreen::clear(void)
{
	int i;
	for(i = 0; i < NumStr; i ++){
		*data[i] = 0;
	}
}

iChatButton::iChatButton(int num_state)
{
	CurState = 0;
	NumState = num_state;
	StateColors = new int[NumState];

	extData = 0;

	string = new char[ISC_MAX_STRING_LEN];
	memset(string,0,ISC_MAX_STRING_LEN);
}

iChatButton::~iChatButton(void)
{
	delete[] StateColors;
	delete[] string;
}

void iChatButton::redraw(void)
{
	int dx,dy;
	iChatScreenObject::redraw();
	dx = (SizeX - aTextWidth32(string,font,1)) / 2;
	dy = (SizeY - aTextHeight32(string,font,0)) / 2;

	aOutText32(PosX + dx,PosY + dy,StateColors[CurState],string,font,1,0);
}

void iChatScreenObject::init(int x,int y,int sx,int sy,int col1,int col2)
{
	PosX = x;
	PosY = y;

	SizeX = sx;
	SizeY = sy;

	fonColor = col1;
	borderColor = col2;
}

void iChatInit(void)
{
	int x,y,sx,sy,bsx,col0,col1,bcol;
	iChatButton* p;

	sx = ICS_HISTORY_SIZE_X + ICS_DELTA + ICS_BUTTON_SIZE_X + (XGR_MAXX - 800);
	sy = ICS_HISTORY_SIZE_Y + ICS_DELTA * 2 + ICS_BUTTON_SIZE_Y * 2 + (XGR_MAXY - 600);

//	  bsx = (sx - ICS_DELTA * 2) / 3;
	bsx = (ICS_HISTORY_SIZE_X - ICS_DELTA * 2) / 3;

	x = (XGR_MAXX - sx) / 2;
	y = (XGR_MAXY - sy) / 2;

	if(!iScreenChat)
		bcol = ICS_BORDER_COLOR;
	else {
		y -= 30;
		bcol = ICS_iBORDER_COLOR;
	}

	iChatButtons = new XList;

	iChatHistory = new iChatHistoryScreen;
	iChatHistory -> init(x,y + (ICS_BUTTON_SIZE_Y + ICS_DELTA) * 1,ICS_HISTORY_SIZE_X,ICS_HISTORY_SIZE_Y,bcol,bcol);

	iChatInput = new iChatInputField;
	iChatInput -> init(x,y + ICS_HISTORY_SIZE_Y + ICS_DELTA + (ICS_BUTTON_SIZE_Y + ICS_DELTA) * 1,ICS_HISTORY_SIZE_X,ICS_BUTTON_SIZE_Y,bcol,bcol);
	iChatInput -> string = ">";
	iChatInput -> position = 1;
	iChatInput -> selectionPosition = 1;

	p = new iChatButton(2);
	p -> init(x,y,bsx,ICS_BUTTON_SIZE_Y,bcol,bcol);
	p -> ID = ICS_MUTE_BUTTON;
	p -> set_string(iSTR_MUTE_ALL);
	if(!iScreenChat){
		col0 = ICS_STRING_COL0;
		col1 = ICS_STRING_COL1;
	}
	else {
		col0 = ICS_iSTRING_COL0;
		col1 = ICS_iSTRING_COL1;
	}
	p -> set_color(0,col0);
	p -> set_color(1,col1);
	iChatButtons -> AddElement((XListElement*)p);

	p -> CurState = iChatMUTE;

	p = new iChatButton(1);
	p -> init(x + bsx + ICS_DELTA,y,bsx,ICS_BUTTON_SIZE_Y,bcol,bcol);
	p -> ID = ICS_CLEAR_BUTTON;
	p -> set_string(iSTR_CLEAR_LOG);
	if(!iScreenChat){
		col0 = ICS_STRING_COL0;
		col1 = ICS_STRING_COL1;
	}
	else {
		col0 = ICS_iSTRING_COL0;
		col1 = ICS_iSTRING_COL1;
	}
	p -> set_color(0,col0);
	iChatButtons -> AddElement((XListElement*)p);

	iChatBackLog = 1;
	if(!iScreenChat){
		p = new iChatButton(2);
		p -> init(x + (bsx + ICS_DELTA) * 3,y,ICS_BUTTON_SIZE_X,ICS_BUTTON_SIZE_Y,bcol,bcol);
		p -> ID = ICS_BACK_BUTTON;
		p -> set_string(iSTR_BACKGROUND);
		if(!iScreenChat){
			col0 = ICS_STRING_COL0;
			col1 = ICS_STRING_COL1;
		}
		else {
			col0 = ICS_iSTRING_COL0;
			col1 = ICS_iSTRING_COL1;
		}
		p -> set_color(0,col0);
		p -> set_color(1,col1);
		p -> CurState = 1;
		iChatButtons -> AddElement((XListElement*)p);
	}
//	  else {
//		  iChatBackLog = 0;
//	  }

	if(!iScreenChat){
		p = new iChatButton(1);
		p -> init(x + bsx * 2 + ICS_DELTA * 2,y,bsx,ICS_BUTTON_SIZE_Y,bcol,bcol);
		p -> ID = ICS_EXIT_BUTTON;
		p -> set_string(iSTR_LEAVE_CHAT);
		col0 = ICS_STRING_COL0;
		p -> set_color(0,col0);
		iChatButtons -> AddElement((XListElement*)p);
	}

	p = new iChatButton(2);
	p -> init(x + ICS_HISTORY_SIZE_X + ICS_DELTA,y + (ICS_BUTTON_SIZE_Y + ICS_DELTA) * 1,ICS_BUTTON_SIZE_X,ICS_BUTTON_SIZE_Y,bcol,bcol);
	p -> ID = ICS_ALL_BUTTON;
	p -> set_string(iSTR_All);
	if(!iScreenChat){
		col0 = ICS_STRING_COL0;
		col1 = ICS_STRING_COL1;
	}
	else {
		col0 = ICS_iSTRING_COL0;
		col1 = ICS_iSTRING_COL1;
	}
	p -> set_color(0,col0);
	p -> set_color(1,col1);
	iChatButtons -> AddElement((XListElement*)p);

	p = new iChatButton(2);
	p -> init(x + ICS_HISTORY_SIZE_X + ICS_DELTA,y + ICS_DELTA + (ICS_BUTTON_SIZE_Y + ICS_DELTA/5) * 2,ICS_BUTTON_SIZE_X/2 -1,ICS_BUTTON_SIZE_Y,bcol,bcol);
	p -> ID = ICS_YELLOW_BUTTON;
	p -> set_string(iSTR_Yellow);
	if(!iScreenChat){
		col0 = ICS_STRING_COL0;
		col1 = ICS_STRING_COL1;
	}
	else {
		col0 = ICS_iSTRING_COL0;
		col1 = ICS_iSTRING_COL1;
	}
	p -> set_color(0,col0);
	p -> set_color(1,col1);
	iChatButtons -> AddElement((XListElement*)p);

	p = new iChatButton(2);
	p -> init(x + ICS_HISTORY_SIZE_X + ICS_DELTA,y + ICS_DELTA + (ICS_BUTTON_SIZE_Y + ICS_DELTA/5) * 3,ICS_BUTTON_SIZE_X/2 -1,ICS_BUTTON_SIZE_Y,bcol,bcol);
	p -> ID = ICS_ORANGE_BUTTON;
	p -> set_string(iSTR_Orange);
	if(!iScreenChat){
		col0 = ICS_STRING_COL0;
		col1 = ICS_STRING_COL1;
	}
	else {
		col0 = ICS_iSTRING_COL0;
		col1 = ICS_iSTRING_COL1;
	}
	p -> set_color(0,col0);
	p -> set_color(1,col1);
	iChatButtons -> AddElement((XListElement*)p);

	p = new iChatButton(2);
	p -> init(x + ICS_HISTORY_SIZE_X + ICS_DELTA,y + ICS_DELTA + (ICS_BUTTON_SIZE_Y + ICS_DELTA/5) * 4,ICS_BUTTON_SIZE_X/2 -1,ICS_BUTTON_SIZE_Y,bcol,bcol);
	p -> ID = ICS_BLUE_BUTTON;
	p -> set_string(iSTR_Blue);
	if(!iScreenChat){
		col0 = ICS_STRING_COL0;
		col1 = ICS_STRING_COL1;
	}
	else {
		col0 = ICS_iSTRING_COL0;
		col1 = ICS_iSTRING_COL1;
	}
	p -> set_color(0,col0);
	p -> set_color(1,col1);
	iChatButtons -> AddElement((XListElement*)p);

	p = new iChatButton(2);
	p -> init(x + ICS_HISTORY_SIZE_X + ICS_DELTA,y + ICS_DELTA + (ICS_BUTTON_SIZE_Y + ICS_DELTA/5) * 5,ICS_BUTTON_SIZE_X/2 -1,ICS_BUTTON_SIZE_Y,bcol,bcol);
	p -> ID = ICS_GREEN_BUTTON;
	p -> set_string(iSTR_Green);
	if(!iScreenChat){
		col0 = ICS_STRING_COL0;
		col1 = ICS_STRING_COL1;
	}
	else {
		col0 = ICS_iSTRING_COL0;
		col1 = ICS_iSTRING_COL1;
	}
	p -> set_color(0,col0);
	p -> set_color(1,col1);
	iChatButtons -> AddElement((XListElement*)p);

	p = new iChatButton(2);
	p -> init(x + ICS_HISTORY_SIZE_X + ICS_DELTA,y + ICS_DELTA + (ICS_BUTTON_SIZE_Y + ICS_DELTA/5) * 6,ICS_BUTTON_SIZE_X/2 - 1,ICS_BUTTON_SIZE_Y,bcol,bcol);
	p -> ID = ICS_RED_BUTTON;
	p -> set_string(iSTR_Red);
	if(!iScreenChat){
		col0 = ICS_STRING_COL0;
		col1 = ICS_STRING_COL1;
	}
	else {
		col0 = ICS_iSTRING_COL0;
		col1 = ICS_iSTRING_COL1;
	}
	p -> set_color(0,col0);
	p -> set_color(1,col1);
	iChatButtons -> AddElement((XListElement*)p);

	p = new iChatButton(2);
	p -> init(x + ICS_HISTORY_SIZE_X + ICS_DELTA + ICS_BUTTON_SIZE_X/2 + 1,y + ICS_DELTA + (ICS_BUTTON_SIZE_Y + ICS_DELTA/5) * 2,ICS_BUTTON_SIZE_X/2 - 1,ICS_BUTTON_SIZE_Y,bcol,bcol);
	p -> ID = ICS_WHITE_BUTTON;
	p -> set_string(iSTR_White);
	if(!iScreenChat){
		col0 = ICS_STRING_COL0;
		col1 = ICS_STRING_COL1;
	}
	else {
		col0 = ICS_iSTRING_COL0;
		col1 = ICS_iSTRING_COL1;
	}
	p -> set_color(0,col0);
	p -> set_color(1,col1);
	iChatButtons -> AddElement((XListElement*)p);

	p = new iChatButton(2);
	p -> init(x + ICS_HISTORY_SIZE_X + ICS_DELTA + ICS_BUTTON_SIZE_X/2 + 1,y + ICS_DELTA + (ICS_BUTTON_SIZE_Y + ICS_DELTA/5) * 3,ICS_BUTTON_SIZE_X/2 - 1,ICS_BUTTON_SIZE_Y,bcol,bcol);
	p -> ID = ICS_GRAY_BUTTON;
	p -> set_string(iSTR_Gray);
	if(!iScreenChat){
		col0 = ICS_STRING_COL0;
		col1 = ICS_STRING_COL1;
	}
	else {
		col0 = ICS_iSTRING_COL0;
		col1 = ICS_iSTRING_COL1;
	}
	p -> set_color(0,col0);
	p -> set_color(1,col1);
	iChatButtons -> AddElement((XListElement*)p);

	p = new iChatButton(2);
	p -> init(x + ICS_HISTORY_SIZE_X + ICS_DELTA + ICS_BUTTON_SIZE_X/2 + 1,y + ICS_DELTA + (ICS_BUTTON_SIZE_Y + ICS_DELTA/5) * 4,ICS_BUTTON_SIZE_X/2 - 1,ICS_BUTTON_SIZE_Y,bcol,bcol);
	p -> ID = ICS_BLACK_BUTTON;
	p -> set_string(iSTR_Black);
	if(!iScreenChat){
		col0 = ICS_STRING_COL0;
		col1 = ICS_STRING_COL1;
	}
	else {
		col0 = ICS_iSTRING_COL0;
		col1 = ICS_iSTRING_COL1;
	}
	p -> set_color(0,col0);
	p -> set_color(1,col1);
	iChatButtons -> AddElement((XListElement*)p);

	p = new iChatButton(2);
	p -> init(x + ICS_HISTORY_SIZE_X + ICS_DELTA + ICS_BUTTON_SIZE_X/2 + 1,y + ICS_DELTA + (ICS_BUTTON_SIZE_Y + ICS_DELTA/5) * 5,ICS_BUTTON_SIZE_X/2 - 1,ICS_BUTTON_SIZE_Y,bcol,bcol);
	p -> ID = ICS_CAMOUFLAGE_BUTTON;
	p -> set_string(iSTR_Camouflage);
	if(!iScreenChat){
		col0 = ICS_STRING_COL0;
		col1 = ICS_STRING_COL1;
	}
	else {
		col0 = ICS_iSTRING_COL0;
		col1 = ICS_iSTRING_COL1;
	}
	p -> set_color(0,col0);
	p -> set_color(1,col1);
	iChatButtons -> AddElement((XListElement*)p);

	p = new iChatButton(2);
	p -> init(x + ICS_HISTORY_SIZE_X + ICS_DELTA + ICS_BUTTON_SIZE_X/2 + 1,y + ICS_DELTA + (ICS_BUTTON_SIZE_Y + ICS_DELTA/5) * 6,ICS_BUTTON_SIZE_X/2 - 1,ICS_BUTTON_SIZE_Y,bcol,bcol);
	p -> ID = ICS_PATROL_BUTTON;
	p -> set_string(iSTR_Patrol);
	if(!iScreenChat){
		col0 = ICS_STRING_COL0;
		col1 = ICS_STRING_COL1;
	}
	else {
		col0 = ICS_iSTRING_COL0;
		col1 = ICS_iSTRING_COL1;
	}
	p -> set_color(0,col0);
	p -> set_color(1,col1);
	iChatButtons -> AddElement((XListElement*)p);

	p = new iChatButton(2);
	p -> init(x + ICS_HISTORY_SIZE_X + ICS_DELTA + ICS_BUTTON_SIZE_X/4,y + (ICS_BUTTON_SIZE_Y + ICS_DELTA) * 6,ICS_BUTTON_SIZE_X/2,ICS_BUTTON_SIZE_Y,bcol,bcol);
	p -> ID = ICS_UP_BUTTON;
	p -> set_string("\x1E");
	if(!iScreenChat){
		col0 = ICS_STRING_COL0;
		col1 = ICS_STRING_COL1;
	}
	else {
		col0 = ICS_iSTRING_COL0;
		col1 = ICS_iSTRING_COL1;
	}
	p -> set_color(0,col0);
	p -> set_color(1,col1);
	iChatButtons -> AddElement((XListElement*)p);

	p = new iChatButton(2);
	p -> init(x + ICS_HISTORY_SIZE_X + ICS_DELTA,y + (ICS_BUTTON_SIZE_Y + ICS_DELTA) * 7,ICS_BUTTON_SIZE_X,ICS_BUTTON_SIZE_Y,bcol,bcol);
	p -> ID = ICS_PLAYER1_BUTTON;
	p -> set_string("123456789012345 (F/B)");
	if(!iScreenChat){
		col0 = ICS_STRING_COL0;
		col1 = ICS_STRING_COL1;
	}
	else {
		col0 = ICS_iSTRING_COL0;
		col1 = ICS_iSTRING_COL1;
	}
	p -> set_color(0,col0);
	p -> set_color(1,col1);
	iChatButtons -> AddElement((XListElement*)p);

	p = new iChatButton(2);
	p -> init(x + ICS_HISTORY_SIZE_X + ICS_DELTA,y + (ICS_BUTTON_SIZE_Y + ICS_DELTA) * 8,ICS_BUTTON_SIZE_X,ICS_BUTTON_SIZE_Y,bcol,bcol);
	p -> ID = ICS_PLAYER2_BUTTON;
	p -> set_string("123456789012345 (F/B)");
	if(!iScreenChat){
		col0 = ICS_STRING_COL0;
		col1 = ICS_STRING_COL1;
	}
	else {
		col0 = ICS_iSTRING_COL0;
		col1 = ICS_iSTRING_COL1;
	}
	p -> set_color(0,col0);
	p -> set_color(1,col1);
	iChatButtons -> AddElement((XListElement*)p);

	p = new iChatButton(2);
	p -> init(x + ICS_HISTORY_SIZE_X + ICS_DELTA,y + (ICS_BUTTON_SIZE_Y + ICS_DELTA) * 9,ICS_BUTTON_SIZE_X,ICS_BUTTON_SIZE_Y,bcol,bcol);
	p -> ID = ICS_PLAYER3_BUTTON;
	p -> set_string("123456789012345 (F/B)");
	if(!iScreenChat){
		col0 = ICS_STRING_COL0;
		col1 = ICS_STRING_COL1;
	}
	else {
		col0 = ICS_iSTRING_COL0;
		col1 = ICS_iSTRING_COL1;
	}
	p -> set_color(0,col0);
	p -> set_color(1,col1);
	iChatButtons -> AddElement((XListElement*)p);

	p = new iChatButton(2);
	p -> init(x + ICS_HISTORY_SIZE_X + ICS_DELTA,y + (ICS_BUTTON_SIZE_Y + ICS_DELTA) * 10,ICS_BUTTON_SIZE_X,ICS_BUTTON_SIZE_Y,bcol,bcol);
	p -> ID = ICS_PLAYER4_BUTTON;
	p -> set_string("123456789012345 (F/B)");
	if(!iScreenChat){
		col0 = ICS_STRING_COL0;
		col1 = ICS_STRING_COL1;
	}
	else {
		col0 = ICS_iSTRING_COL0;
		col1 = ICS_iSTRING_COL1;
	}
	p -> set_color(0,col0);
	p -> set_color(1,col1);
	iChatButtons -> AddElement((XListElement*)p);

	p = new iChatButton(2);
	p -> init(x + ICS_HISTORY_SIZE_X + ICS_DELTA,y + (ICS_BUTTON_SIZE_Y + ICS_DELTA) * 11,ICS_BUTTON_SIZE_X,ICS_BUTTON_SIZE_Y,bcol,bcol);
	p -> ID = ICS_PLAYER5_BUTTON;
	p -> set_string("123456789012345 (F/B)");
	if(!iScreenChat){
		col0 = ICS_STRING_COL0;
		col1 = ICS_STRING_COL1;
	}
	else {
		col0 = ICS_iSTRING_COL0;
		col1 = ICS_iSTRING_COL1;
	}
	p -> set_color(0,col0);
	p -> set_color(1,col1);
	iChatButtons -> AddElement((XListElement*)p);

	p = new iChatButton(2);
	p -> init(x + ICS_HISTORY_SIZE_X + ICS_DELTA + ICS_BUTTON_SIZE_X/4,y + (ICS_BUTTON_SIZE_Y + ICS_DELTA) * 12,ICS_BUTTON_SIZE_X/2,ICS_BUTTON_SIZE_Y,bcol,bcol);
	p -> ID = ICS_DN_BUTTON;
	p -> set_string("\x1F");
	if(!iScreenChat){
		col0 = ICS_STRING_COL0;
		col1 = ICS_STRING_COL1;
	}
	else {
		col0 = ICS_iSTRING_COL0;
		col1 = ICS_iSTRING_COL1;
	}
	p -> set_color(0,col0);
	p -> set_color(1,col1);
	iChatButtons -> AddElement((XListElement*)p);

	iChatExit = 0;
	iChatON = 1;
	aciKeyboardLocked = 1;

	XGR_MouseShow();

	iInitChatScreen();
	SDL_StartTextInput();
}

void iChatQuant(int flush)
{
	iChatButton* p;
	if(!iChatON) return;

//	  if(iScreenChat) put_map(iScreenOffs,0,XGR_MAXX,XGR_MAXY);

	iInitChatScreen();
	p = (iChatButton*)iChatButtons -> fPtr;
	while(p){
		if(!(p -> flags & ICS_HIDDEN_OBJECT)) p -> redraw();
		p = (iChatButton*)p -> next;
	}
	
	iChatInput -> redraw();
	iChatHistory -> redraw();
	
	iChatCursorTimer++;
	if (iChatCursorTimer >= ICS_INPUT_TIMER) {
		iChatCursorTimer = 0;
		iChatCursorFlag = !iChatCursorFlag;
	}
	if (iChatCursorFlag) {
		iChatInputDrawCursor();
	}
	
	if(flush){
		p = (iChatButton*)iChatButtons -> fPtr;
		while(p){
			if(!(p -> flags & ICS_HIDDEN_OBJECT)) p -> flush();
			p = (iChatButton*)p -> next;
		}
		iChatInput -> flush();
		iChatHistory -> flush();
	}
}

void iChatFinit(void)
{
	iChatButton* p,*p1;

	if(iChatButtons){
		p = (iChatButton*)iChatButtons -> fPtr;
		while(p){
			p1 = (iChatButton*)p -> next;
			delete p;
			p = p1;
		}
		delete iChatButtons;
	}

	if(iChatHistory)
		delete iChatHistory;

	if(iChatInput)
		delete iChatInput;

	iChatON = iScreenChat = 0;
	aciKeyboardLocked = 0;

	iChatHistory = NULL;
	iChatInput = NULL;
	iChatButtons = NULL;
	SDL_StopTextInput();
}

void iChatKeyQuant(SDL_Event *k) {
	if (k) {
		iChatInputChar(k);
		iChatInputEditing(k);
	}
	
	if (k -> type != SDL_KEYDOWN && k -> type != SDL_TEXTINPUT) {
		iChatExit = 1;
		return;
	}
	if (iCheckKeyID(iKEY_CHAT, k -> key.keysym.scancode)) {
		iChatExit = 1;
		return;
	}
	
	switch(k->key.keysym.scancode) {
		case SDL_SCANCODE_ESCAPE:
			iChatExit = 1;
			break;
		case SDL_SCANCODE_RETURN:
			iChatInputFlush();
			break;
		case SDL_SCANCODE_BACKSPACE:
			iChatInputBack();
			break;
		default:
			break;
	}
}

void iChatMouseQuant(int x,int y,int bt)
{
	iChatButton* p = (iChatButton*)iChatButtons -> fPtr;
	while(p){
		if(p -> check_xy(x,y)) iChatMouseHandler(p,bt);
		p = (iChatButton*)p -> next;
	}
}

void iChatMouseHandler(iChatButton* bt,int bt_id)
{
	switch(bt -> ID){
		case ICS_EXIT_BUTTON:
			iChatExit = 1;
			break;
		case ICS_UP_BUTTON:
			if(iChatSkip)
				iChatSkip --;
			break;
		case ICS_DN_BUTTON:
			if(iChatSkip < players_list.size() - 5)
				iChatSkip ++;
			break;

		case ICS_ALL_BUTTON:
			iChatClearButtons();
			bt -> CurState = 1;
			iChatFilter = MESSAGE_FOR_ALL;
			iChatFilterID = 0;
			break;
		case ICS_YELLOW_BUTTON:
			iChatClearButtons();
			bt -> CurState = 1;
			iChatFilter = MESSAGE_FOR_TEAM;
			iChatFilterID = TEAM_YELLOW;
			break;
		case ICS_ORANGE_BUTTON:
			iChatClearButtons();
			bt -> CurState = 1;
			iChatFilter = MESSAGE_FOR_TEAM;
			iChatFilterID = TEAM_ORANGE;
			break;
		case ICS_BLUE_BUTTON:
			iChatClearButtons();
			bt -> CurState = 1;
			iChatFilter = MESSAGE_FOR_TEAM;
			iChatFilterID = TEAM_BLUE;
			break;
		case ICS_GREEN_BUTTON:
			iChatClearButtons();
			bt -> CurState = 1;
			iChatFilter = MESSAGE_FOR_TEAM;
			iChatFilterID = TEAM_GREEN;
			break;
		case ICS_RED_BUTTON:
			iChatClearButtons();
			bt -> CurState = 1;
			iChatFilter = MESSAGE_FOR_TEAM;
			iChatFilterID = TEAM_RED;
			break;
		case ICS_WHITE_BUTTON:
			iChatClearButtons();
			bt -> CurState = 1;
			iChatFilter = MESSAGE_FOR_TEAM;
			iChatFilterID = TEAM_WHITE;
			break;
		case ICS_GRAY_BUTTON:
			iChatClearButtons();
			bt -> CurState = 1;
			iChatFilter = MESSAGE_FOR_TEAM;
			iChatFilterID = TEAM_GRAY;
			break;
		case ICS_BLACK_BUTTON:
			iChatClearButtons();
			bt -> CurState = 1;
			iChatFilter = MESSAGE_FOR_TEAM;
			iChatFilterID = TEAM_BLACK;
			break;
		case ICS_CAMOUFLAGE_BUTTON:
			iChatClearButtons();
			bt -> CurState = 1;
			iChatFilter = MESSAGE_FOR_TEAM;
			iChatFilterID = TEAM_CAMOUFLAGE;
			break;
		case ICS_PATROL_BUTTON:
			iChatClearButtons();
			bt -> CurState = 1;
			iChatFilter = MESSAGE_FOR_TEAM;
			iChatFilterID = TEAM_PATROL;
			break;

		case ICS_MUTE_BUTTON:
			bt -> CurState ^= 1;
			iChatMUTE = bt -> CurState;
			break;
		case ICS_BACK_BUTTON:
			bt -> CurState ^= 1;
			iChatBackLog = bt -> CurState;
			break;
		case ICS_CLEAR_BUTTON:
			iChatClearLog();
			break;
		case ICS_PLAYER1_BUTTON:
		case ICS_PLAYER2_BUTTON:
		case ICS_PLAYER3_BUTTON:
		case ICS_PLAYER4_BUTTON:
		case ICS_PLAYER5_BUTTON:
			if(!(bt -> flags & ICS_HIDDEN_OBJECT)){
				iChatClearButtons();
				bt -> CurState = 1;
				iChatFilter = MESSAGE_FOR_PLAYER;
				iChatFilterID = bt -> extData;
			}
			break;
	}
}

void iChatClearButtons(void)
{
	iChatButton* p = (iChatButton*)iChatButtons -> fPtr;
	while(p){
		switch(p -> ID){
			case ICS_ALL_BUTTON:
			case ICS_YELLOW_BUTTON:
			case ICS_ORANGE_BUTTON:
			case ICS_BLUE_BUTTON:
			case ICS_GREEN_BUTTON:
			case ICS_RED_BUTTON:
			case ICS_WHITE_BUTTON:
			case ICS_GRAY_BUTTON:
			case ICS_BLACK_BUTTON:
			case ICS_CAMOUFLAGE_BUTTON:
			case ICS_PATROL_BUTTON:
			case ICS_PLAYER1_BUTTON:
			case ICS_PLAYER2_BUTTON:
			case ICS_PLAYER3_BUTTON:
			case ICS_PLAYER4_BUTTON:
			case ICS_PLAYER5_BUTTON:
				p -> CurState = 0;
				break;
		}
		p = (iChatButton*)p -> next;
	}
}

iChatButton* iGetChatPlayerButton(int id)
{
	iChatButton* p = (iChatButton*)iChatButtons -> fPtr;
	while(p){
		switch(p -> ID){
			case ICS_ALL_BUTTON:
			case ICS_YELLOW_BUTTON:
			case ICS_ORANGE_BUTTON:
			case ICS_BLUE_BUTTON:
			case ICS_GREEN_BUTTON:
			case ICS_RED_BUTTON:
			case ICS_WHITE_BUTTON:
			case ICS_GRAY_BUTTON:
			case ICS_BLACK_BUTTON:
			case ICS_CAMOUFLAGE_BUTTON:
			case ICS_PATROL_BUTTON:
			case ICS_PLAYER1_BUTTON:
			case ICS_PLAYER2_BUTTON:
			case ICS_PLAYER3_BUTTON:
			case ICS_PLAYER4_BUTTON:
			case ICS_PLAYER5_BUTTON:
				if(p -> extData == id) return p;
				break;
		}
		p = (iChatButton*)p -> next;
	}
	return 0;
}

void iChatInputDrawCursor(void) {
	iChatInput -> XConv -> init();
	*iChatInput -> XConv < CurPlayerName < (char*)((iChatInput -> string).substr(0, iChatInput -> position).c_str());
	char* ptr = iChatInput -> XConv -> address();
	int x = aTextWidth32(ptr, iChatInput -> font, 1);
	
	int color;
	if (iScreenChat) {
		color = cursorColorI;
	}
	else {
		color = cursorColor;
	}
	
	XGR_Rectangle(iChatInput -> PosX + x - cursorWidth / 2, iChatInput -> PosY, cursorWidth, iChatInput -> SizeY, color, color, XGR_FILLED);
}

void iChatInputChar(char* input_char) {
	unsigned char chr;
	
	aciFont* hfnt = aScrFonts32[iChatInput->font];

	if ((unsigned char)(input_char[0]) < 128) {
		chr = input_char[0];
	} 
	else {
		unsigned short utf = ((unsigned short*)input_char)[0];
		utf = ntohs(utf);
		// All cyrilic chars should be coding in two octets. 8, 11 bit-index for 1,2 octets
		if ((utf & (1<<(7))) && !(utf & (1<<(10)))) {
			chr = UTF8toCP866(utf);
		} 
		else {
			chr = ' ';
		}
	}
	
	if(hfnt && chr < hfnt->Size) {
		iChatInputDrawCursor();
		iChatCursorFlag = true;
		iChatCursorTimer = 0;
		
		int leftPosition = std::min(iChatInput -> position, iChatInput -> selectionPosition);
		int rightPosition = std::max(iChatInput -> position, iChatInput -> selectionPosition);
		
		std::string new_string = (iChatInput -> string).substr(0, leftPosition);
		new_string.push_back(chr);
		new_string += (iChatInput -> string).substr(rightPosition);
		
		iChatInput -> XConv -> init();
		*iChatInput -> XConv < CurPlayerName < (char*)(new_string.c_str());
		char* ptr = iChatInput -> XConv -> address();

		int x = aTextWidth32(ptr, iChatInput -> font, 1);
		//	if(sz >= ISC_MAX_STRING_LEN && x >= iChatInput->SizeX - 10) {
		if ((new_string.length() > ISC_MAX_STRING_LEN) || (x > iChatInput -> SizeX - 10)) {
			return;
		}
		
		iChatInput -> string = new_string;
		iChatInput -> position = leftPosition + 1;
		iChatInput -> selectionPosition = iChatInput -> position;
	}
}

void iChatInputChar(SDL_Event *event) {
	if (event -> type == SDL_TEXTINPUT) {
		iChatInputChar(event -> text.text);
	}
}

void iChatInputFlush(void) {
	iChatInputDrawCursor();
	iChatCursorFlag = true;
	iChatCursorTimer = 0;

	if ((iChatInput -> string).length() > 1) {
		message_dispatcher.send((char*)((iChatInput -> string).substr(1).c_str()), iChatFilter, iChatFilterID);
	}
	
	iChatInput -> string = ">";
	iChatInput -> position = 1;
	iChatInput -> selectionPosition = 1;
}

void iChatInputBack(void) {
	iChatInputDrawCursor();
	iChatCursorTimer = 0;
	
	std::string new_string;
	if (iChatInput -> position == iChatInput -> selectionPosition) {
		if (iChatInput -> position <= 1) {
			return;
		}
		
		new_string = (iChatInput -> string).substr(0, iChatInput -> position - 1) + (iChatInput -> string).substr(iChatInput -> position);
		iChatInput -> position -= 1;
	}
	else {
		int leftPosition = std::min(iChatInput -> position, iChatInput -> selectionPosition);
		int rightPosition = std::max(iChatInput -> position, iChatInput -> selectionPosition);
		
		new_string = (iChatInput -> string).substr(0, leftPosition) + (iChatInput -> string).substr(rightPosition);
		iChatInput -> position = leftPosition;
	}
	
	iChatInput -> string = new_string;
	iChatInput -> selectionPosition = iChatInput -> position;
}

void iChatInputEditing(SDL_Event *event) {
	SDL_Scancode scancode = event -> key.keysym.scancode;
	
	if (scancode == SDL_SCANCODE_LEFT) {
		iChatInputDrawCursor();
		iChatCursorFlag = true;
		iChatCursorTimer = 0;
		
		int position = iChatInput -> position;
		
		if (SDL_GetModState() & KMOD_CTRL) {
			std::string string = iChatInput -> string;
			int group = getCharGroup(string[position - 1]);
			
			while (position > 1 && getCharGroup(string[position - 1]) == group) {
				position -= 1;
			}
		}
		else if (!(SDL_GetModState() & KMOD_SHIFT) && position != iChatInput -> selectionPosition) {
			position = std::min(position, iChatInput -> selectionPosition);
		}
		else {
			position = std::max(1, position - 1);
		}
		
		iChatInput -> position = position;
		if (!(SDL_GetModState() & KMOD_SHIFT)) {
			iChatInput -> selectionPosition = position;
		}
	}
	else if (scancode == SDL_SCANCODE_RIGHT) {
		iChatInputDrawCursor();
		iChatCursorFlag = true;
		iChatCursorTimer = 0;
		
		int position = iChatInput -> position;
		
		if (SDL_GetModState() & KMOD_CTRL) {
			std::string string = iChatInput -> string;
			int group = getCharGroup(string[position]);
			
			while (position < (int)string.length() && getCharGroup(string[position]) == group) {
				position += 1;
			}
		}
		else if (!(SDL_GetModState() & KMOD_SHIFT) && position != iChatInput -> selectionPosition) {
			position = std::max(position, iChatInput -> selectionPosition);
		}
		else {
			position = std::min(position + 1, (int)((iChatInput -> string).length()));
		}
		
		iChatInput -> position = position;
		if (!(SDL_GetModState() & KMOD_SHIFT)) {
			iChatInput -> selectionPosition = position;
		}
	}
	else if (scancode == SDL_SCANCODE_UP) {
		iChatInputDrawCursor();
		iChatCursorFlag = true;
		iChatCursorTimer = 0;
		
		iChatInput -> position = 1;
		if (!(SDL_GetModState() & KMOD_SHIFT)) {
			iChatInput -> selectionPosition = iChatInput -> position;
		}
	}
	else if (scancode == SDL_SCANCODE_DOWN) {
		iChatInputDrawCursor();
		iChatCursorFlag = true;
		iChatCursorTimer = 0;
		
		iChatInput -> position = (iChatInput -> string).length();
		if (!(SDL_GetModState() & KMOD_SHIFT)) {
			iChatInput -> selectionPosition = iChatInput -> position;
		}
	}
	else if (scancode == SDL_SCANCODE_A && (SDL_GetModState() & KMOD_CTRL)) {
		iChatInput -> position = (iChatInput -> string).length();
		iChatInput -> selectionPosition = 1;
	}
	else if (scancode == SDL_SCANCODE_V && (SDL_GetModState() & KMOD_CTRL) && SDL_HasClipboardText() == SDL_TRUE) {
		char* clipboard = SDL_GetClipboardText();
		
		for (size_t i = 0; i < strlen(clipboard); i++) {
			char* chr = new char[2]{ clipboard[i], 0 };
			if ((unsigned char)(chr[0]) >= 128) {
				chr[1] = clipboard[i + 1];
				i++;
			}

			iChatInputChar(chr);
		}
	}
	else if ((scancode == SDL_SCANCODE_C || scancode == SDL_SCANCODE_X) && (SDL_GetModState() & KMOD_CTRL)) {
		int leftPosition = std::min(iChatInput -> position, iChatInput -> selectionPosition);
		int rightPosition = std::max(iChatInput -> position, iChatInput -> selectionPosition);
		
		if (leftPosition == rightPosition) {
			return;
		}
		
		std::string copy_string = (iChatInput -> string).substr(leftPosition, rightPosition - leftPosition);
		std::string new_string;
		for (size_t i = 0; i < copy_string.length(); i++) {
			if ((unsigned char)(copy_string[i]) < 128) {
				new_string.push_back(copy_string[i]);
			}
			else {
				unsigned short utf = CP866toUTF8((unsigned char)(copy_string[i]));
				new_string.push_back(utf >> 8);
				new_string.push_back(utf & 0xFF);
			}
		}
		
		SDL_SetClipboardText(new_string.c_str());
		
		if (scancode == SDL_SCANCODE_X) {
			iChatInputBack();
		}
	}
}

//returns "group" of the char: 0 - space, 1 - symbol, 2 - letter/number
int getCharGroup(char chr) {
	if (chr == ' ') {
		return 0;
	}
	else {
		std::string symbols = "~`!@\"#$;%^:&?*()_-+=[{]}\\|/\'<>,.";
		
		for (size_t i = 0; i < symbols.length(); i++) {
			if (chr == symbols[i]) {
				return 1;
			}
		}
		
		return 2;
	}
}

unsigned short CP866toUTF8(unsigned char cp866) {
	if (cp866 >= 0x80 && cp866 <= 0xAF) {
		return cp866 + 0xD010;
	}
	if (cp866 >= 0xE0 && cp866 <= 0xEF) {
		return cp866 + 0xD0A0;
	}
	
	switch(cp866) {
		case 0xF0: //Ё
			return 0xD001;
		case 0xF1: //ё
			return 0xD191;
		default:
			return 0x20;
	}
}

void iInitChatScreen(void)
{
	int i;
	int teamFilterID = ICS_GREEN_BUTTON + iChatFilterID;
	iChatButton* p;
	MessageElement* el;

	iInitChatButtons();
	iChatClearButtons();

	if (iChatFilterID > 3) {
		teamFilterID = teamFilterID + 9;
	}

	switch(iChatFilter){
		case MESSAGE_FOR_ALL:
			p = iGetChatButton(ICS_ALL_BUTTON);
			p -> CurState = 1;
			break;
		case MESSAGE_FOR_TEAM:
			p = iGetChatButton(teamFilterID);
			p -> CurState = 1;
			break;
		case MESSAGE_FOR_PLAYER:
			if(!players_list.find(iChatFilterID)){
				p = iGetChatButton(ICS_ALL_BUTTON);
				p -> CurState = 1;
				iChatFilter = MESSAGE_FOR_ALL;
				iChatFilterID = 0;
			}
			p = iGetChatPlayerButton(iChatFilterID);
			if(!p){
				iChatSkip = 0;
				while(!p){
					iInitChatButtons();
					p = iGetChatPlayerButton(iChatFilterID);
					if(!p) iChatSkip ++;
				}
			}
			p -> CurState = 1;
			break;
	}
	i = 0;
	el = message_dispatcher.first();
	iChatHistory -> clear();
	if(!iChatMUTE){
		while(el){
			iChatHistory -> add_str(el -> message,i,el -> color);
//			  iChatHistory -> add_str(el -> message,i);
			i ++;
			el = (MessageElement*)el -> next;
		}
	}
}

void iInitChatButtons(void)
{
	int i;
	PlayerData* p = players_list.first();
	iChatButton* bt;

	if(!p) return;

	if(players_list.size() < 5){
		iChatSkip = 0;
		bt = iGetChatButton(ICS_UP_BUTTON);
		bt -> flags |= ICS_HIDDEN_OBJECT;
		bt = iGetChatButton(ICS_DN_BUTTON);
		bt -> flags |= ICS_HIDDEN_OBJECT;
	}
	else {
		bt = iGetChatButton(ICS_UP_BUTTON);
		bt -> flags &= ~ICS_HIDDEN_OBJECT;
		bt = iGetChatButton(ICS_DN_BUTTON);
		bt -> flags &= ~ICS_HIDDEN_OBJECT;
	}

	for(i = 0; i < 5; i ++){
		bt = iGetChatButton(ICS_PLAYER1_BUTTON + i);
		bt -> flags |= ICS_HIDDEN_OBJECT;
	}
	for(i = 0; i < iChatSkip; i ++){
		if(p){
			p = (PlayerData*)p -> next;
		}
		else {
			iChatSkip = 0;
			p = players_list.first();
		}
	}
	for(i = 0; i < 5; i ++){
		if(p -> client_ID == GlobalStationID)
		p = (PlayerData*)p -> next;
		if(!p) break;
		bt = iGetChatButton(ICS_PLAYER1_BUTTON + i);
		iResBuf -> init();
		*iResBuf < p -> name < " " < iChatWorlds[p -> body.world + 1] < "/" < iChatColors[p -> body.color];
		bt -> set_string(iResBuf -> address());
		bt -> flags &= ~ICS_HIDDEN_OBJECT;
		bt -> extData = p -> client_ID;
		p = (PlayerData*)p -> next;
		if(!p) break;
	}
}

iChatButton* iGetChatButton(int id)
{
	iChatButton* p = (iChatButton*)iChatButtons -> fPtr;
	while(p){
		if(p -> ID == id) return p;
		p = (iChatButton*)p -> next;
	}
	return NULL;
}

void iChatClearLog(void)
{
	MessageElement* p,*p1;
	p = message_dispatcher.first();
	while(p){
		p1 = (MessageElement*)p -> next;
		delete p;
		p = p1;
	}
	message_dispatcher.ClearList();
}

void iChatHistoryScreen::add_str(char* str,int id,int col)
{
	strcpy(data[id],str);
	ColorData[id] = (iScreenChat) ? aciChatColors0[col] : aciChatColors1[col];
}

