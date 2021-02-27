#pragma once

#include "mat4.hpp"

struct mat3
{
	real mat[3][3];//first is rows, second is columns
	mat3()
	{
		//initialize to 0
		for (uint8_t i = 0; i < 3; i++)
		{
			std::fill_n(mat[i], 3, 0.0);
		}
		//and then to identity
		mat[0][0] = 1.0;
		mat[1][1] = 1.0;
		mat[2][2] = 1.0;
	}
	mat3(const mat4& other)//construct a 3x3-matrix out of a 4x4-matrix -> gets the upper left values
	{
		mat[0][0] = other[0][0];
		mat[0][1] = other[0][1];
		mat[0][2] = other[0][2];
		mat[1][0] = other[1][0];
		mat[1][1] = other[1][1];
		mat[1][2] = other[1][2];
		mat[2][0] = other[2][0];
		mat[2][1] = other[2][1];
		mat[2][2] = other[2][2];
	}
	void transpose()
	{
		mat3 result;
		for (uint8_t i = 0; i < 3; i++)
		{
			for (uint8_t j = 0; j < 3; j++)
			{
				result[i][j] = mat[j][i];
			}
		}
		*this = result;
	}
	static mat3 transposed(const mat3& mat)
	{
		mat3 result;
		for (uint8_t i = 0; i < 3; i++)
		{
			for (uint8_t j = 0; j < 3; j++)
			{
				result[i][j] = mat.mat[j][i];
			}
		}
		return result;
	}
	static mat3 inverse(const mat3& mat)
	{
		const float determinant = (mat[0][0] * mat[1][1] * mat[2][2] + (mat[0][1] * mat[1][2] * mat[2][0]) + (mat[0][2] * mat[1][0] * mat[2][1]) - (mat[0][2] * mat[1][1] * mat[2][0]) - (mat[0][0] * mat[1][2] * mat[2][1]) - (mat[0][1] * mat[1][0] * mat[2][2]));
		_ASSERT(determinant != 0.0f);
		const float D_inv = 1.0f / determinant;
		mat3 inverse;
		inverse[0][0] = (mat[1][1] * mat[2][2] - mat[1][2] * mat[2][1]) * D_inv;
		inverse[0][1] = -(mat[0][1] * mat[2][2] - mat[0][2] * mat[2][1]) * D_inv;
		inverse[0][2] = (mat[0][1] * mat[1][2] - mat[0][2] * mat[1][1]) * D_inv;
		inverse[1][0] = -(mat[1][0] * mat[2][2] - mat[1][2] * mat[2][0]) * D_inv;
		inverse[1][1] = (mat[0][0] * mat[2][2] - mat[0][2] * mat[2][0]) * D_inv;
		inverse[1][2] = -(mat[0][0] * mat[1][2] - mat[0][2] * mat[1][0]) * D_inv;
		inverse[2][0] = (mat[1][0] * mat[2][1] - mat[1][1] * mat[2][0]) * D_inv;
		inverse[2][1] = -(mat[0][0] * mat[2][1] - mat[0][1] * mat[2][0]) * D_inv;
		inverse[2][2] = (mat[0][0] * mat[1][1] - mat[0][1] * mat[1][0]) * D_inv;
		return inverse;
	}
	//operators
	inline real* operator[](const uint8_t index) { _ASSERT(index < 4); return mat[index]; }
	inline const real* operator[](const uint8_t index) const { _ASSERT(index < 4); return mat[index]; }
};