#pragma once
#include "rendererAPI.hpp"

namespace Engine
{
	class renderCommand
	{
	public:
		static void drawIndexed(const std::weak_ptr<GLvertexArray> va);//takes in a weak_ptr, because this function just calls an apiImplementation, that convertes it to a shared_ptr
		static void setClearColor(const glm::vec4& color);
		static void clear();
	private:
		static RendererAPI* s_RendererAPI;
	};
}