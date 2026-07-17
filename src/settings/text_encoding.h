#ifndef VANGERS_SETTINGS_TEXT_ENCODING_H
#define VANGERS_SETTINGS_TEXT_ENCODING_H

#include <cstdint>
#include <string>
#include <string_view>

namespace vangers::settings {

constexpr std::uint8_t CP866_REPLACEMENT_CHARACTER = '?';

char32_t cp866_to_unicode(std::uint8_t value);
std::uint8_t
unicode_to_cp866(char32_t value, std::uint8_t replacement = CP866_REPLACEMENT_CHARACTER);

std::string cp866_to_utf8(std::string_view text);
std::string
utf8_to_cp866(std::string_view text, std::uint8_t replacement = CP866_REPLACEMENT_CHARACTER);
bool is_valid_utf8(std::string_view text);

} // namespace vangers::settings

#endif
