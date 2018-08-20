//
// Created by nikita on 3/18/18.
//

#ifndef VANGERS_COMMON_H
#define VANGERS_COMMON_H

#include <GL/glew.h>
#include <string>

namespace gl {
	struct VertexBufferData {
		GLuint vertexArrayObject;
		GLuint vertexBufferObject;
		GLuint elementBufferObject;
	};

	std::shared_ptr<VertexBufferData> gen_buffer(const GLfloat *vertices,
	                                             const GLuint *elements,
	                                             GLsizeiptr verticesSize,
	                                             GLsizeiptr elementsSize);
	void check_GL_error(const char* context);
	GLuint load_shaders(const std::string &vertex_file_path, const std::string &fragment_file_path);
	GLuint load_shaders_str(const std::string &vertex_shader_code, const std::string &fragment_shader_code);
	GLuint gen_palette(int width, uint32_t *data);
	GLuint gen_texture(GLint internalFormat, GLenum format, GLenum type, GLint filter, int width, int height, int numLayers, void *data);


}

#endif //VANGERS_COMMON_H
