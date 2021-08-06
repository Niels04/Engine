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

	void renderCommand::copyFrameBufferContents(const uint32_t width_src, const uint32_t height_src, const uint32_t width_dest, const uint32_t height_dest, const uint32_t buffer, const uint32_t interpolationMode)
	{
		s_RendererAPI->copyFrameBufferContents(width_src, height_src, width_dest, height_dest, buffer, interpolationMode);
	}

	//note that for anything else than color attachments "src_attachment" and "dest_attachment" have to be exactly the same!!!
	void renderCommand::copyFrameBufferContents(const Ref_ptr<FrameBuffer> src_fbo, const uint32_t src_width, const uint32_t src_height, const uint32_t src_attachment,
		const Ref_ptr<FrameBuffer> dest_fbo, const uint32_t dest_width, const uint32_t dest_height, const uint32_t dest_attachment, const uint32_t interpolationMode)
	{
		src_fbo->setReadBuffer();
		dest_fbo->setDrawBuffer();
		if (src_attachment == ENG_DEPTH_BUFFER_BIT)
		{
			ENG_ASSERT(dest_attachment == src_attachment, "When using non-color attachments, source and destination attachments are to be of the same type.");
			ENG_ASSERT(interpolationMode == FILTER_NEAREST, "Linear interpolation can only be used with color attachments.");
			s_RendererAPI->copyFrameBufferContents(src_width, src_height, dest_width, dest_height, ENG_DEPTH_BUFFER_BIT, interpolationMode);
		}
		else if (src_attachment >= ENG_COLOR_ATTACHMENT0 && src_attachment <= ENG_COLOR_ATTACHMENT31)//if both are color attachments
		{
			ENG_ASSERT(dest_attachment >= ENG_COLOR_ATTACHMENT0 && dest_attachment <= ENG_COLOR_ATTACHMENT31, "Can't copy attachment of type color into attachment of other type.");
			src_fbo->setSrcBuffer(src_attachment);
			dest_fbo->setDestBuffer(dest_attachment);
			s_RendererAPI->copyFrameBufferContents(src_width, src_height, dest_width, dest_height, ENG_COLOR_BUFFER_BIT, interpolationMode);
			dest_fbo->unbind();//just so that no frameBuffer is bound after the operation
		}
		else
			ENG_ASSERT(false, "Unsupported value of \"src_attachment\".");
	}

}