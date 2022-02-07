
// iChatScreenObject::type...
#define ICS_STRING		0x01
#define ICS_INPUT_FIELD 	0x02

// iChatScreenObject::flags...
#define ICS_HIDDEN_OBJECT	0x01

extern int iChatMUTE;

struct iChatScreenObject : XListElement
{
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

	int check_xy(int x,int y){
		return (x >= PosX && x < PosX + SizeX && y >= PosY && y < PosY + SizeY);
	}

	void init(int x,int y,int sx,int sy,int col1,int col2);

	void set_font(int f){ font = f; }

	iChatScreenObject(void);
};

#define ISC_MAX_STRING_LEN		100

struct iChatInputField : iChatScreenObject
{
	int color;
	std::string string;
	XBuffer* XConv;

	int cursorPosition;
	int selectionPosition;

	int leftDrawPosition; // position of the first char drawn in the string
	int rightDrawPosition; // position of the last char drawn in the string + 1
	// drawn string = string[leftDrawPosition, rightDrawPosition)

	int getLeftDrawPositionByRight(int rightPosition);
	int getRightDrawPositionByLeft(int leftPosition);

	void selectionRedraw(void);
	void counterRedraw(void);

	virtual void redraw(void);

	iChatInputField(void);
};

#define ICS_MAX_HISTORY_OBJ		20

struct iChatHistoryScreen : iChatScreenObject
{
	int NumStr;
	char** data;
	int* ColorData;

	virtual void redraw(void);

	void clear(void);
	void add_str(char* str,int id,int col = 0);

	iChatHistoryScreen(void);
	~iChatHistoryScreen(void);
};

struct iChatButton : iChatScreenObject
{
	char* string;

	int NumState;
	int CurState;

	int extData;

	int* StateColors;

	virtual void redraw(void);

	void set_string(const char* p){ strcpy(string,p); }
	void set_color(int id,int col){ StateColors[id] = col; }

	iChatButton(int num_state);
	~iChatButton(void);
};
