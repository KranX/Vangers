#include <cassert>

#include "SDL_extensions.h"

using namespace renderer;
using namespace renderer::compositor;

namespace renderer::compositor::sdl_ext {
	Texture texture_from_sdl_surface(AbstractCompositor& renderer, SDL_Surface* surface, TextureType texture_type, BlendMode blend_mode){
		assert(texture_type == TextureType::RGBA32);
		SDL_Surface* converted = SDL_ConvertSurfaceFormat(surface, SDL_PIXELFORMAT_RGBA32, 0);
		Texture texture = renderer.texture_create(converted->w, converted->h, texture_type, blend_mode);
		renderer.texture_set_data(texture, (uint8_t*)converted->pixels);
		SDL_FreeSurface(converted);
		return texture;
	}

	Texture texture_load_bmp(AbstractCompositor& renderer, const char* filepath, TextureType texture_type, BlendMode blend_mode){
		SDL_Surface* bmpSurface = SDL_LoadBMP(filepath);
		Texture texture = texture_from_sdl_surface(renderer, bmpSurface, texture_type, blend_mode);
		SDL_FreeSurface(bmpSurface);
		return texture;
	}
}
