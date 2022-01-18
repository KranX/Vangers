#ifndef QUADVERTEXARRAY_H
#define QUADVERTEXARRAY_H

#include <cstdint>

namespace renderer::compositor::gles3 {
	class QuadVertexArray
	{
	public:
		QuadVertexArray();
		void initialize();
		void bind();
		static void unbind();

		inline uint32_t VAO() const {
			return _vao;
		}

	private:
		uint32_t _vao;
	};
}

#endif // QUADVERTEXARRAY_H
