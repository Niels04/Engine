#include "Engpch.hpp"

#include "vertexArray.hpp"
#include "Platform/OpenGL/OpenGLVertexArray.hpp"

#include "renderer.hpp"

namespace Engine
{
	Ref_ptr<vertexArray> vertexArray::create()
	{
		switch (Renderer::getAPI())
		{
		case RendererAPI::API::OpenGL:
		{
			return std::make_shared<GLvertexArray>();
		}break;
		case RendererAPI::API::NONE:
		{
			ENG_CORE_WARN("Tried to create vertexBuffer while RenderingAPI was set to \"NONE\". Returning nullptr.");
			return nullptr;
		}break;
		default:
		{
			ENG_CORE_ASSERT(false, "RenderingAPI was an unknown value.");
		}break;
		}
	}
}