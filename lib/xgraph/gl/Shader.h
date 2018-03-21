//
// Created by nikita on 3/21/18.
//

#ifndef VANGERS_SHADER_H
#define VANGERS_SHADER_H

#include <string>
#include <GL/glew.h>

namespace gl {
	class Shader{
	protected:
		GLuint programId;
		void bind();
		virtual void render_impl() = 0;
	public:
		explicit Shader(const std::string& shader_path);
		Shader(const std::string& vertex_shader_code, const std::string& fragment_shader_code);
		void render();

	};
}
#endif //VANGERS_SHADER_H
