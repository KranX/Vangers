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
constexpr std::array<char32_t, 128> CP1251_HIGH_CHARACTERS = {
	0x0402, 0x0403, 0x201a, 0x0453, 0x201e, 0x2026, 0x2020, 0x2021,
	0x20ac, 0x2030, 0x0409, 0x2039, 0x040a, 0x040c, 0x040b, 0x040f,
	0x0452, 0x2018, 0x2019, 0x201c, 0x201d, 0x2022, 0x2013, 0x2014,
	0xfffd, 0x2122, 0x0459, 0x203a, 0x045a, 0x045c, 0x045b, 0x045f,
	0x00a0, 0x040e, 0x045e, 0x0408, 0x00a4, 0x0490, 0x00a6, 0x00a7,
	0x0401, 0x00a9, 0x0404, 0x00ab, 0x00ac, 0x00ad, 0x00ae, 0x0407,
	0x00b0, 0x00b1, 0x0406, 0x0456, 0x0491, 0x00b5, 0x00b6, 0x00b7,
	0x0451, 0x2116, 0x0454, 0x00bb, 0x0458, 0x0405, 0x0455, 0x0457,
	0x0410, 0x0411, 0x0412, 0x0413, 0x0414, 0x0415, 0x0416, 0x0417,
	0x0418, 0x0419, 0x041a, 0x041b, 0x041c, 0x041d, 0x041e, 0x041f,
	0x0420, 0x0421, 0x0422, 0x0423, 0x0424, 0x0425, 0x0426, 0x0427,
	0x0428, 0x0429, 0x042a, 0x042b, 0x042c, 0x042d, 0x042e, 0x042f,
	0x0430, 0x0431, 0x0432, 0x0433, 0x0434, 0x0435, 0x0436, 0x0437,
	0x0438, 0x0439, 0x043a, 0x043b, 0x043c, 0x043d, 0x043e, 0x043f,
	0x0440, 0x0441, 0x0442, 0x0443, 0x0444, 0x0445, 0x0446, 0x0447,
	0x0448, 0x0449, 0x044a, 0x044b, 0x044c, 0x044d, 0x044e, 0x044f,
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

// Legacy Diagen resources use Windows-1251, including in the English archive's
// shared room/name table. Undefined byte 0x98 becomes the Unicode replacement.
std::string cp1251_to_utf8(std::string_view text) {
	std::string output;
	output.reserve(text.size() * 2);
	for (const unsigned char value : text)
		append_utf8(output, value < 128 ? value : CP1251_HIGH_CHARACTERS[value - 128]);
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
