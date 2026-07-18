#include "input_binding.h"

#include <array>
#include <charconv>
#include <cstdint>
#include <string>

#include <SDL3/SDL.h>

namespace vangers::settings {
namespace {

constexpr int LEGACY_JOYSTICK_BUTTON_MASK = 1 << 28;
constexpr int LEGACY_GAMEPAD_BUTTON_MASK = 1 << 27;
constexpr int LEGACY_JOYSTICK_HAT_MASK = 1 << 26;
constexpr std::string_view SCANCODE_PREFIX = "scancode:";

struct NamedCode {
	int code;
	const char *name;
};

// Control IDs are persisted by the original controls.dat format. ID 31 is the
// removed joystick-mode switch and deliberately has no settings action.
constexpr std::array<const char *, 38> CONTROL_ACTIONS = {
	nullptr,
	"turn_left",
	"turn_right",
	"move_forward",
	"move_backward",
	"roll_left",
	"roll_right",
	"device_on",
	"device_off",
	"activate_kid",
	"acceleration",
	"vertical_thrust",
	"inventory",
	"open",
	"fire_weapon_1",
	"fire_weapon_2",
	"fire_weapon_3",
	"fire_weapon_4",
	"fire_all",
	"change_target",
	"fullscreen",
	"reduce_view",
	"enlarge_view",
	"camera_rotation",
	"auto_zoom",
	"auto_tilt",
	"zoom_in",
	"zoom_out",
	"zoom_standard",
	"handbrake",
	"use_gluek",
	nullptr,
	"skip_text",
	"use_vector",
	"frag_info",
	"chat",
	"exit",
	"screenshot",
};

// Only names used by the default controls need dedicated aliases. Letters,
// digits, and function keys are handled by their contiguous USB usage ranges;
// every other SDL scancode has a lossless "scancode:<number>" representation.
constexpr NamedCode COMMON_KEYBOARD_CODES[] = {
	{SDL_SCANCODE_RETURN, "enter"},
	{SDL_SCANCODE_TAB, "tab"},
	{SDL_SCANCODE_SPACE, "space"},
	{SDL_SCANCODE_MINUS, "minus"},
	{SDL_SCANCODE_EQUALS, "equals"},
	{SDL_SCANCODE_PAGEUP, "page_up"},
	{SDL_SCANCODE_PAGEDOWN, "page_down"},
	{SDL_SCANCODE_RIGHT, "right"},
	{SDL_SCANCODE_LEFT, "left"},
	{SDL_SCANCODE_DOWN, "down"},
	{SDL_SCANCODE_UP, "up"},
	{SDL_SCANCODE_LCTRL, "left_ctrl"},
	{SDL_SCANCODE_LSHIFT, "left_shift"},
	{SDL_SCANCODE_RSHIFT, "right_shift"},
};

constexpr NamedCode GAMEPAD_CODES[] = {
	{SDL_GAMEPAD_BUTTON_SOUTH, "south"},
	{SDL_GAMEPAD_BUTTON_EAST, "east"},
	{SDL_GAMEPAD_BUTTON_WEST, "west"},
	{SDL_GAMEPAD_BUTTON_NORTH, "north"},
	{SDL_GAMEPAD_BUTTON_BACK, "back"},
	{SDL_GAMEPAD_BUTTON_GUIDE, "guide"},
	{SDL_GAMEPAD_BUTTON_START, "start"},
	{SDL_GAMEPAD_BUTTON_LEFT_STICK, "left_stick"},
	{SDL_GAMEPAD_BUTTON_RIGHT_STICK, "right_stick"},
	{SDL_GAMEPAD_BUTTON_LEFT_SHOULDER, "left_shoulder"},
	{SDL_GAMEPAD_BUTTON_RIGHT_SHOULDER, "right_shoulder"},
	{SDL_GAMEPAD_BUTTON_DPAD_UP, "dpad_up"},
	{SDL_GAMEPAD_BUTTON_DPAD_DOWN, "dpad_down"},
	{SDL_GAMEPAD_BUTTON_DPAD_LEFT, "dpad_left"},
	{SDL_GAMEPAD_BUTTON_DPAD_RIGHT, "dpad_right"},
	{SDL_GAMEPAD_BUTTON_MISC1, "misc_1"},
	{SDL_GAMEPAD_BUTTON_RIGHT_PADDLE1, "right_paddle_1"},
	{SDL_GAMEPAD_BUTTON_LEFT_PADDLE1, "left_paddle_1"},
	{SDL_GAMEPAD_BUTTON_RIGHT_PADDLE2, "right_paddle_2"},
	{SDL_GAMEPAD_BUTTON_LEFT_PADDLE2, "left_paddle_2"},
	{SDL_GAMEPAD_BUTTON_TOUCHPAD, "touchpad"},
	{SDL_GAMEPAD_BUTTON_MISC2, "misc_2"},
	{SDL_GAMEPAD_BUTTON_MISC3, "misc_3"},
	{SDL_GAMEPAD_BUTTON_MISC4, "misc_4"},
	{SDL_GAMEPAD_BUTTON_MISC5, "misc_5"},
	{SDL_GAMEPAD_BUTTON_MISC6, "misc_6"},
};

static_assert(SDL_SCANCODE_Z - SDL_SCANCODE_A == 25);
static_assert(SDL_SCANCODE_9 - SDL_SCANCODE_1 == 8);
static_assert(SDL_SCANCODE_F12 - SDL_SCANCODE_F1 == 11);
static_assert(SDL_SCANCODE_F24 - SDL_SCANCODE_F13 == 11);

template<std::size_t Size>
const NamedCode *find_by_code(const NamedCode (&entries)[Size], int code) {
	for (const NamedCode &entry : entries) {
		if (entry.code == code)
			return &entry;
	}
	return nullptr;
}

template<std::size_t Size>
const NamedCode *find_by_name(const NamedCode (&entries)[Size], std::string_view name) {
	for (const NamedCode &entry : entries) {
		if (entry.name == name)
			return &entry;
	}
	return nullptr;
}

std::optional<int> parse_decimal(std::string_view text) {
	if (text.empty())
		return std::nullopt;
	int value = 0;
	const auto result = std::from_chars(text.data(), text.data() + text.size(), value);
	if (result.ec != std::errc{} || result.ptr != text.data() + text.size())
		return std::nullopt;
	return value;
}

bool valid_scancode(int scancode) {
	return scancode > SDL_SCANCODE_UNKNOWN && scancode < SDL_SCANCODE_COUNT;
}

std::optional<std::string> keyboard_binding_name(int scancode) {
	if (!valid_scancode(scancode))
		return std::nullopt;

	if (scancode >= SDL_SCANCODE_A && scancode <= SDL_SCANCODE_Z) {
		return std::string(1, static_cast<char>('a' + static_cast<int>(scancode - SDL_SCANCODE_A)));
	}
	if (scancode >= SDL_SCANCODE_1 && scancode <= SDL_SCANCODE_9) {
		return std::string(1, static_cast<char>('1' + static_cast<int>(scancode - SDL_SCANCODE_1)));
	}
	if (scancode == SDL_SCANCODE_0)
		return std::string("0");
	if (scancode >= SDL_SCANCODE_F1 && scancode <= SDL_SCANCODE_F12)
		return "f" + std::to_string(scancode - SDL_SCANCODE_F1 + 1);
	if (scancode >= SDL_SCANCODE_F13 && scancode <= SDL_SCANCODE_F24)
		return "f" + std::to_string(scancode - SDL_SCANCODE_F13 + 13);
	if (const NamedCode *entry = find_by_code(COMMON_KEYBOARD_CODES, scancode))
		return std::string(entry->name);
	return std::string(SCANCODE_PREFIX) + std::to_string(scancode);
}

} // namespace

const char *legacy_control_action_name(int control_id) {
	if (control_id < 0 || static_cast<std::size_t>(control_id) >= CONTROL_ACTIONS.size())
		return nullptr;
	return CONTROL_ACTIONS[control_id];
}

std::optional<int> legacy_control_action_id(std::string_view action) {
	for (std::size_t id = 0; id < CONTROL_ACTIONS.size(); ++id) {
		if (CONTROL_ACTIONS[id] && CONTROL_ACTIONS[id] == action)
			return static_cast<int>(id);
	}
	return std::nullopt;
}

DecodedLegacyBinding decode_legacy_control_code(int code) {
	if (code == 0)
		return {LegacyBindingKind::Unassigned, {}};
	if ((code & LEGACY_JOYSTICK_BUTTON_MASK) != 0 || (code & LEGACY_JOYSTICK_HAT_MASK) != 0)
		return {LegacyBindingKind::Unsupported, {}};
	if ((code & LEGACY_GAMEPAD_BUTTON_MASK) != 0) {
		const int button = code ^ LEGACY_GAMEPAD_BUTTON_MASK;
		if (const NamedCode *entry = find_by_code(GAMEPAD_CODES, button))
			return {LegacyBindingKind::Gamepad, entry->name};
		return {LegacyBindingKind::Unsupported, {}};
	}

	int scancode = code;
	if ((static_cast<std::uint32_t>(code) & SDLK_SCANCODE_MASK) != 0)
		scancode = static_cast<int>(static_cast<std::uint32_t>(code) ^ SDLK_SCANCODE_MASK);
	if (const auto name = keyboard_binding_name(scancode))
		return {LegacyBindingKind::Keyboard, *name};
	return {LegacyBindingKind::Unsupported, {}};
}

std::optional<int> keyboard_binding_code(std::string_view name) {
	if (name.size() == 1 && name.front() >= 'a' && name.front() <= 'z')
		return SDL_SCANCODE_A + (name.front() - 'a');
	if (name.size() == 1 && name.front() >= '1' && name.front() <= '9')
		return SDL_SCANCODE_1 + (name.front() - '1');
	if (name == "0")
		return SDL_SCANCODE_0;
	if (name.size() > 1 && name.front() == 'f') {
		if (const auto number = parse_decimal(name.substr(1))) {
			if (*number >= 1 && *number <= 12)
				return SDL_SCANCODE_F1 + *number - 1;
			if (*number >= 13 && *number <= 24)
				return SDL_SCANCODE_F13 + *number - 13;
		}
	}
	if (const NamedCode *entry = find_by_name(COMMON_KEYBOARD_CODES, name))
		return entry->code;
	if (name.substr(0, SCANCODE_PREFIX.size()) == SCANCODE_PREFIX) {
		if (const auto scancode = parse_decimal(name.substr(SCANCODE_PREFIX.size()));
			valid_scancode(scancode.value_or(SDL_SCANCODE_UNKNOWN)))
			return *scancode;
	}
	return std::nullopt;
}

std::optional<int> gamepad_binding_code(std::string_view name) {
	if (const NamedCode *entry = find_by_name(GAMEPAD_CODES, name))
		return entry->code | LEGACY_GAMEPAD_BUTTON_MASK;
	return std::nullopt;
}

} // namespace vangers::settings
