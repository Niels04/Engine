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

	Ref_ptr<FrameBufferTexture> FrameBufferTexture::create(const FrameBufferTextureUsage usage)
	{
		switch (Renderer::getAPI())
		{
		case RendererAPI::API::OpenGL:
		{
			return std::make_shared<GLFrameBufferTexture>(usage, Application::Get().getWindow().getWidth(), Application::Get().getWindow().getHeight());//frameBuffer texture has the size of our current window
		}break;
		case RendererAPI::API::NONE:
		{
			ENG_CORE_WARN("Tried to create a FrameBufferColorTexture while RenderingAPI was set to \"NONE\". Returning nullptr.");
			return nullptr;
		}break;
		default:
		{
			ENG_CORE_ASSERT(false, "RenderingAPI was an unknown value.");
		}break;
		}
	}

	Ref_ptr<FrameBufferTexture> FrameBufferTexture::createShadowMap(const uint32_t width, const uint32_t height)
	{
		switch (Renderer::getAPI())
		{
		case RendererAPI::API::OpenGL:
		{
			return std::make_shared<GLFrameBufferTexture>(width, height);
		}break;
		case RendererAPI::API::NONE:
		{
			ENG_CORE_WARN("Tried to create a FrameBufferColorTexture while RenderingAPI was set to \"NONE\". Returning nullptr.");
			return nullptr;
		}break;
		default:
		{
			ENG_CORE_ASSERT(false, "RenderingAPI was an unknown value.");
		}break;
		}
	}
}