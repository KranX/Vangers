#include "vectormath.h"

using namespace renderer::vectormath;

Transform Transform::concat(const Transform& other)
{
	return {
		.position = position + rotation.transform(other.position * scale),
		.scale = scale * other.scale,
		.rotation = rotation * other.rotation
	};
}

Vector3 Quaternion::transform(const Vector3& v) const
{
	Vector3 u {x, y, z};
	Vector3 uv = u.cross(v);
	return v + ((uv * w) + u.cross(uv)) * ((float)2);
}
