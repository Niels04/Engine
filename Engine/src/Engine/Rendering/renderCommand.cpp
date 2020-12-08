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

	void renderCommand::setClearColor(const glm::vec4& color)
	{
		s_RendererAPI->setClearColor(color);
	}
}