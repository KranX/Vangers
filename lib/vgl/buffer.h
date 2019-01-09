//
// Created by nikita on 2018-12-03.
//

#ifndef UPLOADBENCH_BUFFER_H
#define UPLOADBENCH_BUFFER_H

#include <memory>
#include <iostream>
#include <string.h>

#include "base.h"

namespace vgl {
	enum class BufferTarget : GLuint {
		PixelUnpackBuffer = GL_PIXEL_UNPACK_BUFFER,
		ArrayBuffer = GL_ARRAY_BUFFER,
		ElementArrayBuffer = GL_ELEMENT_ARRAY_BUFFER,
	};

	enum class BufferUsage : GLuint {
		StreamDraw = GL_STREAM_DRAW,
		StaticDraw = GL_STATIC_DRAW
	};

	template<BufferTarget target>
	class Buffer: NamedObject<GLuint>{

	private:
		static const GLuint glTarget = static_cast<GLuint>(target);

		GLsizeiptr size;
	public:
		Buffer(GLuint objectId, GLsizeiptr size) : NamedObject(objectId), size(size) {

		}

		Buffer(Buffer&& other) noexcept: NamedObject(other.objectId), size(other.size) {

		}

		Buffer& operator=(Buffer&& other) noexcept {

			this->objectId = other.objectId;
			this->size = other.size;
			other.objectId = 0;
			other.size = 0;
			return *this;
		};

		Buffer(const Buffer& buffer) = delete;

		Buffer& operator=(const Buffer& buffer) = delete;


		static std::shared_ptr<Buffer> create(GLsizeiptr size, BufferUsage usage, GLvoid* data = nullptr){
			GLuint objectId;
			glGenBuffers(1, &objectId);
			vgl::checkErrorAndThrow("glGenBuffers");

			glBindBuffer(glTarget, objectId);
			vgl::checkErrorAndThrow("glBindBuffer");

			glBufferData(glTarget, size, data, static_cast<GLenum>(usage));
			vgl::checkErrorAndThrow("glBufferData");

			return std::make_shared<Buffer<target>>(objectId, size);
		}

		GLsizeiptr getSize() const {
			return size;
		}

		void update(GLubyte* data){
			glBufferData(glTarget, size, nullptr, GL_STREAM_DRAW);
			checkErrorAndThrow("glBufferData");

			GLubyte* ptr = (GLubyte*)glMapBuffer(glTarget, GL_WRITE_ONLY);
			vgl::checkErrorAndThrow("glMapBuffer");
			if(ptr)
			{
				memcpy(ptr, data, size);
				glUnmapBuffer(glTarget);
				vgl::checkErrorAndThrow("glUnmapBuffer");
			}else{
				std::cerr<<"Unabled to map buffer"<<std::endl;
			}
		}

		void free(){
			glDeleteBuffers(1, &objectId);
			vgl::checkErrorAndThrow("glDeleteBuffers");
		}

		void bind() const{
			glBindBuffer(glTarget, objectId);
			vgl::checkErrorAndThrow("glBindBuffer");
		}

		static void unbind(){
			glBindBuffer(glTarget, 0);
			vgl::checkErrorAndThrow("glBindBuffer to zero");
		}
	};

	typedef Buffer<BufferTarget::ArrayBuffer> ArrayBuffer;
	typedef Buffer<BufferTarget::ElementArrayBuffer> ElementArrayBuffer;
	typedef Buffer<BufferTarget::PixelUnpackBuffer> PixelUnpackBuffer;


}

#endif //UPLOADBENCH_BUFFER_H
