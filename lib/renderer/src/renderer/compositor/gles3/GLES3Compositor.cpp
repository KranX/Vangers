#include "GLES3Compositor.h"
#include "GLES3Texture.h"
#include "Shader.h"
#include "QuadVertexArray.h"
#include "../CompositorException.h"

#include <glad/glad.h>
#include <cstring>

using namespace renderer::compositor;
using namespace renderer::compositor::gles3;

const char* vs_code = R"(#version 300 es
	precision mediump float;

    uniform vec4 vert_transform;
	uniform vec4 uv_transform;
	in vec2 position;
	in vec2 texcoord0;

	out vec2 uv;

	void main() {
        vec2 pos_tr = position * vert_transform.zw + vert_transform.xy;
        gl_Position = vec4(pos_tr, 0.5, 1.0);
        uv = texcoord0 * uv_transform.zw + uv_transform.xy;
	}
)";

const char* fs_code = R"(#version 300 es
	precision mediump float;

	uniform vec4 color;
	uniform sampler2D tex;

	in vec2 uv;
	out vec4 FragColor;
	void main() {
		FragColor = texture(tex, uv) * color;
     //   FragColor = vec4(uv.x, uv.y, 1.0, 1.0f);
	}
)";


const char * get_debug_type_str(GLenum type){
	switch(type){
	case GL_DEBUG_TYPE_ERROR:
		return "ERROR";
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
		return "DEPRECATED_BEHAVIOR";
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
		return "UNDEFINED_BEHAVIOR";
	case GL_DEBUG_TYPE_PORTABILITY:
		return "PORTABILITY";
	case GL_DEBUG_TYPE_PERFORMANCE:
		return "PERFORMANCE";
	case GL_DEBUG_TYPE_OTHER:
		return "OTHER";
	case GL_DEBUG_TYPE_MARKER:
		return "MARKER";
	case GL_DEBUG_TYPE_POP_GROUP:
		return "POP_GROUP";
	case GL_DEBUG_TYPE_PUSH_GROUP:
		return "PUSH_GROUP";
	default:
		return "Unknown";
	}
}

const char* get_debug_severity_str(GLenum severity){
	switch(severity){
	case GL_DEBUG_SEVERITY_HIGH:
		return "HIGH";
	case GL_DEBUG_SEVERITY_LOW:
		return "LOW";
	case GL_DEBUG_SEVERITY_MEDIUM:
		return "MEDIUM";
	case GL_DEBUG_SEVERITY_NOTIFICATION:
		return "NOTIFICATION";
	default:
		return "Unknown";
	}
}

const char* get_debug_source_str(GLenum source){
	switch(source){
	case GL_DEBUG_SOURCE_API:
		return "API";
	case GL_DEBUG_SOURCE_APPLICATION:
		return "APPLICATION";
	case GL_DEBUG_SOURCE_OTHER:
		return "OTHER";
	case GL_DEBUG_SOURCE_SHADER_COMPILER:
		return "SHADER_COMPILER";
	case GL_DEBUG_SOURCE_THIRD_PARTY:
		return "THIRD_PARTY";
	case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
		return "WINDOW_SYSTEM";
	default:
		return "Unknown";
	}
}

void DebugCallbackARB(GLenum source,
                      GLenum type,
                      GLuint id,
                      GLenum severity,
                      GLsizei length,
                      const GLchar* message,
                      const GLvoid* userParam) {
	if(userParam != nullptr ||
	   type == GL_DEBUG_TYPE_ERROR)
	{
		std::cout << "GL Debug Message:"
		          << std::endl
		          << "\tseverity=" << get_debug_severity_str(severity)
		          << ", type=" << get_debug_type_str(type)
		          << ", source=" << get_debug_source_str(source)
		          << std::endl;

		std::cout << "\tmessage=" << std::string(message).substr(0, length) << std::endl;
	}
}

GLES3Compositor::GLES3Compositor(int32_t screen_width, int32_t screen_height, GLADloadproc loadproc)
	: _screen_width(screen_width)
    , _screen_height(screen_height)
	, _logical_screen_width(screen_width)
	, _logical_screen_height(screen_height)
	, _texture_storage()
	, _texture_shader(std::make_unique<Shader>())
	, _vertex_array(std::make_unique<QuadVertexArray>())
	, _previous_vao(0)
{
	gladLoadGLES2Loader(loadproc);


//	 pass here any value to catch only every debug message
	if(glDebugMessageCallback != nullptr){
		glEnable(GL_DEBUG_OUTPUT);
		glDebugMessageCallback(&DebugCallbackARB, nullptr);
	}

	const GLubyte * glVersion = glGetString(GL_VERSION);
	std::cout << "glVersion: " << glVersion <<std::endl;
	_texture_shader->initialize(vs_code, fs_code);
	_texture_shader->use();
	_texture_shader->set_uniform("tex", 0);
	_texture_shader->unuse();
	_vertex_array->initialize();
}

GLES3Compositor::~GLES3Compositor()
{

}

Texture GLES3Compositor::texture_create(int32_t width, int32_t height, TextureType texture_type, BlendMode blend_mode)
{
	return _texture_storage.create(std::make_unique<GLES3Texture>(width, height, texture_type, blend_mode, nullptr));
}

void GLES3Compositor::texture_set_data(Texture texture, uint8_t* data)
{
	auto& t = _texture_storage.getOrThrow(texture);

	glBindTexture(GL_TEXTURE_2D, t->name());
	// reset unpack options
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
	glPixelStorei(GL_UNPACK_IMAGE_HEIGHT, 0);
	glTexSubImage2D(GL_TEXTURE_2D,
					0, 0, 0, t->width(), t->height(),
					// TODO:
					GL_RGBA, GL_UNSIGNED_BYTE, data);
}

void GLES3Compositor::texture_render(Texture texture, const renderer::Rect& src_rect, const renderer::Rect& dst_rect)
{
	auto& t = _texture_storage.getOrThrow(texture);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, t->name());

	GLuint gl_error = glGetError();
	if(gl_error != 0){
		std::cout << "glBindTexture error: " << gl_error << std::endl;
	}

	_texture_shader->use();
	_vertex_array->bind();
// TODO:



	float uv_translate_x = 0;
	float uv_translate_y = 0;
	float uv_scale_x = 1;
	float uv_scale_y = 1;
	if(src_rect.width != 0 && src_rect.height != 0){
		uv_translate_x = (float)src_rect.x / (float)t->width();
		uv_translate_y = (float)src_rect.y / (float)t->height();
		uv_scale_x = (float)(src_rect.x + src_rect.width) / (float)t->width();
		uv_scale_y = (float)(src_rect.y + src_rect.height) / (float)t->height();
	}

	float uv_transform[4] = {
	    uv_translate_x,
	    uv_translate_y,
	    uv_scale_x,
	    uv_scale_y,
	};

	float vert_image_scale_x = 1;
	float vert_image_scale_y = 1;

	float vert_translate_x = 0;
	float vert_translate_y = 0;

	if(dst_rect.width != 0 && dst_rect.height != 0){
		vert_image_scale_x = (float) dst_rect.width / (float) _screen_width;
		vert_image_scale_y = (float) dst_rect.height / (float) _screen_height;

		vert_translate_x = -1.0f + vert_image_scale_x + 2.0f * (float)dst_rect.x / (float)_screen_width;
		vert_translate_y = 1.0f - vert_image_scale_y - 2.0f * (float)dst_rect.y / (float)_screen_height;
	}

	float vert_transform[4] = {
	    vert_translate_x,
	    vert_translate_y,
	    vert_image_scale_x,
	    vert_image_scale_y
	};

	Color tcolor = t->color();
	float color[4] = {
	    (float)tcolor.r / 255.0f,
	    (float)tcolor.g / 255.0f,
	    (float)tcolor.b / 255.0f,
	    (float)tcolor.a / 255.0f
	};

	_texture_shader->set_uniform("vert_transform", vert_transform);
	_texture_shader->set_uniform("uv_transform", uv_transform);
	_texture_shader->set_uniform("color", color);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	_texture_shader->unuse();
	_vertex_array->unbind();
}

void GLES3Compositor::texture_destroy(Texture texture)
{
	auto& t = _texture_storage.getOrThrow(texture);
	t->destroy();
	_texture_storage.remove(texture);
}

void GLES3Compositor::texture_query(Texture texture, int32_t* width, int32_t* height, TextureType* texture_type, BlendMode* blend_mode)
{
	auto& t = _texture_storage.getOrThrow(texture);

	if(width != nullptr){
		*width = t->width();
	}

	if(height != nullptr){
		*height = t->height();
	}

	if(texture_type != nullptr){
		*texture_type = t->texture_type();
	}

	if(blend_mode != nullptr){
		*blend_mode = t->blend_mode();
	}
}

void GLES3Compositor::texture_set_color(Texture texture, const renderer::Color& color)
{
	auto& t = _texture_storage.getOrThrow(texture);
	t->set_color(color);
}

const char* guiDebugGroup = "GLES3Compositor render";

void GLES3Compositor::render_begin()
{
	if(glPushDebugGroup != nullptr){
		glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, std::strlen(guiDebugGroup), guiDebugGroup);
	}

	glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &_previous_vao);

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_SCISSOR_TEST);
	glDisable(GL_STENCIL_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glViewport(0, 0, _logical_screen_width, _logical_screen_height);
}

void GLES3Compositor::render_present()
{
	glBindVertexArray(_previous_vao);
	if(glPopDebugGroup != nullptr){
		glPopDebugGroup();
	}
}

void GLES3Compositor::initialize()
{

}

void GLES3Compositor::dispose()
{
	for(auto& [rid, t]: _texture_storage){
		texture_destroy(rid);
	}

	// TODO: shader
	// TODO: VAO
}

void GLES3Compositor::query_output_size(int32_t* width, int32_t* height)
{
	if(width != nullptr){
		*width = _screen_width;
	}

	if(height != nullptr){
		*height = _screen_height;
	}
}

void GLES3Compositor::set_physical_screen_size(int32_t width, int32_t height)
{
	_screen_width = width;
	_screen_height = height;
}

void GLES3Compositor::set_logical_screen_size(int32_t width, int32_t height)
{
	_logical_screen_width = width;
	_logical_screen_height = height;
}

void GLES3Compositor::read_pixels(uint8_t*)
{
	// TODO:
	throw CompositorException("GLES3Compositor::read_pixels is not implemented");
}
