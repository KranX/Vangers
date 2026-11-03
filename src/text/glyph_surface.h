#ifndef VANGERS_TEXT_GLYPH_SURFACE_H
#define VANGERS_TEXT_GLYPH_SURFACE_H
#include "grain.h"
#include "inside_distance.h"
#include <cmath>

namespace vangers::text::glyph {
// One fixed ACS material, independent of the font and Unicode codepoint.
constexpr double depth = 4.0;
// Small height irregularities are secondary to a coherent face and bevel.
constexpr double grain_amplitude = 0.4 / depth;
constexpr int padding = 5;

// Bilinear samples with transparent/zero height outside the glyph's ink box.
template<class Sample> double sample(int w, int h, double x, double y, Sample value) {
	const int ix = int(std::floor(x)), iy = int(std::floor(y));
	const double u = x - ix, v = y - iy;
	const auto sample = [&](int px, int py) {
		return px < 0 || py < 0 || px >= w || py >= h ? 0.0 : double(value(px, py));
	};
	return (sample(ix, iy) * (1 - u) + sample(ix + 1, iy) * u) * (1 - v) +
		   (sample(ix, iy + 1) * (1 - u) + sample(ix + 1, iy + 1) * u) * v;
}

// Alpha is read-only coverage in 0..255; dimensions/scales must be positive.
// Fractional seeds retain faint strokes and give neighbouring solid pixels a
// consistent distance to the antialiased edge. This is not an exact SDF.
template<class Alpha> std::vector<float> heights(int w, int h, double sx, double sy, Alpha alpha) {
	auto result = inside_distance(w, h, alpha, true);
	for (int y = 0; y < h; ++y)
		for (int x = 0; x < w; ++x) {
			auto &value = result[std::size_t(y) * w + x];
			const double profile = std::min(1.0, value / std::min(sx, sy));
			value = float(profile * (1 + grain_amplitude * smooth_grain(x / sx, y / sy)));
		}
	return result;
}
inline double height(const std::vector<float> &field, int w, int h, double x, double y) {
	return sample(w, h, x, y, [&](int px, int py) { return field[std::size_t(py) * w + px]; });
}

// A moderately lit face, directional bevel and narrow highlight. The highlight
// follows the surface normal, not a glyph's identity or a copied edge bitmap.
inline int tone(const std::vector<float> &field, int w, int h, double sx, double sy, int x, int y) {
	const double nx =
		depth * (height(field, w, h, x - 0.5 * sx, y) - height(field, w, h, x + 0.5 * sx, y));
	const double ny =
		depth * (height(field, w, h, x, y - 0.5 * sy) - height(field, w, h, x, y + 0.5 * sy));
	constexpr double lx = -1, ly = -0.1, lz = 0.45;
	const double nl = std::sqrt(nx * nx + ny * ny + 1);
	const double ll = std::sqrt(lx * lx + ly * ly + lz * lz);
	const double light = (lx * nx + ly * ny + lz) / (nl * ll);
	// Half-vector between the light and a front-facing view (0, 0, 1).
	const double hx = lx / ll, hy = ly / ll, hz = lz / ll + 1;
	double highlight = std::clamp(
		(hx * nx + hy * ny + hz) / (nl * std::sqrt(hx * hx + hy * hy + hz * hz)), 0.0, 1.0
	);
	for (int i = 0; i < 4; ++i)
		highlight *= highlight; // Fixed exponent 16, no general material/pow machinery.
	const double face = std::clamp(double(field[std::size_t(y) * w + x]), 0.0, 1.0);
	// Darken slopes facing away from the light relative to a flat front.
	// Flat and equally/better lit slopes stay unchanged; no alphabet/palette rule.
	const double shade = std::clamp(lz / ll - light, 0.0, 1.0);
	return std::clamp(
		int(std::lround(
			6 + 10.8 * face + 12.96 * std::max(0.0, light) + 12 * highlight - 12.96 * shade
		)),
		2,
		31
	);
}

// Contrast only the front coverage, after height/body generation. A second
// S-curve sharpens small native edges, but fades out where the 3x3 neighbourhood
// has no opaque core. Otherwise fine/low-coverage strokes become too faint.
template<class Alpha> uint8_t coverage(int w, int h, int x, int y, double amount, Alpha alpha) {
	const int value = alpha(x, y);
	if (!value || value == 255 || amount == 0)
		return uint8_t(value);
	double a = value / 255.0;
	a += amount * a * (1 - a) * (2 * a - 1);
	int peak = value;
	for (int py = std::max(0, y - 1); py <= std::min(h - 1, y + 1); ++py)
		for (int px = std::max(0, x - 1); px <= std::min(w - 1, x + 1); ++px)
			peak = std::max(peak, int(alpha(px, py)));
	const double extra = amount * std::clamp((peak - 128) / 127.0, 0.0, 1.0);
	a += extra * a * (1 - a) * (2 * a - 1);
	return uint8_t(std::clamp(int(std::lround(255 * a)), 1, 254));
}

// Dark body behind the face, projected down-left in a fixed oblique view.
// Short external body: the main relief comes from shading on the glyph itself.
// This is a stylized extrusion, not a cast shadow towards the face's light.
inline double side_coverage(
	const std::vector<float> &field,
	int w,
	int h,
	double sx,
	double sy,
	double x,
	double y
) {
	constexpr double dx = 3.25, dy = 1.25;
	const double top = 1 + grain_amplitude;
	// The bilinear height field is zero outside (-1,w) x (-1,h).
	// Reject points whose whole projection ray stays outside that support.
	if (x >= w || y <= -1 || x + dx * top * sx <= -1 || y - dy * top * sy >= h)
		return 0;
	double result = 0;
	for (int step = 1; step <= 16; ++step) {
		const double z = (1 + grain_amplitude) * step / 16;
		const double surface = height(field, w, h, x + dx * z * sx, y - dy * z * sy);
		result = std::max(result, std::clamp((surface - z) / 0.125 + 0.5, 0.0, 1.0));
		if (result >= 1)
			return 1;
	}
	return result;
}
} // namespace vangers::text::glyph
#endif
