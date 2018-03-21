//
// Created by nikita on 3/18/18.
//

#include "SmartShader.h"
#include "common.h"


void gl::SmartShader::init(const std::string &shader_path) {
	programId = load_shaders(shader_path+".vert", shader_path+".frag");
}

void gl::SmartShader::add_texture(const std::string &name) {
	auto * texture_location = new TextureLocation{
			.name = name,
			.location = glGetUniformLocation(programId, name.c_str()),
	};

	textures[name] = texture_location;
}

gl::SmartShader::~SmartShader() {
	for(auto kv: uniforms){
		delete kv.second;
	}

	for(auto kv: textures){
		delete kv.second;
	}
}

GLenum target_from_texture(const gl::Texture& texture){
	if(texture.format == gl::TextureFormat::FormatPalette){
		return GL_TEXTURE_1D;
	}
	return GL_TEXTURE_2D;
}

void gl::SmartShader::set_texture(const std::string &name, const gl::Texture &texture,
                             GLuint texture_index, GLuint texture_unit) {
	auto* texture_location = textures[name];
	glActiveTexture(texture_index);
	auto target = target_from_texture(texture);
	glBindTexture(target, texture.textureId);
	glUniform1i(texture_location->location, texture_unit);
}

void gl::SmartShader::bind() {
	glUseProgram(programId);
}



