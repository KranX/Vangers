#include "text_encoding.h"

#include <array>

namespace vangers::settings {
namespace {

// clang-format off
constexpr std::array<char32_t, 128> CP866_HIGH_CHARACTERS = {
	U'А', U'Б', U'В', U'Г', U'Д', U'Е', U'Ж', U'З', U'И', U'Й', U'К', U'Л', U'М', U'Н', U'О', U'П',
	U'Р', U'С', U'Т', U'У', U'Ф', U'Х', U'Ц', U'Ч', U'Ш', U'Щ', U'Ъ', U'Ы', U'Ь', U'Э', U'Ю', U'Я',
	U'а', U'б', U'в', U'г', U'д', U'е', U'ж', U'з', U'и', U'й', U'к', U'л', U'м', U'н', U'о', U'п',
	U'░', U'▒', U'▓', U'│', U'┤', U'╡', U'╢', U'╖', U'╕', U'╣', U'║', U'╗', U'╝', U'╜', U'╛', U'┐',
	U'└', U'┴', U'┬', U'├', U'─', U'┼', U'╞', U'╟', U'╚', U'╔', U'╩', U'╦', U'╠', U'═', U'╬', U'╧',
	U'╨', U'╤', U'╥', U'╙', U'╘', U'╒', U'╓', U'╫', U'╪', U'┘', U'┌', U'█', U'▄', U'▌', U'▐', U'▀',
	U'р', U'с', U'т', U'у', U'ф', U'х', U'ц', U'ч', U'ш', U'щ', U'ъ', U'ы', U'ь', U'э', U'ю', U'я',
	U'Ё', U'ё', U'Є', U'є', U'Ї', U'ї', U'Ў', U'ў', U'°', U'∙', U'·', U'√', U'№', U'¤', U'■', U' ',
};
// clang-format on

void append_utf8(std::string &output, char32_t codepoint) {
	if (codepoint <= 0x7f) {
		output.push_back(static_cast<char>(codepoint));
	} else if (codepoint <= 0x7ff) {
		output.push_back(static_cast<char>(0xc0 | (codepoint >> 6)));
		output.push_back(static_cast<char>(0x80 | (codepoint & 0x3f)));
	} else if (codepoint <= 0xffff) {
		output.push_back(static_cast<char>(0xe0 | (codepoint >> 12)));
		output.push_back(static_cast<char>(0x80 | ((codepoint >> 6) & 0x3f)));
		output.push_back(static_cast<char>(0x80 | (codepoint & 0x3f)));
	} else {
		output.push_back(static_cast<char>(0xf0 | (codepoint >> 18)));
		output.push_back(static_cast<char>(0x80 | ((codepoint >> 12) & 0x3f)));
		output.push_back(static_cast<char>(0x80 | ((codepoint >> 6) & 0x3f)));
		output.push_back(static_cast<char>(0x80 | (codepoint & 0x3f)));
	}
}

bool is_continuation(std::uint8_t value) {
	return (value & 0xc0) == 0x80;
}

bool decode_utf8(std::string_view input, std::size_t &offset, char32_t &codepoint) {
	const std::size_t first_offset = offset;
	const std::uint8_t first = static_cast<std::uint8_t>(input[offset++]);
	if (first <= 0x7f) {
		codepoint = first;
		return true;
	}

	std::size_t length = 0;
	char32_t value = 0;
	char32_t minimum = 0;
	if (first >= 0xc2 && first <= 0xdf) {
		length = 2;
		value = first & 0x1f;
		minimum = 0x80;
	} else if (first >= 0xe0 && first <= 0xef) {
		length = 3;
		value = first & 0x0f;
		minimum = 0x800;
	} else if (first >= 0xf0 && first <= 0xf4) {
		length = 4;
		value = first & 0x07;
		minimum = 0x10000;
	} else {
		codepoint = U'?';
		return false;
	}

	if (first_offset + length > input.size()) {
		codepoint = U'?';
		return false;
	}
	for (std::size_t index = 1; index < length; ++index) {
		const std::uint8_t next = static_cast<std::uint8_t>(input[first_offset + index]);
		if (!is_continuation(next)) {
			codepoint = U'?';
			return false;
		}
		value = (value << 6) | (next & 0x3f);
	}
	if (value < minimum || value > 0x10ffff || (value >= 0xd800 && value <= 0xdfff)) {
		codepoint = U'?';
		return false;
	}
	offset = first_offset + length;
	codepoint = value;
	return true;
}

} // namespace

char32_t cp866_to_unicode(std::uint8_t value) {
	if (value < 0x80)
		return value;
	return CP866_HIGH_CHARACTERS[value - 0x80];
}

std::uint8_t unicode_to_cp866(char32_t value, std::uint8_t replacement) {
	if (value < 0x80)
		return static_cast<std::uint8_t>(value);
	for (std::size_t index = 0; index < CP866_HIGH_CHARACTERS.size(); ++index) {
		if (CP866_HIGH_CHARACTERS[index] == value)
			return static_cast<std::uint8_t>(index + 0x80);
	}
	return replacement;
}

std::string cp866_to_utf8(std::string_view text) {
	std::string output;
	output.reserve(text.size() * 2);
	for (const unsigned char value : text)
		append_utf8(output, cp866_to_unicode(value));
	return output;
}

std::string utf8_to_cp866(std::string_view text, std::uint8_t replacement) {
	std::string output;
	output.reserve(text.size());
	std::size_t offset = 0;
	while (offset < text.size()) {
		char32_t codepoint = U'?';
		decode_utf8(text, offset, codepoint);
		output.push_back(static_cast<char>(unicode_to_cp866(codepoint, replacement)));
	}
	return output;
}

bool is_valid_utf8(std::string_view text) {
	std::size_t offset = 0;
	while (offset < text.size()) {
		char32_t codepoint = 0;
		if (!decode_utf8(text, offset, codepoint))
			return false;
	}
	return true;
}

} // namespace vangers::settings
