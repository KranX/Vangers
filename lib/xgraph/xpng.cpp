//
// Created by caiiiycuk on 25.06.2021.
//

#define STBI_NO_BMP
#define STBI_NO_TGA
#define STBI_NO_PSD
#define STBI_NO_HDR
#define STBI_NO_PIC
#define STBI_NO_GIF
#define STBI_NO_PNM
#define STB_IMAGE_IMPLEMENTATION

#include "stb_image.h"
#include <fstream>

#include "xpng.h"

namespace {
char *readBytes(const char *file, int &length);
}

SDL_Texture *PNG_CreateTexture(const char *file, SDL_Renderer *renderer) {
	int length;
	char *bytes = readBytes(file, length);

	if (length == 0 || bytes == nullptr) {
		return nullptr;
	}

	int width, height, bpp;
	unsigned char *data =
		stbi_load_from_memory((stbi_uc const *)bytes, length, &width, &height, &bpp, 0);

	if (bpp == 1) {
		// grey
		stbi_image_free(data);

		data = stbi_load_from_memory((stbi_uc const *)bytes, length, &width, &height, &bpp, 3);
		bpp = 3;
	} else if (bpp == 2) {
		// grey + alpha
		stbi_image_free(data);

		data = stbi_load_from_memory((stbi_uc const *)bytes, length, &width, &height, &bpp, 4);
		bpp = 4;
	}

	SDL_Texture *texture = SDL_CreateTexture(renderer, bpp == 3 ? SDL_PIXELFORMAT_RGB24 : SDL_PIXELFORMAT_ABGR8888,
											 SDL_TEXTUREACCESS_STATIC, width, height);
	SDL_UpdateTexture(texture, nullptr, (char*) data, width * bpp);

	if (bpp == 4) {
		SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
	}

	stbi_image_free(data);
	delete[] bytes;

	return texture;
}

namespace {
char *readBytes(const char *filename, int &length) {
	std::ifstream file(filename, std::ifstream::ate | std::ifstream::binary | std::ifstream::in);

	file.is_open();
	length = file.tellg();

	if (length <= 0) {
		return nullptr;
	}

	char *buffer = new char[length];

	file.seekg(0, std::ios::beg);
	file.read(buffer, length);
	file.close();

	return buffer;
}
}
