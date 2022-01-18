#ifndef LIB_RENDERER_SRC_RENDERER_CORE_SDL_EXT_SDL_EXTENSIONS
#define LIB_RENDERER_SRC_RENDERER_CORE_SDL_EXT_SDL_EXTENSIONS

#include <SDL.h>

#include "../../common.h"
#include "../AbstractCompositor.h"

namespace renderer::compositor::sdl_ext {
	Texture texture_load_bmp(AbstractCompositor& compositor, const char* filepath, TextureType texture_type, BlendMode blend_mode);
	Texture texture_from_sdl_surface(AbstractCompositor& compositor, SDL_Surface* surface, TextureType texture_type, BlendMode blend_mode);
}
#endif /* LIB_RENDERER_SRC_RENDERER_CORE_SDL_EXT_SDL_EXTENSIONS */
