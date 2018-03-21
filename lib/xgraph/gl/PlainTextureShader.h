//
// Created by nikita on 3/21/18.
//

#ifndef VANGERS_PLAINTEXTURERENDERE_H
#define VANGERS_PLAINTEXTURERENDERE_H

#include "Shader.h"
#include "common.h"
#include "Texture.h"

struct PlainTextureRendererData {
	std::shared_ptr<gl::Texture> texture;
	GLint textureAttrId;
	GLint positionAttrId;
	GLint texcoordAttrId;
	std::shared_ptr<gl::VertexBufferData> bufferData;

};

class PlainTextureShader: public gl::Shader{
private:
	PlainTextureRendererData data;
public:
	explicit PlainTextureShader(const std::shared_ptr<gl::Texture> &texture);

	void render_impl() override ;

};


#endif //VANGERS_PLAINTEXTURERENDERE_H
