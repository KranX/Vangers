#ifndef SHADER_H
#define SHADER_H

#include <cstdint>

namespace renderer::compositor::gles3 {
	class Shader
	{
	public:
		Shader();
		void initialize(const char* vs_shader, const char* fs_shader);
		void use();
		static void unuse();
		void set_uniform(const char* name, int value);
		void set_uniform(const char* name, float value[4]);
		void set_uniform_mat(const char* name, float value[16]);
	private:
		uint32_t _program;
	};
}


#endif // SHADER_H
