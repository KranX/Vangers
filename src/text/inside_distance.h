#ifndef VANGERS_TEXT_INSIDE_DISTANCE_H
#define VANGERS_TEXT_INSIDE_DISTANCE_H
#include <algorithm>
#include <cstddef>
#include <vector>

namespace vangers::text {
// Two-pass chamfer distance inside a glyph mask; its image boundary is background.
// Alpha reader returns coverage 0..255. O(width * height), no alphabet assumptions.
template<class Alpha> std::vector<float> inside_distance(int w, int h, Alpha alpha) {
	std::vector<float> d(std::size_t(w) * h, float(w + h));
	for (int y = 0; y < h; ++y)
		for (int x = 0; x < w; ++x) {
			const std::size_t i = std::size_t(y) * w + x;
			// Outside the tight ink box is background, including solid/rectangular glyphs.
			d[i] = alpha(x, y) < 128 ? 0.0f : float(std::min({x + 1, y + 1, w - x, h - y}));
		}
	// Two-pass chamfer distance inside the TTF contour, O(width * height).
	for (int y = 0; y < h; ++y)
		for (int x = 0; x < w; ++x) {
			auto &v = d[std::size_t(y) * w + x];
			if (x)
				v = std::min(v, d[std::size_t(y) * w + x - 1] + 1);
			if (y)
				v = std::min(v, d[std::size_t(y - 1) * w + x] + 1);
			if (x && y)
				v = std::min(v, d[std::size_t(y - 1) * w + x - 1] + 1.41421356f);
			if (x + 1 < w && y)
				v = std::min(v, d[std::size_t(y - 1) * w + x + 1] + 1.41421356f);
		}
	for (int y = h - 1; y >= 0; --y)
		for (int x = w - 1; x >= 0; --x) {
			auto &v = d[std::size_t(y) * w + x];
			if (x + 1 < w)
				v = std::min(v, d[std::size_t(y) * w + x + 1] + 1);
			if (y + 1 < h)
				v = std::min(v, d[std::size_t(y + 1) * w + x] + 1);
			if (x + 1 < w && y + 1 < h)
				v = std::min(v, d[std::size_t(y + 1) * w + x + 1] + 1.41421356f);
			if (x && y + 1 < h)
				v = std::min(v, d[std::size_t(y + 1) * w + x - 1] + 1.41421356f);
		}
	return d;
}
} // namespace vangers::text
#endif
