#ifndef GLES3TEXTURE_H
#define GLES3TEXTURE_H

#include <cstdint>
#include "../AbstractCompositor.h"

namespace renderer::compositor::gles3 {
	class GLES3Texture
	{
	public:
		GLES3Texture(int32_t width, int32_t height, TextureType texture_type, BlendMode blend_mode, uint8_t* pixels);

		inline uint32_t name() const {
			return _name;
		}

		inline TextureType texture_type() const {
			return _texture_type;
		}

		inline BlendMode blend_mode() const {
			return _blend_mode;
		}

		inline int32_t width() const {
			return _width;
		}

		inline int32_t height() const {
			return _height;
		}

		void destroy();

		Color color() const;

		void set_color(const Color& color);

	private:
		uint32_t _name;
		int32_t _width;
		int32_t _height;
		TextureType _texture_type;
		BlendMode _blend_mode;
		Color _color;
	};

}

#endif // GLES3TEXTURE_H
