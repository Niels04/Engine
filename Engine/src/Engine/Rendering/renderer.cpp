#include "Engpch.hpp"
#include "renderer.hpp"

namespace Engine
{
	Renderer::sceneData* Renderer::s_sceneData = new Renderer::sceneData;

	void Renderer::init()
	{
		renderCommand::init();
		s_sceneData->init();//setup stuff like the globalBuffer for the view- and projection matrices
	}

	void Renderer::beginScene(perspectiveCamera& cam)//submit camera when beginning scene
	{
		s_sceneData->viewMat = cam.getViewMat();
		s_sceneData->projMat = cam.getProjMat();
		//set uniformBuffers for view - and projection matrices here, they will be used by many shaders
		s_sceneData->viewProjBuffer->bind();
		s_sceneData->viewProjBuffer->updateElement(0, &mat4::transposed(s_sceneData->viewMat));//viewMat is at index 0
		s_sceneData->viewProjBuffer->updateElement(1, &mat4::transposed(s_sceneData->projMat));//projMat is at index 1
		s_sceneData->viewProjBuffer->unbind();
	}

	void Renderer::endScene()
	{
		
	}

	void Renderer::sub(Ref_ptr<GLvertexArray> va, Ref_ptr<shader> shader, const mat4& transform)
	{
		//there will be some kind of sorting algorythm, but for now just draw everything, that gets submited
		shader->bind();//bind the shader(!!!important to do before uploading uniforms!!!)
		shader->setUniformMat4("u_modelMat", transform);//this is set once per submited geometry, so upload it here

		va->bind();//bind the vertexArray
		renderCommand::drawIndexed(va);//draw it
	}

	void Renderer::Flush()
	{

	}
}