#include "text.h"

#include "settings/text_encoding.h"
#include "text/utf8.h"

#include <SDL3/SDL_stdinc.h>
#include <cstring>
#include <stdexcept>

namespace vangers::iscreen {

std::string resource_text(std::string_view bytes, bool utf8) {
	if (!utf8)
		return settings::cp866_to_utf8(bytes);
	if (!settings::is_valid_utf8(bytes))
		throw std::invalid_argument("iscreen resource contains invalid UTF-8");
	return std::string(bytes);
}

std::vector<std::string> text_lines(std::string_view bytes, bool utf8) {
	const auto decoded = resource_text(bytes, utf8);
	std::vector<std::string> lines;
	std::size_t start = 0;
	for (std::size_t pos = 0; pos < decoded.size(); ++pos) {
		if (decoded[pos] != '\n' && decoded[pos] != '\r')
			continue;
		lines.emplace_back(decoded.substr(start, pos - start));
		if (decoded[pos] == '\r' && pos + 1 < decoded.size() && decoded[pos + 1] == '\n')
			++pos;
		start = pos + 1;
	}
	if (start < decoded.size())
		lines.emplace_back(decoded.substr(start));
	return lines;
}

void copy_text(char *destination, std::string_view value, std::size_t max_characters) {
	if (!settings::is_valid_utf8(value))
		throw std::invalid_argument("iscreen expects UTF-8 text");
	const auto count = text::prefix_bytes(value, max_characters);
	std::memmove(destination, value.data(), count);
	destination[count] = 0;
}

bool append_input(std::string &value, std::string_view input, std::size_t maximum, bool numeric) {
	if (!settings::is_valid_utf8(input))
		return false;
	const auto old_size = value.size();
	std::size_t count = text::text_length(value), left = input.size();
	const char *cursor = input.data();
	while (left && count < maximum) {
		const char *begin = cursor;
		const auto cp = SDL_StepUTF8(&cursor, &left);
		if (cp < 32 || cp == 127 || (numeric && (cp < '0' || cp > '9')))
			continue;
		value.append(begin, cursor);
		++count;
	}
	return value.size() != old_size;
}

} // namespace vangers::iscreen
