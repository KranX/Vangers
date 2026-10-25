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

// Byte offsets at Unicode scalar boundaries (not grapheme/shaping support).
std::size_t prefix_bytes(std::string_view text, std::size_t characters);
std::size_t text_length(std::string_view text);
void pop_back(char *text);

using TextWidth = std::function<int(std::string_view)>;
std::vector<std::string> wrap_text(std::string_view text, int width, const TextWidth &measure);
std::string wrap_columns(std::string_view text, int columns);

} // namespace vangers::actint

// UTF-8 entry points for actint. The original bitmap APIs remain CP866 for
// iscreen, chat, ACS and other not-yet-migrated callers.
int aStrLenUtf8(const void *text, int font, int space);
int aStrLen32Utf8(const void *text, int font, int space);
void aPutStrUtf8(
	int x,
	int y,
	int font,
	int color,
	const void *text,
	int bsx,
	unsigned char *buf,
	int space
);
void aPutStr32Utf8(
	int x,
	int y,
	int font,
	int color,
	int color_size,
	const void *text,
	int bsx,
	void *buf,
	int space
);
void aOutStrUtf8(int x, int y, int font, int color, const void *text, int space);
void aOutText32Utf8(int x, int y, int color, const void *text, int font, int hspace, int vspace);
int aTextWidth32Utf8(const void *text, int font, int hspace);
int aTextHeight32Utf8(const void *text, int font, int vspace);

#endif
