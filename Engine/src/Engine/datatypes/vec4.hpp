#pragma once

#include "types.hpp"
#include "vec3.hpp"

//for homogenuous representation
struct vec4
{
	real x, y, z, w;
	vec4() { x = 0.0; y = 0.0; z = 0.0; w = 0.0; }
	vec4(const real inX, const real inY, const real inZ, const real inW) { x = inX; y = inY; z = inZ; w = inW; }//should almost never be used
	vec4(const dir& dir) { x = dir.m_data.x; y = dir.m_data.y; z = dir.m_data.z; w = 0.0; }//initialize with vector(direction) -> set w to 0
	vec4(const point& point) { x = point.m_data.x; y = point.m_data.y; z = point.m_data.z; w = 1.0; }
	//operators
	inline const real* data() const { return &x; }//return pointer to data
	inline const real operator[](const uint8_t index) const { _ASSERT(index < 4); return (real) * (&x + index); }
	std::string toString() const //only for debug purposes
	{
		std::string out;
		out.append(std::to_string(x)); out.push_back('|');
		out.append(std::to_string(y)); out.push_back('|');
		out.append(std::to_string(z)); out.push_back('|');
		out.append(std::to_string(w)); out.push_back('\n');
		return out;
	}
};

inline std::ostream& operator<<(std::ostream& os, const vec4& vec)//this just exists so we can log easier
{
	return os << vec.toString();
}