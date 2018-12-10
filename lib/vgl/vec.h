//
// Created by nikita on 2018-12-04.
//

#ifndef UPLOADBENCH_VEC_H
#define UPLOADBENCH_VEC_H

#include <ostream>

namespace vgl {
	struct uiVec2 {

		union {
			struct {
				unsigned int width, height;
			};

			struct {
				unsigned int x, y;
			};

		};
	};

	std::ostream &operator<<(std::ostream &os, const uiVec2 &vec2) {
		os << "uiVec2(" << vec2.width << ", " << vec2.height<<")";
		return os;
	}


	struct uiVec3 {
		struct {
			unsigned int width, height, depth;
		};

		struct {
			unsigned int x, y, z;
		};
	};

	std::ostream &operator<<(std::ostream &os, const uiVec3 &vec3) {
		os << "uiVec3(" << vec3.width << ", " << vec3.height<<", "<<vec3.depth<<")";
		return os;
	}

}


#endif //UPLOADBENCH_VEC_H
