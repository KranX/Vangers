#include "settings/settings.h"

#include <atomic>
#include <chrono>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

namespace {

using namespace vangers::settings;

class TemporaryDirectory {
  public:
	TemporaryDirectory() {
		const auto stamp = std::chrono::steady_clock::now().time_since_epoch().count();
		path_ = std::filesystem::temp_directory_path() /
				("vangers-settings-test-" + std::to_string(stamp));
		std::filesystem::create_directories(path_);
	}

	~TemporaryDirectory() {
		std::error_code error;
		std::filesystem::remove_all(path_, error);
	}

	const std::filesystem::path &path() const {
		return path_;
	}

  private:
	std::filesystem::path path_;
};

bool check(bool condition, const std::string &message) {
	if (!condition)
		std::cerr << "FAIL: " << message << '\n';
	return condition;
}

std::string read_file(const std::filesystem::path &path) {
	std::ifstream input(path, std::ios::binary);
	return {std::istreambuf_iterator<char>(input), std::istreambuf_iterator<char>()};
}

void write_file(const std::filesystem::path &path, const std::string &contents) {
	std::ofstream output(path, std::ios::binary | std::ios::trunc);
	output << contents;
}

void append_i32(std::vector<std::uint8_t> &bytes, std::int32_t value) {
	const std::uint32_t encoded = static_cast<std::uint32_t>(value);
	bytes.push_back(static_cast<std::uint8_t>(encoded));
	bytes.push_back(static_cast<std::uint8_t>(encoded >> 8));
	bytes.push_back(static_cast<std::uint8_t>(encoded >> 16));
	bytes.push_back(static_cast<std::uint8_t>(encoded >> 24));
}

void append_string(std::vector<std::uint8_t> &bytes, const std::string &value) {
	append_i32(bytes, static_cast<std::int32_t>(value.size()));
	bytes.insert(bytes.end(), value.begin(), value.end());
}

void write_bytes(const std::filesystem::path &path, const std::vector<std::uint8_t> &bytes) {
	std::ofstream output(path, std::ios::binary | std::ios::trunc);
	output.write(reinterpret_cast<const char *>(bytes.data()), bytes.size());
}

bool has_temporary_settings_file(const std::filesystem::path &settings_file) {
	const std::filesystem::path directory = settings_file.parent_path();
	std::error_code error;
	if (!std::filesystem::exists(directory, error) || error)
		return false;
	const std::string prefix = settings_file.filename().string() + ".tmp";
	for (const auto &entry : std::filesystem::directory_iterator(directory)) {
		const std::string filename = entry.path().filename().string();
		if (filename.compare(0, prefix.size(), prefix) == 0)
			return true;
	}
	return false;
}

std::vector<std::uint8_t> legacy_options() {
	std::vector<std::uint8_t> bytes;
	append_i32(bytes, 46);
	for (const int value : {0, 12, 26, 0, 13, 26, 0, 1, 3, 3})
		append_i32(bytes, value);
	append_string(bytes, "temporary-1");
	append_string(bytes, "temporary-2");
	append_string(bytes, "password");
	append_string(bytes, "Legacy player");
	append_i32(bytes, 1); // desktop resolution
	append_string(bytes, "legacy.example.test");
	append_i32(bytes, 1); // keep terrain
	append_i32(bytes, 0); // panning enabled
	append_i32(bytes, 1); // terrain destruction
	append_i32(bytes, 3); // duplicate color
	append_string(bytes, "Legacy player");
	append_i32(bytes, 0); // engine noise enabled
	append_i32(bytes, 0); // background sound enabled
	append_i32(bytes, 0); // obsolete joystick type
	append_i32(bytes, 0); // proxy disabled
	append_string(bytes, "192.1.1.1");
	append_string(bytes, "1080");
	append_string(bytes, "2198");
	append_string(bytes, "Legacy player");
	append_string(bytes, "password");
	append_i32(bytes, 1); // camera rotation
	append_i32(bytes, 1); // camera slope
	append_i32(bytes, 1); // camera zoom
	append_i32(bytes, 1); // fullscreen
	append_i32(bytes, 1); // repeated auto acceleration
	return bytes;
}

std::vector<std::uint8_t> legacy_controls() {
	std::vector<std::uint8_t> bytes;
	append_i32(bytes, 38);
	append_i32(bytes, 2);
	for (int control = 0; control < 38; ++control) {
		append_i32(bytes, control == 1 ? 4 : 0); // SDL_SCANCODE_A for turn left
		append_i32(bytes, 0);
	}
	return bytes;
}

SettingsPaths paths_for(const std::filesystem::path &directory) {
	return {
		directory / "settings.toml",
		directory / "options.dat",
		directory / "controls.dat",
	};
}

bool test_defaults_and_round_trip() {
	TemporaryDirectory directory;
	SettingsManager first(paths_for(directory.path()));
	const SettingsLoadResult first_result = first.load();
	if (!check(
			first_result.source == SettingsLoadSource::Defaults, "defaults source was not used"
		) ||
		!check(first_result.settings_file_written, "default settings were not written") ||
		!check(
			first.get().video.resolution == ResolutionMode::Desktop,
			"fresh installs no longer use the pre-settings desktop resolution"
		) ||
		!check(first.get().video.fps == 20, "fresh-install FPS default changed"))
		return false;

	GameSettings &changed = first.get_mutable();
	changed.video.fullscreen = true;
	changed.video.resolution = ResolutionMode::Desktop;
	changed.video.fps = 60;
	changed.audio.sound_volume = 17;
	changed.network.player_name = "Tester";
	changed.input.keyboard.bindings["fire_all"] = {"space"};
	changed.input.sdl_gamepad.bindings["fire_all"] = {"right_trigger", "south"};
	if (!check(first.save(), "changed settings could not be saved"))
		return false;

	SettingsManager second(paths_for(directory.path()));
	const SettingsLoadResult second_result = second.load();
	const GameSettings &loaded = second.get();
	return check(second_result.source == SettingsLoadSource::Toml, "TOML source was not used") &&
		   check(loaded.video.fullscreen, "fullscreen was not restored") &&
		   check(
			   loaded.video.resolution == ResolutionMode::Desktop, "resolution was not restored"
		   ) &&
		   check(loaded.video.fps == 60, "FPS was not restored") &&
		   check(loaded.audio.sound_volume == 17, "sound volume was not restored") &&
		   check(loaded.network.player_name == "Tester", "player name was not restored") &&
		   check(
			   loaded.input.keyboard.bindings.at("fire_all") == BindingList{"space"},
			   "keyboard binding was not restored"
		   ) &&
		   check(
			   loaded.input.sdl_gamepad.bindings.at("fire_all") ==
				   BindingList{"right_trigger", "south"},
			   "gamepad bindings were not restored"
		   );
}

bool test_comments_unknown_keys_and_normalization() {
	TemporaryDirectory directory;
	const SettingsPaths paths = paths_for(directory.path());
	write_file(
		paths.settings_file,
		"# user comment\n"
		"format_version = 1\n"
		"custom_root = \"keep me\"\n\n"
		"[video]\n"
		"# FPS comment\n"
		"fps = 42\n"
		"fullscreen = true\n"
		"unknown_video = 123\n\n"
		"[audio]\n"
		"sound_volume = 999\n"
		"music_volume = -4\n\n"
		"[network]\n"
		"port = \"invalid\"\n\n"
		"[input.controller]\n"
		"cursor_speed = -2.0\n"
	);

	SettingsManager manager(paths);
	manager.load();
	const GameSettings &settings = manager.get();
	if (!check(settings.video.fps == 20, "invalid FPS was not normalized") ||
		!check(settings.audio.sound_volume == 26, "sound volume was not clamped") ||
		!check(settings.audio.music_volume == 0, "music volume was not clamped") ||
		!check(settings.network.port == 2197, "wrong port type did not use default") ||
		!check(
			settings.input.controller.cursor_speed == 1.0f, "cursor speed did not use default"
		) ||
		!check(manager.save(), "normalized settings could not be saved"))
		return false;

	const std::string saved = read_file(paths.settings_file);
	return check(saved.find("# user comment") != std::string::npos, "root comment was lost") &&
		   check(saved.find("# FPS comment") != std::string::npos, "key comment was lost") &&
		   check(
			   saved.find("custom_root = \"keep me\"") != std::string::npos,
			   "unknown root key was lost"
		   ) &&
		   check(
			   saved.find("unknown_video = 123") != std::string::npos, "unknown table key was lost"
		   ) &&
		   check(
			   !has_temporary_settings_file(paths.settings_file), "temporary settings file remained"
		   );
}

bool test_future_version_is_read_only() {
	TemporaryDirectory directory;
	const SettingsPaths paths = paths_for(directory.path());
	write_file(
		paths.settings_file,
		"format_version = 999\n"
		"future_key = \"must survive\"\n\n"
		"[video]\n"
		"fullscreen = true\n"
	);
	const std::string before = read_file(paths.settings_file);

	SettingsManager manager(paths);
	const SettingsLoadResult result = manager.load();
	const bool fullscreen_was_loaded = manager.get().video.fullscreen;
	manager.get_mutable().video.fullscreen = false;
	return check(result.source == SettingsLoadSource::Toml, "future file was not parsed") &&
		   check(result.read_only, "future file was not marked read-only") &&
		   check(fullscreen_was_loaded, "known future setting was not read") &&
		   check(!manager.save(), "future file was overwritten") &&
		   check(read_file(paths.settings_file) == before, "future file contents changed");
}

bool test_malformed_file_recovery() {
	TemporaryDirectory directory;
	const SettingsPaths paths = paths_for(directory.path());
	write_file(paths.settings_file, "format_version = [ this is not TOML\n");
	write_bytes(paths.legacy_options_file, legacy_options());

	SettingsManager manager(paths);
	const SettingsLoadResult result = manager.load();
	if (!check(
			result.source == SettingsLoadSource::RecoveredDefaults, "broken TOML was not recovered"
		) ||
		!check(result.settings_file_written, "replacement TOML was not written") ||
		!check(
			!manager.get().video.fullscreen,
			"malformed TOML incorrectly fell back to legacy options.dat"
		) ||
		!check(
			result.diagnostic.find("settings.toml") != std::string::npos,
			"parse location was not reported"
		))
		return false;

	bool backup_found = false;
	for (const auto &entry : std::filesystem::directory_iterator(directory.path())) {
		if (entry.path().filename().string().find("settings.toml.broken-") == 0)
			backup_found = true;
	}
	SettingsManager verify(paths);
	return check(backup_found, "broken TOML backup was not created") &&
		   check(
			   verify.load().source == SettingsLoadSource::Toml, "replacement TOML is not readable"
		   );
}

bool test_invalid_utf8_file_recovery() {
	TemporaryDirectory directory;
	const SettingsPaths paths = paths_for(directory.path());
	std::string invalid = "format_version = 1\n[network]\nplayer_name = \"bad";
	invalid.push_back(static_cast<char>(0xff));
	invalid += "name\"\n";
	write_file(paths.settings_file, invalid);

	SettingsManager manager(paths);
	const SettingsLoadResult result = manager.load();
	if (!check(
			result.source == SettingsLoadSource::RecoveredDefaults,
			"invalid UTF-8 TOML was not recovered"
		) ||
		!check(result.settings_file_written, "invalid UTF-8 TOML was not replaced"))
		return false;

	SettingsManager verify(paths);
	return check(
		verify.load().source == SettingsLoadSource::Toml,
		"replacement after invalid UTF-8 is not readable"
	);
}

bool test_old_version_is_upgraded() {
	TemporaryDirectory directory;
	const SettingsPaths paths = paths_for(directory.path());
	write_file(paths.settings_file, "format_version = 0\n[video]\nfps = 60\n");

	SettingsManager manager(paths);
	const SettingsLoadResult result = manager.load();
	const std::string saved = read_file(paths.settings_file);
	return check(result.source == SettingsLoadSource::Toml, "old TOML was not loaded") &&
		   check(result.settings_file_written, "old TOML was not upgraded") &&
		   check(manager.get().video.fps == 60, "old TOML value was not retained") &&
		   check(
			   saved.find("format_version = 1") != std::string::npos,
			   "format version was not updated"
		   );
}

bool test_legacy_migration_is_one_time_and_read_only() {
	TemporaryDirectory directory;
	const SettingsPaths paths = paths_for(directory.path());
	write_bytes(paths.legacy_options_file, legacy_options());
	write_bytes(paths.legacy_controls_file, legacy_controls());
	const std::string options_before = read_file(paths.legacy_options_file);
	const std::string controls_before = read_file(paths.legacy_controls_file);

	SettingsManager first(paths);
	const SettingsLoadResult result = first.load();
	if (!check(result.source == SettingsLoadSource::Legacy, "legacy source was not reported") ||
		!check(result.settings_file_written, "migrated TOML was not written") ||
		!check(first.get().video.fullscreen, "legacy fullscreen was not imported") ||
		!check(
			first.get().video.resolution == ResolutionMode::Desktop,
			"legacy resolution was not imported"
		) ||
		!check(
			first.get().network.player_name == "Legacy player", "legacy name was not imported"
		) ||
		!check(first.get().network.port == 2198, "legacy port was not imported") ||
		!check(
			first.get().input.keyboard.bindings.at("turn_left") == BindingList{"a"},
			"legacy control was not imported"
		) ||
		!check(
			read_file(paths.legacy_options_file) == options_before, "options.dat was modified"
		) ||
		!check(
			read_file(paths.legacy_controls_file) == controls_before, "controls.dat was modified"
		))
		return false;

	// Once TOML exists, even unusable legacy files must never be consulted again.
	write_file(paths.legacy_options_file, "not legacy settings");
	write_file(paths.legacy_controls_file, "not legacy controls");
	SettingsManager second(paths);
	return check(second.load().source == SettingsLoadSource::Toml, "TOML did not win over .dat") &&
		   check(second.get().video.fullscreen, "migrated TOML value was not retained") &&
		   check(
			   second.get().input.keyboard.bindings.at("turn_left") == BindingList{"a"},
			   "migrated TOML control was not retained"
		   );
}

bool test_failed_migration_write_retries_without_touching_legacy() {
	TemporaryDirectory directory;
	SettingsPaths paths = paths_for(directory.path());
	paths.settings_file = directory.path() / "missing-directory" / "settings.toml";
	write_bytes(paths.legacy_options_file, legacy_options());
	const std::string options_before = read_file(paths.legacy_options_file);

	SettingsManager first(paths);
	const SettingsLoadResult first_result = first.load();
	if (!check(first_result.source == SettingsLoadSource::Legacy, "legacy import did not run") ||
		!check(!first_result.settings_file_written, "migration unexpectedly wrote TOML") ||
		!check(first.get().video.fullscreen, "failed write discarded imported settings") ||
		!check(
			read_file(paths.legacy_options_file) == options_before, "failed write changed .dat"
		) ||
		!check(!std::filesystem::exists(paths.settings_file), "partial settings.toml remained") ||
		!check(
			!has_temporary_settings_file(paths.settings_file), "partial temporary file remained"
		))
		return false;

	SettingsManager second(paths);
	const SettingsLoadResult second_result = second.load();
	return check(
			   second_result.source == SettingsLoadSource::Legacy,
			   "failed migration was not retried"
		   ) &&
		   check(second.get().video.fullscreen, "retry did not import legacy settings");
}

bool test_partial_legacy_sources_use_defaults_for_missing_half() {
	TemporaryDirectory directory;
	SettingsPaths paths = paths_for(directory.path());
	write_bytes(paths.legacy_options_file, legacy_options());
	SettingsManager options_only(paths);
	if (!check(
			options_only.load().source == SettingsLoadSource::Legacy,
			"options-only migration was not used"
		) ||
		!check(
			options_only.get().input.keyboard.bindings.at("turn_left") == BindingList{"left"},
			"missing controls did not retain defaults"
		))
		return false;

	TemporaryDirectory controls_directory;
	paths = paths_for(controls_directory.path());
	write_bytes(paths.legacy_controls_file, legacy_controls());
	SettingsManager controls_only(paths);
	return check(
			   controls_only.load().source == SettingsLoadSource::Legacy,
			   "controls-only migration was not used"
		   ) &&
		   check(
			   controls_only.get().network.server == "v5.vangers.net",
			   "missing options did not retain defaults"
		   ) &&
		   check(
			   controls_only.get().input.keyboard.bindings.at("turn_left") == BindingList{"a"},
			   "controls-only binding was not imported"
		   );
}

bool test_concurrent_saves_use_independent_temporary_files() {
	TemporaryDirectory directory;
	const SettingsPaths paths = paths_for(directory.path());
	SettingsManager initial(paths);
	if (!check(initial.load().settings_file_written, "initial settings file was not created"))
		return false;

	constexpr int writer_count = 24;
	std::atomic<int> ready{0};
	std::atomic<int> failures{0};
	std::atomic<bool> start{false};
	std::vector<std::thread> writers;
	writers.reserve(writer_count);
	for (int writer_id = 0; writer_id < writer_count; ++writer_id) {
		writers.emplace_back([&, writer_id] {
			SettingsManager writer(paths);
			if (writer.load().source != SettingsLoadSource::Toml)
				failures.fetch_add(1, std::memory_order_relaxed);
			writer.get_mutable().network.player_name = "writer-" + std::to_string(writer_id);
			ready.fetch_add(1, std::memory_order_release);
			while (!start.load(std::memory_order_acquire))
				std::this_thread::yield();
			std::string diagnostic;
			if (!writer.save(&diagnostic))
				failures.fetch_add(1, std::memory_order_relaxed);
		});
	}
	while (ready.load(std::memory_order_acquire) != writer_count)
		std::this_thread::yield();
	start.store(true, std::memory_order_release);
	for (std::thread &writer : writers)
		writer.join();

	SettingsManager verify(paths);
	const SettingsLoadResult result = verify.load();
	return check(failures.load() == 0, "a concurrent settings save failed") &&
		   check(result.source == SettingsLoadSource::Toml, "concurrent saves damaged TOML") &&
		   check(
			   verify.get().network.player_name.find("writer-") == 0,
			   "concurrent save did not publish a complete document"
		   ) &&
		   check(
			   !has_temporary_settings_file(paths.settings_file),
			   "concurrent save left a temporary file"
		   );
}

bool test_filesystem_errors_are_nonfatal_and_retryable() {
	TemporaryDirectory directory;
	const std::filesystem::path inaccessible = directory.path() / std::string(1024, 'x');

	SettingsPaths paths = paths_for(directory.path());
	paths.settings_file = inaccessible / "settings.toml";
	SettingsManager inaccessible_settings(paths);
	const SettingsLoadResult settings_result = inaccessible_settings.load();
	std::string save_diagnostic;
	if (!check(settings_result.read_only, "settings status error was not made read-only") ||
		!check(!inaccessible_settings.can_save(), "settings status error still permits saving") ||
		!check(!settings_result.diagnostic.empty(), "settings status error has no diagnostic") ||
		!check(
			!inaccessible_settings.save(&save_diagnostic),
			"settings status error unexpectedly allowed a save"
		) ||
		!check(!save_diagnostic.empty(), "read-only save has no diagnostic"))
		return false;

	paths = paths_for(directory.path());
	paths.legacy_options_file = inaccessible / "options.dat";
	SettingsManager inaccessible_legacy(paths);
	const SettingsLoadResult legacy_result = inaccessible_legacy.load();
	return check(legacy_result.read_only, "legacy status error was not made read-only") &&
		   check(
			   !legacy_result.settings_file_written,
			   "legacy status error incorrectly completed migration"
		   ) &&
		   check(!legacy_result.diagnostic.empty(), "legacy status error has no diagnostic") &&
		   check(
			   !std::filesystem::exists(paths.settings_file),
			   "legacy status error created settings.toml"
		   );
}

} // namespace

int main() {
	return test_defaults_and_round_trip() && test_comments_unknown_keys_and_normalization() &&
				   test_future_version_is_read_only() && test_malformed_file_recovery() &&
				   test_invalid_utf8_file_recovery() && test_old_version_is_upgraded() &&
				   test_legacy_migration_is_one_time_and_read_only() &&
				   test_failed_migration_write_retries_without_touching_legacy() &&
				   test_partial_legacy_sources_use_defaults_for_missing_half() &&
				   test_concurrent_saves_use_independent_temporary_files() &&
				   test_filesystem_errors_are_nonfatal_and_retryable()
			   ? 0
			   : 1;
}
