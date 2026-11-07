#ifndef VANGERS_ACTINT_TEXT_H
#define VANGERS_ACTINT_TEXT_H

#include "../settings/text_encoding.h"

#include <string>
#include <string_view>

namespace vangers::actint {

enum class TextEncoding { Cp866, Utf8 };

// Encoding is chosen by the producer, never guessed from the bytes.
std::string decode_text(std::string_view text, TextEncoding encoding);
char *copy_text(std::string_view text);
std::string raster_text(std::string_view text, std::size_t glyph_count);

} // namespace vangers::actint

#endif
