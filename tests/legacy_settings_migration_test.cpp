#include "settings/gamepad_mapping.h"
#include "settings/input_binding.h"
#include "settings/legacy_settings_import.h"
#include "settings/settings.h"

#include <array>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include <SDL3/SDL.h>

namespace {

using namespace vangers::settings;

int failures = 0;

#define CHECK(condition)                                                                       \
	do {                                                                                       \
		if (!(condition)) {                                                                    \
			std::cerr << __FILE__ << ':' << __LINE__ << ": CHECK failed: " #condition << '\n'; \
			++failures;                                                                        \
		}                                                                                      \
	} while (false)

class TempDirectory {
  public:
	TempDirectory() {
		path = std::filesystem::temp_directory_path() /
			   ("vangers-legacy-settings-test-" +
				   std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()));
		std::filesystem::create_directories(path);
	}

	~TempDirectory() {
		std::error_code error;
		std::filesystem::remove_all(path, error);
	}

	std::filesystem::path path;
};

void append_i32(std::vector<std::uint8_t> &bytes, std::int32_t value) {
	const std::uint32_t encoded = static_cast<std::uint32_t>(value);
	bytes.push_back(static_cast<std::uint8_t>(encoded));
	bytes.push_back(static_cast<std::uint8_t>(encoded >> 8));
	bytes.push_back(static_cast<std::uint8_t>(encoded >> 16));
	bytes.push_back(static_cast<std::uint8_t>(encoded >> 24));
}

void overwrite_i32(std::vector<std::uint8_t> &bytes, std::size_t offset, std::int32_t value) {
	const std::uint32_t encoded = static_cast<std::uint32_t>(value);
	bytes.at(offset) = static_cast<std::uint8_t>(encoded);
	bytes.at(offset + 1) = static_cast<std::uint8_t>(encoded >> 8);
	bytes.at(offset + 2) = static_cast<std::uint8_t>(encoded >> 16);
	bytes.at(offset + 3) = static_cast<std::uint8_t>(encoded >> 24);
}

void append_string(std::vector<std::uint8_t> &bytes, const std::string &value) {
	append_i32(bytes, static_cast<std::int32_t>(value.size()));
	bytes.insert(bytes.end(), value.begin(), value.end());
}

void write_bytes(const std::filesystem::path &path, const std::vector<std::uint8_t> &bytes) {
	std::ofstream output(path, std::ios::binary | std::ios::trunc);
	output.write(reinterpret_cast<const char *>(bytes.data()), bytes.size());
}

std::vector<std::uint8_t>
make_options(int option_count, int option_46, int option_47, int repeated_auto) {
	std::vector<std::uint8_t> bytes;
	append_i32(bytes, option_count);

	// IDs 0..9.
	for (const int value : std::array<int, 10>{0, 17, 26, 1, 11, 26, 0, 1, 5, 5})
		append_i32(bytes, value);
	append_string(bytes, "temporary-server-name");	 // 10
	append_string(bytes, "temporary-server-name-2"); // 11
	append_string(bytes, "secret");					 // 12

	// IDs 13..19 are not saved.
	const std::string cp866_tester = std::string("\x92\xa5\xe1\xe2\xa5\xe0", 6); // Тестер
	// 20: canonical player name.
	append_string(bytes, cp866_tester);
	append_i32(bytes, 1);				  // 21 desktop resolution
	append_string(bytes, "example.test"); // 22
	// IDs 23 and 24 are not saved.
	append_i32(bytes, 1);				// 25 keep terrain
	append_i32(bytes, 0);				// 26 panning enabled (legacy inverted state)
	append_i32(bytes, 1);				// 27 terrain destruction
	append_i32(bytes, 5);				// 28 duplicate color
	append_string(bytes, cp866_tester); // 29 duplicate name
	append_i32(bytes, 1);				// 30 engine noise disabled (legacy inverted state)
	append_i32(bytes, 0);				// 31 background sound enabled (legacy inverted state)
	append_i32(bytes, 3);				// 32 obsolete joystick mode
	append_i32(bytes, 1);				// 33 proxy enabled
	append_string(bytes, "proxy.example.test"); // 34
	append_string(bytes, "1234");				// 35
	// IDs 36 and 37 are not saved.
	append_string(bytes, "2345");		// 38
	append_string(bytes, cp866_tester); // 39 duplicate name
	append_string(bytes, "secret");		// 40 duplicate password
	// ID 41 is not saved.
	append_i32(bytes, 1); // 42 camera rotation
	append_i32(bytes, 0); // 43 camera slope
	append_i32(bytes, 1); // 44 camera zoom
	append_i32(bytes, 1); // 45 fullscreen
	if (option_count >= 47)
		append_i32(bytes, option_46);
	if (option_count >= 48)
		append_i32(bytes, option_47);
	append_i32(bytes, repeated_auto);
	return bytes;
}

std::vector<std::uint8_t> make_controls(int control_count) {
	std::vector<std::uint8_t> bytes;
	append_i32(bytes, control_count);
	append_i32(bytes, 2);
	for (int control_id = 0; control_id < control_count; ++control_id) {
		int first = 0;
		int second = 0;
		if (control_id == 1) {
			first = SDL_SCANCODE_A;
			second = static_cast<int>(SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_F1));
		} else if (control_id == 18) {
			first = SDL_GAMEPAD_BUTTON_WEST | (1 << 27);
		} else if (control_id == 31) {
			first = 4 | (1 << 28);
		} else if (control_id == 35) {
			first = SDL_SCANCODE_F4;
		} else if (control_id == 37) {
			first = SDL_SCANCODE_F12;
		}
		append_i32(bytes, first);
		append_i32(bytes, second);
	}
	return bytes;
}

void check_common_options(const GameSettings &settings) {
	CHECK(settings.video.fullscreen);
	CHECK(settings.video.resolution == ResolutionMode::Desktop);
	CHECK(settings.video.detail == 1);
	CHECK(settings.video.keep_terrain_changes);
	CHECK(settings.video.destroy_terrain);
	CHECK(settings.audio.sound_enabled);
	CHECK(settings.audio.sound_volume == 17);
	CHECK(!settings.audio.music_enabled);
	CHECK(settings.audio.music_volume == 11);
	CHECK(settings.audio.panning);
	CHECK(!settings.audio.engine_noise);
	CHECK(settings.audio.background_sound);
	CHECK(settings.gameplay.tutorial);
	CHECK(settings.gameplay.camera_rotation);
	CHECK(!settings.gameplay.camera_slope);
	CHECK(settings.gameplay.camera_zoom);
	CHECK(settings.network.player_name == "Тестер");
	CHECK(settings.network.player_color == 5);
	CHECK(settings.network.player_password == "secret");
	CHECK(settings.network.server == "example.test");
	CHECK(settings.network.port == 2345);
	CHECK(settings.network.proxy_enabled);
	CHECK(settings.network.proxy_server == "proxy.example.test");
	CHECK(settings.network.proxy_port == 1234);
}

void test_options_versions() {
	TempDirectory temp;
	const auto file = temp.path / "options.dat";
	std::string diagnostic;

	write_bytes(file, make_options(46, 0, 0, 1));
	GameSettings settings = default_settings();
	CHECK(import_legacy_options(file, settings, &diagnostic));
	check_common_options(settings);
	CHECK(settings.video.fps == 20);
	CHECK(settings.gameplay.auto_acceleration);

	// Public 2.0 layout: equal slot 46 and tail are duplicate auto-acceleration.
	write_bytes(file, make_options(47, 1, 0, 1));
	settings = default_settings();
	CHECK(import_legacy_options(file, settings, &diagnostic));
	CHECK(settings.video.fps == 20);
	CHECK(settings.gameplay.auto_acceleration);

	// Historical alpha layout: a differing slot 46 is the FPS flag.
	write_bytes(file, make_options(47, 1, 0, 0));
	settings = default_settings();
	CHECK(import_legacy_options(file, settings, &diagnostic));
	CHECK(settings.video.fps == 60);
	CHECK(!settings.gameplay.auto_acceleration);

	write_bytes(file, make_options(48, 1, 1, 0));
	settings = default_settings();
	CHECK(import_legacy_options(file, settings, &diagnostic));
	CHECK(settings.video.fps == 60);
	CHECK(!settings.gameplay.auto_acceleration);
}

void test_invalid_options_are_rejected_atomically() {
	TempDirectory temp;
	const auto file = temp.path / "options.dat";
	std::string diagnostic;
	GameSettings settings = default_settings();
	settings.network.server = "unchanged";

	std::vector<std::uint8_t> truncated = make_options(48, 0, 1, 0);
	truncated.resize(truncated.size() - 3);
	write_bytes(file, truncated);
	CHECK(!import_legacy_options(file, settings, &diagnostic));
	CHECK(settings.network.server == "unchanged");

	std::vector<std::uint8_t> negative_length;
	append_i32(negative_length, 46);
	for (int i = 0; i < 10; ++i)
		append_i32(negative_length, 0);
	append_i32(negative_length, -1);
	write_bytes(file, negative_length);
	CHECK(!import_legacy_options(file, settings, &diagnostic));
	CHECK(settings.network.server == "unchanged");

	std::vector<std::uint8_t> excessive_length;
	append_i32(excessive_length, 46);
	for (int i = 0; i < 10; ++i)
		append_i32(excessive_length, 0);
	append_i32(excessive_length, 4097);
	write_bytes(file, excessive_length);
	CHECK(!import_legacy_options(file, settings, &diagnostic));
	CHECK(settings.network.server == "unchanged");
}

void test_controls_versions_and_binding_vocabulary() {
	TempDirectory temp;
	const auto file = temp.path / "controls.dat";
	std::string diagnostic;

	write_bytes(file, make_controls(37));
	GameSettings settings = default_settings();
	CHECK(import_legacy_controls(file, settings, &diagnostic));
	CHECK((settings.input.keyboard.bindings.at("turn_left") == BindingList{"a", "f1"}));
	CHECK((settings.input.keyboard.bindings.at("chat") == BindingList{"f4"}));
	CHECK((settings.input.sdl_gamepad.bindings.at("fire_all") == BindingList{"west"}));
	CHECK((settings.input.keyboard.bindings.at("screenshot") == BindingList{"f9"}));

	write_bytes(file, make_controls(38));
	settings = default_settings();
	CHECK(import_legacy_controls(file, settings, &diagnostic));
	CHECK((settings.input.keyboard.bindings.at("screenshot") == BindingList{"f12"}));

	CHECK(keyboard_binding_code("left_ctrl") == SDL_SCANCODE_LCTRL);
	CHECK(gamepad_binding_code("touchpad") == (SDL_GAMEPAD_BUTTON_TOUCHPAD | (1 << 27)));
	CHECK(!keyboard_binding_code("localized key name"));
	CHECK(!keyboard_binding_code("scancode:0"));
	CHECK(!keyboard_binding_code("scancode:-1"));
	CHECK(!keyboard_binding_code("scancode:512x"));
	CHECK(!keyboard_binding_code("scancode:" + std::to_string(SDL_SCANCODE_COUNT)));
	CHECK(legacy_control_action_id("chat") == 35);
	CHECK(!legacy_control_action_id("joystick_switch"));
}

void test_stable_binding_names_round_trip() {
	for (int scancode = 1; scancode < SDL_SCANCODE_COUNT; ++scancode) {
		const DecodedLegacyBinding decoded = decode_legacy_control_code(scancode);
		CHECK(decoded.kind == LegacyBindingKind::Keyboard);
		CHECK(keyboard_binding_code(decoded.name) == scancode);
	}
	for (int button = 0; button < SDL_GAMEPAD_BUTTON_COUNT; ++button) {
		const int legacy_code = button | (1 << 27);
		const DecodedLegacyBinding decoded = decode_legacy_control_code(legacy_code);
		CHECK(decoded.kind == LegacyBindingKind::Gamepad);
		CHECK(gamepad_binding_code(decoded.name) == legacy_code);
	}

	CHECK(decode_legacy_control_code(SDL_SCANCODE_A).name == "a");
	CHECK(decode_legacy_control_code(SDL_SCANCODE_0).name == "0");
	CHECK(decode_legacy_control_code(SDL_SCANCODE_F24).name == "f24");
	CHECK(decode_legacy_control_code(SDL_SCANCODE_LEFT).name == "left");
	CHECK(
		decode_legacy_control_code(SDL_SCANCODE_INTERNATIONAL1).name ==
		"scancode:" + std::to_string(SDL_SCANCODE_INTERNATIONAL1)
	);

	const GameSettings defaults = default_settings();
	for (const auto &[action, bindings] : defaults.input.keyboard.bindings) {
		(void)action;
		for (const std::string &binding : bindings)
			CHECK(keyboard_binding_code(binding));
	}
}

void test_gamepad_mapping_and_deadzone_scaling() {
	for (int button = 0; button < SDL_GAMEPAD_BUTTON_COUNT; ++button) {
		const auto code = static_cast<SDL_GamepadButton>(button);
		const auto name = gamepad_button_name(code);
		CHECK(name);
		CHECK(name && gamepad_button_from_name(*name) == code);
	}
	for (int axis = 0; axis < SDL_GAMEPAD_AXIS_COUNT; ++axis) {
		const auto code = static_cast<SDL_GamepadAxis>(axis);
		const auto name = gamepad_axis_name(code);
		CHECK(name);
		CHECK(name && gamepad_axis_from_name(*name) == code);
	}
	CHECK(!gamepad_button_from_name("unknown"));
	CHECK(!gamepad_axis_from_name("unknown"));

	CHECK(normalize_gamepad_axis(0, SDL_GAMEPAD_AXIS_LEFTX, 0.2f, 0.05f, false) == 0.0f);
	CHECK(normalize_gamepad_axis(6000, SDL_GAMEPAD_AXIS_LEFTX, 0.2f, 0.05f, false) == 0.0f);
	CHECK(
		std::abs(
			normalize_gamepad_axis(16384, SDL_GAMEPAD_AXIS_LEFTX, 0.2f, 0.05f, false) - 0.375f
		) < 0.001f
	);
	CHECK(normalize_gamepad_axis(-32768, SDL_GAMEPAD_AXIS_LEFTX, 0.2f, 0.05f, false) == -1.0f);
	CHECK(normalize_gamepad_axis(32767, SDL_GAMEPAD_AXIS_RIGHTY, 0.2f, 0.05f, true) == -1.0f);
	CHECK(normalize_gamepad_axis(1000, SDL_GAMEPAD_AXIS_RIGHT_TRIGGER, 0.2f, 0.05f, false) == 0.0f);
	CHECK(
		normalize_gamepad_axis(32767, SDL_GAMEPAD_AXIS_RIGHT_TRIGGER, 0.2f, 0.05f, false) == 1.0f
	);
	CHECK(gamepad_axis_is_trigger(SDL_GAMEPAD_AXIS_LEFT_TRIGGER));
	CHECK(!gamepad_axis_is_trigger(SDL_GAMEPAD_AXIS_LEFTX));
	CHECK(!gamepad_trigger_pressed(0.499f));
	CHECK(gamepad_trigger_pressed(0.5f));

	const GameSettings defaults = default_settings();
	CHECK(defaults.input.sdl_gamepad.axes.at("steering").axis == "left_x");
	CHECK(defaults.input.sdl_gamepad.axes.at("throttle").axis == "left_y");
	CHECK(defaults.input.sdl_gamepad.axes.at("throttle").inverted);
	CHECK(defaults.input.sdl_gamepad.axes.at("roll").axis == "right_x");
	CHECK(defaults.input.sdl_gamepad.axes.at("rig").axis == "right_y");
	CHECK(defaults.input.sdl_gamepad.axes.at("rig").inverted);
	CHECK((defaults.input.sdl_gamepad.bindings.at("open") == BindingList{"south"}));
	CHECK((defaults.input.sdl_gamepad.bindings.at("handbrake") == BindingList{"east"}));
	CHECK((defaults.input.sdl_gamepad.bindings.at("use_vector") == BindingList{"west"}));
	CHECK((defaults.input.sdl_gamepad.bindings.at("acceleration") == BindingList{"left_trigger"}));
	CHECK((defaults.input.sdl_gamepad.bindings.at("fire_all") == BindingList{"right_trigger"}));
	CHECK((defaults.input.sdl_gamepad.bindings.at("fire_weapon_1") == BindingList{"dpad_up"}));
	CHECK((defaults.input.sdl_gamepad.bindings.at("fire_weapon_2") == BindingList{"dpad_right"}));
	CHECK((defaults.input.sdl_gamepad.bindings.at("fire_weapon_3") == BindingList{"dpad_down"}));
	CHECK((defaults.input.sdl_gamepad.bindings.at("fire_weapon_4") == BindingList{"dpad_left"}));
	for (const auto &[action, bindings] : defaults.input.sdl_gamepad.bindings) {
		(void)action;
		for (const std::string &binding : bindings) {
			CHECK(
				gamepad_button_from_name(binding) ||
				(gamepad_axis_from_name(binding) &&
					gamepad_axis_is_trigger(*gamepad_axis_from_name(binding)))
			);
		}
	}
	for (const auto &[logical_axis, binding] : defaults.input.sdl_gamepad.axes) {
		(void)logical_axis;
		CHECK(gamepad_axis_from_name(binding.axis));
	}
}

void test_legacy_runtime_binding_projection_is_lossless() {
	BindingList keyboard{"a"};
	BindingList gamepad{"south", "east", "right_trigger"};
	LegacyControlCodes codes = encode_legacy_control_bindings(keyboard, gamepad);
	CHECK(codes[0] == SDL_SCANCODE_A);
	CHECK(codes[1] == (SDL_GAMEPAD_BUTTON_SOUTH | (1 << 27)));

	merge_legacy_control_bindings(codes, keyboard, gamepad);
	CHECK((keyboard == BindingList{"a"}));
	CHECK((gamepad == BindingList{"south", "east", "right_trigger"}));

	codes[0] = SDL_SCANCODE_B;
	merge_legacy_control_bindings(codes, keyboard, gamepad);
	CHECK((keyboard == BindingList{"b"}));
	CHECK((gamepad == BindingList{"south", "east", "right_trigger"}));

	codes[1] = 0;
	merge_legacy_control_bindings(codes, keyboard, gamepad);
	CHECK((keyboard == BindingList{"b"}));
	CHECK((gamepad == BindingList{"east", "right_trigger"}));

	keyboard = {"a", "f1", "f2"};
	gamepad = {"west"};
	codes = encode_legacy_control_bindings(keyboard, gamepad);
	codes[0] = SDL_SCANCODE_C;
	merge_legacy_control_bindings(codes, keyboard, gamepad);
	CHECK((keyboard == BindingList{"c", "f1", "f2"}));
	CHECK((gamepad == BindingList{"west"}));
}

void test_invalid_controls_do_not_modify_settings() {
	TempDirectory temp;
	const auto file = temp.path / "controls.dat";
	std::string diagnostic;
	GameSettings settings = default_settings();
	settings.input.keyboard.bindings["turn_left"] = {"f24"};

	std::vector<std::uint8_t> truncated = make_controls(38);
	truncated.pop_back();
	write_bytes(file, truncated);
	CHECK(!import_legacy_controls(file, settings, &diagnostic));
	CHECK((settings.input.keyboard.bindings.at("turn_left") == BindingList{"f24"}));

	std::vector<std::uint8_t> invalid_header;
	append_i32(invalid_header, 38);
	append_i32(invalid_header, 3);
	write_bytes(file, invalid_header);
	CHECK(!import_legacy_controls(file, settings, &diagnostic));
	CHECK((settings.input.keyboard.bindings.at("turn_left") == BindingList{"f24"}));
}

void test_unsupported_control_codes_keep_new_defaults() {
	TempDirectory temp;
	const auto file = temp.path / "controls.dat";
	std::string diagnostic;
	std::vector<std::uint8_t> controls = make_controls(38);
	constexpr std::size_t header_size = 2 * sizeof(std::int32_t);
	constexpr std::size_t control_size = 2 * sizeof(std::int32_t);
	// A valid sibling must survive an unknown code. An unsupported-only action
	// keeps its new default instead of becoming unbound.
	overwrite_i32(controls, header_size + 2 * control_size, 9999);
	overwrite_i32(controls, header_size + 2 * control_size + sizeof(std::int32_t), SDL_SCANCODE_F8);
	overwrite_i32(controls, header_size + 3 * control_size, 4 | (1 << 28));
	write_bytes(file, controls);

	GameSettings settings = default_settings();
	CHECK(import_legacy_controls(file, settings, &diagnostic));
	CHECK((settings.input.keyboard.bindings.at("turn_right") == BindingList{"f8"}));
	CHECK((settings.input.keyboard.bindings.at("move_forward") == BindingList{"up"}));
}

void test_combined_import_and_missing_files() {
	TempDirectory temp;
	SettingsPaths paths{
		temp.path / "settings.toml",
		temp.path / "options.dat",
		temp.path / "controls.dat",
	};
	GameSettings settings = default_settings();
	LegacyImportResult result = import_legacy_settings(paths, settings);
	CHECK(!result.any_imported());

	write_bytes(paths.legacy_options_file, make_options(48, 0, 1, 0));
	write_bytes(paths.legacy_controls_file, make_controls(38));
	result = import_legacy_settings(paths, settings);
	CHECK(result.options_imported);
	CHECK(result.controls_imported);
	CHECK(result.diagnostic.empty());
	CHECK(settings.video.fps == 60);
	CHECK((settings.input.keyboard.bindings.at("turn_left") == BindingList{"a", "f1"}));
}

} // namespace

int main() {
	test_options_versions();
	test_invalid_options_are_rejected_atomically();
	test_controls_versions_and_binding_vocabulary();
	test_stable_binding_names_round_trip();
	test_gamepad_mapping_and_deadzone_scaling();
	test_legacy_runtime_binding_projection_is_lossless();
	test_invalid_controls_do_not_modify_settings();
	test_unsupported_control_codes_keep_new_defaults();
	test_combined_import_and_missing_files();
	return failures == 0 ? 0 : 1;
}
