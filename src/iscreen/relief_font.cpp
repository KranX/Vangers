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

std::vector<uint8_t> ReliefGlyph::heights(int width, int height) const {
	if (width <= 0 || height <= 0)
		return {};
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
	const auto coverage = outline->rasterize(width, height);
	const auto sample = [&](int x, int y) {
		x = std::clamp(x, 0, source_width - 1);
		y = std::clamp(y, 0, source_height - 1);
		return profile[size_t(y) * source_width + x];
	};
	for (int y = 0; y < height; ++y) {
		for (int x = 0; x < width; ++x) {
			const auto alpha = coverage.pixels[size_t(y) * width + x].alpha;
			if (!alpha)
				continue;
			const double px = (x + 0.5) * bounds.w / width + bounds.x + source_left - 0.5;
			const double py = (y + 0.5) * bounds.h / height + bounds.y - 0.5;
			const int ix = int(std::floor(px)), iy = int(std::floor(py));
			const double fx = px - ix, fy = py - iy;
			const double relief =
				(sample(ix, iy) * (1 - fx) + sample(ix + 1, iy) * fx) * (1 - fy) +
				(sample(ix, iy + 1) * (1 - fx) + sample(ix + 1, iy + 1) * fx) * fy;
			result[size_t(y) * width + x] =
				uint8_t(HFONT_NULL_LEVEL + std::lround(relief * alpha / 255));
		}
	}
	return result;
}

ReliefFont::ReliefFont(const HFont &source, actint::FontFamily family, bool icons_only)
	: source_(source), font_(family), icons_only_(icons_only) {}

const ReliefGlyph &ReliefFont::glyph(char32_t cp) {
	const auto found = glyphs_.find(cp);
	if (found != glyphs_.end())
		return found->second;
	ReliefGlyph result;
	const auto legacy = settings::unicode_to_cp866(cp);
	// The legacy lookup is only a compatibility profile. A new Unicode glyph
	// must never inherit '?' widths or heights merely because CP866 lacks it.
	const bool legacy_character = settings::cp866_to_unicode(legacy) == cp &&
								  legacy >= source_.StartChar && legacy <= source_.EndChar;
	const auto index =
		legacy >= source_.StartChar && legacy <= source_.EndChar ? legacy - source_.StartChar : 0;
	const auto &original = *source_.data[index];
	result.source_width = original.SizeX;
	result.source_height = original.SizeY;
	result.source_left = original.LeftOffs;
	result.source.assign(original.HeightMap, original.HeightMap + original.SizeX * original.SizeY);
	// Extend the signed relief profile underneath the new outline. Otherwise
	// neutral source pixels would cut the TTF back to the old bitmap silhouette.
	// Existing heights are unchanged; only previously empty samples are filled.
	result.profile.assign(result.source.size(), 31);
	std::vector<int> queue;
	std::vector<bool> filled(result.source.size());
	for (size_t i = 0; i < result.source.size(); ++i) {
		if (legacy_character && result.source[i] != HFONT_NULL_LEVEL) {
			result.profile[i] = int(result.source[i]) - HFONT_NULL_LEVEL;
			filled[i] = true;
			queue.push_back(int(i));
		}
	}
	for (size_t i = 0; i < queue.size(); ++i) {
		const int pos = queue[i], x = pos % original.SizeX, y = pos / original.SizeX;
		const auto fill = [&](int next) {
			if (!filled[next]) {
				filled[next] = true;
				result.profile[next] = result.profile[pos];
				queue.push_back(next);
			}
		};
		if (x)
			fill(pos - 1);
		if (x + 1 < original.SizeX)
			fill(pos + 1);
		if (y)
			fill(pos - original.SizeX);
		if (y + 1 < original.SizeY)
			fill(pos + original.SizeX);
	}

	result.advance = original.SizeX - original.LeftOffs - original.RightOffs;
	result.bitmap = icons_only_ || cp < 32;
	if (result.bitmap) {
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
