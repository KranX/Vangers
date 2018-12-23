//
// Created by nikita on 2018-12-23.
//

#include "vec.h"

const vgl::iVec1 vgl::iVec1::zero = {0};

const vgl::iVec2 vgl::iVec2::zero = {0,0};

const vgl::iVec3 vgl::iVec3::zero = {0, 0, 0};


std::ostream &vgl::operator<<(std::ostream &os, const vgl::iVec1 &vec1) {
	os << "iVec1(" << vec1.x << ")";
	return os;
}

std::ostream &vgl::operator<<(std::ostream &os, const vgl::iVec2 &vec2) {
	os << "iVec2(" << vec2.x << ", " << vec2.y<<")";
	return os;
}

std::ostream &vgl::operator<<(std::ostream &os, const vgl::iVec3 &vec3) {
	os << "iVec3(" << vec3.x << ", " << vec3.y<<", "<<vec3.z<<")";
	return os;
}


