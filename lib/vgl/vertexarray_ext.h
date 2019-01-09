//
// Created by nikita on 2019-01-03.
//

#ifndef VANGERS_VERTEXARRAY_EXT_H
#define VANGERS_VERTEXARRAY_EXT_H

#include <vector>
#include <memory>

#include "vertex_array.h"
#include "glm/glm.hpp"


namespace vgl {
	struct Pos2Tex2Vertex{
		glm::vec2 pos;
		glm::vec2 uv;

		static const std::vector<Pos2Tex2Vertex> quadElements;

		static void addAttribs(IVertexArray& vertexArray);
	};

	struct Pos2Vertex{
		glm::vec2 pos;

		static const std::vector<Pos2Vertex> quadElements;

		static void addAttribs(IVertexArray& vertexArray);
	};

	template <typename VertexType>
	static std::shared_ptr<VertexArray<VertexType>> createQuad(){
		std::vector<GLuint> elements = {
				0, 1, 2,
				3, 2, 0,
		};

		std::vector<VertexType> vertices = VertexType::quadElements;

		auto result = VertexArray<VertexType>::create(vertices, elements);
		VertexType::addAttribs(*result);
		return result;
	}

	typedef VertexArray<Pos2Tex2Vertex> VertexArrayPos2Tex2;

	typedef VertexArray<Pos2Vertex> VertexArrayPos2;

}

#endif //VANGERS_VERTEXARRAY_EXT_H
