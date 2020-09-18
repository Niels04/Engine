#pragma once

#include <glad/glad.h>

#define GLCALL(x) GLClearError();\
    x;\
    ENG_CORE_ASSERT(GLLogCall(#x, __FILE__, __LINE__), "OpenGL ERROR!");
//#x turns x into a string
/*::::::FUNCTIONS FOR ERROR-CATCHING:::::::::*/
void GLClearError();
bool GLLogCall(const char* function, const char* file, int line);