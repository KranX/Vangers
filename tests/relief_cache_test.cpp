#include "iscreen/relief_font.h"
#include "text/grain.h"
#include "text/inside_distance.h"

#include <cstdlib>
#include <iostream>
#include <new>

// Exercise both caches without private game assets; ACS uses the embedded font.
HFont **HFntTable = nullptr;
int iNumFonts = 0;

namespace {
bool count_allocations = false;
size_t allocations = 0;

struct Outline: xgr::text::Image {
	mutable int calls = 0;
	xgr::text::Raster rasterize(int width, int height) const override {
		++calls;
		return {width, height, std::vector<xgr::text::Pixel>(size_t(width) * height, {0, 255})};
	}
};
} // namespace

void *operator new(std::size_t size) {
	if (count_allocations)
		++allocations;
	if (auto *p = std::malloc(size ? size : 1))
		return p;
	throw std::bad_alloc();
}
void operator delete(void *p) noexcept {
	std::free(p);
}
void operator delete(void *p, std::size_t) noexcept {
	std::free(p);
}

int main() {
	const auto solid = vangers::text::inside_distance(5, 5, [](int, int) { return 255; });
	if (solid != std::vector<float>{
					 1, 1, 1, 1, 1, 1, 2, 2, 2, 1, 1, 2, 3, 2, 1, 1, 2, 2, 2, 1, 1, 1, 1, 1, 1})
		return 1;
	const auto hole = vangers::text::inside_distance(5, 5, [](int x, int y) {
		return x == 2 && y == 2 ? 127 : 128;
	});
	if (hole[12] != 0 || hole[11] != 1 || hole[6] != 1.41421356f)
		return 1;
	for (int x = -3; x < 4; ++x) {
		const double a = vangers::text::grain(x, 5), b = vangers::text::grain(x + 1, 5);
		if (vangers::text::smooth_grain(x, 5) != a ||
			std::abs(vangers::text::smooth_grain(x + 0.5, 5) - (a + b) / 2) > 1e-12)
			return 1;
	}

	vangers::iscreen::ReliefGlyph glyph;
	auto outline = std::make_shared<Outline>();
	glyph.outline = outline;
	count_allocations = true;
	const auto &first = glyph.heights(32, 50);
	count_allocations = false;
	if (!allocations || first.empty() || outline->calls != 1)
		return 1;
	const auto *pixels = first.data();
	allocations = 0;
	bool same = true;
	count_allocations = true;
	for (int i = 0; i < 1000; ++i) {
		const auto &hit = glyph.heights(32, 50);
		same &= &hit == &first && hit.data() == pixels;
	}
	count_allocations = false;
	if (allocations || !same || outline->calls != 1) {
		std::cerr << "Height cache hit allocated/copied/rasterized: " << allocations << '\n';
		return 1;
	}
	// ACS retains only the last native-size raster for an immutable run. A
	// cache hit allocates the returned pixel copy, not masks/height fields.
	vangers::actint::TtfFont font(vangers::actint::FontFamily::Display);
	vangers::actint::FontStyle style;
	style.cell_width = 20;
	style.cell_height = 39;
	style.range = style.embossed = style.fixed_pitch = true;
	const auto run = font.layout("Свободно", style);
	for (int scale : {1, 2, 1}) {
		const auto first = run.image->rasterize(run.ink_bounds.w * scale, run.ink_bounds.h * scale);
		allocations = 0;
		count_allocations = true;
		const auto second = run.image->rasterize(first.width, first.height);
		count_allocations = false;
		if (allocations != 1 || first.pixels.size() != second.pixels.size()) {
			std::cerr << "ACS surface cache regenerated glyphs: " << allocations << '\n';
			return 1;
		}
		for (size_t i = 0; i < first.pixels.size(); ++i) {
			const auto &a = first.pixels[i], &b = second.pixels[i];
			if (a.colour != b.colour || a.alpha != b.alpha || a.border != b.border ||
				a.border_alpha != b.border_alpha)
				return 1;
		}
	}
	return 0;
}
