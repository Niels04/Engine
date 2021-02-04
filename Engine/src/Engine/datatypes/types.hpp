#pragma once

#include <iostream>
#include <string>

#define FLOAT

#ifdef FLOAT
typedef float real;
#define EPSILON 1e-5f
#else
#ifdef DOUBLE
typedef double real;
#define EPSILON 1e-5
#else
static_assert(false);
#endif
#endif

constexpr inline real nearToZero(const real n)//basically just a check if the value is near some very small value EPSILON
{
#ifdef DOUBLE
	return fabs(n) <= EPSILON ? 0.0 : n;
#else
	return fabsf(n) <= EPSILON ? 0.0f : n;
#endif
}

#define close0(x) nearToZero(x)