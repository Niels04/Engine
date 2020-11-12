#pragma once
#include "Engpch.hpp"

struct X {  };
struct Y {  };
struct Z {  };

struct vec4
{
	float v0, v1, v2, v3;

	vec4() {  }//standard constructor
	vec4(float in0)//constructor that initializes the whole vector with a number
	{
		v0 = in0; v1 = in0; v2 = in0; v3 = in0;
	}
	vec4(float in0, float in1, float in2, float in3)//constructor that takes in all values
	{
		v0 = in0;	v1 = in1;	v2 = in2;	v3 = in3;
	}

	float& operator [](unsigned int index)//read/write operator
	{
		_ASSERT(index <= 3U);
		return *(&v0 + (index * sizeof(float)));
	}
};

struct mat33f
{
	float m_mat[3][3] = { };

	//constructors:
	mat33f() {  }

	template<class T>
	static mat33f setRotMat(float rad)
	{
		static_assert(false);//should never happen
	}
	template<>
	static mat33f setRotMat<X>(float rad)//create a rotationMatrix that rotates around the X axis
	{

	}
	template<>
	static mat33f setRotMat<Y>(float rad)//create a rotationMatrix that rotates around the Y axis
	{

	}
	template<>
	static mat33f setRotMat<Z>(float rad)//create a rotationMatrix that rotates around the Z axis
	{
		mat33f out;
		out[0][0] = cosf(rad);
		out[0][1] = sinf(rad);
		out[1][0] = sinf(rad);
		out[1][1] = cosf(rad);
		out[2][2] = 1.0f;

		return out;
	}
	static mat33f setRotMat(float Xrad, float Yrad, float Zrad)
	{
		mat33f out = setRotMat<X>(Xrad) * setRotMat<Y>(Yrad);
		out *= setRotMat<Z>(Zrad);
		return out;
	}

	const mat33f operator * (const mat33f& rhs)//read only operator
	{
		mat33f multip;
		for (unsigned int i = 0; i < 3; i++)
		{
			for (unsigned int j = 0; j < 3; j++)
			{
				multip.m_mat[i][j] = m_mat[i][0] * rhs.m_mat[0][j];
			}
		}
		return multip;
	}
	const mat33f operator *= (const mat33f& rhs)//read only operator
	{
		mat33f multip;
		for (unsigned int i = 0; i < 3; i++)
		{
			for (unsigned int j = 0; j < 3; j++)
			{
				m_mat[i][j] = m_mat[i][0] * rhs.m_mat[0][j];
			}
		}
	}

	float* operator [](unsigned int index)//read/write operator
	{
		_ASSERT(index <= 2U);
		return m_mat[index];
	}

};

//______________4x4Matrix_with_functions_to_init_a_perspective_projMat______________
struct mat44f
{
	float m_mat[4][4] = { };//first is column, second is row

	//constructors:
	mat44f(){  }

	static mat44f empty()
	{
		mat44f out;
		for (unsigned int i = 0; i < 4; i++)
		{
			for (unsigned int j = 0; j < 4; j++)
				out.m_mat[i][j] = 0.0f;
		}
		return out;
	}

	/*// CAN'T USE THIS RN BECAUE IT RELIES ON ROW-MAJOR-MULTIPLICATION
	static mat44f setBasicProjMat(float Znear, float Zfar, float hfov, float vfov)
	{
		ENG_CORE_WARN("Using deprecated function \"setBasicProjMat\". Only use when using row-major-multiplication.");
		mat44f out;

		out.m_mat[0][0] = 1 / tanf((hfov / 2) * (3.141592653f / 180.0f));
		out.m_mat[1][1] = 1 / tanf((vfov / 2) * (3.141592653f / 180.0f));
		out.m_mat[2][2] = Zfar / (Zfar - Znear);
		out.m_mat[3][2] = -Zfar * Znear / (Zfar - Znear);//note that when when multiplying a points z-component with this matrix a non-linear operation is performed
		out.m_mat[2][3] = 1.0f;//we set w to be z
	}*/
	//set a modelMatrix
	static mat44f setModelMat(float x, float y, float z)
	{
		mat44f out;

		out[0][0] = 1.0f;
		out[1][1] = 1.0f;
		out[2][2] = 1.0f;
		out[3][3] = 1.0f;
		out[3][0] = x;//set the x component
		out[3][1] = y;//set the y component
		out[3][2] = z;//set the z component

		return out;
	}
	static mat44f setViewMat()
	{
		
	}
	static mat44f setProjMat(float Znear, float Zfar, float hfov, float vfov)
	{
		mat44f out;

		float t = tanf((vfov * 0.5) * (3.141592653f / 180.0f)) * Znear;
		float b = 0.0f - t;
		float r = tanf((hfov * 0.5) * (3.141592653f / 180.0f)) * Znear;
		float l = 0.0f - r;

		out.m_mat[0][0] = (2.0f * Znear) / (r - l);
		out.m_mat[2][0] = (r + l) / (r - l);
		out.m_mat[1][1] = (2.0f * Znear) / (t - b);
		out.m_mat[2][1] = (t + b) / (t - b);
		out.m_mat[2][2] = (Zfar + Znear) / (Zfar - Znear);
		out.m_mat[3][2] = ((-2.0f) * Zfar * Znear) / (Zfar - Znear);
		out.m_mat[2][3] = 1.0f;

		return out;
	}
	const mat44f operator * (const mat44f& rhs)//read only operator
	{
		mat44f multip;
		for (unsigned int i = 0; i < 4; i++)
		{
			for (unsigned int j = 0; j < 4; j++)
			{
				multip.m_mat[i][j] = m_mat[i][0] * rhs.m_mat[0][j];
			}
		}
		return multip;
	}
	const mat44f operator *= (const mat44f& rhs)//read only operator
	{
		mat44f multip;
		for (unsigned int i = 0; i < 4; i++)
		{
			for (unsigned int j = 0; j < 4; j++)
			{
				m_mat[i][j] = m_mat[i][0] * rhs.m_mat[0][j];
			}
		}
	}

	float* operator [](unsigned int index)//read/write operator
	{
		_ASSERT(index <= 3U);
		return m_mat[index];
	}
};