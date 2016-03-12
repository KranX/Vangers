#include <SDL.h>
#include "xglobal.h"

const char* xtGetKeyNameText(SDL_Keycode key) {
	return SDL_GetKeyName(key);
}
