#include "Engpch.hpp"

#include "Engine/Application.hpp"//for getting width&height for the frameBuffer

#include "Engine/Rendering/renderer.hpp"
#include "texture.hpp"
#include "Platform/OpenGL/OpenGLTexture.hpp"

namespace Engine
{
	Ref_ptr<texture2d> texture2d::create(const std::string& path, const uint32_t filterMin, const uint32_t filterMag)
	{
		switch (Renderer::getAPI())
		{
		case RendererAPI::API::OpenGL:
		{
			return std::make_shared<GLtexture2d>(path, filterMin, filterMag);
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

	Ref_ptr<ShadowMap2d> ShadowMap2d::create(const uint32_t width, const uint32_t height)
	{
		switch (Renderer::getAPI())
		{
		case RendererAPI::API::OpenGL:
		{
			return std::make_shared<GLShadowMap2d>(width, height);
		}break;
		case RendererAPI::API::NONE:
		{
			ENG_CORE_WARN("Tried to create a ShadowMap2d while RenderingAPI was set to \"NONE\". Returning nullptr.");
			return nullptr;
		}break;
		default:
		{
			ENG_CORE_ASSERT(false, "RenderingAPI was an unknown value.");
		}break;
		}
	}

	Ref_ptr<ShadowMap3d> ShadowMap3d::create(const uint32_t width, const uint32_t height)
	{
		switch (Renderer::getAPI())
		{
		case RendererAPI::API::OpenGL:
		{
			return std::make_shared<GLShadowMap3d>(width, height);
		}break;
		case RendererAPI::API::NONE:
		{
			ENG_CORE_WARN("Tried to create a ShadowMap3d while RenderingAPI was set to \"NONE\". Returning nullptr.");
			return nullptr;
		}break;
		default:
		{
			ENG_CORE_ASSERT(false, "RenderingAPI was an unknown value.");
		}break;
		}
	}
}