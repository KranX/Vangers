#include "actint/aci_str.h"
#include "actint/text.h"
#include "text/utf8.h"

#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <stdexcept>

using namespace vangers::actint;
using namespace vangers::settings;
using namespace vangers::text;

static void check(bool condition, const char *message) {
	if (!condition) {
		std::cerr << message << '\n';
		std::exit(1);
	}
}

int main() {
	std::string legacy;
	for (int i = 1; i < 256; ++i)
		legacy += static_cast<char>(i);
	const auto canonical = decode_text(legacy, TextEncoding::Cp866);
	check(is_valid_utf8(canonical), "CP866 import must produce UTF-8");
	check(text_length(canonical) == 255, "CP866 characters must not be counted as UTF-8 bytes");
	check(raster_text(canonical, 256) == legacy, "All legacy glyph bytes must round-trip");

	const std::string unicode = "Ёж ä 日本 😀 e\u0301 $\x1e\x1f";
	check(
		decode_text(unicode, TextEncoding::Utf8) == unicode, "Explicit UTF-8 must not be transcoded"
	);
	check(raster_text(unicode, 256) == utf8_to_cp866(unicode), "Only bitmap view may use fallback");
	check(raster_text("A日", 58) == "  ", "Numeric font must never index nonexistent glyphs");
	check(raster_text(unicode, 0).empty(), "Empty font has no glyphs");
	check(raster_text(unicode, 32).empty(), "Font without a printable fallback has no text view");
	check(raster_text("A日", 33) == "  ", "Smallest printable font must use its space glyph");
	check(raster_text("$\x1e\x1f", 256) == "$\x1e\x1f", "Game icons/control glyphs must survive");

	for (const auto encoding : {TextEncoding::Utf8, static_cast<TextEncoding>(-1)}) {
		bool rejected = false;
		try {
			decode_text("\xff", encoding);
		} catch (const std::invalid_argument &) {
			rejected = true;
		}
		check(rejected, "Invalid explicit UTF-8/encoding must be rejected, not guessed");
	}
	check(
		decode_text("\xc3\xa4", TextEncoding::Cp866) != "ä",
		"CP866 import must not autodetect UTF-8"
	);

	auto *copy = copy_text(unicode);
	check(std::strcmp(copy, unicode.c_str()) == 0, "Owned model must preserve unsupported Unicode");
	std::string clipped = copy;
	delete[] copy;
	const auto count = text_length(unicode);
	std::string long_text;
	for (int i = 0; i < 2000; ++i)
		long_text += "Я日";
	auto *long_copy = copy_text(long_text);
	check(
		std::string(long_copy) == long_text, "Canonical storage must not have a 512/2048 byte cap"
	);
	delete[] long_copy;
	for (std::size_t i = count; i; --i) {
		check(
			clipped.size() == prefix_bytes(unicode, i),
			"Clipping/reveal must share scalar boundaries"
		);
		pop_back(clipped);
		check(is_valid_utf8(clipped), "Clipping must never leave a partial UTF-8 sequence");
	}
	pop_back(clipped);
	check(clipped.empty(), "Clipping empty text must not underflow");
	for (std::size_t i = 0; i <= count + 2; ++i) {
		const auto prefix = unicode.substr(0, prefix_bytes(unicode, i));
		check(is_valid_utf8(prefix), "Animated prefix must be valid UTF-8");
		check(text_length(prefix) == std::min(i, count), "Reveal budget counts characters");
	}

	const auto measure = [](std::string_view text) { return static_cast<int>(text_length(text)); };
	for (const auto &phrase : {std::string(),
			 unicode,
			 std::string("one two three four"),
			 std::string("оченьдлинноесловобезпробелов"),
			 std::string(2048, 'A')}) {
		for (int width : {0, 1, 5, 20, 4096}) {
			std::string restored;
			const auto lines = wrap_text(phrase, width, measure);
			check(!lines.empty(), "Empty panel text still has one line");
			for (const auto &line : lines) {
				check(is_valid_utf8(line), "Wrapped line must be valid UTF-8");
				check(
					line.empty() || width <= 1 || measure(line) < width,
					"Panel line must fit the legacy strict width contract"
				);
				restored += line;
			}
			check(restored == phrase, "Wrapping must not lose spaces or Unicode characters");
		}
	}
	const auto old_style = wrap_text("hello world again", 12, measure);
	check(
		old_style == std::vector<std::string>({"hello world", " again"}),
		"Existing dialogue leading-space convention must be preserved"
	);

	check(
		wrap_columns("Привет мир\n日本語😀", 6) == "Привет\nмир\n日本語😀",
		"Screen-text limits count characters, not bytes"
	);
	check(wrap_columns("абвгдеж", 3) == "абв\nгде\nж", "Long words need lossless scalar wrapping");
	check(
		wrap_columns("abc\r\n\nxyz\n", 80) == "abc\r\n\nxyz\n",
		"Existing paragraph delimiters stay intact"
	);
	check(wrap_columns(unicode, 0) == unicode, "Nonpositive column limit must make progress");
	check(
		is_valid_utf8(aciSTR_OFF2) && std::string(aciSTR_OFF2) == "Выкл.",
		"Source labels must be canonical UTF-8 too"
	);
	check(
		is_valid_utf8(aciSTR_PRICE2) && std::string(aciSTR_PRICE2) == "Цена: ",
		"Composed shop strings must not mix UTF-8 and CP866"
	);
	std::cout
		<< "actint UTF-8 boundaries, glyph compatibility, clipping, reveal and wrapping passed\n";
}
