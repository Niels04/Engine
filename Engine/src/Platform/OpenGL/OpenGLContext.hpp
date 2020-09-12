#pragma once

#include "Engine/Rendering/GraphicsContext.hpp"

struct GLFWwindow;

namespace Engine
{
	class openGLContext : public graphicsContext
	{
	public:
		openGLContext(GLFWwindow* window);

		virtual void init() override;
		virtual void swapBuffers() const override;
	private:
		GLFWwindow* m_window;
	};
}