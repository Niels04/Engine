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
		static uint32_t getMaxGlobalBuffers() { return s_RendererAPI->getMaxGlobalBuffers(); }
		static uint8_t getMaxTextureBinds() { return s_RendererAPI->getMaxTextureBinds(); }
	private:
		static RendererAPI* s_RendererAPI;
	};
}