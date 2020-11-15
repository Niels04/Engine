#include "Engpch.hpp"

#include "buffers.hpp"
#include "Platform/OpenGL/GLbuffers.hpp"
#include "renderer.hpp"

namespace Engine
{
	vertexBuffer* vertexBuffer::create(const uint32_t size, const void* data, const uint32_t usage)
	{
		switch (Renderer::getAPI())
		{
		case RendererAPI::API::OpenGL:
		{
			return new GLvertexBuffer(size, data, usage);
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

	indexBuffer* indexBuffer::create(const uint32_t count, const uint32_t* data, uint32_t usage)
	{
		switch (Renderer::getAPI())
		{
		case RendererAPI::API::OpenGL:
		{
			return new GLindexBuffer(count, data, usage);
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

	globalBuffer* globalBuffer::create(const uint16_t size, const uint32_t usage)
	{
		switch (Renderer::getAPI())
		{
		case RendererAPI::API::OpenGL:
		{
			return new GLglobalBuffer(size, usage);
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