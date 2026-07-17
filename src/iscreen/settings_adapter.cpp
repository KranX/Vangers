#include "settings_adapter.h"

#include "controls.h"
#include "iscreen_options.h"

#include "settings/input_binding.h"
#include "settings/settings.h"
#include "settings/text_encoding.h"

#include <charconv>
#include <optional>
#include <string>
#include <string_view>
#include <utility>

namespace vangers::settings {
namespace {

constexpr std::size_t PLAYER_NAME_MAX_LENGTH = 16;
constexpr std::size_t PLAYER_PASSWORD_MAX_LENGTH = 16;
constexpr std::size_t SERVER_ADDRESS_MAX_LENGTH = 31;
constexpr std::size_t PROXY_SERVER_MAX_LENGTH = 21;
constexpr std::size_t PROXY_PORT_MAX_LENGTH = 11;
constexpr std::size_t SERVER_PORT_MAX_LENGTH = 7;

std::string ui_text(std::string_view utf8, std::size_t maximum_length) {
	std::string text = utf8_to_cp866(utf8);
	if (text.size() > maximum_length)
		text.resize(maximum_length);
	return text;
}

std::string option_text(int option_id) {
	const char *text = iGetOptionValueCHR(option_id);
	return text ? cp866_to_utf8(text) : std::string{};
}

void set_text_option(int option_id, std::string_view text, std::size_t maximum_length) {
	const std::string converted = ui_text(text, maximum_length);
	iSetOptionValueCHR(option_id, converted.c_str());
}

std::optional<int> parse_port(const char *text) {
	if (!text || !*text)
		return std::nullopt;
	const std::string_view value(text);
	int port = 0;
	const auto result = std::from_chars(value.data(), value.data() + value.size(), port);
	if (result.ec != std::errc{} || result.ptr != value.data() + value.size() || port < 1 ||
		port > 65535)
		return std::nullopt;
	return port;
}

void set_port_option(int option_id, int port, std::size_t maximum_length) {
	set_text_option(option_id, std::to_string(port), maximum_length);
}

} // namespace

void apply_settings_to_interface() {
	SettingsManager &manager = settings_manager();
	if (!manager.is_loaded())
		manager.load();
	const GameSettings &settings = manager.get();

	iSetOptionValue(iSOUND_ON, settings.audio.sound_enabled ? 0 : 1);
	iSetOptionValue(iSOUND_VOLUME_CUR, settings.audio.sound_volume);
	iSetOptionValue(iMUSIC_ON, settings.audio.music_enabled ? 0 : 1);
	iSetOptionValue(iMUSIC_VOLUME_CUR, settings.audio.music_volume);
	iSetOptionValue(iTUTORIAL_ON, settings.gameplay.tutorial ? 0 : 1);
	iSetOptionValue(iDETAIL_SETTING, settings.video.detail);
	iSetOptionValue(
		iSCREEN_RESOLUTION, settings.video.resolution == ResolutionMode::Desktop ? 1 : 0
	);
	iSetOptionValue(iPLAYER_COLOR, settings.network.player_color);
	set_text_option(iPLAYER_NAME2, settings.network.player_name, PLAYER_NAME_MAX_LENGTH);
	set_text_option(iPLAYER_PASSWORD, settings.network.player_password, PLAYER_PASSWORD_MAX_LENGTH);
	set_text_option(iHOST_NAME, settings.network.server, SERVER_ADDRESS_MAX_LENGTH);
	iSetOptionValue(iKEEP_MODE, settings.video.keep_terrain_changes ? 1 : 0);
	iSetOptionValue(iPANNING_ON, settings.audio.panning ? 0 : 1);
	iSetOptionValue(iDESTR_MODE, settings.video.destroy_terrain ? 1 : 0);
	iSetOptionValue(iMECH_SOUND, settings.audio.engine_noise ? 0 : 1);
	iSetOptionValue(iBACK_SOUND, settings.audio.background_sound ? 0 : 1);
	iSetOptionValue(iPROXY_USAGE, settings.network.proxy_enabled ? 1 : 0);
	set_text_option(iPROXY_SERVER, settings.network.proxy_server, PROXY_SERVER_MAX_LENGTH);
	set_port_option(iPROXY_PORT, settings.network.proxy_port, PROXY_PORT_MAX_LENGTH);
	set_port_option(iSERVER_PORT, settings.network.port, SERVER_PORT_MAX_LENGTH);
	iSetOptionValue(iCAMERA_TURN, settings.gameplay.camera_rotation ? 1 : 0);
	iSetOptionValue(iCAMERA_SLOPE, settings.gameplay.camera_slope ? 1 : 0);
	iSetOptionValue(iCAMERA_SCALE, settings.gameplay.camera_zoom ? 1 : 0);
	iSetOptionValue(iFULLSCREEN, settings.video.fullscreen ? 1 : 0);
	iSetOptionValue(iAUTO_ACCELERATION, settings.gameplay.auto_acceleration ? 1 : 0);
	iSetOptionValue(iFPS_60, settings.video.fps == 60 ? 1 : 0);
}

void capture_settings_from_interface() {
	SettingsManager &manager = settings_manager();
	if (!manager.is_loaded())
		manager.load();
	GameSettings &settings = manager.get_mutable();

	settings.audio.sound_enabled = iGetOptionValue(iSOUND_ON) == 0;
	settings.audio.sound_volume = iGetOptionValue(iSOUND_VOLUME_CUR);
	settings.audio.music_enabled = iGetOptionValue(iMUSIC_ON) == 0;
	settings.audio.music_volume = iGetOptionValue(iMUSIC_VOLUME_CUR);
	settings.audio.panning = iGetOptionValue(iPANNING_ON) == 0;
	settings.audio.engine_noise = iGetOptionValue(iMECH_SOUND) == 0;
	settings.audio.background_sound = iGetOptionValue(iBACK_SOUND) == 0;
	settings.gameplay.tutorial = iGetOptionValue(iTUTORIAL_ON) == 0;
	settings.gameplay.auto_acceleration = iGetOptionValue(iAUTO_ACCELERATION) != 0;
	settings.gameplay.camera_rotation = iGetOptionValue(iCAMERA_TURN) != 0;
	settings.gameplay.camera_slope = iGetOptionValue(iCAMERA_SLOPE) != 0;
	settings.gameplay.camera_zoom = iGetOptionValue(iCAMERA_SCALE) != 0;
	settings.video.fullscreen = iGetOptionValue(iFULLSCREEN) != 0;
	settings.video.resolution = iGetOptionValue(iSCREEN_RESOLUTION) != 0
									? ResolutionMode::Desktop
									: ResolutionMode::Legacy800x600;
	settings.video.fps = iGetOptionValue(iFPS_60) != 0 ? 60 : 20;
	settings.video.detail = iGetOptionValue(iDETAIL_SETTING);
	settings.video.keep_terrain_changes = iGetOptionValue(iKEEP_MODE) != 0;
	settings.video.destroy_terrain = iGetOptionValue(iDESTR_MODE) != 0;
	settings.network.player_name = option_text(iPLAYER_NAME2);
	settings.network.player_color = iGetOptionValue(iPLAYER_COLOR);
	settings.network.player_password = option_text(iPLAYER_PASSWORD);
	settings.network.server = option_text(iHOST_NAME);
	settings.network.proxy_enabled = iGetOptionValue(iPROXY_USAGE) != 0;
	settings.network.proxy_server = option_text(iPROXY_SERVER);
	if (const auto port = parse_port(iGetOptionValueCHR(iSERVER_PORT)))
		settings.network.port = *port;
	if (const auto port = parse_port(iGetOptionValueCHR(iPROXY_PORT)))
		settings.network.proxy_port = *port;

	normalize_settings(settings);
}

void apply_settings_to_controls() {
	SettingsManager &manager = settings_manager();
	if (!manager.is_loaded())
		manager.load();
	const GameSettings &settings = manager.get();

	for (int control_id = 1; control_id < iKEY_MAX_ID; ++control_id) {
		const char *action = legacy_control_action_name(control_id);
		if (!action)
			continue;
		iResetControlCode(control_id, 0);
		iResetControlCode(control_id, 1);
		int slot = 0;
		const auto keyboard = settings.input.keyboard.bindings.find(action);
		if (keyboard != settings.input.keyboard.bindings.end()) {
			for (const std::string &name : keyboard->second) {
				if (slot >= 2)
					break;
				if (const auto code = keyboard_binding_code(name))
					iSetControlCode(control_id, *code, slot++);
			}
		}
		const auto gamepad = settings.input.sdl_gamepad.bindings.find(action);
		if (gamepad != settings.input.sdl_gamepad.bindings.end()) {
			for (const std::string &name : gamepad->second) {
				if (slot >= 2)
					break;
				if (const auto code = gamepad_binding_code(name))
					iSetControlCode(control_id, *code, slot++);
			}
		}
	}
}

void capture_settings_from_controls() {
	SettingsManager &manager = settings_manager();
	if (!manager.is_loaded())
		manager.load();
	GameSettings &settings = manager.get_mutable();

	for (int control_id = 1; control_id < iKEY_MAX_ID; ++control_id) {
		const char *action = legacy_control_action_name(control_id);
		if (!action)
			continue;
		BindingList keyboard;
		BindingList gamepad;
		for (int slot = 0; slot < 2; ++slot) {
			const DecodedLegacyBinding binding =
				decode_legacy_control_code(iGetControlCode(control_id, slot));
			if (binding.kind == LegacyBindingKind::Keyboard)
				keyboard.push_back(binding.name);
			else if (binding.kind == LegacyBindingKind::Gamepad)
				gamepad.push_back(binding.name);
		}
		settings.input.keyboard.bindings[action] = std::move(keyboard);
		if (!gamepad.empty())
			settings.input.sdl_gamepad.bindings[action] = std::move(gamepad);
	}
	normalize_settings(settings);
}

} // namespace vangers::settings
