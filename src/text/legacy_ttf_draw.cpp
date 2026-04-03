#include "legacy_ttf_draw.h"

#include "language_policy.h"
#include "unicode.h"
#include "xgraph.h"

#include <algorithm>
#include <climits>
#include <cstdio>
#include <cstdlib>
#include <sstream>
#include <unordered_map>

namespace text
{

struct RenderableGlyph
{
	TtfFontFace* face = nullptr;
	const GlyphBitmap* glyph = nullptr;

	bool valid(void) const { return face && glyph; }
};

namespace
{

using FaceVector = std::vector<std::shared_ptr<TtfFontFace>>;

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

const std::vector<std::string>& builtin_default_ui_font_candidates(void)
{
	static const std::vector<std::string> candidates = {
		"resource/fonts/ui.ttf",
		"/usr/share/fonts/truetype/noto/NotoSansCJK-Regular.ttc",
		"/usr/share/fonts/opentype/noto/NotoSansCJK-Regular.ttc",
		"/usr/share/fonts/droid/DroidSansFallback.ttf"
	};

	return candidates;
}

const std::vector<std::string>& builtin_japanese_ui_font_candidates(void)
{
	static const std::vector<std::string> candidates = {
		"/usr/share/fonts/droid/DroidSansJapanese.ttf",
		"/usr/share/fonts/ja-ipafonts/ipag.ttf",
		"/usr/share/fonts/takao-fonts/TakaoPGothic.ttf",
		"/usr/share/fonts/takao-fonts/TakaoGothic.ttf",
		"/usr/share/fonts/opentype/noto/NotoSansCJKJP-Regular.otf",
		"/usr/share/fonts/opentype/noto/NotoSansJP-Regular.otf",
		"/usr/share/fonts/opentype/source-han-sans/SourceHanSansJP-Regular.otf",
		"/usr/share/fonts/truetype/noto/NotoSansJP-Regular.ttf"
	};

	return candidates;
}

void append_font_candidates_from_env(std::vector<std::string>& out,const char* env_value)
{
	if(!env_value || !*env_value)
		return;

	std::stringstream stream(env_value);
	std::string candidate;
	while(std::getline(stream, candidate, ':')){
		if(candidate.empty())
			continue;
		if(!file_exists(candidate.c_str()))
			continue;
		if(std::find(out.begin(), out.end(), candidate) == out.end())
			out.push_back(candidate);
	}
}

std::vector<std::string> default_ui_font_candidates(void)
{
	std::vector<std::string> candidates;

	append_font_candidates_from_env(candidates, std::getenv("VANGERS_UI_TTF_FONT"));
	if(language_prefers_japanese_fonts()){
		for(const std::string& candidate : builtin_japanese_ui_font_candidates()){
			if(file_exists(candidate.c_str()) &&
			   std::find(candidates.begin(), candidates.end(), candidate) == candidates.end())
				candidates.push_back(candidate);
		}
	}
	for(const std::string& candidate : builtin_default_ui_font_candidates()){
		if(file_exists(candidate.c_str()) &&
		   std::find(candidates.begin(), candidates.end(), candidate) == candidates.end())
			candidates.push_back(candidate);
	}
	append_font_candidates_from_env(candidates, std::getenv("VANGERS_UI_TTF_FALLBACK"));

	return candidates;
}

std::shared_ptr<TtfFontFace> best_face_for_path(const std::string& font_path,int target_height,int hinting,bool kerning,int outline,int style)
{
	std::shared_ptr<TtfFontFace> best_face;
	int best_delta = INT_MAX;

	for(int point_size = std::max(6, target_height - 6); point_size <= target_height + 6; point_size++){
		auto face = TtfFontManager::instance().get_face(font_path, point_size, hinting, kerning, outline, style);
		if(!face)
			continue;

		const int delta = abs(face->get_height() - target_height);
		if(delta < best_delta){
			best_delta = delta;
			best_face = face;
		}
	}

	return best_face;
}

std::shared_ptr<TtfFontFace> best_hfont_face_for_path(const std::string& font_path,int legacy_height,int hinting,bool kerning,int outline,int style)
{
	std::shared_ptr<TtfFontFace> best_face;
	int best_delta = INT_MAX;

	for(int point_size = std::max(6, legacy_height - 12); point_size <= legacy_height + 24; point_size++){
		auto face = TtfFontManager::instance().get_face(font_path, point_size, hinting, kerning, outline, style);
		if(!face)
			continue;

		const int ink_box_height = std::max(1, face->get_ascent() - face->get_descent());
		const int delta = abs(ink_box_height - legacy_height);
		if(delta < best_delta){
			best_delta = delta;
			best_face = face;
		}
	}

	return best_face;
}

const FaceVector& default_ui_fallback_registry(void)
{
	static const FaceVector empty_faces;
	return empty_faces;
}

std::unordered_map<const TtfFontFace*, FaceVector>& default_ui_fallback_faces(void)
{
	static std::unordered_map<const TtfFontFace*, FaceVector> fallback_faces;
	return fallback_faces;
}

const FaceVector& lookup_fallback_faces(TtfFontFace& face)
{
	auto& registry = default_ui_fallback_faces();
	auto it = registry.find(&face);
	if(it != registry.end())
		return it->second;

	return default_ui_fallback_registry();
}

RenderableGlyph find_face_glyph(TtfFontFace& face,uint32_t codepoint)
{
	const GlyphBitmap* glyph = face.get_glyph(codepoint);
	if(glyph && glyph->provided)
		return RenderableGlyph{ &face, glyph };

	return {};
}

RenderableGlyph get_renderable_glyph(TtfFontFace& face,uint32_t codepoint)
{
	RenderableGlyph glyph = find_face_glyph(face, codepoint);
	if(glyph.valid())
		return glyph;

	for(const auto& fallback_face : lookup_fallback_faces(face)){
		if(!fallback_face)
			continue;

		glyph = find_face_glyph(*fallback_face, codepoint);
		if(glyph.valid())
			return glyph;
	}

	glyph = find_face_glyph(face, '?');
	if(glyph.valid())
		return glyph;

	for(const auto& fallback_face : lookup_fallback_faces(face)){
		if(!fallback_face)
			continue;

		glyph = find_face_glyph(*fallback_face, '?');
		if(glyph.valid())
			return glyph;
	}

	return {};
}

int text_line_height(const TtfFontFace& face)
{
	return std::max(face.get_line_skip(), face.get_height());
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

template<typename DecodeFn>
int measure_text_width_impl(TtfFontFace& face,int hspace,DecodeFn&& decode_next)
{
	int max_width = 0;
	int pen_x = 0;
	int line_min_x = INT_MAX;
	int line_max_x = 0;
	int line_end_x = 0;

	for(;;){
		uint32_t codepoint = 0;
		if(!decode_next(codepoint))
			break;

		if(codepoint == '\r')
			continue;

		if(codepoint == '\n'){
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

		RenderableGlyph glyph = get_renderable_glyph(face, codepoint);
		if(glyph.valid()){
			line_min_x = std::min(line_min_x, pen_x + glyph.glyph->minx);
			line_max_x = std::max(line_max_x, pen_x + glyph.glyph->maxx);
			pen_x += std::max(glyph.glyph->advance, 0) + hspace;
			line_end_x = pen_x;
		}
	}

	if(line_min_x != INT_MAX)
		max_width = std::max(max_width, std::max(line_max_x, line_end_x) - std::min(0, line_min_x));

	return max_width;
}

template<typename DecodeFn>
int measure_text_height_impl(TtfFontFace const& face,int vspace,DecodeFn&& decode_next)
{
	int lines = 1;

	for(;;){
		uint32_t codepoint = 0;
		if(!decode_next(codepoint))
			break;
		if(codepoint == '\n')
			lines++;
	}

	return lines * text_line_height(face) + lines * vspace;
}

template<typename DecodeFn>
void draw_text_8bit_impl(int x,int y,int color,TtfFontFace& face,int hspace,int vspace,bool clip,DecodeFn&& decode_next)
{
	const int palette_shift = (color >> 16) & 0xFF;
	const int palette_base = color & 0xFFFF;
	const int line_height = text_line_height(face);

	int pen_x = x;
	int pen_y = y;

	for(;;){
		uint32_t codepoint = 0;
		if(!decode_next(codepoint))
			break;

		if(codepoint == '\r')
			continue;

		if(codepoint == '\n'){
			pen_x = x;
			pen_y += line_height + vspace;
			continue;
		}

		RenderableGlyph glyph = get_renderable_glyph(face, codepoint);
		if(!glyph.valid())
			continue;

		const int draw_x = pen_x + glyph.glyph->minx;
		const int draw_y = pen_y + glyph.face->get_ascent() - glyph.glyph->maxy;
		blit_alpha_mask_8bit(draw_x, draw_y, *glyph.glyph, palette_base, palette_shift, clip);
		pen_x += std::max(glyph.glyph->advance, 0) + hspace;
	}
}

std::string make_default_ui_face_key(const std::string& font_path,int target_height,int hinting,bool kerning,int outline,int style)
{
	return font_path + "#" +
	       std::to_string(target_height) + "#" +
	       std::to_string(hinting) + "#" +
	       (kerning ? "1" : "0") + "#" +
	       std::to_string(outline) + "#" +
	       std::to_string(style);
}

}

const std::string& default_ui_ttf_font_path(void)
{
	static const std::string resolved = []() -> std::string {
		std::vector<std::string> candidates = default_ui_font_candidates();
		if(!candidates.empty())
			return candidates.front();
		return std::string();
	}();

	return resolved;
}

std::shared_ptr<TtfFontFace> default_ui_ttf_face(int target_height,int hinting,bool kerning,int outline,int style)
{
	const std::string& font_path = default_ui_ttf_font_path();
	if(font_path.empty())
		return nullptr;

	target_height = std::max(6, target_height);

	static std::unordered_map<std::string, std::weak_ptr<TtfFontFace>> face_cache;
	const std::string key = make_default_ui_face_key(font_path, target_height, hinting, kerning, outline, style);
	auto cache_it = face_cache.find(key);
	if(cache_it != face_cache.end()){
		auto cached_face = cache_it->second.lock();
		if(cached_face)
			return cached_face;
	}

	std::shared_ptr<TtfFontFace> best_face = best_face_for_path(font_path, target_height, hinting, kerning, outline, style);

	if(best_face){
		FaceVector fallbacks;
		for(const std::string& candidate : default_ui_font_candidates()){
			if(candidate == font_path)
				continue;

			auto fallback_face = best_face_for_path(candidate, target_height, hinting, kerning, outline, style);
			if(!fallback_face || fallback_face.get() == best_face.get())
				continue;

			bool duplicate = false;
			for(const auto& existing_face : fallbacks){
				if(existing_face && existing_face->get_file_name() == fallback_face->get_file_name() &&
				   existing_face->get_point_size() == fallback_face->get_point_size()){
					duplicate = true;
					break;
				}
			}
			if(!duplicate)
				fallbacks.push_back(fallback_face);
		}

		default_ui_fallback_faces()[best_face.get()] = std::move(fallbacks);
		face_cache[key] = best_face;
	}

	return best_face;
}

std::shared_ptr<TtfFontFace> default_ui_hfont_ttf_face(int legacy_height,int hinting,bool kerning,int outline,int style)
{
	const std::string& font_path = default_ui_ttf_font_path();
	if(font_path.empty())
		return nullptr;

	legacy_height = std::max(6, legacy_height);

	static std::unordered_map<std::string, std::weak_ptr<TtfFontFace>> face_cache;
	const std::string key = std::string("hfont#") + make_default_ui_face_key(font_path, legacy_height, hinting, kerning, outline, style);
	auto cache_it = face_cache.find(key);
	if(cache_it != face_cache.end()){
		auto cached_face = cache_it->second.lock();
		if(cached_face)
			return cached_face;
	}

	std::shared_ptr<TtfFontFace> best_face = best_hfont_face_for_path(font_path, legacy_height, hinting, kerning, outline, style);

	if(best_face){
		FaceVector fallbacks;
		for(const std::string& candidate : default_ui_font_candidates()){
			if(candidate == font_path)
				continue;

			auto fallback_face = best_hfont_face_for_path(candidate, legacy_height, hinting, kerning, outline, style);
			if(!fallback_face || fallback_face.get() == best_face.get())
				continue;

			bool duplicate = false;
			for(const auto& existing_face : fallbacks){
				if(existing_face && existing_face->get_file_name() == fallback_face->get_file_name() &&
				   existing_face->get_point_size() == fallback_face->get_point_size()){
					duplicate = true;
					break;
				}
			}
			if(!duplicate)
				fallbacks.push_back(fallback_face);
		}

		default_ui_fallback_faces()[best_face.get()] = std::move(fallbacks);
		face_cache[key] = best_face;
	}

	return best_face;
}

std::shared_ptr<TtfFontFace> default_ui_text32_ttf_face(int legacy_height,int hinting,bool kerning,int outline,int style)
{
	int target_height = std::max(6, legacy_height);
	if(language_prefers_japanese_fonts())
		target_height += 1;

	return default_ui_ttf_face(target_height, hinting, kerning, outline, style);
}

int default_ui_text32_extra_hspace(void)
{
	return language_prefers_japanese_fonts() ? 1 : 0;
}

int measure_legacy_ttf_text_width(std::string_view text,TtfFontFace& face,LegacyEncoding encoding,int hspace)
{
	size_t index = 0;
	return measure_text_width_impl(face, hspace, [&](uint32_t& codepoint) -> bool {
		if(index >= text.size())
			return false;
		codepoint = decode_legacy_char((unsigned char)text[index++], encoding);
		return true;
	});
}

int measure_legacy_ttf_text_height(std::string_view text,const TtfFontFace& face,int vspace)
{
	size_t index = 0;
	return measure_text_height_impl(face, vspace, [&](uint32_t& codepoint) -> bool {
		if(index >= text.size())
			return false;
		codepoint = decode_legacy_char((unsigned char)text[index++], LegacyEncoding::ASCII);
		return true;
	});
}

void draw_legacy_ttf_text_8bit(int x,int y,int color,std::string_view text,TtfFontFace& face,LegacyEncoding encoding,int hspace,int vspace,bool clip)
{
	size_t index = 0;
	draw_text_8bit_impl(x, y, color, face, hspace, vspace, clip, [&](uint32_t& codepoint) -> bool {
		if(index >= text.size())
			return false;
		codepoint = decode_legacy_char((unsigned char)text[index++], encoding);
		return true;
	});
}

int measure_utf8_text_width(std::string_view text,TtfFontFace& face,int hspace)
{
	size_t offset = 0;
	return measure_text_width_impl(face, hspace, [&](uint32_t& codepoint) -> bool {
		return utf8_next(text, offset, codepoint);
	});
}

int measure_utf8_text_height(std::string_view text,const TtfFontFace& face,int vspace)
{
	size_t offset = 0;
	return measure_text_height_impl(face, vspace, [&](uint32_t& codepoint) -> bool {
		return utf8_next(text, offset, codepoint);
	});
}

void draw_utf8_text_8bit(int x,int y,int color,std::string_view text,TtfFontFace& face,int hspace,int vspace,bool clip)
{
	size_t offset = 0;
	draw_text_8bit_impl(x, y, color, face, hspace, vspace, clip, [&](uint32_t& codepoint) -> bool {
		return utf8_next(text, offset, codepoint);
	});
}

}
