//
// Created by nikita on 2018-12-02.
//

#ifndef UPLOADBENCH_UTIL_H
#define UPLOADBENCH_UTIL_H

#include <string>
#include <GL/glew.h>
#include <iostream>
//#include <stdio>

//#define runAndCheck(func, __VA_ARGS__) {printf("%")}

namespace vgl{
	GLuint loadShaders(const std::string &vertexFilePath, const std::string &fragmentFilePath);
	GLuint loadShadersStr(const std::string &vertex_shader_code, const std::string &fragment_shader_code);

	char const *getErrorString(GLenum const err) noexcept;

	GLenum checkError(const char *name = nullptr);

	class Exception{
	public:
		GLenum code;
		const std::string message;
	};

	void checkErrorAndThrow(const char* name = nullptr);

	template<typename T>
	void print(T a) {
		// In real-world code, we wouldn't compare floating point values like
		// this. It would make sense to specialize this function for floating
		// point types to use approximate comparison.
		std::cout<<a<<std::endl;
	}

	template<typename T, typename... Args>
	void print(T a, Args... args) {
		std::cout<<a;
		print(args...);
	}

	template<typename Fn, Fn fn, typename... Args>
	typename std::result_of<Fn(Args...)>::type
	wrapper(Args&&... args) {
		print(std::forward<Args>(args)...);
		auto res = fn(std::forward<Args>(args)...);
		return res;
	}

	#define WRAPPER(FUNC, ...) wrapper<decltype(&FUNC), &FUNC>(__VA_ARGS__)



}
#endif //UPLOADBENCH_UTIL_H
