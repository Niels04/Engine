#pragma once
#include "rendererAPI.hpp"

namespace Engine
{
	class renderCommand
	{
	public:
		static void init();
		static void drawIndexed(const Ref_ptr<vertexArray> va);
		static void setClearColor(const glm::vec4& color);
		static void clear();
		static void clearDepth();
		static void setViewport(const uint32_t width, const uint32_t heigt);
		static void setBlend(const uint32_t sfactor, const uint32_t dfactor);
		static void enableBlend(const bool enabled);
		static void setDepth(const uint32_t method);
		static void enableDepth(const bool enabled);
		static void cullFace(const uint32_t face);
		static void drawToBuffers(const uint32_t count, ...);
		static uint32_t getMaxGlobalBuffers() { return s_RendererAPI->getMaxGlobalBuffers(); }
		static uint8_t getMaxTextureBinds() { return s_RendererAPI->getMaxTextureBinds(); }
	private:
		static RendererAPI* s_RendererAPI;
	};
}