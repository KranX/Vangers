//
// Created by nikita on 2018-12-03.
//
#include "shader.h"
#include "util.h"

void vgl::Shader::bindUniformAttribs(UniformData &data) {
	for(auto* member: data.members){
		member->attrId = getAttribute(member->name);
	}
}

std::shared_ptr<vgl::Shader> vgl::Shader::create(const std::string &vertexShaderCode,
                              const std::string &fragmentShaderCode) {
	auto objectId = vgl::loadShadersStr(vertexShaderCode, fragmentShaderCode);
	return std::make_shared<vgl::Shader>(objectId);
}


GLint vgl::Shader::getAttribute(const std::string &name) const{
	auto res = glGetUniformLocation(objectId, name.c_str());
	vgl::checkErrorAndThrow("glGetUniformLocation");
	return res;
}

void vgl::Shader::addTexture(vgl::ITexture& texture, const std::string &name) {
	auto textureAttribute = getAttribute(name);
	textureAttributes.emplace_back(TextureAttribute(textureAttribute, texture));
}

std::shared_ptr<vgl::Shader>
vgl::Shader::createFromPath(const std::string &vertexShaderPath, const std::string &fragmentShaderPath) {
	auto objectId = vgl::loadShaders(vertexShaderPath, fragmentShaderPath);
	return std::make_shared<vgl::Shader>(objectId);
}
