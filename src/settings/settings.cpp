#include "settings.h"

#include <algorithm>
#include <cmath>
#include <set>

namespace vangers::settings {
namespace {

BindingMap default_keyboard_bindings() {
	return {
		{"turn_left", {"left"}},
		{"turn_right", {"right"}},
		{"move_forward", {"up"}},
		{"move_backward", {"down"}},
		{"roll_left", {"a"}},
		{"roll_right", {"d"}},
		{"device_on", {"r", "page_up"}},
		{"device_off", {"f", "page_down"}},
		{"activate_kid", {"w"}},
		{"acceleration", {"left_shift", "right_shift"}},
		{"vertical_thrust", {}},
		{"inventory", {"enter"}},
		{"open", {"space"}},
		{"fire_weapon_1", {"1"}},
		{"fire_weapon_2", {"2"}},
		{"fire_weapon_3", {"3"}},
		{"fire_weapon_4", {"4"}},
		{"fire_all", {"e", "left_ctrl"}},
		{"change_target", {"tab"}},
		{"fullscreen", {"f1"}},
		{"reduce_view", {}},
		{"enlarge_view", {}},
		{"camera_rotation", {"f5"}},
		{"auto_zoom", {"f7"}},
		{"auto_tilt", {"f6"}},
		{"zoom_in", {"equals"}},
		{"zoom_out", {"minus"}},
		{"zoom_standard", {"0"}},
		{"handbrake", {"q"}},
		{"use_gluek", {"5"}},
		{"skip_text", {"tab"}},
		{"use_vector", {"v"}},
		{"frag_info", {"s"}},
		{"chat", {"f3"}},
		{"exit", {}},
		{"screenshot", {"f9"}},
	};
}

void normalize_bindings(BindingMap &bindings) {
	for (auto &[action, values] : bindings) {
		(void)action;
		BindingList normalized;
		std::set<std::string> seen;
		for (const std::string &value : values) {
			if (value.empty() || !seen.insert(value).second)
				continue;
			normalized.push_back(value);
			if (normalized.size() == 8)
				break;
		}
		values = std::move(normalized);
	}
}

float normalize_float(float value, float fallback, float minimum, float maximum) {
	if (!std::isfinite(value) || value < minimum || value > maximum)
		return fallback;
	return value;
}

} // namespace

GameSettings default_settings() {
	GameSettings settings;
	settings.input.keyboard.bindings = default_keyboard_bindings();
	settings.input.sdl_gamepad.axes = {
		{"steering", {"left_x", false}},
		{"throttle", {"left_y", true}},
		{"cursor_x", {"right_x", false}},
		{"cursor_y", {"right_y", false}},
	};
	settings.input.sdl_gamepad.bindings = {
		{"open", {"south"}},
		{"inventory", {"north"}},
		{"fire_all", {"right_trigger"}},
		{"pause", {"start"}},
		{"menu_confirm", {"south"}},
		{"menu_cancel", {"east"}},
	};
	return settings;
}

void normalize_settings(GameSettings &settings) {
	settings.video.fps = settings.video.fps == 60 ? 60 : 20;
	settings.video.detail = std::clamp(settings.video.detail, 0, 1);
	settings.audio.sound_volume = std::clamp(settings.audio.sound_volume, 0, 26);
	settings.audio.music_volume = std::clamp(settings.audio.music_volume, 0, 26);
	settings.network.player_color = std::clamp(settings.network.player_color, 0, 9);
	if (settings.network.port < 1 || settings.network.port > 65535)
		settings.network.port = 2197;
	if (settings.network.proxy_port < 1 || settings.network.proxy_port > 65535)
		settings.network.proxy_port = 1080;
	settings.input.controller.cursor_speed =
		normalize_float(settings.input.controller.cursor_speed, 1.0f, 0.1f, 10.0f);
	settings.input.sdl_gamepad.stick_deadzone =
		normalize_float(settings.input.sdl_gamepad.stick_deadzone, 0.18f, 0.0f, 0.95f);
	settings.input.sdl_gamepad.trigger_deadzone =
		normalize_float(settings.input.sdl_gamepad.trigger_deadzone, 0.05f, 0.0f, 0.95f);
	normalize_bindings(settings.input.keyboard.bindings);
	normalize_bindings(settings.input.sdl_gamepad.bindings);
}

} // namespace vangers::settings
