//
// Created by nikita on 3/18/18.
//

#ifndef VANGERS_CAMERA_H
#define VANGERS_CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <memory>

namespace gl{
	class Camera {
	public:
		glm::vec2 viewport;
		glm::vec3 position;
		glm::quat rotation;
		float focus;

		void from_player(float x, float y, float z, float turn, float slope);
		glm::mat4 mvp();

	};
}


#endif //VANGERS_CAMERA_H
