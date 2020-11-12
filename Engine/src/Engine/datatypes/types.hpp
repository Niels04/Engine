#pragma once

#include <iostream>
#include <string>

#define FLOAT

#ifdef FLOAT
typedef float real;
#else
#ifdef DOUBLE
typedef double real;
#else
static_assert(false);
#endif
#endif