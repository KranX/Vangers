/* ---------------------------- INCLUDE SECTION ----------------------------- */

#include "../global.h"
#include "../settings/text_encoding.h"
#include "../text/network_text.h"
#include "../text/utf8.h"

#include "../actint/actint.h"
#include "../actint/item_api.h"
#include "../actint/raster_text.h"
#include "../network.h"
#include "controls.h"
#include "hfont.h"
#include "i_chat.h"

#include "iscreen.h"

#include <algorithm>

#ifndef _WIN32
#	include <arpa/inet.h> // ntohl() FIXME: remove
#endif

/* ----------------------------- EXTERN SECTION ----------------------------- */

extern XBuffer *iResBuf;
extern int aciKeyboardLocked;

extern int iScreenOffs;

extern const char *iSTR_WORLD_NONE_CHAR;
extern const char *iSTR_WORLD_FOSTRAL_CHAR;
extern const char *iSTR_WORLD_GLORX_CHAR;
extern const char *iSTR_WORLD_NECROSS_CHAR;
extern const char *iSTR_WORLD_XPLO_CHAR;
extern const char *iSTR_WORLD_KHOX_CHAR;
extern const char *iSTR_WORLD_BOOZEENA_CHAR;
extern const char *iSTR_WORLD_WEEXOW_CHAR;
extern const char *iSTR_WORLD_HMOK_CHAR;
extern const char *iSTR_WORLD_THREALL_CHAR;
extern const char *iSTR_WORLD_ARK_A_ZNOY_CHAR;
extern const char *iSTR_COLOR_GREEN_CHAR;
extern const char *iSTR_COLOR_ORANGE_CHAR;
extern const char *iSTR_COLOR_BLUE_CHAR;
extern const char *iSTR_COLOR_YELLOW_CHAR;
extern const char *iSTR_COLOR_RED_CHAR;
extern const char *iSTR_COLOR_WHITE_CHAR;
extern const char *iSTR_COLOR_GRAY_CHAR;
extern const char *iSTR_COLOR_BLACK_CHAR;
extern const char *iSTR_COLOR_CAMOUFLAGE_CHAR;
extern const char *iSTR_COLOR_PATROL_CHAR;
extern const char *iSTR_MUTE_ALL;
extern const char *iSTR_CLEAR_LOG;
extern const char *iSTR_BACKGROUND;
extern const char *iSTR_LEAVE_CHAT;
extern const char *iSTR_All;
extern const char *iSTR_Yellow;
extern const char *iSTR_Orange;
extern const char *iSTR_Blue;
extern const char *iSTR_Green;
extern const char *iSTR_Red;
extern const char *iSTR_White;
extern const char *iSTR_Gray;
extern const char *iSTR_Black;
extern const char *iSTR_Camouflage;
extern const char *iSTR_Patrol;

/* --------------------------- PROTOTYPE SECTION ---------------------------- */

int iGetKeyName(int vkey, int shift, int lng_flag);

void iChatInit(void);
void iChatQuant(int flush = 0);
void iChatFinit(void);
void iChatKeyQuant(SDL_Event *k);
void iChatMouseQuant(int x, int y, int bt);

void iInitChatButtons(void);
void iInitChatScreen(void);

void iChatInputChar(SDL_Event *code);
void iChatInputChar(const char *input_char);
void iChatInputFlush(void);
void iChatInputBack(void);
void iChatInputEditing(SDL_Event *code);
void iChatInputDrawCursor(void);

void iChatMouseHandler(iChatButton *bt, int bt_id);
void iChatClearButtons(void);
void iChatClearLog(void);

iChatButton *iGetChatButton(int id);
iChatButton *iGetChatPlayerButton(int id);

void put_map(int x, int y, int sx, int sy);

int getCharGroup(char chr);
/* --------------------------- DEFINITION SECTION --------------------------- */

#define ICS_INPUT_TIMER 10
#define ICS_INPUT_MAX_LENGTH 200

// Button IDs...
#define ICS_UP_BUTTON 0x01
#define ICS_DN_BUTTON 0x02

#define ICS_ALL_BUTTON 0x03
#define ICS_GREEN_BUTTON 0x04
#define ICS_ORANGE_BUTTON 0x05
#define ICS_BLUE_BUTTON 0x06
#define ICS_YELLOW_BUTTON 0x07

#define ICS_MUTE_BUTTON 0x08
#define ICS_CLEAR_BUTTON 0x09

#define ICS_PLAYER1_BUTTON 0x0A
#define ICS_PLAYER2_BUTTON 0x0B
#define ICS_PLAYER3_BUTTON 0x0C
#define ICS_PLAYER4_BUTTON 0x0D
#define ICS_PLAYER5_BUTTON 0x0E

#define ICS_EXIT_BUTTON 0x0F

#define ICS_BACK_BUTTON 0x10

#define ICS_RED_BUTTON 0x11
#define ICS_WHITE_BUTTON 0x12
#define ICS_GRAY_BUTTON 0x13
#define ICS_BLACK_BUTTON 0x14
#define ICS_CAMOUFLAGE_BUTTON 0x15
#define ICS_PATROL_BUTTON 0x16

// Chat Screen Colors...
const int ICS_FON_COLOR = 184;
const int ICS_BORDER_COLOR = 184;

const int ICS_STRING_COL0 = 227 + (3 << 16);
const int ICS_STRING_COL1 = 88 + (1 << 16);

const int ICS_iFON_COLOR = 80;
const int ICS_iBORDER_COLOR = 80;

const int ICS_iSTRING_COL0 = 2 + (3 << 16);
const int ICS_iSTRING_COL1 = 144 + (1 << 16);

// Chat Screen Coordinates...
#define ICS_DELTA 10

#define ICS_HISTORY_SIZE_X 450
#define ICS_HISTORY_SIZE_Y 320

#define ICS_BUTTON_SIZE_X 170
#define ICS_BUTTON_SIZE_Y 20

XList *iChatButtons = NULL;
iChatHistoryScreen *iChatHistory = NULL;
iChatInputField *iChatInput = NULL;

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

static bool iChatReady(void) {
	return iChatON && iChatInput && iChatHistory && iChatButtons;
}

static int chatBoundary(std::string_view text, int pos) {
	pos = std::clamp(pos, 0, static_cast<int>(text.size()));
	while (pos > 0 && pos < static_cast<int>(text.size()) &&
		   (static_cast<unsigned char>(text[pos]) & 0xc0) == 0x80)
		--pos;
	return pos;
}

static int chatPrevious(std::string_view text, int pos) {
	return chatBoundary(text, pos - 1);
}

static int chatNext(std::string_view text, int pos) {
	return pos + static_cast<int>(vangers::text::prefix_bytes(text.substr(pos), 1));
}

static std::string chatInputLabel(std::string_view text) {
	std::string label = network_player_name();
	label += '>';
	label += text;
	return label;
}

static void iChatNormalizeInput(void) {
	if (!iChatInput)
		return;

	if ((iChatInput->string).empty())
		iChatInput->string = ">";
	else if ((iChatInput->string)[0] != '>')
		(iChatInput->string).insert((iChatInput->string).begin(), '>');

	iChatInput->cursorPosition =
		std::max(1, chatBoundary(iChatInput->string, iChatInput->cursorPosition));
	iChatInput->selectionPosition =
		std::max(1, chatBoundary(iChatInput->string, iChatInput->selectionPosition));
	iChatInput->leftDrawPosition =
		std::max(1, chatBoundary(iChatInput->string, iChatInput->leftDrawPosition));
	iChatInput->rightDrawPosition = std::max(
		iChatInput->leftDrawPosition,
		chatBoundary(iChatInput->string, iChatInput->rightDrawPosition)
	);

	if (iChatInput->cursorPosition < iChatInput->leftDrawPosition) {
		iChatInput->leftDrawPosition = iChatInput->cursorPosition;
		iChatInput->rightDrawPosition =
			iChatInput->getRightDrawPositionByLeft(iChatInput->leftDrawPosition);
	} else if (iChatInput->cursorPosition > iChatInput->rightDrawPosition) {
		iChatInput->rightDrawPosition = iChatInput->cursorPosition;
		iChatInput->leftDrawPosition =
			iChatInput->getLeftDrawPositionByRight(iChatInput->rightDrawPosition);
	}

	iChatInput->leftDrawPosition =
		std::max(1, chatBoundary(iChatInput->string, iChatInput->leftDrawPosition));
	iChatInput->rightDrawPosition = std::max(
		iChatInput->leftDrawPosition,
		chatBoundary(iChatInput->string, iChatInput->rightDrawPosition)
	);
}

static int aciChatColors0[10] = {(214 | (2 << 16)),
	(230 | (2 << 16)),
	(244 | (2 << 16)),
	(150 | (2 << 16)),
	(177 | (2 << 16)),
	(6 | (2 << 16)),
	(6 | (2 << 16)),
	(6 | (2 << 16)),
	(214 | (2 << 16)),
	(244 | (2 << 16))};

static int aciChatColors1[10] = {(244 | (2 << 16)),
	(134 | (2 << 16)),
	(150 | (2 << 16)),
	(176 | (2 << 16)),
	(196 | (2 << 16)),
	(230 | (2 << 16)),
	(230 | (2 << 16)),
	(230 | (2 << 16)),
	(244 | (2 << 16)),
	(150 | (2 << 16))};

int selectionColorI = (244 | (2 << 16)); // aciChatColors0[2] blue color for selection
int selectionColor = (150 | (2 << 16));	 // aciChatColors1[2]

int cursorColorI = (6 | (2 << 16));	 // aciChatColors0[5] white color for cursor
int cursorColor = (230 | (2 << 16)); // aciChatColors1[5]
int cursorWidth = 2;

int counterColorI = 2 + (3 << 16);	// ICS_iSTRING_COL0 gray color for counter
int counterColor = 227 + (3 << 16); // ICS_STRING_COL0

int scrollBgColorI = 2 + (3 << 16);	 // ICS_iSTRING_COL0 gray color for scroll background
int scrollBgColor = 227 + (3 << 16); // ICS_STRING_COL0
int scrollColorI = (6 | (2 << 16));	 // aciChatColors0[5] white color for scroll
int scrollColor = (230 | (2 << 16)); // aciChatColors1[5]
int scrollSizeX = 5;

static const char **iChatWorlds[] = {&iSTR_WORLD_NONE_CHAR,
	&iSTR_WORLD_FOSTRAL_CHAR,
	&iSTR_WORLD_GLORX_CHAR,
	&iSTR_WORLD_NECROSS_CHAR,
	&iSTR_WORLD_XPLO_CHAR,
	&iSTR_WORLD_KHOX_CHAR,
	&iSTR_WORLD_BOOZEENA_CHAR,
	&iSTR_WORLD_WEEXOW_CHAR,
	&iSTR_WORLD_HMOK_CHAR,
	&iSTR_WORLD_THREALL_CHAR,
	&iSTR_WORLD_ARK_A_ZNOY_CHAR};

static const char **iChatColors[] = {&iSTR_COLOR_GREEN_CHAR,
	&iSTR_COLOR_ORANGE_CHAR,
	&iSTR_COLOR_BLUE_CHAR,
	&iSTR_COLOR_YELLOW_CHAR,
	&iSTR_COLOR_RED_CHAR,
	&iSTR_COLOR_WHITE_CHAR,
	&iSTR_COLOR_GRAY_CHAR,
	&iSTR_COLOR_BLACK_CHAR,
	&iSTR_COLOR_CAMOUFLAGE_CHAR,
	&iSTR_COLOR_PATROL_CHAR};

iChatScreenObject::iChatScreenObject(void) {
	ID = 0;
	type = ICS_STRING;
	flags = 0;

	font = 0;

	PosX = PosY = 0;
	SizeX = SizeY = 0;
	fonColor = borderColor = 0;
}

void iChatScreenObject::redraw(void) {
	if (iChatBackLog)
		XGR_Rectangle(PosX, PosY, SizeX, SizeY, borderColor, fonColor, XGR_FILLED);
	else
		XGR_Rectangle(PosX, PosY, SizeX, SizeY, borderColor, fonColor, XGR_OUTLINED);
}

void iChatScreenObject::flush(void) {
	XGR_Flush(PosX, PosY, SizeX, SizeY);
}

iChatInputField::iChatInputField(void) {
	string = std::string();


	if (!iScreenChat)
		color = ICS_STRING_COL1;
	else
		color = ICS_iSTRING_COL1;

	cursorPosition = 0;
	selectionPosition = 0;
	leftDrawPosition = 0;
	rightDrawPosition = 0;
}

void iChatInputField::redraw(void) {
	iChatScreenObject::redraw();

	this->selectionRedraw();
	this->counterRedraw();

	const auto label = chatInputLabel(
		std::string_view(string).substr(leftDrawPosition, rightDrawPosition - leftDrawPosition)
	);
	aOutText32Utf8(PosX, PosY, color, label.c_str(), font, 1, 0);
}

void iChatInputField::selectionRedraw(void) {
	int color;
	if (iScreenChat) {
		color = selectionColorI;
	} else {
		color = selectionColor;
	}

	int leftSelectionPosition = std::min(
		std::max(std::min(cursorPosition, selectionPosition), leftDrawPosition), rightDrawPosition
	);
	int rightSelectionPosition = std::min(
		std::max(std::max(cursorPosition, selectionPosition), leftDrawPosition), rightDrawPosition
	);

	const std::string_view visible(string);
	const auto left =
		chatInputLabel(visible.substr(leftDrawPosition, leftSelectionPosition - leftDrawPosition));
	const auto right =
		chatInputLabel(visible.substr(leftDrawPosition, rightSelectionPosition - leftDrawPosition));
	int left_x = aTextWidth32Utf8(left.c_str(), font, 1);
	int right_x = aTextWidth32Utf8(right.c_str(), font, 1);

	XGR_Rectangle(PosX + left_x, PosY, right_x - left_x, SizeY, color, color, XGR_FILLED);
}

void iChatInputField::counterRedraw(void) {
	int color;
	if (iScreenChat) {
		color = counterColorI;
	} else {
		color = counterColor;
	}

	const auto max_label = std::to_string(ICS_INPUT_MAX_LENGTH);
	const int x = aTextWidth32Utf8(max_label.c_str(), font, 1);
	const auto label =
		std::to_string(ICS_INPUT_MAX_LENGTH - (vangers::text::text_length(string) - 1));
	XGR_Rectangle(PosX - x - 10, PosY, x + 10, SizeY, fonColor, fonColor, XGR_FILLED);
	aOutText32Utf8(PosX - x - 10, PosY, color, label.c_str(), font, 1, 0);
}

int iChatInputField::getLeftDrawPositionByRight(int rightPosition) {
	rightPosition = std::max(1, chatBoundary(string, rightPosition));
	int left = rightPosition;
	while (left > 1) {
		const int candidate = chatPrevious(string, left);
		const auto label =
			chatInputLabel(std::string_view(string).substr(candidate, rightPosition - candidate));
		if (aTextWidth32Utf8(label.c_str(), font, 1) > SizeX - 10)
			break;
		left = candidate;
	}
	return left;
}

int iChatInputField::getRightDrawPositionByLeft(int leftPosition) {
	leftPosition = std::max(1, chatBoundary(string, leftPosition));
	int right = leftPosition;
	while (right < static_cast<int>(string.size())) {
		const int candidate = chatNext(string, right);
		const auto label =
			chatInputLabel(std::string_view(string).substr(leftPosition, candidate - leftPosition));
		if (aTextWidth32Utf8(label.c_str(), font, 1) > SizeX - 10)
			break;
		right = candidate;
	}
	return right;
}

iChatHistoryScreen::iChatHistoryScreen(void) {
	position = 0;
}

void iChatHistoryScreen::redraw(void) {
	iChatScreenObject::redraw();

	this->redrawScroll();

	int y = 0;

	for (int i = position; i < std::min((int)(data.size()), position + ICS_HISTORY_MAX_MESSAGES);
		i++) {
		if (!data[i].text.empty())
			aOutText32Utf8(PosX, PosY + y, data[i].color, data[i].text.c_str(), font, 1, 0);
		y += aTextHeight32Utf8(data[i].text.c_str(), font, 0) + 2;
	}
}

void iChatHistoryScreen::redrawScroll(void) {
	int bgColor, color;
	if (iScreenChat) {
		bgColor = scrollBgColorI;
		color = scrollColorI;
	} else {
		bgColor = scrollBgColor;
		color = scrollColor;
	}

	int dataSize = (int)(data.size());
	int messages_num = std::max(ICS_HISTORY_MAX_MESSAGES, dataSize);
	int maxPosition = std::max(0, dataSize - ICS_HISTORY_MAX_MESSAGES);
	int safePosition = std::max(0, std::min(position, maxPosition));
	float percent = (float)safePosition / messages_num;
	float percentSizeY = (float)ICS_HISTORY_MAX_MESSAGES / messages_num;

	XGR_Rectangle(PosX + SizeX, PosY, scrollSizeX, SizeY, bgColor, bgColor, XGR_FILLED);
	XGR_Rectangle(
		PosX + SizeX,
		PosY + round(percent * SizeY),
		scrollSizeX,
		round(percentSizeY * SizeY),
		color,
		color,
		XGR_FILLED
	);
}

iChatButton::iChatButton(int num_state) {
	CurState = 0;
	NumState = num_state;
	StateColors = new int[NumState];

	extData = 0;

}

iChatButton::~iChatButton(void) {
	delete[] StateColors;
}

void iChatButton::redraw(void) {
	int dx, dy;
	iChatScreenObject::redraw();
	if (ID == ICS_UP_BUTTON || ID == ICS_DN_BUTTON) {
		dx = (SizeX - aTextWidth32(string.data(), font, 1)) / 2;
		dy = (SizeY - aTextHeight32(string.data(), font, 0)) / 2;
		aOutText32(PosX + dx, PosY + dy, StateColors[CurState], string.data(), font, 1, 0);
		return;
	}
	dx = (SizeX - aTextWidth32Utf8(string.c_str(), font, 1)) / 2;
	dy = (SizeY - aTextHeight32Utf8(string.c_str(), font, 0)) / 2;

	aOutText32Utf8(PosX + dx, PosY + dy, StateColors[CurState], string.c_str(), font, 1, 0);
}

void iChatScreenObject::init(int x, int y, int sx, int sy, int col1, int col2) {
	PosX = x;
	PosY = y;

	SizeX = sx;
	SizeY = sy;

	fonColor = col1;
	borderColor = col2;
}

void iChatInit(void) {
	int x, y, sx, sy, bsx, col0, col1, bcol;
	iChatButton *p;

	sx = ICS_HISTORY_SIZE_X + ICS_DELTA + ICS_BUTTON_SIZE_X;
	sy = ICS_HISTORY_SIZE_Y + ICS_DELTA * 2 + ICS_BUTTON_SIZE_Y * 2;

	//	  bsx = (sx - ICS_DELTA * 2) / 3;
	bsx = (ICS_HISTORY_SIZE_X - ICS_DELTA * 2) / 3;

	x = (XGR_MAXX - sx) / 2;
	y = (XGR_MAXY - sy) / 2;

	if (!iScreenChat)
		bcol = ICS_BORDER_COLOR;
	else {
		y -= 30;
		x -= (XGR_MAXX - 800) / 2;
		y -= (XGR_MAXY - 600) / 2;
		bcol = ICS_iBORDER_COLOR;
	}

	iChatButtons = new XList;

	iChatHistory = new iChatHistoryScreen;
	iChatHistory->init(
		x,
		y + (ICS_BUTTON_SIZE_Y + ICS_DELTA) * 1,
		ICS_HISTORY_SIZE_X,
		ICS_HISTORY_SIZE_Y,
		bcol,
		bcol
	);
	iChatHistory->position = std::max(0, message_dispatcher.ListSize - ICS_HISTORY_MAX_MESSAGES);

	iChatInput = new iChatInputField;
	iChatInput->init(
		x,
		y + ICS_HISTORY_SIZE_Y + ICS_DELTA + (ICS_BUTTON_SIZE_Y + ICS_DELTA) * 1,
		ICS_HISTORY_SIZE_X,
		ICS_BUTTON_SIZE_Y,
		bcol,
		bcol
	);
	iChatInput->string = ">";
	iChatInput->cursorPosition = 1;
	iChatInput->selectionPosition = 1;
	iChatInput->leftDrawPosition = 1;
	iChatInput->rightDrawPosition = 1;
	iChatInputPrev = *iChatInput;

	p = new iChatButton(2);
	p->init(x, y, bsx, ICS_BUTTON_SIZE_Y, bcol, bcol);
	p->ID = ICS_MUTE_BUTTON;
	p->set_string(iSTR_MUTE_ALL);
	if (!iScreenChat) {
		col0 = ICS_STRING_COL0;
		col1 = ICS_STRING_COL1;
	} else {
		col0 = ICS_iSTRING_COL0;
		col1 = ICS_iSTRING_COL1;
	}
	p->set_color(0, col0);
	p->set_color(1, col1);
	iChatButtons->AddElement((XListElement *)p);

	p->CurState = iChatMUTE;

	p = new iChatButton(1);
	p->init(x + bsx + ICS_DELTA, y, bsx, ICS_BUTTON_SIZE_Y, bcol, bcol);
	p->ID = ICS_CLEAR_BUTTON;
	p->set_string(iSTR_CLEAR_LOG);
	if (!iScreenChat) {
		col0 = ICS_STRING_COL0;
		col1 = ICS_STRING_COL1;
	} else {
		col0 = ICS_iSTRING_COL0;
		col1 = ICS_iSTRING_COL1;
	}
	p->set_color(0, col0);
	iChatButtons->AddElement((XListElement *)p);

	iChatBackLog = 1;
	if (!iScreenChat) {
		p = new iChatButton(2);
		p->init(x + (bsx + ICS_DELTA) * 3, y, ICS_BUTTON_SIZE_X, ICS_BUTTON_SIZE_Y, bcol, bcol);
		p->ID = ICS_BACK_BUTTON;
		p->set_string(iSTR_BACKGROUND);
		if (!iScreenChat) {
			col0 = ICS_STRING_COL0;
			col1 = ICS_STRING_COL1;
		} else {
			col0 = ICS_iSTRING_COL0;
			col1 = ICS_iSTRING_COL1;
		}
		p->set_color(0, col0);
		p->set_color(1, col1);
		p->CurState = 1;
		iChatButtons->AddElement((XListElement *)p);
	}
	//	  else {
	//		  iChatBackLog = 0;
	//	  }

	if (!iScreenChat) {
		p = new iChatButton(1);
		p->init(x + bsx * 2 + ICS_DELTA * 2, y, bsx, ICS_BUTTON_SIZE_Y, bcol, bcol);
		p->ID = ICS_EXIT_BUTTON;
		p->set_string(iSTR_LEAVE_CHAT);
		col0 = ICS_STRING_COL0;
		p->set_color(0, col0);
		iChatButtons->AddElement((XListElement *)p);
	}

	p = new iChatButton(2);
	p->init(
		x + ICS_HISTORY_SIZE_X + ICS_DELTA,
		y + (ICS_BUTTON_SIZE_Y + ICS_DELTA) * 1,
		ICS_BUTTON_SIZE_X,
		ICS_BUTTON_SIZE_Y,
		bcol,
		bcol
	);
	p->ID = ICS_ALL_BUTTON;
	p->set_string(iSTR_All);
	if (!iScreenChat) {
		col0 = ICS_STRING_COL0;
		col1 = ICS_STRING_COL1;
	} else {
		col0 = ICS_iSTRING_COL0;
		col1 = ICS_iSTRING_COL1;
	}
	p->set_color(0, col0);
	p->set_color(1, col1);
	iChatButtons->AddElement((XListElement *)p);

	p = new iChatButton(2);
	p->init(
		x + ICS_HISTORY_SIZE_X + ICS_DELTA,
		y + ICS_DELTA + (ICS_BUTTON_SIZE_Y + ICS_DELTA / 5) * 2,
		ICS_BUTTON_SIZE_X / 2 - 1,
		ICS_BUTTON_SIZE_Y,
		bcol,
		bcol
	);
	p->ID = ICS_YELLOW_BUTTON;
	p->set_string(iSTR_Yellow);
	if (!iScreenChat) {
		col0 = ICS_STRING_COL0;
		col1 = ICS_STRING_COL1;
	} else {
		col0 = ICS_iSTRING_COL0;
		col1 = ICS_iSTRING_COL1;
	}
	p->set_color(0, col0);
	p->set_color(1, col1);
	iChatButtons->AddElement((XListElement *)p);

	p = new iChatButton(2);
	p->init(
		x + ICS_HISTORY_SIZE_X + ICS_DELTA,
		y + ICS_DELTA + (ICS_BUTTON_SIZE_Y + ICS_DELTA / 5) * 3,
		ICS_BUTTON_SIZE_X / 2 - 1,
		ICS_BUTTON_SIZE_Y,
		bcol,
		bcol
	);
	p->ID = ICS_ORANGE_BUTTON;
	p->set_string(iSTR_Orange);
	if (!iScreenChat) {
		col0 = ICS_STRING_COL0;
		col1 = ICS_STRING_COL1;
	} else {
		col0 = ICS_iSTRING_COL0;
		col1 = ICS_iSTRING_COL1;
	}
	p->set_color(0, col0);
	p->set_color(1, col1);
	iChatButtons->AddElement((XListElement *)p);

	p = new iChatButton(2);
	p->init(
		x + ICS_HISTORY_SIZE_X + ICS_DELTA,
		y + ICS_DELTA + (ICS_BUTTON_SIZE_Y + ICS_DELTA / 5) * 4,
		ICS_BUTTON_SIZE_X / 2 - 1,
		ICS_BUTTON_SIZE_Y,
		bcol,
		bcol
	);
	p->ID = ICS_BLUE_BUTTON;
	p->set_string(iSTR_Blue);
	if (!iScreenChat) {
		col0 = ICS_STRING_COL0;
		col1 = ICS_STRING_COL1;
	} else {
		col0 = ICS_iSTRING_COL0;
		col1 = ICS_iSTRING_COL1;
	}
	p->set_color(0, col0);
	p->set_color(1, col1);
	iChatButtons->AddElement((XListElement *)p);

	p = new iChatButton(2);
	p->init(
		x + ICS_HISTORY_SIZE_X + ICS_DELTA,
		y + ICS_DELTA + (ICS_BUTTON_SIZE_Y + ICS_DELTA / 5) * 5,
		ICS_BUTTON_SIZE_X / 2 - 1,
		ICS_BUTTON_SIZE_Y,
		bcol,
		bcol
	);
	p->ID = ICS_GREEN_BUTTON;
	p->set_string(iSTR_Green);
	if (!iScreenChat) {
		col0 = ICS_STRING_COL0;
		col1 = ICS_STRING_COL1;
	} else {
		col0 = ICS_iSTRING_COL0;
		col1 = ICS_iSTRING_COL1;
	}
	p->set_color(0, col0);
	p->set_color(1, col1);
	iChatButtons->AddElement((XListElement *)p);

	p = new iChatButton(2);
	p->init(
		x + ICS_HISTORY_SIZE_X + ICS_DELTA,
		y + ICS_DELTA + (ICS_BUTTON_SIZE_Y + ICS_DELTA / 5) * 6,
		ICS_BUTTON_SIZE_X / 2 - 1,
		ICS_BUTTON_SIZE_Y,
		bcol,
		bcol
	);
	p->ID = ICS_RED_BUTTON;
	p->set_string(iSTR_Red);
	if (!iScreenChat) {
		col0 = ICS_STRING_COL0;
		col1 = ICS_STRING_COL1;
	} else {
		col0 = ICS_iSTRING_COL0;
		col1 = ICS_iSTRING_COL1;
	}
	p->set_color(0, col0);
	p->set_color(1, col1);
	iChatButtons->AddElement((XListElement *)p);

	p = new iChatButton(2);
	p->init(
		x + ICS_HISTORY_SIZE_X + ICS_DELTA + ICS_BUTTON_SIZE_X / 2 + 1,
		y + ICS_DELTA + (ICS_BUTTON_SIZE_Y + ICS_DELTA / 5) * 2,
		ICS_BUTTON_SIZE_X / 2 - 1,
		ICS_BUTTON_SIZE_Y,
		bcol,
		bcol
	);
	p->ID = ICS_WHITE_BUTTON;
	p->set_string(iSTR_White);
	if (!iScreenChat) {
		col0 = ICS_STRING_COL0;
		col1 = ICS_STRING_COL1;
	} else {
		col0 = ICS_iSTRING_COL0;
		col1 = ICS_iSTRING_COL1;
	}
	p->set_color(0, col0);
	p->set_color(1, col1);
	iChatButtons->AddElement((XListElement *)p);

	p = new iChatButton(2);
	p->init(
		x + ICS_HISTORY_SIZE_X + ICS_DELTA + ICS_BUTTON_SIZE_X / 2 + 1,
		y + ICS_DELTA + (ICS_BUTTON_SIZE_Y + ICS_DELTA / 5) * 3,
		ICS_BUTTON_SIZE_X / 2 - 1,
		ICS_BUTTON_SIZE_Y,
		bcol,
		bcol
	);
	p->ID = ICS_GRAY_BUTTON;
	p->set_string(iSTR_Gray);
	if (!iScreenChat) {
		col0 = ICS_STRING_COL0;
		col1 = ICS_STRING_COL1;
	} else {
		col0 = ICS_iSTRING_COL0;
		col1 = ICS_iSTRING_COL1;
	}
	p->set_color(0, col0);
	p->set_color(1, col1);
	iChatButtons->AddElement((XListElement *)p);

	p = new iChatButton(2);
	p->init(
		x + ICS_HISTORY_SIZE_X + ICS_DELTA + ICS_BUTTON_SIZE_X / 2 + 1,
		y + ICS_DELTA + (ICS_BUTTON_SIZE_Y + ICS_DELTA / 5) * 4,
		ICS_BUTTON_SIZE_X / 2 - 1,
		ICS_BUTTON_SIZE_Y,
		bcol,
		bcol
	);
	p->ID = ICS_BLACK_BUTTON;
	p->set_string(iSTR_Black);
	if (!iScreenChat) {
		col0 = ICS_STRING_COL0;
		col1 = ICS_STRING_COL1;
	} else {
		col0 = ICS_iSTRING_COL0;
		col1 = ICS_iSTRING_COL1;
	}
	p->set_color(0, col0);
	p->set_color(1, col1);
	iChatButtons->AddElement((XListElement *)p);

	p = new iChatButton(2);
	p->init(
		x + ICS_HISTORY_SIZE_X + ICS_DELTA + ICS_BUTTON_SIZE_X / 2 + 1,
		y + ICS_DELTA + (ICS_BUTTON_SIZE_Y + ICS_DELTA / 5) * 5,
		ICS_BUTTON_SIZE_X / 2 - 1,
		ICS_BUTTON_SIZE_Y,
		bcol,
		bcol
	);
	p->ID = ICS_CAMOUFLAGE_BUTTON;
	p->set_string(iSTR_Camouflage);
	if (!iScreenChat) {
		col0 = ICS_STRING_COL0;
		col1 = ICS_STRING_COL1;
	} else {
		col0 = ICS_iSTRING_COL0;
		col1 = ICS_iSTRING_COL1;
	}
	p->set_color(0, col0);
	p->set_color(1, col1);
	iChatButtons->AddElement((XListElement *)p);

	p = new iChatButton(2);
	p->init(
		x + ICS_HISTORY_SIZE_X + ICS_DELTA + ICS_BUTTON_SIZE_X / 2 + 1,
		y + ICS_DELTA + (ICS_BUTTON_SIZE_Y + ICS_DELTA / 5) * 6,
		ICS_BUTTON_SIZE_X / 2 - 1,
		ICS_BUTTON_SIZE_Y,
		bcol,
		bcol
	);
	p->ID = ICS_PATROL_BUTTON;
	p->set_string(iSTR_Patrol);
	if (!iScreenChat) {
		col0 = ICS_STRING_COL0;
		col1 = ICS_STRING_COL1;
	} else {
		col0 = ICS_iSTRING_COL0;
		col1 = ICS_iSTRING_COL1;
	}
	p->set_color(0, col0);
	p->set_color(1, col1);
	iChatButtons->AddElement((XListElement *)p);

	p = new iChatButton(2);
	p->init(
		x + ICS_HISTORY_SIZE_X + ICS_DELTA + ICS_BUTTON_SIZE_X / 4,
		y + (ICS_BUTTON_SIZE_Y + ICS_DELTA) * 6,
		ICS_BUTTON_SIZE_X / 2,
		ICS_BUTTON_SIZE_Y,
		bcol,
		bcol
	);
	p->ID = ICS_UP_BUTTON;
	p->set_string("\x1E");
	if (!iScreenChat) {
		col0 = ICS_STRING_COL0;
		col1 = ICS_STRING_COL1;
	} else {
		col0 = ICS_iSTRING_COL0;
		col1 = ICS_iSTRING_COL1;
	}
	p->set_color(0, col0);
	p->set_color(1, col1);
	iChatButtons->AddElement((XListElement *)p);

	p = new iChatButton(2);
	p->init(
		x + ICS_HISTORY_SIZE_X + ICS_DELTA,
		y + (ICS_BUTTON_SIZE_Y + ICS_DELTA) * 7,
		ICS_BUTTON_SIZE_X,
		ICS_BUTTON_SIZE_Y,
		bcol,
		bcol
	);
	p->ID = ICS_PLAYER1_BUTTON;
	p->set_string("123456789012345 (F/B)");
	if (!iScreenChat) {
		col0 = ICS_STRING_COL0;
		col1 = ICS_STRING_COL1;
	} else {
		col0 = ICS_iSTRING_COL0;
		col1 = ICS_iSTRING_COL1;
	}
	p->set_color(0, col0);
	p->set_color(1, col1);
	iChatButtons->AddElement((XListElement *)p);

	p = new iChatButton(2);
	p->init(
		x + ICS_HISTORY_SIZE_X + ICS_DELTA,
		y + (ICS_BUTTON_SIZE_Y + ICS_DELTA) * 8,
		ICS_BUTTON_SIZE_X,
		ICS_BUTTON_SIZE_Y,
		bcol,
		bcol
	);
	p->ID = ICS_PLAYER2_BUTTON;
	p->set_string("123456789012345 (F/B)");
	if (!iScreenChat) {
		col0 = ICS_STRING_COL0;
		col1 = ICS_STRING_COL1;
	} else {
		col0 = ICS_iSTRING_COL0;
		col1 = ICS_iSTRING_COL1;
	}
	p->set_color(0, col0);
	p->set_color(1, col1);
	iChatButtons->AddElement((XListElement *)p);

	p = new iChatButton(2);
	p->init(
		x + ICS_HISTORY_SIZE_X + ICS_DELTA,
		y + (ICS_BUTTON_SIZE_Y + ICS_DELTA) * 9,
		ICS_BUTTON_SIZE_X,
		ICS_BUTTON_SIZE_Y,
		bcol,
		bcol
	);
	p->ID = ICS_PLAYER3_BUTTON;
	p->set_string("123456789012345 (F/B)");
	if (!iScreenChat) {
		col0 = ICS_STRING_COL0;
		col1 = ICS_STRING_COL1;
	} else {
		col0 = ICS_iSTRING_COL0;
		col1 = ICS_iSTRING_COL1;
	}
	p->set_color(0, col0);
	p->set_color(1, col1);
	iChatButtons->AddElement((XListElement *)p);

	p = new iChatButton(2);
	p->init(
		x + ICS_HISTORY_SIZE_X + ICS_DELTA,
		y + (ICS_BUTTON_SIZE_Y + ICS_DELTA) * 10,
		ICS_BUTTON_SIZE_X,
		ICS_BUTTON_SIZE_Y,
		bcol,
		bcol
	);
	p->ID = ICS_PLAYER4_BUTTON;
	p->set_string("123456789012345 (F/B)");
	if (!iScreenChat) {
		col0 = ICS_STRING_COL0;
		col1 = ICS_STRING_COL1;
	} else {
		col0 = ICS_iSTRING_COL0;
		col1 = ICS_iSTRING_COL1;
	}
	p->set_color(0, col0);
	p->set_color(1, col1);
	iChatButtons->AddElement((XListElement *)p);

	p = new iChatButton(2);
	p->init(
		x + ICS_HISTORY_SIZE_X + ICS_DELTA,
		y + (ICS_BUTTON_SIZE_Y + ICS_DELTA) * 11,
		ICS_BUTTON_SIZE_X,
		ICS_BUTTON_SIZE_Y,
		bcol,
		bcol
	);
	p->ID = ICS_PLAYER5_BUTTON;
	p->set_string("123456789012345 (F/B)");
	if (!iScreenChat) {
		col0 = ICS_STRING_COL0;
		col1 = ICS_STRING_COL1;
	} else {
		col0 = ICS_iSTRING_COL0;
		col1 = ICS_iSTRING_COL1;
	}
	p->set_color(0, col0);
	p->set_color(1, col1);
	iChatButtons->AddElement((XListElement *)p);

	p = new iChatButton(2);
	p->init(
		x + ICS_HISTORY_SIZE_X + ICS_DELTA + ICS_BUTTON_SIZE_X / 4,
		y + (ICS_BUTTON_SIZE_Y + ICS_DELTA) * 12,
		ICS_BUTTON_SIZE_X / 2,
		ICS_BUTTON_SIZE_Y,
		bcol,
		bcol
	);
	p->ID = ICS_DN_BUTTON;
	p->set_string("\x1F");
	if (!iScreenChat) {
		col0 = ICS_STRING_COL0;
		col1 = ICS_STRING_COL1;
	} else {
		col0 = ICS_iSTRING_COL0;
		col1 = ICS_iSTRING_COL1;
	}
	p->set_color(0, col0);
	p->set_color(1, col1);
	iChatButtons->AddElement((XListElement *)p);

	iChatExit = 0;
	iChatON = 1;
	aciKeyboardLocked = 1;

	XGR_MouseShow();

	iInitChatScreen();
	if (!SDL_StartTextInput(XGR_Obj.get_window()))
		ErrH.Abort(SDL_GetError(), XERR_USER, 0);
}

void iChatQuant(int flush) {
	iChatButton *p;
	if (!iChatON)
		return;
	if (!iChatReady())
		return;

	//	  if(iScreenChat) put_map(iScreenOffs,0,XGR_MAXX,XGR_MAXY);

	iInitChatScreen();
	p = (iChatButton *)iChatButtons->fPtr;
	while (p) {
		if (!(p->flags & ICS_HIDDEN_OBJECT))
			p->redraw();
		p = (iChatButton *)p->next;
	}

	iChatInput->redraw();
	iChatHistory->redraw();

	iChatCursorTimer++;
	if (iChatCursorTimer >= ICS_INPUT_TIMER) {
		iChatCursorTimer = 0;
		iChatCursorFlag = !iChatCursorFlag;
	}
	if (iChatCursorFlag) {
		iChatInputDrawCursor();
	}

	if (flush) {
		p = (iChatButton *)iChatButtons->fPtr;
		while (p) {
			if (!(p->flags & ICS_HIDDEN_OBJECT))
				p->flush();
			p = (iChatButton *)p->next;
		}
		iChatInput->flush();
		iChatHistory->flush();
	}
}

void iChatFinit(void) {
	iChatButton *p, *p1;

	if (!iChatON && !iChatButtons && !iChatHistory && !iChatInput)
		return;

	if (iChatButtons) {
		p = (iChatButton *)iChatButtons->fPtr;
		while (p) {
			p1 = (iChatButton *)p->next;
			delete p;
			p = p1;
		}
		delete iChatButtons;
	}

	if (iChatHistory)
		delete iChatHistory;

	if (iChatInput)
		delete iChatInput;

	iChatON = iScreenChat = 0;
	iChatExit = 0;
	aciKeyboardLocked = 0;

	iChatHistory = NULL;
	iChatInput = NULL;
	iChatButtons = NULL;
	SDL_StopTextInput(XGR_Obj.get_window());
}

void iChatKeyQuant(SDL_Event *k) {
	if (!k || !iChatReady()) {
		return;
	}

	iChatInputChar(k);
	iChatInputEditing(k);

	if (k->type != SDL_EVENT_KEY_DOWN) {
		return;
	}

	if (iCheckKeyID(iKEY_CHAT, k->key.scancode)) {
		iChatExit = 1;
		return;
	}

	switch (k->key.scancode) {
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

void iChatMouseQuant(int x, int y, int bt) {
	if (!iChatReady())
		return;

	iChatButton *p = (iChatButton *)iChatButtons->fPtr;
	while (p) {
		if (p->check_xy(x, y))
			iChatMouseHandler(p, bt);
		p = (iChatButton *)p->next;
	}
}

void iChatMouseHandler(iChatButton *bt, int bt_id) {
	switch (bt->ID) {
	case ICS_EXIT_BUTTON:
		iChatExit = 1;
		break;
	case ICS_UP_BUTTON:
		if (iChatSkip)
			iChatSkip--;
		break;
	case ICS_DN_BUTTON:
		if (iChatSkip < players_list.size() - 5)
			iChatSkip++;
		break;

	case ICS_ALL_BUTTON:
		iChatClearButtons();
		bt->CurState = 1;
		iChatFilter = MESSAGE_FOR_ALL;
		iChatFilterID = 0;
		break;
	case ICS_YELLOW_BUTTON:
		iChatClearButtons();
		bt->CurState = 1;
		iChatFilter = MESSAGE_FOR_TEAM;
		iChatFilterID = TEAM_YELLOW;
		break;
	case ICS_ORANGE_BUTTON:
		iChatClearButtons();
		bt->CurState = 1;
		iChatFilter = MESSAGE_FOR_TEAM;
		iChatFilterID = TEAM_ORANGE;
		break;
	case ICS_BLUE_BUTTON:
		iChatClearButtons();
		bt->CurState = 1;
		iChatFilter = MESSAGE_FOR_TEAM;
		iChatFilterID = TEAM_BLUE;
		break;
	case ICS_GREEN_BUTTON:
		iChatClearButtons();
		bt->CurState = 1;
		iChatFilter = MESSAGE_FOR_TEAM;
		iChatFilterID = TEAM_GREEN;
		break;
	case ICS_RED_BUTTON:
		iChatClearButtons();
		bt->CurState = 1;
		iChatFilter = MESSAGE_FOR_TEAM;
		iChatFilterID = TEAM_RED;
		break;
	case ICS_WHITE_BUTTON:
		iChatClearButtons();
		bt->CurState = 1;
		iChatFilter = MESSAGE_FOR_TEAM;
		iChatFilterID = TEAM_WHITE;
		break;
	case ICS_GRAY_BUTTON:
		iChatClearButtons();
		bt->CurState = 1;
		iChatFilter = MESSAGE_FOR_TEAM;
		iChatFilterID = TEAM_GRAY;
		break;
	case ICS_BLACK_BUTTON:
		iChatClearButtons();
		bt->CurState = 1;
		iChatFilter = MESSAGE_FOR_TEAM;
		iChatFilterID = TEAM_BLACK;
		break;
	case ICS_CAMOUFLAGE_BUTTON:
		iChatClearButtons();
		bt->CurState = 1;
		iChatFilter = MESSAGE_FOR_TEAM;
		iChatFilterID = TEAM_CAMOUFLAGE;
		break;
	case ICS_PATROL_BUTTON:
		iChatClearButtons();
		bt->CurState = 1;
		iChatFilter = MESSAGE_FOR_TEAM;
		iChatFilterID = TEAM_PATROL;
		break;

	case ICS_MUTE_BUTTON:
		bt->CurState ^= 1;
		iChatMUTE = bt->CurState;
		break;
	case ICS_BACK_BUTTON:
		bt->CurState ^= 1;
		iChatBackLog = bt->CurState;
		break;
	case ICS_CLEAR_BUTTON:
		iChatClearLog();
		break;
	case ICS_PLAYER1_BUTTON:
	case ICS_PLAYER2_BUTTON:
	case ICS_PLAYER3_BUTTON:
	case ICS_PLAYER4_BUTTON:
	case ICS_PLAYER5_BUTTON:
		if (!(bt->flags & ICS_HIDDEN_OBJECT)) {
			iChatClearButtons();
			bt->CurState = 1;
			iChatFilter = MESSAGE_FOR_PLAYER;
			iChatFilterID = bt->extData;
		}
		break;
	}
}

void iChatClearButtons(void) {
	iChatButton *p = (iChatButton *)iChatButtons->fPtr;
	while (p) {
		switch (p->ID) {
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
			p->CurState = 0;
			break;
		}
		p = (iChatButton *)p->next;
	}
}

iChatButton *iGetChatPlayerButton(int id) {
	iChatButton *p = (iChatButton *)iChatButtons->fPtr;
	while (p) {
		switch (p->ID) {
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
			if (p->extData == id)
				return p;
			break;
		}
		p = (iChatButton *)p->next;
	}
	return 0;
}

void iChatInputDrawCursor(void) {
	if (!iChatReady())
		return;
	iChatNormalizeInput();

	int color;
	if (iScreenChat) {
		color = cursorColorI;
	} else {
		color = cursorColor;
	}

	int cursorPosition = std::min(
		iChatInput->rightDrawPosition,
		std::max(iChatInput->leftDrawPosition, iChatInput->cursorPosition)
	);

	const auto label = chatInputLabel(
		std::string_view(iChatInput->string)
			.substr(iChatInput->leftDrawPosition, cursorPosition - iChatInput->leftDrawPosition)
	);
	const int x = aTextWidth32Utf8(label.c_str(), iChatInput->font, 1);

	XGR_Rectangle(
		iChatInput->PosX + x - cursorWidth / 2,
		iChatInput->PosY,
		cursorWidth,
		iChatInput->SizeY,
		color,
		color,
		XGR_FILLED
	);
}

void iChatInputChar(const char *input_char) {
	if (!iChatReady() || !input_char || !input_char[0])
		return;
	iChatNormalizeInput();

	std::string_view incoming(input_char);
	if (!vangers::settings::is_valid_utf8(incoming))
		return;

	const int left = std::min(iChatInput->cursorPosition, iChatInput->selectionPosition);
	const int right = std::max(iChatInput->cursorPosition, iChatInput->selectionPosition);
	const int remaining = ICS_INPUT_MAX_LENGTH -
						  static_cast<int>(
							  vangers::text::text_length(iChatInput->string) - 1 -
							  vangers::text::text_length(
								  std::string_view(iChatInput->string).substr(left, right - left)
							  )
						  );
	if (remaining <= 0)
		return;
	const auto inserted = vangers::network_text::single_line(
		incoming.substr(0, vangers::text::prefix_bytes(incoming, remaining))
	);

	iChatCursorFlag = true;
	iChatCursorTimer = 0;
	iChatInputPrev = *iChatInput;
	iChatInput->string.replace(left, right - left, inserted);
	iChatInput->cursorPosition = iChatInput->selectionPosition =
		left + static_cast<int>(inserted.size());
	iChatInput->rightDrawPosition = iChatInput->cursorPosition;
	iChatInput->leftDrawPosition =
		iChatInput->getLeftDrawPositionByRight(iChatInput->rightDrawPosition);
	iChatNormalizeInput();
}

void iChatInputChar(SDL_Event *event) {
	if (event && iChatReady() && event->type == SDL_EVENT_TEXT_INPUT) {
		iChatInputChar(event->text.text);
	}
}

void iChatInputFlush(void) {
	if (!iChatReady())
		return;
	iChatNormalizeInput();

	iChatCursorFlag = true;
	iChatCursorTimer = 0;

	const std::string_view message = std::string_view(iChatInput->string).substr(1);
	if (!message.empty()) {
		if (!message_dispatcher.can_send_text(message)) {
			message_dispatcher.show_legacy_text_warning();
			return; // The user can edit or copy the unsent draft.
		}
		const std::string prefix = std::string(network_player_name()) + ": ";
		const int width = iChatHistory->SizeX - 10;
		std::size_t left = 0;
		std::size_t right = 0;
		while (right < message.size()) {
			const auto next = right + vangers::text::prefix_bytes(message.substr(right), 1);
			const auto candidate = message.substr(left, next - left);
			const auto label = prefix + std::string(candidate);
			if (right > left && (aTextWidth32Utf8(label.c_str(), iChatInput->font, 1) > width ||
									!message_dispatcher.fits_wire(candidate))) {
				message_dispatcher.send(
					message.substr(left, right - left), iChatFilter, iChatFilterID
				);
				left = right;
			}
			right = next;
		}
		message_dispatcher.send(message.substr(left), iChatFilter, iChatFilterID);
	}

	iChatInput->string = ">";
	iChatInput->cursorPosition = 1;
	iChatInput->selectionPosition = 1;
	iChatInput->rightDrawPosition = 1;
	iChatInput->leftDrawPosition = 1;

	iChatInputPrev = *iChatInput;

	// Next refresh follows the bottom of the existing history viewport.
	iChatHistory->position =
		std::max(0, static_cast<int>(iChatHistory->data.size()) - ICS_HISTORY_MAX_MESSAGES);
}

void iChatInputBack(void) {
	if (!iChatReady())
		return;
	iChatNormalizeInput();

	iChatCursorFlag = true;
	iChatCursorTimer = 0;

	int leftPosition = std::min(iChatInput->cursorPosition, iChatInput->selectionPosition);
	int rightPosition = std::max(iChatInput->cursorPosition, iChatInput->selectionPosition);

	if (leftPosition == rightPosition) {
		if (iChatInput->cursorPosition <= 1)
			return;
		leftPosition = chatPrevious(iChatInput->string, iChatInput->cursorPosition);
		rightPosition = iChatInput->cursorPosition;
	}

	leftPosition = std::max(leftPosition, 1);
	rightPosition =
		std::min(std::max(rightPosition, leftPosition), (int)((iChatInput->string).length()));
	if (leftPosition >= rightPosition)
		return;

	iChatInputPrev = *iChatInput;

	std::string new_string =
		(iChatInput->string).substr(0, leftPosition) + (iChatInput->string).substr(rightPosition);

	iChatInput->string = new_string;
	iChatInput->cursorPosition = leftPosition;
	iChatInput->selectionPosition = iChatInput->cursorPosition;
	if (iChatInput->cursorPosition <= iChatInput->leftDrawPosition) {
		iChatInput->leftDrawPosition = iChatInput->getLeftDrawPositionByRight(
			chatNext(iChatInput->string, iChatInput->cursorPosition)
		);
		iChatInput->rightDrawPosition =
			iChatInput->getRightDrawPositionByLeft(iChatInput->leftDrawPosition);
	} else {
		iChatInput->rightDrawPosition =
			std::min(iChatInput->rightDrawPosition, (int)((iChatInput->string).length()));
		iChatInput->leftDrawPosition =
			iChatInput->getLeftDrawPositionByRight(iChatInput->rightDrawPosition);
	}
	iChatNormalizeInput();
}

void iChatInputEditing(SDL_Event *event) {
	if (!event || !iChatReady())
		return;
	iChatNormalizeInput();

	if (event->type == SDL_EVENT_MOUSE_WHEEL) {
		const int x = XGR_MouseObj.PosX;
		const int y = XGR_MouseObj.PosY;

		iChatHistory->SizeX += scrollSizeX; // check history bounds with scroll
		if (iChatHistory->check_xy(x, y)) {
			if (event->wheel.y > 0) {
				iChatHistory->scrollUp();
			} else if (event->wheel.y < 0) {
				iChatHistory->scrollDown();
			}
		}
		iChatHistory->SizeX -= scrollSizeX;
	} else if (event->type == SDL_EVENT_KEY_DOWN) {
		SDL_Keycode keycode = event->key.key;
		SDL_Keymod keymod = event->key.mod;

		// because SDL doesn't know what numlock is
		if (!(keymod & SDL_KMOD_NUM)) {
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

			int position = iChatInput->cursorPosition;

			if (keymod & SDL_KMOD_CTRL) {
				std::string string = iChatInput->string;
				int group = getCharGroup(string[chatPrevious(string, position)]);

				while (
					position > 1 && getCharGroup(string[chatPrevious(string, position)]) == group) {
					position = chatPrevious(string, position);
				}
			} else if (!(keymod & SDL_KMOD_SHIFT) && position != iChatInput->selectionPosition) {
				position = std::min(position, iChatInput->selectionPosition);
			} else {
				position = std::max(1, chatPrevious(iChatInput->string, position));
			}

			iChatInput->cursorPosition = position;
			if (!(keymod & SDL_KMOD_SHIFT)) {
				iChatInput->selectionPosition = position;
			}

			iChatInput->leftDrawPosition = std::min(iChatInput->leftDrawPosition, position);
			iChatInput->rightDrawPosition =
				iChatInput->getRightDrawPositionByLeft(iChatInput->leftDrawPosition);
		} else if (keycode == SDLK_RIGHT) {
			iChatCursorFlag = true;
			iChatCursorTimer = 0;

			int position = iChatInput->cursorPosition;

			if (keymod & SDL_KMOD_CTRL) {
				std::string string = iChatInput->string;
				int group = getCharGroup(string[position]);

				while (position < (int)string.length() && getCharGroup(string[position]) == group) {
					position = chatNext(string, position);
				}
			} else if (!(keymod & SDL_KMOD_SHIFT) && position != iChatInput->selectionPosition) {
				position = std::max(position, iChatInput->selectionPosition);
			} else {
				position = chatNext(iChatInput->string, position);
			}

			iChatInput->cursorPosition = position;
			if (!(keymod & SDL_KMOD_SHIFT)) {
				iChatInput->selectionPosition = position;
			}

			iChatInput->rightDrawPosition = std::max(iChatInput->rightDrawPosition, position);
			iChatInput->leftDrawPosition =
				iChatInput->getLeftDrawPositionByRight(iChatInput->rightDrawPosition);
		} else if (keycode == SDLK_UP) {
			iChatCursorFlag = true;
			iChatCursorTimer = 0;

			if (iChatInput->cursorPosition == 1) {
				iChatHistory->scrollUp();
			}

			iChatInput->cursorPosition = 1;
			if (!(keymod & SDL_KMOD_SHIFT)) {
				iChatInput->selectionPosition = iChatInput->cursorPosition;
			}

			iChatInput->leftDrawPosition = 1;
			iChatInput->rightDrawPosition =
				iChatInput->getRightDrawPositionByLeft(iChatInput->leftDrawPosition);
		} else if (keycode == SDLK_DOWN) {
			iChatCursorFlag = true;
			iChatCursorTimer = 0;

			if (iChatInput->cursorPosition == (int)((iChatInput->string).length())) {
				iChatHistory->scrollDown();
			}

			iChatInput->cursorPosition = (iChatInput->string).length();
			if (!(keymod & SDL_KMOD_SHIFT)) {
				iChatInput->selectionPosition = iChatInput->cursorPosition;
			}

			iChatInput->rightDrawPosition = iChatInput->cursorPosition;
			iChatInput->leftDrawPosition =
				iChatInput->getLeftDrawPositionByRight(iChatInput->rightDrawPosition);
		} else if (keycode == SDLK_A && (keymod & SDL_KMOD_CTRL)) {
			iChatInput->cursorPosition = (iChatInput->string).length();
			iChatInput->selectionPosition = 1;
			iChatInput->rightDrawPosition = iChatInput->cursorPosition;
			iChatInput->leftDrawPosition =
				iChatInput->getLeftDrawPositionByRight(iChatInput->rightDrawPosition);
		} else if (keycode == SDLK_V && (keymod & SDL_KMOD_CTRL) && SDL_HasClipboardText()) {
			iChatInputField prevBuf = *iChatInput;

			char *clipboard = SDL_GetClipboardText();
			if (clipboard) {
				iChatInputChar(clipboard);
				SDL_free(clipboard);
			}

			iChatInputPrev = prevBuf;
		} else if ((keycode == SDLK_C || keycode == SDLK_X) && (keymod & SDL_KMOD_CTRL)) {
			int leftSelectionPosition =
				std::min(iChatInput->cursorPosition, iChatInput->selectionPosition);
			int rightSelectionPosition =
				std::max(iChatInput->cursorPosition, iChatInput->selectionPosition);

			if (leftSelectionPosition == rightSelectionPosition) {
				return;
			}

			std::string copy_string =
				(iChatInput->string)
					.substr(leftSelectionPosition, rightSelectionPosition - leftSelectionPosition);
			SDL_SetClipboardText(copy_string.c_str());

			if (keycode == SDLK_X) {
				iChatInputField prevBuf = *iChatInput;
				iChatInputBack();
				iChatInputPrev = prevBuf;
			}
		} else if (keycode == SDLK_Z && (keymod & SDL_KMOD_CTRL)) {
			iChatInputField prevBuf = *iChatInput;
			*iChatInput = iChatInputPrev;
			iChatNormalizeInput();
			iChatInputPrev = prevBuf;
		}
	}
}

// returns "group" of the char: 0 - space, 1 - symbol, 2 - letter/number
int getCharGroup(char chr) {
	if (chr == ' ') {
		return 0;
	} else {
		std::string symbols = "~`!@\"#$;%^:&?*()_-+=[{]}\\|/\'<>,.";

		for (size_t i = 0; i < symbols.length(); i++) {
			if (chr == symbols[i]) {
				return 1;
			}
		}

		return 2;
	}
}

void iInitChatScreen(void) {
	int teamFilterID = ICS_GREEN_BUTTON + iChatFilterID;
	iChatButton *p;
	MessageElement *el;

	iInitChatButtons();
	iChatClearButtons();

	if (iChatFilterID > 3) {
		teamFilterID = teamFilterID + 9;
	}

	switch (iChatFilter) {
	case MESSAGE_FOR_ALL:
		p = iGetChatButton(ICS_ALL_BUTTON);
		p->CurState = 1;
		break;
	case MESSAGE_FOR_TEAM:
		p = iGetChatButton(teamFilterID);
		p->CurState = 1;
		break;
	case MESSAGE_FOR_PLAYER:
		if (!players_list.find(iChatFilterID)) {
			p = iGetChatButton(ICS_ALL_BUTTON);
			p->CurState = 1;
			iChatFilter = MESSAGE_FOR_ALL;
			iChatFilterID = 0;
		}
		p = iGetChatPlayerButton(iChatFilterID);
		if (!p) {
			iChatSkip = 0;
			while (!p) {
				iInitChatButtons();
				p = iGetChatPlayerButton(iChatFilterID);
				if (!p)
					iChatSkip++;
			}
		}
		p->CurState = 1;
		break;
	}

	if (iChatHistory->revision == message_dispatcher.revision && iChatHistory->muted == iChatMUTE)
		return;
	iChatHistory->revision = message_dispatcher.revision;
	iChatHistory->muted = iChatMUTE;
	const bool at_end = iChatHistory->data.empty() ||
						iChatHistory->position >=
							static_cast<int>(iChatHistory->data.size()) - ICS_HISTORY_MAX_MESSAGES;

	el = message_dispatcher.first();
	(iChatHistory->data).clear();
	if (!iChatMUTE) {
		while (el) {
			iChatHistory->add_str(el->message.c_str(), el->color);
			el = (MessageElement *)el->next;
		}
	}
	int maxPosition = std::max(0, (int)(iChatHistory->data.size()) - ICS_HISTORY_MAX_MESSAGES);
	iChatHistory->position =
		at_end ? maxPosition : std::clamp(iChatHistory->position, 0, maxPosition);
}

void iInitChatButtons(void) {
	int i;
	PlayerData *p = players_list.first();
	iChatButton *bt;

	if (players_list.size() < 5) {
		iChatSkip = 0;
		bt = iGetChatButton(ICS_UP_BUTTON);
		bt->flags |= ICS_HIDDEN_OBJECT;
		bt = iGetChatButton(ICS_DN_BUTTON);
		bt->flags |= ICS_HIDDEN_OBJECT;
	} else {
		bt = iGetChatButton(ICS_UP_BUTTON);
		bt->flags &= ~ICS_HIDDEN_OBJECT;
		bt = iGetChatButton(ICS_DN_BUTTON);
		bt->flags &= ~ICS_HIDDEN_OBJECT;
	}

	for (i = 0; i < 5; i++) {
		bt = iGetChatButton(ICS_PLAYER1_BUTTON + i);
		bt->flags |= ICS_HIDDEN_OBJECT;
	}
	if (!p)
		return;
	for (i = 0; i < iChatSkip; i++) {
		if (p) {
			p = (PlayerData *)p->next;
		} else {
			iChatSkip = 0;
			p = players_list.first();
		}
	}
	for (i = 0; i < 5; i++) {
		if (p->client_ID == GlobalStationID)
			p = (PlayerData *)p->next;
		if (!p)
			break;
		bt = iGetChatButton(ICS_PLAYER1_BUTTON + i);
		iResBuf->init();
		const auto world = std::clamp(
			static_cast<int>(p->body.world) + 1, 0, static_cast<int>(std::size(iChatWorlds)) - 1
		);
		const auto color = std::clamp(
			static_cast<int>(p->body.color), 0, static_cast<int>(std::size(iChatColors)) - 1
		);
		*iResBuf < p->name.c_str() < " " < *iChatWorlds[world] < "/" < *iChatColors[color];
		bt->set_string(iResBuf->address());
		bt->flags &= ~ICS_HIDDEN_OBJECT;
		bt->extData = p->client_ID;
		p = (PlayerData *)p->next;
		if (!p)
			break;
	}
}

iChatButton *iGetChatButton(int id) {
	iChatButton *p = (iChatButton *)iChatButtons->fPtr;
	while (p) {
		if (p->ID == id)
			return p;
		p = (iChatButton *)p->next;
	}
	return NULL;
}

void iChatClearLog(void) {
	message_dispatcher.clear_messages();
	iChatHistory->data.clear();
	iChatHistory->position = 0;
}

void iChatHistoryScreen::add_str(const char *str, int col) {
	col = std::clamp(col, 0, 9);
	int color = (iScreenChat) ? aciChatColors0[col] : aciChatColors1[col];
	if (aTextWidth32Utf8(str, font, 1) <= SizeX - 10) {
		data.emplace_back(str, color);
		return;
	}
	const std::string_view text(str);
	// Wider incoming messages may use the existing history rows and scroll.
	for (int start = 0; start < static_cast<int>(text.size());) {
		int end = chatNext(text, start);
		while (end < static_cast<int>(text.size())) {
			const int candidate = chatNext(text, end);
			const std::string line(text.substr(start, candidate - start));
			if (aTextWidth32Utf8(line.c_str(), font, 1) > SizeX - 10)
				break;
			end = candidate;
		}
		data.emplace_back(std::string(text.substr(start, end - start)).c_str(), color);
		start = end;
	}
}
