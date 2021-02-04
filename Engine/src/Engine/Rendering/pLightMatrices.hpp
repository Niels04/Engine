#pragma once
#include "Engpch.hpp"

struct pointLightMatrices
{
	pointLightMatrices() = default;
	mat4 positiveX;
	mat4 negativeX;
	mat4 positiveY;
	mat4 negativeY;
	mat4 positiveZ;
	mat4 negativeZ;
};