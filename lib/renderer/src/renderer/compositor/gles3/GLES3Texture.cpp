#include "GLES3Texture.h"

#include "../CompositorException.h"
#include "../AbstractCompositor.h"

#include <glad/glad.h>

using namespace renderer;
using namespace renderer::compositor;
using namespace renderer::compositor::gles3;

GLES3Texture::GLES3Texture(int32_t width, int32_t height, TextureType texture_type, BlendMode blend_mode, uint8_t* pixels)
	: _width(width)
	, _height(height)
	, _texture_type(texture_type)
	, _blend_mode(blend_mode)
	, _color({255, 255, 255, 255})
{
	GLenum gl_error;
	glGenTextures(1, &_name);

	glBindTexture(GL_TEXTURE_2D, _name);
#ifndef EMSCRIPTEN
	gl_error = glGetError();
	if(gl_error != 0){
        printf("%s\n", (std::string("glBindTexture error: ") + std::to_string(gl_error)).c_str());
        abort();
	}
#endif
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	GLint internalformat;
	GLenum format;
	GLenum type;

	switch (_texture_type) {
	case TextureType::RGBA32:
		internalformat = GL_RGBA;
		format = GL_RGBA;
		type = GL_UNSIGNED_BYTE;
		break;
	default: {
        printf("%s\n", (std::string("Unknown TextureType: ") + std::to_string((int) _texture_type)).c_str());
        abort();
    }
	}

	glTexImage2D(GL_TEXTURE_2D, 0, internalformat, width, height, 0,
				 format, type, pixels);

#ifndef EMSCRIPTEN
	gl_error = glGetError();
	if(gl_error != 0){
        printf("%s\n", ((std::string("glTexImage2D error: ") + std::to_string(gl_error)).c_str()));
        abort();
	}
#endif

	glGenerateMipmap(GL_TEXTURE_2D);
}

void GLES3Texture::destroy()
{
	glDeleteTextures(1, &_name);
	_name = 0;
}

Color GLES3Texture::color() const
{
	return _color;
}

void GLES3Texture::set_color(const Color& color)
{
	_color = color;
}

