//
// Created by nikita on 2018-12-09.
//

#ifndef VANGERS_TEXTURE_EXT_H
#define VANGERS_TEXTURE_EXT_H

#include "texture.h"

namespace vgl {
	typedef Texture<glm::ivec2, GL_TEXTURE_2D> Texture2D;
	typedef Texture<glm::ivec3, GL_TEXTURE_2D_ARRAY> Texture2DArray;
	typedef Texture<uint, GL_TEXTURE_1D> Texture1D;

	template <>
	void Texture<glm::ivec2, GL_TEXTURE_2D>::TexStorage(GLenum internalFormat, const glm::ivec2 &dimensions);

	template <>
	void Texture<glm::ivec2, GL_TEXTURE_2D>::TexSubImage(const glm::ivec2 &offset, const glm::ivec2 &dimensions, GLenum format, GLenum type, GLvoid *data);

	template <>
	void Texture<glm::ivec3, GL_TEXTURE_2D_ARRAY>::TexStorage(GLenum internalFormat, const glm::ivec3 &dimensions);

	template <>
	void Texture<glm::ivec3, GL_TEXTURE_2D_ARRAY>::TexSubImage(const glm::ivec3 &offset, const glm::ivec3 &dimensions,
			GLenum format, GLenum type, GLvoid *data);

	template <>
	void Texture<uint, GL_TEXTURE_1D>::TexStorage(GLenum internalFormat, const uint &dimensions) ;

	template <>
	void Texture<uint, GL_TEXTURE_1D>::TexSubImage(const uint &offset, const uint &dimensions, GLenum format, GLenum type, GLvoid *data);
}
#endif //VANGERS_TEXTURE_EXT_H
