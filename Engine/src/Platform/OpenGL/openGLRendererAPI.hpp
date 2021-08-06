#pragma once
#include "Engine/Rendering/rendererAPI.hpp"

namespace Engine
{
	class OpenGLRendererAPI : public RendererAPI
	{
	public:
		virtual void init() const override;
		virtual void clear() const override;
		virtual void clearDepth() const override;
		virtual void setClearColor(const glm::vec4& color) const override;
		virtual void drawIndexed(const Ref_ptr<vertexArray> va) const override;//take in a shared_ptr here, cuz we would need to convert anyway, because we want to access it in this function
		virtual void setViewport(const uint32_t width, const uint32_t height) const override;
		virtual void setBlend(const uint32_t sfactor, const uint32_t dfactor) const override;
		virtual void enableBlend(const bool enabled) const override;
		virtual void setDepth(const uint32_t method) const override;
		virtual void enableDepth(const bool enabled) const override;
		virtual void enableCullFace(const bool enabled) const override;
		virtual void cullFace(const uint32_t face) const override;
		virtual void drawToBuffers(const uint32_t count, va_list params) const override;
		virtual void copyFrameBufferContents(const uint32_t width, const uint32_t const height, const uint32_t const buffer, const uint32_t interpolationMode) const override;
		virtual void copyFrameBufferContents(const uint32_t width_src, const uint32_t height_src, const uint32_t width_dest, const uint32_t height_dest, const uint32_t buffer, const uint32_t interpolationMode) const override;
		virtual const uint32_t getMaxGlobalBuffers() const override;
		virtual const uint8_t getMaxTextureBinds() const override;
	};
}