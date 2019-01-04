//
// Created by nikita on 2018-12-09.
//

#include "uniform_ext.h"
#include "util.h"

template <>
void vgl::Uniform<glm::vec4>::assign(GLint attrId, const glm::vec4& data){
	glUniform4fv(attrId, 1, &data[0]);
	vgl::checkErrorAndThrow("glUniform4fv");
}

template <>
void vgl::Uniform<glm::vec3>::assign(GLint attrId, const glm::vec3& data){
	glUniform3fv(attrId, 1, &data[0]);
	vgl::checkErrorAndThrow("glUniform3fv");
}


template <>
void vgl::Uniform<glm::vec2>::assign(GLint attrId, const glm::vec2& data){
	glUniform2fv(attrId, 1, &data[0]);
	vgl::checkErrorAndThrow("glUniform2fv");
}

template <>
void vgl::Uniform<float>::assign(GLint attrId, const float& data){
	glUniform1fv(attrId, 1, &data);
	vgl::checkErrorAndThrow("glUniform1fv");
}

template <>
void vgl::Uniform<glm::mat4>::assign(GLint attrId, const glm::mat4& data){
	glUniformMatrix4fv(attrId, 1, GL_FALSE, &data[0][0]);
	vgl::checkErrorAndThrow("glUniformMatrix4fv");
}