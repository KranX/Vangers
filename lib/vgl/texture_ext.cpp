//
// Created by nikita on 2018-12-09.
//

#include "texture_ext.h"
template <>
void vgl::Texture<glm::ivec2, GL_TEXTURE_2D>::TexStorage(GLenum internalFormat, const glm::ivec2 &dimensions) {
	glTexStorage2D(GL_TEXTURE_2D, 1, internalFormat, dimensions.x, dimensions.y);
	checkErrorAndThrow("glTexStorage2D");
}

template <>
void vgl::Texture<glm::ivec2, GL_TEXTURE_2D>::TexSubImage(const glm::ivec2 &offset, const glm::ivec2 &dimensions, GLenum format, GLenum type, GLvoid *data) {
	glTexSubImage2D(
			GL_TEXTURE_2D, 0,
			offset.x, offset.y,
			dimensions.x, dimensions.y,
			format, type, data);
	checkErrorAndThrow("glTexSubImage2D");
}

template <>
void vgl::Texture<glm::ivec3, GL_TEXTURE_2D_ARRAY>::TexStorage(GLenum internalFormat, const glm::ivec3 &dimensions) {
	glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, internalFormat, dimensions.x, dimensions.y,
	               dimensions.z);
	checkErrorAndThrow("glTexStorage3D");
}

template <>
void vgl::Texture<glm::ivec3, GL_TEXTURE_2D_ARRAY>::TexSubImage(const glm::ivec3 &offset, const glm::ivec3 &dimensions, GLenum format, GLenum type, GLvoid *data) {
	glTexSubImage3D(
			GL_TEXTURE_2D_ARRAY, 0,
			offset.x, offset.y, offset.z,
			dimensions.x, dimensions.y, dimensions.z,
			format, type, data);
	checkErrorAndThrow("glTexSubImage3D");
}

template <>
void vgl::Texture<uint, GL_TEXTURE_1D>::TexStorage(GLenum internalFormat, const uint &dimensions) {
	glTexStorage1D(GL_TEXTURE_1D, 1, internalFormat, dimensions);
	checkErrorAndThrow("GL_TEXTURE_1D");
}

template <>
void vgl::Texture<uint, GL_TEXTURE_1D>::TexSubImage(const uint &offset, const uint &dimensions, GLenum format, GLenum type, GLvoid *data) {
	glTexSubImage1D(
			GL_TEXTURE_1D, 0,
			offset,
			dimensions,
			format, type, data);
	checkErrorAndThrow("glTexSubImage1D");
}