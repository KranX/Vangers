#ifndef MATH_H
#define MATH_H

#include <math.h>

namespace renderer::vectormath {
	struct Vector3 {
		float x;
		float y;
		float z;

		inline Vector3 operator+(const Vector3& other) const {
			return {
				.x = x + other.x,
				.y = y + other.y,
				.z = z + other.z,
			};
		}

		inline Vector3 operator-(const Vector3& other) const {
			return {
				.x = x - other.x,
				.y = y - other.y,
				.z = z - other.z,
			};
		}

		inline Vector3 operator*(float value) const {
			return {
				.x = value * x,
				.y = value * y,
				.z = value * z,
			};
		}


		inline Vector3 operator/(float value) const {
			Vector3 r = {
			    .x = x / value,
			    .y = y / value,
			    .z = z / value,
			};
			return r;
		}

		inline Vector3 cross(const Vector3& other) const {
			return {
				.x = (y * other.z) - (z * other.y),
				.y = (z * other.x) - (x * other.z),
				.z = (x * other.y) - (y * other.x),
			};
		}

		inline float dot(const Vector3& other) const {
			return x * other.x + y * other.y + z * other.z;
		}
	};

	struct Quaternion {
		float x;
		float y;
		float z;
		float w;

		inline friend Quaternion operator*(const Quaternion& q1, const Quaternion& q2){
			return {
				.x = (q1.x * q2.w) + (q1.y * q2.z) - (q1.z * q2.y) + (q1.w * q2.x),
				.y = (-q1.x * q2.z) + (q1.y * q2.w) + (q1.z * q2.x) + (q1.w * q2.y),
				.z = (q1.x * q2.y) - (q1.y * q2.x) + (q1.z * q2.w) + (q1.w * q2.z),
				.w = (-q1.x * q2.x) - (q1.y * q2.y) - (q1.z * q2.z) + (q1.w * q2.w),
			};
		}

		inline Quaternion operator*(float value) const {
			return {
				x * value,
				y * value,
				z * value,
				w * value,
			};
		}

		inline Quaternion operator/(float value) const {
			return {
				x / value,
				y / value,
				z / value,
				w / value,
			};
		}

		inline float length() const {
			return std::sqrt(x * x + y * y + z * z + w * w);
		}

		inline Quaternion normalized() const {
			return *this / length();
		}

		Vector3 transform(const Vector3& v) const;
	};

	struct Transform {
		Vector3 position;
		float scale;
		Quaternion rotation;

		Transform concat(const Transform& other);
	};
}
#endif // MATH_H
