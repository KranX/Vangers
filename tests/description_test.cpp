#include "localization/catalog.h"
#include "localization/description.h"
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

#include <algorithm>
#include <filesystem>
#include <iostream>
#include <set>
#include <stdexcept>

namespace {
using namespace vangers::localization;
void check(bool ok, const char *message) {
	if (!ok)
		throw std::runtime_error(message);
}
template<class F> void rejects(F action) {
	try {
		action();
	} catch (const std::exception &) {
		return;
	}
	throw std::runtime_error("Invalid fixed-panel description accepted");
}

void overlapping_lines() {
	// Adjacent rows from Nymbos: a descender overlaps weaker ink on the next row.
	const std::string first = "первые бибы. Он получается из старых";
	const std::string second = "элиподов. Мокрый, скользкий и";
	const Description upper(first, DescriptionStyle::CompactCentered);
	const Description lower(second, DescriptionStyle::CompactCentered);
	const Description together(first + "\u2028" + second, DescriptionStyle::CompactCentered);
	// A centered two-line block places each row six pixels from a single row.
	constexpr int half_advance = 6;
	int overlaps = 0;
	for (int y = 0; y < Description::height; ++y)
		for (int x = 0; x < Description::width; ++x) {
			const auto a = y + half_advance < Description::height
							   ? upper.pixels()[(y + half_advance) * Description::width + x]
							   : 0xFF000000u;
			const auto b = y >= half_advance
							   ? lower.pixels()[(y - half_advance) * Description::width + x]
							   : 0xFF000000u;
			overlaps += a != b && a != 0xFF000000u && b != 0xFF000000u;
			check(
				together.pixels()[y * Description::width + x] == std::max(a, b),
				"Overlapping description rows weakened existing glyph coverage"
			);
		}
	check(overlaps > 0, "Description overlap fixture no longer exercises overlapping ink");
}
} // namespace

int main(int argc, char **argv) {
	try {
		check(argc == 2 || argc == 3, "Expected locale root and optional image directory");
		struct Font {
			TTF_Font *face;
			Font() {
				check(TTF_Init(), "Cannot initialise font coverage check");
				face = TTF_OpenFont(TEST_FONT, 18);
				if (!face) {
					TTF_Quit();
					throw std::runtime_error("Cannot open stock font for coverage check");
				}
			}
			~Font() {
				TTF_CloseFont(face);
				TTF_Quit();
			}
		} font;
		std::set<std::string> keys;
		check(
			description_key(DescriptionKind::Item, 0).data() == nullptr,
			"Unmapped item zero lost AVI fallback"
		);
		for (const auto kind : {DescriptionKind::Item, DescriptionKind::Mechos}) {
			const int first = kind == DescriptionKind::Item ? 1 : 0;
			const int last = kind == DescriptionKind::Item ? 78 : 38;
			check(description_key(kind, -1).empty(), "Negative mod ID indexed stock table");
			check(description_key(kind, last + 1).empty(), "Unknown mod ID lost AVI fallback");
			for (int id = first; id <= last; ++id) {
				const auto key = description_key(kind, id);
				check(!key.empty(), "Unmapped stock game ID");
				keys.emplace(key);
			}
		}
		check(keys.size() == 92, "Description inventory changed without an audit");
		for (const auto *locale : {"en", "ru"}) {
			Catalog catalog(argv[1], locale);
			int empty = 0;
			for (const auto &key : keys) {
				const auto &text = catalog.text(key);
				const char *cursor = text.data();
				std::size_t left = text.size();
				while (left) {
					const auto cp = SDL_StepUTF8(&cursor, &left);
					check(
						cp == 0x2028 || TTF_FontHasGlyph(font.face, cp),
						"Stock description contains a missing font glyph"
					);
				}
				empty += text.empty();
				try {
					Description image(text, catalog.description_style(key));
					check(image.pixels().size() == 340 * 255, "Description canvas changed");
					const auto ink =
						std::count_if(image.pixels().begin(), image.pixels().end(), [](uint32_t p) {
							return p != 0xFF000000u;
						});
					check(text.empty() == (ink == 0), "Empty or visible description lost its text");
					std::vector<uint32_t> target(350 * 265, 0x12345678u);
					image.draw(target.data(), 350, 265, 5, 5, 1);
					check(
						target.front() == 0x12345678u && target.back() == 0x12345678u,
						"Description overwrote another panel"
					);
					for (int y = 0; y < 255; ++y)
						check(
							std::equal(
								image.pixels().begin() + y * 340,
								image.pixels().begin() + (y + 1) * 340,
								target.begin() + (y + 5) * 350 + 5
							),
							"Draw changed cached text"
						);
					image.draw(target.data(), 350, 265, 5, 5, 0);
					check(target[5 * 350 + 5] == 0xFF000000u, "Fade changed opaque background");
					image.draw(target.data(), 350, 265, 5, 5, .5f);
					const auto first_ink =
						std::find_if(image.pixels().begin(), image.pixels().end(), [](uint32_t p) {
							return p != 0xFF000000u;
						});
					if (first_ink != image.pixels().end()) {
						const auto offset = std::size_t(first_ink - image.pixels().begin());
						const auto faded = target[(offset / 340 + 5) * 350 + offset % 340 + 5];
						check(
							((faded >> 16) & 255) == ((*first_ink >> 16) & 255) / 2 &&
								((faded >> 8) & 255) == ((*first_ink >> 8) & 255) / 2,
							"Description fade differs from the old AVI brightness multiplier"
						);
					}
					image.draw(target.data(), 350, 265, -339, -254, .5f);
					image.draw(target.data(), 350, 265, 349, 264, 1);
					image.draw(target.data(), 350, 265, 500, 500, 1);
					image.draw(nullptr, 0, 0, 0, 0, 1);
					if (argc == 3) {
						const auto path = std::filesystem::path(argv[2]) / locale / (key + ".bmp");
						std::filesystem::create_directories(path.parent_path());
						auto *surface = SDL_CreateSurfaceFrom(
							340,
							255,
							SDL_PIXELFORMAT_ARGB8888,
							const_cast<uint32_t *>(image.pixels().data()),
							340 * 4
						);
						check(surface != nullptr, "Cannot export comparison surface");
						const bool saved = SDL_SaveBMP(surface, path.string().c_str());
						SDL_DestroySurface(surface);
						check(saved, "Cannot export comparison image");
					}
				} catch (const std::exception &e) {
					throw std::runtime_error(std::string(locale) + ":" + key + ": " + e.what());
				}
			}
			check(empty == 9, "Missing source descriptions silently changed");
			check(catalog.diagnostics().empty(), "Stock descriptions use fallback");
		}
		overlapping_lines();
		rejects([] { Description bad("one\ftwo"); });
		rejects([] { Description bad(std::string(100000, 'W')); });
		rejects([] { Description bad(std::string("bad\xff", 4)); });
		std::cout << "117 IDs, 92 descriptions, RU/EN fixed-panel layout and clipping OK\n";
		return 0;
	} catch (const std::exception &e) {
		std::cerr << e.what() << '\n';
		return 1;
	}
}
