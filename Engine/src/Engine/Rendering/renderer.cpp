#include "Engpch.hpp"
#include "renderer.hpp"

#include "material.hpp"

namespace Engine
{
	Renderer::sceneData* Renderer::s_sceneData = new Renderer::sceneData;
	shaderLib* Renderer::s_shaderLib = new shaderLib;
	std::vector<Ref_ptr<mesh>> Renderer::s_renderQueue = {  };
	uint32_t Renderer::s_mainViewport[2] = { 0, 0 };
	//bool Renderer::s_drawLights = false;

	void Renderer::init(uint32_t viewportWidth, uint32_t viewportHeight)
	{
		s_mainViewport[0] = viewportWidth; s_mainViewport[1] = viewportHeight;
		renderCommand::init();
		materialLib::init();
		s_sceneData->init();//setup stuff like the globalBuffer for the view- and projection matrices and lights
	}

	//maybe make this a templated function that eigther takes a perspective or an orthographic camera
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
	}

	void Renderer::endScene()
	{
		
	}

	void Renderer::sub(const Ref_ptr<vertexArray> va, const Ref_ptr<shader> shader, const mat4& transform)
	{
		ENG_CORE_WARN("Using deprecated \"Renderer::sub(const Ref_ptr<vertexArray> va, const Ref_ptr<shader> shader, const mat4& transform)\".");
		//there will be some kind of sorting algorythm, but for now just draw everything, that gets submited
		shader->bind();//bind the shader(!!!important to do before uploading uniforms!!!)
		shader->setUniformMat4("u_modelMat", transform);//this is set once per submited geometry, so upload it here

		va->bind();//bind the vertexArray
		renderCommand::drawIndexed(va);//draw it
	}
	
	void Renderer::sub(const Ref_ptr<mesh> Mesh)
	{
		s_renderQueue.push_back(Mesh);
	}

	void Renderer::Flush()
	{
		//there will be some kind of sorting algorythm, but for now just draw everything, that got submited
		//::::::::I ACTUALLY THINK I HAVE TO DO SOMETHING IN ORDER TO PREVENT PIXELS IN THE BACK FROM CONTRIBUTING TO THE FINAL OUTPUT, SINCE THEIR COLOR WOULD JUST BE ADDED LIKE THIS
		//dissable blending on the first pass
		renderCommand::clear();
		uint32_t index = 0;
		auto& dirLightMap = s_sceneData->lightManager.dirLightMaps();
		if (s_sceneData->lightManager.getDirLightCount())
		{
			(*dirLightMap)->bind(8);
			for (const auto& Mesh : s_renderQueue)
			{
				Ref_ptr<material> mat = Mesh->getMaterial();
				mat->bind(10);//start binding materials at slot 10 and reserve all slots below that for matrices and lights
				mat->getShader()->setUniformMat4("u_modelMat", Mesh->getModelMat(), 1);
				mat->getShader()->setUniformMat3("u_normalMat", Mesh->getNormalMat(), 1);
				mat->getShader()->setUniform1i("u_shadowMap", 8);
				mat->getShader()->setUniform1ui("u_lightIndex", index);
				Ref_ptr<vertexArray> geometry = Mesh->getVa();
				geometry->bind();
				renderCommand::drawIndexed(geometry);
			}
			index++;
			dirLightMap++;
		}
		renderCommand::setBlend(ENG_ONE, ENG_ONE);//enable additive blending
		renderCommand::setDepth(ENG_EQUAL);
		for (uint8_t i = 1; i < s_sceneData->lightManager.getDirLightCount(); i++, dirLightMap++)
		{
			(*dirLightMap)->bind(8);
			for (const auto& Mesh : s_renderQueue)
			{
				Ref_ptr<material> mat = Mesh->getMaterial();
				mat->bind(10);//start binding materials at globalBuffer-slot 10 and reserve all slots below that for matrices and lights
				mat->getShader()->setUniformMat4("u_modelMat", Mesh->getModelMat(), 1);
				mat->getShader()->setUniformMat3("u_normalMat", Mesh->getNormalMat(), 1);
				mat->getShader()->setUniform1i("u_shadowMap", 8);
				mat->getShader()->setUniform1ui("u_lightIndex", index);
				Ref_ptr<vertexArray> geometry = Mesh->getVa();
				geometry->bind();
				renderCommand::drawIndexed(geometry);
			}
			index++;
		}
		index = 0;
		auto& pointLightMap = s_sceneData->lightManager.pointLightMaps();
		for (uint8_t i = 0; i < s_sceneData->lightManager.getPointLightCount(); i++, pointLightMap++)
		{
			(*pointLightMap)->bind(8);
			for (const auto& Mesh : s_renderQueue)
			{
				Ref_ptr<material> mat = Mesh->getMaterial();
				mat->bind(10, 1);//start binding materials at globalBuffer-slot 10 and reserve all slots below that for matrices and lights
				mat->getShader(1)->setUniformMat4("u_modelMat", Mesh->getModelMat(), 1);
				mat->getShader()->setUniformMat3("u_normalMat", Mesh->getNormalMat(), 1);
				mat->getShader(1)->setUniform1i("u_shadowMap", 8);
				mat->getShader(1)->setUniform1ui("u_lightIndex", index);
				Ref_ptr<vertexArray> geometry = Mesh->getVa();
				geometry->bind();
				renderCommand::drawIndexed(geometry);
			}
			index++;
		}
		index = 0;
		auto& spotLightMap = s_sceneData->lightManager.spotLightMaps();
		for (uint8_t i = 0; i < s_sceneData->lightManager.getSpotLightCount(); i++, spotLightMap++)
		{
			(*spotLightMap)->bind(8);
			for (const auto& Mesh : s_renderQueue)
			{
				Ref_ptr<material> mat = Mesh->getMaterial();
				mat->bind(10, 2);//start binding materials at globalBuffer-slot 10 and reserve all slots below that for matrices and lights
				mat->getShader(2)->setUniformMat4("u_modelMat", Mesh->getModelMat(), 1);
				mat->getShader()->setUniformMat3("u_normalMat", Mesh->getNormalMat(), 1);
				mat->getShader(2)->setUniform1i("u_shadowMap", 8);
				mat->getShader(2)->setUniform1ui("u_lightIndex", index);
				Ref_ptr<vertexArray> geometry = Mesh->getVa();
				geometry->bind();
				renderCommand::drawIndexed(geometry);
			}
			index++;
		}
		s_renderQueue.clear();
		renderCommand::setDepth(ENG_LESS);
		renderCommand::setBlend(ENG_SRC_ALPHA, ENG_ONE_MINUS_SRC_ALPHA);//set blending back to normal
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

	void Renderer::RenderDepthMaps()
	{
		renderCommand::setViewport(Shadow_Width, Shadow_Height);
		renderCommand::cullFace(ENG_FRONT);
		WeakRef_ptr<FrameBuffer> frameBuffer = s_sceneData->lightManager.getDepthFB();
		frameBuffer->bind();
		WeakRef_ptr<shader> depthMapShader_dir = s_sceneData->lightManager.getDepthShader_dir();
		depthMapShader_dir->bind();
		//render to depthMaps:
		auto& dirLightMap = s_sceneData->lightManager.dirLightMaps();//retrieve an iterator to the begin of the dirLightShadowMaps
		for (auto& it = s_sceneData->lightManager.dirLightBegin(); it != s_sceneData->lightManager.dirLightEnd(); ++it, dirLightMap++)
		{
			frameBuffer->attachTexture(*dirLightMap);
			renderCommand::clearDepth();
			const auto& data = *it;
			depthMapShader_dir->setUniformMat4("u_toLightSpace", data.it->second);
			for (const auto& Mesh : s_renderQueue)
			{
				depthMapShader_dir->setUniformMat4("u_modelMat", Mesh->getModelMat(), 1);
				Ref_ptr<vertexArray> geometry = Mesh->getVa();
				geometry->bind();
				renderCommand::drawIndexed(geometry);
			}
		}
		depthMapShader_dir->unbind();
		WeakRef_ptr<shader> depthMapShader_point = s_sceneData->lightManager.getDepthShader_point();
		depthMapShader_point->bind();
		auto& pointLightMap = s_sceneData->lightManager.pointLightMaps();
		for (auto& it = s_sceneData->lightManager.pointLightBegin(); it != s_sceneData->lightManager.pointLightEnd(); ++it, pointLightMap++)
		{
			frameBuffer->attachTexture(*pointLightMap);
			renderCommand::clearDepth();
			const auto& data = *it;
			depthMapShader_point->setUniformMat4_6("u_toLightSpace", data.it->second);
			depthMapShader_point->setUniform4f("u_lightPos", data.it->first.position);
			for (const auto& Mesh : s_renderQueue)
			{
				depthMapShader_point->setUniformMat4("u_modelMat", Mesh->getModelMat(), 1);
				Ref_ptr<vertexArray> geometry = Mesh->getVa();
				geometry->bind();
				renderCommand::drawIndexed(geometry);
			}
		}
		depthMapShader_point->unbind();
		WeakRef_ptr<shader> depthMapShader_spot = s_sceneData->lightManager.getDepthShader_spot();
		depthMapShader_spot->bind();
		auto& spotLightMap = s_sceneData->lightManager.spotLightMaps();
		for (auto& it = s_sceneData->lightManager.spotLightBegin(); it != s_sceneData->lightManager.spotLightEnd(); ++it, spotLightMap++)
		{
			frameBuffer->attachTexture(*spotLightMap);
			renderCommand::clearDepth();
			const auto& data = *it;
			depthMapShader_spot->setUniformMat4("u_toLightSpace", data.it->second);
			for (const auto& Mesh : s_renderQueue)
			{
				depthMapShader_spot->setUniformMat4("u_modelMat", Mesh->getModelMat(), 1);
				Ref_ptr<vertexArray> geometry = Mesh->getVa();
				geometry->bind();
				renderCommand::drawIndexed(geometry);
			}
		}
		depthMapShader_spot->unbind();
		frameBuffer->unbind();
		renderCommand::cullFace(ENG_BACK);
		renderCommand::setViewport(s_mainViewport[0], s_mainViewport[1]);
	}
}