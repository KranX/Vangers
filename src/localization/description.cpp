#include "description.h"
#include "actint/ttf_font.h"

#include "text/layout.h"

#include <algorithm>
#include <array>
#include <stdexcept>

namespace vangers::localization {
namespace {
constexpr std::array<std::string_view, 79> items{{
	{},						// 0: not a stock item
	"descriptions.item.01", // 1
	"descriptions.item.02", // 2
	"descriptions.item.03", // 3
	"descriptions.item.04", // 4
	"descriptions.item.05", // 5
	"descriptions.item.06", // 6
	"descriptions.item.07", // 7
	"descriptions.item.08", // 8
	"descriptions.item.09", // 9
	"descriptions.item.10", // 10
	"descriptions.item.11", // 11
	"descriptions.item.12", // 12
	"descriptions.item.13", // 13
	"descriptions.item.14", // 14
	"descriptions.item.14", // 15
	"descriptions.item.16", // 16
	"descriptions.item.16", // 17
	"descriptions.item.18", // 18
	"descriptions.item.18", // 19
	"descriptions.item.20", // 20
	"descriptions.item.21", // 21
	"descriptions.item.22", // 22
	"descriptions.item.24", // 23
	"descriptions.item.24", // 24
	"descriptions.item.26", // 25
	"descriptions.item.26", // 26
	"descriptions.item.27", // 27
	"descriptions.item.28", // 28
	"descriptions.item.29", // 29
	"descriptions.item.30", // 30
	"descriptions.item.31", // 31
	"descriptions.item.32", // 32
	"descriptions.item.33", // 33
	"descriptions.item.34", // 34
	"descriptions.item.35", // 35
	"descriptions.item.36", // 36
	"descriptions.item.37", // 37
	"descriptions.item.38", // 38
	"descriptions.item.39", // 39
	"descriptions.item.40", // 40
	"descriptions.item.56", // 41
	"descriptions.item.55", // 42
	"descriptions.item.41", // 43
	"descriptions.item.42", // 44
	"descriptions.item.43", // 45
	"descriptions.item.44", // 46
	"descriptions.item.45", // 47
	"descriptions.item.46", // 48
	"descriptions.item.47", // 49
	"descriptions.item.48", // 50
	"descriptions.item.49", // 51
	"descriptions.item.50", // 52
	"descriptions.item.51", // 53
	"descriptions.item.52", // 54
	"descriptions.item.53", // 55
	"descriptions.item.54", // 56
	"descriptions.item.61", // 57
	"descriptions.item.59", // 58
	"descriptions.item.57", // 59
	"descriptions.item.58", // 60
	"descriptions.item.60", // 61
	"descriptions.item.62", // 62
	"descriptions.item.63", // 63
	"descriptions.item.64", // 64
	"descriptions.item.65", // 65
	"descriptions.item.66", // 66
	"descriptions.item.67", // 67
	"descriptions.item.68", // 68
	"descriptions.item.69", // 69
	"descriptions.item.70", // 70
	"descriptions.item.60", // 71
	"descriptions.item.60", // 72
	"descriptions.item.61", // 73
	"descriptions.item.59", // 74
	"descriptions.item.57", // 75
	"descriptions.item.72", // 76
	"descriptions.item.77", // 77
	"descriptions.item.71", // 78
}};
constexpr std::array<std::string_view, 39> mechos{{
	"descriptions.mechos.00", // 0
	"descriptions.mechos.01", // 1
	"descriptions.mechos.02", // 2
	"descriptions.mechos.03", // 3
	"descriptions.mechos.04", // 4
	"descriptions.mechos.05", // 5
	"descriptions.mechos.06", // 6
	"descriptions.mechos.07", // 7
	"descriptions.mechos.09", // 8
	"descriptions.mechos.10", // 9
	"descriptions.mechos.11", // 10
	"descriptions.mechos.12", // 11
	"descriptions.mechos.13", // 12
	"descriptions.mechos.14", // 13
	"descriptions.mechos.18", // 14
	"descriptions.mechos.19", // 15
	"descriptions.mechos.20", // 16
	"descriptions.mechos.21", // 17
	"descriptions.mechos.22", // 18
	"descriptions.mechos.08", // 19
	"descriptions.mechos.08", // 20
	"descriptions.mechos.08", // 21
	"descriptions.mechos.08", // 22
	"descriptions.mechos.15", // 23
	"descriptions.mechos.15", // 24
	"descriptions.mechos.15", // 25
	"descriptions.mechos.15", // 26
	"descriptions.mechos.16", // 27
	"descriptions.mechos.16", // 28
	"descriptions.mechos.16", // 29
	"descriptions.mechos.16", // 30
	"descriptions.mechos.17", // 31
	"descriptions.mechos.17", // 32
	"descriptions.mechos.17", // 33
	"descriptions.mechos.17", // 34
	"descriptions.mechos.35", // 35
	"descriptions.mechos.35", // 36
	"descriptions.mechos.35", // 37
	"descriptions.mechos.35", // 38
}};
} // namespace

std::string_view description_key(DescriptionKind kind, int id) {
	const auto lookup = [id](const auto &keys) -> std::string_view {
		return id >= 0 && std::size_t(id) < keys.size() ? keys[id] : std::string_view{};
	};
	return kind == DescriptionKind::Item ? lookup(items) : lookup(mechos);
}

Description::Description(std::string_view text, DescriptionStyle presentation)
	: pixels_(width * height, 0xFF000000u) {
	if (text.find('\f') != std::string_view::npos)
		throw std::invalid_argument("Descriptions cannot add pages to the fixed panel");
	if (text.empty())
		return;
	actint::TtfFont font;
	actint::FontStyle style;
	const bool compact = presentation == DescriptionStyle::CompactCentered;
	style.cell_width = compact ? 9 : 10;
	style.cell_height = 16;
	style.spacing = 0;
	style.line_spacing = 0;
	style.highlight_numbers = false;
	const int margin = compact ? 36 : 30;
	const auto lines = text::layout_lines(
		text,
		width - 2 * margin,
		[&](std::string_view s) { return font.layout(s, style).width; },
		text::LineMode::Paragraphs
	);
	const int line_advance = compact ? 12 : 13;
	const int block_height = (int(lines.size()) - 1) * line_advance + style.cell_height;
	const int top_margin =
		presentation != DescriptionStyle::RegularTop ? (height - block_height) / 2 : 24;
	if (top_margin < 4 || top_margin + block_height > height - 4)
		throw std::runtime_error("Description exceeds its fixed 340x255 panel");
	int y = top_margin;
	for (const auto &line : lines) {
		const auto run = font.layout(line, style);
		if (run.width > width - 2 * margin)
			throw std::runtime_error("Description word exceeds its fixed panel");
		if (run.image) {
			const auto raster = run.image->rasterize(run.ink_bounds.w, run.ink_bounds.h);
			const int left = (width - run.width) / 2 + run.ink_bounds.x;
			const int top = y + run.ink_bounds.y;
			for (int row = 0; row < raster.height; ++row)
				for (int col = 0; col < raster.width; ++col) {
					const auto alpha = raster.pixels[std::size_t(row) * raster.width + col].alpha;
					const int x = left + col, dy = top + row;
					if (alpha && x >= 0 && x < width && dy >= 0 && dy < height) {
						auto &pixel = pixels_[std::size_t(dy) * width + x];
						// Compact rows may overlap. Union their coverage instead of
						// letting a weaker edge erase the preceding glyph's stroke.
						const auto coverage = std::max(uint32_t(alpha), (pixel >> 16) & 255);
						pixel = 0xFF000000u | (coverage << 16) | ((coverage * 211 / 255) << 8);
					}
				}
		}
		y += line_advance;
	}
}

void Description::draw(uint32_t *target, int w, int h, int x, int y, float brightness) const {
	if (!target || w <= 0 || h <= 0)
		return;
	brightness = std::clamp(brightness, 0.0f, 1.0f);
	const int first_x = std::max(0, -x), last_x = std::min(width, w - x);
	const int first_y = std::max(0, -y), last_y = std::min(height, h - y);
	if (first_x >= last_x || first_y >= last_y)
		return;
	for (int row = first_y; row < last_y; ++row) {
		if (brightness == 1.0f) {
			std::copy_n(
				pixels_.data() + std::size_t(row) * width + first_x,
				last_x - first_x,
				target + std::size_t(y + row) * w + x + first_x
			);
			continue;
		}
		for (int col = first_x; col < last_x; ++col) {
			const auto colour = pixels_[std::size_t(row) * width + col];
			target[std::size_t(y + row) * w + x + col] =
				0xFF000000u | (uint32_t(((colour >> 16) & 255) * brightness) << 16) |
				(uint32_t(((colour >> 8) & 255) * brightness) << 8);
		}
	}
}
} // namespace vangers::localization
