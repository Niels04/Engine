#pragma once
#include "renderCommand.hpp"
#include "perspCam.hpp"
#include "orthographicCam.hpp"
#include "shader.hpp"
#include "buffers.hpp"
#include "Engine/objects/mesh.hpp"
#include "lights.hpp"

#define VIEWPROJ_BIND 0

namespace Engine
{
	class Renderer
	{
	public:
		static void init(const uint32_t viewportWidth, const uint32_t viewportHeight, const uint32_t maxViewportWidth, const uint32_t maxViewportHeight);//some things that need to be initiallized and need a graphics context, so they have to be initiallized after the graphics context has been created, thats why this function exists
		static void shutdown();

		static void beginScene(const perspectiveCamera& cam);//takes in a camera (viewprojMat) and the lights to update
		static void beginScene(const OrthographicCamera& cam);
		static void endScene();
		static void Flush();//render all the elements currently in renderqueue and clear the renderqueue
		static void sub(const Ref_ptr<vertexArray> va, const Ref_ptr<shader> shader, const mat4& transform = mat4::identity());//stands for submit, gonna overload this, so that we can submit different things //submit a vertexArray and a shader
		static void sub(const Ref_ptr<mesh> mesh);//submit a mesh

		inline static void setMainViewPort(const uint32_t viewportWidth, const uint32_t viewportHeight) { s_mainViewport[0] = viewportWidth; s_mainViewport[1] = viewportHeight; }

		static void RenderDepthMaps();
		static void onImGuiRender();

		inline static RendererAPI::API getAPI() { return RendererAPI::getAPI(); }
		inline static shaderLib* getShaderLib() { return s_shaderLib; }
		inline static LightManager* getLightManager() { return &s_sceneData->lightManager; }

		//inline static void setDebugDrawLights(bool DebugDrawLights) { s_DebugDrawLights = DebugDrawLights; /*initialize shaders and models that are needet to draw the lights HERE(if set to false de-initialize all the buffers and shaders)*/ }

		[[nodiscard]] static directionalLight** addStaticDirLight(const directionalLight& light, const vec3& pos = { 0.0f, 0.0f, 0.0f }) { return s_sceneData->lightManager.addStaticDirLight(light); }
		[[nodiscard]] static pointLight** addStaticPointLight(const pointLight& light) { return s_sceneData->lightManager.addStaticPointLight(light); }
		[[nodiscard]] static spotLight** addStaticSpotLight(const spotLight& light) { return s_sceneData->lightManager.addStaticSpotLight(light); }
		[[nodiscard]] static directionalLight** addDynamicDirLight(const directionalLight& light, const vec3& pos = { 0.0f, 0.0f, 0.0f }) { return s_sceneData->lightManager.addDynamicDirLight(light); }
		[[nodiscard]] static pointLight** addDynamicPointLight(const pointLight& light) { return s_sceneData->lightManager.addDynamicPointLight(light); }
		[[nodiscard]] static spotLight** addDynamicSpotLight(const spotLight& light) { return s_sceneData->lightManager.addDynamicSpotLight(light); }
	private:
		struct sceneData
		{
			void init();
			mat4 viewMat;
			mat4 projMat;
			Ref_ptr<globalBuffer> viewProjBuffer;
			Ref_ptr<globalBuffer> testBuffer;
			LightManager lightManager;
		};
		static std::vector<Ref_ptr<mesh>> s_renderQueue;
		static sceneData* s_sceneData;
		static shaderLib* s_shaderLib;
		static uint32_t s_mainViewport[2];
		static uint32_t s_maxViewportSize[2];
		static Ref_ptr<FrameBuffer> s_hdr_fbo;
		static Ref_ptr<texture2d> s_hdr_tex;
		static Ref_ptr<texture2d> s_bright_tex;//second attachment to the hdr_fbo for rendering with bloom
		static Ref_ptr<texture2d> s_pingPongTex[2];
		static Ref_ptr<FrameBuffer> s_pingPongFbo[2];
		static Ref_ptr<RenderBuffer> s_hdr_depthStencil;
		static Ref_ptr<vertexArray> s_hdr_quad_va;
		static Ref_ptr<shader> s_hdr_quad_shader;
		static Ref_ptr<shader> s_hdr_quad_shader_bloom;
		static Ref_ptr<shader> s_blur_shader;
		static bool s_bloom;
		//static bool s_DebugDrawLights;
	};
}