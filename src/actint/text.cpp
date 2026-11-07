#include "text.h"

#include <cstring>
#include <stdexcept>

namespace vangers::actint {

std::string decode_text(std::string_view text, TextEncoding encoding) {
	if (encoding == TextEncoding::Cp866)
		return settings::cp866_to_utf8(text);
	if (encoding != TextEncoding::Utf8 || !settings::is_valid_utf8(text))
		throw std::invalid_argument("Invalid actint UTF-8 text");
	return std::string(text);
}

std::string raster_text(std::string_view text, std::size_t glyph_count) {
	// The C-string raster APIs need a printable fallback, not an embedded terminator.
	if (glyph_count <= ' ')
		return {};
	auto bytes = settings::utf8_to_cp866(text);
	const char fallback = glyph_count > '?' ? '?' : ' ';
	for (char &byte : bytes)
		if (static_cast<unsigned char>(byte) >= glyph_count)
			byte = fallback;
	return bytes;
}

char *copy_text(std::string_view text) {
	auto *copy = new char[text.size() + 1];
	if (!text.empty())
		std::memcpy(copy, text.data(), text.size());
	copy[text.size()] = '\0';
	return copy;
}

} // namespace vangers::actint
