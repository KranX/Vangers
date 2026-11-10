#ifndef VANGERS_TEXT_UTF8_H
#define VANGERS_TEXT_UTF8_H

#include <string>
#include <string_view>

namespace vangers::text {

// Input is valid UTF-8. Boundaries are Unicode scalars, not grapheme clusters.
std::size_t prefix_bytes(std::string_view text, std::size_t characters);
std::size_t text_length(std::string_view text);
void pop_back(std::string &text);
char32_t first_scalar(std::string_view text);
char32_t last_scalar(std::string_view text);
bool combining_mark(char32_t cp);
// Compose adjacent canonical pairs (e + acute, kana + dakuten), without
// changing stored text. Not a bidi/shaping engine or a full NFC normalizer.
std::string compose_pairs(std::string_view text);

} // namespace vangers::text

#endif
