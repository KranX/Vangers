/* ---------------------------- INCLUDE SECTION ----------------------------- */

#include "../global.h"

#include "../xjoystick.h"

#include "hfont.h"
#include "iscreen.h"
#include "ikeys.h"
#include "controls.h"

/* ----------------------------- STRUCT SECTION ----------------------------- */

// flags...
#define iKEY_NO_RESET	0x01

struct iKeyControls
{
	int* keyCodes;
	int* defaultCodes;
	int* flags;

	void init(void);
	void reset(void);

	void setFlag(int id,int fl){ flags[id] |= fl; }
	void dropFlag(int id,int fl){ flags[id] &= ~fl; }
	void clearFlag(int id){ flags[id] = 0; }

	void addCode(int id,int key,int num = 0);
	void removeCode(int id,int num = 0);
	void addDefaultCode(int id,int key,int num = 0);

	int getCode(int id,int num);

	int GetID(int key);
	int CheckID(int id,int key);

	iKeyControls(void);
};

/* ----------------------------- EXTERN SECTION ----------------------------- */

extern int RecorderMode;

/* --------------------------- PROTOTYPE SECTION ---------------------------- */

void iInitControlObjects(void);

/* --------------------------- DEFINITION SECTION --------------------------- */

int iKeyFirstInit = 0;
iKeyControls* iControlsObj = NULL;

void iInitControls(void)
{
	if(!iControlsObj){
		iControlsObj = new iKeyControls;
		iControlsObj -> init();
	}
}

int iGetKeyID(int key)
{
	if(iControlsObj)
		return iControlsObj -> GetID(key);
	return -1;
}

int iCheckKeyID(int id,int key)
{
	if(iControlsObj)
		return iControlsObj -> CheckID(id,key);
	return 0;
}

void iKeyControls::init(void)
{
	addDefaultCode(iKEY_TURN_WHEELS_LEFT,SDL_SCANCODE_LEFT);
	addDefaultCode(iKEY_TURN_WHEELS_RIGHT,SDL_SCANCODE_RIGHT);
	addDefaultCode(iKEY_MOVE_FORWARD,SDL_SCANCODE_UP);
	addDefaultCode(iKEY_MOVE_BACKWARD,SDL_SCANCODE_DOWN);
	//addDefaultCode(iKEY_MOVE_BACKWARD,SDL_SCANCODE_KP_0,1);

	addDefaultCode(iKEY_TURN_OVER_LEFT,SDL_SCANCODE_END);
	addDefaultCode(iKEY_TURN_OVER_LEFT,SDL_SCANCODE_COMMA,1);

	addDefaultCode(iKEY_TURN_OVER_RIGHT,SDL_SCANCODE_PAGEDOWN);
	addDefaultCode(iKEY_TURN_OVER_RIGHT,SDL_SCANCODE_PERIOD,1);

	addDefaultCode(iKEY_DEVICE_ON,SDL_SCANCODE_PAGEUP);
	//addDefaultCode(iKEY_DEVICE_ON,VK_STICK_SWITCH_9,1);

	addDefaultCode(iKEY_DEVICE_OFF,SDL_SCANCODE_HOME);
	//addDefaultCode(iKEY_DEVICE_OFF,VK_STICK_SWITCH_7,1);

	addDefaultCode(iKEY_ACTIVATE_KID,SDL_SCANCODE_INSERT);
	addDefaultCode(iKEY_ACTIVATE_KID,SDL_SCANCODE_SLASH,1);

//	  addDefaultCode(iKEY_ACTIVATE_KID,'A',1);
	addDefaultCode(iKEY_ACCELERATION,SDL_SCANCODE_LSHIFT);
//	  addDefaultCode(iKEY_VERTICAL_THRUST,'Z');
	addDefaultCode(iKEY_INVENTORY,SDL_SCANCODE_RETURN);
	//addDefaultCode(iKEY_INVENTORY,VK_STICK_SWITCH_3,1);

	addDefaultCode(iKEY_OPEN,SDL_SCANCODE_SPACE);
	//addDefaultCode(iKEY_OPEN,VK_STICK_SWITCH_2,1);

	addDefaultCode(iKEY_FIRE_WEAPON1,SDL_SCANCODE_1);
	addDefaultCode(iKEY_FIRE_WEAPON2,SDL_SCANCODE_2);
	addDefaultCode(iKEY_FIRE_WEAPON3,SDL_SCANCODE_3);
	addDefaultCode(iKEY_FIRE_WEAPON4,SDL_SCANCODE_4);
	addDefaultCode(iKEY_FIRE_ALL_WEAPONS,SDL_SCANCODE_LCTRL);
	addDefaultCode(iKEY_FIRE_ALL_WEAPONS,SDL_SCANCODE_RCTRL, 1);

	addDefaultCode(iKEY_CHANGE_TARGET,SDL_SCANCODE_TAB);
	//addDefaultCode(iKEY_CHANGE_TARGET,VK_STICK_SWITCH_1,1);

	addDefaultCode(iKEY_FULLSCREEN,SDL_SCANCODE_F1);
	addDefaultCode(iKEY_REDUCE_VIEW,SDL_SCANCODE_F2);
	addDefaultCode(iKEY_ENLARGE_VIEW,SDL_SCANCODE_F3);
	addDefaultCode(iKEY_ROTATION,SDL_SCANCODE_F5);
	addDefaultCode(iKEY_AUTO_ZOOM,SDL_SCANCODE_F6);
	addDefaultCode(iKEY_AUTO_TILT,SDL_SCANCODE_F7);
	addDefaultCode(iKEY_ZOOM_IN,SDL_SCANCODE_KP_PLUS);
	addDefaultCode(iKEY_ZOOM_OUT,SDL_SCANCODE_KP_MINUS);
	addDefaultCode(iKEY_ZOOM_STANDART,'0');

	addDefaultCode(iKEY_HANDBRAKE,SDL_SCANCODE_C);

	addDefaultCode(iKEY_USE_GLUEK,SDL_SCANCODE_5);
	addDefaultCode(iKEY_JOYSTICK_SWITCH,VK_BUTTON_1);

	addDefaultCode(iKEY_SKIP_TEXT,SDL_SCANCODE_TAB);

	addDefaultCode(iKEY_USE_VECTOR, SDL_SCANCODE_GRAVE);
	addDefaultCode(iKEY_FRAG_INFO,SDL_SCANCODE_S);
	addDefaultCode(iKEY_CHAT,SDL_SCANCODE_F12);

	addDefaultCode(iKEY_EXIT,SDL_SCANCODE_F10);

	addDefaultCode(iKEY_SCREENSHOT,SDL_SCANCODE_F9);

	setFlag(iKEY_JOYSTICK_SWITCH,iKEY_NO_RESET);

	reset();
}

void iKeyControls::reset(void)
{
	int i;
	if(iKeyFirstInit){
		for(i = 0; i < iKEY_MAX_ID; i ++){
			if(!(flags[i] & iKEY_NO_RESET)){
				memcpy(keyCodes + i * iKEY_OBJECT_SIZE,defaultCodes + i * iKEY_OBJECT_SIZE,iKEY_OBJECT_SIZE * sizeof(int));
			}
		}
	}
	else {
		memcpy(keyCodes,defaultCodes,iKEY_OBJECT_SIZE * iKEY_MAX_ID * sizeof(int));
		iKeyFirstInit = 1;
	}
}

void iKeyControls::addDefaultCode(int id,int key,int num)
{
	defaultCodes[id * iKEY_OBJECT_SIZE + num] = key;
}

void iKeyControls::addCode(int id,int key,int num)
{
	keyCodes[id * iKEY_OBJECT_SIZE + num] = key;
}

void iKeyControls::removeCode(int id,int num)
{
	keyCodes[id * iKEY_OBJECT_SIZE + num] = 0;
}

int iKeyControls::getCode(int id,int num)
{
	return keyCodes[id * iKEY_OBJECT_SIZE + num];
}

int iKeyControls::GetID(int key)
{
	int i,id = -1,sz = iKEY_OBJECT_SIZE * iKEY_MAX_ID;
	for(i = 0; i < sz; i ++){
		if(keyCodes[i] == key){
			id = i / iKEY_OBJECT_SIZE;
			return id;
		}
	}
	return id;
}

int iKeyControls::CheckID(int id,int key)
{
	int i,index = iKEY_OBJECT_SIZE * id;
	for(i = 0; i < iKEY_OBJECT_SIZE; i ++){
		if(keyCodes[i + index] == key)
			return 1;
	}
	return 0;
}

void iSetControlCode(int id,int key,int num)
{
	if(iControlsObj)
		iControlsObj -> addCode(id,key,num);
}

void iResetControlCode(int id,int num)
{
	if(iControlsObj)
		iControlsObj -> removeCode(id,num);
}

int iGetControlCode(int id,int num)
{
	if(iControlsObj)
		return iControlsObj -> getCode(id,num);

	return 0;
}

void iResetControls(void)
{
	if(iControlsObj)
		iControlsObj -> reset();
}

iKeyControls::iKeyControls(void)
{
	keyCodes = new int[iKEY_OBJECT_SIZE * iKEY_MAX_ID];
	defaultCodes = new int[iKEY_OBJECT_SIZE * iKEY_MAX_ID];
	flags = new int[iKEY_MAX_ID];

	memset(keyCodes,0,iKEY_OBJECT_SIZE * iKEY_MAX_ID * sizeof(int));
	memset(defaultCodes,0,iKEY_OBJECT_SIZE * iKEY_MAX_ID * sizeof(int));
	memset(flags,0,iKEY_MAX_ID * sizeof(int));
}

void iSaveControls(void)
{
	if(!iControlsObj || RecorderMode) return;
	XStream fh("controls.dat",XS_OUT);
	fh < (int)iKEY_MAX_ID < (int)iKEY_OBJECT_SIZE;
	fh.write(iControlsObj -> keyCodes,iKEY_OBJECT_SIZE * iKEY_MAX_ID * sizeof(int));
	fh.close();
}

void iLoadControls(void)
{
	int sz0,sz1;
	if(!iControlsObj || RecorderMode) return;

	XStream fh(0);
	if(!fh.open("controls.dat",XS_IN)) return;

	fh > sz0 > sz1;
	if(sz0 != iKEY_MAX_ID || sz1 != iKEY_OBJECT_SIZE){
		fh.close();
		return;
	}
	fh.read(iControlsObj -> keyCodes,iKEY_OBJECT_SIZE * iKEY_MAX_ID * sizeof(int));
	fh.close();
	iInitControlObjects();
}

int iKeyPressed(int id)
{
	int i,code, state = 0;
	SDL_Joystick *joy = get_joystick();
	SDL_GameController *ctrl = get_gamecontroller();

	if(!iControlsObj) return 0;
	for(i = 0; i < iKEY_OBJECT_SIZE; i ++){
		code = iControlsObj->getCode(id, i);
		//std::cout<<"iKeyPressed code:"<<(int)code<<" id:"<<id<<" i:"<<i<<std::endl;
		if(code) {
			if (code & SDLK_JOYSTICK_BUTTON_MASK && joy) {
				state = SDL_JoystickGetButton(joy, code ^ SDLK_JOYSTICK_BUTTON_MASK);
			} else if (code & SDLK_GAMECONTROLLER_BUTTON_MASK && ctrl) {
				state = SDL_GameControllerGetButton(ctrl, (SDL_GameControllerButton)(code ^ SDLK_GAMECONTROLLER_BUTTON_MASK));
			} else if (code & SDLK_JOYSTICK_HAT_MASK && joy) {
				state = SDL_JoystickGetHat(joy, (code ^ SDLK_JOYSTICK_HAT_MASK) / 10 ) == (code ^ SDLK_JOYSTICK_HAT_MASK) % 10;
			} else if (code & SDLK_SCANCODE_MASK) {
				state = SDL_GetKeyboardState(NULL)[SDL_GetScancodeFromKey(code)];
			} else {
				state = SDL_GetKeyboardState(NULL)[code];
			}
			if (state) {
				return state;
			}
		}
	}
	return 0;
}

char* iGetJoystickButtonName(int vkey)
{
	return NULL;
}

