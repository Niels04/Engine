#include "Engpch.hpp"
#include "renderer.hpp"

namespace Engine
{
	void Renderer::beginScene(glm::mat4x4 viewproj)
	{
		s_cam_viewProj = viewproj;
	}

	void Renderer::endScene()
	{
	}

	void Renderer::sub(std::shared_ptr<GLvertexArray> va)
	{
		va->bind();
		renderCommand::drawIndexed(va);
	}

	void Renderer::Flush()
	{

	}
}