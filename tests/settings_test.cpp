#include "settings/settings.h"

#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

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
		!check(first_result.settings_file_written, "default settings were not written"))
		return false;

	GameSettings &changed = first.get_mutable();
	changed.video.fullscreen = true;
	changed.video.resolution = ResolutionMode::Desktop;
	changed.video.fps = 60;
	changed.audio.sound_volume = 17;
	changed.network.player_name = "Tester";
	changed.input.keyboard.bindings["fire_all"] = {"space"};
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
			   !std::filesystem::exists(paths.settings_file.string() + ".tmp"),
			   "temporary settings file remained"
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

	SettingsManager manager(paths);
	const SettingsLoadResult result = manager.load();
	if (!check(
			result.source == SettingsLoadSource::RecoveredDefaults, "broken TOML was not recovered"
		) ||
		!check(result.settings_file_written, "replacement TOML was not written") ||
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

} // namespace

int main() {
	return test_defaults_and_round_trip() && test_comments_unknown_keys_and_normalization() &&
				   test_future_version_is_read_only() && test_malformed_file_recovery() &&
				   test_old_version_is_upgraded()
			   ? 0
			   : 1;
}
