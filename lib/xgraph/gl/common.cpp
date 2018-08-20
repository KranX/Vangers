//
// Created by nikita on 3/18/18.
//

#include "common.h"

#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>

namespace gl{
	void check_GL_error(const char* context) {
		GLenum err = glGetError();
		if (err) {
			auto message = std::string("OpenGL error when using ")+context+", code: "+std::to_string(err);
			std::cout<<message<<std::endl;
//		throw OpenGlException(std::string("OpenGL error when using ")+context+", code: "+std::to_string(err));
		}
	}

	GLuint load_shaders(const std::string &vertex_file_path, const std::string &fragment_file_path){
		std::cout<<"load_shaders: "<<vertex_file_path<<" "<<fragment_file_path<<std::endl;
		std::string VertexShaderCode;
		std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
		if(VertexShaderStream.is_open()){
			std::stringstream sstr;
			sstr << VertexShaderStream.rdbuf();
			VertexShaderCode = sstr.str();
			VertexShaderStream.close();
		}else{
			// TODO: handle this right
			printf("Impossible to open %s.\n", vertex_file_path.c_str());
			exit(1);
		}

		std::string FragmentShaderCode;
		std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
		if(FragmentShaderStream.is_open()){
			std::stringstream sstr;
			sstr << FragmentShaderStream.rdbuf();
			FragmentShaderCode = sstr.str();
			FragmentShaderStream.close();
		}
		return load_shaders_str(VertexShaderCode, FragmentShaderCode);
	}

	GLuint load_shaders_str(const std::string &vertex_shader_code, const std::string &fragment_shader_code)
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


	std::shared_ptr<VertexBufferData> gen_buffer(const GLfloat *vertices, const GLuint *elements,
	                                                     GLsizeiptr verticesSize, GLsizeiptr elementsSize) {
		// Create Vertex Array Object
		GLuint vao;
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		// Create a Vertex Buffer Object and copy the vertex data to it
		GLuint vbo;
		glGenBuffers(1, &vbo);

		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, verticesSize, vertices, GL_STATIC_DRAW);

		// Create an element array
		GLuint ebo;
		glGenBuffers(1, &ebo);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, elementsSize, elements, GL_STATIC_DRAW);
		return std::shared_ptr<VertexBufferData>(new VertexBufferData{
				.vertexArrayObject= vao,
				.vertexBufferObject= vbo,
				.elementBufferObject= ebo,
		});
	}

	GLuint gen_texture(GLint internalFormat, GLenum format, GLenum type, GLint filter, int width, int height, int numLayers, void *data) {
		GLenum  target;
		if(numLayers == 0){
		    target = GL_TEXTURE_2D;
		}else{
		    target = GL_TEXTURE_2D_ARRAY;
		}

	    GLuint textureID;
		glGenTextures(1, &textureID);
		glBindTexture(target, textureID);

		if(numLayers == 0){
            glTexImage2D(
                GL_TEXTURE_2D,
                0,
                internalFormat, //GL_R8UI,
                width,
                height,
                0,
                format, //GL_RED_INTEGER,
                type,
                data
            );
		}else{
            glTexImage3D(
                target,
                0,
                internalFormat, //GL_R8UI,
                width,
                height,
                numLayers,
                0,
                format, //GL_RED_INTEGER,
                type,
                data
            );
		}

		check_GL_error("glTexImage3D");
		glTexParameteri(target, GL_TEXTURE_MAG_FILTER, filter);
		glTexParameteri(target, GL_TEXTURE_MIN_FILTER, filter);
		check_GL_error("glTexParameteri");
		return textureID;
	}

	GLuint gen_palette(int width, uint32_t *data) {
		GLuint textureID;
		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_1D, textureID);

		glTexImage1D(
				GL_TEXTURE_1D,
				0,
				GL_RGB,
				width,
				0,
				GL_RGBA,
				GL_UNSIGNED_INT_8_8_8_8,
				data
		);
		glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

		return textureID;
	}
}
