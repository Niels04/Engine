#include "Engpch.hpp"
#include "vertexBufferLayout.hpp"
#include "Platform/OpenGL/OpenGLVertexBufferLayout.hpp"

#include "renderer.hpp"

namespace Engine
{
	vertexBufferLayout* vertexBufferLayout::create()
	{
		switch (Renderer::getAPI())
		{
		case RenderingAPI::OpenGL:
		{
			return new GLvertexBufferLayout;
		}break;
		case RenderingAPI::NONE:
		{
			ENG_CORE_WARN("Tried to create vertexBufferLayout while RenderingAPI was set to \"NONE\". Returning nullptr.");
			return nullptr;
		}break;
		default:
		{
			ENG_CORE_ASSERT(false, "RenderingAPI was out of range of the possible values.");
		}break;
		}
	}
}