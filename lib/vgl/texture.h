//
// Created by nikita on 2018-12-03.
//

#ifndef UPLOADBENCH_TEXTURE_H
#define UPLOADBENCH_TEXTURE_H

#include "util.h"
#include "buffer.h"
#include "itexture.h"

#include <glm/glm.hpp>

namespace vgl {
	enum class TextureType : GLenum {
		Texture2D = GL_TEXTURE_2D,
		Texture2DArray = GL_TEXTURE_2D_ARRAY
	};

	enum class TextureInternalFormat : GLenum {
		R8ui = GL_R8UI,
		RGBA8 = GL_RGBA8,
	};

	enum class TextureFormat : GLenum {
		RedInteger = GL_RED_INTEGER,
		BGRA = GL_BGRA,
		BGR = GL_BGR,
		RGB = GL_RGB,
		RGBA = GL_RGBA
	};

	enum class TextureDataType : GLenum {
		UnsignedInt8888=  GL_UNSIGNED_INT_8_8_8_8,
		UnsignedByte = GL_UNSIGNED_BYTE
	};

	enum class TextureFilter : GLint {
		Nearest = GL_NEAREST,
		Linear = GL_LINEAR
	};

	template<class T, GLenum textureType>
	class Texture : public ITexture, NamedObject<GLuint>{
	private:
		TextureInternalFormat internalFormat;

		T dimensions;
	private:
//		static const GLuint glTarget = static_cast<GLuint>(textureType);
		static void TexStorage(GLenum internalFormat, const T &dimensions) {
			throw "Unimplemented";
		}

		static void TexSubImage(const T &offset, const T &dimensions, GLenum format, GLenum type, GLvoid *data) {
			throw "Unimplemented";
		}
	public:
		Texture(GLuint textureId, TextureInternalFormat internalFormat, const T &dimensions)
				: NamedObject(textureId), internalFormat(internalFormat), dimensions(dimensions) {}

		Texture(Texture&& other) noexcept : NamedObject(other.objectId){
			internalFormat = other.internalFormat;
			std::swap(dimensions, other.dimensions);
		}

		T getDimensions() const { return dimensions; }

		static std::shared_ptr<Texture<T, textureType>>
		create(const T &dimensions, TextureInternalFormat internalFormat, TextureFilter filter = TextureFilter::Nearest) {
			auto glInternalFormat = static_cast<GLenum>(internalFormat);

			GLuint textureId;

			glGenTextures(1, &textureId);
			checkErrorAndThrow("glGenTextures");

			glBindTexture(textureType, textureId);
			checkErrorAndThrow("glGenTextures");

			auto glFilter = static_cast<GLint>(filter);
			glTexParameteri(textureType, GL_TEXTURE_MAG_FILTER, glFilter);
			glTexParameteri(textureType, GL_TEXTURE_MIN_FILTER, glFilter);
			checkErrorAndThrow("glTexParameteri");

			TexStorage(glInternalFormat, dimensions);
			return std::make_shared<Texture<T, textureType>>(textureId, internalFormat, dimensions);
		}

		static std::shared_ptr<Texture<T, textureType>>
		create(const T &dimensions,
				TextureInternalFormat internalFormat,
				TextureFilter filter,
				TextureFormat format,
				TextureDataType dataType,
				GLvoid* data) {
			GLuint objectId;

			glGenTextures(1, &objectId);
			checkErrorAndThrow("glGenTextures");

			glBindTexture(textureType, objectId);
			checkErrorAndThrow("glGenTextures");

			auto glFilter = static_cast<GLint>(filter);
			glTexParameteri(textureType, GL_TEXTURE_MAG_FILTER, glFilter);
			glTexParameteri(textureType, GL_TEXTURE_MIN_FILTER, glFilter);
			checkErrorAndThrow("glTexParameteri");

			auto texture = std::make_shared<Texture<T, textureType>>(objectId, internalFormat, dimensions);
			texture->TexStorage(static_cast<GLenum>(internalFormat), dimensions);
			texture->TexSubImage(T::zero, dimensions, static_cast<GLenum>(format), static_cast<GLenum>(dataType), data);
			return texture;
		}

		void subImage(const T &offset, const T &dimensions, TextureFormat format, TextureDataType dataType, PixelUnpackBuffer &buffer) {
			bind();
			buffer.bind();

			auto glFormat = static_cast<GLenum>(format);
			TexSubImage(offset, dimensions, glFormat, static_cast<GLenum>(dataType), 0);
		}

		void subImage(TextureFormat format, TextureDataType dataType, PixelUnpackBuffer &buffer) {
			bind();
			buffer.bind();

			auto glFormat = static_cast<GLenum>(format);
			TexSubImage(T::zero, dimensions, glFormat, static_cast<GLenum>(dataType), 0);
		}

		void subImage(const T &offset, const T &dimensions, TextureFormat format, TextureDataType dataType, GLvoid *data) {
			bind();

			auto glFormat = static_cast<GLenum>(format);
			TexSubImage(offset, dimensions, glFormat, static_cast<GLenum>(dataType), data);
		}

		void subImage(TextureFormat format, TextureDataType dataType, GLvoid *data) {
			bind();

			auto glFormat = static_cast<GLenum>(format);
			TexSubImage(T::zero, dimensions, glFormat, static_cast<GLenum>(dataType), data);
		}

		void bind() override {
			glBindTexture(textureType, objectId);
			checkErrorAndThrow("glBindTexture");
		}

		static void bindToZero() {
			glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
		}

		void free() override {
			glDeleteTextures(1, &objectId);
			checkErrorAndThrow("glDeleteTextures");
		}
	};
}

#endif //UPLOADBENCH_TEXTURE_H
