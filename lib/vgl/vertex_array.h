#include <utility>

#include <utility>

//
// Created by nikita on 2018-12-03.
//

#ifndef UPLOADBENCH_VERTEX_ARRAY_H
#define UPLOADBENCH_VERTEX_ARRAY_H

#include <vector>
#include <memory>
#include <string>

#include "util.h"
#include "base.h"
#include "buffer.h"


namespace vgl {

class VertexArrayAttrib : public NamedObject<GLuint>{
	private:
		GLint size;
		size_t offset;
		size_t stride;
	public:
		VertexArrayAttrib(GLuint objectId, GLint size, size_t offset, size_t stride) : NamedObject(objectId) {
			this->size = size;
			this->offset = offset;
			this->stride = stride;
		}

		void enable(){
			glEnableVertexAttribArray(objectId);
			vgl::checkErrorAndThrow("glEnableVertexAttribArray");

			glVertexAttribPointer(objectId, size, GL_FLOAT, GL_FALSE, stride, (void*)(offset));
			vgl::checkErrorAndThrow("glVertexAttribPointer");
		}

	};

	class IVertexArray {
	public:
		virtual int getNumElements() const = 0 ;

		virtual  void free() = 0;

		virtual void enable() = 0;

		virtual void bind() = 0;

		virtual  void unbind() = 0;

		virtual  void disable() = 0;

		virtual void addAttrib(GLuint attribId, size_t size, size_t offset) = 0;

	};


	template<typename VertexType, typename ElementType>
	class VertexArray : public IVertexArray, NamedObject<GLuint>{
	private:
		std::shared_ptr<ArrayBuffer> vertexBuffer;
		std::shared_ptr<ElementArrayBuffer> elementBuffer;

		std::vector<VertexArrayAttrib> vertexArrayAttribs;
		int numElements;
		int numVerticies;

	public:
		VertexArray(GLuint objectId,
					int numElements,
				    int numVerticies,
				    std::shared_ptr<ArrayBuffer> vertexBuffer,
				    std::shared_ptr<ElementArrayBuffer> elementBuffer):
				NamedObject(objectId),
				vertexBuffer(std::move(vertexBuffer)),
				elementBuffer(std::move(elementBuffer)),
				numElements(numElements),
				numVerticies(numVerticies)
				{
		}

		VertexArray(VertexArray&& other) noexcept:
			NamedObject(other.objectId),
			vertexBuffer(other.vertexBuffer),
			elementBuffer(other.elementBuffer),
			numElements(other.numElements),
			numVerticies(other.numVerticies)
			{

			}

		VertexArray(const VertexArray& ) = delete;
		VertexArray& operator=(const VertexArray& ) = delete;

		static std::shared_ptr<VertexArray> create(std::vector<VertexType> &verticies, std::vector<ElementType>& elements){
			GLuint objectId;
			glGenVertexArrays(1, &objectId);
			vgl::checkErrorAndThrow("glGenVertexArrays");
			glBindVertexArray(objectId);
			vgl::checkErrorAndThrow("glBindVertexArray");
			auto vertexBuffer = ArrayBuffer::create(sizeof(VertexType) * verticies.size(), BufferUsage::StaticDraw, &verticies[0]);
			auto elementBuffer = ElementArrayBuffer::create(sizeof(ElementType) * elements.size(), BufferUsage::StaticDraw, &elements[0]);
			return std::make_shared<VertexArray<VertexType, ElementType>>(
				objectId,
				elements.size(),
				verticies.size(),
				vertexBuffer,
				elementBuffer
			);
		}

		int getNumElements() const {
			return numElements;
		}

		void free(){
			vertexBuffer->free();
			elementBuffer->free();
		}
		void enable(){
			bind();
			vertexBuffer->bind();
			elementBuffer->bind();

			for(int i = 0; i < vertexArrayAttribs.size(); i++){
				vertexArrayAttribs[i].enable();
			}
		}

		void bind() {
			glBindVertexArray(objectId);
			vgl::checkErrorAndThrow("glBindVertexArray");
		}

		void unbind(){
			glBindVertexArray(0);
		}

		void disable(){
			for(int i = 0; i < vertexArrayAttribs.size(); i++){
				glDisableVertexAttribArray(i);
				vgl::checkErrorAndThrow("glDisableVertexAttribArray");
			}
		}

//		void defineData(std::vector<VertexType> &data, std::vector<ElementType>& elements){
//			glBindVertexArray(objectId);
//			vertexBuffer = Buffer::create(sizeof(VertexType) * data.size(), GL_STATIC_DRAW, &data[0]);
//			elementBuffer = Buffer::create(sizeof(ElementType) * elements.size(), GL_STATIC_DRAW, &elements[0]);
//			numElements = data.size();
//		}

		void addAttrib(GLuint attribId, size_t size, size_t offset) {
			VertexArrayAttrib vat(attribId, size, offset, sizeof(VertexType));
			vertexArrayAttribs.push_back(vat);
		}
	};


}

#endif //UPLOADBENCH_VERTEX_ARRAY_H
