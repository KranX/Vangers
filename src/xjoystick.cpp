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
#include <time.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include "xglobal.h"
#include "xjoystick.h"

SDL_GameController *ctrl = NULL;
SDL_Joystick *joy = NULL;

int JoystickMode = JOYSTICK_None;
int JoystickStickSwitchButton = VK_BUTTON_2;
int CurrentStickSwitchCode = 0;
unsigned int next_joystick_input = 0;

int JoystickAvailable = 0;
char* XJoystickLastErrorString = 0;
int XJoystickLastErrorCode = 0;
static int XJoystickErrHUsed = 1;
#define XJOYSTICK_ABORT(str,code) { if(XJoystickErrHUsed) ErrH.Abort(str,XERR_USER,code); else{ XJoystickLastErrorString = (char *)str; XJoystickLastErrorCode = code; }}

// Global State of Joystick
XJOYSTATE  XJoystickState;

int JoystickWhatsPressedNow()
{
	std::cout<<"JoystickWhatsPressedNow "<<std::endl;
	if(!JoystickAvailable)
		return 0;
	if(SDL_GetTicks() > next_joystick_input){
		XJoystickInput();
		next_joystick_input = SDL_GetTicks() + 20;
		}
	if(CurrentStickSwitchCode)
		return CurrentStickSwitchCode;
	for(int i = 0;i < 32;i++)
		if(XJoystickState.rgbButtons[i])
			return VK_BUTTON_1 + i;
	return 0;
}

int isJoystickButtonPressed(int vk_code)
{
	std::cout<<"isJoystickButtonPressed "<<vk_code<<std::endl;
	if(!JoystickAvailable)
		return 0;
	if(SDL_GetTicks() > next_joystick_input){
		XJoystickInput();
		next_joystick_input = SDL_GetTicks() + 20;
		}
	if(vk_code & VK_BUTTON)
		return XJoystickState.rgbButtons[vk_code - VK_BUTTON_1] ? 1 : 0;
	if(vk_code & VK_STICK_SWITCH)
		return vk_code == CurrentStickSwitchCode ? 1 : 0;
	return 0;
}

//===========================================================================
// inputPrepareDevice
//
// Performs device preparation by setting the device's parameters (ie
// deadzone).
//
// Parameters:
//
// Returns:
//
//===========================================================================
static bool inputPrepareDevice(void)
{
    return true;
} //** end inputPrepareDevice()


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
	int i;
	
	SDL_InitSubSystem(SDL_INIT_JOYSTICK);
	SDL_InitSubSystem(SDL_INIT_GAMECONTROLLER);
	SDL_JoystickEventState(SDL_ENABLE);
	SDL_GameControllerEventState(SDL_ENABLE);
	
	if(!SDL_WasInit(SDL_INIT_JOYSTICK) && SDL_InitSubSystem(SDL_INIT_JOYSTICK)) {
		std::cout<<"Unable to initialize the joystick subsystem"<<std::endl;
		return false;
	}
	
	for(i = 0; i < SDL_NumJoysticks(); ++i) {
		if (SDL_IsGameController(i)) {
			printf("Index \'%i\' is a compatible controller, named \'%s\'\n", i, SDL_GameControllerNameForIndex(i));
			ctrl = SDL_GameControllerOpen(i);
			joy = SDL_GameControllerGetJoystick(ctrl);
			break;
		} else {
			printf("Index \'%s\' is not a compatible controller.\n", SDL_JoystickNameForIndex(i));
		}
	}
	if (ctrl) {
		JoystickAvailable = 1;
		return true;
	}
	for(i = 0; i < SDL_NumJoysticks(); ++i) {
		joy = SDL_JoystickOpen(i);
		if (joy) {
			printf("Index \'%i\' is a compatible joystick, named \'%s\'\n", i, SDL_JoystickNameForIndex(i));
			break;
		}
	}
	if (joy) {
		JoystickAvailable = 1;
		return true;
	} else {
		return false;
	}
}

SDL_GameController *get_gamecontroller() {
	return ctrl;
}

SDL_Joystick *get_joystick() {
	return joy;
}

bool XJoystickInit_old(int ErrHUsed)
{
    JoystickAvailable = 0;
    XJoystickErrHUsed = ErrHUsed;

    SDL_InitSubSystem(SDL_INIT_JOYSTICK);
	//SDL_JoystickEventState(SDL_ENABLE);
    //SDL_JoystickEventState(SDL_IGNORE); // we will poll ourselves

    int nJoysticks = SDL_NumJoysticks();

    // FIXME: pick first
    if (nJoysticks < 1)
	return false;

	std::cout<<"Found "<<nJoysticks<<" joysticks"<<std::endl;
    joy = SDL_JoystickOpen(0);
    if (!joy)
		return false;

	std::cout<<"Init joystick: "<<SDL_JoystickNameForIndex(0)<<std::endl;
    // set joystick parameters (deadzone, etc)
    if(!inputPrepareDevice())
    {
        XJOYSTICK_ABORT("Device preparation failed\nXJoystick - Force Feedback", -1);
        return false;
    }

    // if we get here, we succeeded
    return true;
/* */
} //*** end XJoystickInit()


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
void XJoystickCleanup(void)
{
    //if (SDL_JoystickOpened(0) && joystick)
	//SDL_JoystickClose(joystick);
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
int XJoystickInput()
{
	std::cout<<"XJoystickInput"<<std::endl;
	if(!JoystickAvailable)
		return 0;

	SDL_JoystickUpdate(); // update all open joysticks

	for (int i = 0; i < SDL_JoystickNumButtons(joy) && i < 32; i++)
	    XJoystickState.rgbButtons[i] = SDL_JoystickGetButton(joy, i);

	for (int i = 0; i < SDL_JoystickNumAxes(joy) && i < 2; i++)
	{
	    if (i == 0)
		XJoystickState.lX = SDL_JoystickGetAxis(joy, i);
	    else if (i == 1)
		XJoystickState.lY = SDL_JoystickGetAxis(joy, i);
	}

	CurrentStickSwitchCode = 0;
	if(JoystickStickSwitchButton && XJoystickState.rgbButtons[JoystickStickSwitchButton - VK_BUTTON_1])
	{
		int dx = XJoystickState.lX;
		if(abs(dx) < RANGE_MAX/16)
			dx = 0;
		int dy = XJoystickState.lY;
		if(abs(dy) < RANGE_MAX/16)
			dy = 0;

		if(dy < 0){
			if(dx < 0)
				CurrentStickSwitchCode = VK_STICK_SWITCH_7;
			else
				if(dx > 0)
					CurrentStickSwitchCode = VK_STICK_SWITCH_9;
				else
					CurrentStickSwitchCode = VK_STICK_SWITCH_8;
			}
		else
			if(dy > 0){
				if(dx < 0)
					CurrentStickSwitchCode = VK_STICK_SWITCH_1;
				else
					if(dx > 0)
						CurrentStickSwitchCode = VK_STICK_SWITCH_3;
					else
						CurrentStickSwitchCode = VK_STICK_SWITCH_2;
				}
			else
				if(dx < 0)
					CurrentStickSwitchCode = VK_STICK_SWITCH_4;
				else
					if(dx > 0)
						CurrentStickSwitchCode = VK_STICK_SWITCH_6;
					else
						CurrentStickSwitchCode = VK_STICK_SWITCH_5;
		}

	return 1;

} //*** end XJoystickInput()

