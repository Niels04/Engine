#pragma once
#include "rendererAPI.hpp"

namespace Engine
{
	class renderCommand
	{
	public:
		static void init();
		static void drawIndexed(const Ref_ptr<GLvertexArray> va);
		static void setClearColor(const glm::vec4& color);
		static void clear();
	private:
		static RendererAPI* s_RendererAPI;
	};
}