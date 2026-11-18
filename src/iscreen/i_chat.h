#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

// iChatScreenObject::type...
#define ICS_STRING 0x01
#define ICS_INPUT_FIELD 0x02

// iChatScreenObject::flags...
#define ICS_HIDDEN_OBJECT 0x01

extern int iChatMUTE;

struct iChatScreenObject: XListElement {
	int ID;

	int type;
	int flags;

	int PosX;
	int PosY;

	int SizeX;
	int SizeY;

	int font;

	int fonColor;
	int borderColor;

	virtual void redraw(void);
	virtual void flush(void);

	int check_xy(int x, int y) {
		return (x >= PosX && x < PosX + SizeX && y >= PosY && y < PosY + SizeY);
	}

	void init(int x, int y, int sx, int sy, int col1, int col2);

	void set_font(int f) {
		font = f;
	}

	iChatScreenObject(void);
};

struct iChatInputField: iChatScreenObject {
	int color;
	std::string string;

	// Byte offsets at UTF-8 scalar boundaries.
	int cursorPosition;
	int selectionPosition;

	int leftDrawPosition;  // position of the first char drawn in the string
	int rightDrawPosition; // position of the last char drawn in the string + 1
	// drawn string = string[leftDrawPosition, rightDrawPosition)

	int getLeftDrawPositionByRight(int rightPosition);
	int getRightDrawPositionByLeft(int leftPosition);

	void selectionRedraw(void);
	void counterRedraw(void);

	virtual void redraw(void);

	iChatInputField(void);
};

struct Message {
	std::string text;
	int color;

	Message(const char *text, int color) {
		this->text = std::string(text);
		this->color = color;
	}
};

#define ICS_HISTORY_MAX_MESSAGES 15

struct iChatHistoryScreen: iChatScreenObject {
	int position; // position of the first line drawn in the history
	std::size_t revision = 0;
	int muted = -1;
	std::vector<Message> data;

	void redrawScroll(void);
	void scrollUp(void) {
		position = std::max(0, position - 1);
	}
	void scrollDown(void) {
		position =
			std::min(std::max(0, (int)(data.size()) - ICS_HISTORY_MAX_MESSAGES), position + 1);
	}

	virtual void redraw(void);

	void add_str(const char *str, int col = 0);

	iChatHistoryScreen(void);
};

struct iChatButton: iChatScreenObject {
	std::string string;

	int NumState;
	int CurState;

	int player_slot = 0;
	std::uint64_t player_identity = 0;

	int *StateColors;

	virtual void redraw(void);

	void set_string(const char *p) {
		string = p;
	}
	void set_player_label(std::string_view name, std::string_view details);
	void set_color(int id, int col) {
		StateColors[id] = col;
	}

	iChatButton(int num_state);
	~iChatButton(void);
};
