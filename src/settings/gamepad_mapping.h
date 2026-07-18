#ifndef VANGERS_SETTINGS_GAMEPAD_MAPPING_H
#define VANGERS_SETTINGS_GAMEPAD_MAPPING_H

#include <optional>
#include <string_view>

#include <SDL3/SDL.h>

namespace vangers::settings {

std::optional<SDL_GamepadButton> gamepad_button_from_name(std::string_view name);
std::optional<std::string_view> gamepad_button_name(SDL_GamepadButton button);

std::optional<SDL_GamepadAxis> gamepad_axis_from_name(std::string_view name);
std::optional<std::string_view> gamepad_axis_name(SDL_GamepadAxis axis);

bool gamepad_axis_is_trigger(SDL_GamepadAxis axis);
bool gamepad_trigger_pressed(float normalized_value);
float normalize_gamepad_axis(
	Sint16 raw_value,
	SDL_GamepadAxis axis,
	float stick_deadzone,
	float trigger_deadzone,
	bool inverted
);

} // namespace vangers::settings

#endif
