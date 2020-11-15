#include "Engpch.hpp"
#include "renderer.hpp"

namespace Engine
{
	Renderer::sceneData* Renderer::s_sceneData = new Renderer::sceneData;

	void Renderer::init()
	{
		s_sceneData->init();
	}

	void Renderer::beginScene(perspectiveCamera& cam)//submit camera when beginning scene
	{
		s_sceneData->viewMat = cam.getViewMat();
		s_sceneData->projMat = cam.getProjMat();
		//set uniformBuffers for view - and projection matrices here, they will be used by many shaders
		s_sceneData->viewProjBuffer->bind();
		s_sceneData->viewProjBuffer->updateElement(0, &mat4::transposed(s_sceneData->viewMat));
		s_sceneData->viewProjBuffer->updateElement(1, &mat4::transposed(s_sceneData->projMat));
		s_sceneData->viewProjBuffer->unbind();
	}

	void Renderer::endScene()
	{
		
	}

	void Renderer::sub(std::shared_ptr<GLvertexArray> va, std::shared_ptr<shader> shader, const mat4& transform)
	{
		shader->bind();//bind the shader(!!!important to do before uploading uniforms!!!)
		shader->setUniformMat4("u_modelMat", transform);

		va->bind();//bind the vertexArray
		renderCommand::drawIndexed(va);//draw it
	}

	void Renderer::Flush()
	{

	}
}