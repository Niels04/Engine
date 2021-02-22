#include "Engpch.hpp"

#include "Engine/Events/ApplicationEvent.hpp"
#include "Engine/Events/KeyEvent.hpp"
#include "Engine/Events/MouseEvent.hpp"

#include "Engine/Rendering/GraphicsContext.hpp"
//for using the appropriate graphics API
#include "Engine/Rendering/renderer.hpp"

#include "windowsWindow.hpp"

#include "GLFW/glfw3.h"

namespace Engine
{
	static bool s_GLFW_initialized = false;//static, because we only initialize this once

	static void GLFWErrorCallback(int errorCode, const char* description)
	{
		ENG_CORE_ERROR("GLFW error: Code: {0} Description: {1}", errorCode, description);
	}

	window* window::create(const windowProps& props)
	{
		return new windowsWindow(props);
	}

	windowsWindow::windowsWindow(const windowProps& props)
	{
		init(props);
	}

	windowsWindow::~windowsWindow()
	{
		shutdown();
	}

	void windowsWindow::init(const windowProps& props)
	{
		m_data.title = props.title;
		m_data.height = props.height;
		m_data.width = props.width;

		ENG_CORE_INFO("Creating window \"{0}\" ({1} x {2}).", m_data.title, m_data.width, m_data.height);

		if (!s_GLFW_initialized)
		{
			int success = glfwInit();
			ENG_CORE_ASSERT(success, "Could not initialize GLFW!");//if glfwInit() failed
			glfwSetErrorCallback(GLFWErrorCallback);

			s_GLFW_initialized = true;
		}

		m_window = glfwCreateWindow((int)m_data.width, (int)m_data.height, m_data.title.c_str(), nullptr, nullptr);
		
		m_context = graphicsContext::create((window*)m_window);//cast back to a normal window
		m_context->init();

		glfwSetWindowUserPointer(m_window, &m_data);
		setVsync(true);

		//set GLFW callbacks:
		//windowResizeCallback:
		glfwSetWindowSizeCallback(m_window, [](GLFWwindow* window, int width, int height) {//use a lambda, that gets set as the callback-func
			windowData& data = *(windowData*)glfwGetWindowUserPointer(window);
			//set variables that keep track of the width and height of the window
			data.height = height;
			data.width = width;

			windowResizeEvent event(height, width);//create the actual event
			data.callback(event);//dispatch the event
		});
		glfwSetWindowCloseCallback(m_window, [](GLFWwindow* window) {
			windowData& data = *(windowData*)glfwGetWindowUserPointer(window);
			windowCloseEvent event;
			data.callback(event);
		});

		glfwSetKeyCallback(m_window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
			windowData& data = *(windowData*)glfwGetWindowUserPointer(window);

			switch (action)
			{
			case GLFW_PRESS:
			{
				keyPressedEvent event(key, 0);//repeat count to 0 because the hasn't been pressed repeatedly
				data.callback(event);
			}break;
			case GLFW_RELEASE:
			{
				keyReleasedEvent event(key);
				data.callback(event);
			}break;
			case GLFW_REPEAT:
			{
				keyPressedEvent event(key, 1);//just set that to 1 for now, it is possible to extract the repeat count from GLFW
				data.callback(event);
			}break;
			}
		});

		glfwSetCharCallback(m_window, [](GLFWwindow* window, unsigned int keycode) {
			windowData& data = *(windowData*)glfwGetWindowUserPointer(window);
			keyTypedEvent event(keycode);
			data.callback(event);
			});

		glfwSetMouseButtonCallback(m_window, [](GLFWwindow* window, int button, int action, int mods) {
			windowData& data = *(windowData*)glfwGetWindowUserPointer(window);

			switch (action)
			{
			case GLFW_PRESS:
			{
				mouseButtonPressedEvent event(button);
				data.callback(event);
			}break;
			case GLFW_RELEASE:
			{
				mouseButtonReleasedEvent event(button);
				data.callback(event);
			}break;
			}
		});

		glfwSetScrollCallback(m_window, [](GLFWwindow* window, double Xoffset, double Yoffset) {
			windowData& data = *(windowData*)glfwGetWindowUserPointer(window);
			mouseScrollEvent event((float)Xoffset, (float)Yoffset);
			data.callback(event);
		});

		glfwSetCursorPosCallback(m_window, [](GLFWwindow* window, double Xpos, double Ypos) {
			windowData& data = *(windowData*)glfwGetWindowUserPointer(window);
			mouseMoveEvent event((float)Xpos, (float)Ypos);
			data.callback(event);
		});

		ENG_CORE_ASSERT(glfwRawMouseMotionSupported(), "This computer doesn't seem to provide raw mouse motion data.");
	}

	void windowsWindow::shutdown()
	{
		glfwDestroyWindow(m_window);
	}

	void windowsWindow::onUpdate()
	{
		glfwPollEvents();
		m_context->swapBuffers();
	}

	void windowsWindow::setVsync(const bool enabled)
	{
		if (enabled)
		{
			glfwSwapInterval(1);
		}
		else
		{
			glfwSwapInterval(0);
		}

		m_data.Vsync = enabled;
	}
	bool windowsWindow::isVsync() const
	{
		return m_data.Vsync;
	}

	void windowsWindow::setDissableCursor(const bool enabled)
	{
		if (enabled)
			glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		else
			glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		m_data.dissableCursor = enabled;
	}
	bool windowsWindow::isDissableCursor() const
	{
		return m_data.dissableCursor;
	}

	void windowsWindow::setFullscreen(const bool enabled)
	{
		if (enabled)
			glfwSetWindowMonitor(m_window, glfwGetPrimaryMonitor(), 0, 0, 1920, 1080, GLFW_DONT_CARE);
		else
			glfwSetWindowMonitor(m_window, nullptr, 100, 100, m_data.width, m_data.height, GLFW_DONT_CARE);
		m_data.fullscreen = enabled;
	}
	bool windowsWindow::isFullscreen() const
	{
		return m_data.fullscreen;
	}

	void windowsWindow::setRawMouseInput(const bool enabled)
	{
		if (!m_data.dissableCursor)
			ENG_CORE_WARN("Tried to enable rawMouse input, while cursor was still enabled. Make sure to dissable cursor first using \"setDissableCursor(const bool enable)\".");
		if (enabled)
			glfwSetInputMode(m_window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
		else
			glfwSetInputMode(m_window, GLFW_RAW_MOUSE_MOTION, GLFW_FALSE);
		m_data.rawMouseInput = enabled;
	}
	bool windowsWindow::isRawMouseInput() const
	{
		return m_data.rawMouseInput;
	}
}