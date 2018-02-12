//
// Created by nikita on 2/15/18.
//

#ifndef VANGERS_GL_H
#define VANGERS_GL_H

#include <GL/glew.h>
#include <string>
#include <exception>
#include <memory>
#include <utility>

class OpenGlException: public std::exception
{
private:
	std::string message;
public:
	explicit OpenGlException(std::string message):message(std::move(message)){}
	const char* what() const throw() override {
		return message.c_str();
	}
};

enum class TextureFormat {
	Format8Bit,
	Format32bit,
	FormatPalette,
};

class Texture {
public:
	int width;
	int height;
	GLuint textureId = 0;
	TextureFormat format;
	static std::shared_ptr<Texture> createTexture(int width, int height, TextureFormat format);
	static std::shared_ptr<Texture> createTexture(int width, int height, TextureFormat format, void* data);
	static std::shared_ptr<Texture> createPalette(int width);
	void bindData(void* data);
	void update(int x, int y, int width, int height, void *data);
private:
	Texture(int width, int height, TextureFormat format) : width(width), height(height),
	                                                                         textureId(textureId), format(format) {}

};

struct BufferData {
	GLuint vertexArrayObject;
	GLuint vertexBufferObject;
	GLuint elementBufferObject;
};

GLuint bindTexture8bit(int width, int height, uint8_t* data);
GLuint bindTexture32bit(int width, int height, uint8_t* data);
GLuint bindTexturePalette(int width, uint8_t* data);
std::shared_ptr<BufferData> genBuffer(const GLfloat * vertices, const GLuint* elements, GLsizeiptr verticesSize, GLsizeiptr elementsSize);
GLuint load_shaders_str(const std::string &vertex_shader_code, const std::string &fragment_shader_code);
GLuint load_shaders(const std::string &vertex_file_path, const std::string &fragment_file_path);
void check_GL_error(const char* context);


template <class T> class Shader {

protected:
	virtual void render_impl() = 0;
public:
	std::shared_ptr<T> data;
	GLuint programId;
	Shader (const std::shared_ptr<T> data, const std::string& shader_path)
	{
		programId = load_shaders(shader_path+".vert", shader_path+".frag");
		this->data = data;
	}

	Shader (const std::shared_ptr<T> data, const std::string &vertex_shader_code, const std::string &fragment_shader_code)
	{
		programId = load_shaders_str(vertex_shader_code, fragment_shader_code);
		this->data = data;
	}

	void render(){
		glUseProgram(programId);
		render_impl();
	}
};

class TextureShaderData {
public:
	std::shared_ptr<Texture> texture;
	GLint textureAttrId;
	GLint positionAttrId;
	GLint texcoordAttrId;
	std::shared_ptr<BufferData> bufferData;
};

class TextureShader : public Shader<TextureShaderData> {
private:

public:
	explicit TextureShader(const std::shared_ptr<Texture> &texture);
	void render_impl() override;
};
#endif //VANGERS_GL_H

