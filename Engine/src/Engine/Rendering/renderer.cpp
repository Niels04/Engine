#include "Engpch.hpp"
#include "renderer.hpp"

#include "material.hpp"

namespace Engine
{
	Renderer::sceneData* Renderer::s_sceneData = new Renderer::sceneData;
	shaderLib* Renderer::s_shaderLib = new shaderLib;

	void Renderer::init()
	{
		renderCommand::init();
		materialLib::init();
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

	void Renderer::sub(const Ref_ptr<vertexArray> va, const Ref_ptr<shader> shader, const mat4& transform)
	{
		//there will be some kind of sorting algorythm, but for now just draw everything, that gets submited
		shader->bind();//bind the shader(!!!important to do before uploading uniforms!!!)
		shader->setUniformMat4("u_modelMat", transform);//this is set once per submited geometry, so upload it here

		va->bind();//bind the vertexArray
		renderCommand::drawIndexed(va);//draw it
	}
	
	void Renderer::sub(const Ref_ptr<mesh> Mesh)
	{
		//there will be some kind of sorting algorythm, but for now just draw everything, that gets submited
		Ref_ptr<material> mat = Mesh->getMaterial();
		mat->bind(1);//always bind materials to slot 1 for now because slot 0 is always used for viewProjMats
		mat->getShader()->setUniformMat4("u_modelMat", Mesh->getModelMat());//set the modelMatrix in the shader(get it from the mesh)
		Ref_ptr<vertexArray> geometry = Mesh->getVa();
		geometry->bind();
		renderCommand::drawIndexed(geometry);
	}

	void Renderer::Flush()
	{

	}
}