#include "Engpch.hpp"

#include "Engine/Rendering/renderer.hpp"
#include "texture.hpp"
#include "Platform/OpenGL/OpenGLTexture.hpp"

namespace Engine
{
	Ref_ptr<texture2d> texture2d::create(const std::string& path)
	{
		switch (Renderer::getAPI())
		{
		case RendererAPI::API::OpenGL:
		{
			return std::make_shared<GLtexture2d>(path);
		}break;
		case RendererAPI::API::NONE:
		{
			ENG_CORE_WARN("Tried to create a 2d-texture while RenderingAPI was set to \"NONE\". Returning nullptr.");
			return nullptr;
		}break;
		default:
		{
			ENG_CORE_ASSERT(false, "RenderingAPI was an unknown value.");
		}break;
		}
	}
}