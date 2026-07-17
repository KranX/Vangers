#include "input_binding.h"

#include <array>
#include <cstdint>

#include <SDL3/SDL.h>

namespace vangers::settings {
namespace {

constexpr int LEGACY_JOYSTICK_BUTTON_MASK = 1 << 28;
constexpr int LEGACY_GAMEPAD_BUTTON_MASK = 1 << 27;
constexpr int LEGACY_JOYSTICK_HAT_MASK = 1 << 26;

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

// This table is an application-owned, non-localized persistence vocabulary.
// Never replace it with SDL_GetScancodeName(), whose text is display-oriented
// and therefore not a stable file format.
// clang-format off
constexpr NamedCode KEYBOARD_CODES[] = {
	{SDL_SCANCODE_A, "a"}, {SDL_SCANCODE_B, "b"}, {SDL_SCANCODE_C, "c"},
	{SDL_SCANCODE_D, "d"}, {SDL_SCANCODE_E, "e"}, {SDL_SCANCODE_F, "f"},
	{SDL_SCANCODE_G, "g"}, {SDL_SCANCODE_H, "h"}, {SDL_SCANCODE_I, "i"},
	{SDL_SCANCODE_J, "j"}, {SDL_SCANCODE_K, "k"}, {SDL_SCANCODE_L, "l"},
	{SDL_SCANCODE_M, "m"}, {SDL_SCANCODE_N, "n"}, {SDL_SCANCODE_O, "o"},
	{SDL_SCANCODE_P, "p"}, {SDL_SCANCODE_Q, "q"}, {SDL_SCANCODE_R, "r"},
	{SDL_SCANCODE_S, "s"}, {SDL_SCANCODE_T, "t"}, {SDL_SCANCODE_U, "u"},
	{SDL_SCANCODE_V, "v"}, {SDL_SCANCODE_W, "w"}, {SDL_SCANCODE_X, "x"},
	{SDL_SCANCODE_Y, "y"}, {SDL_SCANCODE_Z, "z"},
	{SDL_SCANCODE_1, "1"}, {SDL_SCANCODE_2, "2"}, {SDL_SCANCODE_3, "3"},
	{SDL_SCANCODE_4, "4"}, {SDL_SCANCODE_5, "5"}, {SDL_SCANCODE_6, "6"},
	{SDL_SCANCODE_7, "7"}, {SDL_SCANCODE_8, "8"}, {SDL_SCANCODE_9, "9"},
	{SDL_SCANCODE_0, "0"},
	{SDL_SCANCODE_RETURN, "enter"}, {SDL_SCANCODE_ESCAPE, "escape"},
	{SDL_SCANCODE_BACKSPACE, "backspace"}, {SDL_SCANCODE_TAB, "tab"},
	{SDL_SCANCODE_SPACE, "space"}, {SDL_SCANCODE_MINUS, "minus"},
	{SDL_SCANCODE_EQUALS, "equals"}, {SDL_SCANCODE_LEFTBRACKET, "left_bracket"},
	{SDL_SCANCODE_RIGHTBRACKET, "right_bracket"}, {SDL_SCANCODE_BACKSLASH, "backslash"},
	{SDL_SCANCODE_NONUSHASH, "non_us_hash"}, {SDL_SCANCODE_SEMICOLON, "semicolon"},
	{SDL_SCANCODE_APOSTROPHE, "apostrophe"}, {SDL_SCANCODE_GRAVE, "grave"},
	{SDL_SCANCODE_COMMA, "comma"}, {SDL_SCANCODE_PERIOD, "period"},
	{SDL_SCANCODE_SLASH, "slash"}, {SDL_SCANCODE_CAPSLOCK, "caps_lock"},
	{SDL_SCANCODE_F1, "f1"}, {SDL_SCANCODE_F2, "f2"}, {SDL_SCANCODE_F3, "f3"},
	{SDL_SCANCODE_F4, "f4"}, {SDL_SCANCODE_F5, "f5"}, {SDL_SCANCODE_F6, "f6"},
	{SDL_SCANCODE_F7, "f7"}, {SDL_SCANCODE_F8, "f8"}, {SDL_SCANCODE_F9, "f9"},
	{SDL_SCANCODE_F10, "f10"}, {SDL_SCANCODE_F11, "f11"}, {SDL_SCANCODE_F12, "f12"},
	{SDL_SCANCODE_PRINTSCREEN, "print_screen"}, {SDL_SCANCODE_SCROLLLOCK, "scroll_lock"},
	{SDL_SCANCODE_PAUSE, "pause"}, {SDL_SCANCODE_INSERT, "insert"},
	{SDL_SCANCODE_HOME, "home"}, {SDL_SCANCODE_PAGEUP, "page_up"},
	{SDL_SCANCODE_DELETE, "delete"}, {SDL_SCANCODE_END, "end"},
	{SDL_SCANCODE_PAGEDOWN, "page_down"}, {SDL_SCANCODE_RIGHT, "right"},
	{SDL_SCANCODE_LEFT, "left"}, {SDL_SCANCODE_DOWN, "down"}, {SDL_SCANCODE_UP, "up"},
	{SDL_SCANCODE_NUMLOCKCLEAR, "num_lock"}, {SDL_SCANCODE_KP_DIVIDE, "kp_divide"},
	{SDL_SCANCODE_KP_MULTIPLY, "kp_multiply"}, {SDL_SCANCODE_KP_MINUS, "kp_minus"},
	{SDL_SCANCODE_KP_PLUS, "kp_plus"}, {SDL_SCANCODE_KP_ENTER, "kp_enter"},
	{SDL_SCANCODE_KP_1, "kp_1"}, {SDL_SCANCODE_KP_2, "kp_2"},
	{SDL_SCANCODE_KP_3, "kp_3"}, {SDL_SCANCODE_KP_4, "kp_4"},
	{SDL_SCANCODE_KP_5, "kp_5"}, {SDL_SCANCODE_KP_6, "kp_6"},
	{SDL_SCANCODE_KP_7, "kp_7"}, {SDL_SCANCODE_KP_8, "kp_8"},
	{SDL_SCANCODE_KP_9, "kp_9"}, {SDL_SCANCODE_KP_0, "kp_0"},
	{SDL_SCANCODE_KP_PERIOD, "kp_period"}, {SDL_SCANCODE_NONUSBACKSLASH, "non_us_backslash"},
	{SDL_SCANCODE_APPLICATION, "application"}, {SDL_SCANCODE_POWER, "power"},
	{SDL_SCANCODE_KP_EQUALS, "kp_equals"},
	{SDL_SCANCODE_F13, "f13"}, {SDL_SCANCODE_F14, "f14"}, {SDL_SCANCODE_F15, "f15"},
	{SDL_SCANCODE_F16, "f16"}, {SDL_SCANCODE_F17, "f17"}, {SDL_SCANCODE_F18, "f18"},
	{SDL_SCANCODE_F19, "f19"}, {SDL_SCANCODE_F20, "f20"}, {SDL_SCANCODE_F21, "f21"},
	{SDL_SCANCODE_F22, "f22"}, {SDL_SCANCODE_F23, "f23"}, {SDL_SCANCODE_F24, "f24"},
	{SDL_SCANCODE_EXECUTE, "execute"}, {SDL_SCANCODE_HELP, "help"},
	{SDL_SCANCODE_MENU, "menu"}, {SDL_SCANCODE_SELECT, "select"},
	{SDL_SCANCODE_STOP, "stop"}, {SDL_SCANCODE_AGAIN, "again"},
	{SDL_SCANCODE_UNDO, "undo"}, {SDL_SCANCODE_CUT, "cut"},
	{SDL_SCANCODE_COPY, "copy"}, {SDL_SCANCODE_PASTE, "paste"},
	{SDL_SCANCODE_FIND, "find"}, {SDL_SCANCODE_MUTE, "mute"},
	{SDL_SCANCODE_VOLUMEUP, "volume_up"}, {SDL_SCANCODE_VOLUMEDOWN, "volume_down"},
	{SDL_SCANCODE_KP_COMMA, "kp_comma"}, {SDL_SCANCODE_KP_EQUALSAS400, "kp_equals_as400"},
	{SDL_SCANCODE_INTERNATIONAL1, "international_1"},
	{SDL_SCANCODE_INTERNATIONAL2, "international_2"},
	{SDL_SCANCODE_INTERNATIONAL3, "international_3"},
	{SDL_SCANCODE_INTERNATIONAL4, "international_4"},
	{SDL_SCANCODE_INTERNATIONAL5, "international_5"},
	{SDL_SCANCODE_INTERNATIONAL6, "international_6"},
	{SDL_SCANCODE_INTERNATIONAL7, "international_7"},
	{SDL_SCANCODE_INTERNATIONAL8, "international_8"},
	{SDL_SCANCODE_INTERNATIONAL9, "international_9"},
	{SDL_SCANCODE_LANG1, "lang_1"}, {SDL_SCANCODE_LANG2, "lang_2"},
	{SDL_SCANCODE_LANG3, "lang_3"}, {SDL_SCANCODE_LANG4, "lang_4"},
	{SDL_SCANCODE_LANG5, "lang_5"}, {SDL_SCANCODE_LANG6, "lang_6"},
	{SDL_SCANCODE_LANG7, "lang_7"}, {SDL_SCANCODE_LANG8, "lang_8"},
	{SDL_SCANCODE_LANG9, "lang_9"}, {SDL_SCANCODE_ALTERASE, "alternate_erase"},
	{SDL_SCANCODE_SYSREQ, "sys_req"}, {SDL_SCANCODE_CANCEL, "cancel"},
	{SDL_SCANCODE_CLEAR, "clear"}, {SDL_SCANCODE_PRIOR, "prior"},
	{SDL_SCANCODE_RETURN2, "return_2"}, {SDL_SCANCODE_SEPARATOR, "separator"},
	{SDL_SCANCODE_OUT, "out"}, {SDL_SCANCODE_OPER, "oper"},
	{SDL_SCANCODE_CLEARAGAIN, "clear_again"}, {SDL_SCANCODE_CRSEL, "cr_sel"},
	{SDL_SCANCODE_EXSEL, "ex_sel"}, {SDL_SCANCODE_KP_00, "kp_00"},
	{SDL_SCANCODE_KP_000, "kp_000"}, {SDL_SCANCODE_THOUSANDSSEPARATOR, "thousands_separator"},
	{SDL_SCANCODE_DECIMALSEPARATOR, "decimal_separator"},
	{SDL_SCANCODE_CURRENCYUNIT, "currency_unit"},
	{SDL_SCANCODE_CURRENCYSUBUNIT, "currency_subunit"},
	{SDL_SCANCODE_KP_LEFTPAREN, "kp_left_parenthesis"},
	{SDL_SCANCODE_KP_RIGHTPAREN, "kp_right_parenthesis"},
	{SDL_SCANCODE_KP_LEFTBRACE, "kp_left_brace"},
	{SDL_SCANCODE_KP_RIGHTBRACE, "kp_right_brace"},
	{SDL_SCANCODE_KP_TAB, "kp_tab"}, {SDL_SCANCODE_KP_BACKSPACE, "kp_backspace"},
	{SDL_SCANCODE_KP_A, "kp_a"}, {SDL_SCANCODE_KP_B, "kp_b"},
	{SDL_SCANCODE_KP_C, "kp_c"}, {SDL_SCANCODE_KP_D, "kp_d"},
	{SDL_SCANCODE_KP_E, "kp_e"}, {SDL_SCANCODE_KP_F, "kp_f"},
	{SDL_SCANCODE_KP_XOR, "kp_xor"}, {SDL_SCANCODE_KP_POWER, "kp_power"},
	{SDL_SCANCODE_KP_PERCENT, "kp_percent"}, {SDL_SCANCODE_KP_LESS, "kp_less"},
	{SDL_SCANCODE_KP_GREATER, "kp_greater"},
	{SDL_SCANCODE_KP_AMPERSAND, "kp_ampersand"},
	{SDL_SCANCODE_KP_DBLAMPERSAND, "kp_double_ampersand"},
	{SDL_SCANCODE_KP_VERTICALBAR, "kp_vertical_bar"},
	{SDL_SCANCODE_KP_DBLVERTICALBAR, "kp_double_vertical_bar"},
	{SDL_SCANCODE_KP_COLON, "kp_colon"}, {SDL_SCANCODE_KP_HASH, "kp_hash"},
	{SDL_SCANCODE_KP_SPACE, "kp_space"}, {SDL_SCANCODE_KP_AT, "kp_at"},
	{SDL_SCANCODE_KP_EXCLAM, "kp_exclamation"},
	{SDL_SCANCODE_KP_MEMSTORE, "kp_memory_store"},
	{SDL_SCANCODE_KP_MEMRECALL, "kp_memory_recall"},
	{SDL_SCANCODE_KP_MEMCLEAR, "kp_memory_clear"},
	{SDL_SCANCODE_KP_MEMADD, "kp_memory_add"},
	{SDL_SCANCODE_KP_MEMSUBTRACT, "kp_memory_subtract"},
	{SDL_SCANCODE_KP_MEMMULTIPLY, "kp_memory_multiply"},
	{SDL_SCANCODE_KP_MEMDIVIDE, "kp_memory_divide"},
	{SDL_SCANCODE_KP_PLUSMINUS, "kp_plus_minus"}, {SDL_SCANCODE_KP_CLEAR, "kp_clear"},
	{SDL_SCANCODE_KP_CLEARENTRY, "kp_clear_entry"},
	{SDL_SCANCODE_KP_BINARY, "kp_binary"}, {SDL_SCANCODE_KP_OCTAL, "kp_octal"},
	{SDL_SCANCODE_KP_DECIMAL, "kp_decimal"},
	{SDL_SCANCODE_KP_HEXADECIMAL, "kp_hexadecimal"},
	{SDL_SCANCODE_LCTRL, "left_ctrl"}, {SDL_SCANCODE_LSHIFT, "left_shift"},
	{SDL_SCANCODE_LALT, "left_alt"}, {SDL_SCANCODE_LGUI, "left_gui"},
	{SDL_SCANCODE_RCTRL, "right_ctrl"}, {SDL_SCANCODE_RSHIFT, "right_shift"},
	{SDL_SCANCODE_RALT, "right_alt"}, {SDL_SCANCODE_RGUI, "right_gui"},
	{SDL_SCANCODE_MODE, "mode"}, {SDL_SCANCODE_SLEEP, "sleep"},
	{SDL_SCANCODE_WAKE, "wake"}, {SDL_SCANCODE_CHANNEL_INCREMENT, "channel_up"},
	{SDL_SCANCODE_CHANNEL_DECREMENT, "channel_down"},
	{SDL_SCANCODE_MEDIA_PLAY, "media_play"}, {SDL_SCANCODE_MEDIA_PAUSE, "media_pause"},
	{SDL_SCANCODE_MEDIA_RECORD, "media_record"},
	{SDL_SCANCODE_MEDIA_FAST_FORWARD, "media_fast_forward"},
	{SDL_SCANCODE_MEDIA_REWIND, "media_rewind"},
	{SDL_SCANCODE_MEDIA_NEXT_TRACK, "media_next_track"},
	{SDL_SCANCODE_MEDIA_PREVIOUS_TRACK, "media_previous_track"},
	{SDL_SCANCODE_MEDIA_STOP, "media_stop"}, {SDL_SCANCODE_MEDIA_EJECT, "media_eject"},
	{SDL_SCANCODE_MEDIA_PLAY_PAUSE, "media_play_pause"},
	{SDL_SCANCODE_MEDIA_SELECT, "media_select"}, {SDL_SCANCODE_AC_NEW, "ac_new"},
	{SDL_SCANCODE_AC_OPEN, "ac_open"}, {SDL_SCANCODE_AC_CLOSE, "ac_close"},
	{SDL_SCANCODE_AC_EXIT, "ac_exit"}, {SDL_SCANCODE_AC_SAVE, "ac_save"},
	{SDL_SCANCODE_AC_PRINT, "ac_print"}, {SDL_SCANCODE_AC_PROPERTIES, "ac_properties"},
	{SDL_SCANCODE_AC_SEARCH, "ac_search"}, {SDL_SCANCODE_AC_HOME, "ac_home"},
	{SDL_SCANCODE_AC_BACK, "ac_back"}, {SDL_SCANCODE_AC_FORWARD, "ac_forward"},
	{SDL_SCANCODE_AC_STOP, "ac_stop"}, {SDL_SCANCODE_AC_REFRESH, "ac_refresh"},
	{SDL_SCANCODE_AC_BOOKMARKS, "ac_bookmarks"}, {SDL_SCANCODE_SOFTLEFT, "soft_left"},
	{SDL_SCANCODE_SOFTRIGHT, "soft_right"}, {SDL_SCANCODE_CALL, "call"},
	{SDL_SCANCODE_ENDCALL, "end_call"},
};

constexpr NamedCode GAMEPAD_CODES[] = {
	{SDL_GAMEPAD_BUTTON_SOUTH, "south"}, {SDL_GAMEPAD_BUTTON_EAST, "east"},
	{SDL_GAMEPAD_BUTTON_WEST, "west"}, {SDL_GAMEPAD_BUTTON_NORTH, "north"},
	{SDL_GAMEPAD_BUTTON_BACK, "back"}, {SDL_GAMEPAD_BUTTON_GUIDE, "guide"},
	{SDL_GAMEPAD_BUTTON_START, "start"}, {SDL_GAMEPAD_BUTTON_LEFT_STICK, "left_stick"},
	{SDL_GAMEPAD_BUTTON_RIGHT_STICK, "right_stick"},
	{SDL_GAMEPAD_BUTTON_LEFT_SHOULDER, "left_shoulder"},
	{SDL_GAMEPAD_BUTTON_RIGHT_SHOULDER, "right_shoulder"},
	{SDL_GAMEPAD_BUTTON_DPAD_UP, "dpad_up"}, {SDL_GAMEPAD_BUTTON_DPAD_DOWN, "dpad_down"},
	{SDL_GAMEPAD_BUTTON_DPAD_LEFT, "dpad_left"},
	{SDL_GAMEPAD_BUTTON_DPAD_RIGHT, "dpad_right"}, {SDL_GAMEPAD_BUTTON_MISC1, "misc_1"},
	{SDL_GAMEPAD_BUTTON_RIGHT_PADDLE1, "right_paddle_1"},
	{SDL_GAMEPAD_BUTTON_LEFT_PADDLE1, "left_paddle_1"},
	{SDL_GAMEPAD_BUTTON_RIGHT_PADDLE2, "right_paddle_2"},
	{SDL_GAMEPAD_BUTTON_LEFT_PADDLE2, "left_paddle_2"},
	{SDL_GAMEPAD_BUTTON_TOUCHPAD, "touchpad"}, {SDL_GAMEPAD_BUTTON_MISC2, "misc_2"},
	{SDL_GAMEPAD_BUTTON_MISC3, "misc_3"}, {SDL_GAMEPAD_BUTTON_MISC4, "misc_4"},
	{SDL_GAMEPAD_BUTTON_MISC5, "misc_5"}, {SDL_GAMEPAD_BUTTON_MISC6, "misc_6"},
};
// clang-format on

const NamedCode *find_by_code(const NamedCode *begin, const NamedCode *end, int code) {
	for (const NamedCode *entry = begin; entry != end; ++entry) {
		if (entry->code == code)
			return entry;
	}
	return nullptr;
}

const NamedCode *find_by_name(const NamedCode *begin, const NamedCode *end, std::string_view name) {
	for (const NamedCode *entry = begin; entry != end; ++entry) {
		if (entry->name == name)
			return entry;
	}
	return nullptr;
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
		if (const NamedCode *entry =
				find_by_code(std::begin(GAMEPAD_CODES), std::end(GAMEPAD_CODES), button))
			return {LegacyBindingKind::Gamepad, entry->name};
		return {LegacyBindingKind::Unsupported, {}};
	}

	int scancode = code;
	if ((static_cast<std::uint32_t>(code) & SDLK_SCANCODE_MASK) != 0)
		scancode = static_cast<int>(static_cast<std::uint32_t>(code) ^ SDLK_SCANCODE_MASK);
	if (const NamedCode *entry =
			find_by_code(std::begin(KEYBOARD_CODES), std::end(KEYBOARD_CODES), scancode))
		return {LegacyBindingKind::Keyboard, entry->name};
	return {LegacyBindingKind::Unsupported, {}};
}

std::optional<int> keyboard_binding_code(std::string_view name) {
	if (const NamedCode *entry =
			find_by_name(std::begin(KEYBOARD_CODES), std::end(KEYBOARD_CODES), name))
		return entry->code;
	return std::nullopt;
}

std::optional<int> gamepad_binding_code(std::string_view name) {
	if (const NamedCode *entry =
			find_by_name(std::begin(GAMEPAD_CODES), std::end(GAMEPAD_CODES), name))
		return entry->code | LEGACY_GAMEPAD_BUTTON_MASK;
	return std::nullopt;
}

} // namespace vangers::settings
