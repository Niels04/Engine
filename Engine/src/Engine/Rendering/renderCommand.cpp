#include "Engpch.hpp"
#include "renderCommand.hpp"

#include "Platform/OpenGL/openGLRendererAPI.hpp"

namespace Engine
{
	RendererAPI* renderCommand::s_RendererAPI = new OpenGLRendererAPI;

	void renderCommand::init()
	{
		s_RendererAPI->init();
	}

	void renderCommand::drawIndexed(const Ref_ptr<vertexArray> va)
	{
		s_RendererAPI->drawIndexed(va);
	}

	void renderCommand::clear()
	{
		s_RendererAPI->clear();
	}

	void renderCommand::clearDepth()
	{
		s_RendererAPI->clearDepth();
	}

	void renderCommand::setClearColor(const glm::vec4& color)
	{
		s_RendererAPI->setClearColor(color);
	}

	void renderCommand::setViewport(const uint32_t width, const uint32_t height)
	{
		s_RendererAPI->setViewport(width, height);
	}

	void renderCommand::setBlend(const uint32_t sfactor, const uint32_t dfactor)
	{
		s_RendererAPI->setBlend(sfactor, dfactor);
	}

	void renderCommand::enableBlend(const bool enabled)
	{
		s_RendererAPI->enableBlend(enabled);
	}

	void renderCommand::setDepth(const uint32_t method)
	{
		s_RendererAPI->setDepth(method);
	}

	void renderCommand::enableDepth(const bool enabled)
	{
		s_RendererAPI->enableDepth(enabled);
	}

	void renderCommand::enableCullFace(const bool enabled)
	{
		s_RendererAPI->enableCullFace(enabled);
	}

	void renderCommand::cullFace(const uint32_t face)
	{
		s_RendererAPI->cullFace(face);
	}

	void renderCommand::drawToBuffers(const uint32_t count, ...)
	{
		va_list params;
		va_start(params, count);//pass in the last fixed parameter to get a ptr to the begin of the variadic parameters
		s_RendererAPI->drawToBuffers(count, params);
		va_end(params);
	}

	void renderCommand::copyFrameBufferContents(const uint32_t width, const uint32_t height, const uint32_t buffer, const uint32_t interpolationMode)
	{
		s_RendererAPI->copyFrameBufferContents(width, height, buffer, interpolationMode);
	}
}