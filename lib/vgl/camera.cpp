//
// Created by nikita on 3/18/18.
//

#include "camera.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/ext.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>
#include <iostream>


glm::mat4 vgl::Camera::mvp() const {
	float FOV = atan(viewport.y / 2 / focus) * 2;
	auto proj = glm::perspective(
			FOV,
			viewport.x / viewport.y,
			100.0f, 9000.0f
	);
	auto view = glm::mat4_cast(glm::inverse(rotation)) * glm::translate(glm::mat4(1.0f), -position);
	return proj * view;
}

void vgl::Camera::from_player(float x, float y, float z, float turn, float slope) {
	auto playerPos = glm::vec3(2048.0f - x, y, 0);

	auto turnRot = glm::angleAxis(-turn + glm::radians(180.0f), glm::vec3(0.0f, 0, 1.0f));
	auto slopeRot = glm::angleAxis(-slope, glm::vec3(1.0f, 0, 0));
	rotation = turnRot * slopeRot;

	position = playerPos + rotation * glm::vec3(0, 0, z);

}
