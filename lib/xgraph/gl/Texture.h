//
// Created by nikita on 3/18/18.
//

#ifndef VANGERS_TEXTURE_H
#define VANGERS_TEXTURE_H

#include <memory>
#include <GL/glew.h>

namespace gl {
	enum class TextureFormat {
		Format8Bit,
		Format32bit,
		FormatPalette,
	};

	class Texture {
	public:
		int width;
		int height;
		int numLayers = 0; // > 0 - texture array
		GLuint textureId = 0;
		TextureFormat format;
		static std::shared_ptr<Texture> createTexture(int width, int height, int numLayers, TextureFormat format);
		static std::shared_ptr<Texture>
        createTexture(int width, int height, int numLayers, TextureFormat format, void *data);
		static std::shared_ptr<Texture> createTexture(int width, int height, TextureFormat format, int numLayers, void* data);
		static std::shared_ptr<Texture> createPalette(int width);
		void bindData(void* data);
		void update(int x, int y, int width, int height, void *data);
		void release();
		~Texture(){ release(); }
	private:
		Texture(int width, int height, int numLayers, TextureFormat format)
                : width(width), height(height), format(format), numLayers(numLayers) {}


	};
}

#endif //VANGERS_TEXTURE_H
