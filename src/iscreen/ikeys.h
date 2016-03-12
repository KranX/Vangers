
//#define _ISCREEN_GERMAN_

#define CUR_KEY_PRESSED 	0x01
#define CUR_KEY_UNPRESSED	0x02

#undef SHIFT_PRESSED

const int 	MAX_SIZE		= 500;

const int 	CTRL_PRESSED		= 0x01;
const int 	SHIFT_PRESSED		= 0x02;
const int 	KBD_ENABLE		= 0x04;

struct KeyBuffer
{
	int* table;
	char* state_table;

	int size;
	int first_index;
	int last_index;
	int flag;

	int cur_state;

	void clear(void);
	void put(int key,int state);
	int get(void);

	KeyBuffer(void);
};

void key(int);
void unpress_key(int);

void KBD_init(void);

extern KeyBuffer* KeyBuf;
extern int KeyType;

