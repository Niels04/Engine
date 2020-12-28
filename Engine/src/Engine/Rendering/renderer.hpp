#pragma once
#include "renderCommand.hpp"
#include "perspCam.hpp"
#include "shader.hpp"
#include "buffers.hpp"
#include "Engine/objects/mesh.hpp"
#include "lights.hpp"

namespace Engine
{
	class Renderer
	{
	public:
		static void init();//some things that need to be initiallized, need a graphics context, so they have to be initiallized after the graphics context has been created, thats why this function exists

		static void beginScene(const perspectiveCamera& cam);//takes in a camera (viewprojMat) and the lights to update
		static void endScene();
		static void Flush();
		static void sub(const Ref_ptr<vertexArray> va, const Ref_ptr<shader> shader, const mat4& transform = mat4::identity());//stands for submit, gonna overload this, so that we can submit different things //submit a vertexArray and a shader
		static void sub(const Ref_ptr<mesh> mesh);//submit a mesh

		inline static RendererAPI::API getAPI() { return RendererAPI::getAPI(); }
		inline static shaderLib* getShaderLib() { return s_shaderLib; }

		static directionalLight* addStaticDirLight(const directionalLight& light) { return s_sceneData->lightManager.addStaticDirLight(light); }
		static pointLight* addStaticPointLight(const pointLight& light) { return s_sceneData->lightManager.addStaticPointLight(light); }
		static spotLight* addStaticSpotLight(const spotLight& light) { return s_sceneData->lightManager.addStaticSpotLight(light); }
		static directionalLight* addDynamicDirLight(const directionalLight& light) { return s_sceneData->lightManager.addDynamicDirLight(light); }
		static pointLight* addDynamicPointLight(const pointLight& light) { return s_sceneData->lightManager.addDynamicPointLight(light); }
		static spotLight* addDynamicSpotLight(const spotLight& light) { return s_sceneData->lightManager.addDynamicSpotLight(light); }
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
		static sceneData* s_sceneData;
		static shaderLib* s_shaderLib;
	};
}