#ifndef VANGERS_XGAMEPAD_H
#define VANGERS_XGAMEPAD_H

#include <string_view>

#include <SDL3/SDL.h>

constexpr float XGAMEPAD_DIGITAL_AXIS_THRESHOLD = 0.5f;

bool XGamepadInit();
void XGamepadCleanup();
bool XGamepadIsAvailable();

bool XGamepadActionPressed(std::string_view action);
bool XGamepadButtonPressed(SDL_GamepadButton button);
bool XGamepadButtonMatchesAction(std::string_view action, SDL_GamepadButton button);
bool XGamepadOwnsEvent(const SDL_Event &event);
bool XGamepadGeneratedKeyEvent(const SDL_Event &event);
float XGamepadAxisValue(std::string_view logical_axis);
bool XGamepadIsControllingCursor();
void XGamepadUseFocusNavigation();
bool XGamepadHasManualDrivingInput();
void XGamepadRumble(float low_frequency, float high_frequency, Uint32 duration_ms);

// The gamepad manager owns this SDL handle; callers must not close it.
SDL_Gamepad *XGamepadHandle();

#endif
