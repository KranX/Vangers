//
// Created by nikita on 2019-01-03.
//

#include "vertexarray_ext.h"

const std::vector<vgl::Pos2Tex2Vertex> vgl::Pos2Tex2Vertex::quadElements = {
	{{-1.0f,  -1.0f}, {0.0f, 0.0f}}, // Top-left
	{{1.0f,  -1.0f}, {1.0f, 0.0f}}, // Top-right
	{{1.0f, 1.0f}, {1.0f, 1.0f}}, // Bottom-right
	{{-1.0f, 1.0f}, {0.0f, 1.0f}}  // Bottom-left
};

void vgl::Pos2Tex2Vertex::addAttribs(vgl::IVertexArray &vertexArray) {
	vertexArray.addAttrib(0, 2, offsetof(Pos2Tex2Vertex, pos));
	vertexArray.addAttrib(1, 2, offsetof(Pos2Tex2Vertex, uv));
}

const std::vector<vgl::Pos2Vertex> vgl::Pos2Vertex::quadElements = {
		{{-1.0f,  -1.0f}}, // Top-left
		{{1.0f,  -1.0f}}, // Top-right
		{{1.0f, 1.0f}}, // Bottom-right
		{{-1.0f, 1.0f}}  // Bottom-left
};

void vgl::Pos2Vertex::addAttribs(vgl::IVertexArray &vertexArray) {
	vertexArray.addAttrib(0, 2, offsetof(Pos2Tex2Vertex, pos));
}
