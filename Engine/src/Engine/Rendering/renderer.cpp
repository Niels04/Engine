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
	Ref_ptr<RenderBuffer> Renderer::s_hdr_depth = nullptr;
	Ref_ptr<vertexArray> Renderer::s_hdr_quad_va = nullptr;
	Ref_ptr<shader> Renderer::s_hdr_quad_shader = nullptr;
	Ref_ptr<shader> Renderer::s_hdr_quad_shader_bloom = nullptr;
	Ref_ptr<shader> Renderer::s_blur_shader[2] = { nullptr, nullptr };
	bool Renderer::s_bloom = false;
	float Renderer::s_exposure = 1.0f;
	//bool Renderer::s_drawLights = false;
	//G-Buffer:::::::::::
	Ref_ptr<FrameBuffer> Renderer::s_gBuffer = nullptr;
	Ref_ptr<texture2d> Renderer::s_gPosition = nullptr;
	Ref_ptr<texture2d> Renderer::s_gNormal = nullptr;
	Ref_ptr<texture2d> Renderer::s_gAlbSpec = nullptr;
	Ref_ptr<RenderBuffer> Renderer::s_gDepth = nullptr;
	Ref_ptr<shader> Renderer::s_gLightingShader[2] = { nullptr, nullptr };
	//SSAO
	Ref_ptr<texture2d> Renderer::s_SSAONoise = nullptr;
	Ref_ptr<FrameBuffer> Renderer::s_SSAO_FBO = nullptr;
	Ref_ptr<texture2d> Renderer::s_AO = nullptr;
	Ref_ptr<shader> Renderer::s_SSAO_shader = nullptr;
	Ref_ptr<shader> Renderer::s_debug_display_ssao = nullptr;
	//skybox
	Ref_ptr<vertexArray> Renderer::s_skyboxVa = nullptr;
	Ref_ptr<shader> Renderer::s_skyboxShader = nullptr;
	WeakRef_ptr<texture> Renderer::s_skyboxTex = nullptr;
	//other
	/*Ref_ptr<shader> Renderer::s_debug_display_cascaded = nullptr;
	Ref_ptr<vertexArray> Renderer::s_debug_cascade_va = nullptr;
	Ref_ptr<shader> Renderer::s_debug_renderCascade = nullptr;*/

	float lerp(float a, float b, float f)
	{
		return a + f * (b - a);
	}

	void Renderer::init(uint32_t viewportWidth, uint32_t viewportHeight, uint32_t maxViewportWidth, uint32_t maxViewportHeight/*this is the resolution that everything is rendered at*/)
	{
		s_mainViewport[0] = viewportWidth; s_mainViewport[1] = viewportHeight;
		s_maxViewportSize[0] = maxViewportWidth; s_maxViewportSize[1] = maxViewportHeight;
		renderCommand::init();
		materialLib::init();
		s_sceneData->init();//setup stuff like the globalBuffer for the view- and projection matrices and lights

		s_hdr_tex = Engine::texture2d::create(s_maxViewportSize[0], s_maxViewportSize[1], ENG_RGBA16F);
		s_bright_tex = Engine::texture2d::create(s_maxViewportSize[0], s_maxViewportSize[1], ENG_RGBA16F, FILTER_LINEAR, FILTER_LINEAR, ENG_CLAMP_TO_EDGE);//bloom texture should not repeat!
		s_hdr_depth = RenderBuffer::create(RenderBufferUsage::DEPTH, s_maxViewportSize[0], s_maxViewportSize[1]);
		s_hdr_fbo = FrameBuffer::create();
		s_hdr_fbo->bind();
		s_hdr_fbo->attachRenderBuffer(s_hdr_depth);//attach a depth-buffer
		s_hdr_fbo->attachTexture(s_hdr_tex);//attach a color-buffer
		s_hdr_fbo->attachTexture(s_bright_tex, 1);//attach the bloom-texture to the second color-buffer
		renderCommand::drawToBuffers(2, ENG_COLOR_ATTACHMENT0, ENG_COLOR_ATTACHMENT1);
		s_hdr_fbo->checkStatus();
		s_hdr_fbo->unbind();
		//setup the pingPong textures and fbo for 
		s_pingPongTex[0] = texture2d::create(s_maxViewportSize[0] / 2, s_maxViewportSize[1] / 2, ENG_RGBA16F, FILTER_LINEAR, FILTER_LINEAR, ENG_CLAMP_TO_EDGE);
		s_pingPongTex[1] = texture2d::create(s_maxViewportSize[0] / 2, s_maxViewportSize[1] / 2, ENG_RGBA16F, FILTER_LINEAR, FILTER_LINEAR, ENG_CLAMP_TO_EDGE);
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
		s_hdr_quad_va->unbind();//this is only for testing
		s_hdr_quad_shader = shader::create("hdr/hdr_quad_shader_exposure.shader");
		s_hdr_quad_shader->bind();
		s_hdr_quad_shader->setUniform1i("u_hdr", 0);
		s_hdr_quad_shader->unbind();
		s_hdr_quad_shader_bloom = shader::create("hdr/hdr_quad_shader_bloom_exposure.shader");
		s_hdr_quad_shader_bloom->bind();
		s_hdr_quad_shader_bloom->setUniform1i("u_hdr", 0);
		s_hdr_quad_shader_bloom->setUniform1i("u_bloom_blur", 1);
		s_hdr_quad_shader_bloom->unbind();
		//horizontal is at index 1 and vertical is at index 0
		s_blur_shader[1] = shader::create("blur/gaussian_blur_bilinear_hor.shader");
		s_blur_shader[1]->bind();
		s_blur_shader[1]->setUniform1i("u_texture", 0);
		s_blur_shader[1]->unbind();
		s_blur_shader[0] = shader::create("blur/gaussian_blur_bilinear_ver.shader");
		s_blur_shader[0]->bind();
		s_blur_shader[0]->setUniform1i("u_texture", 0);
		s_blur_shader[0]->unbind();
		
		////////
		//SETUP THE G-BUFFER
		////////
		////LAYOUT:
		//- s_gPosition: RGBA 32bit non normalized floats that store: x = ?, y = ?, z = fragmentZViewSpace, w = ambCoefficient
		//- s_gNormal: RGBA 16bit non normalized floats that store: x = normal.x, y = normal.y, z = emissiveMultiplier, w = shininess * sign(normal.z) -> w stores the shininess multiplied with the sign of normal.z
		//- s_gAlbSpec: RGBA 8bit normalized floats that store: x = alb.r, y = alb.g, z = alb.b, w = specCoefficient
		//- s_gDepth: Renderbuffer that is attached as depthBuffer
		s_gBuffer = FrameBuffer::create();
		s_gBuffer->bind();
		renderCommand::drawToBuffers(3, ENG_COLOR_ATTACHMENT0, ENG_COLOR_ATTACHMENT1, ENG_COLOR_ATTACHMENT2);
		s_gPosition = texture2d::create(s_maxViewportSize[0], s_maxViewportSize[1], ENG_RGBA32F);
		s_gNormal = texture2d::create(s_maxViewportSize[0], s_maxViewportSize[1], ENG_RGBA16F);
		s_gAlbSpec = texture2d::create(s_maxViewportSize[0], s_maxViewportSize[1], ENG_RGBA8);
		s_gDepth = RenderBuffer::create(RenderBufferUsage::DEPTH, s_maxViewportSize[0], s_maxViewportSize[1]);
		s_gBuffer->attachTexture(s_gPosition, 0);
		s_gBuffer->attachTexture(s_gNormal, 1);
		s_gBuffer->attachTexture(s_gAlbSpec, 2);
		s_gBuffer->attachRenderBuffer(s_gDepth);
		s_gBuffer->checkStatus();

		s_gLightingShader[0] = shader::create("deferred/lighting_pass/shadow/cascaded/normal_compression/hardware_pcf/lighting.shader");
		s_gLightingShader[0]->bind();
		s_gLightingShader[0]->bindUniformBlock("ViewProjection", VIEWPROJ_BIND);
		s_gLightingShader[0]->bindUniformBlock("directionalLights", DIRECTIONAL_LIGHTS_BIND);
		s_gLightingShader[0]->bindUniformBlock("pointLights", POINT_LIGHTS_BIND);
		s_gLightingShader[0]->bindUniformBlock("spotLights", SPOT_LIGHTS_BIND);
		s_gLightingShader[0]->setUniform1i("u_gPosition", 0);
		s_gLightingShader[0]->setUniform1i("u_gNormal", 1);
		s_gLightingShader[0]->setUniform1i("u_gAlbSpec", 2);
		s_gLightingShader[0]->setUniform1i("u_dirShadowMaps", 3);
		s_gLightingShader[0]->setUniform1i("u_pointShadowMaps", 4);
		s_gLightingShader[0]->setUniform1i("u_spotShadowMaps", 5);
		s_gLightingShader[0]->setUniform1i("u_skybox", 6);
		s_gLightingShader[0]->unbind();
		
		s_gLightingShader[1] = shader::create("deferred/lighting_pass/shadow/cascaded/normal_compression/hardware_pcf/lighting_bloom.shader");
		s_gLightingShader[1]->bind();
		s_gLightingShader[1]->bindUniformBlock("ViewProjection", VIEWPROJ_BIND);
		s_gLightingShader[1]->bindUniformBlock("directionalLights", DIRECTIONAL_LIGHTS_BIND);
		s_gLightingShader[1]->bindUniformBlock("pointLights", POINT_LIGHTS_BIND);
		s_gLightingShader[1]->bindUniformBlock("spotLights", SPOT_LIGHTS_BIND);
		s_gLightingShader[1]->setUniform1i("u_gPosition", 0);
		s_gLightingShader[1]->setUniform1i("u_gNormal", 1);
		s_gLightingShader[1]->setUniform1i("u_gAlbSpec", 2);
		s_gLightingShader[1]->setUniform1i("u_dirShadowMaps", 3);
		s_gLightingShader[1]->setUniform1i("u_pointShadowMaps", 4);
		s_gLightingShader[1]->setUniform1i("u_spotShadowMaps", 5);
		s_gLightingShader[1]->setUniform1i("u_skybox", 6);
		s_gLightingShader[1]->unbind();

		//setup SSAO
		{
			std::uniform_real_distribution<float> randomFloats(0.0f, 1.0f);//random Floats between 0 and 1
			std::default_random_engine generator;
			vec3 ssaoKernel[SSAO_kernelSize];
			for (uint8_t i = 0; i < SSAO_kernelSize; i++)
			{
				vec3 sample(randomFloats(generator) * 2.0f - 1.0f, randomFloats(generator) * 2.0f - 1.0f, randomFloats(generator));//we want the vector's z-component to always be oriented in the positive z-direction,
				//because in tangent-space that is the same as the normal
				sample.normalize();
				sample *= randomFloats(generator);
				float scale = static_cast<float>(i) / static_cast<float>(SSAO_kernelSize);
				scale = lerp(0.1f, 1.0f, scale * scale);//we want most samples to be closer to the origin -> this function achieves exactly that
				sample *= scale;
				ssaoKernel[i] = sample;
			}

			vec3 ssaoNoise[16];
			for (uint8_t i = 0; i < 16; i++)
			{
				vec3 noise = { randomFloats(generator) * 2.0f - 1.0f, randomFloats(generator) * 2.0f - 1.0f, 0.0f };
				noise.normalize();
				ssaoNoise[i] = noise;
			}
			s_SSAONoise = texture2d::create(4U, 4U, ENG_RGBA16F, FILTER_NEAREST, FILTER_NEAREST, ENG_REPEAT, ssaoNoise);
			s_SSAO_FBO = FrameBuffer::create();
			s_AO = texture2d::create(s_maxViewportSize[0], s_maxViewportSize[1], ENG_R8, FILTER_NEAREST, FILTER_NEAREST);
			s_SSAO_FBO->bind();
			s_SSAO_FBO->attachTexture(s_AO);
			s_SSAO_FBO->checkStatus();
			s_SSAO_FBO->unbind();

			s_SSAO_shader = shader::create("SSAO/SSAO.shader");
			s_SSAO_shader->bind();
			s_SSAO_shader->setUniform1i("u_gPosition", 0);
			s_SSAO_shader->setUniform1i("u_gNormal", 1);
			s_SSAO_shader->setUniform1i("u_ssaoNoise", 2);
			s_SSAO_shader->setUniform3fArr("u_samples", SSAO_kernelSize, ssaoKernel);
			s_SSAO_shader->unbind();

			s_debug_display_ssao = shader::create("SSAO/debug_display.shader");
			s_debug_display_ssao->bind();
			s_debug_display_ssao->setUniform1i("u_texture", 0);
			s_debug_display_ssao->unbind();
		}

		//setup skybox-drawing
		s_skyboxVa = vertexArray::create();
		s_skyboxVa->load("cube.model");
		s_skyboxVa->unbind();
		s_skyboxShader = s_shaderLib->load("skybox/skybox_hdr.shader");

		//setup pcf
		//create a random jitter-texture
	}

	/*void Renderer::setupCascadeDisplay()
	{
		vec4 pos(0.0f, 0.0f, 0.0f, 1.0f);
		vec4 rot(0.0f, 0.0f, 0.0f, 0.0f);
		mat4 rotation = mat4::Rotx(-rot.x) * mat4::Roty(-rot.y) * mat4::Rotz(-rot.z);
		mat4 translation = mat4::transMat(-pos);
		mat4 viewMat = rotation * translation;
		mat4 inverseViewMat = mat4::inverse3x3(viewMat) * mat4::transMat({ -viewMat.mat[0][3], -viewMat.mat[1][3], -viewMat.mat[2][3], 1.0f });

		//initialize cascaded shadowMap boundingboxes
		vec3 frustumPointsViewSpace[8];
		float aspectRatio = 1.77777779f;
		float hFov = 90.0f, vFov = std::atanf((1.0f / aspectRatio) * std::tanf(0.5f * RAD(hFov))) * 2.0f * (180.0f / 3.1415927);
		float nearPlane = 0.1f, farPlane = 100.0f;
		const float Nn = nearPlane;//the near cascade's nearPlane is the camera's nearPlane
		const float Nf = farPlane * 0.3333333f;
		const float tanFovH = tanf(RAD(hFov * 0.5f));
		const float tanFovV = tanf(RAD(vFov * 0.5f));
		frustumPointsViewSpace[0] = { Nn * tanFovH, 0.0f, -Nn };//near-cascade | nearPlane | positive x
		frustumPointsViewSpace[1] = { -frustumPointsViewSpace[0].x, 0.0f, -Nn };//near-cascade | nearPlane | negative x
		frustumPointsViewSpace[2] = { 0.0f, Nn * tanFovV, -Nn };//near-cascade | nearPlane | positive y
		frustumPointsViewSpace[3] = { 0.0f, -frustumPointsViewSpace[2].y, -Nn };//near-cascade | nearPlane | negative y
		frustumPointsViewSpace[4] = { Nf * tanFovH, 0.0f, -Nf };//near-cascade | farPlane | positive x
		frustumPointsViewSpace[5] = { -frustumPointsViewSpace[4].x, 0.0f, -Nf };//near-cascade | farPlane | negative x
		frustumPointsViewSpace[6] = { 0.0f, Nf * tanFovV, -Nf };//near-cascade | farPlane | positive y
		frustumPointsViewSpace[7] = { 0.0f, -frustumPointsViewSpace[6].y, -Nf };//near-cascade | farPlane | negative y

		vec3 frustumPointsWorldSpace[8];
		for (uint8_t i = 0; i < 8; i++)
		{
			frustumPointsWorldSpace[i] = (inverseViewMat * vec4(frustumPointsViewSpace[i], 1.0f)).xyz();
		}

		const float vb[] = { frustumPointsWorldSpace[0].x, frustumPointsWorldSpace[0].y, frustumPointsWorldSpace[0].z,
			frustumPointsWorldSpace[1].x, frustumPointsWorldSpace[1].y, frustumPointsWorldSpace[1].z,
			frustumPointsWorldSpace[2].x, frustumPointsWorldSpace[2].y, frustumPointsWorldSpace[2].z,
			frustumPointsWorldSpace[3].x, frustumPointsWorldSpace[3].y, frustumPointsWorldSpace[3].z,
			frustumPointsWorldSpace[4].x, frustumPointsWorldSpace[4].y, frustumPointsWorldSpace[4].z,
			frustumPointsWorldSpace[5].x, frustumPointsWorldSpace[5].y, frustumPointsWorldSpace[5].z,
			frustumPointsWorldSpace[6].x, frustumPointsWorldSpace[6].y, frustumPointsWorldSpace[6].z,
			frustumPointsWorldSpace[7].x, frustumPointsWorldSpace[7].y, frustumPointsWorldSpace[7].z, };
		Ref_ptr<vertexBuffer> cascade_vb = vertexBuffer::create(sizeof(vb), (void*)vb);
		Ref_ptr<vertexBufferLayout> cascade_layout = vertexBufferLayout::create();
		cascade_layout->pushFloat(3U);//position
		cascade_vb->setLayout(cascade_layout);
		const uint32_t indices[] = { 4, 2, 0,
			2, 7, 3,
			6, 5, 7,
			1, 7, 5,
			0, 3, 1,
			4, 1, 5,
			4, 6, 2,
			2, 6, 7,
			6, 4, 5,
			1, 3, 7,
			0, 2, 3,
			4, 0, 1 };
		Ref_ptr<indexBuffer> cascade_ib = indexBuffer::create(sizeof(indices) / sizeof(uint32_t), indices);
		s_debug_cascade_va = vertexArray::create();
		s_debug_cascade_va->addBuffer(cascade_vb);
		s_debug_cascade_va->addBuffer(cascade_ib);
		s_debug_cascade_va->unbind();
		s_debug_renderCascade = shader::create("forward/debug_cascaded.shader");
		s_debug_renderCascade->bindUniformBlock("ViewProjection", VIEWPROJ_BIND);
		s_debug_renderCascade->unbind();
	}*/

	void Renderer::shutdown()
	{
		//assuming this is the only strong reference to these pointers
		s_hdr_tex.reset();
		s_bright_tex.reset();
		s_pingPongTex[0].reset();
		s_pingPongTex[1].reset();
		s_pingPongFbo[0].reset();
		s_pingPongFbo[1].reset();
		s_hdr_depth.reset();
		s_hdr_fbo.reset();
		s_hdr_quad_va.reset();
		s_hdr_quad_shader.reset();
		s_hdr_quad_shader_bloom.reset();
		s_blur_shader[0].reset();
		s_blur_shader[1].reset();
		s_gBuffer.reset();
		s_gPosition.reset();
		s_gNormal.reset();
		s_gAlbSpec.reset();
		s_gDepth.reset();
		s_gLightingShader[0].reset();
		s_gLightingShader[1].reset();
		s_SSAONoise.reset();
		s_SSAO_FBO.reset();
		s_AO.reset();
		s_SSAO_shader.reset();
		s_debug_display_ssao.reset();
		//s_debug_display_cascaded.reset();
		//s_debug_cascade_va.reset();
		//s_debug_renderCascade.reset();
		s_skyboxVa.reset();
		s_skyboxTex.reset();
		s_skyboxShader.reset();
		//s_pcfJitter.reset();
	}

	void Renderer::beginScene(const perspectiveCamera& cam)
	{
		s_sceneData->viewProjMat = cam.getViewProjMat();
		s_sceneData->viewPos = { cam.getPos(), 1.0f };
		s_sceneData->viewMat = cam.getViewMat();
		lightMatrixCalculator::setViewMat(cam.getViewMat());
		/*s_gLightingShader[0]->bind();
		s_gLightingShader[0]->setUniformMat4("u_viewMat", cam.getViewMat(), 1);
		s_gLightingShader[1]->bind();
		s_gLightingShader[1]->setUniformMat4("u_viewMat", cam.getViewMat(), 1);
		s_gLightingShader[1]->unbind();*/
		s_gLightingShader[0]->bind();
		s_gLightingShader[0]->setUniformMat4("u_invViewMat", mat4::inverse3x3(s_sceneData->viewMat) * mat4::transMat({ -s_sceneData->viewMat.mat[0][3], -s_sceneData->viewMat.mat[1][3], -s_sceneData->viewMat.mat[2][3], 1.0f }), 1);
		s_gLightingShader[1]->bind();
		s_gLightingShader[1]->setUniformMat4("u_invViewMat", mat4::inverse3x3(s_sceneData->viewMat) * mat4::transMat({ -s_sceneData->viewMat.mat[0][3], -s_sceneData->viewMat.mat[1][3], -s_sceneData->viewMat.mat[2][3], 1.0f }), 1);
		s_gLightingShader[1]->unbind();
		//supply skyboxShader with required information
		s_skyboxShader->bind();
		s_skyboxShader->setUniformMat4("u_viewProj", cam.getProjMat() * mat4::rmTranslation(cam.getViewMat()), 1);//set a viewProjMat without translation applied
		s_skyboxShader->unbind();
		//set uniformBuffers for view - and projection matrices here, they will be used by many shaders
		s_sceneData->viewProjBuffer->bind();
		s_sceneData->viewProjBuffer->updateElement(0, &mat4::transposed(s_sceneData->viewProjMat));//viewProjMat is at index 0
		s_sceneData->viewProjBuffer->updateElement(1, &s_sceneData->viewPos);//viewPos is at index 1
		s_sceneData->viewProjBuffer->updateElement(2, &mat4::transposed(s_sceneData->viewMat));//viewMat is at index 2
		s_sceneData->viewProjBuffer->unbind();
		//update the ssao-shader with the new matrices
		s_SSAO_shader->bind();
		s_SSAO_shader->setUniformMat4("u_viewMat", cam.getViewMat(), 1);
		s_SSAO_shader->setUniformMat4("u_projMat", cam.getProjMat(), 1);
		s_SSAO_shader->unbind();
		//update dynamic lights
		s_sceneData->lightManager.updateLights();//update lights for all shaders
		//update the exposure
		s_hdr_quad_shader->bind();
		s_hdr_quad_shader->setUniform1f("u_exposure", s_exposure);
		s_hdr_quad_shader_bloom->bind();
		s_hdr_quad_shader_bloom->setUniform1f("u_exposure", s_exposure);
		s_hdr_quad_shader_bloom->unbind();
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
		//geometry-pass:
		renderCommand::setViewport(s_maxViewportSize[0], s_maxViewportSize[1]);
		s_gBuffer->bind();
		renderCommand::clear();
		renderCommand::setBlend(ENG_ONE, ENG_ZERO);//->because the specular intensity value is interpreted as alpha, this has to be set manually
		for (const auto& Mesh : s_renderQueue)
		{
			const auto mat = Mesh->getMaterial();
			if (!(mat->getFlags() & flag_no_deferred))
			{
				if (mat->getFlags() & flag_depth_test)
					renderCommand::setDepth(ENG_LESS);
				else
					renderCommand::setDepth(ENG_ALWAYS);
				renderCommand::enableCullFace(!(mat->getFlags() & flag_no_backface_cull));//dissable face culling if the object has no backface-culling enabled
				mat->bind(10);//always use the 0th shader with deferred shading
				mat->getShader()->setUniformMat4("u_modelMat", Mesh->getModelMat(), 1);
				mat->getShader()->setUniformMat3("u_normalMat", Mesh->getNormalMat(), 1);
				const auto geometry = Mesh->getVa();
				geometry->bind();
				renderCommand::drawIndexed(geometry);
			}
		}
		//fill the ambient-occlusion texture:
		/*s_SSAO_FBO->bind();
		renderCommand::clear();
		s_SSAO_shader->bind();
		s_gPosition->bind(0);
		s_gNormal->bind(1);
		s_SSAONoise->bind(2);
		s_hdr_quad_va->bind();
		renderCommand::drawIndexed(s_hdr_quad_va);
		s_SSAO_FBO->unbind();
		//test-display of the ambient-occlusion texture
		renderCommand::setViewport(s_mainViewport[0], s_mainViewport[1]);
		renderCommand::clear();
		s_debug_display_ssao->bind();
		s_AO->bind(0);
		renderCommand::drawIndexed(s_hdr_quad_va);
		s_debug_display_ssao->unbind();*/
		s_hdr_quad_va->unbind();
		s_hdr_fbo->bind();
		renderCommand::clear();
		renderCommand::setViewport(s_maxViewportSize[0], s_maxViewportSize[1]);
		//lighting-pass(into the hdr-texture):
		s_gLightingShader[s_bloom]->bind();//depending on whether bloom is enabled, we use the bloom or the non-bloom shader
		s_gPosition->bind(0);
		s_gNormal->bind(1);
		s_gAlbSpec->bind(2);
		//bind the shadowMaps
		s_sceneData->lightManager.getDirLightMapArray()->bind(3);
		s_sceneData->lightManager.getPointLightMapArray()->bind(4);
		s_sceneData->lightManager.getSpotLightMapArray()->bind(5);
		//bind the skybox for reflections
		s_skyboxTex->bind(6);//wow, we actually have 7 textures attached
		//////
		s_hdr_quad_va->bind();
		renderCommand::drawIndexed(s_hdr_quad_va);
		////////////now the forward-rendering part
		renderCommand::copyFrameBufferContents(s_gBuffer, s_maxViewportSize[0], s_maxViewportSize[1], ENG_DEPTH_BUFFER_BIT,
			s_hdr_fbo, s_maxViewportSize[0], s_maxViewportSize[1], ENG_DEPTH_BUFFER_BIT, FILTER_NEAREST);
		s_hdr_fbo->bind();//bind the hdr_fbo to both read and draw again
		for (auto& it = s_renderQueue.rbegin(); it != s_renderQueue.rend(); it++)//NOTE THAT iterating through this backwards is important, so that the skybox is drawn last!!!
		{
			const auto& Mesh = *it;
			const auto mat = Mesh->getMaterial();
			if (mat->getFlags() & flag_no_deferred)
			{
				if (mat->getFlags() & flag_depth_test)
					renderCommand::setDepth(ENG_LEQUAL);//ENG_LESS by default
				else
					renderCommand::setDepth(ENG_ALWAYS);
				renderCommand::enableCullFace(!(mat->getFlags() & flag_no_backface_cull));//dissable face culling if the object has no backface-culling enabled
				mat->bind(10);
				mat->getShader()->setUniformMat4("u_modelMat", Mesh->getModelMat(), 1);
				mat->getShader()->setUniformMat3("u_normalMat", Mesh->getNormalMat(), 1);
				const auto geometry = Mesh->getVa();
				geometry->bind();
				renderCommand::drawIndexed(geometry);
			}
		}
		/////////////////end of forward-rendering part
		s_renderQueue.clear();
		s_hdr_fbo->unbind();//don't remove this
		//post-processing:
		if (s_bloom)
		{
			renderCommand::copyFrameBufferContents(s_hdr_fbo, s_maxViewportSize[0], s_maxViewportSize[1], ENG_COLOR_ATTACHMENT1,
				s_pingPongFbo[0], s_maxViewportSize[0] / 2, s_maxViewportSize[1] / 2, ENG_COLOR_ATTACHMENT0, FILTER_LINEAR);
			s_pingPongFbo[1]->bind();
			renderCommand::setViewport(s_maxViewportSize[0] / 2, s_maxViewportSize[1] / 2);
			renderCommand::clear();
			bool horizontal = false;
			//on the first iteration we want to blur vertically -> the bright_tex is in pingPongTex[0], so we bind pingPongFbo[1] for rendering!
			constexpr uint8_t blurCount = 6;
			for (uint8_t i = 0; i < blurCount; i++)
			{
				s_pingPongFbo[!horizontal]->bind();
				//shader at index 0 is vertical, shader at index 1 is horizontal
				s_blur_shader[horizontal]->bind();
				s_pingPongTex[horizontal]->bind(0);
				s_hdr_quad_va->bind();
				renderCommand::drawIndexed(s_hdr_quad_va);
				horizontal = !horizontal;
			}
			//brightTex is in pingPongTex[0] -> pingPongFbo[1] is bound and the texture is rendered and transfered into pingPongTex[1] using blurShader[0]
			//now the horizontal-variable flips(now true): brightTex is in pingPongTex[1] -> pingPongFbo[0] is bound and the texture is rendered and transfered into pingPongTex[0] again using blurShader[1]
			//renderCommand::setViewport(s_maxViewportSize[0], s_maxViewportSize[1]); //<- normally we would have to set the viewport back to normal after setting it to half resolution for blurring
			s_pingPongFbo[0]->unbind();
		}
		//now render to screen-spaced quad:
		renderCommand::setViewport(s_mainViewport[0], s_mainViewport[1]);
		renderCommand::clear();
		s_hdr_tex->bind(0);
		s_hdr_quad_va->bind();
		if (s_bloom)
		{
			//s_hdr_quad_shader->bind();
			s_hdr_quad_shader_bloom->bind();
			s_pingPongTex[0]->bind(1);
		}
		else
		{
			s_hdr_quad_shader->bind();
		}
		renderCommand::drawIndexed(s_hdr_quad_va);
		s_hdr_quad_va->unbind();
		/*s_hdr_quad_va->bind();
		s_sceneData->lightManager.getDirLightMapArray()->bind(0);
		s_debug_display_cascaded->bind();
		renderCommand::drawIndexed(s_hdr_quad_va);
		s_hdr_quad_va->unbind();*/
	}

	void Renderer::sceneData::init()
	{
		//initialize buffer for viewProjection
		viewProjBuffer = globalBuffer::create(sizeof(mat4) + sizeof(vec4) + sizeof(mat4), DYNAMIC_DRAW);
		viewProjBuffer->lAddMat4B();//viewProjMat
		viewProjBuffer->lAddVec4B();//viewPosition
		viewProjBuffer->lAddMat4B();//viewMat
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
		WeakRef_ptr<FrameBuffer> depth_fbo = s_sceneData->lightManager.getDepthFB();
		depth_fbo->bind();
		WeakRef_ptr<shader> depthShader_dir = s_sceneData->lightManager.getDepthShader_dir();
		depthShader_dir->bind();
		for (auto& it = s_sceneData->lightManager.dirLightBegin(); it != s_sceneData->lightManager.dirLightEnd(); ++it)
		{
			const auto& data = *it;
			//near cascade
			depthShader_dir->setUniformMat4("u_toLightSpace", data.it->second.nearCascade);
			depth_fbo->attachTexture(s_sceneData->lightManager.getDirLightMapArray(), data.texIndex * 3 + 0);
			renderCommand::clearDepth();
			for (const auto& Mesh : s_renderQueue)
			{
				if (Mesh->getMaterial()->getFlags() & flag_cast_shadow)
				{
					depthShader_dir->setUniformMat4("u_modelMat", Mesh->getModelMat(), 1);
					Ref_ptr<vertexArray> geometry = Mesh->getVa();
					geometry->bind();
					renderCommand::drawIndexed(geometry);
				}
			}
			//middle cascade
			depthShader_dir->setUniformMat4("u_toLightSpace", data.it->second.middleCascade);
			depth_fbo->attachTexture(s_sceneData->lightManager.getDirLightMapArray(), data.texIndex * 3 + 1);
			renderCommand::clearDepth();
			for (const auto& Mesh : s_renderQueue)
			{
				if (Mesh->getMaterial()->getFlags() & flag_cast_shadow)
				{
					depthShader_dir->setUniformMat4("u_modelMat", Mesh->getModelMat(), 1);
					Ref_ptr<vertexArray> geometry = Mesh->getVa();
					geometry->bind();
					renderCommand::drawIndexed(geometry);
				}
			}
			//far cascade
			depthShader_dir->setUniformMat4("u_toLightSpace", data.it->second.farCascade);
			depth_fbo->attachTexture(s_sceneData->lightManager.getDirLightMapArray(), data.texIndex * 3 + 2);
			renderCommand::clearDepth();
			for (const auto& Mesh : s_renderQueue)
			{
				if (Mesh->getMaterial()->getFlags() & flag_cast_shadow)
				{
					depthShader_dir->setUniformMat4("u_modelMat", Mesh->getModelMat(), 1);
					Ref_ptr<vertexArray> geometry = Mesh->getVa();
					geometry->bind();
					renderCommand::drawIndexed(geometry);
				}
			}
		}
		depthShader_dir->unbind();
		WeakRef_ptr<shader> depthShader_point = s_sceneData->lightManager.getDepthShader_point();
		depthShader_point->bind();
		depth_fbo->attachTexture(s_sceneData->lightManager.getPointLightMapArray());//->don't specify any layer here, because of weird opengl-api -> do that with a uniform
		renderCommand::clearDepth();
		for (auto& it = s_sceneData->lightManager.pointLightBegin(); it != s_sceneData->lightManager.pointLightEnd(); ++it)
		{
			const auto& data = *it;
			depthShader_point->setUniformMat4_6("u_toLightSpace", data.it->second);
			depthShader_point->setUniform4f("u_lightPos", data.it->first.position);
			depthShader_point->setUniform1ui("u_layer", data.texIndex * 6);
			for (const auto& Mesh : s_renderQueue)
			{
				if (Mesh->getMaterial()->getFlags() & flag_cast_shadow)
				{
					depthShader_point->setUniformMat4("u_modelMat", Mesh->getModelMat(), 1);
					Ref_ptr<vertexArray> geometry = Mesh->getVa();
					geometry->bind();
					renderCommand::drawIndexed(geometry);
				}
			}
		}
		depthShader_point->unbind();
		WeakRef_ptr<shader> depthShader_spot = s_sceneData->lightManager.getDepthShader_spot();
		depthShader_spot->bind();
		for (auto& it = s_sceneData->lightManager.spotLightBegin(); it != s_sceneData->lightManager.spotLightEnd(); ++it)
		{
			const auto& data = *it;
			depthShader_spot->setUniformMat4("u_toLightSpace", data.it->second);
			depth_fbo->attachTexture(s_sceneData->lightManager.getSpotLightMapArray(), data.texIndex);
			renderCommand::clearDepth();
			for (const auto& Mesh : s_renderQueue)
			{
				if (Mesh->getMaterial()->getFlags() & flag_cast_shadow)
				{
					depthShader_spot->setUniformMat4("u_modelMat", Mesh->getModelMat(), 1);
					Ref_ptr<vertexArray> geometry = Mesh->getVa();
					geometry->bind();
					renderCommand::drawIndexed(geometry);
				}
			}
		}
		depthShader_spot->bind();
		depth_fbo->unbind();
		renderCommand::cullFace(ENG_BACK);
		renderCommand::setViewport(s_mainViewport[0], s_mainViewport[1]);
	}

	void Renderer::onImGuiRender()
	{
		ImGui::Begin("Rendering options");
		ImGui::Checkbox("bloom", &s_bloom);
		ImGui::SliderFloat("exposure", &s_exposure, 0.01f, 100.0f);
		ImGui::End();
		/*ImGui::Begin("Textures");
		ImGui::Image((ImTextureID)s_gPosition->getRenderer_id(), { 128, 128 });
		ImGui::Image((ImTextureID)s_gNormal->getRenderer_id(), { 128, 128 });
		ImGui::Image((ImTextureID)s_gAlbSpec->getRenderer_id(), { 128, 118 });
		ImGui::End();*/
		/*ImGui::Begin("HDR_texture");
		ImGui::Image((ImTextureID)s_hdr_tex->getRenderer_id(), { 512, 512 });
		ImGui::End();*/
		/*ImGui::Begin("Bright_texture");
		ImGui::Image((ImTextureID)s_bright_tex->getRenderer_id(), { 512, 512 });
		ImGui::End();*/
		/*ImGui::Begin("PingPongTex0");
		ImGui::Image((ImTextureID)s_pingPongTex[0]->getRenderer_id(), { 960, 540 });
		ImGui::End();*/
	}
}