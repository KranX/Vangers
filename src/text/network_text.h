#ifndef VANGERS_TEXT_NETWORK_TEXT_H
#define VANGERS_TEXT_NETWORK_TEXT_H

#include "settings/text_encoding.h"
#include "text/utf8.h"
#include <optional>
#include <string>
#include <string_view>

namespace vangers::network_text {

// Exact opt-in on the initial SERVER_TIME_QUERY; echoed after the four-byte time.
inline constexpr std::string_view capability = "VANGERS-UTF8/1";
inline constexpr std::size_t chat_limit = 139; // The server's 140-byte field includes NUL.
inline constexpr std::size_t name_limit = 64;  // At most sixteen four-byte scalars.

// User-generated network text is single-line, never a glyph/icon control stream.
inline std::string single_line(std::string_view text) {
	std::string result;
	result.reserve(text.size());
	while (!text.empty()) {
		const auto scalar = vangers::text::first_scalar(text);
		const auto bytes = vangers::text::prefix_bytes(text, 1);
		if (scalar < 32 || scalar == 127 || scalar == 0x85 || scalar == 0x2028 || scalar == 0x2029)
			result += ' ';
		else
			result += text.substr(0, bytes);
		text.remove_prefix(bytes);
	}
	return result;
}

inline std::optional<std::string> encode(std::string_view text, bool unicode) {
	if (text.find('\0') != text.npos || !settings::is_valid_utf8(text))
		return std::nullopt;
	if (unicode)
		return std::string(text);
	auto encoded = settings::utf8_to_cp866(text);
	if (settings::cp866_to_utf8(encoded) != text)
		return std::nullopt; // '?' is a character, not an implicit fallback.
	return encoded;
}

inline std::optional<std::string> decode(std::string_view bytes, bool unicode) {
	if (bytes.find('\0') != bytes.npos)
		return std::nullopt;
	auto text = unicode ? std::string(bytes) : settings::cp866_to_utf8(bytes);
	if (!settings::is_valid_utf8(text))
		return std::nullopt;
	return single_line(text);
}

} // namespace vangers::network_text

#endif
