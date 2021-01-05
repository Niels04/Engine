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
		virtual const uint32_t getMaxGlobalBuffers() const override;
		virtual const uint8_t getMaxTextureBinds() const override;
	};
}