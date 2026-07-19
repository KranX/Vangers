/* ---------------------------- INCLUDE SECTION ----------------------------- */

#include "../global.h"
#include "../settings/input_binding.h"
#include "../settings/settings.h"

#include "../xgamepad.h"

#include "controls.h"
#include "hfont.h"
#include "ikeys.h"
#include "iscreen.h"
#include "settings_adapter.h"

/* ----------------------------- STRUCT SECTION ----------------------------- */

struct iKeyControls {
	int *keyCodes;
	int *defaultCodes;

	void init(void);
	void reset(void);

	void addCode(int id, int key, int num = 0);
	void removeCode(int id, int num = 0);
	void addDefaultCode(int id, int key, int num = 0);

	int getCode(int id, int num);

	int GetID(int key);
	int CheckID(int id, int key);

	iKeyControls(void);
};

/* --------------------------- PROTOTYPE SECTION ---------------------------- */

void iInitControlObjects(void);

/* --------------------------- DEFINITION SECTION --------------------------- */

iKeyControls *iControlsObj = NULL;

void iInitControls(void) {
	if (!iControlsObj) {
		iControlsObj = new iKeyControls;
		iControlsObj->init();
	}
}

int iGetKeyID(int key) {
	if (iControlsObj)
		return iControlsObj->GetID(key);
	return -1;
}

int iCheckKeyID(int id, int key) {
	if (iControlsObj && iControlsObj->CheckID(id, key))
		return 1;
	if ((key & SDLK_GAMEPAD_BUTTON_MASK) != 0) {
		const char *action = vangers::settings::legacy_control_action_name(id);
		if (action) {
			const auto button = static_cast<SDL_GamepadButton>(key ^ SDLK_GAMEPAD_BUTTON_MASK);
			return XGamepadButtonMatchesAction(action, button) ? 1 : 0;
		}
	}
	return 0;
}

void iKeyControls::init(void) {
	// Movement
	addDefaultCode(iKEY_TURN_WHEELS_LEFT, SDL_SCANCODE_LEFT);
	addDefaultCode(iKEY_TURN_WHEELS_RIGHT, SDL_SCANCODE_RIGHT);
	addDefaultCode(iKEY_MOVE_FORWARD, SDL_SCANCODE_UP);
	addDefaultCode(iKEY_MOVE_BACKWARD, SDL_SCANCODE_DOWN);
	// addDefaultCode(iKEY_MOVE_BACKWARD,SDL_SCANCODE_KP_0,1);
	addDefaultCode(iKEY_ACCELERATION, SDL_SCANCODE_LSHIFT);
	addDefaultCode(iKEY_ACCELERATION, SDL_SCANCODE_RSHIFT, 1);
	//	addDefaultCode(iKEY_ACTIVATE_KID,'A',1);
	addDefaultCode(iKEY_ACTIVATE_KID, SDL_SCANCODE_W);
	//	addDefaultCode(iKEY_ACTIVATE_KID,SDL_SCANCODE_SLASH,1);
	addDefaultCode(iKEY_HANDBRAKE, SDL_SCANCODE_Q);

	// Utility
	addDefaultCode(iKEY_OPEN, SDL_SCANCODE_SPACE);
	addDefaultCode(iKEY_INVENTORY, SDL_SCANCODE_RETURN);
	addDefaultCode(iKEY_TURN_OVER_LEFT, SDL_SCANCODE_A);
	//	addDefaultCode(iKEY_TURN_OVER_LEFT,SDL_SCANCODE_COMMA,1);
	addDefaultCode(iKEY_TURN_OVER_RIGHT, SDL_SCANCODE_D);
	//	addDefaultCode(iKEY_TURN_OVER_RIGHT,SDL_SCANCODE_PERIOD,1);
	addDefaultCode(iKEY_DEVICE_ON, SDL_SCANCODE_R);
	addDefaultCode(iKEY_DEVICE_ON, SDL_SCANCODE_PAGEUP, 1);
	addDefaultCode(iKEY_DEVICE_OFF, SDL_SCANCODE_F);
	addDefaultCode(iKEY_DEVICE_OFF, SDL_SCANCODE_PAGEDOWN, 1);

	// Combat
	addDefaultCode(iKEY_FIRE_ALL_WEAPONS, SDL_SCANCODE_E);
	addDefaultCode(iKEY_FIRE_ALL_WEAPONS, SDL_SCANCODE_LCTRL, 1);
	addDefaultCode(iKEY_FIRE_WEAPON4, SDL_SCANCODE_4);
	addDefaultCode(iKEY_USE_GLUEK, SDL_SCANCODE_5);
	addDefaultCode(iKEY_FIRE_WEAPON1, SDL_SCANCODE_1);
	addDefaultCode(iKEY_FIRE_WEAPON2, SDL_SCANCODE_2);
	addDefaultCode(iKEY_FIRE_WEAPON3, SDL_SCANCODE_3);
	addDefaultCode(iKEY_USE_VECTOR, SDL_SCANCODE_V);
	addDefaultCode(iKEY_CHANGE_TARGET, SDL_SCANCODE_TAB);

	// Camera
	addDefaultCode(iKEY_FULLSCREEN, SDL_SCANCODE_F1);
	addDefaultCode(iKEY_ZOOM_OUT, SDL_SCANCODE_MINUS);
	addDefaultCode(iKEY_ZOOM_IN, SDL_SCANCODE_EQUALS);
	addDefaultCode(iKEY_ZOOM_STANDART, SDL_SCANCODE_0);
	addDefaultCode(iKEY_ROTATION, SDL_SCANCODE_F5);
	addDefaultCode(iKEY_AUTO_TILT, SDL_SCANCODE_F6);
	addDefaultCode(iKEY_AUTO_ZOOM, SDL_SCANCODE_F7);

	// Multiplayer and misc.
	addDefaultCode(iKEY_CHAT, SDL_SCANCODE_F3);
	addDefaultCode(iKEY_FRAG_INFO, SDL_SCANCODE_S);
	addDefaultCode(iKEY_SCREENSHOT, SDL_SCANCODE_F9);
	addDefaultCode(iKEY_SKIP_TEXT, SDL_SCANCODE_TAB);
	//	addDefaultCode(iKEY_EXIT,SDL_SCANCODE_F10);
	//	addDefaultCode(iKEY_REDUCE_VIEW,SDL_SCANCODE_F2);
	//	addDefaultCode(iKEY_ENLARGE_VIEW,SDL_SCANCODE_F3);

	reset();
}

void iKeyControls::reset(void) {
	memcpy(keyCodes, defaultCodes, iKEY_OBJECT_SIZE * iKEY_MAX_ID * sizeof(int));
}

void iKeyControls::addDefaultCode(int id, int key, int num) {
	defaultCodes[id * iKEY_OBJECT_SIZE + num] = key;
}

void iKeyControls::addCode(int id, int key, int num) {
	keyCodes[id * iKEY_OBJECT_SIZE + num] = key;
}

void iKeyControls::removeCode(int id, int num) {
	keyCodes[id * iKEY_OBJECT_SIZE + num] = 0;
}

int iKeyControls::getCode(int id, int num) {
	return keyCodes[id * iKEY_OBJECT_SIZE + num];
}

int iKeyControls::GetID(int key) {
	int i, id = -1, sz = iKEY_OBJECT_SIZE * iKEY_MAX_ID;
	for (i = 0; i < sz; i++) {
		if (keyCodes[i] == key) {
			id = i / iKEY_OBJECT_SIZE;
			return id;
		}
	}
	return id;
}

int iKeyControls::CheckID(int id, int key) {
	int i, index = iKEY_OBJECT_SIZE * id;
	for (i = 0; i < iKEY_OBJECT_SIZE; i++) {
		if (keyCodes[i + index] == key)
			return 1;
	}
	return 0;
}

void iSetControlCode(int id, int key, int num) {
	if (iControlsObj)
		iControlsObj->addCode(id, key, num);
}

void iResetControlCode(int id, int num) {
	if (iControlsObj)
		iControlsObj->removeCode(id, num);
}

int iGetControlCode(int id, int num) {
	if (iControlsObj)
		return iControlsObj->getCode(id, num);

	return 0;
}

void iResetControls(void) {
	if (!iControlsObj)
		return;

	vangers::settings::SettingsManager &manager = vangers::settings::settings_manager();
	if (!manager.is_loaded())
		manager.load();
	vangers::settings::reset_input_bindings_to_defaults(manager.get_mutable());
	vangers::settings::apply_settings_to_controls();
}

iKeyControls::iKeyControls(void) {
	keyCodes = new int[iKEY_OBJECT_SIZE * iKEY_MAX_ID];
	defaultCodes = new int[iKEY_OBJECT_SIZE * iKEY_MAX_ID];

	memset(keyCodes, 0, iKEY_OBJECT_SIZE * iKEY_MAX_ID * sizeof(int));
	memset(defaultCodes, 0, iKEY_OBJECT_SIZE * iKEY_MAX_ID * sizeof(int));
}

void iSaveControls(void) {
	if (!iControlsObj)
		return;
	vangers::settings::capture_settings_from_controls();
	vangers::settings::save_settings();
}

void iLoadControls(void) {
	if (!iControlsObj)
		return;
	vangers::settings::apply_settings_to_controls();
	iInitControlObjects();
}

int iKeyPressed(int id) {
	int i, code, state = 0;

	if (!iControlsObj)
		return 0;
	if (const char *action = vangers::settings::legacy_control_action_name(id);
		action && XGamepadActionPressed(action)) {
		return 1;
	}
	for (i = 0; i < iKEY_OBJECT_SIZE; i++) {
		code = iControlsObj->getCode(id, i);
		// std::cout<<"iKeyPressed code:"<<(int)code<<" id:"<<id<<" i:"<<i<<std::endl;
		if (code) {
			if (code & SDLK_GAMEPAD_BUTTON_MASK) {
				state = XGamepadButtonPressed(
					static_cast<SDL_GamepadButton>(code ^ SDLK_GAMEPAD_BUTTON_MASK)
				);
			} else if (code & SDLK_SCANCODE_MASK) {
				state = SDL_GetKeyboardState(NULL)[SDL_GetScancodeFromKey(code, nullptr)];
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
