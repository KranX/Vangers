//
// Created by nikita on 2018-12-02.
//
#include <vector>
#include <fstream>
#include <iostream>
#include <sstream>

#include "util.h"

GLuint vgl::loadShaders(const std::string &vertexFilePath, const std::string &fragmentFilePath){
	std::cout<<"load_shaders: "<<vertexFilePath<<" "<<fragmentFilePath<<std::endl;
	std::string VertexShaderCode;
	std::ifstream VertexShaderStream(vertexFilePath, std::ios::in);
	if(VertexShaderStream.is_open()){
		std::stringstream sstr;
		sstr << VertexShaderStream.rdbuf();
		VertexShaderCode = sstr.str();
		VertexShaderStream.close();
	}else{
		// TODO: handle this right
		printf("Impossible to open %s.\n", vertexFilePath.c_str());
		exit(1);
	}

	std::string FragmentShaderCode;
	std::ifstream FragmentShaderStream(fragmentFilePath, std::ios::in);
	if(FragmentShaderStream.is_open()){
		std::stringstream sstr;
		sstr << FragmentShaderStream.rdbuf();
		FragmentShaderCode = sstr.str();
		FragmentShaderStream.close();
	}
	return vgl::loadShadersStr(VertexShaderCode, FragmentShaderCode);
}

GLuint vgl::loadShadersStr(const std::string &vertex_shader_code, const std::string &fragment_shader_code)
{
	// Create the shaders
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	GLint Result = GL_FALSE;
	int InfoLogLength;


	// Compile Vertex SmartShader
	printf("Compiling shader\n");
	char const * VertexSourcePointer = vertex_shader_code.c_str();
	glShaderSource(VertexShaderID, 1, &VertexSourcePointer , NULL);
	glCompileShader(VertexShaderID);

	// Check Vertex SmartShader
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if ( InfoLogLength > 0 ){
		std::vector<char> VertexShaderErrorMessage(InfoLogLength+1);
		glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
		printf("%s\n", &VertexShaderErrorMessage[0]);
	}

	// Compile Fragment SmartShader
	printf("Compiling shader\n");
	char const * FragmentSourcePointer = fragment_shader_code.c_str();
	glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer , NULL);
	glCompileShader(FragmentShaderID);

	// Check Fragment SmartShader
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if ( InfoLogLength > 0 ){
		std::vector<char> FragmentShaderErrorMessage(InfoLogLength+1);
		glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
		printf("%s\n", &FragmentShaderErrorMessage[0]);
	}

	// Link the program
	printf("Linking program\n");
	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, VertexShaderID);
	glAttachShader(ProgramID, FragmentShaderID);
	glLinkProgram(ProgramID);

	// Check the program
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if ( InfoLogLength > 0 ){
		std::vector<char> ProgramErrorMessage(InfoLogLength+1);
		glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
		printf("%s\n", &ProgramErrorMessage[0]);
	}


	glDetachShader(ProgramID, VertexShaderID);
	glDetachShader(ProgramID, FragmentShaderID);

	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);

	return ProgramID;
}

char const *vgl::getErrorString(GLenum const err) noexcept {
	switch (err) {
		// opengl 2 errors (8)
		case GL_NO_ERROR:
			return "GL_NO_ERROR";

		case GL_INVALID_ENUM:
			return "GL_INVALID_ENUM";

		case GL_INVALID_VALUE:
			return "GL_INVALID_VALUE";

		case GL_INVALID_OPERATION:
			return "GL_INVALID_OPERATION";

		case GL_STACK_OVERFLOW:
			return "GL_STACK_OVERFLOW";

		case GL_STACK_UNDERFLOW:
			return "GL_STACK_UNDERFLOW";

		case GL_OUT_OF_MEMORY:
			return "GL_OUT_OF_MEMORY";

		case GL_TABLE_TOO_LARGE:
			return "GL_TABLE_TOO_LARGE";

			// opengl 3 errors (1)
		case GL_INVALID_FRAMEBUFFER_OPERATION:
			return "GL_INVALID_FRAMEBUFFER_OPERATION";

			// gles 2, 3 and gl 4 error are handled by the switch above
		default:

			return nullptr;
	}
}

GLenum vgl::checkError(const char *name) {
	GLenum error = glGetError();
	if (error != 0) {
		if (name == nullptr) {
			name = "";
		}
		std::cout << name << " Gl error: " << error << " " << getErrorString(error) << std::endl;
	}
	return error;
}

void vgl::checkErrorAndThrow(const char *name) {
//	std::cout<<"gl: "<<name<<std::endl;
	auto error = glGetError();
	if(error != 0){
		std::stringstream ss;
		ss << name << " Gl error: " << error << " " << getErrorString(error);
		auto message = ss.str();
		std::cerr<<message<<std::endl;
//		throw Exception{
//				.code=error,
//				.message=message
//		};
	}
}
