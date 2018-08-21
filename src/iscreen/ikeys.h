#ifndef __IKEYS_H__
#define __IKEYS_H__
#include <SDL.h>

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
	SDL_Event* table;
	char* state_table;

	int size;
	int first_index;
	int last_index;
	int flag;

	int cur_state;

	void clear(void);
	void put(SDL_Event *key, int state);
	SDL_Event *get(void);

	KeyBuffer(void);
};

void key(SDL_Event *);
void unpress_key(SDL_Event *);

void KBD_init(void);

extern KeyBuffer* KeyBuf;
extern int KeyType;

#endif
