#include "Engpch.hpp"

#include "Engine/Application.hpp"//for getting width&height for the frameBuffer

#include "Engine/Rendering/renderer.hpp"
#include "texture.hpp"
#include "Platform/OpenGL/OpenGLTexture.hpp"

namespace Engine
{
	Ref_ptr<texture2d> texture2d::create(const std::string& path, const bool sRGB, const uint32_t filterMin, const uint32_t filterMag)
	{
		switch (Renderer::getAPI())
		{
		case RendererAPI::API::OpenGL:
		{
			return std::make_shared<GLtexture2d>(path, sRGB, filterMin, filterMag);
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

	Ref_ptr<texture2d> texture2d::create(const uint32_t width, const uint32_t height, const uint32_t format, const uint32_t filterMin, const uint32_t filterMag)
	{
		switch (Renderer::getAPI())
		{
		case RendererAPI::API::OpenGL:
		{
			return std::make_shared<GLtexture2d>(width, height, format, filterMin, filterMag);
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

	Ref_ptr<texture2d> texture2d::create(const uint32_t width, const uint32_t height, const uint32_t format, const uint32_t filterMin, const uint32_t filterMag, const uint32_t textureWrap, const void* data)
	{
		switch (Renderer::getAPI())
		{
		case RendererAPI::API::OpenGL:
		{
			return std::make_shared<GLtexture2d>(width, height, format, filterMin, filterMag, textureWrap, data);
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

	Ref_ptr<ShadowMap2dArray> ShadowMap2dArray::create(const uint32_t width, const uint32_t height, const uint8_t layerCount)
	{
		switch (Renderer::getAPI())
		{
		case RendererAPI::API::OpenGL:
		{
			return std::make_shared<GLShadowMap2dArray>(width, height, layerCount);
		}break;
		case RendererAPI::API::NONE:
		{
			ENG_CORE_WARN("Tried to create a ShadowMap2dArray while RenderingAPI was set to \"NONE\". Returning nullptr.");
			return nullptr;
		}break;
		default:
		{
			ENG_CORE_ASSERT(false, "RenderingAPI was an unknown value.");
		}break;
		}
	}

	Ref_ptr<ShadowMap3dArray> ShadowMap3dArray::create(const uint32_t size, const uint8_t layerCount)
	{
		switch (Renderer::getAPI())
		{
		case RendererAPI::API::OpenGL:
		{
			return std::make_shared<GLShadowMap3dArray>(size, layerCount);
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