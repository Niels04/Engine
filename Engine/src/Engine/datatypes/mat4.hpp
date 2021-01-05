#pragma once

#include "types.hpp"
#include "vec4.hpp"

//a 4x4 matrix
struct mat4
{
	real mat[4][4];//first is rows, second is columns
	mat4()
	{ 
		//initialize to 0
		for (uint8_t i = 0; i < 4; i++)
		{
			std::fill_n(mat[i], 4, 0.0);
		}
		//and then to identity
		mat[0][0] = 1.0;
		mat[1][1] = 1.0;
		mat[2][2] = 1.0;
		mat[3][3] = 1.0;
	}
	void transpose()
	{
		mat4 result;
		for (uint8_t i = 0; i < 4; i++)
		{
			for (uint8_t j = 0; j < 4; j++)
			{
				result[i][j] = mat[j][i];
			}
		}
		*this = result;
	}
	static mat4 transposed(const mat4& mat)
	{
		mat4 result;
		for (uint8_t i = 0; i < 4; i++)
		{
			for (uint8_t j = 0; j < 4; j++)
			{
				result[i][j] = mat.mat[j][i];
			}
		}
		return result;
	}
	//operators
	inline real* operator[](const uint8_t index) { _ASSERT(index < 4); return mat[index]; }
	mat4 multip(const mat4& rhs) const
	{
		mat4 out;
		for (uint8_t i = 0; i < 4; i++)
		{
			for (uint8_t j = 0; j < 4; j++)
			{
				out[i][j] = (mat[i][0] * rhs.mat[0][j]) + (mat[i][1] * rhs.mat[1][j]) + (mat[i][2] * rhs.mat[2][j]) + (mat[i][3] * rhs.mat[3][j]);//dotProduct of corresponding row in this and corresponding column in rhs
			}
		}
		return out;
	}
	vec4 multip(const vec4& rhs) const
	{
		vec4 out;
		out.x = (mat[0][0] * rhs[0]) + (mat[0][1] * rhs[1]) + (mat[0][2] * rhs[2]) + (mat[0][3] * rhs[3]);
		out.y = (mat[1][0] * rhs[0]) + (mat[1][1] * rhs[1]) + (mat[1][2] * rhs[2]) + (mat[1][3] * rhs[3]);
		out.z = (mat[2][0] * rhs[0]) + (mat[2][1] * rhs[1]) + (mat[2][2] * rhs[2]) + (mat[2][3] * rhs[3]);
		out.w = (mat[3][0] * rhs[0]) + (mat[3][1] * rhs[1]) + (mat[3][2] * rhs[2]) + (mat[3][3] * rhs[3]);
		return out;
	}
	mat4 isMultip(const mat4& rhs)//multiply with another matrix and assign result to this
	{
		*this = multip(rhs);
	}
	inline mat4 operator*(const mat4& rhs) const { return multip(rhs); }
	inline vec4 operator*(const vec4& rhs) const { return multip(rhs); }
	inline mat4 operator*=(const mat4& rhs) { isMultip(rhs); }

	//build matrices for different actions
	void setTransMat(const vec3& trans)
	{
		mat[0][0] = 1.0;
		mat[0][3] = trans.x;
		mat[1][1] = 1.0;
		mat[1][3] = trans.y;
		mat[2][2] = 1.0;
		mat[2][3] = trans.z;
		mat[3][3] = 1.0;
	}
	static mat4 transMat(const vec3& trans)
	{
		mat4 out;
		out.mat[0][0] = 1.0;
		out.mat[0][3] = trans.x;
		out.mat[1][1] = 1.0;
		out.mat[1][3] = trans.y;
		out.mat[2][2] = 1.0;
		out.mat[2][3] = trans.z;
		out.mat[3][3] = 1.0;
		return out;
	}
	static mat4 transMat(const point& trans)
	{
		mat4 out;
		out.mat[0][0] = 1.0;
		out.mat[0][3] = trans.m_data.x;
		out.mat[1][1] = 1.0;
		out.mat[1][3] = trans.m_data.y;
		out.mat[2][2] = 1.0;
		out.mat[2][3] = trans.m_data.z;
		out.mat[3][3] = 1.0;
		return out;
	}
	void setTransMat(const real transX, const real transY, const real transZ)
	{
		mat[0][0] = 1.0;
		mat[0][3] = transX;
		mat[1][1] = 1.0;
		mat[1][3] = transY;
		mat[2][2] = 1.0;
		mat[2][3] = transZ;
		mat[3][3] = 1.0;
	}
	static mat4 transMat(const real transX, const real transY, const real transZ)
	{
		mat4 out;
		out.mat[0][0] = 1.0;
		out.mat[0][3] = transX;
		out.mat[1][1] = 1.0;
		out.mat[1][3] = transY;
		out.mat[2][2] = 1.0;
		out.mat[2][3] = transZ;
		out.mat[3][3] = 1.0;
		return out;
	}
	void setScaleMat(const vec3& factor)
	{
		mat[0][0] = factor.x;
		mat[1][1] = factor.y;
		mat[2][2] = factor.z;
		mat[3][3] = 1.0;
	}
	void setScaleMat(const real factorX, const real factorY, const real factorZ)
	{
		mat[0][0] = factorX;
		mat[1][1] = factorY;
		mat[2][2] = factorZ;
		mat[3][3] = 1.0;
	}
	void setScaleMat(const real factorAll)//use, if all axis are to be scaled by the same factor
	{
		mat[0][0] = factorAll;
		mat[1][1] = factorAll;
		mat[2][2] = factorAll;
		mat[3][3] = 1.0;
	}
	static mat4 Scale(const vec3& factor)
	{
		mat4 out;
		out.mat[0][0] = factor.x;
		out.mat[1][1] = factor.y;
		out.mat[2][2] = factor.z;
		out.mat[3][3] = 1.0;

		return out;
	}
	static mat4 Scale(const real factorX, const real factorY, const real factorZ)
	{
		mat4 out;
		out.mat[0][0] = factorX;
		out.mat[1][1] = factorY;
		out.mat[2][2] = factorZ;
		out.mat[3][3] = 1.0;
		return out;
	}
	static mat4 Scale(const real factorAll)
	{
		mat4 out;
		out.mat[0][0] = factorAll;
		out.mat[1][1] = factorAll;
		out.mat[2][2] = factorAll;
		out.mat[3][3] = 1.0;
		return out;
	}
	enum class reflectType : uint8_t
	{
		yz = 0, xz = 1, xy = 2, x = 3, y = 4, z = 5, o = 6
	};
	void setReflectMat(reflectType type)
	{
		switch (type)
		{
		case(reflectType::yz)://reflect around the yz-plane
		{
			mat[0][0] = -1.0;
			mat[1][1] = 1.0;
			mat[2][2] = 1.0;
			mat[3][3] = 1.0;
		}break;
		case(reflectType::xz)://reflect around the xz-plane
		{
			mat[0][0] = 1.0;
			mat[1][1] = -1.0;
			mat[2][2] = 1.0;
			mat[3][3] = 1.0;
		}break;
		case(reflectType::xy)://reflect around the xy-plane
		{
			mat[0][0] = 1.0;
			mat[1][1] = 1.0;
			mat[2][2] = -1.0;
			mat[3][3] = 1.0;
		}break;
		case(reflectType::x)://reflect around the xz and the xy-plane
		{
			mat[0][0] = 1.0;
			mat[1][1] = -1.0;
			mat[2][2] = -1.0;
			mat[3][3] = 1.0;
		}break;
		case(reflectType::y)://reflect around the xy and the yz-plane
		{
			mat[0][0] = -1.0;
			mat[1][1] = 1.0;
			mat[2][2] = -1.0;
			mat[3][3] = 1.0;
		}break;
		case(reflectType::z)://reflect around the xz and the yz-plane
		{
			mat[0][0] = -1.0;
			mat[1][1] = -1.0;
			mat[2][2] = 1.0;
			mat[3][3] = 1.0;
		}break;
		case(reflectType::o)://reflect around all planes, because we reflect around the origin
		{
			mat[0][0] = -1.0;
			mat[1][1] = -1.0;
			mat[2][2] = -1.0;
			mat[3][3] = 1.0;
		}break;
		default:
		{
			std::cout << "Error occured when creating reflection matrix. \"reflectType\" was none of the presets.\n";
			_ASSERT(false);
		}break;
		}
	}
	void setShearMat(real PCxy, real PCxz, real PCyx, real PCyz, real PCzx, real PCzy)//PC stands for proportionality constant
	{
		mat[0][0] = 1.0; mat[0][1] = PCxy; mat[0][2] = PCxz;
		mat[1][0] = PCyx; mat[1][1] = 1.0; mat[1][2] = PCyz;
		mat[2][0] = PCzx; mat[2][1] = PCzy; mat[2][2] = 1.0;
		mat[3][3] = 1.0;
	}
	void setRotx(real angle/*input in radians*/)//rotation around the x-axis
	{
		mat[0][0] = 1.0;
#ifdef DOUBLE
		mat[1][1] = std::cos(angle);
		mat[1][2] = std::sin(-angle);
#else
		mat[1][1] = std::cosf(angle);
		mat[1][2] = std::sinf(-angle);
#endif
		mat[2][1] = -mat[1][2];//avoid calling trigonometric functions again, since the need time to calculate
		mat[2][2] = mat[1][1];//same here
		mat[3][3] = 1.0;
	}
	static mat4 Rotx(real angle)
	{
		mat4 out;
		out.mat[0][0] = 1.0;
#ifdef DOUBLE
		out.mat[1][1] = std::cos(angle);
		out.mat[1][2] = std::sin(-angle);
#else
		out.mat[1][1] = std::cosf(angle);
		out.mat[1][2] = std::sinf(-angle);
#endif
		out.mat[2][1] = -out.mat[1][2];//avoid calling trigonometric functions again, since the need time to calculate
		out.mat[2][2] = out.mat[1][1];//same here
		out.mat[3][3] = 1.0;
		return out;
	}
	void setRoty(real angle/*input in radians*/)//rotation around the y-axis
	{
#ifdef DOUBLE
		mat[0][0] = std::cos(angle);
		mat[0][2] = std::sin(angle);
#else
		mat[0][0] = std::cosf(angle);
		mat[0][2] = std::sinf(angle);
#endif
		mat[1][1] = 1.0;
		mat[2][0] = -mat[0][2];//avoid calling trigonometric functions again, since the need time to calculate
		mat[2][2] = mat[0][0];//same here
		mat[3][3] = 1.0;
	}
	static mat4 Roty(real angle)
	{
		mat4 out;
#ifdef DOUBLE
		out.mat[0][0] = std::cos(angle);
		out.mat[0][2] = std::sin(angle);
#else
		out.mat[0][0] = std::cosf(angle);
		out.mat[0][2] = std::sinf(angle);
#endif
		out.mat[1][1] = 1.0;
		out.mat[2][0] = -out.mat[0][2];//avoid calling trigonometric functions again, since the need time to calculate
		out.mat[2][2] = out.mat[0][0];//same here
		out.mat[3][3] = 1.0;
		return out;
	}
	void setRotz(real angle/*input in radians*/)//rotation around the z-axis
	{
#ifdef DOUBLE
		mat[0][0] = std::cos(angle);
		mat[0][1] = std::sin(-angle);
#else
		mat[0][0] = std::cosf(angle);
		mat[0][1] = std::sinf(-angle);
#endif
		mat[1][0] = -mat[0][1];//avoid calling trigonometric functions again, since the need time to calculate
		mat[1][1] = mat[0][0];//same here
		mat[2][2] = 1.0;
		mat[3][3] = 1.0;
	}
	static mat4 Rotz(real angle)
	{
		mat4 out;
#ifdef DOUBLE
		out.mat[0][0] = std::cos(angle);
		out.mat[0][1] = std::sin(-angle);
#else
		out.mat[0][0] = std::cosf(angle);
		out.mat[0][1] = std::sinf(-angle);
#endif
		out.mat[1][0] = -out.mat[0][1];//avoid calling trigonometric functions again, since the need time to calculate
		out.mat[1][1] = out.mat[0][0];//same here
		out.mat[2][2] = 1.0;
		out.mat[3][3] = 1.0;
		return out;
	}
	static mat4 projMat(real zNear, real zFar, real hFov, real vFov)
	{
		mat4 out;
#ifdef DOUBLE
		float t = tan((vFov * 0.5) * (3.141592653f / 180.0f)) * zNear;
		float b = -t;
		float r = tan((hFov * 0.5) * (3.141592653f / 180.0f)) * zNear;
		float l = -r;
#else
		float t = tanf((vFov * 0.5f) * (3.141592653f / 180.0f)) * zNear;
		float b = -t;
		float r = tanf((hFov * 0.5f) * (3.141592653f / 180.0f)) * zNear;
		float l = -r;
#endif

		out.mat[0][0] = (2.0f * zNear) / (r - l);
		out.mat[2][0] = (r + l) / (r - l);
		out.mat[1][1] = (2.0f * zNear) / (t - b);
		out.mat[2][1] = (t + b) / (t - b);
		out.mat[2][2] = (zFar + zNear) / (zFar - zNear);
		out.mat[3][2] = ((-2.0f) * zFar * zNear) / (zFar - zNear);
		out.mat[2][3] = -1.0f;
		return out;
	}
	void setProjMat(real Znear, real Zfar, real hfov, real vfov)//set the openGL projection-Matrix
	{
#ifdef DOUBLE
		float t = tan((vfov * 0.5) * (3.141592653f / 180.0f)) * Znear;
		float b = -t;
		float r = tan((hfov * 0.5) * (3.141592653f / 180.0f)) * Znear;
		float l = -r;
#else
		float t = tanf((vfov * 0.5f) * (3.141592653f / 180.0f)) * Znear;
		float b = -t;
		float r = tanf((hfov * 0.5f) * (3.141592653f / 180.0f)) * Znear;
		float l = -r;
#endif

		mat[0][0] = (2.0f * Znear) / (r - l);
		mat[2][0] = (r + l) / (r - l);
		mat[1][1] = (2.0f * Znear) / (t - b);
		mat[2][1] = (t + b) / (t - b);
		mat[2][2] = -(Zfar + Znear) / (Zfar - Znear);
		mat[3][2] = ((-2.0f) * Zfar * Znear) / (Zfar - Znear);
		mat[2][3] = -1.0f;
	}
	void setProjMatOrtho(real left, real right, real bottom, real top, real Znear, real Zfar)//set an orthographic projection matrix
	{
#ifdef DOUBLE
		mat[0][0] = 2.0 / (right - left);
		mat[0][3] = -((right + left) / (right - left));
		mat[1][1] = 2.0 / (top - bottom);
		mat[1][3] = -((top + bottom) / (top - bottom));
		mat[2][2] = 2.0 / (Zfar - Znear);
		mat[2][3] = -((Znear + Zfar) / (Zfar - Znear));
		mat[3][3] = 1.0;
#else
		mat[0][0] = 2.0f / (right - left);
		mat[0][3] = -((right + left) / (right - left));
		mat[1][1] = 2.0f / (top - bottom);
		mat[1][3] = -((top + bottom) / (top - bottom));
		mat[2][2] = 2.0f / (Zfar - Znear);
		mat[2][3] = -((Znear + Zfar) / (Zfar - Znear));
		mat[3][3] = 1.0f;
#endif
	}
	void Identity()
	{
		for (uint8_t i = 0; i < 4; i++)
		{
			std::fill_n(mat[i], 4, 0.0);
		}

		mat[0][0] = 1.0;
		mat[1][1] = 1.0;
		mat[2][2] = 1.0;
		mat[3][3] = 1.0;
	}
	static mat4 identity()
	{
		return mat4();//just return a new matrix, that gets initialized to identity
	}
	std::string toString() const//only for debug puroposes, outputs the matrix as a string
	{
		std::string out("\n");
		for (uint8_t i = 0; i < 4; i++)
		{
			for (uint8_t j = 0; j < 4; j++)
			{
				out.append(std::to_string(mat[i][j]));
				out.push_back('|');
			}
			out.push_back('\n');
			out.append("------------------------------------\n");
		}
		return out;
	}
};

inline std::ostream& operator<<(std::ostream& os, const mat4& mat)//this just exists so we can log easier
{
	return os << mat.toString();
}