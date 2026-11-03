#ifndef VANGERS_ISCREEN_TEXT_H
#define VANGERS_ISCREEN_TEXT_H

#include <string>
#include <string_view>
#include <vector>

namespace vangers::iscreen {

// Resource declarations select the encoding; paths and object IDs are opaque.
std::string resource_text(std::string_view bytes, bool utf8);
std::vector<std::string> text_lines(std::string_view bytes, bool utf8);

// Fixed UI buffers reserve four bytes per Unicode scalar, plus NUL/cursor.
// Truncation is by complete scalars, never by bytes.
void copy_text(char *destination, std::string_view text, std::size_t max_characters);
bool append_input(std::string &value, std::string_view input, std::size_t maximum, bool numeric);

} // namespace vangers::iscreen

#endif
