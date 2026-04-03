#include "font_manager.h"

#include "ttf_runtime.h"

#include <SDL.h>
#include <utility>

namespace text
{

TtfFontFace::TtfFontFace(std::string file_name_,int point_size_,int hinting_,bool kerning_,int outline_,int style_)
	: file_name(std::move(file_name_)),
	  point_size(point_size_),
	  hinting(hinting_),
	  kerning(kerning_),
	  outline(outline_),
	  style(style_)
{
}

TtfFontFace::~TtfFontFace(void)
{
	if(font)
		TTF_CloseFont(font);
}

bool TtfFontFace::load(void)
{
	last_error.clear();

	if(font)
		return true;

	if(!ttf_runtime_ready()){
		last_error = "SDL2_ttf runtime is not initialized";
		return false;
	}

	font = TTF_OpenFont(file_name.c_str(), point_size);
	if(!font){
		last_error = TTF_GetError();
		return false;
	}

	TTF_SetFontHinting(font, hinting);
	TTF_SetFontKerning(font, kerning ? 1 : 0);
	TTF_SetFontOutline(font, outline);
	TTF_SetFontStyle(font, style);
	return true;
}

int TtfFontFace::get_height(void) const
{
	return font ? TTF_FontHeight(font) : 0;
}

int TtfFontFace::get_ascent(void) const
{
	return font ? TTF_FontAscent(font) : 0;
}

int TtfFontFace::get_descent(void) const
{
	return font ? TTF_FontDescent(font) : 0;
}

int TtfFontFace::get_line_skip(void) const
{
	return font ? TTF_FontLineSkip(font) : 0;
}

bool TtfFontFace::has_glyph(uint32_t codepoint) const
{
	return font && TTF_GlyphIsProvided32(font, codepoint) != 0;
}

const GlyphBitmap* TtfFontFace::get_glyph(uint32_t codepoint)
{
	if(!font && !load())
		return nullptr;

	auto it = glyph_cache.find(codepoint);
	if(it != glyph_cache.end())
		return &it->second;

	GlyphBitmap glyph;
	if(!rasterize_glyph(codepoint, glyph))
		return nullptr;

	auto insert_it = glyph_cache.emplace(codepoint, std::move(glyph)).first;
	return &insert_it->second;
}

bool TtfFontFace::rasterize_glyph(uint32_t codepoint,GlyphBitmap& glyph)
{
	if(!font)
		return false;

	glyph.provided = TTF_GlyphIsProvided32(font, codepoint) != 0;
	if(!glyph.provided){
		glyph.width = 0;
		glyph.height = 0;
		glyph.pitch = 0;
		glyph.minx = 0;
		glyph.maxx = 0;
		glyph.miny = 0;
		glyph.maxy = 0;
		glyph.advance = 0;
		return true;
	}

	if(TTF_GlyphMetrics32(font, codepoint,
	                      &glyph.minx,&glyph.maxx,
	                      &glyph.miny,&glyph.maxy,
	                      &glyph.advance) != 0){
		last_error = TTF_GetError();
		return false;
	}

	SDL_Color white = {255,255,255,255};
	SDL_Surface* rendered = TTF_RenderGlyph32_Blended(font, codepoint, white);
	if(!rendered){
		if(glyph.provided){
			glyph.width = 0;
			glyph.height = 0;
			glyph.pitch = 0;
			return true;
		}

		last_error = TTF_GetError();
		return false;
	}

	SDL_Surface* surface = SDL_ConvertSurfaceFormat(rendered, SDL_PIXELFORMAT_RGBA32, 0);
	SDL_FreeSurface(rendered);
	if(!surface){
		last_error = SDL_GetError();
		return false;
	}

	glyph.width = surface->w;
	glyph.height = surface->h;
	glyph.pitch = surface->w;
	glyph.alpha.resize((size_t)glyph.width * (size_t)glyph.height);

	unsigned char* src = (unsigned char*)surface->pixels;
	for(int y = 0; y < surface->h; y++){
		unsigned int* row = (unsigned int*)(src + y * surface->pitch);
		for(int x = 0; x < surface->w; x++){
			unsigned char r,g,b,a;
			SDL_GetRGBA(row[x], surface->format, &r, &g, &b, &a);
			glyph.alpha[(size_t)y * (size_t)glyph.pitch + (size_t)x] = a;
		}
	}

	SDL_FreeSurface(surface);
	return true;
}

TtfFontManager& TtfFontManager::instance(void)
{
	static TtfFontManager manager;
	return manager;
}

std::string TtfFontManager::make_key(const std::string& file_name,int point_size,int hinting,bool kerning,int outline,int style) const
{
	return file_name + "#" +
	       std::to_string(point_size) + "#" +
	       std::to_string(hinting) + "#" +
	       (kerning ? "1" : "0") + "#" +
	       std::to_string(outline) + "#" +
	       std::to_string(style);
}

std::shared_ptr<TtfFontFace> TtfFontManager::get_face(const std::string& file_name,int point_size,int hinting,bool kerning,int outline,int style)
{
	const std::string key = make_key(file_name, point_size, hinting, kerning, outline, style);
	auto it = faces.find(key);
	if(it != faces.end())
		return it->second;

	auto face = std::make_shared<TtfFontFace>(file_name, point_size, hinting, kerning, outline, style);
	if(!face->load()){
		last_error = face->get_error();
		return nullptr;
	}

	faces.emplace(key, face);
	return face;
}

void TtfFontManager::clear(void)
{
	faces.clear();
	last_error.clear();
}

}
