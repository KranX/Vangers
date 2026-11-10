#include "ttf_font.h"

#include "settings/text_encoding.h"
#include "text/glyph_surface.h"
#include "text/utf8.h"

#include <SDL3_ttf/SDL_ttf.h>

#include <algorithm>
#include <cmath>
#include <iostream>
#include <map>
#include <set>
#include <stdexcept>

namespace vangers::actint {
extern const unsigned char font_data[];
extern const std::size_t font_data_size;
extern const unsigned char menu_font_data[], display_font_data[];
extern const std::size_t menu_font_data_size, display_font_data_size;
extern const unsigned char fallback_font_data[];
extern const std::size_t fallback_font_data_size;
namespace {
constexpr double fallback_em_ratio = .84;

void require(bool success) {
	if (!success)
		throw std::runtime_error(std::string("actint TTF: ") + SDL_GetError());
}

double glyph_alpha(const SDL_Surface &image, double x, double y) {
	return text::glyph::sample(image.w, image.h, x, y, [&](int px, int py) {
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

struct Glyph {
	Uint32 codepoint;
	double x, y;
	uint8_t colour, border, range_size;
	bool fallback = false;
};
} // namespace

struct TtfFont::Face {
	TTF_Font *metrics = nullptr;
	TTF_Font *drawing = nullptr;
	std::map<Uint32, int> advances;
	int cell_width = 10, cell_height = 18, baseline = 15, tracking = 1;
	std::shared_ptr<Face> fallback;

	explicit Face(
		const std::string &path,
		FontFamily family = FontFamily::Text,
		bool backup = false
	) {
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
		if (backup) {
			bytes = fallback_font_data;
			size = fallback_font_data_size;
			cell_width = cell_height = 100;
			baseline = 88;
			tracking = 0;
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

	Face &backup() {
		// One lazy face shared by all three roles; runs retain its lifetime.
		static std::weak_ptr<Face> shared;
		if (!fallback) {
			fallback = shared.lock();
			if (!fallback) {
				fallback = std::make_shared<Face>("", FontFamily::Text, true);
				shared = fallback;
			}
		}
		return *fallback;
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
			const double em =
				style.fixed_pitch
					? std::min(double(style.cell_width), style.cell_height * fallback_em_ratio)
					: style.cell_height * fallback_em_ratio;
			if (face->fallback) {
				auto *font = face->fallback->drawing;
				TTF_SetFontHinting(font, em * sy <= 18 ? TTF_HINTING_LIGHT : TTF_HINTING_NONE);
				require(TTF_SetFontSizeDPI(
					font,
					1,
					std::max(1, int(std::lround(em * sx * 72))),
					std::max(1, int(std::lround(em * sy * 72)))
				));
			}
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
				auto &source = glyph.fallback ? *face->fallback : *face;
				int minx, maxy;
				require(TTF_GetGlyphMetrics(
					source.drawing, glyph.codepoint, &minx, nullptr, nullptr, &maxy, nullptr
				));
				std::unique_ptr<SDL_Surface, decltype(&SDL_DestroySurface)> image(
					TTF_GetGlyphImage(source.drawing, glyph.codepoint, nullptr), SDL_DestroySurface
				);
				require(bool(image));
				const int left = int(std::lround(glyph.x * sx + minx));
				const double baseline =
					glyph.fallback
						? (style.cell_height - em) / 2 + em * source.baseline / source.cell_height
						: style.cell_height * double(face->baseline) / face->cell_height;
				const int top = int(std::lround((glyph.y + baseline) * sy)) - maxy;
				const int w = image->w;
				const int h = image->h;
				if (material)
					strengthen_strokes(*image, sx, sy);
				const auto alpha_at = [&](int x, int y) {
					const auto *row = reinterpret_cast<const uint32_t *>(
						static_cast<const uint8_t *>(image->pixels) + y * image->pitch
					);
					return row[x] >> 24;
				};
				const auto heights =
					material ? text::glyph::heights(w, h, sx, sy, alpha_at) : std::vector<float>{};
				if (material) {
					const int pad_x = int(std::ceil(text::glyph::padding * sx)),
							  pad_y = int(std::ceil(text::glyph::padding * sy));
					for (int y = -pad_y; y < h + pad_y; ++y)
						for (int x = -pad_x; x < w + pad_x; ++x) {
							if (left + x < 0 || top + y < 0 || left + x >= output_width ||
								top + y >= output_height)
								continue;
							const int coverage = int(std::lround(
								255 * text::glyph::side_coverage(heights, w, h, sx, sy, x, y)
							));
							// Retain the dark-body tone/opacity without offsetting a copy
							// of the glyph mask.
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
						if (material)
							tone = text::glyph::tone(heights, w, h, sx, sy, x, y);
						const int ramp =
							style.range ? (tone >> std::min(7, int(glyph.range_size))) : 0;
						if (style.range && style.zero_range_transparent && !ramp)
							continue;
						pixel.colour = uint8_t(glyph.colour + ramp);
						pixel.alpha =
							material ? text::glyph::coverage(w, h, x, y, edge_contrast, alpha_at)
									 : alpha;
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

bool TtfFont::has_glyph(char32_t cp) const {
	return TTF_FontHasGlyph(face_->metrics, cp) || TTF_FontHasGlyph(face_->backup().metrics, cp);
}

std::vector<char32_t> TtfFont::missing_glyphs(std::string_view text) const {
	if (!settings::is_valid_utf8(text))
		throw std::invalid_argument("actint TTF: invalid UTF-8");
	const auto composed = text::compose_pairs(text);
	text = composed;
	std::set<char32_t> missing;
	const char *cursor = text.data();
	std::size_t left = text.size();
	while (left) {
		const auto cp = SDL_StepUTF8(&cursor, &left);
		if (cp >= 32 && cp != 0x2028 && !has_glyph(cp))
			missing.insert(cp);
	}
	return {missing.begin(), missing.end()};
}

TextLayout TtfFont::layout(std::string_view text, const FontStyle &style) const {
	if (!settings::is_valid_utf8(text))
		throw std::invalid_argument("actint TTF: invalid UTF-8");
	if (style.cell_width <= 0 || style.cell_height <= 0 || style.line_spacing < 0)
		throw std::invalid_argument("actint TTF: invalid font dimensions");
	const auto composed = text::compose_pairs(text);
	text = composed;
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
		bool backup = !TTF_FontHasGlyph(face_->metrics, cp);
		if (backup && !TTF_FontHasGlyph(face_->backup().metrics, cp)) {
			static std::set<Uint32> reported;
			if (reported.insert(cp).second)
				std::cerr << "Missing font glyph U+" << std::hex << cp << std::dec << '\n';
			cp = '?'; // Presentation only; retain the original UTF-8 at the input boundary.
			backup = false;
		}
		run->glyphs.push_back(
			{cp, x, y, uint8_t(colour), uint8_t(colour >> 8), uint8_t(colour_size), backup}
		);
		// ACS retains its fixed cells. Proportional roles remove the face's
		// baked-in tracking before applying the caller's spacing.
		double advance = style.cell_width;
		if (!style.fixed_pitch) {
			if (backup) {
				const double em = style.cell_height * fallback_em_ratio;
				advance =
					face_->fallback->advance(cp) / (100.0 * face_->fallback->cell_height) * em;
			} else {
				advance = (face_->advance(cp) / 100.0 - face_->tracking) * style.cell_width /
						  face_->cell_width;
			}
		}
		x += advance + style.spacing;
	}
	maximum = std::max(maximum, x);
	const int width = std::max(0, int(std::ceil(maximum)));
	const int height = int(y) + style.cell_height + style.line_spacing;
	if (run->glyphs.empty())
		return {width, height, {}, nullptr};
	// Border ink can extend outside the advance without changing hit boxes.
	const int padding = style.range && style.embossed ? text::glyph::padding : 1;
	run->width = width + 2 * padding;
	run->height = height + 2 * padding;
	for (auto &glyph : run->glyphs) {
		glyph.x += padding;
		glyph.y += padding;
	}
	return {width, height, {-padding, -padding, run->width, run->height}, run};
}

} // namespace vangers::actint
