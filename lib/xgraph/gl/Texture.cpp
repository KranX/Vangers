//
// Created by nikita on 3/18/18.
//

#include "Texture.h"
#include "common.h"

namespace gl {
	std::shared_ptr<Texture> Texture::createTexture(int width, int height, TextureFormat format) {
		Texture* tex = new Texture(width, height, format);
		auto texture = std::shared_ptr<Texture>(tex);
		return texture;
	}

	std::shared_ptr<Texture> Texture::createTexture(int width, int height, TextureFormat format, void *data) {
		auto texture = createTexture(width, height, format);
		texture->bindData(data);
		return texture;
	}

	std::shared_ptr<Texture> Texture::createPalette(int width) {
		auto texture = std::shared_ptr<Texture>(new Texture(width, 0, TextureFormat::FormatPalette));
		return texture;
	}

	void Texture::bindData(void *data) {
		switch (format) {
			case TextureFormat::FormatPalette:
				textureId = gen_palette(width, static_cast<uint32_t *>(data));
				break;
			case TextureFormat ::Format8Bit:
				textureId = gen_texture(GL_R8UI, GL_RED_INTEGER, GL_UNSIGNED_BYTE, GL_NEAREST, width, data, height);
				break;
			case TextureFormat ::Format32bit:
				textureId = gen_texture(GL_RGBA, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, GL_LINEAR, width, data, height);
				break;
		}

	}

	GLuint genTexture(GLint internalFormat, GLenum format, GLenum type, GLint filter, int width, void *data, int height) {
		GLuint textureID;
		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(
				GL_TEXTURE_2D,
				0,
				internalFormat, //GL_R8UI,
				width,
				height,
				0,
				format, //GL_RED_INTEGER,
				type,
				data
		);
		check_GL_error("glTexImage2D");
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
		check_GL_error("glTexParameteri");
		return textureID;
	}

	GLuint genPalette(int width, uint32_t *data) {
		GLuint textureID;
		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_1D, textureID);

		glTexImage1D(
				GL_TEXTURE_1D,
				0,
				GL_RGB,
				width,
				0,
				GL_RGBA,
				GL_UNSIGNED_INT_8_8_8_8,
				data
		);
		glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

		return textureID;
	}

	void Texture::update(int x, int y, int width, int height, void *data) {
		if(format == TextureFormat::Format32bit){
			glBindTexture(GL_TEXTURE_2D, textureId);
			glTexSubImage2D(
					GL_TEXTURE_2D,
					0,
					x,
					y,
					width,
					height,
					GL_RGBA,
					GL_UNSIGNED_INT_8_8_8_8,
					data
			);
		} else if(format == TextureFormat::FormatPalette) {
			glBindTexture(GL_TEXTURE_1D, textureId);
			glTexSubImage1D(
					GL_TEXTURE_1D,
					0,
					x,
					width,
					GL_RGBA,
					GL_UNSIGNED_INT_8_8_8_8,
					data
			);
		} else if(format == TextureFormat::Format8Bit) {
			glBindTexture(GL_TEXTURE_2D, textureId);
			glTexSubImage2D(
					GL_TEXTURE_2D,
					0,
					x,
					y,
					width,
					height,
					GL_RED_INTEGER,
					GL_UNSIGNED_BYTE,
					data
			);

		} else {
			fprintf(stderr, "Invalid format\n");
		}
		check_GL_error("glTextureSubImage2D");
	}
}
