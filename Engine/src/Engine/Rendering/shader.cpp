#include "Engpch.hpp"
#include "shader.hpp"
#include "renderer.hpp"

#include "Platform/OpenGL/OpenGLShader.hpp"

namespace Engine
{
	shader* shader::create(const char* name)
	{
		switch (Renderer::getAPI())
		{
		case RendererAPI::API::OpenGL:
		{
			return new GLshader(name);
		}break;
		case RendererAPI::API::NONE:
		{
			ENG_CORE_WARN("Tried to create vertexBuffer while RenderingAPI was set to \"NONE\". Returning nullptr.");
			return nullptr;
		}break;
		default:
		{
			ENG_CORE_ASSERT(false, "RenderingAPI was out of range of the possible values.");
		}break;
		}
	}
}