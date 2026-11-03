#include "ttf_font.h"

#include "settings/text_encoding.h"
#include "text/grain.h"
#include "text/inside_distance.h"

#include <SDL3_ttf/SDL_ttf.h>

#include <algorithm>
#include <cmath>
#include <map>
#include <stdexcept>

namespace vangers::actint {
extern const unsigned char font_data[];
extern const std::size_t font_data_size;
extern const unsigned char menu_font_data[], display_font_data[];
extern const std::size_t menu_font_data_size, display_font_data_size;
namespace {
void require(bool success) {
	if (!success)
		throw std::runtime_error(std::string("actint TTF: ") + SDL_GetError());
}

// Bilinear samples with transparent/zero height outside the glyph's ink box.
template<class Sample> double sample_grid(int w, int h, double x, double y, Sample value) {
	const int ix = int(std::floor(x)), iy = int(std::floor(y));
	const double u = x - ix, v = y - iy;
	const auto sample = [&](int px, int py) {
		return px < 0 || py < 0 || px >= w || py >= h ? 0.0 : double(value(px, py));
	};
	return (sample(ix, iy) * (1 - u) + sample(ix + 1, iy) * u) * (1 - v) +
		   (sample(ix, iy + 1) * (1 - u) + sample(ix + 1, iy + 1) * u) * v;
}

double glyph_alpha(const SDL_Surface &image, double x, double y) {
	return sample_grid(image.w, image.h, x, y, [&](int px, int py) {
		const auto *row = reinterpret_cast<const uint32_t *>(
			static_cast<const uint8_t *>(image.pixels) + py * image.pitch
		);
		return double(row[px] >> 24) / 255.0;
	});
}

// Small optical weight compensation, confined to the TTF ink box. Work from
// the unmodified mask so traversal order cannot enlarge later pixels again.
void strengthen_strokes(SDL_Surface &image, double sx, double sy) {
	std::vector<uint8_t> alpha(size_t(image.w) * image.h);
	for (int y = 0; y < image.h; ++y)
		for (int x = 0; x < image.w; ++x) {
			const double coverage = std::max(
				{glyph_alpha(image, x, y),
					glyph_alpha(image, x - 0.2 * sx, y),
					glyph_alpha(image, x + 0.2 * sx, y),
					glyph_alpha(image, x, y - 0.2 * sy),
					glyph_alpha(image, x, y + 0.2 * sy)}
			);
			alpha[size_t(y) * image.w + x] = uint8_t(std::lround(255 * coverage));
		}
	for (int y = 0; y < image.h; ++y) {
		auto *row =
			reinterpret_cast<uint32_t *>(static_cast<uint8_t *>(image.pixels) + y * image.pitch);
		for (int x = 0; x < image.w; ++x)
			row[x] = (row[x] & 0x00ffffffu) | (uint32_t(alpha[size_t(y) * image.w + x]) << 24);
	}
}

// A shallow convex profile with small height irregularities. Lighting is derived
// from this surface, rather than sprinkling brightness noise on a flat fill.
std::vector<float> glyph_heights(const SDL_Surface &image, double sx, double sy) {
	auto heights = text::inside_distance(image.w, image.h, [&](int x, int y) {
		const auto *row = reinterpret_cast<const uint32_t *>(
			static_cast<const uint8_t *>(image.pixels) + y * image.pitch
		);
		return row[x] >> 24;
	});
	for (int y = 0; y < image.h; ++y)
		for (int x = 0; x < image.w; ++x) {
			auto &height = heights[size_t(y) * image.w + x];
			const double distance = std::max(0.0, (height - 0.5) / std::min(sx, sy));
			const double profile = std::sin(std::min(1.0, distance / 1.2) * 1.5707963268);
			height = float(profile * (1 + 0.32 * text::smooth_grain(x / sx, y / sy)));
		}
	return heights;
}

double surface_height(const std::vector<float> &heights, int w, int h, double x, double y) {
	return sample_grid(w, h, x, y, [&](int px, int py) { return heights[size_t(py) * w + px]; });
}

// Contrast only the face coverage, after height/shadow generation. Preserve
// transparent/opaque samples and tiny strokes; never threshold the glyph mask.
uint8_t sharpen_coverage(uint8_t alpha, double amount) {
	if (!alpha || alpha == 255 || amount == 0)
		return alpha;
	const double a = alpha / 255.0;
	const double coverage = a + amount * a * (1 - a) * (2 * a - 1);
	return uint8_t(std::clamp(int(std::lround(255 * coverage)), 1, 254));
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
	int cell_width = 10, cell_height = 18, baseline = 15, tracking = 1;

	explicit Face(const std::string &path, FontFamily family = FontFamily::Text) {
		const unsigned char *bytes = font_data;
		std::size_t size = font_data_size;
		if (family != FontFamily::Text) {
			cell_height = 50;
			baseline = 44;
			tracking = 5;
			cell_width = family == FontFamily::Menu ? 45 : 26;
			bytes = family == FontFamily::Menu ? menu_font_data : display_font_data;
			size = family == FontFamily::Menu ? menu_font_data_size : display_font_data_size;
		}
		require(TTF_Init());
		const auto open = [&path, bytes, size](float points) {
			if (!path.empty())
				return TTF_OpenFont(path.c_str(), points);
			return TTF_OpenFontIO(SDL_IOFromConstMem(bytes, size), true, points);
		};
		metrics = open(cell_height * 100);
		if (metrics)
			drawing = open(cell_height);
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
		// Runs are immutable. Retain only the last native-size ACS raster; it
		// stores palette indices, not RGB, so fades still resolve at presentation.
		mutable xgr::text::Raster surface_cache{};

		xgr::text::Raster rasterize(int output_width, int output_height) const override {
			const bool material = style.range && style.embossed;
			if (material && surface_cache.width == output_width &&
				surface_cache.height == output_height)
				return surface_cache;
			xgr::text::Raster raster{output_width,
				output_height,
				std::vector<xgr::text::Pixel>(size_t(output_width) * output_height)};
			const double sx = double(output_width) / width, sy = double(output_height) / height;
			// Restrict edge contrast to small native glyphs; fade it smoothly
			// between 40 and 64 pixel cells rather than sharpening large text.
			const double edge_contrast =
				std::clamp((64.0 - style.cell_height * sy) / 24.0, 0.0, 1.0);
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
				std::max(
					1,
					int(std::lround(
						style.cell_width * double(face->cell_height) / face->cell_width * sx * 72
					))
				),
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
							if (value && (style.range || value == 1 || glyph.border) &&
								!(style.range && style.zero_range_transparent &&
									colour == glyph.colour))
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
					int(std::lround(
						(glyph.y + style.cell_height * double(face->baseline) / face->cell_height) *
						sy
					)) -
					maxy;
				const int w = image->w;
				const int h = image->h;
				if (material)
					strengthen_strokes(*image, sx, sy);
				const auto heights =
					material ? glyph_heights(*image, sx, sy) : std::vector<float>{};
				if (material) {
					const int pad_x = int(std::ceil(sx)), pad_y = int(std::ceil(sy));
					for (int y = -pad_y; y < h + pad_y; ++y)
						for (int x = -pad_x; x < w + pad_x; ++x) {
							if (left + x < 0 || top + y < 0 || left + x >= output_width ||
								top + y >= output_height)
								continue;
							const int coverage =
								int(std::lround(255 * glyph_alpha(*image, x + sx, y - 0.5 * sy)));
							// Slightly denser shadow, with the same support and offset.
							const auto alpha = uint8_t(std::min(255, (115 * coverage + 50) / 100));
							const int level = 2 >> std::min(7, int(glyph.range_size));
							if (!alpha || (style.zero_range_transparent && !level))
								continue;
							auto &pixel = raster.pixels[size_t(top + y) * output_width + left + x];
							pixel.border = uint8_t(glyph.colour + level);
							pixel.border_alpha = std::max(pixel.border_alpha, alpha);
						}
				}
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
						int tone = 31;
						if (material) {
							const double nx =
								2.0 * (surface_height(heights, w, h, x - 0.5 * sx, y) -
										  surface_height(heights, w, h, x + 0.5 * sx, y));
							const double ny =
								2.0 * (surface_height(heights, w, h, x, y - 0.5 * sy) -
										  surface_height(heights, w, h, x, y + 0.5 * sy));
							const double light = (-0.6 * nx - 0.4 * ny + 0.9) /
												 std::sqrt((nx * nx + ny * ny + 1) * 1.33);
							tone =
								std::clamp(int(std::lround(4 + 27 * std::max(0.0, light))), 2, 31);
						}
						const int ramp =
							style.range ? (tone >> std::min(7, int(glyph.range_size))) : 0;
						if (style.range && style.zero_range_transparent && !ramp)
							continue;
						pixel.colour = uint8_t(glyph.colour + ramp);
						pixel.alpha = material ? sharpen_coverage(alpha, edge_contrast) : alpha;
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
			if (material)
				surface_cache = raster;
			return raster;
		}
	};
};

TtfFont::TtfFont(): TtfFont("") {}
TtfFont::TtfFont(FontFamily family): face_(std::make_shared<Face>("", family)) {}
TtfFont::TtfFont(const std::string &path, FontFamily family)
	: face_(std::make_shared<Face>(path, family)) {}
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
		// ACS retains its fixed cells. Proportional roles remove the face's
		// baked-in tracking before applying the caller's spacing.
		x += style.fixed_pitch ? style.cell_width + style.spacing
							   : (face_->advance(cp) / 100.0 - face_->tracking) * style.cell_width /
										 face_->cell_width +
									 style.spacing;
	}
	maximum = std::max(maximum, x);
	const int width = std::max(0, int(std::ceil(maximum)));
	const int height = int(y) + style.cell_height + style.line_spacing;
	if (run->glyphs.empty())
		return {width, height, {}, nullptr};
	// Border ink can extend outside the advance without changing hit boxes.
	const int padding = style.range && style.embossed ? 2 : 1;
	run->width = width + 2 * padding;
	run->height = height + 2 * padding;
	for (auto &glyph : run->glyphs) {
		glyph.x += padding;
		glyph.y += padding;
	}
	return {width, height, {-padding, -padding, run->width, run->height}, run};
}

} // namespace vangers::actint
