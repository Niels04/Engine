#pragma once

#include "types.hpp"
#include "vec3.hpp"

//for homogenuous representation
struct vec4
{
	real x, y, z, w;
	vec4() { x = 0.0; y = 0.0; z = 0.0; w = 0.0; }
	vec4(const real inX, const real inY, const real inZ, const real inW) { x = inX; y = inY; z = inZ; w = inW; }//should almost never be used
	vec4(const vec3& vec, const real inW) { x = vec.x; y = vec.y; z = vec.z; w = inW; }
	vec4(const dir& dir) { x = dir.m_data.x; y = dir.m_data.y; z = dir.m_data.z; w = 0.0; }//initialize with vector(direction) -> set w to 0
	vec4(const point& point) { x = point.m_data.x; y = point.m_data.y; z = point.m_data.z; w = 1.0; }
	//operators
	inline const real* data() const { return &x; }//return pointer to data
	inline const real operator[](const uint8_t index) const { _ASSERT(index < 4); return (real) * (&x + index); }
	inline vec4 operator -() const { return { -x, -y, -z, -w }; }
	inline vec4 operator*(const float factor) const { return {x * factor, y * factor, z * factor, w * factor}; }
	inline void operator -=(const vec4& other) { x -= other.x; y -= other.y; z -= other.z; w -= other.w; }
	inline void operator +=(const vec4& other) { x += other.x; y += other.y; z += other.z; w += other.w; }
	std::string toString() const //only for debug purposes
	{
		std::string out;
		out.append(std::to_string(x)); out.push_back('|');
		out.append(std::to_string(y)); out.push_back('|');
		out.append(std::to_string(z)); out.push_back('|');
		out.append(std::to_string(w)); out.push_back('\n');
		return out;
	}
	inline vec3 xyz() const { return { x, y, z }; }
	//mathematical operation
	static vec4 cross(const vec4& a, const vec4& b)
	{
		return { a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x, 0.0f };
	}
	static vec4 normalized(const vec4& vec) { float len = sqrtf(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z); return { vec.x / len, vec.y / len, vec.z / len, 0.0f }; }
};

inline std::ostream& operator<<(std::ostream& os, const vec4& vec)//this just exists so we can log easier
{
	return os << vec.toString();
}