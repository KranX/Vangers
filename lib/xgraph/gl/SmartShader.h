//
// Created by nikita on 3/18/18.
//

#ifndef VANGERS_SHADER_H
#define VANGERS_SHADER_H

#include <string>
#include <map>
#include <vector>
#include <memory>
#include <GL/glew.h>
#include <glm/glm.hpp>

#include "Texture.h"

namespace gl{
//	struct VertexBufferData {
//		GLuint vertexArrayObject;
//		GLuint vertexBufferObject;
//		GLuint elementBufferObject;
//	};
//
//	std::shared_ptr<VertexBufferData > gen_buffer(const GLfloat *vertices, const GLuint *elements,
//	                                              GLsizeiptr verticesSize, GLsizeiptr elementsSize);

	struct TextureLocation {
		std::string name;
		GLint location;
	};

	struct UniformLocation{
	public:
		std::string name;
		GLint location;
	};

	template <typename T>
	void set_uniform_impl(GLint location, const T& data){
		assert(false);
	}

	template <>
	void set_uniform_impl<glm::mat4>(GLint location, const glm::mat4& data){
		glUniformMatrix4fv(location, 1, GL_FALSE, &data[0][0]);
	}

	template <>
	void set_uniform_impl<glm::vec4>(GLint location, const glm::vec4& data){
		glUniform4fv(location, 1, &data[0]);
	}

	class SmartShader {
	private:
		GLuint programId;
		std::map<std::string, TextureLocation*> textures;
		std::map<std::string, UniformLocation*> uniforms;

	public:
		void init(const std::string& shader_path);
		void bind();
		void add_texture(const std::string &name);

		template <class T> void add_uniform(
				const std::string& name
		){
			auto location = glGetUniformLocation(programId, name.c_str());
			auto * uniform = new UniformLocation{
					.name = name,
					.location = location,
			};
			uniforms[name] = uniform;
		}

		template<typename T>
		void set_uniform(const std::string& name, const T &data){
			auto* uniform = uniforms[name];
			set_uniform_impl<T>(uniform->location, data);
		}

		void set_texture(const std::string &name,
		                 const gl::Texture &texture,
		                 GLuint texture_index,
		                 GLuint texture_unit);

		virtual ~SmartShader();
	};

}


#endif //VANGERS_SHADER_H
