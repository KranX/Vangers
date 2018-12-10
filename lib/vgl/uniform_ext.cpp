//
// Created by nikita on 2018-12-09.
//

#include "uniform_ext.h"

template <>
void vgl::Uniform<glm::vec4>::_assign(GLint attrId, const glm::vec4& data){
	glUniform4fv(attrId, 1, &data[0]);
}

template <>
void vgl::Uniform<glm::vec3>::_assign(GLint attrId, const glm::vec3& data){
	glUniform3fv(attrId, 1, &data[0]);
}


template <>
void vgl::Uniform<glm::vec2>::_assign(GLint attrId, const glm::vec2& data){
	glUniform2fv(attrId, 1, &data[0]);
}

template <>
void vgl::Uniform<glm::mat4>::_assign(GLint attrId, const glm::mat4& data){
	glUniformMatrix4fv(attrId, 1, GL_FALSE, &data[0][0]);
}