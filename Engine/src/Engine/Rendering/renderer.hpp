#pragma once
#include "renderCommand.hpp"

namespace Engine
{

	class Renderer
	{
	public:
		static void beginScene(glm::mat4x4 viewproj);//takes in a camera (viewprojMat)
		static void endScene();
		static void Flush();
		static void sub(const std::shared_ptr<GLvertexArray> va);//stands for submit, gonna overload this, so that we can submit different things

		inline static RendererAPI::API getAPI() { return RendererAPI::getAPI(); }
	private:
		static glm::mat4x4 s_cam_viewProj;
	};
}