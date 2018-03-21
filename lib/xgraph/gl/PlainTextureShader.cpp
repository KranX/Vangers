//
// Created by nikita on 3/21/18.
//

#include "PlainTextureShader.h"
static const std::string fragment_code = "#version 150 core\n"
		"in vec2 Texcoord;\n"
		"\n"
		"out vec4 outColor;\n"
		"uniform sampler2D t_Texture;\n"
		"\n"
		"void main()\n"
		"{\n"
		"    outColor = texture(t_Texture, Texcoord);\n"
		"}";

static const std::string vertex_code = "#version 150 core\n"
		"in vec2 position;\n"
		"in vec2 texcoord;\n"
		"\n"
		"out vec2 Texcoord;\n"
		"\n"
		"void main()\n"
		"{\n"
		"    Texcoord = texcoord;\n"
		"    gl_Position = vec4(position.x, -position.y, 0.0f, 1.0f);\n"
		"}";

PlainTextureShader::PlainTextureShader(const std::shared_ptr<gl::Texture> &texture)
		: Shader(vertex_code, fragment_code) {
	GLfloat vertices[] = {
//		   Position   , Texcoords
			-1.0f,  -1.0f, 0.0f, 0.0f, // Top-left
			1.0f,  -1.0f, 1.0f, 0.0f, // Top-right
			1.0f, 1.0f, 1.0f, 1.0f, // Bottom-right
			-1.0f, 1.0f, 0.0f, 1.0f  // Bottom-left
	};


	GLuint elements[] = {
			0, 1, 2,
			3, 2, 0,
	};

	auto bufferData = gl::gen_buffer(vertices, elements, sizeof(vertices), sizeof(elements));
	data.bufferData = bufferData;
	data.texture = texture;
	data.textureAttrId = glGetUniformLocation(programId, "t_Texture");
	data.positionAttrId = glGetAttribLocation(programId, "position");
	data.texcoordAttrId = glGetAttribLocation(programId, "texcoord");
}

void PlainTextureShader::render_impl() {
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, data.texture->textureId);
	glUniform1i(data.textureAttrId, 0);

	glBindVertexArray(data.bufferData->vertexArrayObject);

	glEnableVertexAttribArray(data.positionAttrId);
	glBindBuffer(GL_ARRAY_BUFFER, data.bufferData->vertexBufferObject);
	glVertexAttribPointer(data.positionAttrId, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);

	glEnableVertexAttribArray(data.texcoordAttrId);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, data.bufferData->elementBufferObject);
	glVertexAttribPointer(data.texcoordAttrId, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*)(2 * sizeof(GLfloat)));

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
}

