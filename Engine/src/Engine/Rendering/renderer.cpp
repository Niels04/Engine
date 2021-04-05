#include "Engpch.hpp"
#include "renderer.hpp"

#include "material.hpp"

#include "imgui.h"

namespace Engine
{
	Renderer::sceneData* Renderer::s_sceneData = new Renderer::sceneData;
	shaderLib* Renderer::s_shaderLib = new shaderLib;
	std::vector<Ref_ptr<mesh>> Renderer::s_renderQueue = {  };
	uint32_t Renderer::s_mainViewport[2] = { 0, 0 };
	uint32_t Renderer::s_maxViewportSize[2] = { 0, 0 };
	Ref_ptr<FrameBuffer> Renderer::s_hdr_fbo = nullptr;
	Ref_ptr<texture2d> Renderer::s_hdr_tex = nullptr;
	Ref_ptr<texture2d> Renderer::s_bright_tex = nullptr;
	Ref_ptr<texture2d> Renderer::s_pingPongTex[2] = { nullptr, nullptr };
	Ref_ptr<FrameBuffer> Renderer::s_pingPongFbo[2] = { nullptr, nullptr };
	Ref_ptr<RenderBuffer> Renderer::s_hdr_depthStencil = nullptr;
	Ref_ptr<vertexArray> Renderer::s_hdr_quad_va = nullptr;
	Ref_ptr<shader> Renderer::s_hdr_quad_shader = nullptr;
	Ref_ptr<shader> Renderer::s_hdr_quad_shader_bloom = nullptr;
	Ref_ptr<shader> Renderer::s_blur_shader = nullptr;
	bool Renderer::s_bloom = false;
	//bool Renderer::s_drawLights = false;

	void Renderer::init(uint32_t viewportWidth, uint32_t viewportHeight, uint32_t maxViewportWidth, uint32_t maxViewportHeight/*this is the resolution that everything is rendered at*/)
	{
		s_mainViewport[0] = viewportWidth; s_mainViewport[1] = viewportHeight;
		s_maxViewportSize[0] = maxViewportWidth; s_maxViewportSize[1] = maxViewportHeight;
		renderCommand::init();
		materialLib::init();
		s_sceneData->init();//setup stuff like the globalBuffer for the view- and projection matrices and lights

		s_hdr_tex = Engine::texture2d::create(s_maxViewportSize[0], s_maxViewportSize[1], ENG_RGBA16F);
		s_bright_tex = Engine::texture2d::create(s_maxViewportSize[0], s_maxViewportSize[1], ENG_RGBA16F);
		s_hdr_depthStencil = RenderBuffer::create(RenderBufferUsage::DEPTH_STENCIL, s_maxViewportSize[0], s_maxViewportSize[1]);
		s_hdr_fbo = FrameBuffer::create();
		s_hdr_fbo->bind();
		s_hdr_fbo->attachRenderBuffer(s_hdr_depthStencil);//attach a depth&stenicl-buffer
		s_hdr_fbo->attachTexture(s_hdr_tex);//attach a color-buffer
		s_hdr_fbo->attachTexture(s_bright_tex, 1);//attach the bloom-texture to the second color-buffer
		renderCommand::drawToBuffers(2, ENG_COLOR_ATTACHMENT0, ENG_COLOR_ATTACHMENT1);
		s_hdr_fbo->checkStatus();
		s_hdr_fbo->unbind();
		//setup the pingPong textures and fbo for 
		s_pingPongTex[0] = texture2d::create(s_maxViewportSize[0], s_maxViewportSize[1], ENG_RGBA16F);
		s_pingPongTex[1] = texture2d::create(s_maxViewportSize[0], s_maxViewportSize[1], ENG_RGBA16F);
		s_pingPongFbo[0] = FrameBuffer::create();
		s_pingPongFbo[0]->bind();
		s_pingPongFbo[0]->attachTexture(s_pingPongTex[0]);
		s_pingPongFbo[1] = FrameBuffer::create();
		s_pingPongFbo[1]->bind();
		s_pingPongFbo[1]->attachTexture(s_pingPongTex[1]);
		s_pingPongFbo[1]->unbind();
		//now setup the screen-space-quad
		const float vb[] = { -1.0f, -1.0f, 0.0f, 0.0f,
			-1.0f, 1.0f, 0.0f, 1.0f,
			1.0f, 1.0f, 1.0f, 1.0f,
			1.0f, -1.0f, 1.0f, 0.0f };
		Ref_ptr<vertexBuffer> quad_vb = vertexBuffer::create(sizeof(vb), (void*)vb);
		Ref_ptr<vertexBufferLayout> quad_layout = vertexBufferLayout::create();
		quad_layout->pushFloat(2U);//position
		quad_layout->pushFloat(2U);//texCoord
		quad_vb->setLayout(quad_layout);
		const uint32_t indices[] = { 0U, 2U, 1U, 0U, 3U, 2U };
		Ref_ptr<indexBuffer> quad_ib = indexBuffer::create(sizeof(indices)/sizeof(uint32_t), indices);
		s_hdr_quad_va = vertexArray::create();
		s_hdr_quad_va->addBuffer(quad_vb);
		s_hdr_quad_va->addBuffer(quad_ib);
		s_hdr_quad_shader = shader::create("hdr/hdr_quad_shader.shader");
		s_hdr_quad_shader_bloom = shader::create("hdr/hdr_quad_shader_bloom.shader");
		s_blur_shader = shader::create("blur/gaussian_blur.shader");
	}

	void Renderer::shutdown()
	{
		//assuming this is the only strong reference to these pointers
		s_hdr_tex.reset();
		s_bright_tex.reset();
		s_pingPongTex[0].reset();
		s_pingPongTex[1].reset();
		s_pingPongFbo[0].reset();
		s_pingPongFbo[1].reset();
		s_hdr_depthStencil.reset();
		s_hdr_fbo.reset();
		s_hdr_quad_va.reset();
		s_hdr_quad_shader.reset();
		s_hdr_quad_shader_bloom.reset();
		s_blur_shader.reset();
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
		//dissable blending on the first pass
		renderCommand::setViewport(s_maxViewportSize[0], s_maxViewportSize[1]);
		s_hdr_fbo->bind();
		bool first_pass = true;//after the first pass, we want to enable additive blending
		renderCommand::clear();
		s_hdr_tex->clear({ 0.0171f, 0.0171f, 0.0171f, 1.0f });
		for (auto it = s_renderQueue.begin(); it != s_renderQueue.end(); it++)
		{
			const auto Mesh = *it;
			if (!(Mesh->getMaterial()->getFlags() & flag_light_infl))//only render meshes whose appearance isn't influenced by lighting
			{
				Ref_ptr<material> mat = Mesh->getMaterial();
				/*if (mat->getFlags() & flag_depth_test) //::::::::::::::::::::IN ORDER TO ENABLE THIS FEATURE, WE WOULD HAVE TO DETECT THE FIRST DRAW OF A MESH -> implement this when no additive blending is used anymore
					renderCommand::setDepth(ENG_LESS);
				else
					renderCommand::setDepth(ENG_ALWAYS);*/
				renderCommand::enableCullFace(!(mat->getFlags() & flag_no_backface_cull));//dissable face culling if the object has no backface-culling enabled
				mat->bind(10);
				mat->getShader()->setUniformMat4("u_modelMat", Mesh->getModelMat(), 1);
				Ref_ptr<vertexArray> geometry = Mesh->getVa();
				geometry->bind();
				renderCommand::drawIndexed(geometry);
			}
		}
		uint32_t index = 0;
		auto& dirLightMap = s_sceneData->lightManager.dirLightMaps();
		for (uint8_t i = 0; i < s_sceneData->lightManager.getDirLightCount(); i++, dirLightMap++)
		{
			(*dirLightMap)->bind(8);
			for (const auto& Mesh : s_renderQueue)
			{
				if (Mesh->getMaterial()->getFlags() & flag_light_infl)//only render meshes whose appearance isn influenced by lighting
				{
					Ref_ptr<material> mat = Mesh->getMaterial();
					/*if (mat->getFlags() & flag_depth_test)
						renderCommand::setDepth(ENG_LESS);
					else
						renderCommand::setDepth(ENG_ALWAYS);*/
					renderCommand::enableCullFace(!(mat->getFlags() & flag_no_backface_cull));//dissable face culling if the object has no backface-culling enabled
					mat->bind(10);//start binding materials at globalBuffer-slot 10 and reserve all slots below that for matrices and lights
					mat->getShader()->setUniformMat4("u_modelMat", Mesh->getModelMat(), 1);
					mat->getShader()->setUniformMat3("u_normalMat", Mesh->getNormalMat(), 1);
					mat->getShader()->setUniform1i("u_shadowMap", 8);
					mat->getShader()->setUniform1ui("u_lightIndex", index);
					Ref_ptr<vertexArray> geometry = Mesh->getVa();
					geometry->bind();
					renderCommand::drawIndexed(geometry);
				}
			}
			index++;
			if (first_pass)
			{
				renderCommand::setBlend(ENG_ONE, ENG_ONE);//enable additive blending
				renderCommand::setDepth(ENG_EQUAL);
				first_pass = false;
			}
		}
		index = 0;
		auto& pointLightMap = s_sceneData->lightManager.pointLightMaps();
		for (uint8_t i = 0; i < s_sceneData->lightManager.getPointLightCount(); i++, pointLightMap++)
		{
			(*pointLightMap)->bind(8);
			for (const auto& Mesh : s_renderQueue)
			{
				if (Mesh->getMaterial()->getFlags() & flag_light_infl)//only render meshes whose appearance isn influenced by lighting
				{
					Ref_ptr<material> mat = Mesh->getMaterial();
					/*if (mat->getFlags() & flag_depth_test)
						renderCommand::setDepth(ENG_LESS);
					else
						renderCommand::setDepth(ENG_ALWAYS);*/
					renderCommand::enableCullFace(!(mat->getFlags() & flag_no_backface_cull));//dissable face culling if the object has no backface-culling enabled
					mat->bind(10, 1);//start binding materials at globalBuffer-slot 10 and reserve all slots below that for matrices and lights
					mat->getShader(1)->setUniformMat4("u_modelMat", Mesh->getModelMat(), 1);
					mat->getShader()->setUniformMat3("u_normalMat", Mesh->getNormalMat(), 1);
					mat->getShader(1)->setUniform1i("u_shadowMap", 8);
					mat->getShader(1)->setUniform1ui("u_lightIndex", index);
					Ref_ptr<vertexArray> geometry = Mesh->getVa();
					geometry->bind();
					renderCommand::drawIndexed(geometry);
				}
			}
			index++;
			if (first_pass)
			{
				renderCommand::setBlend(ENG_ONE, ENG_ONE);//enable additive blending
				renderCommand::setDepth(ENG_EQUAL);
				first_pass = false;
			}
		}
		index = 0;
		auto& spotLightMap = s_sceneData->lightManager.spotLightMaps();
		for (uint8_t i = 0; i < s_sceneData->lightManager.getSpotLightCount(); i++, spotLightMap++)
		{
			(*spotLightMap)->bind(8);
			for (const auto& Mesh : s_renderQueue)
			{
				if (Mesh->getMaterial()->getFlags() & flag_light_infl)//only render meshes whose appearance isn influenced by lighting
				{
					Ref_ptr<material> mat = Mesh->getMaterial();
					/*if (mat->getFlags() & flag_depth_test)
						renderCommand::setDepth(ENG_LESS);
					else
						renderCommand::setDepth(ENG_ALWAYS);*/
					renderCommand::enableCullFace(!(mat->getFlags() & flag_no_backface_cull));//dissable face culling if the object has no backface-culling enabled
					mat->bind(10, 2);//start binding materials at globalBuffer-slot 10 and reserve all slots below that for matrices and lights
					mat->getShader(2)->setUniformMat4("u_modelMat", Mesh->getModelMat(), 1);
					mat->getShader()->setUniformMat3("u_normalMat", Mesh->getNormalMat(), 1);
					mat->getShader(2)->setUniform1i("u_shadowMap", 8);
					mat->getShader(2)->setUniform1ui("u_lightIndex", index);
					Ref_ptr<vertexArray> geometry = Mesh->getVa();
					geometry->bind();
					renderCommand::drawIndexed(geometry);
				}
			}
			index++;
			if (first_pass)
			{
				renderCommand::setBlend(ENG_ONE, ENG_ONE);//enable additive blending
				renderCommand::setDepth(ENG_EQUAL);
				first_pass = false;
			}
		}
		s_renderQueue.clear();

		s_hdr_fbo->unbind();
		if (s_bloom)
		{
			s_blur_shader->bind();
			s_blur_shader->setUniform1i("u_texture", 0);
			s_pingPongFbo[0]->bind();
			renderCommand::clear();
			s_pingPongFbo[1]->bind();
			renderCommand::clear();
			bool horizontal = true, first_it = true;
			uint8_t blurCount = 4;
			for (uint8_t i = 0; i < blurCount; i++)
			{
				s_pingPongFbo[horizontal]->bind();
				s_blur_shader->setUniform1b("horizontal", horizontal);
				if (first_it)
					s_bright_tex->bind(0);
				else
					s_pingPongTex[!horizontal]->bind(0);
				s_hdr_quad_va->bind();
				renderCommand::drawIndexed(s_hdr_quad_va);
				s_hdr_quad_va->unbind();
				horizontal = !horizontal;
				if (first_it)
					first_it = false;
			}
			s_pingPongFbo[1]->unbind();
		}
		//render to the screen-space quad:
		renderCommand::setViewport(s_mainViewport[0], s_mainViewport[1]);
		renderCommand::clear();
		renderCommand::setDepth(ENG_ALWAYS);
		renderCommand::setBlend(ENG_ONE, ENG_ZERO);
		s_hdr_tex->bind(0);
		if (s_bloom)
		{
			s_hdr_quad_shader_bloom->bind();
			s_hdr_quad_shader_bloom->setUniform1i("u_hdr", 0);
			s_hdr_quad_shader_bloom->setUniform1i("u_bloom_blur", 1);
			s_pingPongTex[1]->bind(1);
		}
		else
		{
			s_hdr_quad_shader->bind();
			s_hdr_quad_shader->setUniform1i("u_texture", 0);
		}
		s_hdr_quad_va->bind();
		renderCommand::drawIndexed(s_hdr_quad_va);
		s_hdr_quad_va->unbind();
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
		renderCommand::enableCullFace(true);
		renderCommand::cullFace(ENG_FRONT);
		renderCommand::setViewport(Shadow_Width, Shadow_Height);
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
				if (Mesh->getMaterial()->getFlags() & flag_cast_shadow)//only render a mesh to a depth-map, if it's light_interaction-flag is set
				{
					depthMapShader_dir->setUniformMat4("u_modelMat", Mesh->getModelMat(), 1);
					Ref_ptr<vertexArray> geometry = Mesh->getVa();
					geometry->bind();
					renderCommand::drawIndexed(geometry);
				}
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
				if (Mesh->getMaterial()->getFlags() & flag_cast_shadow)//only render a mesh to a depth-map, if it's light_interaction-flag is set
				{
					depthMapShader_point->setUniformMat4("u_modelMat", Mesh->getModelMat(), 1);
					Ref_ptr<vertexArray> geometry = Mesh->getVa();
					geometry->bind();
					renderCommand::drawIndexed(geometry);
				}
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
				if (Mesh->getMaterial()->getFlags() & flag_cast_shadow)//only render a mesh to a depth-map, if it's light_interaction-flag is set
				{
					depthMapShader_spot->setUniformMat4("u_modelMat", Mesh->getModelMat(), 1);
					Ref_ptr<vertexArray> geometry = Mesh->getVa();
					geometry->bind();
					renderCommand::drawIndexed(geometry);
				}
			}
		}
		depthMapShader_spot->unbind();
		frameBuffer->unbind();
		renderCommand::cullFace(ENG_BACK);
		renderCommand::setViewport(s_mainViewport[0], s_mainViewport[1]);
	}

	void Renderer::onImGuiRender()
	{
		ImGui::Begin("Rendering options");
		ImGui::Checkbox("bloom", &s_bloom);
		ImGui::End();
	}
}