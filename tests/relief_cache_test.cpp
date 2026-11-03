#include "iscreen/relief_font.h"

#include <cstdlib>
#include <iostream>
#include <new>

// This executable exercises the height cache without loading game/font assets.
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
	return 0;
}
