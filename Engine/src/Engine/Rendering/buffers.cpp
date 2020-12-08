#include "Engpch.hpp"

#include "buffers.hpp"
#include "Platform/OpenGL/GLbuffers.hpp"
#include "renderer.hpp"

namespace Engine
{
	Ref_ptr<vertexBuffer> vertexBuffer::create(const uint32_t size, const void* data, const uint32_t usage)
	{
		switch (Renderer::getAPI())
		{
		case RendererAPI::API::OpenGL:
		{
			return std::make_shared<GLvertexBuffer>(size, data, usage);
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

	Ref_ptr<indexBuffer> indexBuffer::create(const uint32_t count, const uint32_t* data, uint32_t usage)
	{
		switch (Renderer::getAPI())
		{
		case RendererAPI::API::OpenGL:
		{
			return std::make_shared<GLindexBuffer>(count, data, usage);
		}break;
		case RendererAPI::API::NONE:
		{
			ENG_CORE_WARN("Tried to create indexBuffer while RenderingAPI was set to \"NONE\". Returning nullptr.");
			return nullptr;
		}break;
		default:
		{
			ENG_CORE_ASSERT(false, "RenderingAPI was an unknown value.");
		}break;
		}
	}

	Ref_ptr<globalBuffer> globalBuffer::create(const uint16_t size, const uint32_t usage)
	{
		switch (Renderer::getAPI())
		{
		case RendererAPI::API::OpenGL:
		{
			return std::make_shared<GLglobalBuffer>(size, usage);
		}break;
		case RendererAPI::API::NONE:
		{
			ENG_CORE_WARN("Tried to create globalBuffe while RenderingAPI was set to \"NONE\". Returning nullptr.");
			return nullptr;
		}break;
		default:
		{
			ENG_CORE_ASSERT(false, "RenderingAPI was an unknown value.");
		}break;
		}
	}

	Scope_ptr<globalBuffer> globalBuffer::createUnique(const uint16_t size, const uint32_t usage)
	{
		switch (Renderer::getAPI())
		{
		case RendererAPI::API::OpenGL:
		{
			return std::make_unique<GLglobalBuffer>(size, usage);
		}break;
		case RendererAPI::API::NONE:
		{
			ENG_CORE_WARN("Tried to create globalBuffe while RenderingAPI was set to \"NONE\". Returning nullptr.");
			return nullptr;
		}break;
		default:
		{
			ENG_CORE_ASSERT(false, "RenderingAPI was an unknown value.");
		}break;
		}
	}
}