#pragma once

#include "types.hpp"

struct vec3
{
	real x, y, z;

	inline vec3( real inX, real inY, real inZ) { x = inX; y = inY; z = inZ; }
	inline vec3() : x(0.0), y(0.0), z(0.0) {  }
	inline const real operator[](const uint8_t index) const { _ASSERT(index < 3); return (real) * (&x + index); }//REMEMBER:::::WHEN ADDING TO POINTERS C++ AUTOMATICALLY MULTIPLIES BY THE SIZE OF THE OBJECT AND ADDS THAT SO DON'T DO IT MANUALLY OR USE CHAR-POINTERS
	std::string toString() const //only for debug purposes
	{
		std::string out;
		out.append(std::to_string(x)); out.push_back('|');
		out.append(std::to_string(y)); out.push_back('|');
		out.append(std::to_string(z)); out.push_back('\n');
		return out;
	}
	inline vec3 operator -() const { return {-x, -y, -z}; }
	inline vec3 operator -(const vec3& other) const { return { x - other.x, y - other.y, z - other.z }; }
	inline vec3 operator *(const float factor) const { return {x * factor, y * factor, z * factor}; }
};

inline std::ostream& operator<<(std::ostream& os, const vec3& vec)//this just exists so we can log easier
{
	return os << vec.toString();
}

//represents a vec3 that is a direction
struct dir
{
	vec3 m_data;
	inline dir(const real x, const real y, const real z) : m_data({ x, y, z }) {  }
	//operators
	inline const real operator[](const uint8_t index) const { return m_data[index]; }
	inline void isAdd(const dir& dir) { m_data.x += dir.m_data.x; m_data.y += dir.m_data.x; m_data.z += dir.m_data.z; }
	inline dir add(const dir& dir) const { return { m_data.x + dir.m_data.x, m_data.y + dir.m_data.y, m_data.z + dir.m_data.z }; }
	inline void isSub(const dir& dir) { m_data.x -= dir.m_data.x; m_data.y -= dir.m_data.y; m_data.z -= dir.m_data.z; }
	inline dir sub(const dir& dir) const { return { m_data.x - dir.m_data.x, m_data.y - dir.m_data.y, m_data.z - dir.m_data.z }; }
	inline void isMulScalar(const real scalar) { m_data.x *= scalar; m_data.y *= scalar; m_data.z *= scalar; }
	inline dir mulScalar(const real scalar) const { return { m_data.x * scalar, m_data.y * scalar, m_data.z * scalar }; }
	inline void isdiv(const real scalar) { m_data.x /= scalar; m_data.y /= scalar; m_data.z /= scalar; }
	inline dir div(const real scalar) const { return { m_data.x / scalar, m_data.y / scalar, m_data.z / scalar }; }
	inline real dot(const dir& dir) const { return (m_data.x * dir.m_data.x) + (m_data.y * dir.m_data.y) + (m_data.z * dir.m_data.z); }
	inline dir cross(const dir& dir) const { return{ (m_data.y * dir.m_data.z) - (m_data.z * dir.m_data.y), (m_data.z * dir.m_data.x) - (m_data.x * dir.m_data.z), (m_data.x * dir.m_data.y) - (m_data.y * dir.m_data.x) }; }
	inline real length() const { return std::sqrtf((m_data.x * m_data.x) + (m_data.y * m_data.y) + (m_data.z * m_data.z)); }
	inline real squaredLength() const { return dot(*this); }//squared length is just the dotProduct with itself
	inline void normalize() { isdiv(length()); }
	inline dir normalized() const { return div(length()); }
	inline real angleBetween(const dir& dir) const { return dot(dir) / (length() * dir.length()); }//returns the cosine
	inline real angleBetweenNorm(const dir& dir)/*other vector is normalized*/ const { return dot(dir) / length(); }//call this if ONLY the vector that is parameter is normalized //returns the cosine
	inline void operator+=(const dir& dir) { isAdd(dir); }
	inline dir operator+(const dir& dir) const { return add(dir); }
	inline void operator-=(const dir& dir) { isSub(dir); }
	inline dir operator-(const dir& dir) const { return sub(dir); }
	inline void operator*=(const real scalar) { isMulScalar(scalar); }
	inline dir operator*(const real scalar) const { return mulScalar(scalar); }
	inline real operator*(const dir& dir) const { return dot(dir); }
	inline void operator/=(const real scalar) { isdiv(scalar); }
	inline dir operator/(const real scalar) const { return div(scalar); }
	std::string toString() const //only for debug purposes
	{
		return m_data.toString();
	}
};

inline std::ostream& operator<<(std::ostream& os, const dir& dir)//this just exists so we can log easier
{
	return os << dir.toString();
}

//represents a vec3 that is a point
struct point
{
	vec3 m_data;
	inline point(const real x, const real y, const real z) : m_data({ x, y, z }) {  }
	inline point() : m_data({0.0, 0.0, 0.0}) {  }
	//operators
	inline const real operator[](const uint8_t index) const { return m_data[index]; }
	inline const point operator-() const { return point(-m_data.x, -m_data.y, -m_data.z); }
	inline void operator=(const vec3& vec) { m_data = vec; }
	inline void isAdd(const dir& dir) { m_data.x += dir.m_data.x; m_data.y += dir.m_data.x; m_data.z += dir.m_data.z; }
	inline dir add(const dir& dir) { return { m_data.x + dir.m_data.x, m_data.y + dir.m_data.y, m_data.z + dir.m_data.z }; }
	inline void isSub(const dir& dir) { m_data.x -= dir.m_data.x; m_data.y -= dir.m_data.y; m_data.z -= dir.m_data.z; }
	inline dir sub(const dir& dir) { return { m_data.x - dir.m_data.x, m_data.y - dir.m_data.y, m_data.z - dir.m_data.z }; }
	std::string toString() const //only for debug purposes
	{
		return m_data.toString();
	}
};

inline std::ostream& operator<<(std::ostream& os, const point& point)//this just exists so we can log easier
{
	return os << point.toString();
}