#include "actint/ttf_font.h"
#include "text_layer.h"
#include <SDL3_ttf/SDL_ttf.h>

#include <algorithm>
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <stdexcept>

namespace {
void check(bool condition, const char *message) {
	if (!condition) {
		std::cerr << message << '\n';
		std::exit(1);
	}
}

class Solid final: public xgr::text::Image {
  public:
	xgr::text::Raster rasterize(int width, int height) const override {
		return {width, height, std::vector<xgr::text::Pixel>(size_t(width) * height, {7, 128})};
	}
};
} // namespace

int main() {
	SDL_Color palette[256]{};
	palette[7] = {200, 100, 50, 255};
	palette[255] = {255, 255, 255, 255};
	using xgr::text::Layer;
	const auto solid = std::make_shared<Solid>();
	Layer source(4, 3);
	source.paint({-1, 0, 4, 2}, solid);
	auto pixels = source.render(8, 6, palette);
	check(pixels[0] == 0x80643219, "Palette colour must be premultiplied by coverage");
	check(pixels[6] == 0 && pixels[32] == 0, "Layer bounds must clip a draw");
	uint8_t mask[2] = {0, 1};
	source.erase({0, 0, 2, 1}, mask, 2);
	pixels = source.render(8, 6, palette);
	check(pixels[0] != 0 && pixels[2] == 0, "Transparent bitmap writes must not erase text");
	Layer destination(8, 6);
	destination.copy(source, {0, 0, 4, 3}, 3, 2);
	source.clear();
	pixels = destination.render(8, 6, palette);
	check(
		pixels[2 * 8 + 3] != 0 && pixels[2 * 8 + 4] == 0,
		"Buffer copies must own their text and preserve partial erasure"
	);
	destination.copy(destination, {3, 2, 3, 2}, 0, 0);
	check(destination.render(8, 6, palette)[0] != 0, "Overlapping copies must snapshot first");
	destination.erase({0, 0, 8, 6});
	check(destination.empty(), "Overwritten text must release its retained image");

	// Replacing one field must not erase neighbours whose ink bounds overlap.
	const auto neighbour = std::make_shared<Solid>();
	source.paint({0, 0, 4, 3}, neighbour);
	const auto neighbour_pixels = source.render(8, 6, palette);
	source.paint({0, 0, 4, 3}, solid);
	destination.copy(source, {0, 0, 4, 3}, 0, 0);
	const auto background_pixels = destination.render(8, 6, palette);
	source.remove(solid.get());
	check(
		source.render(8, 6, palette) == neighbour_pixels,
		"Removing one image damaged overlapping neighbour"
	);
	check(
		destination.render(8, 6, palette) == background_pixels,
		"Removing one image changed a captured background"
	);
	source.remove(neighbour.get());
	source.remove(neighbour.get());
	check(source.empty(), "Removed image retained draw entries");

	// Palette lookup happens at presentation, not when text is queued.
	Layer clipped(4, 3);
	const SDL_Rect clip{1, 1, 2, 1};
	clipped.paint({0, 0, 4, 3}, solid, &clip);
	palette[7] = {20, 10, 0, 255};
	uint8_t covered[12]{};
	covered[5] = 1;
	pixels = clipped.render(8, 6, palette, covered, 4);
	check(
		pixels[2 * 8 + 2] == 0 && pixels[2 * 8 + 4] == 0x800a0500 && pixels[0] == 0,
		"Native clipping, top-layer occlusion and palette fades must compose"
	);
	Layer masked_copy(4, 3);
	masked_copy.copy(clipped, {0, 0, 4, 3}, 0, 0, covered, 4);
	pixels = masked_copy.render(4, 3, palette);
	check(pixels[5] && !pixels[6], "Terrain-height copy masks must restrict native text");
	int key = 0;
	check(!xgr::text::prepare(&key, 4, 3), "Bitmap mode must not register companion layers");
	xgr::text::enabled = true;
	xgr::text::prepare(&key, 4, 3)->paint({0, 0, 4, 3}, solid);
	check(
		!xgr::text::prepare(&key, 4, 3, false)->empty(), "Preparing without reset must retain ink"
	);
	check(xgr::text::prepare(&key, 8, 6, false)->empty(), "Resize must discard stale coordinates");
	xgr::text::forget(&key);
	check(!xgr::text::find(&key), "Released buffers must not retain native text");
	xgr::text::enabled = false;

	vangers::actint::TtfFont font(TEST_FONT);
	vangers::actint::TtfFont embedded;
	vangers::actint::FontStyle style;
	for (const auto family :
		{vangers::actint::FontFamily::Menu, vangers::actint::FontFamily::Display}) {
		const std::string name = family == vangers::actint::FontFamily::Menu ? "Menu" : "Display";
		vangers::actint::TtfFont bundled(family);
		vangers::actint::TtfFont original(
			(std::filesystem::path(TEST_FONT).parent_path() / ("Vangers" + name + "-Regular.ttf"))
				.string(),
			family
		);
		vangers::actint::FontStyle large_style;
		large_style.cell_width = family == vangers::actint::FontFamily::Menu ? 45 : 26;
		large_style.cell_height = 50;
		large_style.spacing = 5;
		large_style.line_spacing = 0;
		large_style.highlight_numbers = false;
		const auto bundled_run = bundled.layout("НОВАЯ ИГРА Ёё09", large_style);
		const auto original_run = original.layout("НОВАЯ ИГРА Ёё09", large_style);
		check(
			bundled_run.width == original_run.width && bundled_run.height == 50,
			"Relief face metrics differ from approved asset"
		);
		Layer a(700, 60), b(700, 60);
		a.paint(bundled_run.ink_bounds, bundled_run.image);
		b.paint(original_run.ink_bounds, original_run.image);
		for (int scale : {1, 2}) {
			const auto rendered = a.render(700 * scale, 60 * scale, palette);
			check(
				rendered == b.render(700 * scale, 60 * scale, palette),
				"Menu/Display embedding changed outlines"
			);
			check(
				std::any_of(rendered.begin(), rendered.end(), [](auto p) { return p; }),
				"Relief face has no ink"
			);
		}
	}

	// Low-resolution strokes get light vertical grid fitting, but enlarged
	// output retains the unhinted outlines. Exercise both transitions on one face.
	const auto soft_sign = font.layout("ь", style);
	const auto word_width = font.layout("есть", style).width;
	std::unique_ptr<TTF_Font, decltype(&TTF_CloseFont)> reference_font(
		TTF_OpenFont(TEST_FONT, 18), TTF_CloseFont
	);
	check(bool(reference_font), "Cannot open hinting reference font");
	for (int scale : {1, 2, 1}) {
		TTF_SetFontHinting(reference_font.get(), scale == 1 ? TTF_HINTING_LIGHT : TTF_HINTING_NONE);
		check(
			TTF_SetFontSizeDPI(reference_font.get(), 1, 18 * scale * 72, 18 * scale * 72),
			"Cannot size hinting reference font"
		);
		int minx = 0, maxy = 0;
		check(
			TTF_GetGlyphMetrics(
				reference_font.get(), 0x44c, &minx, nullptr, nullptr, &maxy, nullptr
			),
			"Cannot measure soft sign"
		);
		std::unique_ptr<SDL_Surface, decltype(&SDL_DestroySurface)> glyph(
			TTF_GetGlyphImage(reference_font.get(), 0x44c, nullptr), SDL_DestroySurface
		);
		check(bool(glyph), "Cannot render reference soft sign");
		const auto raster = soft_sign.image->rasterize(
			soft_sign.ink_bounds.w * scale, soft_sign.ink_bounds.h * scale
		);
		std::vector<uint8_t> expected(raster.pixels.size());
		const int left = scale + minx, top = 16 * scale - maxy;
		for (int y = 0; y < glyph->h; ++y) {
			const auto *row = reinterpret_cast<const uint32_t *>(
				static_cast<const uint8_t *>(glyph->pixels) + y * glyph->pitch
			);
			for (int x = 0; x < glyph->w; ++x) {
				const auto alpha = uint8_t(row[x] >> 24);
				if (!alpha)
					continue;
				check(
					left + x >= 0 && left + x < raster.width && top + y >= 0 &&
						top + y < raster.height,
					"Soft-sign ink extends beyond its run allocation"
				);
				expected[(top + y) * raster.width + left + x] = alpha;
			}
		}
		for (size_t i = 0; i < expected.size(); ++i)
			check(raster.pixels[i].alpha == expected[i], "Unexpected soft-sign raster/hinting");
		check(
			font.layout("есть", style).width == word_width, "Drawing hinting changed layout metrics"
		);
	}
	const auto disk_layout = font.layout("Привет 09", style);
	const auto embedded_layout = embedded.layout("Привет 09", style);
	Layer disk_layer(200, 30), embedded_layer(200, 30);
	disk_layer.paint(disk_layout.ink_bounds, disk_layout.image);
	embedded_layer.paint(embedded_layout.ink_bounds, embedded_layout.image);
	check(
		disk_layout.width == embedded_layout.width &&
			disk_layout.height == embedded_layout.height &&
			disk_layer.render(400, 60, palette) == embedded_layer.render(400, 60, palette),
		"Embedded and source font must have identical metrics and pixels"
	);
	for (const auto text : {"Welcome back, poor pilgarlic!", "Очнулся, жалкий?", "Ёж ёлка 09 $"}) {
		const auto layout = font.layout(text, style);
		check(
			layout.width > 0 && layout.height == 19, "Single-line metrics must use the source cell"
		);
		Layer layer(layout.width, layout.height);
		layer.paint(layout.ink_bounds, layout.image);
		const auto small = layer.render(layout.width, layout.height, palette);
		const auto large = layer.render(layout.width * 2, layout.height * 2, palette);
		check(
			std::any_of(small.begin(), small.end(), [](uint32_t v) { return v; }),
			"TTF must render real ink"
		);
		bool native_detail = false;
		for (int y = 0; y < layout.height; ++y)
			for (int x = 0; x < layout.width; ++x) {
				const int offset = (2 * y) * (2 * layout.width) + 2 * x;
				native_detail |= large[offset] != large[offset + 1] ||
								 large[offset] != large[offset + 2 * layout.width];
			}
		check(
			native_detail, "High resolution must rerasterize outlines, not enlarge logical pixels"
		);
	}
	check(font.layout("A\nБ", style).height == 38, "Multiline height must match drawing");
	check(font.layout("A\rB", style).width == font.layout("AB", style).width, "CR is not a glyph");
	const auto a = font.layout("A", style).width;
	style.spacing = 3;
	check(
		font.layout("AA", style).width == 2 * a + 4, "Existing TTF tracking must not be added twice"
	);
	style = {};
	style.colour = 7 | (9u << 8) | (11u << 16) | (13u << 24);
	const auto coloured = font.layout("A1$", style).image->rasterize(90, 60);
	const auto has_colour = [&](int colour, bool border = false) {
		return std::any_of(coloured.pixels.begin(), coloured.pixels.end(), [&](auto p) {
			return border ? p.border_alpha && p.border == colour : p.alpha && p.colour == colour;
		});
	};
	check(
		has_colour(7) && has_colour(11) && has_colour(9, true) && has_colour(13, true),
		"Standard-font fill/border and numeric/currency highlight must remain separate"
	);
	style.range = true;
	style.colour = 32 | (64u << 16);
	style.colour_size = 1 | (2u << 16);
	const auto ranged = font.layout("A1", style).image->rasterize(60, 40);
	bool normal = false, highlighted = false;
	for (auto p : ranged.pixels) {
		if (p.alpha) {
			normal |= p.colour == 47;
			highlighted |= p.colour == 71;
		}
		check(!p.border_alpha, "Range fonts must not grow a standard-font border");
	}
	check(
		normal && highlighted, "Range colours must select the matching palette bank and intensity"
	);

	style = {};
	auto icons = std::make_shared<std::vector<vangers::actint::FontIcon>>(32);
	(*icons)[30] = {2, 3, std::vector<uint8_t>(180)};
	(*icons)[30].pixels[5 * 10 + 2] = 1;
	(*icons)[30].pixels[5 * 10 + 3] = 2;
	style.icons = icons;
	style.colour = 7 | (9u << 8);
	const auto icon = font.layout("\x1e", style);
	check(icon.width == 6, "Control icons must retain their bitmap bearings and advance");
	const auto icon_pixels = icon.image->rasterize(icon.ink_bounds.w, icon.ink_bounds.h);
	check(
		icon_pixels.pixels[6 * icon.ink_bounds.w + 1].colour == 7 &&
			icon_pixels.pixels[6 * icon.ink_bounds.w + 2].colour == 9,
		"Control icons must retain their original fill/border pixels"
	);
	font.layout("\x1f", style).image->rasterize(30, 30); // Missing icon storage safely falls back.
	check(
		!font.layout("", style).image && font.layout("", style).width == 0,
		"Empty text must not retain an empty draw command"
	);
	for (const auto cell : {std::pair{5, 9}, std::pair{7, 18}, std::pair{20, 18}}) {
		style.cell_width = cell.first;
		style.cell_height = cell.second;
		const auto layout = font.layout("Ёж 09", style);
		const auto raster =
			layout.image->rasterize(layout.ink_bounds.w * 3 / 2, layout.ink_bounds.h * 3 / 2);
		check(
			std::any_of(raster.pixels.begin(), raster.pixels.end(), [](auto p) { return p.alpha; }),
			"Narrow/wide source cells and fractional scaling must render native ink"
		);
	}
	// U+E000 has no CP866 representation: this also guards against accidentally
	// routing the native backend back through the legacy bitmap codec.
	style = {};
	const auto currency = font.layout("$", style),
			   private_currency = font.layout("\xee\x80\x80", style);
	Layer currency_layer(20, 22), private_currency_layer(20, 22);
	currency_layer.paint(currency.ink_bounds, currency.image);
	private_currency_layer.paint(private_currency.ink_bounds, private_currency.image);
	check(
		currency.width == private_currency.width &&
			currency_layer.render(40, 44, palette) ==
				private_currency_layer.render(40, 44, palette),
		"Native Unicode currency alias must not become the CP866 fallback glyph"
	);
	// ACS FNC faces use fixed cells, even for narrow letters, spaces and new
	// Unicode scalars. Ordinary actint/iscreen roles remain proportional.
	vangers::actint::TtfFont display(vangers::actint::FontFamily::Display);
	for (const auto cell : {std::pair{20, 39}, std::pair{16, 31}}) {
		vangers::actint::FontStyle fixed;
		fixed.cell_width = cell.first;
		fixed.cell_height = cell.second;
		const auto proportional = display.layout("Свободно", fixed).width;
		fixed.fixed_pitch = true;
		for (const auto text : {"Свободно", "iiiiiiii", "ШШШШШШШШ", " A_Ё日😀$!"})
			check(
				display.layout(text, fixed).width == 8 * (cell.first + 1),
				"ACS pitch must count Unicode scalars, not proportional advances/UTF-8 bytes"
			);
		check(
			display.layout("ПАУЗА", fixed).width == 5 * (cell.first + 1),
			"ACS heading lost its original pitch"
		);
		check(
			display.layout("A\nББ", fixed).width == 2 * (cell.first + 1),
			"Fixed-pitch multiline width changed"
		);
		check(
			proportional != display.layout("Свободно", fixed).width,
			"Fixed-pitch opt-in changed the default layout"
		);
		for (const auto text : {"A", "Ё", "_"}) {
			fixed.fixed_pitch = false;
			const auto a = display.layout(text, fixed);
			fixed.fixed_pitch = true;
			const auto b = display.layout(text, fixed);
			Layer left(50, 50), right(50, 50);
			left.paint(a.ink_bounds, a.image);
			right.paint(b.ink_bounds, b.image);
			check(
				left.render(100, 100, palette) == right.render(100, 100, palette),
				"Fixed pitch stretched or repositioned a glyph's outline"
			);
		}
	}
	// ACS material uses only TTF ink, with small optical compensation and an
	// unchanged advance. Exercise Latin, Cyrillic and non-CP866 ink.
	for (const auto family :
		{vangers::actint::FontFamily::Text, vangers::actint::FontFamily::Display}) {
		vangers::actint::TtfFont surface_font(family);
		vangers::actint::FontStyle material;
		material.fixed_pitch = true;
		material.cell_width = 20;
		material.cell_height = 39;
		material.colour = 48;
		material.colour_size = 1;
		material.range = material.zero_range_transparent = true;
		material.highlight_numbers = false;
		const auto flat = surface_font.layout("AO Ёж $\xee\x80\x80", material);
		material.embossed = true;
		const auto shaded = surface_font.layout("AO Ёж $\xee\x80\x80", material);
		check(
			flat.width == shaded.width && flat.height == shaded.height,
			"Material changed layout metrics"
		);
		for (int scale : {1, 2}) {
			const auto a =
				flat.image->rasterize(flat.ink_bounds.w * scale, flat.ink_bounds.h * scale);
			const auto b =
				shaded.image->rasterize(shaded.ink_bounds.w * scale, shaded.ink_bounds.h * scale);
			const auto repeat = shaded.image->rasterize(b.width, b.height);
			const auto flat_again = flat.image->rasterize(a.width, a.height);
			for (size_t i = 0; i < a.pixels.size(); ++i)
				check(
					a.pixels[i].alpha == flat_again.pixels[i].alpha,
					"ACS optical compensation modified cached glyph ink in another role"
				);
			bool levels[16]{};
			bool shadow = false, sharpened_edge = false;
			for (int y = 0; y < a.height; ++y)
				for (int x = 0; x < a.width; ++x) {
					const auto flat_alpha = a.pixels[size_t(y) * a.width + x].alpha;
					const int dx = (flat.ink_bounds.x - shaded.ink_bounds.x) * scale;
					const int dy = (flat.ink_bounds.y - shaded.ink_bounds.y) * scale;
					const auto surface_alpha = b.pixels[size_t(y + dy) * b.width + x + dx].alpha;
					sharpened_edge |= surface_alpha < flat_alpha;
					check(!flat_alpha || surface_alpha, "Surface contrast erased a thin stroke");
					check(
						flat_alpha < 128 || surface_alpha >= 128,
						"Surface contrast eroded the solid stroke core"
					);
					if (scale == 2)
						check(
							flat_alpha <= surface_alpha,
							"Large native glyphs must keep their original antialiasing"
						);
				}
			check(
				sharpened_edge == (scale == 1), "Edge contrast must target small native glyphs only"
			);
			for (size_t i = 0; i < b.pixels.size(); ++i) {
				const auto &pixel = b.pixels[i], &again = repeat.pixels[i];
				check(
					pixel.colour == again.colour && pixel.alpha == again.alpha &&
						pixel.border == again.border && pixel.border_alpha == again.border_alpha,
					"Surface grain changes between frames"
				);
				if (pixel.alpha) {
					check(
						pixel.colour > 48 && pixel.colour <= 63, "Surface escaped its palette ramp"
					);
					levels[pixel.colour - 48] = true;
				}
				// The raised body must visibly protrude, not disappear inside the
				// antialiased face as the rejected shallow material did.
				shadow |= pixel.border_alpha && !pixel.alpha;
			}
			check(
				std::count(std::begin(levels), std::end(levels), true) >= 6,
				"Surface lost its light/shade/grain variation"
			);
			check(shadow, "Raised body no longer protrudes outside the face");
		}
		for (double scale : {1.25, 1.5}) {
			const auto a = shaded.image->rasterize(
				int(shaded.ink_bounds.w * scale), int(shaded.ink_bounds.h * scale)
			);
			const auto b = shaded.image->rasterize(a.width, a.height);
			check(
				std::any_of(a.pixels.begin(), a.pixels.end(), [](auto p) { return p.alpha; }),
				"Fractional ACS surface has no ink"
			);
			for (size_t i = 0; i < a.pixels.size(); ++i)
				check(
					a.pixels[i].colour == b.pixels[i].colour &&
						a.pixels[i].alpha == b.pixels[i].alpha &&
						a.pixels[i].border_alpha == b.pixels[i].border_alpha,
					"Fractional ACS surface changes between frames"
				);
		}
		SDL_Color first_palette[256], second_palette[256];
		for (int i = 0; i < 256; ++i) {
			first_palette[i] = {10, 20, 30, 255};
			second_palette[i] = {50, 60, 70, 255};
		}
		Layer fading(400, 60);
		fading.paint(shaded.ink_bounds, shaded.image);
		const auto before = fading.render(400, 60, first_palette);
		const auto after = fading.render(400, 60, second_palette);
		check(before != after, "Cached surface froze the displayed palette");
		for (size_t i = 0; i < before.size(); ++i)
			check((before[i] >> 24) == (after[i] >> 24), "Palette fade changed cached coverage");
		// Hover/selection palette changes must not change material or its coverage.
		material.colour += 16;
		const auto selected = surface_font.layout("AO Ёж $\xee\x80\x80", material);
		const auto a = shaded.image->rasterize(shaded.ink_bounds.w, shaded.ink_bounds.h);
		const auto b = selected.image->rasterize(selected.ink_bounds.w, selected.ink_bounds.h);
		for (size_t i = 0; i < a.pixels.size(); ++i) {
			check(
				a.pixels[i].alpha == b.pixels[i].alpha &&
					a.pixels[i].border_alpha == b.pixels[i].border_alpha,
				"Palette state changed material coverage"
			);
			if (a.pixels[i].alpha)
				check(b.pixels[i].colour == a.pixels[i].colour + 16, "Palette state changed grain");
		}
	}

	const std::string unicode = "Привет 日本 😀";
	font.layout(unicode, style);
	check(unicode == "Привет 日本 😀", "Missing-glyph fallback must not alter UTF-8 storage");
	bool rejected = false;
	try {
		font.layout("\xff", style);
	} catch (const std::invalid_argument &) {
		rejected = true;
	}
	check(rejected, "Invalid UTF-8 must not be interpreted as legacy bytes");
	std::cout << "Native text layer and TTF metrics passed\n";
}
