#include "Engpch.hpp"
#include "vertexBufferLayout.hpp"
#include "Platform/OpenGL/OpenGLVertexBufferLayout.hpp"

#include "renderer.hpp"

namespace Engine
{
	Ref_ptr<vertexBufferLayout> vertexBufferLayout::create()
	{
		switch (Renderer::getAPI())
		{
		case RendererAPI::API::OpenGL:
		{
			return std::make_shared<GLvertexBufferLayout>();
		}break;
		case RendererAPI::API::NONE:
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