
/* ---------------------------- INCLUDE SECTION ----------------------------- */


#include "../global.h"

#include "ikeys.h"

/* ----------------------------- EXTERN SECTION ----------------------------- */

extern int MultiLog;

/* --------------------------- PROTOTYPE SECTION ---------------------------- */

void ShowMTargets(void);
/* --------------------------- DEFINITION SECTION --------------------------- */

int KeyType = 0;  // 1 - BIOSKEY...
KeyBuffer* KeyBuf = NULL;

void KBD_init(void) {
	if(!KeyBuf)
		KeyBuf = new KeyBuffer;

	set_key_nadlers(&key, &unpress_key);
}

void key(SDL_Event *key) {
	if(KeyBuf -> flag & KBD_ENABLE &&
	   (key->type == SDL_KEYDOWN || key->type == SDL_TEXTINPUT)
	) {
		KeyBuf -> put(key, CUR_KEY_PRESSED);
	}
}

void unpress_key(SDL_Event *key) {
	if (key->type == SDL_KEYUP) {
		KeyBuf -> put(key, CUR_KEY_UNPRESSED);
	}
}

KeyBuffer::KeyBuffer(void)
{
	table = new SDL_Event[MAX_SIZE];
	state_table = new char[MAX_SIZE];

	clear();
	flag = KBD_ENABLE;
}

void KeyBuffer::put(SDL_Event *key, int state)
{
	if(size < MAX_SIZE) {
		table[last_index] = *key;
//		std::cout<<"KeyBuffer::put key:"<<key<<" last_index:"<<last_index<<std::endl;
		state_table[last_index] = state;
		last_index ++;
		if(last_index >= MAX_SIZE)
			last_index = 0;
		size ++;
	}
//	  else
//		  ErrH.Abort("Keyboard buffer overflow");
}

SDL_Event *KeyBuffer::get(void)
{
	SDL_Event *key;
	if(size){
		key = &table[first_index];
//		std::cout<<"KeyBuffer::get key:"<<key<<" first_index:"<<first_index<<std::endl;
		cur_state = state_table[first_index];

		first_index ++;
		if(first_index >= MAX_SIZE)
			first_index = 0;

		//if(cur_state == CUR_KEY_UNPRESSED)
		//	key |= 0x1000;

		size --;

		return key;
	}
	return NULL;
}

void KeyBuffer::clear(void)
{
	size = 0;
	first_index = 0;
	last_index = 0;
	cur_state = 0;
}




