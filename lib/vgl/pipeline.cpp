//
// Created by nikita on 2018-12-17.
//

#include "pipeline.h"

vgl::Pipeline::Pipeline(const std::shared_ptr<vgl::Shader> &shader,
                        const std::shared_ptr<vgl::IVertexArray> &vertexArray,
                        const std::vector<vgl::TextureAttribute> &textureAttributes) : shader(shader),
                                                                                       vertexArray(vertexArray),
                                                                                       textureAttributes(
		                                                                                       textureAttributes) {}

void vgl::Pipeline::render(const UniformData &data) {
	shader->render(data, *vertexArray, textureAttributes);
}

std::shared_ptr<vgl::Pipeline> vgl::Pipeline::create(
		const std::shared_ptr<vgl::Shader> &shader,
		const std::shared_ptr<vgl::IVertexArray> &vertexArray,
		const std::vector<vgl::TextureAttribute> &textureAttributes) {
	return std::make_shared<vgl::Pipeline>(shader, vertexArray, textureAttributes);
}