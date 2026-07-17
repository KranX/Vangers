#include "settings.h"

#include <algorithm>
#include <cerrno>
#include <chrono>
#include <cstdio>
#include <ctime>
#include <fstream>
#include <iostream>
#include <limits>
#include <optional>
#include <sstream>
#include <system_error>
#include <utility>

#ifdef _WIN32
#	include <windows.h>
#endif

#include <toml.hpp>

namespace vangers::settings {
namespace {

using TomlValue = toml::ordered_value;
using TomlTable = toml::ordered_table;
using TomlArray = toml::ordered_array;

const TomlValue *find_value(const TomlValue &root, std::initializer_list<const char *> path) {
	const TomlValue *current = &root;
	for (const char *key : path) {
		if (!current->is_table() || !current->contains(key))
			return nullptr;
		current = &current->at(key);
	}
	return current;
}

std::optional<bool> read_bool(const TomlValue &root, std::initializer_list<const char *> path) {
	const TomlValue *value = find_value(root, path);
	if (!value || !value->is_boolean())
		return std::nullopt;
	return value->as_boolean(std::nothrow);
}

std::optional<int> read_int(const TomlValue &root, std::initializer_list<const char *> path) {
	const TomlValue *value = find_value(root, path);
	if (!value || !value->is_integer())
		return std::nullopt;
	const std::int64_t number = value->as_integer(std::nothrow);
	if (number < std::numeric_limits<int>::min() || number > std::numeric_limits<int>::max())
		return std::nullopt;
	return static_cast<int>(number);
}

std::optional<float> read_float(const TomlValue &root, std::initializer_list<const char *> path) {
	const TomlValue *value = find_value(root, path);
	if (!value)
		return std::nullopt;
	if (value->is_floating())
		return static_cast<float>(value->as_floating(std::nothrow));
	if (value->is_integer())
		return static_cast<float>(value->as_integer(std::nothrow));
	return std::nullopt;
}

std::optional<std::string>
read_string(const TomlValue &root, std::initializer_list<const char *> path) {
	const TomlValue *value = find_value(root, path);
	if (!value || !value->is_string())
		return std::nullopt;
	return value->as_string(std::nothrow);
}

std::optional<BindingList> read_bindings(const TomlValue &table, const std::string &action) {
	if (!table.is_table() || !table.contains(action))
		return std::nullopt;
	const TomlValue &value = table.at(action);
	if (!value.is_array())
		return std::nullopt;
	BindingList bindings;
	for (const TomlValue &entry : value.as_array(std::nothrow)) {
		if (!entry.is_string())
			return std::nullopt;
		bindings.push_back(entry.as_string(std::nothrow));
	}
	return bindings;
}

void load_bindings(
	const TomlValue &root,
	const std::initializer_list<const char *> path,
	BindingMap &out
) {
	const TomlValue *table = find_value(root, path);
	if (!table || !table->is_table())
		return;
	for (auto &[action, bindings] : out) {
		if (const auto value = read_bindings(*table, action))
			bindings = *value;
	}
}

GameSettings decode_settings(const TomlValue &document) {
	GameSettings settings = default_settings();

	if (const auto value = read_bool(document, {"video", "fullscreen"}))
		settings.video.fullscreen = *value;
	if (const auto value = read_string(document, {"video", "resolution"})) {
		if (*value == "desktop")
			settings.video.resolution = ResolutionMode::Desktop;
		else if (*value == "800x600")
			settings.video.resolution = ResolutionMode::Legacy800x600;
	}
	if (const auto value = read_int(document, {"video", "fps"}))
		settings.video.fps = *value;
	if (const auto value = read_int(document, {"video", "detail"}))
		settings.video.detail = *value;
	if (const auto value = read_bool(document, {"video", "keep_terrain_changes"}))
		settings.video.keep_terrain_changes = *value;
	if (const auto value = read_bool(document, {"video", "destroy_terrain"}))
		settings.video.destroy_terrain = *value;

	if (const auto value = read_bool(document, {"audio", "sound_enabled"}))
		settings.audio.sound_enabled = *value;
	if (const auto value = read_int(document, {"audio", "sound_volume"}))
		settings.audio.sound_volume = *value;
	if (const auto value = read_bool(document, {"audio", "music_enabled"}))
		settings.audio.music_enabled = *value;
	if (const auto value = read_int(document, {"audio", "music_volume"}))
		settings.audio.music_volume = *value;
	if (const auto value = read_bool(document, {"audio", "panning"}))
		settings.audio.panning = *value;
	if (const auto value = read_bool(document, {"audio", "engine_noise"}))
		settings.audio.engine_noise = *value;
	if (const auto value = read_bool(document, {"audio", "background_sound"}))
		settings.audio.background_sound = *value;

	if (const auto value = read_bool(document, {"gameplay", "tutorial"}))
		settings.gameplay.tutorial = *value;
	if (const auto value = read_bool(document, {"gameplay", "auto_acceleration"}))
		settings.gameplay.auto_acceleration = *value;
	if (const auto value = read_bool(document, {"gameplay", "camera_rotation"}))
		settings.gameplay.camera_rotation = *value;
	if (const auto value = read_bool(document, {"gameplay", "camera_slope"}))
		settings.gameplay.camera_slope = *value;
	if (const auto value = read_bool(document, {"gameplay", "camera_zoom"}))
		settings.gameplay.camera_zoom = *value;

	if (const auto value = read_string(document, {"network", "player_name"}))
		settings.network.player_name = *value;
	if (const auto value = read_int(document, {"network", "player_color"}))
		settings.network.player_color = *value;
	if (const auto value = read_string(document, {"network", "player_password"}))
		settings.network.player_password = *value;
	if (const auto value = read_string(document, {"network", "server"}))
		settings.network.server = *value;
	if (const auto value = read_int(document, {"network", "port"}))
		settings.network.port = *value;
	if (const auto value = read_bool(document, {"network", "proxy_enabled"}))
		settings.network.proxy_enabled = *value;
	if (const auto value = read_string(document, {"network", "proxy_server"}))
		settings.network.proxy_server = *value;
	if (const auto value = read_int(document, {"network", "proxy_port"}))
		settings.network.proxy_port = *value;

	load_bindings(document, {"input", "keyboard", "bindings"}, settings.input.keyboard.bindings);
	if (const auto value = read_bool(document, {"input", "controller", "enabled"}))
		settings.input.controller.enabled = *value;
	if (const auto value = read_float(document, {"input", "controller", "cursor_speed"}))
		settings.input.controller.cursor_speed = *value;
	if (const auto value = read_bool(document, {"input", "controller", "rumble"}))
		settings.input.controller.rumble = *value;
	if (const auto value = read_float(document, {"input", "sdl_gamepad", "stick_deadzone"}))
		settings.input.sdl_gamepad.stick_deadzone = *value;
	if (const auto value = read_float(document, {"input", "sdl_gamepad", "trigger_deadzone"}))
		settings.input.sdl_gamepad.trigger_deadzone = *value;
	load_bindings(
		document, {"input", "sdl_gamepad", "bindings"}, settings.input.sdl_gamepad.bindings
	);

	const TomlValue *axes = find_value(document, {"input", "sdl_gamepad", "axes"});
	if (axes && axes->is_table()) {
		for (auto &[name, binding] : settings.input.sdl_gamepad.axes) {
			if (!axes->contains(name) || !axes->at(name).is_table())
				continue;
			const TomlValue &axis = axes->at(name);
			if (const auto value = read_string(axis, {"axis"}))
				binding.axis = *value;
			if (const auto value = read_bool(axis, {"inverted"}))
				binding.inverted = *value;
		}
	}

	normalize_settings(settings);
	return settings;
}

TomlValue &ensure_table(TomlValue &parent, const std::string &key, const char *comment = nullptr) {
	if (!parent.is_table())
		parent = TomlTable{};
	if (!parent.contains(key) || !parent.at(key).is_table()) {
		TomlValue table(TomlTable{});
		if (comment)
			table.comments().push_back(comment);
		parent[key] = std::move(table);
	}
	return parent[key];
}

void set_value(
	TomlValue &table,
	const std::string &key,
	TomlValue value,
	const char *comment = nullptr
) {
	if (table.contains(key)) {
		value.comments() = table.at(key).comments();
	} else if (comment) {
		value.comments().push_back(comment);
	}
	table[key] = std::move(value);
}

TomlValue bindings_value(const BindingList &bindings) {
	TomlArray array;
	for (const std::string &binding : bindings)
		array.emplace_back(binding);
	return TomlValue(std::move(array));
}

void update_bindings(TomlValue &table, const BindingMap &bindings) {
	for (const auto &[action, values] : bindings)
		set_value(table, action, bindings_value(values));
}

void encode_settings(TomlValue &document, const GameSettings &settings) {
	if (!document.is_table())
		document = TomlTable{};
	set_value(
		document,
		"format_version",
		TomlValue(SETTINGS_FORMAT_VERSION),
		"Settings schema version. Newer versions are not overwritten by older builds."
	);

	TomlValue &video = ensure_table(document, "video", "Display and rendering settings.");
	set_value(video, "fullscreen", TomlValue(settings.video.fullscreen));
	set_value(
		video,
		"resolution",
		TomlValue(settings.video.resolution == ResolutionMode::Desktop ? "desktop" : "800x600")
	);
	set_value(video, "fps", TomlValue(settings.video.fps), "Supported values: 20 or 60.");
	set_value(video, "detail", TomlValue(settings.video.detail));
	set_value(video, "keep_terrain_changes", TomlValue(settings.video.keep_terrain_changes));
	set_value(video, "destroy_terrain", TomlValue(settings.video.destroy_terrain));

	TomlValue &audio = ensure_table(document, "audio", "Sound and music settings.");
	set_value(audio, "sound_enabled", TomlValue(settings.audio.sound_enabled));
	set_value(audio, "sound_volume", TomlValue(settings.audio.sound_volume));
	set_value(audio, "music_enabled", TomlValue(settings.audio.music_enabled));
	set_value(audio, "music_volume", TomlValue(settings.audio.music_volume));
	set_value(audio, "panning", TomlValue(settings.audio.panning));
	set_value(audio, "engine_noise", TomlValue(settings.audio.engine_noise));
	set_value(audio, "background_sound", TomlValue(settings.audio.background_sound));

	TomlValue &gameplay = ensure_table(document, "gameplay", "General gameplay preferences.");
	set_value(gameplay, "tutorial", TomlValue(settings.gameplay.tutorial));
	set_value(gameplay, "auto_acceleration", TomlValue(settings.gameplay.auto_acceleration));
	set_value(gameplay, "camera_rotation", TomlValue(settings.gameplay.camera_rotation));
	set_value(gameplay, "camera_slope", TomlValue(settings.gameplay.camera_slope));
	set_value(gameplay, "camera_zoom", TomlValue(settings.gameplay.camera_zoom));

	TomlValue &network = ensure_table(document, "network", "Player identity and server settings.");
	set_value(network, "player_name", TomlValue(settings.network.player_name));
	set_value(network, "player_color", TomlValue(settings.network.player_color));
	set_value(network, "player_password", TomlValue(settings.network.player_password));
	set_value(network, "server", TomlValue(settings.network.server));
	set_value(network, "port", TomlValue(settings.network.port));
	set_value(network, "proxy_enabled", TomlValue(settings.network.proxy_enabled));
	set_value(network, "proxy_server", TomlValue(settings.network.proxy_server));
	set_value(network, "proxy_port", TomlValue(settings.network.proxy_port));

	TomlValue &input = ensure_table(document, "input", "Input settings use stable action names.");
	TomlValue &keyboard = ensure_table(input, "keyboard");
	TomlValue &keyboard_bindings = ensure_table(keyboard, "bindings");
	update_bindings(keyboard_bindings, settings.input.keyboard.bindings);

	TomlValue &controller = ensure_table(input, "controller");
	set_value(controller, "enabled", TomlValue(settings.input.controller.enabled));
	set_value(
		controller,
		"cursor_speed",
		TomlValue(static_cast<double>(settings.input.controller.cursor_speed))
	);
	set_value(controller, "rumble", TomlValue(settings.input.controller.rumble));

	TomlValue &sdl_gamepad = ensure_table(input, "sdl_gamepad");
	set_value(
		sdl_gamepad,
		"stick_deadzone",
		TomlValue(static_cast<double>(settings.input.sdl_gamepad.stick_deadzone))
	);
	set_value(
		sdl_gamepad,
		"trigger_deadzone",
		TomlValue(static_cast<double>(settings.input.sdl_gamepad.trigger_deadzone))
	);
	TomlValue &axes = ensure_table(sdl_gamepad, "axes");
	for (const auto &[name, binding] : settings.input.sdl_gamepad.axes) {
		TomlValue &axis = ensure_table(axes, name);
		axis.as_table_fmt(std::nothrow).fmt = toml::table_format::oneline;
		set_value(axis, "axis", TomlValue(binding.axis));
		set_value(axis, "inverted", TomlValue(binding.inverted));
	}
	TomlValue &gamepad_bindings = ensure_table(sdl_gamepad, "bindings");
	update_bindings(gamepad_bindings, settings.input.sdl_gamepad.bindings);
}

TomlValue new_document(const GameSettings &settings) {
	TomlValue document(TomlTable{});
	document.comments().push_back(
		"Vangers user settings. Legacy .dat files are kept for old game versions."
	);
	encode_settings(document, settings);
	return document;
}

bool replace_file(const std::filesystem::path &temporary, const std::filesystem::path &target) {
#ifdef _WIN32
	return MoveFileExW(
			   temporary.c_str(), target.c_str(), MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH
		   ) != 0;
#else
	return std::rename(temporary.c_str(), target.c_str()) == 0;
#endif
}

bool atomic_write(const std::filesystem::path &path, const std::string &contents) {
	std::filesystem::path temporary = path;
	temporary += ".tmp";
	std::error_code error;
	std::filesystem::remove(temporary, error);

	std::ofstream output(temporary, std::ios::binary | std::ios::trunc);
	if (!output)
		return false;
	output.write(contents.data(), static_cast<std::streamsize>(contents.size()));
	output.flush();
	const bool write_ok = output.good();
	output.close();
	if (!write_ok || output.fail()) {
		std::filesystem::remove(temporary, error);
		return false;
	}
	if (!replace_file(temporary, path)) {
		std::filesystem::remove(temporary, error);
		return false;
	}
	return true;
}

std::string timestamp() {
	const std::time_t now = std::time(nullptr);
	std::tm local{};
#ifdef _WIN32
	localtime_s(&local, &now);
#else
	localtime_r(&now, &local);
#endif
	char buffer[32];
	std::strftime(buffer, sizeof(buffer), "%Y%m%d-%H%M%S", &local);
	return buffer;
}

std::filesystem::path available_backup_path(const std::filesystem::path &settings_path) {
	std::filesystem::path candidate = settings_path;
	candidate += ".broken-" + timestamp();
	for (int suffix = 1; std::filesystem::exists(candidate); ++suffix) {
		candidate = settings_path;
		candidate += ".broken-" + timestamp() + "-" + std::to_string(suffix);
	}
	return candidate;
}

std::string format_errors(const std::vector<toml::error_info> &errors) {
	std::string message;
	for (const toml::error_info &error : errors)
		message += toml::format_error(error);
	return message;
}

} // namespace

struct SettingsManager::Impl {
	explicit Impl(SettingsPaths settings_paths): paths(std::move(settings_paths)) {}

	SettingsPaths paths;
	GameSettings settings = default_settings();
	std::optional<TomlValue> document;
	SettingsLoadResult last_result;
	bool loaded = false;
	bool read_only = false;
};

SettingsManager::SettingsManager(SettingsPaths paths)
	: impl_(std::make_unique<Impl>(std::move(paths))) {}

SettingsManager::~SettingsManager() = default;
SettingsManager::SettingsManager(SettingsManager &&) noexcept = default;
SettingsManager &SettingsManager::operator=(SettingsManager &&) noexcept = default;

SettingsLoadResult SettingsManager::load() {
	if (impl_->loaded)
		return impl_->last_result;
	impl_->loaded = true;
	impl_->settings = default_settings();

	if (!std::filesystem::exists(impl_->paths.settings_file)) {
		impl_->document = new_document(impl_->settings);
		impl_->last_result.source = SettingsLoadSource::Defaults;
		impl_->last_result.settings_file_written = save();
		if (!impl_->last_result.settings_file_written)
			impl_->last_result.diagnostic = "cannot create settings.toml";
		return impl_->last_result;
	}

	auto parsed = toml::try_parse<toml::ordered_type_config>(impl_->paths.settings_file);
	if (parsed.is_err()) {
		impl_->last_result.source = SettingsLoadSource::RecoveredDefaults;
		impl_->last_result.diagnostic = format_errors(parsed.unwrap_err());
		std::cerr << "Cannot parse " << impl_->paths.settings_file << ":\n"
				  << impl_->last_result.diagnostic;

		const std::filesystem::path backup = available_backup_path(impl_->paths.settings_file);
		std::error_code error;
		if (!std::filesystem::copy_file(
				impl_->paths.settings_file, backup, std::filesystem::copy_options::none, error
			)) {
			impl_->read_only = true;
			impl_->last_result.read_only = true;
			impl_->last_result.diagnostic +=
				"cannot preserve broken settings file: " + error.message();
			return impl_->last_result;
		}

		impl_->document = new_document(impl_->settings);
		impl_->last_result.settings_file_written = save();
		if (!impl_->last_result.settings_file_written) {
			impl_->read_only = true;
			impl_->last_result.read_only = true;
			impl_->last_result.diagnostic += "cannot replace broken settings file";
		}
		return impl_->last_result;
	}

	impl_->document = std::move(parsed.unwrap());
	impl_->settings = decode_settings(*impl_->document);
	impl_->last_result.source = SettingsLoadSource::Toml;
	const int version = read_int(*impl_->document, {"format_version"}).value_or(0);
	if (version > SETTINGS_FORMAT_VERSION) {
		impl_->read_only = true;
		impl_->last_result.read_only = true;
		impl_->last_result.diagnostic = "settings.toml was created by a newer game version";
	} else if (version < SETTINGS_FORMAT_VERSION) {
		impl_->last_result.settings_file_written = save();
	}
	return impl_->last_result;
}

bool SettingsManager::save() {
	if (impl_->read_only)
		return false;
	normalize_settings(impl_->settings);
	if (!impl_->document)
		impl_->document = new_document(impl_->settings);
	else
		encode_settings(*impl_->document, impl_->settings);
	try {
		return atomic_write(impl_->paths.settings_file, toml::format(*impl_->document));
	} catch (const std::exception &error) {
		std::cerr << "Cannot serialize " << impl_->paths.settings_file << ": " << error.what()
				  << '\n';
		return false;
	}
}

const GameSettings &SettingsManager::get() const {
	return impl_->settings;
}

GameSettings &SettingsManager::get_mutable() {
	return impl_->settings;
}

const SettingsPaths &SettingsManager::paths() const {
	return impl_->paths;
}

bool SettingsManager::is_loaded() const {
	return impl_->loaded;
}

bool SettingsManager::can_save() const {
	return !impl_->read_only;
}

SettingsManager &settings_manager() {
	static SettingsManager manager;
	return manager;
}

} // namespace vangers::settings
