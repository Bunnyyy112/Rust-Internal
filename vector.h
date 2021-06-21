#pragma once
#include <algorithm>
#include <cmath>

class Vector2
{
public:
	Vector2()
	{
		x = y = 0.f;
	}

	Vector2(float fx, float fy)
	{
		x = fx;
		y = fy;
	}

	float x, y;

	Vector2 operator+(const Vector2& input) const
	{
		return Vector2{ x + input.x, y + input.y };
	}

	Vector2 operator-(const Vector2& input) const
	{
		return Vector2{ x - input.x, y - input.y };
	}

	Vector2 operator/(float input) const
	{
		return Vector2{ x / input, y / input };
	}

	Vector2 operator*(float input) const
	{
		return Vector2{ x * input, y * input };
	}

	Vector2& operator-=(const Vector2& v)
	{
		x -= v.x;
		y -= v.y;
		return *this;
	}

	Vector2& operator/=(float input)
	{
		x /= input;
		y /= input;
		return *this;
	}

	Vector2& operator*=(float input)
	{
		x *= input;
		y *= input;
		return *this;
	}

	float length() const
	{
		return std::sqrt((x * x) + (y * y));
	}

	Vector2 normalized() const
	{
		return { x / length(), y / length() };
	}

	float dot_product(Vector2 input) const
	{
		return (x * input.x) + (y * input.y);
	}

	float distance(Vector2 input) const
	{
		return (*this - input).length();
	}

	bool empty() const
	{
		return x == 0.f && y == 0.f;
	}
};

class Vector3
{
public:
	Vector3()
	{
		x = y = z = 0.f;
	}

	Vector3(float fx, float fy, float fz)
	{
		x = fx;
		y = fy;
		z = fz;
	}

	float x, y, z;

	Vector3 operator+(const Vector3& input) const
	{
		return Vector3{ x + input.x, y + input.y, z + input.z };
	}

	Vector3 operator-(const Vector3& input) const
	{
		return Vector3{ x - input.x, y - input.y, z - input.z };
	}

	Vector3 operator/(float input) const
	{
		return Vector3{ x / input, y / input, z / input };
	}

	Vector3 operator*(float input) const
	{
		return Vector3{ x * input, y * input, z * input };
	}

	Vector3& operator-=(const Vector3& v)
	{
		x -= v.x;
		y -= v.y;
		z -= v.z;

		return *this;
	}

	Vector3& operator+=(const Vector3& v)
	{
		x += v.x;
		y += v.y;
		z += v.z;
		return *this;
	}

	Vector3& operator/=(float input)
	{
		x /= input;
		y /= input;
		z /= input;
		return *this;
	}

	Vector3& operator*=(float input)
	{
		x *= input;
		y *= input;
		z *= input;
		return *this;
	}

	bool operator==(const Vector3& input) const
	{
		return x == input.x && y == input.y && z == input.z;
	}

	void make_absolute()
	{
		x = std::abs(x);
		y = std::abs(y);
		z = std::abs(z);
	}

	float length_sqr() const
	{
		return (x * x) + (y * y) + (z * z);
	}

	float length() const
	{
		return std::sqrt(length_sqr());
	}

	float length_2d() const
	{
		return std::sqrt((x * x) + (y * y));
	}

	Vector3 normalized() const
	{
		return { x / length(), y / length(), z / length() };
	}

	Vector3 midPoint(Vector3 v2)
	{
		return Vector3((x + v2.x) / 2, (y + v2.y) / 2, (z + v2.z) / 2);
	}

	float dot_product(Vector3 input) const
	{
		return (x * input.x) + (y * input.y) + (z * input.z);
	}

	float distance(Vector3 input) const
	{
		return (*this - input).length();
	}

	float distance_2d(Vector3 input) const
	{
		return (*this - input).length_2d();
	}

	void clamp()
	{
		std::clamp(x, -89.f, 89.f);
		std::clamp(y, -180.f, 180.f);

		z = 0.f;
	}

	bool empty() const
	{
		return x == 0.f && y == 0.f && z == 0.f;
	}
};

class Matrix
{
public:
	inline float* operator[](int i)
	{
		return m[i];
	}

	inline const float* operator[](int i) const
	{
		return m[i];
	}

	inline float* Base()
	{
		return &m[0][0];
	}

	inline const float* Base() const
	{
		return &m[0][0];
	}
public:

	inline Matrix()
	{
	}

	inline Matrix(
		float m00, float m01, float m02, float m03,
		float m10, float m11, float m12, float m13,
		float m20, float m21, float m22, float m23,
		float m30, float m31, float m32, float m33)
	{
		Init(
			m00, m01, m02, m03,
			m10, m11, m12, m13,
			m20, m21, m22, m23,
			m30, m31, m32, m33
		);
	}

	inline void Init(
		float m00, float m01, float m02, float m03,
		float m10, float m11, float m12, float m13,
		float m20, float m21, float m22, float m23,
		float m30, float m31, float m32, float m33
	)
	{
		m[0][0] = m00;
		m[0][1] = m01;
		m[0][2] = m02;
		m[0][3] = m03;

		m[1][0] = m10;
		m[1][1] = m11;
		m[1][2] = m12;
		m[1][3] = m13;

		m[2][0] = m20;
		m[2][1] = m21;
		m[2][2] = m22;
		m[2][3] = m23;

		m[3][0] = m30;
		m[3][1] = m31;
		m[3][2] = m32;
		m[3][3] = m33;
	}

	Matrix transpose() const
	{
		return Matrix(
			m[0][0], m[1][0], m[2][0], m[3][0],
			m[0][1], m[1][1], m[2][1], m[3][1],
			m[0][2], m[1][2], m[2][2], m[3][2],
			m[0][3], m[1][3], m[2][3], m[3][3]);
	}

	float m[4][4];
};

using vec2_t = Vector2;
using vec3_t = Vector3;
using mat4x4_t = Matrix;
