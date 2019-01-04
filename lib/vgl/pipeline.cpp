//
// Created by nikita on 2018-12-17.
//

#include "pipeline.h"
#include <vector>

vgl::Pipeline::Pipeline(const std::shared_ptr<vgl::Shader> &shader,
                        const std::shared_ptr<vgl::IVertexArray> &vertexArray,
                        const std::vector<vgl::TextureAttribute> &textureAttributes) : shader(shader),
                                                                                       vertexArray(vertexArray),
                                                                                       textureAttributes(
		                                                                                       textureAttributes) {}

void vgl::Pipeline::render(const UniformData &data) {
	shader->render(data, *vertexArray, textureAttributes);
}

void vgl::Pipeline::render() {
	shader->render(*vertexArray, textureAttributes);
}

std::shared_ptr<vgl::Pipeline> vgl::Pipeline::create(
		const std::shared_ptr<vgl::Shader> &shader,
		const std::shared_ptr<vgl::IVertexArray> &vertexArray,
		const std::vector<vgl::TextureAttribute> &textureAttributes) {
	return std::make_shared<vgl::Pipeline>(shader, vertexArray, textureAttributes);
}

void vgl::Pipeline::free() {
	// TODO: decide what do we need to free
	shader->free();
	vertexArray->free();
}

std::shared_ptr<vgl::Pipeline>
vgl::Pipeline::create(const std::string &shader_base_path,
					  const std::shared_ptr<vgl::IVertexArray>& vertexArray,
                      const std::map<std::string, std::shared_ptr<vgl::ITexture>>& textureMapping) {
	auto shader = vgl::Shader::createFromPath(shader_base_path+".vert", shader_base_path+".frag");
	std::vector<vgl::TextureAttribute> textureAttributes;
	for(auto const& pair: textureMapping){
		textureAttributes.emplace_back(
				shader->getAttribute(pair.first), pair.second
		);
	}
	return vgl::Pipeline::create(shader, vertexArray, textureAttributes);
}

void vgl::Pipeline::useShader() {
	shader->use();
}
