#pragma once

#include "Engine/Rendering/GraphicsContext.hpp"

struct GLFWwindow;

namespace Engine
{
	class GLcontext : public graphicsContext
	{
	public:
		GLcontext(GLFWwindow* window);

		virtual void init() override;
		virtual void swapBuffers() const override;
	private:
		GLFWwindow* m_window;
	};
}