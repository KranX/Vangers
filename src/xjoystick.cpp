/*==========================================================================
 *
 * Functions:
 * XJoystickInit()
 * XJoystickCleanup()
 * XJoystickInput(void);
 *
 ***************************************************************************/

#define KDWIN
#define __ROUND__
#include "xjoystick.h"
#include "xglobal.h"
#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

SDL_Gamepad *gamepad = NULL;
SDL_Joystick *joy = NULL;

int JoystickMode = JOYSTICK_None;
int JoystickStickSwitchButton = VK_BUTTON_2;
int CurrentStickSwitchCode = 0;
Uint64 next_joystick_input = 0;

int JoystickAvailable = 0;

// Global State of Joystick
XJOYSTATE XJoystickState;

int JoystickWhatsPressedNow() {
	std::cout << "JoystickWhatsPressedNow " << std::endl;
	if (!JoystickAvailable)
		return 0;
	if (SDL_GetTicks() > next_joystick_input) {
		XJoystickInput();
		next_joystick_input = SDL_GetTicks() + 20;
	}
	if (CurrentStickSwitchCode)
		return CurrentStickSwitchCode;
	for (int i = 0; i < 32; i++)
		if (XJoystickState.rgbButtons[i])
			return VK_BUTTON_1 + i;
	return 0;
}

int isJoystickButtonPressed(int vk_code) {
	std::cout << "isJoystickButtonPressed " << vk_code << std::endl;
	if (!JoystickAvailable)
		return 0;
	if (SDL_GetTicks() > next_joystick_input) {
		XJoystickInput();
		next_joystick_input = SDL_GetTicks() + 20;
	}
	if (vk_code & VK_BUTTON)
		return XJoystickState.rgbButtons[vk_code - VK_BUTTON_1] ? 1 : 0;
	if (vk_code & VK_STICK_SWITCH)
		return vk_code == CurrentStickSwitchCode ? 1 : 0;
	return 0;
}

//===========================================================================
// XJoystickInit
//
// Creates and initializes joysticks.
//
// Parameters:
//
// Returns:
//
//===========================================================================
bool XJoystickInit() {
	if (!SDL_InitSubSystem(SDL_INIT_GAMEPAD)) {
		std::cout << "Unable to initialize the joystick subsystem" << std::endl;
		return false;
	}
	SDL_SetJoystickEventsEnabled(true);
	SDL_SetGamepadEventsEnabled(true);

	int joystickCount = 0;
	SDL_JoystickID *joysticks = SDL_GetJoysticks(&joystickCount);
	if (!joysticks)
		return false;

	for (int i = 0; i < joystickCount; ++i) {
		SDL_JoystickID id = joysticks[i];
		if (SDL_IsGamepad(id)) {
			const char *name = SDL_GetGamepadNameForID(id);
			printf(
				"Joystick \'%u\' is a compatible gamepad, named \'%s\'\n",
				static_cast<unsigned>(id),
				name ? name : "unknown"
			);
			gamepad = SDL_OpenGamepad(id);
			joy = gamepad ? SDL_GetGamepadJoystick(gamepad) : nullptr;
			if (joy)
				break;
			if (gamepad) {
				SDL_CloseGamepad(gamepad);
				gamepad = nullptr;
			}
		} else {
			const char *name = SDL_GetJoystickNameForID(id);
			printf(
				"Joystick \'%u\' named \'%s\' is not a compatible gamepad.\n",
				static_cast<unsigned>(id),
				name ? name : "unknown"
			);
		}
	}
	if (gamepad) {
		SDL_free(joysticks);
		JoystickAvailable = 1;
		return true;
	}
	for (int i = 0; i < joystickCount; ++i) {
		SDL_JoystickID id = joysticks[i];
		joy = SDL_OpenJoystick(id);
		if (joy) {
			const char *name = SDL_GetJoystickName(joy);
			printf(
				"Joystick \'%u\' is available, named \'%s\'\n",
				static_cast<unsigned>(id),
				name ? name : "unknown"
			);
			break;
		}
	}
	SDL_free(joysticks);
	if (joy) {
		JoystickAvailable = 1;
		return true;
	} else {
		return false;
	}
}

SDL_Gamepad *get_gamepad() {
	return gamepad;
}

SDL_Joystick *get_joystick() {
	return joy;
}

//===========================================================================
// XJoystickCleanup
//
// Cleans up joystick structures
//
// Parameters: none
//
// Returns: nothing
//
//===========================================================================
void XJoystickCleanup(void) {
	if (gamepad) {
		SDL_CloseGamepad(gamepad);
		gamepad = nullptr;
		joy = nullptr;
	} else if (joy) {
		SDL_CloseJoystick(joy);
		joy = nullptr;
	}
	JoystickAvailable = 0;
} //*** end XJoystickCleanup()

//===========================================================================
// XJoystickInput
//
// Processes data from the input device.  Uses GetDeviceState().
//
// Returns: 1 if joysticks state was updated, 0 otherwise.
//
//===========================================================================
int XJoystickInput() {
	std::cout << "XJoystickInput" << std::endl;
	if (!JoystickAvailable)
		return 0;

	SDL_UpdateJoysticks(); // update all open joysticks

	for (int i = 0; i < SDL_GetNumJoystickButtons(joy) && i < 32; i++)
		XJoystickState.rgbButtons[i] = SDL_GetJoystickButton(joy, i);

	for (int i = 0; i < SDL_GetNumJoystickAxes(joy) && i < 2; i++) {
		if (i == 0)
			XJoystickState.lX = SDL_GetJoystickAxis(joy, i);
		else if (i == 1)
			XJoystickState.lY = SDL_GetJoystickAxis(joy, i);
	}

	CurrentStickSwitchCode = 0;
	if (JoystickStickSwitchButton &&
		XJoystickState.rgbButtons[JoystickStickSwitchButton - VK_BUTTON_1]) {
		int dx = XJoystickState.lX;
		if (abs(dx) < RANGE_MAX / 16)
			dx = 0;
		int dy = XJoystickState.lY;
		if (abs(dy) < RANGE_MAX / 16)
			dy = 0;

		if (dy < 0) {
			if (dx < 0)
				CurrentStickSwitchCode = VK_STICK_SWITCH_7;
			else if (dx > 0)
				CurrentStickSwitchCode = VK_STICK_SWITCH_9;
			else
				CurrentStickSwitchCode = VK_STICK_SWITCH_8;
		} else if (dy > 0) {
			if (dx < 0)
				CurrentStickSwitchCode = VK_STICK_SWITCH_1;
			else if (dx > 0)
				CurrentStickSwitchCode = VK_STICK_SWITCH_3;
			else
				CurrentStickSwitchCode = VK_STICK_SWITCH_2;
		} else if (dx < 0)
			CurrentStickSwitchCode = VK_STICK_SWITCH_4;
		else if (dx > 0)
			CurrentStickSwitchCode = VK_STICK_SWITCH_6;
		else
			CurrentStickSwitchCode = VK_STICK_SWITCH_5;
	}

	return 1;

} //*** end XJoystickInput()
