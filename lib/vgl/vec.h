//
// Created by nikita on 2018-12-04.
//

#ifndef VGL_VEC_H
#define VGL_VEC_H

#include <ostream>

namespace vgl {
	struct iVec1 {
		int32_t x;

		static const int len = 1;

		static const iVec1 zero;
	};

	std::ostream &operator<<(std::ostream &os, const iVec1 &vec1);


	struct iVec2 {
		int32_t x, y;

		static const int len = 2;

		static const iVec2 zero;
	};

	std::ostream &operator<<(std::ostream &os, const iVec2 &vec2);


	struct iVec3 {
		int32_t x, y, z;

		static const int len = 3;

		static const iVec3 zero;
	};

	std::ostream &operator<<(std::ostream &os, const iVec3 &vec3);

}


#endif //VGL_VEC_H
