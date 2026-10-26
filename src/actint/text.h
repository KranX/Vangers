#ifndef VANGERS_ACTINT_TEXT_H
#define VANGERS_ACTINT_TEXT_H

#include "../settings/text_encoding.h"

#include <functional>
#include <string>
#include <string_view>
#include <vector>

namespace vangers::actint {

enum class TextEncoding { Cp866, Utf8 };

// Encoding is chosen by the producer, never guessed from the bytes.
std::string decode_text(std::string_view text, TextEncoding encoding);
char *copy_text(std::string_view text);
std::string raster_text(std::string_view text, std::size_t glyph_count);

using TextWidth = std::function<int(std::string_view)>;
std::vector<std::string> wrap_text(std::string_view text, int width, const TextWidth &measure);
std::string wrap_columns(std::string_view text, int columns);

} // namespace vangers::actint

#endif
