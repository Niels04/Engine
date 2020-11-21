#pragma once
#include "renderCommand.hpp"
#include "perspCam.hpp"
#include "shader.hpp"
#include "buffers.hpp"

namespace Engine
{
	class Renderer
	{
	public:
		static void init();//some things that need to be initiallized, need a graphics context, so they have to be initiallized after the graphics context has been created, thats why this function exists

		static void beginScene(perspectiveCamera& cam);//takes in a camera (viewprojMat)
		static void endScene();
		static void Flush();
		static void sub(const Ref_ptr<GLvertexArray> va, const Ref_ptr<shader> shader, const mat4& transform = mat4::identity());//stands for submit, gonna overload this, so that we can submit different things //submit a vertexArray and a shader

		inline static RendererAPI::API getAPI() { return RendererAPI::getAPI(); }
	private:
		struct sceneData
		{
			void init()
			{
				viewProjBuffer = globalBuffer::create(2 * sizeof(mat4), STATIC_DRAW);
				viewProjBuffer->lAddMat4();
				viewProjBuffer->lAddMat4();
				viewProjBuffer->bindToPoint(0);//binding point for viewProjectionBuffer always is 0
				viewProjBuffer->unbind();
			}
			mat4 viewMat;
			mat4 projMat;
			Ref_ptr<globalBuffer> viewProjBuffer;
		};
		static sceneData* s_sceneData;
	};
}