#include "Engpch.hpp"
#include "glCore.hpp"
#include "OpenGLContext.hpp"

#include <GLFW/glfw3.h>

namespace Engine
{
	GLcontext::GLcontext(GLFWwindow* window)
		:m_window(window)
	{
		ENG_CORE_ASSERT(window, "GLFWwindow* was NULL when trying to create OpenGL context!");
	}

	void GLcontext::init()
	{
		glfwMakeContextCurrent(m_window);
		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);//initialize the glLoader
		ENG_CORE_ASSERT(status, "Failed to initialize Glad!");

		ENG_CORE_INFO("Graphics driver: {0} {1}", glGetString(GL_VENDOR), glGetString(GL_RENDERER));
		ENG_CORE_INFO("OpenGL version: {0}", glGetString(GL_VERSION));
	}

	void GLcontext::swapBuffers() const
	{
		glfwSwapBuffers(m_window);
	}
}