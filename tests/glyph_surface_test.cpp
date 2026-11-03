#include "text/glyph_surface.h"

#include <cstdlib>
#include <iostream>

namespace {
void check(bool ok, const char *message) {
	if (!ok) {
		std::cerr << message << '\n';
		std::exit(1);
	}
}
} // namespace

int main() {
	namespace surface = vangers::text::glyph;
	constexpr int w = 17, h = 19;
	const auto empty = [](int, int) { return 0; };
	const auto solid = [](int, int) { return 255; };
	for (double scale : {1.0, 1.25, 1.5, 2.0, 3.0}) {
		const auto zero = surface::heights(w, h, scale, scale, empty);
		const auto full = surface::heights(w, h, scale, scale, solid);
		check(zero == std::vector<float>(w * h), "Empty mask gained height");
		check(full == surface::heights(w, h, scale, scale, solid), "Unstable grain/height");
		for (float value : full)
			check(
				std::isfinite(value) && value >= 0 && value <= float(1 + surface::grain_amplitude),
				"Height out of bounds"
			);
		// Compare the profile independently of the deterministic surface grain.
		const auto profile = [&](int x, int y) {
			return full[y * w + x] / (1 + surface::grain_amplitude *
											  vangers::text::smooth_grain(x / scale, y / scale));
		};
		check(profile(w / 2, h / 2) + 1e-6 >= profile(0, 0), "Inverted boundary bevel");
		check(
			full[(h / 2) * w + w / 2] > surface::height(full, w, h, -0.5, 0),
			"Solid image lost its outer boundary bevel"
		);
		for (int y = -10; y < h + 10; ++y)
			for (int x = -10; x < w + 10; ++x) {
				check(
					surface::side_coverage(zero, w, h, scale, scale, x, y) == 0,
					"Shadow copied alpha instead of following height"
				);

				const double shadow = surface::side_coverage(full, w, h, scale, scale, x, y);
				check(
					std::isfinite(shadow) && shadow >= 0 && shadow <= 1,
					"Shadow coverage out of bounds"
				);
				if (x < -surface::padding * scale || y < -surface::padding * scale ||
					x >= w + surface::padding * scale || y >= h + surface::padding * scale)
					check(shadow == 0, "Shadow escaped layout padding");
			}
	}

	// A separate thick stroke must not affect the bevel across a transparent gap.
	const auto thin = [](int x, int) { return x == 5 ? 255 : 0; };
	const auto neighbouring = [](int x, int) { return x == 5 || (x >= 7 && x <= 14) ? 255 : 0; };
	const auto narrow = surface::heights(w, h, 1, 1, thin);
	const auto adjacent = surface::heights(w, h, 1, 1, neighbouring);
	for (int y = 0; y < h; ++y)
		check(narrow[y * w + 5] == adjacent[y * w + 5], "Bevel crossed a counter/gap");
	const double expected = 1 + surface::grain_amplitude * vangers::text::smooth_grain(5, 9);
	check(std::abs(narrow[9 * w + 5] - expected) < 1e-6, "Linear bevel profile lost");
	// A faint antialiased stroke must not disappear from the height field.
	for (int coverage : {1, 32, 64, 127, 128, 255}) {
		const auto soft =
			surface::heights(w, h, 1, 1, [=](int x, int) { return x == 5 ? coverage : 0; });
		check(
			soft[9 * w + 5] > 0 && soft[9 * w + 4] == 0,
			"Fractional edge lost height or spread into transparent pixels"
		);
		check(soft[9 * w + 5] <= narrow[9 * w + 5], "Coverage inverted edge height");
	}
	const std::vector<float> raised(w * h, 1), low_face(w * h, 0.25f);
	check(
		surface::tone(raised, w, h, 1, 1, 8, 9) > surface::tone(low_face, w, h, 1, 1, 8, 9),
		"Raised face lost its height-based contrast"
	);
	// Opposing slopes on a thin vertical ridge must not cancel into a flat
	// normal. The fixed left-facing material keeps a broad lit facet there.
	std::vector<float> ridge(w * h);
	for (int y = 0; y < h; ++y)
		ridge[y * w + 8] = 1;
	check(
		surface::tone(ridge, w, h, 1, 1, 8, 9) > surface::tone(raised, w, h, 1, 1, 8, 9) + 3,
		"Thin ridge lost its light-facing facet"
	);
	// The fractional edge affects solid neighbours, not just its own pixel.
	const auto soft_edge = [](int x, int) { return x == 4 ? 64 : x >= 5 && x <= 11 ? 255 : 0; };
	const auto edge_distance = vangers::text::inside_distance(w, h, soft_edge, true);
	check(
		std::abs(edge_distance[9 * w + 5] - (1 + 64.0 / 255)) < 1e-6,
		"Fractional edge was not propagated into the solid stroke"
	);
	const auto legacy_distance = vangers::text::inside_distance(w, h, soft_edge);
	check(
		legacy_distance[9 * w + 4] == 0 && legacy_distance[9 * w + 5] == 1,
		"Default distance changed the legacy alpha threshold"
	);
	std::vector<float> toward(w * h), away(w * h);
	for (int y = 0; y < h; ++y)
		for (int x = 0; x < w; ++x) {
			toward[y * w + x] = float(std::clamp(0.6 + 0.16 * (x - 8) + 0.016 * (y - 9), 0.0, 1.0));
			away[y * w + x] = float(std::clamp(0.6 - 0.16 * (x - 8) - 0.016 * (y - 9), 0.0, 1.0));
		}
	check(
		surface::tone(toward, w, h, 1, 1, 8, 9) > surface::tone(away, w, h, 1, 1, 8, 9) + 3,
		"Directional facet lost contrast"
	);
	// Extra contrast must not suppress thin strokes with no opaque local core.
	for (int peak : {64, 128, 200, 255})
		for (double strength : {0.0, 0.25, 0.5, 1.0}) {
			int last = 0;
			for (int value = 0; value <= 255; ++value) {
				const auto read_alpha = [=](int x, int y) {
					return x == 1 && y == 1 ? value : peak;
				};
				const int result = surface::coverage(3, 3, 1, 1, strength, read_alpha);
				check(result >= last, "Local coverage contrast is not monotonic");
				check(!value || result, "Coverage contrast erased a faint stroke");
				check(value < 128 || result >= 128, "Coverage contrast erased the solid core");
				check(value != 0 || result == 0, "Coverage contrast filled a counter");
				check(value != 255 || result == 255, "Coverage contrast changed opaque ink");
				if (strength == 0)
					check(result == value, "Large-size antialiasing changed");
				if (value && value < 255 && std::max(value, peak) <= 128) {
					const double a = value / 255.0;
					const int single = std::clamp(
						int(std::lround(255 * (a + strength * a * (1 - a) * (2 * a - 1)))), 1, 254
					);
					check(result == single, "Extra sharpening suppressed a low-coverage stroke");
				}
				last = result;
			}
		}

	// Keep alpha immutable; heights change the projected shadow with the SAME mask.
	std::vector<unsigned char> alpha(w * h);
	std::vector<float> low(w * h), high(w * h);
	for (int y = 3; y <= 14; ++y)
		for (int x = 5; x <= 11; ++x) {
			alpha[y * w + x] = 255;
			low[y * w + x] = 0.5f;
			high[y * w + x] = 1;
		}
	const auto saved = alpha;
	const auto read = [&](int x, int y) { return alpha[y * w + x]; };
	surface::heights(w, h, 1.5, 2, read);
	double change = 0;
	for (int y = -4; y < h + 4; ++y)
		for (int x = -4; x < w + 4; ++x) {
			const double a = surface::side_coverage(low, w, h, 1, 1, x, y);
			const double b = surface::side_coverage(high, w, h, 1, 1, x, y);
			check(b >= a, "Increasing height reduced occlusion");
			change += b - a;
		}
	check(change > 0, "Shadow ignored height changes");
	check(
		surface::side_coverage(high, w, h, 1, 1, 4, 8) > 0 &&
			surface::side_coverage(high, w, h, 1, 1, 8, 2) == 0 &&
			surface::side_coverage(high, w, h, 1, 1, 12, 8) == 0 &&
			surface::side_coverage(high, w, h, 1, 1, 8, 14.6) > 0,
		"Raised body lost its down-left projection"
	);
	check(alpha == saved, "Material modified the source mask");
	return 0;
}
