#include "Engpch.hpp"
#include "GraphicsContext.hpp"
#include "Platform/OpenGL/OpenGLContext.hpp"

#include "renderer.hpp"

namespace Engine
{
	graphicsContext* graphicsContext::create(window* window)
	{
		switch (Renderer::getAPI())
		{
		case RendererAPI::API::OpenGL:
		{
			#ifdef ENG_PLATFORM_WINDOWS//GLFW is the default on windows right now
			return new GLcontext((GLFWwindow*)window);
			#else
			ENG_CORE_ASSERT(false, "Right now only windows is supportet, and thererfore only GLFW windows.");
			#endif
		}break;
		case RendererAPI::API::NONE:
		{
			ENG_CORE_WARN("Tried to create graphicsContext while RenderingAPI was set to \"NONE\". Returning nullptr.");
			return nullptr;
		}break;
		default:
		{
			ENG_CORE_ASSERT(false, "RenderingAPI was out of range of the possible values.");
		}break;
		}
	}
}