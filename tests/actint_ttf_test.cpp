#include "actint/ttf_font.h"
#include "text_layer.h"

#include <algorithm>
#include <cstdlib>
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
