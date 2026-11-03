#include "text.h"
#include "settings/text_encoding.h"

#include <array>
#include <cstdint>
#include <fstream>
#include <stdexcept>

namespace vangers::saves {
namespace {
bool valid(std::string_view value) {
	return value.find('\0') == std::string_view::npos && settings::is_valid_utf8(value);
}
} // namespace
int text_header(std::string_view name, std::string_view description) {
	if (name.size() > 255 || description.size() > 255 || !valid(name) || !valid(description))
		throw std::invalid_argument(
			"Save metadata must be valid UTF-8 and at most 255 bytes per field"
		);
	return int(name.size() | (description.size() << 16)) | utf8_text;
}

std::optional<Text> read_text(const char *filename) {
	std::ifstream file(filename, std::ios::binary);
	std::array<unsigned char, 4> bytes{};
	if (!file.read(reinterpret_cast<char *>(bytes.data()), bytes.size()))
		return {};
	const auto header = uint32_t(bytes[0]) | (uint32_t(bytes[1]) << 8) |
						(uint32_t(bytes[2]) << 16) | (uint32_t(bytes[3]) << 24);
	Text text{std::string(header & 255, '\0'), std::string((header >> 16) & 255, '\0')};
	if (!file.read(text.name.data(), text.name.size()) ||
		!file.read(text.description.data(), text.description.size()))
		return {};
	if (header & utf8_text) {
		if (!valid(text.name) || !valid(text.description))
			return {};
	} else {
		text.name = settings::cp866_to_utf8(text.name);
		text.description = settings::cp866_to_utf8(text.description);
	}
	return text;
}
} // namespace vangers::saves
