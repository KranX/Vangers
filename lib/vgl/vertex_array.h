#include <utility>

#include <utility>

//
// Created by nikita on 2018-12-03.
//

#ifndef VGLVERTEX_ARRAY_H
#define VGLVERTEX_ARRAY_H

#include <vector>
#include <memory>
#include <string>

#include "util.h"
#include "base.h"
#include "buffer.h"
#include "ivertexarray.h"


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

		void enable() const{
			glEnableVertexAttribArray(objectId);
			vgl::checkErrorAndThrow("glEnableVertexAttribArray");

			glVertexAttribPointer(objectId, size, GL_FLOAT, GL_FALSE, stride, (void*)(offset));
			vgl::checkErrorAndThrow("glVertexAttribPointer");
		}

	};


	template<typename VertexType>
	class VertexArray : public IVertexArray, NamedObject<GLuint>{
	private:
		std::shared_ptr<ArrayBuffer> vertexBuffer;
		std::shared_ptr<ElementArrayBuffer> elementBuffer;

		std::vector<VertexArrayAttrib> vertexArrayAttribs;
		int numElements;
		int numVerticies;

	public:
		typedef VertexType vertexType;

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

		static std::shared_ptr<VertexArray> create(std::vector<VertexType> &verticies, std::vector<GLuint>& elements){
			GLuint objectId;
			glGenVertexArrays(1, &objectId);
			vgl::checkErrorAndThrow("glGenVertexArrays");
			glBindVertexArray(objectId);
			vgl::checkErrorAndThrow("glBindVertexArray");
			auto vertexBuffer = ArrayBuffer::create(sizeof(VertexType) * verticies.size(), BufferUsage::StaticDraw, &verticies[0]);
			auto elementBuffer = ElementArrayBuffer::create(sizeof(GLuint) * elements.size(), BufferUsage::StaticDraw, &elements[0]);
			return std::make_shared<VertexArray<VertexType>>(
				objectId,
				elements.size(),
				verticies.size(),
				vertexBuffer,
				elementBuffer
			);
		}

		int getNumElements() const override {
			return numElements;
		}

		void free() override {
			vertexBuffer->free();
			elementBuffer->free();
			glDeleteVertexArrays(1, &objectId);
		}

		void enable() const override {
			vertexBuffer->bind();
			elementBuffer->bind();

			for(size_t i = 0; i < vertexArrayAttribs.size(); i++){
				vertexArrayAttribs[i].enable();
			}
		}

		void bind() const override {
			glBindVertexArray(objectId);
			vgl::checkErrorAndThrow("glBindVertexArray");
		}

		void unbind() const override {
			glBindVertexArray(0);
		}

		void disable() const override {
			for(int i = 0; i < vertexArrayAttribs.size(); i++){
				glDisableVertexAttribArray(i);
				vgl::checkErrorAndThrow("glDisableVertexAttribArray");
			}
		}

		void addAttrib(GLuint attribId, size_t size, size_t offset) override {
			VertexArrayAttrib vat(attribId, size, offset, sizeof(VertexType));
			vertexArrayAttribs.push_back(vat);
		}
	};


}

#endif //VGL_VERTEX_ARRAY_H
