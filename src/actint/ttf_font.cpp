#include "ttf_font.h"

#include "settings/text_encoding.h"

#include <SDL3_ttf/SDL_ttf.h>

#include <algorithm>
#include <cmath>
#include <map>
#include <stdexcept>

namespace vangers::actint {
extern const unsigned char font_data[];
extern const std::size_t font_data_size;
namespace {
void require(bool success) {
	if (!success)
		throw std::runtime_error(std::string("actint TTF: ") + SDL_GetError());
}

struct Glyph {
	Uint32 codepoint;
	double x, y;
	uint8_t colour, border, range_size;
};
} // namespace

struct TtfFont::Face {
	TTF_Font *metrics = nullptr;
	TTF_Font *drawing = nullptr;
	std::map<Uint32, int> advances;

	explicit Face(const std::string &path) {
		require(TTF_Init());
		const auto open = [&path](float size) {
			if (!path.empty())
				return TTF_OpenFont(path.c_str(), size);
			return TTF_OpenFontIO(SDL_IOFromConstMem(font_data, font_data_size), true, size);
		};
		metrics = open(1800);
		if (metrics)
			drawing = open(18);
		if (!metrics || !drawing) {
			const std::string error = SDL_GetError();
			TTF_CloseFont(drawing);
			TTF_CloseFont(metrics);
			TTF_Quit();
			throw std::runtime_error("actint TTF: " + path + ": " + error);
		}
		TTF_SetFontHinting(metrics, TTF_HINTING_NONE);
		TTF_SetFontHinting(drawing, TTF_HINTING_NONE);
		TTF_SetFontKerning(metrics, false);
		TTF_SetFontKerning(drawing, false);
	}

	~Face() {
		TTF_CloseFont(drawing);
		TTF_CloseFont(metrics);
		TTF_Quit();
	}

	int advance(Uint32 codepoint) {
		const auto found = advances.find(codepoint);
		if (found != advances.end())
			return found->second;
		int result = 0;
		require(
			TTF_GetGlyphMetrics(metrics, codepoint, nullptr, nullptr, nullptr, nullptr, &result)
		);
		advances.emplace(codepoint, result);
		return result;
	}

	class Run final: public xgr::text::Image {
	  public:
		std::shared_ptr<Face> face;
		FontStyle style;
		std::vector<Glyph> glyphs;
		int width, height;

		xgr::text::Raster rasterize(int output_width, int output_height) const override {
			xgr::text::Raster raster{output_width,
				output_height,
				std::vector<xgr::text::Pixel>(size_t(output_width) * output_height)};
			const double sx = double(output_width) / width, sy = double(output_height) / height;
			// At the source face's 18-pixel size (or below), use light vertical
			// grid fitting. Keep enlarged outlines and layout metrics
			// unhinted; never resample a low-resolution glyph to sharpen it.
			TTF_SetFontHinting(
				face->drawing, style.cell_height * sy <= 18.0 ? TTF_HINTING_LIGHT : TTF_HINTING_NONE
			);
			// One point at N*72 DPI is N pixels. Give FreeType both scales so
			// legacy narrow/wide cells still rasterize outlines, not stretched bitmaps.
			require(TTF_SetFontSizeDPI(
				face->drawing,
				1,
				std::max(1, int(std::lround(style.cell_width * 18.0 / 10 * sx * 72))),
				std::max(1, int(std::lround(style.cell_height * sy * 72)))
			));
			for (const auto &glyph : glyphs) {
				if (glyph.codepoint < 32 && style.icons) {
					const auto &icon = (*style.icons)[glyph.codepoint];
					const int left = int(std::lround((glyph.x - icon.left) * sx));
					const int top = int(std::lround(glyph.y * sy));
					const int w = int(std::lround(style.cell_width * sx));
					const int h = int(std::lround(style.cell_height * sy));
					for (int y = 0; y < h; ++y)
						for (int x = 0; x < w; ++x) {
							if (left + x < 0 || top + y < 0 || left + x >= output_width ||
								top + y >= output_height)
								continue;
							const auto value =
								icon.pixels
									[size_t(y * style.cell_height / h) * style.cell_width +
										x * style.cell_width / w];
							const auto colour =
								style.range
									? glyph.colour + (value >> std::min(7, int(glyph.range_size)))
								: value == 1 ? glyph.colour
											 : glyph.border;
							if (value && (style.range || value == 1 || glyph.border))
								raster.pixels[size_t(top + y) * output_width + left + x] = {
									uint8_t(colour), 255};
						}
					continue;
				}
				int minx, maxy;
				require(TTF_GetGlyphMetrics(
					face->drawing, glyph.codepoint, &minx, nullptr, nullptr, &maxy, nullptr
				));
				std::unique_ptr<SDL_Surface, decltype(&SDL_DestroySurface)> image(
					TTF_GetGlyphImage(face->drawing, glyph.codepoint, nullptr), SDL_DestroySurface
				);
				require(bool(image));
				const int left = int(std::lround(glyph.x * sx + minx));
				const int top =
					int(std::lround((glyph.y + style.cell_height * 15.0 / 18) * sy)) - maxy;
				const int w = image->w;
				const int h = image->h;
				for (int y = 0; y < h; ++y) {
					if (top + y < 0 || top + y >= output_height)
						continue;
					const auto *row = reinterpret_cast<const uint32_t *>(
						static_cast<const uint8_t *>(image->pixels) + y * image->pitch
					);
					for (int x = 0; x < w; ++x) {
						if (left + x < 0 || left + x >= output_width)
							continue;
						const uint8_t alpha = row[x] >> 24;
						if (!alpha)
							continue;
						auto &pixel = raster.pixels[size_t(top + y) * output_width + left + x];
						const int ramp =
							style.range ? (31 >> std::min(7, int(glyph.range_size))) : 0;
						pixel.colour = uint8_t(glyph.colour + ramp);
						pixel.alpha = alpha;
						if (!style.range && glyph.border) {
							const int radius_x = std::max(1, int(std::lround(sx)));
							const int radius_y = std::max(1, int(std::lround(sy)));
							for (int by = std::max(0, top + y - radius_y);
								by <= std::min(output_height - 1, top + y + radius_y);
								++by)
								for (int bx = std::max(0, left + x - radius_x);
									bx <= std::min(output_width - 1, left + x + radius_x);
									++bx) {
									auto &border = raster.pixels[size_t(by) * output_width + bx];
									border.border = glyph.border;
									border.border_alpha = std::max(border.border_alpha, alpha);
								}
						}
					}
				}
			}
			return raster;
		}
	};
};

TtfFont::TtfFont(): TtfFont("") {}
TtfFont::TtfFont(const std::string &path): face_(std::make_shared<Face>(path)) {}
TtfFont::~TtfFont() = default;

TextLayout TtfFont::layout(std::string_view text, const FontStyle &style) const {
	if (!settings::is_valid_utf8(text))
		throw std::invalid_argument("actint TTF: invalid UTF-8");
	if (style.cell_width <= 0 || style.cell_height <= 0 || style.line_spacing < 0)
		throw std::invalid_argument("actint TTF: invalid font dimensions");
	auto run = std::make_shared<Face::Run>();
	run->face = face_;
	run->style = style;
	double x = 0, y = 0, maximum = 0;
	const char *cursor = text.data();
	size_t left = text.size();
	while (left) {
		Uint32 cp = SDL_StepUTF8(&cursor, &left);
		if (cp == '\r')
			continue;
		if (cp == '\n') {
			maximum = std::max(maximum, x);
			x = 0;
			y += style.cell_height + style.line_spacing;
			continue;
		}
		const bool number =
			style.highlight_numbers && ((cp >= '0' && cp <= '9') || cp == '$' || cp == 0xE000);
		const auto colour = (number && (style.colour >> 16)) ? style.colour >> 16 : style.colour;
		const auto colour_size =
			(number && (style.colour >> 16)) ? style.colour_size >> 16 : style.colour_size;
		if (cp < 32 && style.icons && cp < style.icons->size() &&
			(*style.icons)[cp].pixels.size() == size_t(style.cell_width) * style.cell_height) {
			const auto &icon = (*style.icons)[cp];
			run->glyphs.push_back(
				{cp, x, y, uint8_t(colour), uint8_t(colour >> 8), uint8_t(colour_size)}
			);
			x += style.cell_width - icon.left - icon.right + style.spacing;
			continue;
		}
		if (!TTF_FontHasGlyph(face_->metrics, cp))
			cp = '?'; // Only a presentation fallback; the caller's UTF-8 stays untouched.
		run->glyphs.push_back(
			{cp, x, y, uint8_t(colour), uint8_t(colour >> 8), uint8_t(colour_size)}
		);
		// The TTF already contains one source pixel of tracking, not two.
		x += (face_->advance(cp) / 100.0 - 1) * style.cell_width / 10.0 + style.spacing;
	}
	maximum = std::max(maximum, x);
	const int width = std::max(0, int(std::ceil(maximum)));
	const int height = int(y) + style.cell_height + style.line_spacing;
	if (run->glyphs.empty())
		return {width, height, {}, nullptr};
	// Border ink can extend outside the advance without changing hit boxes.
	run->width = width + 2;
	run->height = height + 2;
	for (auto &glyph : run->glyphs) {
		glyph.x += 1;
		glyph.y += 1;
	}
	return {width, height, {-1, -1, run->width, run->height}, run};
}

} // namespace vangers::actint
