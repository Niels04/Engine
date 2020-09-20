#include "Engpch.hpp"
#include "renderCommand.hpp"

#include "Platform/OpenGL/openGLRendererAPI.hpp"

namespace Engine
{
	RendererAPI* renderCommand::s_RendererAPI = new OpenGLRendererAPI;

	void renderCommand::drawIndexed(const std::weak_ptr<GLvertexArray> va)
	{
		s_RendererAPI->drawIndexed(va.lock());
	}

	void renderCommand::clear()
	{
		s_RendererAPI->clear();
	}

	void renderCommand::setClearColor(const glm::vec4& color)
	{
		s_RendererAPI->setClearColor(color);
	}
}