//
// Created by nikita on 2018-12-09.
//

#include "texture_ext.h"


template <>
void vgl::Texture<vgl::iVec1, GL_TEXTURE_1D>::TexStorage(GLenum internalFormat, const vgl::iVec1 &dimensions) {
	glTexStorage1D(GL_TEXTURE_1D, 1, internalFormat, dimensions.x);
	checkErrorAndThrow("GL_TEXTURE_1D");
}

template <>
void vgl::Texture<vgl::iVec1, GL_TEXTURE_1D>::TexSubImage(const vgl::iVec1 &offset, const vgl::iVec1 &dimensions, GLenum format, GLenum type, GLvoid *data) {
	glTexSubImage1D(
			GL_TEXTURE_1D, 0,
			offset.x,
			dimensions.x,
			format, type, data);
	checkErrorAndThrow("glTexSubImage1D");
}


template <>
void vgl::Texture<vgl::iVec2, GL_TEXTURE_2D>::TexStorage(GLenum internalFormat, const vgl::iVec2 &dimensions) {
	glTexStorage2D(GL_TEXTURE_2D, 1, internalFormat, dimensions.x, dimensions.y);
	checkErrorAndThrow("glTexStorage2D");
}

template <>
void vgl::Texture<vgl::iVec2, GL_TEXTURE_2D>::TexSubImage(const vgl::iVec2 &offset, const vgl::iVec2 &dimensions, GLenum format, GLenum type, GLvoid *data) {
	glTexSubImage2D(
			GL_TEXTURE_2D, 0,
			offset.x, offset.y,
			dimensions.x, dimensions.y,
			format, type, data);
	checkErrorAndThrow("glTexSubImage2D");
}


template <>
void vgl::Texture<vgl::iVec3, GL_TEXTURE_2D_ARRAY>::TexStorage(GLenum internalFormat, const vgl::iVec3 &dimensions) {
	glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, internalFormat, dimensions.x, dimensions.y,
	               dimensions.z);
	checkErrorAndThrow("glTexStorage3D");
}

template <>
void vgl::Texture<vgl::iVec3, GL_TEXTURE_2D_ARRAY>::TexSubImage(const vgl::iVec3 &offset, const vgl::iVec3 &dimensions, GLenum format, GLenum type, GLvoid *data) {
	glTexSubImage3D(
			GL_TEXTURE_2D_ARRAY, 0,
			offset.x, offset.y, offset.z,
			dimensions.x, dimensions.y, dimensions.z,
			format, type, data);
	checkErrorAndThrow("glTexSubImage3D");
}