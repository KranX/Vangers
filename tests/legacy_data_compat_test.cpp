#include "controls.h"
#include "iscreen_options.h"
#include "xtcore.h"

#include <cstdint>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include <SDL3/SDL_scancode.h>
#include <zlib.h>

namespace {

constexpr std::uint32_t ACI_SAVE_COMPRESSED = 1;

bool check(bool condition, const std::string &message) {
	if (!condition)
		std::cerr << "FAIL: " << message << '\n';
	return condition;
}

std::vector<std::uint8_t> read_file(const std::filesystem::path &path) {
	std::ifstream stream(path, std::ios::binary);
	if (!stream)
		return {};
	return {
		std::istreambuf_iterator<char>(stream),
		std::istreambuf_iterator<char>(),
	};
}

std::uint32_t read_u32(const std::vector<std::uint8_t> &data, std::size_t offset) {
	return static_cast<std::uint32_t>(data[offset]) |
		   (static_cast<std::uint32_t>(data[offset + 1]) << 8) |
		   (static_cast<std::uint32_t>(data[offset + 2]) << 16) |
		   (static_cast<std::uint32_t>(data[offset + 3]) << 24);
}

bool is_valid_saved_control_code(std::uint32_t code) {
	constexpr std::uint32_t joystickButtonMask = SDLK_JOYSTICK_BUTTON_MASK;
	constexpr std::uint32_t gamepadButtonMask = SDLK_GAMEPAD_BUTTON_MASK;
	constexpr std::uint32_t joystickHatMask = SDLK_JOYSTICK_HAT_MASK;
	constexpr std::uint32_t scancodeMask = SDLK_SCANCODE_MASK;
	constexpr std::uint32_t categoryMask =
		joystickButtonMask | gamepadButtonMask | joystickHatMask | scancodeMask;

	const std::uint32_t category = code & categoryMask;
	const std::uint32_t payload = code ^ category;
	if (category == 0)
		return code < SDL_SCANCODE_COUNT;
	if (category == joystickButtonMask)
		return payload <= UINT8_MAX;
	if (category == gamepadButtonMask)
		return payload < SDL_GAMEPAD_BUTTON_COUNT;
	if (category == joystickHatMask)
		return payload <= UINT8_MAX * 10u + SDL_HAT_LEFTUP;
	if (category == scancodeMask)
		return payload < SDL_SCANCODE_COUNT;
	return false;
}

bool test_control_code_validation() {
	return check(is_valid_saved_control_code(0), "unassigned control was rejected") &&
		   check(
			   is_valid_saved_control_code(SDL_SCANCODE_LCTRL), "keyboard scancode was rejected"
		   ) &&
		   check(
			   is_valid_saved_control_code(SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_LCTRL)),
			   "scancode-derived keycode was rejected"
		   ) &&
		   check(
			   is_valid_saved_control_code(SDLK_JOYSTICK_BUTTON_MASK | 19),
			   "joystick button binding was rejected"
		   ) &&
		   check(
			   is_valid_saved_control_code(
				   SDLK_GAMEPAD_BUTTON_MASK | SDL_GAMEPAD_BUTTON_DPAD_RIGHT
			   ),
			   "gamepad button binding was rejected"
		   ) &&
		   check(
			   is_valid_saved_control_code(SDLK_JOYSTICK_HAT_MASK | (2 * 10 + SDL_HAT_LEFTUP)),
			   "joystick hat binding was rejected"
		   ) &&
		   check(
			   !is_valid_saved_control_code(SDLK_GAMEPAD_BUTTON_MASK | SDL_GAMEPAD_BUTTON_COUNT),
			   "out-of-range gamepad button was accepted"
		   ) &&
		   check(
			   !is_valid_saved_control_code(SDLK_JOYSTICK_BUTTON_MASK | SDLK_GAMEPAD_BUTTON_MASK),
			   "control code with multiple categories was accepted"
		   );
}

bool test_controls(const std::filesystem::path &dataDirectory) {
	const auto controls = read_file(dataDirectory / "controls.dat");
	const std::size_t expectedSize =
		2 * sizeof(std::uint32_t) +
		static_cast<std::size_t>(iKEY_MAX_ID) * iKEY_OBJECT_SIZE * sizeof(std::uint32_t);
	if (!check(controls.size() == expectedSize, "legacy controls.dat has an unexpected size"))
		return false;
	if (!check(read_u32(controls, 0) == iKEY_MAX_ID, "legacy control count is incompatible"))
		return false;
	if (!check(
			read_u32(controls, 4) == iKEY_OBJECT_SIZE,
			"legacy controls-per-action count is incompatible"
		))
		return false;

	for (std::size_t offset = 8; offset < controls.size(); offset += sizeof(std::uint32_t)) {
		const std::uint32_t code = read_u32(controls, offset);
		if (!check(
				is_valid_saved_control_code(code),
				"legacy controls.dat contains an invalid control code"
			))
			return false;
	}
	return true;
}

bool test_options(const std::filesystem::path &dataDirectory) {
	const auto options = read_file(dataDirectory / "options.dat");
	if (!check(options.size() >= 5 * sizeof(std::uint32_t), "legacy options.dat is truncated"))
		return false;
	if (!check(read_u32(options, 0) == iMAX_OPTION_ID, "legacy option count is incompatible"))
		return false;

	const std::size_t tail = options.size() - 4 * sizeof(std::uint32_t);
	const std::uint32_t fullscreen = read_u32(options, tail);
	const std::uint32_t autoAcceleration = read_u32(options, tail + 4);
	const std::uint32_t fps60 = read_u32(options, tail + 8);
	const std::uint32_t repeatedAutoAcceleration = read_u32(options, tail + 12);
	return check(fullscreen <= 1, "legacy fullscreen option is invalid") &&
		   check(autoAcceleration <= 1, "legacy auto-acceleration option is invalid") &&
		   check(fps60 <= 1, "legacy FPS option is invalid") &&
		   check(
			   autoAcceleration == repeatedAutoAcceleration,
			   "legacy repeated auto-acceleration value is inconsistent"
		   );
}

bool test_save(const std::filesystem::path &path) {
	const auto save = read_file(path);
	if (!check(save.size() >= 10, path.string() + " is truncated"))
		return false;

	const std::uint32_t header = read_u32(save, 0);
	const std::size_t nameLength = header & 0xff;
	const std::size_t timeLength = (header >> 16) & 0xff;
	const std::uint32_t flags = header >> 24;
	const std::size_t payloadOffset = sizeof(std::uint32_t) + nameLength + timeLength;
	if (!check(nameLength > 0 && timeLength > 0, path.string() + " has an invalid header"))
		return false;
	if (!check(payloadOffset + 6 <= save.size(), path.string() + " has an invalid payload offset"))
		return false;

	if (!(flags & ACI_SAVE_COMPRESSED))
		return check(save.size() > payloadOffset, path.string() + " has an empty save payload");
	if (!check(
			save[payloadOffset] == 8 && save[payloadOffset + 1] == 0,
			path.string() + " uses an unsupported save compression method"
		))
		return false;

	const std::uint32_t unpackedSize = read_u32(save, payloadOffset + 2);
	if (!check(unpackedSize > 0, path.string() + " declares an empty uncompressed payload"))
		return false;
	std::vector<std::uint8_t> unpacked(unpackedSize);
	uLongf actualSize = unpacked.size();
	const int status = uncompress(
		unpacked.data(),
		&actualSize,
		save.data() + payloadOffset + 6,
		save.size() - payloadOffset - 6
	);
	return check(status == Z_OK, path.string() + " cannot be decompressed") &&
		   check(actualSize == unpackedSize, path.string() + " decompressed to an unexpected size");
}

bool test_saves(const std::filesystem::path &dataDirectory) {
	const std::filesystem::path saveDirectory = dataDirectory / "savegame";
	std::size_t saveCount = 0;
	for (const auto &entry : std::filesystem::directory_iterator(saveDirectory)) {
		if (!entry.is_regular_file() || entry.path().extension() != ".dat")
			continue;
		++saveCount;
		if (!test_save(entry.path()))
			return false;
	}
	return check(saveCount > 0, "no representative legacy save files were found");
}

bool test_installed_legacy_data(const std::filesystem::path &dataDirectory) {
	const std::filesystem::path controls = dataDirectory / "controls.dat";
	const std::filesystem::path options = dataDirectory / "options.dat";
	const std::filesystem::path saves = dataDirectory / "savegame";
	if (!std::filesystem::exists(controls) && !std::filesystem::exists(options) &&
		!std::filesystem::exists(saves)) {
		std::cout << "Legacy user data is not part of the OSS checkout; "
				  << "runtime compatibility checks were skipped\n";
		return true;
	}

	return check(std::filesystem::is_regular_file(controls), "legacy controls.dat is missing") &&
		   check(std::filesystem::is_regular_file(options), "legacy options.dat is missing") &&
		   check(std::filesystem::is_directory(saves), "legacy savegame directory is missing") &&
		   test_controls(dataDirectory) && test_options(dataDirectory) && test_saves(dataDirectory);
}

} // namespace

int main() {
	static_assert(SDL_SCANCODE_RIGHT == 79);
	static_assert(SDL_SCANCODE_LEFT == 80);
	static_assert(SDL_SCANCODE_DOWN == 81);
	static_assert(SDL_SCANCODE_UP == 82);
	static_assert(SDL_SCANCODE_LSHIFT == 225);
	static_assert(SDL_SCANCODE_RSHIFT == 229);
	static_assert(SDLK_JOYSTICK_BUTTON_MASK == (1 << 28));
	static_assert(SDLK_GAMEPAD_BUTTON_MASK == (1 << 27));
	static_assert(SDLK_JOYSTICK_HAT_MASK == (1 << 26));
	static_assert(SDL_GAMEPAD_BUTTON_SOUTH == 0);
	static_assert(SDL_GAMEPAD_BUTTON_DPAD_RIGHT == 14);

	const std::filesystem::path dataDirectory = TEST_DATA_DIR;
	return test_control_code_validation() && test_installed_legacy_data(dataDirectory) ? 0 : 1;
}
