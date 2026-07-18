#include "gamepad_mapping.h"

#include <algorithm>
#include <array>
#include <cmath>

namespace vangers::settings {
namespace {

template<typename Code> struct NamedGamepadCode {
	Code code;
	std::string_view name;
};

template<typename Code> NamedGamepadCode(Code, std::string_view) -> NamedGamepadCode<Code>;

constexpr std::array BUTTONS = {
	NamedGamepadCode{SDL_GAMEPAD_BUTTON_SOUTH, std::string_view{"south"}},
	NamedGamepadCode{SDL_GAMEPAD_BUTTON_EAST, std::string_view{"east"}},
	NamedGamepadCode{SDL_GAMEPAD_BUTTON_WEST, std::string_view{"west"}},
	NamedGamepadCode{SDL_GAMEPAD_BUTTON_NORTH, std::string_view{"north"}},
	NamedGamepadCode{SDL_GAMEPAD_BUTTON_BACK, std::string_view{"back"}},
	NamedGamepadCode{SDL_GAMEPAD_BUTTON_GUIDE, std::string_view{"guide"}},
	NamedGamepadCode{SDL_GAMEPAD_BUTTON_START, std::string_view{"start"}},
	NamedGamepadCode{SDL_GAMEPAD_BUTTON_LEFT_STICK, std::string_view{"left_stick"}},
	NamedGamepadCode{SDL_GAMEPAD_BUTTON_RIGHT_STICK, std::string_view{"right_stick"}},
	NamedGamepadCode{SDL_GAMEPAD_BUTTON_LEFT_SHOULDER, std::string_view{"left_shoulder"}},
	NamedGamepadCode{SDL_GAMEPAD_BUTTON_RIGHT_SHOULDER, std::string_view{"right_shoulder"}},
	NamedGamepadCode{SDL_GAMEPAD_BUTTON_DPAD_UP, std::string_view{"dpad_up"}},
	NamedGamepadCode{SDL_GAMEPAD_BUTTON_DPAD_DOWN, std::string_view{"dpad_down"}},
	NamedGamepadCode{SDL_GAMEPAD_BUTTON_DPAD_LEFT, std::string_view{"dpad_left"}},
	NamedGamepadCode{SDL_GAMEPAD_BUTTON_DPAD_RIGHT, std::string_view{"dpad_right"}},
	NamedGamepadCode{SDL_GAMEPAD_BUTTON_MISC1, std::string_view{"misc_1"}},
	NamedGamepadCode{SDL_GAMEPAD_BUTTON_RIGHT_PADDLE1, std::string_view{"right_paddle_1"}},
	NamedGamepadCode{SDL_GAMEPAD_BUTTON_LEFT_PADDLE1, std::string_view{"left_paddle_1"}},
	NamedGamepadCode{SDL_GAMEPAD_BUTTON_RIGHT_PADDLE2, std::string_view{"right_paddle_2"}},
	NamedGamepadCode{SDL_GAMEPAD_BUTTON_LEFT_PADDLE2, std::string_view{"left_paddle_2"}},
	NamedGamepadCode{SDL_GAMEPAD_BUTTON_TOUCHPAD, std::string_view{"touchpad"}},
	NamedGamepadCode{SDL_GAMEPAD_BUTTON_MISC2, std::string_view{"misc_2"}},
	NamedGamepadCode{SDL_GAMEPAD_BUTTON_MISC3, std::string_view{"misc_3"}},
	NamedGamepadCode{SDL_GAMEPAD_BUTTON_MISC4, std::string_view{"misc_4"}},
	NamedGamepadCode{SDL_GAMEPAD_BUTTON_MISC5, std::string_view{"misc_5"}},
	NamedGamepadCode{SDL_GAMEPAD_BUTTON_MISC6, std::string_view{"misc_6"}},
};

constexpr std::array AXES = {
	NamedGamepadCode{SDL_GAMEPAD_AXIS_LEFTX, std::string_view{"left_x"}},
	NamedGamepadCode{SDL_GAMEPAD_AXIS_LEFTY, std::string_view{"left_y"}},
	NamedGamepadCode{SDL_GAMEPAD_AXIS_RIGHTX, std::string_view{"right_x"}},
	NamedGamepadCode{SDL_GAMEPAD_AXIS_RIGHTY, std::string_view{"right_y"}},
	NamedGamepadCode{SDL_GAMEPAD_AXIS_LEFT_TRIGGER, std::string_view{"left_trigger"}},
	NamedGamepadCode{SDL_GAMEPAD_AXIS_RIGHT_TRIGGER, std::string_view{"right_trigger"}},
};

template<typename Code, std::size_t Size>
std::optional<Code>
code_from_name(const std::array<NamedGamepadCode<Code>, Size> &entries, std::string_view name) {
	for (const auto &entry : entries) {
		if (entry.name == name)
			return entry.code;
	}
	return std::nullopt;
}

template<typename Code, std::size_t Size>
std::optional<std::string_view>
name_from_code(const std::array<NamedGamepadCode<Code>, Size> &entries, Code code) {
	for (const auto &entry : entries) {
		if (entry.code == code)
			return entry.name;
	}
	return std::nullopt;
}

float apply_deadzone(float value, float deadzone) {
	deadzone = std::clamp(deadzone, 0.0f, 0.95f);
	const float magnitude = std::abs(value);
	if (magnitude <= deadzone)
		return 0.0f;
	return std::copysign((magnitude - deadzone) / (1.0f - deadzone), value);
}

} // namespace

std::optional<SDL_GamepadButton> gamepad_button_from_name(std::string_view name) {
	return code_from_name(BUTTONS, name);
}

std::optional<std::string_view> gamepad_button_name(SDL_GamepadButton button) {
	return name_from_code(BUTTONS, button);
}

std::optional<SDL_GamepadAxis> gamepad_axis_from_name(std::string_view name) {
	return code_from_name(AXES, name);
}

std::optional<std::string_view> gamepad_axis_name(SDL_GamepadAxis axis) {
	return name_from_code(AXES, axis);
}

bool gamepad_axis_is_trigger(SDL_GamepadAxis axis) {
	return axis == SDL_GAMEPAD_AXIS_LEFT_TRIGGER || axis == SDL_GAMEPAD_AXIS_RIGHT_TRIGGER;
}

bool gamepad_trigger_pressed(float normalized_value) {
	return normalized_value >= 0.5f;
}

float normalize_gamepad_axis(
	Sint16 raw_value,
	SDL_GamepadAxis axis,
	float stick_deadzone,
	float trigger_deadzone,
	bool inverted
) {
	float value;
	if (gamepad_axis_is_trigger(axis)) {
		value = std::max(0.0f, static_cast<float>(raw_value) / 32767.0f);
		value = apply_deadzone(value, trigger_deadzone);
	} else {
		value = raw_value < 0 ? static_cast<float>(raw_value) / 32768.0f
							  : static_cast<float>(raw_value) / 32767.0f;
		value = apply_deadzone(value, stick_deadzone);
	}
	return inverted ? -value : value;
}

} // namespace vangers::settings
