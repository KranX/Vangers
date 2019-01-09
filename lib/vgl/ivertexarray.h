//
// Created by nikita on 2018-12-12.
//

#ifndef VANGERS_IVERTEXARRAY_H
#define VANGERS_IVERTEXARRAY_H

namespace vgl {


	class IVertexArray {
	public:
		virtual int getNumElements() const = 0 ;

		virtual  void free() = 0;

		virtual void enable() const = 0;

		virtual void bind() const = 0;

		virtual  void unbind() const = 0;

		virtual  void disable() const = 0;

		virtual void addAttrib(GLuint attribId, size_t size, size_t offset) = 0;

	};

}


#endif //VANGERS_IVERTEXARRAY_H
