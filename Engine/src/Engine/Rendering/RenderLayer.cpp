#include "Engpch.hpp"

#include "RenderLayer.hpp"

namespace Engine
{
	RenderLayer::RenderLayer()
		: layer("RenderLayer")
	{

	}

	void RenderLayer::onImGuiRender()
	{
		Engine::Renderer::onImGuiRender();
	}

	void RenderLayer::begin()
	{
		//Engine::renderCommand::setClearColor({ 0.0171f, 0.0171f, 0.0171f, 1.0f });
		renderCommand::setClearColor({ 0.0f, 0.0f, 0.0f, 1.0f });
		renderCommand::clear();

		Renderer::beginScene();
	}

	void RenderLayer::end()
	{
		Engine::Renderer::endScene();
		//first render to the depth maps
		Engine::Renderer::RenderDepthMaps();
		//then render the scene with the shadows applied
		Engine::Renderer::Flush();
		//Engine::Renderer::test();
	}
}