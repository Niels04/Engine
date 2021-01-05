#include "Engpch.hpp"
#include "renderer.hpp"

#include "material.hpp"

namespace Engine
{
	Renderer::sceneData* Renderer::s_sceneData = new Renderer::sceneData;
	shaderLib* Renderer::s_shaderLib = new shaderLib;
	//bool Renderer::s_drawLights = false;

	void Renderer::init()
	{
		renderCommand::init();
		materialLib::init();
		s_sceneData->init();//setup stuff like the globalBuffer for the view- and projection matrices and lights
	}

	void Renderer::beginScene(const perspectiveCamera& cam)
	{
		s_sceneData->viewMat = cam.getViewMat();
		s_sceneData->projMat = cam.getProjMat();
		//set uniformBuffers for view - and projection matrices here, they will be used by many shaders
		s_sceneData->viewProjBuffer->bind();
		s_sceneData->viewProjBuffer->updateElement(0, &mat4::transposed(s_sceneData->viewMat));//viewMat is at index 0
		s_sceneData->viewProjBuffer->updateElement(1, &mat4::transposed(s_sceneData->projMat));//projMat is at index 1
		s_sceneData->viewProjBuffer->unbind();
		//update dynamic lights
		s_sceneData->lightManager.updateLights();//updates lights for all shaders
		//render shadowMaps(make a function for this):
		//renderCommand::setViewport(Shadow_Width, Shadow_Height);//set the viewport to fit the resolution of the shadowMap
		//then for each light:
		//bindFrameBuffer
		//renderCommand::clearDepth();
		//doRenderingStuff
		//unbind frameBuffer

		//after all shadowMaps have been rendered:
		//renderCommand::setViewport(windowWidth, windowHeight);
	}

	void Renderer::beginScene(const OrthographicCamera& cam)
	{
		s_sceneData->viewMat = cam.getViewMat();
		s_sceneData->projMat = cam.getProjMat();
		//set uniformBuffers for view - and projection matrices here, they will be used by many shaders
		s_sceneData->viewProjBuffer->bind();
		s_sceneData->viewProjBuffer->updateElement(0, &mat4::transposed(s_sceneData->viewMat));//viewMat is at index 0
		s_sceneData->viewProjBuffer->updateElement(1, &mat4::transposed(s_sceneData->projMat));//projMat is at index 1
		s_sceneData->viewProjBuffer->unbind();
		//update dynamic lights
		s_sceneData->lightManager.updateLights();//updates lights for all shaders
		//render shadowMaps(make a function for this):
		//renderCommand::setViewport(Shadow_Width, Shadow_Height);//set the viewport to fit the resolution of the shadowMap
		//then for each light:
		//bindFrameBuffer
		//renderCommand::clearDepth();
		//doRenderingStuff
		//unbind frameBuffer

		//after all shadowMaps have been rendered:
		//renderCommand::setViewport(windowWidth, windowHeight);
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
		mat->bind(10);//start binding materials at slot 10 and reserve all slots below that for matrices and lights
		mat->getShader()->setUniformMat4("u_modelMat", Mesh->getModelMat());//set the modelMatrix in the shader(get it from the mesh)
		Ref_ptr<vertexArray> geometry = Mesh->getVa();
		geometry->bind();
		renderCommand::drawIndexed(geometry);
	}

	void Renderer::Flush()
	{

	}

	void Renderer::sceneData::init()
	{
		//initialize buffer for viewProjection
		viewProjBuffer = globalBuffer::create(2 * sizeof(mat4), DYNAMIC_DRAW);
		viewProjBuffer->lAddMat4B();
		viewProjBuffer->lAddMat4B();
		viewProjBuffer->bindToPoint(VIEWPROJ_BIND);//binding point for viewProjectionBuffer always is 0
		viewProjBuffer->unbind();
		//initialize lights
		lightManager.init();
	}
}