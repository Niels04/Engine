#include "Engpch.hpp"

#include "windowsInput.hpp"
#include <GLFW/glfw3.h>
#include "Engine/Application.hpp"

namespace Engine
{
	input* input::s_instance = new windowsInput;//we can just set this to a new windowsInput because the constructor doesn't rely on anything and the class has no data in it

	bool windowsInput::isKeyDownImpl(int keycode)
	{
		GLFWwindow* window = static_cast<GLFWwindow*>(Application::Get().getWindow().getNativeWindow());
		int state = glfwGetKey(window, keycode);

		return state == GLFW_PRESS || state == GLFW_REPEAT;//if the key is pressed of beeing pressed repeatedly, we return true, otherwise we return false
	}

	bool windowsInput::isMouseButtonDownImpl(int button)
	{
		GLFWwindow* window = static_cast<GLFWwindow*>(Application::Get().getWindow().getNativeWindow());
		int state = glfwGetMouseButton(window, button);

		return state == GLFW_PRESS;
	}

	std::pair<float, float> windowsInput::getMousePosImpl()
	{
		GLFWwindow* window = static_cast<GLFWwindow*>(Application::Get().getWindow().getNativeWindow());
		double Xpos, Ypos;
		glfwGetCursorPos(window, &Xpos, &Ypos);

		return {(float)Xpos, (float)Ypos};
	}

	float windowsInput::getMouseXImpl()
	{
		GLFWwindow* window = static_cast<GLFWwindow*>(Application::Get().getWindow().getNativeWindow());
		double pos;
		glfwGetCursorPos(window, &pos, NULL);

		return (float)pos;
	}

	float windowsInput::getMouseYImpl()
	{
		GLFWwindow* window = static_cast<GLFWwindow*>(Application::Get().getWindow().getNativeWindow());
		double pos;
		glfwGetCursorPos(window, NULL, &pos);

		return (float)pos;
	}
}