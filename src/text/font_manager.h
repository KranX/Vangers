#ifndef __TEXT_FONT_MANAGER_H__
#define __TEXT_FONT_MANAGER_H__

#include <cstdint>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include <SDL_ttf.h>

namespace text
{

struct GlyphBitmap
{
	int width = 0;
	int height = 0;
	int pitch = 0;

	int minx = 0;
	int maxx = 0;
	int miny = 0;
	int maxy = 0;
	int advance = 0;

	bool provided = false;

	std::vector<unsigned char> alpha;

	bool empty(void) const { return alpha.empty(); }
};

class TtfFontFace
{
public:
	TtfFontFace(std::string file_name,int point_size,int hinting = TTF_HINTING_NORMAL,bool kerning = true,int outline = 0);
	~TtfFontFace(void);

	TtfFontFace(const TtfFontFace&) = delete;
	TtfFontFace& operator=(const TtfFontFace&) = delete;

	bool load(void);
	bool ready(void) const { return font != nullptr; }
	const std::string& get_error(void) const { return last_error; }

	const std::string& get_file_name(void) const { return file_name; }
	int get_point_size(void) const { return point_size; }
	int get_hinting(void) const { return hinting; }
	bool get_kerning(void) const { return kerning; }
	int get_outline(void) const { return outline; }

	int get_height(void) const;
	int get_ascent(void) const;
	int get_descent(void) const;
	int get_line_skip(void) const;

	bool has_glyph(uint32_t codepoint) const;
	const GlyphBitmap* get_glyph(uint32_t codepoint);

private:
	bool rasterize_glyph(uint32_t codepoint,GlyphBitmap& glyph);

	std::string file_name;
	int point_size;
	int hinting;
	bool kerning;
	int outline;

	std::string last_error;
	TTF_Font* font = nullptr;
	std::unordered_map<uint32_t, GlyphBitmap> glyph_cache;
};

class TtfFontManager
{
public:
	static TtfFontManager& instance(void);

	std::shared_ptr<TtfFontFace> get_face(const std::string& file_name,int point_size,int hinting = TTF_HINTING_NORMAL,bool kerning = true,int outline = 0);
	void clear(void);

	const std::string& get_error(void) const { return last_error; }

private:
	TtfFontManager(void) = default;

	std::string make_key(const std::string& file_name,int point_size,int hinting,bool kerning,int outline) const;

	std::string last_error;
	std::unordered_map<std::string, std::shared_ptr<TtfFontFace>> faces;
};

}

#endif
