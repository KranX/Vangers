#include "legacy_settings_import.h"

#include "input_binding.h"
#include "text_encoding.h"

#include <array>
#include <charconv>
#include <cstdint>
#include <fstream>
#include <limits>
#include <optional>
#include <string_view>
#include <vector>

namespace vangers::settings {
namespace {

constexpr std::size_t MAX_LEGACY_FILE_SIZE = 1024 * 1024;
constexpr std::size_t MAX_LEGACY_STRING_SIZE = 4096;
constexpr int LEGACY_CONTROL_BINDINGS = 2;
constexpr int LEGACY_CONTROL_COUNT_WITHOUT_SCREENSHOT = 37;
constexpr int LEGACY_CONTROL_COUNT_CURRENT = 38;

enum class OptionValueType {
	Integer,
	String,
	NotSaved,
};

constexpr std::array<OptionValueType, 48> OPTION_TYPES = {
	OptionValueType::Integer,  // 0 sound on
	OptionValueType::Integer,  // 1 sound volume
	OptionValueType::Integer,  // 2 sound volume maximum
	OptionValueType::Integer,  // 3 music on
	OptionValueType::Integer,  // 4 music volume
	OptionValueType::Integer,  // 5 music volume maximum
	OptionValueType::Integer,  // 6 tutorial on
	OptionValueType::Integer,  // 7 detail
	OptionValueType::Integer,  // 8 player color duplicate
	OptionValueType::Integer,  // 9 player color
	OptionValueType::String,   // 10 server name (temporary)
	OptionValueType::String,   // 11 server name (temporary)
	OptionValueType::String,   // 12 player password
	OptionValueType::NotSaved, // 13 multiplayer game
	OptionValueType::NotSaved, // 14 multiplayer game
	OptionValueType::NotSaved, // 15 multiplayer game
	OptionValueType::NotSaved, // 16 multiplayer game
	OptionValueType::NotSaved, // 17 multiplayer game
	OptionValueType::NotSaved, // 18 current multiplayer game
	OptionValueType::NotSaved, // 19 current multiplayer game
	OptionValueType::String,   // 20 player name
	OptionValueType::Integer,  // 21 resolution
	OptionValueType::String,   // 22 host
	OptionValueType::NotSaved, // 23 keep status text
	OptionValueType::NotSaved, // 24 keep cleanup text
	OptionValueType::Integer,  // 25 keep mode
	OptionValueType::Integer,  // 26 panning
	OptionValueType::Integer,  // 27 terrain destruction
	OptionValueType::Integer,  // 28 player color duplicate
	OptionValueType::String,   // 29 player name duplicate
	OptionValueType::Integer,  // 30 engine noise
	OptionValueType::Integer,  // 31 background sound
	OptionValueType::Integer,  // 32 obsolete joystick mode
	OptionValueType::Integer,  // 33 proxy enabled
	OptionValueType::String,   // 34 proxy server
	OptionValueType::String,   // 35 proxy port
	OptionValueType::NotSaved, // 36 proxy server UI text
	OptionValueType::NotSaved, // 37 proxy port UI text
	OptionValueType::String,   // 38 server port
	OptionValueType::String,   // 39 player name duplicate
	OptionValueType::String,   // 40 player password duplicate
	OptionValueType::NotSaved, // 41 temporary IP address
	OptionValueType::Integer,  // 42 camera rotation
	OptionValueType::Integer,  // 43 camera slope
	OptionValueType::Integer,  // 44 camera zoom
	OptionValueType::Integer,  // 45 fullscreen
	OptionValueType::Integer,  // 46 auto acceleration or historical FPS
	OptionValueType::Integer,  // 47 FPS
};

class BinaryReader {
  public:
	explicit BinaryReader(std::vector<std::uint8_t> bytes): bytes_(std::move(bytes)) {}

	bool read_i32(std::int32_t &value) {
		if (remaining() < sizeof(std::int32_t))
			return false;
		const std::uint32_t encoded = static_cast<std::uint32_t>(bytes_[offset_]) |
									  (static_cast<std::uint32_t>(bytes_[offset_ + 1]) << 8) |
									  (static_cast<std::uint32_t>(bytes_[offset_ + 2]) << 16) |
									  (static_cast<std::uint32_t>(bytes_[offset_ + 3]) << 24);
		offset_ += sizeof(std::int32_t);
		value = static_cast<std::int32_t>(encoded);
		return true;
	}

	bool read_string(std::string &value) {
		std::int32_t length = 0;
		if (!read_i32(length) || length < 0 ||
			static_cast<std::uint32_t>(length) > MAX_LEGACY_STRING_SIZE ||
			static_cast<std::size_t>(length) > remaining())
			return false;
		value.assign(
			reinterpret_cast<const char *>(bytes_.data() + offset_),
			static_cast<std::size_t>(length)
		);
		offset_ += static_cast<std::size_t>(length);
		return true;
	}

	std::size_t remaining() const {
		return bytes_.size() - offset_;
	}

  private:
	std::vector<std::uint8_t> bytes_;
	std::size_t offset_ = 0;
};

std::optional<std::vector<std::uint8_t>>
read_legacy_file(const std::filesystem::path &path, std::string *diagnostic) {
	std::error_code error;
	const std::uintmax_t file_size = std::filesystem::file_size(path, error);
	if (error || file_size > MAX_LEGACY_FILE_SIZE) {
		if (diagnostic)
			*diagnostic = error ? error.message() : "legacy settings file is too large";
		return std::nullopt;
	}

	std::ifstream input(path, std::ios::binary);
	if (!input) {
		if (diagnostic)
			*diagnostic = "cannot open legacy settings file";
		return std::nullopt;
	}
	std::vector<std::uint8_t> bytes(static_cast<std::size_t>(file_size));
	if (!bytes.empty())
		input.read(
			reinterpret_cast<char *>(bytes.data()), static_cast<std::streamsize>(bytes.size())
		);
	if (!input || input.gcount() != static_cast<std::streamsize>(bytes.size())) {
		if (diagnostic)
			*diagnostic = "cannot read complete legacy settings file";
		return std::nullopt;
	}
	return bytes;
}

bool valid_legacy_bool(std::int32_t value) {
	return value == 0 || value == 1;
}

void assign_bool(bool &target, std::int32_t value, bool inverted = false) {
	if (valid_legacy_bool(value))
		target = inverted ? value == 0 : value != 0;
}

std::optional<int> parse_port(std::string_view text) {
	if (text.empty())
		return std::nullopt;
	int value = 0;
	const auto result = std::from_chars(text.data(), text.data() + text.size(), value);
	if (result.ec != std::errc{} || result.ptr != text.data() + text.size() || value < 1 ||
		value > 65535)
		return std::nullopt;
	return value;
}

void apply_integer_option(
	GameSettings &settings,
	int option_id,
	std::int32_t value,
	std::optional<std::int32_t> &ambiguous_option,
	std::optional<std::int32_t> &fps_option
) {
	switch (option_id) {
	case 0:
		assign_bool(settings.audio.sound_enabled, value, true);
		break;
	case 1:
		settings.audio.sound_volume = value;
		break;
	case 3:
		assign_bool(settings.audio.music_enabled, value, true);
		break;
	case 4:
		settings.audio.music_volume = value;
		break;
	case 6:
		assign_bool(settings.gameplay.tutorial, value, true);
		break;
	case 7:
		settings.video.detail = value;
		break;
	case 9:
		settings.network.player_color = value;
		break;
	case 21:
		if (value == 0)
			settings.video.resolution = ResolutionMode::Legacy800x600;
		else if (value == 1)
			settings.video.resolution = ResolutionMode::Desktop;
		break;
	case 25:
		assign_bool(settings.video.keep_terrain_changes, value);
		break;
	case 26:
		assign_bool(settings.audio.panning, value, true);
		break;
	case 27:
		assign_bool(settings.video.destroy_terrain, value);
		break;
	case 30:
		assign_bool(settings.audio.engine_noise, value, true);
		break;
	case 31:
		assign_bool(settings.audio.background_sound, value, true);
		break;
	case 33:
		assign_bool(settings.network.proxy_enabled, value);
		break;
	case 42:
		assign_bool(settings.gameplay.camera_rotation, value);
		break;
	case 43:
		assign_bool(settings.gameplay.camera_slope, value);
		break;
	case 44:
		assign_bool(settings.gameplay.camera_zoom, value);
		break;
	case 45:
		assign_bool(settings.video.fullscreen, value);
		break;
	case 46:
		ambiguous_option = value;
		break;
	case 47:
		fps_option = value;
		break;
	default:
		break;
	}
}

void apply_string_option(GameSettings &settings, int option_id, const std::string &value) {
	const std::string utf8 = cp866_to_utf8(value);
	switch (option_id) {
	case 12:
		settings.network.player_password = utf8;
		break;
	case 20:
		settings.network.player_name = utf8;
		break;
	case 22:
		settings.network.server = utf8;
		break;
	case 34:
		settings.network.proxy_server = utf8;
		break;
	case 35:
		if (const auto port = parse_port(value))
			settings.network.proxy_port = *port;
		break;
	case 38:
		if (const auto port = parse_port(value))
			settings.network.port = *port;
		break;
	default:
		break;
	}
}

void append_diagnostic(std::string &target, const std::string &source) {
	if (source.empty())
		return;
	if (!target.empty())
		target += "; ";
	target += source;
}

} // namespace

bool import_legacy_options(
	const std::filesystem::path &path,
	GameSettings &settings,
	std::string *diagnostic
) {
	const auto bytes = read_legacy_file(path, diagnostic);
	if (!bytes)
		return false;
	BinaryReader reader(*bytes);
	std::int32_t option_count = 0;
	if (!reader.read_i32(option_count) ||
		(option_count != 46 && option_count != 47 && option_count != 48)) {
		if (diagnostic)
			*diagnostic = "unsupported options.dat version";
		return false;
	}

	GameSettings imported = settings;
	std::optional<std::int32_t> ambiguous_option;
	std::optional<std::int32_t> fps_option;
	for (int option_id = 0; option_id < option_count; ++option_id) {
		switch (OPTION_TYPES[option_id]) {
		case OptionValueType::NotSaved:
			break;
		case OptionValueType::Integer: {
			std::int32_t value = 0;
			if (!reader.read_i32(value)) {
				if (diagnostic)
					*diagnostic = "truncated integer in options.dat";
				return false;
			}
			apply_integer_option(imported, option_id, value, ambiguous_option, fps_option);
			break;
		}
		case OptionValueType::String: {
			std::string value;
			if (!reader.read_string(value)) {
				if (diagnostic)
					*diagnostic = "invalid string in options.dat";
				return false;
			}
			apply_string_option(imported, option_id, value);
			break;
		}
		}
	}

	std::int32_t repeated_auto_acceleration = 0;
	if (!reader.read_i32(repeated_auto_acceleration) || reader.remaining() != 0) {
		if (diagnostic)
			*diagnostic = "invalid options.dat tail";
		return false;
	}
	assign_bool(imported.gameplay.auto_acceleration, repeated_auto_acceleration);

	if (option_count == 47 && ambiguous_option && valid_legacy_bool(*ambiguous_option) &&
		valid_legacy_bool(repeated_auto_acceleration)) {
		// Two historical layouts used the same count. A differing slot 46 is the
		// alpha layout's FPS flag; equal values are the public 2.0 duplicate of
		// auto-acceleration and deliberately leave FPS at its legacy default.
		if (*ambiguous_option != repeated_auto_acceleration)
			imported.video.fps = *ambiguous_option ? 60 : 20;
	} else if (option_count == 48 && fps_option && valid_legacy_bool(*fps_option)) {
		imported.video.fps = *fps_option ? 60 : 20;
	}

	normalize_settings(imported);
	settings = std::move(imported);
	if (diagnostic)
		diagnostic->clear();
	return true;
}

bool import_legacy_controls(
	const std::filesystem::path &path,
	GameSettings &settings,
	std::string *diagnostic
) {
	const auto bytes = read_legacy_file(path, diagnostic);
	if (!bytes)
		return false;
	BinaryReader reader(*bytes);
	std::int32_t control_count = 0;
	std::int32_t bindings_per_control = 0;
	if (!reader.read_i32(control_count) || !reader.read_i32(bindings_per_control) ||
		(control_count != LEGACY_CONTROL_COUNT_WITHOUT_SCREENSHOT &&
			control_count != LEGACY_CONTROL_COUNT_CURRENT) ||
		bindings_per_control != LEGACY_CONTROL_BINDINGS) {
		if (diagnostic)
			*diagnostic = "unsupported controls.dat version";
		return false;
	}

	std::vector<std::array<std::int32_t, LEGACY_CONTROL_BINDINGS>> controls(control_count);
	for (auto &control : controls) {
		for (std::int32_t &binding : control) {
			if (!reader.read_i32(binding)) {
				if (diagnostic)
					*diagnostic = "truncated controls.dat";
				return false;
			}
		}
	}
	if (reader.remaining() != 0) {
		if (diagnostic)
			*diagnostic = "unexpected controls.dat tail";
		return false;
	}

	GameSettings imported = settings;
	for (int control_id = 1; control_id < control_count; ++control_id) {
		const char *action = legacy_control_action_name(control_id);
		if (!action)
			continue;
		BindingList keyboard;
		BindingList gamepad;
		bool has_unsupported_binding = false;
		for (const std::int32_t code : controls[control_id]) {
			const DecodedLegacyBinding binding = decode_legacy_control_code(code);
			if (binding.kind == LegacyBindingKind::Keyboard)
				keyboard.push_back(binding.name);
			else if (binding.kind == LegacyBindingKind::Gamepad)
				gamepad.push_back(binding.name);
			else if (binding.kind == LegacyBindingKind::Unsupported)
				has_unsupported_binding = true;
		}
		// Unknown codes and raw joystick/hat bindings cannot be represented
		// faithfully. Preserve the new default for that action instead of
		// silently turning a usable control into an unbound one.
		if (has_unsupported_binding)
			continue;
		imported.input.keyboard.bindings[action] = std::move(keyboard);
		if (!gamepad.empty())
			imported.input.sdl_gamepad.bindings[action] = std::move(gamepad);
	}

	normalize_settings(imported);
	settings = std::move(imported);
	if (diagnostic)
		diagnostic->clear();
	return true;
}

LegacyImportResult import_legacy_settings(const SettingsPaths &paths, GameSettings &settings) {
	LegacyImportResult result;
	if (std::filesystem::exists(paths.legacy_options_file)) {
		std::string diagnostic;
		result.options_imported =
			import_legacy_options(paths.legacy_options_file, settings, &diagnostic);
		if (!result.options_imported)
			append_diagnostic(result.diagnostic, "options.dat: " + diagnostic);
	}
	if (std::filesystem::exists(paths.legacy_controls_file)) {
		std::string diagnostic;
		result.controls_imported =
			import_legacy_controls(paths.legacy_controls_file, settings, &diagnostic);
		if (!result.controls_imported)
			append_diagnostic(result.diagnostic, "controls.dat: " + diagnostic);
	}
	normalize_settings(settings);
	return result;
}

} // namespace vangers::settings
