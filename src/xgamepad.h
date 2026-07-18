#ifndef VANGERS_XGAMEPAD_H
#define VANGERS_XGAMEPAD_H

#include <string_view>

#include <SDL3/SDL.h>

bool XGamepadInit();
void XGamepadCleanup();
bool XGamepadIsAvailable();

bool XGamepadActionPressed(std::string_view action);
bool XGamepadButtonPressed(SDL_GamepadButton button);
bool XGamepadButtonMatchesAction(std::string_view action, SDL_GamepadButton button);
bool XGamepadOwnsEvent(const SDL_Event &event);
float XGamepadAxisValue(std::string_view logical_axis);

// The gamepad manager owns this SDL handle; callers must not close it.
SDL_Gamepad *XGamepadHandle();

#endif
