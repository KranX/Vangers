//
// Created by nikita on 2018-12-09.
//

#ifndef VANGERS_TEXTURE_EXT_H
#define VANGERS_TEXTURE_EXT_H

#include "texture.h"
#include "vec.h"

namespace vgl {
	typedef Texture<vgl::iVec1, GL_TEXTURE_1D> Texture1D;
	typedef Texture<vgl::iVec2, GL_TEXTURE_2D> Texture2D;
	typedef Texture<vgl::iVec3, GL_TEXTURE_2D_ARRAY> Texture2DArray;


	template <>
	void Texture<vgl::iVec1, GL_TEXTURE_1D>::TexStorage(GLenum internalFormat, const vgl::iVec1 &dimensions) ;

	template <>
	void Texture<vgl::iVec1, GL_TEXTURE_1D>::TexSubImage(const vgl::iVec1 &offset, const vgl::iVec1 &dimensions, GLenum format, GLenum type, GLvoid *data);

	template <>
	void Texture<vgl::iVec2, GL_TEXTURE_2D>::TexStorage(GLenum internalFormat, const vgl::iVec2 &dimensions);

	template <>
	void Texture<vgl::iVec2, GL_TEXTURE_2D>::TexSubImage(const vgl::iVec2 &offset, const vgl::iVec2 &dimensions, GLenum format, GLenum type, GLvoid *data);

	template <>
	void Texture<vgl::iVec3, GL_TEXTURE_2D_ARRAY>::TexStorage(GLenum internalFormat, const vgl::iVec3 &dimensions);

	template <>
	void Texture<vgl::iVec3, GL_TEXTURE_2D_ARRAY>::TexSubImage(const vgl::iVec3 &offset, const vgl::iVec3 &dimensions,
			GLenum format, GLenum type, GLvoid *data);

}
#endif //VANGERS_TEXTURE_EXT_H
