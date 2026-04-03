#include "legacy_ttf_draw.h"

#include "xgraph.h"

#include <algorithm>
#include <climits>
#include <cstdio>
#include <cstdlib>
#include <unordered_map>

namespace text
{

namespace
{

bool file_exists(const char* file_name)
{
	if(!file_name || !*file_name)
		return false;

	FILE* fh = fopen(file_name, "rb");
	if(!fh)
		return false;

	fclose(fh);
	return true;
}

uint32_t decode_legacy_char(unsigned char ch,LegacyEncoding encoding)
{
	switch(encoding){
		case LegacyEncoding::ASCII:
			return ch < 0x80 ? ch : '?';
		case LegacyEncoding::CP866:
			return cp866_to_unicode(ch);
		case LegacyEncoding::CP1251:
			return cp1251_to_unicode(ch);
	}
	return '?';
}

const GlyphBitmap* get_renderable_glyph(TtfFontFace& face,uint32_t codepoint)
{
	const GlyphBitmap* glyph = face.get_glyph(codepoint);
	if(glyph && glyph->provided)
		return glyph;

	glyph = face.get_glyph('?');
	if(glyph && glyph->provided)
		return glyph;

	return nullptr;
}

int text_line_height(const TtfFontFace& face)
{
	return std::max(face.get_line_skip(), face.get_height());
}

std::string make_default_ui_face_key(const std::string& font_path,int target_height,int hinting,bool kerning,int outline)
{
	return font_path + "#" +
	       std::to_string(target_height) + "#" +
	       std::to_string(hinting) + "#" +
	       (kerning ? "1" : "0") + "#" +
	       std::to_string(outline);
}

void blit_alpha_mask_8bit(int x,int y,const GlyphBitmap& glyph,int palette_base,int palette_shift,bool clip)
{
	if(glyph.alpha.empty())
		return;

	const int max_palette_step = std::max(0, 255 - palette_base);

	for(int gy = 0; gy < glyph.height; gy++){
		for(int gx = 0; gx < glyph.width; gx++){
			unsigned int level = glyph.alpha[(size_t)gy * (size_t)glyph.pitch + (size_t)gx];
			if(palette_shift > 0)
				level >>= palette_shift;
			if(!level)
				continue;

			level = std::min(level, (unsigned int)max_palette_step);
			if(!level)
				continue;

			if(clip)
				XGR_SetPixel(x + gx, y + gy, palette_base + (int)level);
			else
				XGR_SetPixelFast(x + gx, y + gy, palette_base + (int)level);
		}
	}
}

}

const std::string& default_ui_ttf_font_path(void)
{
	static const std::string resolved = []() -> std::string {
		const char* env_font = std::getenv("VANGERS_UI_TTF_FONT");
		if(file_exists(env_font))
			return env_font;

		static const char* candidates[] = {
			"resource/fonts/ui.ttf",
			"/usr/share/fonts/droid/DroidSansFallback.ttf",
			"/usr/share/fonts/droid/DroidSansJapanese.ttf",
			"/usr/share/fonts/ja-ipafonts/ipag.ttf",
			"/usr/share/fonts/takao-fonts/TakaoPGothic.ttf",
			"/usr/share/fonts/takao-fonts/TakaoGothic.ttf"
		};

		for(const char* candidate : candidates){
			if(file_exists(candidate))
				return candidate;
		}

		return std::string();
	}();

	return resolved;
}

std::shared_ptr<TtfFontFace> default_ui_ttf_face(int target_height,int hinting,bool kerning,int outline)
{
	const std::string& font_path = default_ui_ttf_font_path();
	if(font_path.empty())
		return nullptr;

	target_height = std::max(6, target_height);

	static std::unordered_map<std::string, std::weak_ptr<TtfFontFace>> face_cache;
	const std::string key = make_default_ui_face_key(font_path, target_height, hinting, kerning, outline);
	auto cache_it = face_cache.find(key);
	if(cache_it != face_cache.end()){
		auto cached_face = cache_it->second.lock();
		if(cached_face)
			return cached_face;
	}

	std::shared_ptr<TtfFontFace> best_face;
	int best_delta = INT_MAX;

	for(int point_size = std::max(6, target_height - 6); point_size <= target_height + 6; point_size++){
		auto face = TtfFontManager::instance().get_face(font_path, point_size, hinting, kerning, outline);
		if(!face)
			continue;

		const int delta = abs(face->get_height() - target_height);
		if(delta < best_delta){
			best_delta = delta;
			best_face = face;
		}
	}

	if(best_face)
		face_cache[key] = best_face;

	return best_face;
}

int measure_legacy_ttf_text_width(std::string_view text,TtfFontFace& face,LegacyEncoding encoding,int hspace)
{
	int max_width = 0;
	int pen_x = 0;
	int line_min_x = INT_MAX;
	int line_max_x = 0;
	int line_end_x = 0;

	for(unsigned char ch : text){
		if(ch == '\r')
			continue;

		if(ch == '\n'){
			if(line_min_x != INT_MAX)
				max_width = std::max(max_width, std::max(line_max_x, line_end_x) - std::min(0, line_min_x));
			else
				max_width = std::max(max_width, 0);

			pen_x = 0;
			line_min_x = INT_MAX;
			line_max_x = 0;
			line_end_x = 0;
			continue;
		}

		const GlyphBitmap* glyph = get_renderable_glyph(face, decode_legacy_char(ch, encoding));
		if(glyph){
			line_min_x = std::min(line_min_x, pen_x + glyph->minx);
			line_max_x = std::max(line_max_x, pen_x + glyph->maxx);
			pen_x += std::max(glyph->advance, 0) + hspace;
			line_end_x = pen_x;
		}
	}

	if(line_min_x != INT_MAX)
		max_width = std::max(max_width, std::max(line_max_x, line_end_x) - std::min(0, line_min_x));

	return max_width;
}

int measure_legacy_ttf_text_height(std::string_view text,const TtfFontFace& face,int vspace)
{
	int lines = 1;
	for(unsigned char ch : text){
		if(ch == '\n')
			lines++;
	}

	return lines * text_line_height(face) + lines * vspace;
}

void draw_legacy_ttf_text_8bit(int x,int y,int color,std::string_view text,TtfFontFace& face,LegacyEncoding encoding,int hspace,int vspace,bool clip)
{
	const int palette_shift = (color >> 16) & 0xFF;
	const int palette_base = color & 0xFFFF;
	const int ascent = face.get_ascent();
	const int line_height = text_line_height(face);

	int pen_x = x;
	int pen_y = y;

	for(unsigned char ch : text){
		if(ch == '\r')
			continue;

		if(ch == '\n'){
			pen_x = x;
			pen_y += line_height + vspace;
			continue;
		}

		const GlyphBitmap* glyph = get_renderable_glyph(face, decode_legacy_char(ch, encoding));
		if(!glyph)
			continue;

		const int draw_x = pen_x + glyph->minx;
		const int draw_y = pen_y + ascent - glyph->maxy;
		blit_alpha_mask_8bit(draw_x, draw_y, *glyph, palette_base, palette_shift, clip);
		pen_x += std::max(glyph->advance, 0) + hspace;
	}
}

}
