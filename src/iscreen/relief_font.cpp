#include "relief_font.h"

#include "settings/settings.h"
#include "settings/text_encoding.h"

extern HFont **HFntTable;
extern int iNumFonts;

#include <algorithm>
#include <cmath>
#include <stdexcept>

namespace vangers::iscreen {
namespace {
std::vector<std::unique_ptr<ReliefFont>> faces;
}
ReliefFont *relief_font(int index) {
	if (!settings::settings_manager().get().video.ttf_text || !HFntTable || index < 0 ||
		index >= iNumFonts)
		return nullptr;
	if (faces.empty())
		faces.resize(iNumFonts);
	if (!faces[index])
		faces[index] = std::make_unique<ReliefFont>(
			*HFntTable[index],
			index == 1 ? actint::FontFamily::Menu : actint::FontFamily::Display,
			index == 2
		);
	return faces[index].get();
}
void free_relief_fonts() {
	faces.clear();
}

namespace {
// Fixed, stateless grain in glyph-local logical pixels: never depends on a frame
// number, global random generator, language or screen position.
double relief_noise(int x, int y) {
	uint32_t n = uint32_t(x) * 374761393u + uint32_t(y) * 668265263u + 1274126177u;
	n = (n ^ (n >> 13)) * 1274126177u;
	n ^= n >> 16;
	return double(n & 65535u) / 32767.5 - 1;
}
double smooth_relief_noise(double x, double y) {
	const int ix = int(std::floor(x)), iy = int(std::floor(y));
	double u = x - ix, v = y - iy;
	u = u * u * (3 - 2 * u);
	v = v * v * (3 - 2 * v);
	return (relief_noise(ix, iy) * (1 - u) + relief_noise(ix + 1, iy) * u) * (1 - v) +
		   (relief_noise(ix, iy + 1) * (1 - u) + relief_noise(ix + 1, iy + 1) * u) * v;
}
constexpr double relief_grain_amplitude = 2.1;
} // namespace

const std::vector<uint8_t> &ReliefGlyph::heights(int width, int height) const {
	if (width <= 0 || height <= 0) {
		static const std::vector<uint8_t> empty;
		return empty;
	}
	const auto key = std::make_pair(width, height);
	if (auto found = height_cache_.find(key); found != height_cache_.end())
		return found->second;
	return height_cache_.emplace(key, generate_heights(width, height)).first->second;
}

std::vector<uint8_t> ReliefGlyph::generate_heights(int width, int height) const {
	std::vector<uint8_t> result(size_t(width) * height, HFONT_NULL_LEVEL);
	if (bitmap) {
		for (int y = 0; y < height; ++y)
			for (int x = 0; x < width; ++x)
				result[size_t(y) * width + x] = source
					[size_t(y * source_height / height) * source_width + x * source_width / width];
		return result;
	}
	if (!outline)
		return result;
	constexpr int ss = 4;
	const int w = width * ss, h = height * ss;
	auto mask = outline->rasterize(w, h);
	if (!decorative) {
		// Quarter-pixel optical compensation, confined to the glyph's ink box.
		const auto input = mask.pixels;
		for (int y = 0; y < h; ++y)
			for (int x = 0; x < w; ++x) {
				auto alpha = input[size_t(y) * w + x].alpha;
				for (int yy = std::max(0, y - 1); yy <= std::min(h - 1, y + 1); ++yy)
					for (int xx = std::max(0, x - 1); xx <= std::min(w - 1, x + 1); ++xx)
						alpha = std::max(alpha, input[size_t(yy) * w + xx].alpha);
				mask.pixels[size_t(y) * w + x].alpha = alpha;
			}
	}
	std::vector<float> d(size_t(w) * h, float(w + h));
	for (int y = 0; y < h; ++y)
		for (int x = 0; x < w; ++x) {
			const size_t i = size_t(y) * w + x;
			// Outside the tight ink box is background, including solid/rectangular glyphs.
			d[i] =
				mask.pixels[i].alpha < 128 ? 0.0f : float(std::min({x + 1, y + 1, w - x, h - y}));
		}
	// Two-pass chamfer distance inside the TTF contour, O(width * height).
	for (int y = 0; y < h; ++y)
		for (int x = 0; x < w; ++x) {
			auto &v = d[size_t(y) * w + x];
			if (x)
				v = std::min(v, d[size_t(y) * w + x - 1] + 1);
			if (y)
				v = std::min(v, d[size_t(y - 1) * w + x] + 1);
			if (x && y)
				v = std::min(v, d[size_t(y - 1) * w + x - 1] + 1.41421356f);
			if (x + 1 < w && y)
				v = std::min(v, d[size_t(y - 1) * w + x + 1] + 1.41421356f);
		}
	for (int y = h - 1; y >= 0; --y)
		for (int x = w - 1; x >= 0; --x) {
			auto &v = d[size_t(y) * w + x];
			if (x + 1 < w)
				v = std::min(v, d[size_t(y) * w + x + 1] + 1);
			if (y + 1 < h)
				v = std::min(v, d[size_t(y + 1) * w + x] + 1);
			if (x + 1 < w && y + 1 < h)
				v = std::min(v, d[size_t(y + 1) * w + x + 1] + 1.41421356f);
			if (x && y + 1 < h)
				v = std::min(v, d[size_t(y + 1) * w + x - 1] + 1.41421356f);
		}
	const double floor = decorative ? 0.0 : 12.0;
	const double peak = decorative ? 24.0 : 27.0;
	const double bevel = (decorative ? 2.6 : 1.2) * height / 52.0;
	for (int y = 0; y < height; ++y)
		for (int x = 0; x < width; ++x) {
			double sum = 0;
			for (int sy = 0; sy < ss; ++sy)
				for (int sx = 0; sx < ss; ++sx) {
					const int i = (y * ss + sy) * w + x * ss + sx;
					const double alpha = mask.pixels[i].alpha / 255.0;
					const double distance = std::max(0.0, (d[i] - 0.5) / ss);
					const double px = x + (sx + 0.5) / ss, py = y + (sy + 0.5) / ss;
					double relief =
						floor +
						(peak - floor) * std::sin(std::min(1.0, distance / bevel) * 1.5707963268);
					relief *= 0.88 + 0.17 * smooth_relief_noise(px / 5.0, py / 5.0) +
							  0.06 * smooth_relief_noise(px / 2.3 + 17, py / 2.3 + 29);
					sum += relief * alpha;
				}
			double relief = sum / (ss * ss);
			// Grain belongs to the FINAL height grid, not the supersampled mask:
			// downsampling must not average the surface texture away.
			relief += relief_grain_amplitude * (decorative ? 1.0 : 0.45) *
					  relief_noise(x + 41, y + 67) * std::min(1.0, relief / 8.0);
			result[size_t(y) * width + x] =
				uint8_t(HFONT_NULL_LEVEL + std::clamp(int(std::lround(relief)), 0, 127));
		}
	return result;
}

ReliefFont::ReliefFont(const HFont &source, actint::FontFamily family, bool icons_only)
	: source_(source), font_(family), icons_only_(icons_only), family_(family) {}

const ReliefGlyph &ReliefFont::glyph(char32_t cp) {
	const auto found = glyphs_.find(cp);
	if (found != glyphs_.end())
		return found->second;
	ReliefGlyph result;
	result.decorative = family_ == actint::FontFamily::Menu;
	const auto legacy = settings::unicode_to_cp866(cp);
	// The legacy lookup preserves existing UI metrics, not height profiles. A new Unicode glyph
	// must never inherit '?' widths or heights merely because CP866 lacks it.
	const bool legacy_character = settings::cp866_to_unicode(legacy) == cp &&
								  legacy >= source_.StartChar && legacy <= source_.EndChar;
	const auto index =
		legacy >= source_.StartChar && legacy <= source_.EndChar ? legacy - source_.StartChar : 0;
	const auto &original = *source_.data[index];
	result.advance = original.SizeX - original.LeftOffs - original.RightOffs;
	result.bitmap = icons_only_ || cp < 32;
	if (result.bitmap) {
		result.source_width = original.SizeX;
		result.source_height = original.SizeY;
		result.source.assign(
			original.HeightMap, original.HeightMap + original.SizeX * original.SizeY
		);
		result.bounds = {-original.LeftOffs, 0, original.SizeX, original.SizeY};
	} else {
		char encoded[5]{};
		*SDL_UCS4ToUTF8(cp, encoded) = 0;
		actint::FontStyle style;
		style.cell_width = source_.SizeX;
		style.cell_height = source_.SizeY;
		style.spacing = style.line_spacing = 0;
		style.highlight_numbers = false;
		const auto layout = font_.layout(encoded, style);
		result.bounds = layout.ink_bounds;
		result.outline = layout.image;
		// Known glyphs keep legacy hit boxes/spacing; newly supplied outlines
		// (e.g. Menu lowercase) use their own advance instead of a blank cell.
		if (!legacy_character || ((original.Flags & NULL_HCHAR) && cp != ' ' && cp != 0xa0))
			result.advance = layout.width;
	}
	return glyphs_.emplace(cp, std::move(result)).first->second;
}

int ReliefFont::width(std::string_view utf8, int spacing) {
	if (!settings::is_valid_utf8(utf8))
		throw std::invalid_argument("iscreen relief expects UTF-8");
	int line = 0, maximum = 0;
	const char *cursor = utf8.data();
	size_t left = utf8.size();
	while (left) {
		const auto cp = SDL_StepUTF8(&cursor, &left);
		if (cp == '\r')
			continue;
		if (cp == '\n') {
			maximum = std::max(maximum, line);
			line = 0;
		} else {
			line += glyph(cp).advance + spacing;
		}
	}
	return std::max(maximum, line);
}

} // namespace vangers::iscreen
