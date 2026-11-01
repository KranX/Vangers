#ifndef VANGERS_TEXT_UTF8_H
#define VANGERS_TEXT_UTF8_H

#include <string>
#include <string_view>

namespace vangers::text {

// Input is valid UTF-8. Boundaries are Unicode scalars, not grapheme clusters.
std::size_t prefix_bytes(std::string_view text, std::size_t characters);
std::size_t text_length(std::string_view text);
void pop_back(std::string &text);

} // namespace vangers::text

#endif
