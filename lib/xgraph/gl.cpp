//
// Created by nikita on 2/15/18.
//

#include "gl.h"
#include <fstream>
#include <sstream>
#include <vector>
#include <iostream>

GLuint load_shaders(const std::string &vertex_file_path, const std::string &fragment_file_path){
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


	// Compile Vertex Shader
	printf("Compiling shader\n");
	char const * VertexSourcePointer = vertex_shader_code.c_str();
	glShaderSource(VertexShaderID, 1, &VertexSourcePointer , NULL);
	glCompileShader(VertexShaderID);

	// Check Vertex Shader
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if ( InfoLogLength > 0 ){
		std::vector<char> VertexShaderErrorMessage(InfoLogLength+1);
		glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
		printf("%s\n", &VertexShaderErrorMessage[0]);
	}

	// Compile Fragment Shader
	printf("Compiling shader\n");
	char const * FragmentSourcePointer = fragment_shader_code.c_str();
	glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer , NULL);
	glCompileShader(FragmentShaderID);

	// Check Fragment Shader
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

void check_GL_error(const char* context) {
	GLenum err = glGetError();
	if (err) {
		auto message = std::string("OpenGL error when using ")+context+", code: "+std::to_string(err);
		std::cout<<message<<std::endl;
//		throw OpenGlException(std::string("OpenGL error when using ")+context+", code: "+std::to_string(err));
	}
}

GLuint genTexture(GLint internalFormat, GLenum format, GLenum type, GLint filter, int width, void *data, int height) {
	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
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
	check_GL_error("glTexImage2D");
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
	check_GL_error("glTexParameteri");
	return textureID;
}

GLuint genPalette(int width, uint32_t *data) {
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

std::shared_ptr<Texture> Texture::createTexture(int width, int height, TextureFormat format) {
	Texture* tex = new Texture(width, height, format);
	auto texture = std::shared_ptr<Texture>(tex);
	return texture;
}

std::shared_ptr<Texture> Texture::createPalette(int width) {
	auto texture = std::shared_ptr<Texture>(new Texture(width, 0, TextureFormat::FormatPalette));
	return texture;
}

void Texture::bindData(void *data) {
	switch (format) {
		case TextureFormat::FormatPalette:
			textureId = genPalette(width, static_cast<uint32_t *>(data));
			break;
		case TextureFormat ::Format8Bit:
			textureId = genTexture(GL_R8UI, GL_RED_INTEGER, GL_UNSIGNED_BYTE, GL_NEAREST, width, data, height);
			break;
		case TextureFormat ::Format32bit:
			textureId = genTexture(GL_RGBA, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, GL_LINEAR, width, data, height);
			break;
	}

}

void Texture::update(int x, int y, int width, int height, void *data) {
	if(format == TextureFormat::Format32bit){
		glBindTexture(GL_TEXTURE_2D, textureId);
		glTexSubImage2D(
				GL_TEXTURE_2D,
				0,
				x,
				y,
				width,
				height,
				GL_RGBA,
				GL_UNSIGNED_INT_8_8_8_8,
				data
		);
	} else if(format == TextureFormat::FormatPalette) {
		glBindTexture(GL_TEXTURE_1D, textureId);
		glTexSubImage1D(
				GL_TEXTURE_1D,
				0,
				x,
				width,
				GL_RGBA,
				GL_UNSIGNED_INT_8_8_8_8,
				data
		);
	} else if(format == TextureFormat::Format8Bit) {
		glBindTexture(GL_TEXTURE_2D, textureId);
		glTexSubImage2D(
				GL_TEXTURE_2D,
				0,
				x,
				y,
				width,
				height,
				GL_RED_INTEGER,
				GL_UNSIGNED_BYTE,
				data
		);

	} else {
		fprintf(stderr, "Invalid format\n");
	}
	check_GL_error("glTextureSubImage2D");
}

std::shared_ptr<Texture> Texture::createTexture(int width, int height, TextureFormat format, void *data) {
	auto texture = createTexture(width, height, format);
	texture->bindData(data);
	return texture;
}

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

TextureShader::TextureShader(const std::shared_ptr<Texture> &texture)
		: Shader(std::shared_ptr<TextureShaderData>(new TextureShaderData), vertex_code, fragment_code) {

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

	auto bufferData = genBuffer(vertices, elements, sizeof(vertices), sizeof(elements));
	data->bufferData = bufferData;
	data->texture = texture;
	data->textureAttrId = glGetUniformLocation(programId, "t_Texture");
	data->positionAttrId = glGetAttribLocation(programId, "position");
	data->texcoordAttrId = glGetAttribLocation(programId, "texcoord");

}

std::shared_ptr<BufferData> genBuffer(const GLfloat * vertices, const GLuint* elements, GLsizeiptr verticesSize, GLsizeiptr elementsSize) {
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
	return std::shared_ptr<BufferData>(new BufferData{
			vertexArrayObject: vao,
			vertexBufferObject: vbo,
			elementBufferObject: ebo,
	});
}

void TextureShader::render_impl() {
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, data->texture->textureId);
	glUniform1i(data->textureAttrId, 0);

	glBindVertexArray(data->bufferData->vertexArrayObject);

	glEnableVertexAttribArray(data->positionAttrId);
	glBindBuffer(GL_ARRAY_BUFFER, data->bufferData->vertexBufferObject);
	glVertexAttribPointer(data->positionAttrId, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);

	glEnableVertexAttribArray(data->texcoordAttrId);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, data->bufferData->elementBufferObject);
	glVertexAttribPointer(data->texcoordAttrId, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*)(2 * sizeof(GLfloat)));

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
}
