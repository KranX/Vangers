#ifndef VANGERS_SETTINGS_SETTINGS_H
#define VANGERS_SETTINGS_SETTINGS_H

#include <filesystem>
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace vangers::settings {

constexpr int SETTINGS_FORMAT_VERSION = 1;

enum class ResolutionMode {
	Legacy800x600,
	Desktop,
};

using BindingList = std::vector<std::string>;
using BindingMap = std::map<std::string, BindingList>;

struct VideoSettings {
	bool fullscreen = false;
	ResolutionMode resolution = ResolutionMode::Desktop;
	int fps = 20;
	int detail = 0;
	bool keep_terrain_changes = false;
	bool destroy_terrain = true;
};

struct AudioSettings {
	bool sound_enabled = true;
	int sound_volume = 26;
	bool music_enabled = true;
	int music_volume = 26;
	bool panning = true;
	bool engine_noise = true;
	bool background_sound = true;
};

struct GameplaySettings {
	bool tutorial = true;
	bool auto_acceleration = false;
	bool camera_rotation = false;
	bool camera_slope = false;
	bool camera_zoom = false;
};

struct NetworkSettings {
	std::string player_name = "Vanger";
	int player_color = 0;
	std::string player_password = "SOME UNIQUE STRING";
	std::string server = "v5.vangers.net";
	int port = 2197;
	bool proxy_enabled = false;
	std::string proxy_server = "192.1.1.1";
	int proxy_port = 1080;
};

struct KeyboardSettings {
	BindingMap bindings;
};

struct ControllerSettings {
	bool enabled = true;
	float cursor_speed = 1.0f;
	bool rumble = true;
};

struct AxisBinding {
	std::string axis;
	bool inverted = false;
};

struct SdlGamepadSettings {
	float stick_deadzone = 0.18f;
	float trigger_deadzone = 0.05f;
	std::map<std::string, AxisBinding> axes;
	BindingMap bindings;
};

struct InputSettings {
	KeyboardSettings keyboard;
	ControllerSettings controller;
	SdlGamepadSettings sdl_gamepad;
};

struct GameSettings {
	VideoSettings video;
	AudioSettings audio;
	GameplaySettings gameplay;
	NetworkSettings network;
	InputSettings input;
};

GameSettings default_settings();
void reset_input_bindings_to_defaults(GameSettings &settings);
void normalize_settings(GameSettings &settings);

struct SettingsPaths {
	std::filesystem::path settings_file = "settings.toml";
	std::filesystem::path legacy_options_file = "options.dat";
	std::filesystem::path legacy_controls_file = "controls.dat";
};

enum class SettingsLoadSource {
	Toml,
	Legacy,
	Defaults,
	RecoveredDefaults,
};

struct SettingsLoadResult {
	SettingsLoadSource source = SettingsLoadSource::Defaults;
	bool settings_file_written = false;
	bool read_only = false;
	std::string diagnostic;
};

class SettingsManager {
  public:
	explicit SettingsManager(SettingsPaths paths = {});
	~SettingsManager();

	SettingsManager(SettingsManager &&) noexcept;
	SettingsManager &operator=(SettingsManager &&) noexcept;

	SettingsManager(const SettingsManager &) = delete;
	SettingsManager &operator=(const SettingsManager &) = delete;

	SettingsLoadResult load();
	bool save(std::string *diagnostic = nullptr);

	const GameSettings &get() const;
	GameSettings &get_mutable();
	const SettingsPaths &paths() const;
	bool is_loaded() const;
	bool can_save() const;

  private:
	struct Impl;
	std::unique_ptr<Impl> impl_;
};

SettingsManager &settings_manager();

} // namespace vangers::settings

#endif
