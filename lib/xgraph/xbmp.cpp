//
// Created by caiiiycuk on 25.06.2021.
//

#include <fstream>

#include "xbmp.h"

SDL_Texture *BMP_CreateTexture(const char *file, SDL_Renderer *renderer) {
	SDL_Surface *surface = SDL_LoadBMP(file);
	if (!surface)
		return nullptr;
	SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
	if (texture && (!SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND) ||
					   !SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_LINEAR))) {
		SDL_DestroyTexture(texture);
		texture = nullptr;
	}
	SDL_DestroySurface(surface);
	return texture;
}
