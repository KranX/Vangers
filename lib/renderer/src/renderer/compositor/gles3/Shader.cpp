#include "Shader.h"

#include <glad/glad.h>
#include <iostream>
#include "../CompositorException.h"

using namespace renderer::compositor::gles3;

Shader::Shader()
	: _program(0)
{

}

void Shader::initialize(const char* vs_shader, const char* fs_shader)
{
	GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	{
		glShaderSource(vertex_shader, 1, &vs_shader, nullptr);
		glCompileShader(vertex_shader);
		GLint success;
		glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
		if(!success){
			char infoLog[1024];
			glGetShaderInfoLog(vertex_shader, 1024, nullptr, infoLog);
			throw CompositorException(std::string("Vertex shader compilation error: " + std::string(infoLog)));
		}
	}

	GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	{
		glShaderSource(fragment_shader, 1, &fs_shader, nullptr);
		glCompileShader(fragment_shader);
		GLint success;
		glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
		if(!success){
			char infoLog[1024];
			glGetShaderInfoLog(fragment_shader, 1024, nullptr, infoLog);
			throw CompositorException(std::string("Fragment shader compilation error: " + std::string(infoLog)));
		}
	}

	_program = glCreateProgram();
	{
		glAttachShader(_program, vertex_shader);
		glAttachShader(_program, fragment_shader);
		glLinkProgram(_program);
		GLint success;
		glGetProgramiv(_program, GL_LINK_STATUS, &success);
		if(!success){
			char infoLog[1024];
			throw CompositorException(std::string("Program link error: " + std::string(infoLog)));
		}
	}
	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);
}

void Shader::use()
{
	if(_program == 0){
		throw CompositorException("shader is not initialized");
	}
	glUseProgram(_program);
}

void Shader::unuse()
{
	glUseProgram(0);
}

void Shader::set_uniform(const char* name, int value)
{
	if(_program == 0){
		throw CompositorException("shader is not initialized");
	}

	glUniform1i(glGetUniformLocation(_program, name), value);
}

void Shader::set_uniform(const char* name, float value[4])
{
	if(_program == 0){
		throw CompositorException("shader is not initialized");
	}

	glUniform4f(glGetUniformLocation(_program, name), value[0], value[1], value[2], value[3]);
}

void Shader::set_uniform_mat(const char* name, float value[])
{
	if(_program == 0){
		throw CompositorException("shader is not initialized");
	}

	glUniformMatrix4fv(glGetUniformLocation(_program, name), 1, false, value);
}
