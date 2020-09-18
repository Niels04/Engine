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
		case RenderingAPI::OpenGL:
		{
			return new GLvertexBuffer(size, data, usage);
		}break;
		case RenderingAPI::NONE:
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

	indexBuffer* indexBuffer::create(const uint32_t count, const uint32_t* data, uint32_t usage)
	{
		switch (Renderer::getAPI())
		{
		case RenderingAPI::OpenGL:
		{
			return new GLindexBuffer(count, data, usage);
		}break;
		case RenderingAPI::NONE:
		{
			ENG_CORE_WARN("Tried to create indexBuffer while RenderingAPI was set to \"NONE\". Returning nullptr.");
			return nullptr;
		}break;
		default:
		{
			ENG_CORE_ASSERT(false, "RenderingAPI was out of range of the possible values.");
		}break;
		}
	}
}