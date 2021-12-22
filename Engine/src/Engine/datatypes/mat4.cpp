#include "Engpch.hpp"

#include "mat4.hpp"
#include "mat3.hpp"

mat3 mat4::NormalFromModel(const mat4& model)
{
	return mat3::transposed(mat3::inverse(mat3(model)));
}