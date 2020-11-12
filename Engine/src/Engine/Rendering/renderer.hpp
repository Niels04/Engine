#pragma once
#include "renderCommand.hpp"
#include "perspCam.hpp"
#include "shader.hpp"

namespace Engine
{
	class Renderer
	{
	public:
		static void beginScene(perspectiveCamera& cam);//takes in a camera (viewprojMat)
		static void endScene();
		static void Flush();
		static void sub(const std::shared_ptr<GLvertexArray> va, const std::shared_ptr<shader> shader, const mat4& transform = mat4::identity());//stands for submit, gonna overload this, so that we can submit different things //submit a vertexArray and a shader

		inline static RendererAPI::API getAPI() { return RendererAPI::getAPI(); }
	private:
		struct sceneData
		{
			mat4 viewProjMat;
		};
		static sceneData* s_sceneData;
	};
}