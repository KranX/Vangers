//
// Created by nikita on 3/21/18.
//

#include "Shader.h"
#include "common.h"

void gl::Shader::bind() {
	glUseProgram(programId);
}

gl::Shader::Shader(const std::string &shader_path) {
	programId = load_shaders(
			shader_path+".vert",
			shader_path+".frag"
	);
}

gl::Shader::Shader(const std::string &vertex_shader_code, const std::string &fragment_shader_code) {
	programId = load_shaders_str(vertex_shader_code, fragment_shader_code);
}

void gl::Shader::render() {
	bind();
	render_impl();
}
