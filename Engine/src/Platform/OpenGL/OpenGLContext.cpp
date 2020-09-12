#include "Engpch.hpp"
#include "OpenGLContext.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace Engine
{
	openGLContext::openGLContext(GLFWwindow* window)
		:m_window(window)
	{
		ENG_CORE_ASSERT(window, "GLFWwindow* was NULL when trying to create OpenGL context!");
	}

	void openGLContext::init()
	{
		glfwMakeContextCurrent(m_window);
		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);//initialize the glLoader
		ENG_CORE_ASSERT(status, "Failed to initialize Glad!");

		ENG_CORE_INFO("Graphics driver: {0} {1}", glGetString(GL_VENDOR), glGetString(GL_RENDERER));
		ENG_CORE_INFO("OpenGL version: {0}", glGetString(GL_VERSION));
	}

	void openGLContext::swapBuffers() const
	{
		glfwSwapBuffers(m_window);
	}
}