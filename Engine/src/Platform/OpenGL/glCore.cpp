#include "Engpch.hpp"
#include "glCore.hpp"

void GLClearError()
{
    while (glGetError() != GL_NO_ERROR);//we dont care about errors right now, we just clear them with an empty for-loop
}

bool GLLogCall(const char* function, const char* file, int line)
{
    while (GLenum error = glGetError())
    {
        ENG_CORE_ERROR("[OpenGL ERROR] ({0}) In function: {1} {2}    line: {3}.", error, function, file, line);
        return false;//______________Reminder: This only returns the first error!___________________
    }
    return true;
}