/* ---------------------------- INCLUDE SECTION ----------------------------- */


#include "../global.h"
#include "lang.h"

#include "../network.h"
#include "controls.h"
#include "i_chat.h"
#include "i_str.h"
#include "hfont.h"
#include "iscreen.h"
#include "../actint/item_api.h"
#include "../actint/actint.h"

#include "../network.h"
#include "../text/legacy_codec.h"
#include "../text/legacy_ttf_draw.h"
#include "../text/unicode.h"

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
void iChatInputChar(const char* input_text);
void iChatInputInsertText(std::string_view utf8_text);
int iChatTextWidth(const char* text,int font,int hspace);
int iChatTextHeight(const char* text,int font,int vspace);
void iChatOutText(int x,int y,int color,void* text,int font,int hspace,int vspace);
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

int getCharGroup(uint32_t codepoint);
/* --------------------------- DEFINITION SECTION --------------------------- */

#define ICS_INPUT_TIMER 	10
#define ICS_INPUT_MAX_LENGTH	200

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

iChatInputField iChatInputPrev;

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

int counterColorI = 2 + (3 << 16); //ICS_iSTRING_COL0 gray color for counter
int counterColor = 227 + (3 << 16); //ICS_STRING_COL0

int scrollBgColorI = 2 + (3 << 16); //ICS_iSTRING_COL0 gray color for scroll background
int scrollBgColor = 227 + (3 << 16); //ICS_STRING_COL0
int scrollColorI = (6 | (2 << 16)); //aciChatColors0[5] white color for scroll
int scrollColor = (230 | (2 << 16)); //aciChatColors1[5]
int scrollSizeX = 5;

static text::LegacyEncoding iChatTextEncoding(void)
{
	return lang() == RUSSIAN ? text::LegacyEncoding::CP866 : text::LegacyEncoding::ASCII;
}

static std::shared_ptr<text::TtfFontFace> iChatGetTtfFace(int font)
{
	if(font < 0 || font >= AS_NUM_FONTS_32)
		return nullptr;
	if(!aScrFonts32 || !aScrFonts32[font])
		return nullptr;

	return text::default_ui_ttf_face(aScrFonts32[font] -> SizeY, TTF_HINTING_NORMAL, false, 0);
}

static int iChatUtf8Length(const std::string& text)
{
	return (int)text::utf8_length(text);
}

static std::string iChatUtf8Substr(const std::string& text,int start_position,int end_position)
{
	if(end_position < start_position)
		end_position = start_position;
	return text::utf8_substr_by_codepoints(text, (size_t)start_position, (size_t)(end_position - start_position));
}

static std::string iChatPlayerNameUtf8(void)
{
	return text::legacy_to_utf8(CurPlayerName ? CurPlayerName : "", iChatTextEncoding());
}

static std::string iChatInputDisplayUtf8(int start_position,int end_position)
{
	return iChatPlayerNameUtf8() + iChatUtf8Substr(iChatInput -> string, start_position, end_position);
}

static int iChatUtf8TextWidth(const std::string& text,int font,int hspace)
{
	auto face = iChatGetTtfFace(font);
	if(face)
		return text::measure_utf8_text_width(text, *face, hspace);

	std::string legacy_text = text::utf8_to_cp866_lossy(text,' ');
	return iChatTextWidth(legacy_text.c_str(), font, hspace);
}

static void iChatUtf8OutText(int x,int y,int color,const std::string& text,int font,int hspace,int vspace)
{
	auto face = iChatGetTtfFace(font);
	if(face){
		text::draw_utf8_text_8bit(x, y, color, text, *face, hspace, vspace, false);
		return;
	}

	std::string legacy_text = text::utf8_to_cp866_lossy(text,' ');
	iChatOutText(x, y, color, (void*)legacy_text.c_str(), font, hspace, vspace);
}

static uint32_t iChatCodepointAt(const std::string& text_value,int position)
{
	return text::utf8_codepoint_at(text_value, (size_t)position, ' ');
}

int iChatTextWidth(const char* text,int font,int hspace)
{
	auto face = iChatGetTtfFace(font);
	if(face)
		return text::measure_legacy_ttf_text_width(text ? text : "", *face, iChatTextEncoding(), hspace);

	return aTextWidth32((void*)(text ? text : ""), font, hspace);
}

int iChatTextHeight(const char* text,int font,int vspace)
{
	auto face = iChatGetTtfFace(font);
	if(face)
		return text::measure_legacy_ttf_text_height(text ? text : "", *face, vspace);

	return aTextHeight32((void*)(text ? text : ""), font, vspace);
}

void iChatOutText(int x,int y,int color,void* text_ptr,int font,int hspace,int vspace)
{
	const char* text = text_ptr ? (const char*)text_ptr : "";
	auto face = iChatGetTtfFace(font);
	if(face){
		text::draw_legacy_ttf_text_8bit(x, y, color, text, *face, iChatTextEncoding(), hspace, vspace, false);
		return;
	}

	aOutText32(x, y, color, (void*)text, font, hspace, vspace);
}

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

	cursorPosition = 0;
	selectionPosition = 0;
	leftDrawPosition = 0;
	rightDrawPosition = 0;
}

void iChatInputField::redraw(void)
{
	iChatScreenObject::redraw();

	this -> selectionRedraw();
	this -> counterRedraw();

	iChatUtf8OutText(PosX, PosY, color, iChatInputDisplayUtf8(leftDrawPosition, rightDrawPosition), font, 1, 0);
}

void iChatInputField::selectionRedraw(void) {
	int color;
	if (iScreenChat) {
		color = selectionColorI;
	}
	else {
		color = selectionColor;
	}

	int leftSelectionPosition = std::min(std::max(std::min(cursorPosition, selectionPosition), leftDrawPosition), rightDrawPosition);
	int rightSelectionPosition = std::min(std::max(std::max(cursorPosition, selectionPosition), leftDrawPosition), rightDrawPosition);

	int left_x = iChatUtf8TextWidth(iChatInputDisplayUtf8(leftDrawPosition, leftSelectionPosition), font, 1);
	int right_x = iChatUtf8TextWidth(iChatInputDisplayUtf8(leftDrawPosition, rightSelectionPosition), font, 1);

	XGR_Rectangle(PosX + left_x, PosY, right_x - left_x, SizeY, color, color, XGR_FILLED);
}

void iChatInputField::counterRedraw(void) {
	int color;
	if (iScreenChat) {
		color = counterColorI;
	}
	else {
		color = counterColor;
	}

	XConv -> init();
	*XConv < (char*)((std::to_string(ICS_INPUT_MAX_LENGTH)).c_str());
	int x = iChatTextWidth(XConv -> address(), font, 1);

	XConv -> init();
	*XConv < (char*)((std::to_string(ICS_INPUT_MAX_LENGTH - (iChatUtf8Length(string) - 1))).c_str());

	XGR_Rectangle(PosX - x - 10, PosY, x + 10, SizeY, fonColor, fonColor, XGR_FILLED);
	iChatOutText(PosX - x - 10, PosY, color, XConv -> address(), font, 1, 0);
}

int iChatInputField::getLeftDrawPositionByRight(int rightPosition) {
	rightPosition = std::min(iChatUtf8Length(string), rightPosition);

	int leftPosition = rightPosition - 1;
	while (leftPosition >= 1) {
		if (iChatUtf8TextWidth(iChatInputDisplayUtf8(leftPosition, rightPosition), font, 1) > SizeX - 10) {
			break;
		}

		leftPosition -= 1;
	}

	return leftPosition + 1;
}

int iChatInputField::getRightDrawPositionByLeft(int leftPosition) {
	leftPosition = std::max(1, leftPosition);

	int rightPosition = leftPosition + 1;
	while (rightPosition <= iChatUtf8Length(string)) {
		if (iChatUtf8TextWidth(iChatInputDisplayUtf8(leftPosition, rightPosition), font, 1) > SizeX - 10) {
			break;
		}

		rightPosition += 1;
	}

	return rightPosition - 1;
}

iChatHistoryScreen::iChatHistoryScreen(void) {
	position = 0;
}

void iChatHistoryScreen::redraw(void) {
	iChatScreenObject::redraw();

	this -> redrawScroll();

	int y = 0;

	for(int i = position; i < std::min((int)(data.size()), position + ICS_HISTORY_MAX_MESSAGES); i++){
		if (data[i].text.length()) {
			iChatOutText(PosX, PosY + y, data[i].color, (char*)(data[i].text.c_str()), font, 1, 0);
		}
		y += iChatTextHeight((char*)(data[i].text.c_str()), font, 0) + 2;
	}
}

void iChatHistoryScreen::redrawScroll(void) {
	int bgColor, color;
	if (iScreenChat) {
		bgColor = scrollBgColorI;
		color = scrollColorI;
	}
	else {
		bgColor = scrollBgColor;
		color = scrollColor;
	}

	int messages_num = std::max(ICS_HISTORY_MAX_MESSAGES, (int)(data.size()));
	float percent = (float)position / messages_num;
	float percentSizeY = (float)ICS_HISTORY_MAX_MESSAGES / messages_num;

	XGR_Rectangle(PosX + SizeX, PosY, scrollSizeX, SizeY, bgColor, bgColor, XGR_FILLED);
	XGR_Rectangle(PosX + SizeX, PosY + round(percent * SizeY), scrollSizeX, round(percentSizeY * SizeY), color, color, XGR_FILLED);
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
	dx = (SizeX - iChatTextWidth(string,font,1)) / 2;
	dy = (SizeY - iChatTextHeight(string,font,0)) / 2;

	iChatOutText(PosX + dx,PosY + dy,StateColors[CurState],string,font,1,0);
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

	sx = ICS_HISTORY_SIZE_X + ICS_DELTA + ICS_BUTTON_SIZE_X;
	sy = ICS_HISTORY_SIZE_Y + ICS_DELTA * 2 + ICS_BUTTON_SIZE_Y * 2;

//	  bsx = (sx - ICS_DELTA * 2) / 3;
	bsx = (ICS_HISTORY_SIZE_X - ICS_DELTA * 2) / 3;

	x = (XGR_MAXX - sx) / 2;
	y = (XGR_MAXY - sy) / 2;

	if(!iScreenChat)
		bcol = ICS_BORDER_COLOR;
	else {
		y -= 30;
		x -= (XGR_MAXX - 800) / 2;
		y -= (XGR_MAXY - 600) / 2;
		bcol = ICS_iBORDER_COLOR;
	}

	iChatButtons = new XList;

	iChatHistory = new iChatHistoryScreen;
	iChatHistory -> init(x,y + (ICS_BUTTON_SIZE_Y + ICS_DELTA) * 1,ICS_HISTORY_SIZE_X,ICS_HISTORY_SIZE_Y,bcol,bcol);
	iChatHistory -> position = std::max(0, message_dispatcher.ListSize - ICS_HISTORY_MAX_MESSAGES);

	iChatInput = new iChatInputField;
	iChatInput -> init(x,y + ICS_HISTORY_SIZE_Y + ICS_DELTA + (ICS_BUTTON_SIZE_Y + ICS_DELTA) * 1,ICS_HISTORY_SIZE_X,ICS_BUTTON_SIZE_Y,bcol,bcol);
	iChatInput -> string = ">";
	iChatInput -> cursorPosition = 1;
	iChatInput -> selectionPosition = 1;
	iChatInput -> leftDrawPosition = 1;
	iChatInput -> rightDrawPosition = 1;
	iChatInputPrev = *iChatInput;

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
	if (!k) {
		return;
	}

	iChatInputChar(k);
	iChatInputEditing(k);

	if (k -> type != SDL_KEYDOWN) {
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
	int color;
	if (iScreenChat) {
		color = cursorColorI;
	}
	else {
		color = cursorColor;
	}

	int cursorPosition = std::min(iChatInput -> rightDrawPosition, std::max(iChatInput -> leftDrawPosition, iChatInput -> cursorPosition));

	int x = iChatUtf8TextWidth(iChatInputDisplayUtf8(iChatInput -> leftDrawPosition, cursorPosition), iChatInput -> font, 1);

	XGR_Rectangle(iChatInput -> PosX + x - cursorWidth / 2, iChatInput -> PosY, cursorWidth, iChatInput -> SizeY, color, color, XGR_FILLED);
}

void iChatInputChar(const char* input_text) {
	if(!input_text || !*input_text)
		return;
	iChatInputInsertText(input_text);
}

void iChatInputInsertText(std::string_view utf8_text)
{
	int leftSelectionPosition = std::min(iChatInput -> cursorPosition, iChatInput -> selectionPosition);
	int rightSelectionPosition = std::max(iChatInput -> cursorPosition, iChatInput -> selectionPosition);

	std::string new_string = iChatUtf8Substr(iChatInput -> string, 0, leftSelectionPosition);

	size_t offset = 0;
	uint32_t codepoint = 0;
	while(text::utf8_next(utf8_text, offset, codepoint)){
		if(codepoint < 32)
			codepoint = ' ';
		text::append_utf8(new_string, codepoint);
	}

	new_string += iChatUtf8Substr(iChatInput -> string, rightSelectionPosition, iChatUtf8Length(iChatInput -> string));

	if(iChatUtf8Length(new_string) - 1 > ICS_INPUT_MAX_LENGTH)
		return;

	iChatCursorFlag = true;
	iChatCursorTimer = 0;

	iChatInputPrev = *iChatInput;

	iChatInput -> string = new_string;
	const int inserted_length = iChatUtf8Length(std::string(utf8_text));
	iChatInput -> cursorPosition = leftSelectionPosition + inserted_length;
	iChatInput -> selectionPosition = iChatInput -> cursorPosition;
	iChatInput -> rightDrawPosition = std::max(std::min(iChatInput -> rightDrawPosition, iChatUtf8Length(iChatInput -> string)), iChatInput -> cursorPosition + 1);
	iChatInput -> leftDrawPosition = iChatInput -> getLeftDrawPositionByRight(iChatInput -> rightDrawPosition);
}

void iChatInputChar(SDL_Event *event) {
	if (event && event -> type == SDL_TEXTINPUT) {
		iChatInputChar(event -> text.text);
	}
}

void iChatInputFlush(void) {
	iChatCursorFlag = true;
	iChatCursorTimer = 0;

	if (iChatUtf8Length(iChatInput -> string) > 1) {
		std::string string = iChatInput -> string;

		int leftPosition = 1;
		for (int rightPosition = 2; rightPosition <= iChatUtf8Length(string); rightPosition++) {
			std::string display_line = iChatPlayerNameUtf8() + ": " + iChatUtf8Substr(string, leftPosition, rightPosition);

			if (iChatUtf8TextWidth(display_line, iChatInput -> font, 1) > iChatHistory -> SizeX - 10) {
				std::string send_text = text::utf8_to_cp866_lossy(iChatUtf8Substr(string, leftPosition, rightPosition - 1),' ');
				message_dispatcher.send((char*)send_text.c_str(), iChatFilter, iChatFilterID);
				leftPosition = rightPosition - 1;
			}
		}

		std::string send_text = text::utf8_to_cp866_lossy(iChatUtf8Substr(string, leftPosition, iChatUtf8Length(string)),' ');
		message_dispatcher.send((char*)send_text.c_str(), iChatFilter, iChatFilterID);
	}

	iChatInput -> string = ">";
	iChatInput -> cursorPosition = 1;
	iChatInput -> selectionPosition = 1;
	iChatInput -> rightDrawPosition = 1;
	iChatInput -> leftDrawPosition = 1;

	iChatInputPrev = *iChatInput;

	// jump to the end of new history after sending message
	iChatHistory -> position = std::max(0, message_dispatcher.ListSize - ICS_HISTORY_MAX_MESSAGES);
}

void iChatInputBack(void) {
	iChatCursorFlag = true;
	iChatCursorTimer = 0;

	if (iChatInput -> cursorPosition == iChatInput -> selectionPosition && iChatInput -> cursorPosition <= 1) {
		return;
	}

	iChatInputPrev = *iChatInput;

	std::string new_string;
	if (iChatInput -> cursorPosition == iChatInput -> selectionPosition) {
		new_string = iChatUtf8Substr(iChatInput -> string, 0, iChatInput -> cursorPosition - 1) +
		             iChatUtf8Substr(iChatInput -> string, iChatInput -> cursorPosition, iChatUtf8Length(iChatInput -> string));
		iChatInput -> cursorPosition -= 1;
	}
	else {
		int leftSelectionPosition = std::min(iChatInput -> cursorPosition, iChatInput -> selectionPosition);
		int rightSelectionPosition = std::max(iChatInput -> cursorPosition, iChatInput -> selectionPosition);

		new_string = iChatUtf8Substr(iChatInput -> string, 0, leftSelectionPosition) +
		             iChatUtf8Substr(iChatInput -> string, rightSelectionPosition, iChatUtf8Length(iChatInput -> string));
		iChatInput -> cursorPosition = leftSelectionPosition;
	}

	iChatInput -> string = new_string;
	iChatInput -> selectionPosition = iChatInput -> cursorPosition;
	if (iChatInput -> cursorPosition <= iChatInput -> leftDrawPosition) {
		iChatInput -> leftDrawPosition = iChatInput -> getLeftDrawPositionByRight(iChatInput -> cursorPosition + 1);
		iChatInput -> rightDrawPosition = iChatInput -> getRightDrawPositionByLeft(iChatInput -> leftDrawPosition);
	}
	else {
		iChatInput -> rightDrawPosition = std::min(iChatInput -> rightDrawPosition, iChatUtf8Length(iChatInput -> string));
		iChatInput -> leftDrawPosition = iChatInput -> getLeftDrawPositionByRight(iChatInput -> rightDrawPosition);
	}
}

void iChatInputEditing(SDL_Event *event) {
	if (event -> type == SDL_MOUSEWHEEL) {
		int x, y;
		SDL_GetMouseState(&x, &y);

		iChatHistory -> SizeX += scrollSizeX; // check history bounds with scroll
		if (iChatHistory -> check_xy(x, y)) {
			if (event -> wheel.y > 0) {
				iChatHistory -> scrollUp();
			}
			else if (event -> wheel.y < 0) {
				iChatHistory -> scrollDown();
			}
		}
		iChatHistory -> SizeX -= scrollSizeX;
	}
	else if (event -> type == SDL_KEYDOWN) {
		SDL_Keycode keycode = event -> key.keysym.sym;
		uint16_t keymod = event -> key.keysym.mod;

		// because SDL doesn't know what numlock is
		if (!(keymod & KMOD_NUM)) {
			switch (keycode) {
	 			case SDLK_KP_2:
					keycode = SDLK_DOWN;
					break;
				case SDLK_KP_4:
					keycode = SDLK_LEFT;
					break;
				case SDLK_KP_6:
					keycode = SDLK_RIGHT;
					break;
				case SDLK_KP_8:
					keycode = SDLK_UP;
					break;
			}
		}

		if (keycode == SDLK_LEFT) {
			iChatCursorFlag = true;
			iChatCursorTimer = 0;

			int position = iChatInput -> cursorPosition;

			if (keymod & KMOD_CTRL) {
				int group = getCharGroup(iChatCodepointAt(iChatInput -> string, position - 1));

				while (position > 1 && getCharGroup(iChatCodepointAt(iChatInput -> string, position - 1)) == group) {
					position -= 1;
				}
			}
			else if (!(keymod & KMOD_SHIFT) && position != iChatInput -> selectionPosition) {
				position = std::min(position, iChatInput -> selectionPosition);
			}
			else {
				position = std::max(1, position - 1);
			}

			iChatInput -> cursorPosition = position;
			if (!(keymod & KMOD_SHIFT)) {
				iChatInput -> selectionPosition = position;
			}

			iChatInput -> leftDrawPosition = std::min(iChatInput -> leftDrawPosition, position);
			iChatInput -> rightDrawPosition = iChatInput -> getRightDrawPositionByLeft(iChatInput -> leftDrawPosition);
		}
		else if (keycode == SDLK_RIGHT) {
			iChatCursorFlag = true;
			iChatCursorTimer = 0;

			int position = iChatInput -> cursorPosition;

			if (keymod & KMOD_CTRL) {
				int group = getCharGroup(iChatCodepointAt(iChatInput -> string, position));

				while (position < iChatUtf8Length(iChatInput -> string) && getCharGroup(iChatCodepointAt(iChatInput -> string, position)) == group) {
					position += 1;
				}
			}
			else if (!(keymod & KMOD_SHIFT) && position != iChatInput -> selectionPosition) {
				position = std::max(position, iChatInput -> selectionPosition);
			}
			else {
				position = std::min(position + 1, iChatUtf8Length(iChatInput -> string));
			}

			iChatInput -> cursorPosition = position;
			if (!(keymod & KMOD_SHIFT)) {
				iChatInput -> selectionPosition = position;
			}

			iChatInput -> rightDrawPosition = std::max(iChatInput -> rightDrawPosition, position);
			iChatInput -> leftDrawPosition = iChatInput -> getLeftDrawPositionByRight(iChatInput -> rightDrawPosition);
		}
		else if (keycode == SDLK_UP) {
			iChatCursorFlag = true;
			iChatCursorTimer = 0;

			if (iChatInput -> cursorPosition == 1) {
				iChatHistory -> scrollUp();
			}

			iChatInput -> cursorPosition = 1;
			if (!(keymod & KMOD_SHIFT)) {
				iChatInput -> selectionPosition = iChatInput -> cursorPosition;
			}

			iChatInput -> leftDrawPosition = 1;
			iChatInput -> rightDrawPosition = iChatInput -> getRightDrawPositionByLeft(iChatInput -> leftDrawPosition);
		}
		else if (keycode == SDLK_DOWN) {
			iChatCursorFlag = true;
			iChatCursorTimer = 0;

			if (iChatInput -> cursorPosition == iChatUtf8Length(iChatInput -> string)) {
				iChatHistory -> scrollDown();
			}

			iChatInput -> cursorPosition = iChatUtf8Length(iChatInput -> string);
			if (!(keymod & KMOD_SHIFT)) {
				iChatInput -> selectionPosition = iChatInput -> cursorPosition;
			}

			iChatInput -> rightDrawPosition = iChatInput -> cursorPosition;
			iChatInput -> leftDrawPosition = iChatInput -> getLeftDrawPositionByRight(iChatInput -> rightDrawPosition);
		}
		else if (keycode == SDLK_a && (keymod & KMOD_CTRL)) {
			iChatInput -> cursorPosition = iChatUtf8Length(iChatInput -> string);
			iChatInput -> selectionPosition = 1;
			iChatInput -> rightDrawPosition = iChatInput -> cursorPosition;
			iChatInput -> leftDrawPosition = iChatInput -> getLeftDrawPositionByRight(iChatInput -> rightDrawPosition);
		}
		else if (keycode == SDLK_v && (keymod & KMOD_CTRL) && SDL_HasClipboardText() == SDL_TRUE) {
			iChatInputField prevBuf = *iChatInput;

			char* clipboard = SDL_GetClipboardText();
			iChatInputInsertText(clipboard ? clipboard : "");

			SDL_free(clipboard);
			iChatInputPrev = prevBuf;
		}
		else if ((keycode == SDLK_c || keycode == SDLK_x) && (keymod & KMOD_CTRL)) {
			int leftSelectionPosition = std::min(iChatInput -> cursorPosition, iChatInput -> selectionPosition);
			int rightSelectionPosition = std::max(iChatInput -> cursorPosition, iChatInput -> selectionPosition);

			if (leftSelectionPosition == rightSelectionPosition) {
				return;
			}

			std::string copy_string = iChatUtf8Substr(iChatInput -> string, leftSelectionPosition, rightSelectionPosition);

			SDL_SetClipboardText(copy_string.c_str());

			if (keycode == SDLK_x) {
				iChatInputField prevBuf = *iChatInput;
				iChatInputBack();
				iChatInputPrev = prevBuf;
			}
		}
		else if (keycode == SDLK_z && (keymod & KMOD_CTRL)) {
			iChatInputField prevBuf = *iChatInput;
			*iChatInput = iChatInputPrev;
			iChatInputPrev = prevBuf;
		}
	}
}

//returns "group" of the char: 0 - space, 1 - symbol, 2 - letter/number
int getCharGroup(uint32_t codepoint) {
	if(codepoint == ' ' || codepoint == '\t' || codepoint == '\n' || codepoint == '\r') {
		return 0;
	}
	else {
		std::string symbols = "~`!@\"#$;%^:&?*()_-+=[{]}\\|/\'<>,.";

		for (size_t i = 0; i < symbols.length(); i++) {
			if (codepoint == (unsigned char)symbols[i]) {
				return 1;
			}
		}

		return 2;
	}
}

void iInitChatScreen(void)
{
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

	// jump to the end of new history if last position at the current history end
	if (iChatHistory -> position + ICS_HISTORY_MAX_MESSAGES == (int)((iChatHistory -> data).size())) {
		iChatHistory -> position = std::max(0, message_dispatcher.ListSize - ICS_HISTORY_MAX_MESSAGES);
	}

	el = message_dispatcher.first();
	(iChatHistory -> data).clear();
	if (!iChatMUTE) {
		while (el) {
			iChatHistory -> add_str(el -> message, el -> color);
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

	(iChatHistory -> data).clear();
	iChatHistory -> position = 0;
}

void iChatHistoryScreen::add_str(char* str, int col)
{
	int color = (iScreenChat) ? aciChatColors0[col] : aciChatColors1[col];
	data.push_back(Message(str, color));
}
