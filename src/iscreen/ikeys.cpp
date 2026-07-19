
/* ---------------------------- INCLUDE SECTION ----------------------------- */

#include "../global.h"
#include "../xgamepad.h"

#include "ikeys.h"

/* ----------------------------- EXTERN SECTION ----------------------------- */

extern int MultiLog;

/* --------------------------- PROTOTYPE SECTION ---------------------------- */

void ShowMTargets(void);
/* --------------------------- DEFINITION SECTION --------------------------- */

int KeyType = 0; // 1 - BIOSKEY...
KeyBuffer *KeyBuf = NULL;

void KBD_init(void) {
	if (!KeyBuf)
		KeyBuf = new KeyBuffer;

	set_key_handlers(&key, NULL);
}

void key(SDL_Event *key) {
	if (KeyBuf->flag & KBD_ENABLE &&
		(key->type == SDL_EVENT_KEY_DOWN || key->type == SDL_EVENT_TEXT_INPUT ||
			key->type == SDL_EVENT_MOUSE_WHEEL ||
			(key->type == SDL_EVENT_GAMEPAD_BUTTON_DOWN && XGamepadOwnsEvent(*key)))) {
		KeyBuf->put(key, CUR_KEY_PRESSED);
	}
}

void unpress_key(SDL_Event *key) {
	if (key->type == SDL_EVENT_KEY_UP) {
		KeyBuf->put(key, CUR_KEY_UNPRESSED);
	}
}

KeyBuffer::KeyBuffer(void) {
	table = new SDL_Event[MAX_SIZE];
	text_table = new std::string[MAX_SIZE];
	state_table = new char[MAX_SIZE];

	clear();
	flag = KBD_ENABLE;
}

KeyBuffer::~KeyBuffer(void) {
	delete[] table;
	delete[] text_table;
	delete[] state_table;
}

void KeyBuffer::put(SDL_Event *key, int state) {
	if (size < MAX_SIZE) {
		table[last_index] = *key;
		if (key->type == SDL_EVENT_TEXT_INPUT) {
			text_table[last_index] = key->text.text ? key->text.text : "";
			table[last_index].text.text = text_table[last_index].c_str();
		} else {
			text_table[last_index].clear();
		}
		//		std::cout<<"KeyBuffer::put key:"<<key<<" last_index:"<<last_index<<std::endl;
		state_table[last_index] = state;
		last_index++;
		if (last_index >= MAX_SIZE)
			last_index = 0;
		size++;
	}
	//	  else
	//		  ErrH.Abort("Keyboard buffer overflow");
}

SDL_Event *KeyBuffer::get(void) {
	SDL_Event *key;
	if (size) {
		key = &table[first_index];
		//		std::cout<<"KeyBuffer::get key:"<<key<<" first_index:"<<first_index<<std::endl;
		cur_state = state_table[first_index];

		first_index++;
		if (first_index >= MAX_SIZE)
			first_index = 0;

		// if(cur_state == CUR_KEY_UNPRESSED)
		//	key |= 0x1000;

		size--;

		return key;
	}
	return NULL;
}

void KeyBuffer::clear(void) {
	size = 0;
	first_index = 0;
	last_index = 0;
	cur_state = 0;
}
