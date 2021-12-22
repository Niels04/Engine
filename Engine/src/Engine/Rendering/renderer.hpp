#pragma once
#include "renderCommand.hpp"
#include "perspCam.hpp"
#include "orthographicCam.hpp"
#include "shader.hpp"
#include "buffers.hpp"
#include "lights.hpp"
#include "Engine/objects/components.hpp"

#define VIEWPROJ_BIND 0
#define SSAO_kernelSize 64U

namespace Engine
{
	class Renderer
	{
	public:
		static void init(const uint32_t viewportWidth, const uint32_t viewportHeight, const uint32_t maxViewportWidth, const uint32_t maxViewportHeight);//some things that need to be initiallized and need a graphics context, so they have to be initiallized after the graphics context has been created, thats why this function exists
		static void shutdown();

		static void setCam(const perspectiveCamera& cam);//takes in a camera (viewprojMat) and the lights to update
		static void beginScene();
		static void endScene();
		static void test();
		static void Flush();//render all the elements currently in renderqueue and clear the renderqueue
		static void sub(const MeshComponent& mesh, const TransformComponent& transform);//submit a mesh with a transform

		inline static void setMainViewPort(const uint32_t viewportWidth, const uint32_t viewportHeight) { s_mainViewport[0] = viewportWidth; s_mainViewport[1] = viewportHeight; }
		inline static void setSkyboxTex(const Ref_ptr<texture>& skyboxTex) { s_skyboxTex = skyboxTex; }

		static void RenderDepthMaps();
		static void onImGuiRender();

		inline static RendererAPI::API getAPI() { return RendererAPI::getAPI(); }
		inline static shaderLib* getShaderLib() { return s_shaderLib; }
		inline static LightRenderingSystem* getLightRenderingSystem() { return s_sceneData->lightRenderingSystem; }
		inline static void initLightRenderingSystem(LightRenderingSystem* lightRenderingSystem) { s_sceneData->lightRenderingSystem = lightRenderingSystem; s_sceneData->lightRenderingSystem->init(); }

		inline static Ref_ptr<shader> getSkyboxShader() { return s_skyboxShader; }
		inline static Ref_ptr<vertexArray> getSkyboxVa() { return s_skyboxVa; }

		//inline static void setDebugDrawLights(bool DebugDrawLights) { s_DebugDrawLights = DebugDrawLights; /*initialize shaders and models that are needet to draw the lights HERE(if set to false de-initialize all the buffers and shaders)*/ }
	private:
		struct sceneData
		{
			void init();
			mat4 viewProjMat;
			vec4 viewPos;
			mat4 viewMat;
			Ref_ptr<globalBuffer> viewProjBuffer;
			LightRenderingSystem* lightRenderingSystem;
		};
		//static void setupCascadeDisplay();
		static std::vector<std::pair<const MeshComponent&, const TransformComponent&>> s_renderQueue;
		static sceneData* s_sceneData;
		static shaderLib* s_shaderLib;
		static uint32_t s_mainViewport[2];
		static uint32_t s_maxViewportSize[2];
		static Ref_ptr<FrameBuffer> s_hdr_fbo;
		static Ref_ptr<texture2d> s_hdr_tex;
		static Ref_ptr<texture2d> s_bright_tex;//second attachment to the hdr_fbo for rendering with bloom
		static Ref_ptr<texture2d> s_pingPongTex[2];
		static Ref_ptr<FrameBuffer> s_pingPongFbo[2];
		static Ref_ptr<RenderBuffer> s_hdr_depth;
		static Ref_ptr<vertexArray> s_hdr_quad_va;
		static Ref_ptr<shader> s_hdr_quad_shader;
		static Ref_ptr<shader> s_hdr_quad_shader_bloom;
		static Ref_ptr<shader> s_blur_shader[2];
		static bool s_bloom;
		static float s_exposure;//exposure for hdr rendering
		//static bool s_DebugDrawLights;
		//G-BUFFER:::::
		static Ref_ptr<FrameBuffer> s_gBuffer;
		static Ref_ptr<texture2d> s_gPosition;
		static Ref_ptr<texture2d> s_gNormal;
		static Ref_ptr<texture2d> s_gAlbSpec;
		static Ref_ptr<RenderBuffer> s_gDepth;
		static Ref_ptr<shader> s_gLightingShader[2];//one with and one without bloom
		//SSAO
		static Ref_ptr<texture2d> s_SSAONoise;
		static Ref_ptr<FrameBuffer> s_SSAO_FBO;
		static Ref_ptr<texture2d> s_AO;//ambient occlusion texture that only has a red-component
		static Ref_ptr<shader> s_SSAO_shader;
		static Ref_ptr<shader> s_debug_display_ssao;
		//skybox
		static Ref_ptr<shader> s_skyboxShader;
		static Ref_ptr<vertexArray> s_skyboxVa;
		static WeakRef_ptr<texture> s_skyboxTex;//can be updated by Sandbox whenever necessary ->Renderer does not have ownership over this!
		//other
		/*static Ref_ptr<shader> s_debug_display_cascaded;//renders a cascaded shadowMap to screen
		static Ref_ptr<vertexArray> s_debug_cascade_va;//vertexArray used to visualize a cascade
		static Ref_ptr<shader> s_debug_renderCascade;//renders the debug_cascade vertexArray*/
	};
}