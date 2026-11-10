#include "actint/ttf_font.h"
#include <SDL3_ttf/SDL_ttf.h>
#include <algorithm>
#include <filesystem>
#include <iostream>
#include <sstream>
#include <stdexcept>

namespace {
void check(bool ok, const char *message) {
	if (!ok)
		throw std::runtime_error(message);
}
auto raster(const vangers::actint::TextLayout &run) {
	check(bool(run.image), "No font image");
	return run.image->rasterize(run.ink_bounds.w, run.ink_bounds.h);
}
bool equal(const xgr::text::Raster &a, const xgr::text::Raster &b) {
	if (a.width != b.width || a.height != b.height)
		return false;
	for (std::size_t i = 0; i < a.pixels.size(); ++i)
		if (a.pixels[i].alpha != b.pixels[i].alpha || a.pixels[i].colour != b.pixels[i].colour ||
			a.pixels[i].border != b.pixels[i].border ||
			a.pixels[i].border_alpha != b.pixels[i].border_alpha)
			return false;
	return true;
}
} // namespace
int main(int argc, char **argv) {
	try {
		using namespace vangers::actint;
		int index = 0;
		for (auto family : {FontFamily::Text, FontFamily::Menu, FontFamily::Display}) {
			TtfFont font(family);
			const char *names[]{"Text", "Menu", "Display"};
			const auto path = std::filesystem::path(FONT_DIR) /
							  (std::string("Vangers") + names[index] + "-Regular.ttf");
			check(TTF_Init(), "Font initialization failed");
			auto *primary = TTF_OpenFont(path.string().c_str(), 18);
			check(primary != nullptr, "Cannot inspect primary font");
			const char *letters =
				"äöüß ÄÖÜ ÉÈÊË ç à â œ Œ ł Ł ą Ą ć Ć ę Ę ń Ń ó Ó ś Ś ź Ź ż Ż ș Ș ț Ț ı İ ĵ ȷ";
			std::size_t left = std::char_traits<char>::length(letters);
			const char *p = letters;
			while (left)
				check(
					TTF_FontHasGlyph(primary, SDL_StepUTF8(&p, &left)),
					"European letter fell through to generic font"
				);
			TTF_CloseFont(primary);
			TTF_Quit();
			for (auto cp : {U'日',
					 U'本',
					 U'語',
					 U'が',
					 U'ガ',
					 U'漢',
					 U'字',
					 U'「',
					 U'。',
					 U'ｶ',
					 U'　',
					 U'𠮷',
					 U'𠮟'})
				check(font.has_glyph(cp), "Japanese fallback coverage missing");
			check(
				font.missing_glyphs("é が か\u3099 e\u0301").empty(),
				"Canonical input coverage rejected"
			);
			check(
				font.missing_glyphs("\U0010FFFF\U0010FFFF") == std::vector<char32_t>{0x10FFFF},
				"Missing glyph diagnostics duplicated/lost"
			);
			FontStyle style;
			style.cell_width = family == FontFamily::Text	? 10
							   : family == FontFamily::Menu ? 45
															: 26;
			style.cell_height = family == FontFamily::Text ? 18 : 50;
			style.spacing = 1;
			style.highlight_numbers = false;
			auto composed = font.layout("é í ï ĵ が ガ", style),
				 decomposed =
					 font.layout("e\u0301 i\u0301 i\u0308 j\u0302 か\u3099 カ\u3099", style);
			check(
				composed.width == decomposed.width && equal(raster(composed), raster(decomposed)),
				"Canonical input changes font output"
			);
			auto sample =
				font.layout("Été ÄÖÜ ß Łódź Æ Œ æ œ\n日本語「カタカナ」が表示。𠮷", style);
			auto image = raster(sample);
			check(
				std::any_of(
					image.pixels.begin(), image.pixels.end(), [](auto p) { return p.alpha; }
				),
				"New scripts have no ink"
			);
			auto jp = raster(font.layout("語", style)), question = raster(font.layout("?", style));
			check(!equal(jp, question), "Fallback silently rendered a question mark");
			if (argc == 2) {
				std::filesystem::create_directories(argv[1]);
				std::vector<uint32_t> pixels(image.pixels.size());
				for (std::size_t i = 0; i < pixels.size(); ++i) {
					const auto a = image.pixels[i].alpha;
					pixels[i] = 0xFF000000u | (uint32_t(a) << 16) | (uint32_t(a) << 8) | a;
				}
				auto *surface = SDL_CreateSurfaceFrom(
					image.width,
					image.height,
					SDL_PIXELFORMAT_ARGB8888,
					pixels.data(),
					image.width * 4
				);
				check(surface != nullptr, "Cannot create visual proof");
				const auto file =
					std::filesystem::path(argv[1]) / (std::string(names[index]) + ".bmp");
				bool saved = SDL_SaveBMP(surface, file.string().c_str());
				SDL_DestroySurface(surface);
				check(saved, "Cannot save visual proof");
			}
			// Exercise fallback under the ACS material and its immutable size cache.
			style.range = true;
			style.embossed = true;
			style.fixed_pitch = true;
			style.colour = 128;
			auto embossed = font.layout("保存 語 が", style);
			check(equal(raster(embossed), raster(embossed)), "Fallback ACS cache is unstable");
			++index;
		}
		std::shared_ptr<const xgr::text::Image> retained;
		SDL_Rect bounds{};
		{
			TtfFont temporary;
			const auto run = temporary.layout("日本語", FontStyle{});
			retained = run.image;
			bounds = run.ink_bounds;
		}
		const auto alive = retained->rasterize(bounds.w, bounds.h);
		check(
			std::any_of(alive.pixels.begin(), alive.pixels.end(), [](auto p) { return p.alpha; }),
			"Retained fallback run outlived its font storage"
		);
		{
			TtfFont other(FontFamily::Menu);
			FontStyle large;
			large.cell_width = 45;
			large.cell_height = 50;
			raster(other.layout("表示", large));
		}
		check(
			equal(alive, retained->rasterize(bounds.w, bounds.h)),
			"Shared fallback drawing size leaked between roles"
		);
		std::cout << "Three primary European faces, Japanese fallback, canonical pairs, missing "
					 "coverage, material/cache OK\n";
	} catch (const std::exception &error) {
		std::cerr << error.what() << '\n';
		return 1;
	}
}
