#include "xgamepad.h"

#include "settings/gamepad_mapping.h"
#include "settings/settings.h"

#include "xglobal.h"
#include "xgraph.h"

#include <algorithm>
#include <cmath>
#include <cstdio>

void xtRegisterSysMsgFnc(void (*fPtr)(SDL_Event *), int id);

namespace {

constexpr int XGAMEPAD_SYSOBJ_ID = 0x05;
constexpr float CURSOR_PIXELS_PER_SECOND = 600.0f;

SDL_Gamepad *active_gamepad = nullptr;
SDL_JoystickID active_gamepad_id = 0;
bool gamepad_initialized = false;
bool message_handler_registered = false;
bool quant_registered = false;
bool confirm_pressed = false;
bool confirm_action_down = false;
bool pause_action_down = false;
bool cancel_action_down = false;
Uint64 previous_cursor_time = 0;
float cursor_remainder_x = 0.0f;
float cursor_remainder_y = 0.0f;

const vangers::settings::GameSettings &current_settings() {
	auto &manager = vangers::settings::settings_manager();
	if (!manager.is_loaded())
		manager.load();
	return manager.get();
}

bool controller_enabled() {
	return current_settings().input.controller.enabled;
}

void release_confirm() {
	if (!confirm_pressed)
		return;
	if (XGR_MouseObj.flags & XGM_INIT) {
		XGR_MouseUnPress(XGM_LEFT_BUTTON, 0, XGR_MouseObj.PosX, XGR_MouseObj.PosY);
	}
	confirm_pressed = false;
}

void reset_ui_actions() {
	release_confirm();
	confirm_action_down = false;
	pause_action_down = false;
	cancel_action_down = false;
}

void close_active_gamepad() {
	reset_ui_actions();
	if (active_gamepad)
		SDL_CloseGamepad(active_gamepad);
	active_gamepad = nullptr;
	active_gamepad_id = 0;
}

bool open_gamepad(SDL_JoystickID instance_id) {
	if (active_gamepad || !SDL_IsGamepad(instance_id))
		return false;

	SDL_Gamepad *opened = SDL_OpenGamepad(instance_id);
	if (!opened)
		return false;

	active_gamepad = opened;
	active_gamepad_id = SDL_GetGamepadID(opened);
	const char *name = SDL_GetGamepadName(opened);
	std::printf(
		"Gamepad '%u' opened: '%s'\n",
		static_cast<unsigned>(active_gamepad_id),
		name ? name : "unknown"
	);
	return true;
}

bool open_first_gamepad() {
	int count = 0;
	SDL_JoystickID *gamepads = SDL_GetGamepads(&count);
	if (!gamepads)
		return false;

	bool opened = false;
	for (int i = 0; i < count && !opened; ++i)
		opened = open_gamepad(gamepads[i]);
	SDL_free(gamepads);
	return opened;
}

bool action_has_button(std::string_view action, SDL_GamepadButton button) {
	const auto &bindings = current_settings().input.sdl_gamepad.bindings;
	const auto action_bindings = bindings.find(std::string(action));
	if (action_bindings == bindings.end())
		return false;

	for (const std::string &name : action_bindings->second) {
		if (vangers::settings::gamepad_button_from_name(name) == button)
			return true;
	}
	return false;
}

float axis_value(SDL_GamepadAxis axis, bool inverted) {
	if (!active_gamepad || !SDL_GamepadHasAxis(active_gamepad, axis))
		return 0.0f;
	const auto &settings = current_settings().input.sdl_gamepad;
	return vangers::settings::normalize_gamepad_axis(
		SDL_GetGamepadAxis(active_gamepad, axis),
		axis,
		settings.stick_deadzone,
		settings.trigger_deadzone,
		inverted
	);
}

void push_escape_event() {
	SDL_Event event;
	SDL_zero(event);
	event.type = SDL_EVENT_KEY_DOWN;
	event.key.timestamp = SDL_GetTicksNS();
	event.key.scancode = SDL_SCANCODE_ESCAPE;
	event.key.key = SDLK_ESCAPE;
	event.key.down = true;
	SDL_PushEvent(&event);
}

void update_ui_actions(bool cursor_visible, bool actions_enabled) {
	const bool confirm = XGamepadActionPressed("menu_confirm");
	if (actions_enabled && cursor_visible && confirm && !confirm_action_down) {
		confirm_pressed = true;
		XGR_MousePress(XGM_LEFT_BUTTON, 0, XGR_MouseObj.PosX, XGR_MouseObj.PosY);
	}
	if (confirm_pressed && (!actions_enabled || !confirm || !cursor_visible))
		release_confirm();
	confirm_action_down = confirm;

	const bool pause = XGamepadActionPressed("pause");
	const bool cancel = XGamepadActionPressed("menu_cancel");
	if (actions_enabled &&
		((pause && !pause_action_down) || (cursor_visible && cancel && !cancel_action_down)))
		push_escape_event();
	pause_action_down = pause;
	cancel_action_down = cancel;
}

void handle_gamepad_event(SDL_Event *event) {
	if (!gamepad_initialized)
		return;

	switch (event->type) {
	case SDL_EVENT_GAMEPAD_ADDED:
		if (!active_gamepad)
			open_gamepad(event->gdevice.which);
		break;
	case SDL_EVENT_GAMEPAD_REMOVED:
		if (event->gdevice.which == active_gamepad_id) {
			close_active_gamepad();
			open_first_gamepad();
		}
		break;
	default:
		break;
	}
}

void update_gamepad_cursor(Uint64 elapsed, bool cursor_visible) {
	if (!cursor_visible) {
		cursor_remainder_x = 0.0f;
		cursor_remainder_y = 0.0f;
		return;
	}

	const float x_axis = XGamepadAxisValue("cursor_x");
	const float y_axis = XGamepadAxisValue("cursor_y");
	if (x_axis == 0.0f && y_axis == 0.0f) {
		cursor_remainder_x = 0.0f;
		cursor_remainder_y = 0.0f;
		return;
	}

	const float seconds = static_cast<float>(std::min<Uint64>(elapsed, 100)) / 1000.0f;
	const float resolution_scale = std::max(1.0f, static_cast<float>(XGR_MAXX) / 800.0f);
	const float speed = CURSOR_PIXELS_PER_SECOND *
						current_settings().input.controller.cursor_speed * resolution_scale;
	cursor_remainder_x += x_axis * speed * seconds;
	cursor_remainder_y += y_axis * speed * seconds;

	const int step_x = static_cast<int>(cursor_remainder_x);
	const int step_y = static_cast<int>(cursor_remainder_y);
	if (!step_x && !step_y)
		return;
	cursor_remainder_x -= static_cast<float>(step_x);
	cursor_remainder_y -= static_cast<float>(step_y);

	const int current_x = XGR_MouseObj.PosX + XGR_MouseObj.SpotX;
	const int current_y = XGR_MouseObj.PosY + XGR_MouseObj.SpotY;
	const int next_x = std::clamp(current_x + step_x, 0, XGR_MAXX - 1);
	const int next_y = std::clamp(current_y + step_y, 0, XGR_MAXY - 1);
	XGR_MouseSetPos(next_x, next_y);
	XGR_MouseObj.MovementX = next_x - current_x;
	XGR_MouseObj.MovementY = next_y - current_y;
	XGR_MouseMove(0, XGR_MouseObj.PosX, XGR_MouseObj.PosY);
}

void update_gamepad_input() {
	const Uint64 now = SDL_GetTicks();
	const Uint64 elapsed = previous_cursor_time ? now - previous_cursor_time : 0;
	previous_cursor_time = now;

	if (!gamepad_initialized || !active_gamepad || !controller_enabled()) {
		reset_ui_actions();
		cursor_remainder_x = 0.0f;
		cursor_remainder_y = 0.0f;
		return;
	}

	SDL_UpdateGamepads();
	const bool cursor_visible = XGR_MouseVisible();
	update_gamepad_cursor(elapsed, cursor_visible);
	update_ui_actions(cursor_visible, !SDL_TextInputActive(XGR_Obj.get_window()));
}

} // namespace

bool XGamepadInit() {
	if (gamepad_initialized)
		return active_gamepad != nullptr;
	if (!SDL_InitSubSystem(SDL_INIT_GAMEPAD)) {
		std::fprintf(stderr, "Unable to initialize SDL gamepads: %s\n", SDL_GetError());
		return false;
	}

	gamepad_initialized = true;
	previous_cursor_time = SDL_GetTicks();
	cursor_remainder_x = 0.0f;
	cursor_remainder_y = 0.0f;
	SDL_SetGamepadEventsEnabled(true);
	if (!message_handler_registered) {
		xtRegisterSysMsgFnc(handle_gamepad_event, XGAMEPAD_SYSOBJ_ID);
		message_handler_registered = true;
	}
	if (!quant_registered) {
		xtRegisterSysQuant(update_gamepad_input, XGAMEPAD_SYSOBJ_ID);
		quant_registered = true;
	}
	open_first_gamepad();
	return active_gamepad != nullptr;
}

void XGamepadCleanup() {
	if (!gamepad_initialized)
		return;
	close_active_gamepad();
	if (quant_registered) {
		xtUnRegisterSysQuant(XGAMEPAD_SYSOBJ_ID);
		quant_registered = false;
	}
	previous_cursor_time = 0;
	cursor_remainder_x = 0.0f;
	cursor_remainder_y = 0.0f;
	gamepad_initialized = false;
	SDL_QuitSubSystem(SDL_INIT_GAMEPAD);
}

bool XGamepadIsAvailable() {
	return active_gamepad != nullptr;
}

bool XGamepadActionPressed(std::string_view action) {
	if (!active_gamepad || !controller_enabled())
		return false;

	const auto &settings = current_settings().input.sdl_gamepad;
	const auto action_bindings = settings.bindings.find(std::string(action));
	if (action_bindings == settings.bindings.end())
		return false;

	for (const std::string &name : action_bindings->second) {
		if (const auto button = vangers::settings::gamepad_button_from_name(name)) {
			if (XGamepadButtonPressed(*button)) {
				return true;
			}
			continue;
		}
		const auto axis = vangers::settings::gamepad_axis_from_name(name);
		if (axis && vangers::settings::gamepad_axis_is_trigger(*axis) &&
			vangers::settings::gamepad_trigger_pressed(axis_value(*axis, false))) {
			return true;
		}
	}
	return false;
}

bool XGamepadButtonPressed(SDL_GamepadButton button) {
	return active_gamepad && controller_enabled() && SDL_GamepadHasButton(active_gamepad, button) &&
		   SDL_GetGamepadButton(active_gamepad, button);
}

bool XGamepadButtonMatchesAction(std::string_view action, SDL_GamepadButton button) {
	return active_gamepad && controller_enabled() && action_has_button(action, button);
}

bool XGamepadOwnsEvent(const SDL_Event &event) {
	// Configuration must still see the connected device while gameplay input is
	// disabled, otherwise the controls screen cannot bind a button before the
	// controller option is enabled again.
	if (!active_gamepad)
		return false;
	if (event.type == SDL_EVENT_GAMEPAD_BUTTON_DOWN || event.type == SDL_EVENT_GAMEPAD_BUTTON_UP) {
		return event.gbutton.which == active_gamepad_id;
	}
	return false;
}

float XGamepadAxisValue(std::string_view logical_axis) {
	if (!active_gamepad || !controller_enabled())
		return 0.0f;

	const auto &axes = current_settings().input.sdl_gamepad.axes;
	const auto binding = axes.find(std::string(logical_axis));
	if (binding == axes.end())
		return 0.0f;
	const auto axis = vangers::settings::gamepad_axis_from_name(binding->second.axis);
	if (!axis)
		return 0.0f;
	return axis_value(*axis, binding->second.inverted);
}

bool XGamepadIsControllingCursor() {
	return active_gamepad && controller_enabled() && XGR_MouseVisible();
}

void XGamepadRumble(float low_frequency, float high_frequency, Uint32 duration_ms) {
	if (!active_gamepad || !controller_enabled() || !current_settings().input.controller.rumble)
		return;

	const auto intensity = [](float value) {
		return static_cast<Uint16>(std::lround(std::clamp(value, 0.0f, 1.0f) * 65535.0f));
	};
	SDL_RumbleGamepad(
		active_gamepad, intensity(low_frequency), intensity(high_frequency), duration_ms
	);
}

SDL_Gamepad *XGamepadHandle() {
	return active_gamepad;
}
