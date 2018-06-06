//
// Created by nikita on 3/18/18.
//

#include "Camera.h"
#include <glm/gtc/matrix_transform.hpp>

glm::mat4 gl::Camera::mvp() {
	auto proj = glm::perspective(
			glm::radians(45.0f),
			viewport.x / viewport.y,
			100.0f, 9000.0f
	);

//	auto proj = glm::ortho(0.0f, viewport.x, 0.0f, viewport.y);

	auto view = glm::mat4_cast(glm::inverse(rotation)) * glm::translate(glm::mat4(), -position);
	return proj * view;
}

void gl::Camera::from_player(float x, float y, float z, float turn, float slope) {
	auto playerPos = glm::vec3(2048.0f - x, y, 0);

	auto turnRot = glm::angleAxis(-turn + glm::radians(180.0f), glm::vec3(0.0f, 0, 1.0f));
	auto slopeRot = glm::angleAxis(-slope, glm::vec3(1.0f, 0, 0));
	rotation = turnRot * slopeRot;

	position = playerPos + rotation * glm::vec3(0, 0, z);

}
