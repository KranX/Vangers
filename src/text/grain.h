#ifndef VANGERS_TEXT_GRAIN_H
#define VANGERS_TEXT_GRAIN_H
#include <cmath>
#include <cstdint>

namespace vangers::text {
// Stateless signed grain at glyph-local logical coordinates, independent of the
// language, palette, screen position, frame counter and gameplay RNG.
inline double grain(int x, int y) {
	uint32_t n = uint32_t(x) * 374761393u + uint32_t(y) * 668265263u + 1274126177u;
	n = (n ^ (n >> 13)) * 1274126177u;
	n ^= n >> 16;
	return double(n & 65535u) / 32767.5 - 1;
}
// Interpolate the same logical grid when rasterizing between logical pixels.
inline double smooth_grain(double x, double y) {
	const int ix = int(std::floor(x)), iy = int(std::floor(y));
	double u = x - ix, v = y - iy;
	u = u * u * (3 - 2 * u);
	v = v * v * (3 - 2 * v);
	return (grain(ix, iy) * (1 - u) + grain(ix + 1, iy) * u) * (1 - v) +
		   (grain(ix, iy + 1) * (1 - u) + grain(ix + 1, iy + 1) * u) * v;
}
} // namespace vangers::text
#endif
