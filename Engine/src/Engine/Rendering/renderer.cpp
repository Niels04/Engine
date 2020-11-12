#include "Engpch.hpp"
#include "renderer.hpp"

namespace Engine
{
	Renderer::sceneData* Renderer::s_sceneData = new Renderer::sceneData;

	void Renderer::beginScene(perspectiveCamera& cam)//submit camera when beginning scene
	{
		s_sceneData->viewProjMat = cam.getViewProjMat();//cam.getViewProjMat();//copy the camera's viewProjMat, because it will be needed, to be uploaded to the shaders later
	}

	void Renderer::endScene()
	{
		
	}

	void Renderer::sub(std::shared_ptr<GLvertexArray> va, std::shared_ptr<shader> shader, const mat4& transform)
	{
		shader->bind();//bind the shader(!!!important befor uploading uniforms!!!)
		shader->setUniformMat4("u_viewProjMat", s_sceneData->viewProjMat);//set the viewProjMat as a uniform //probably gonna do that once per scene later and not for every submit
		shader->setUniformMat4("u_transformMat", transform);

		va->bind();//bind the vertexArray
		renderCommand::drawIndexed(va);//draw it
	}

	void Renderer::Flush()
	{

	}
}