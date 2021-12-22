#include "Engpch.hpp"

#include "imgui.h"
#include "scene.hpp"
#include "Engine/Rendering/renderer.hpp"

namespace Engine
{
	void Scene::initialize(const float fov, const float nearPlane, const float farPlane, const float aspectRatio)
	{
		m_camControl.initialize(nearPlane, farPlane, fov, aspectRatio);
		lightMatrixCalculator::init(fov, std::atanf((1.0f / aspectRatio) * std::tanf(0.5f * fov * (3.14159265f / 180.0f))) * 2.0f * (180.0f / 3.14159265f), nearPlane, farPlane);
		//create a skybox and push it as a mesh:
		Ref_ptr<texture3d_hdr> skyboxTex = texture3d_hdr::create("skyboxes/stadium_hdr/");
		m_skyboxTex = skyboxTex;
		Ref_ptr<material> skyboxMat = material::create(Renderer::getSkyboxShader(), "skyboxMat", flag_no_deferred | flag_no_backface_cull | flag_depth_test);
		skyboxMat->setTexture("u_skybox", skyboxTex);
		m_matLib.add(skyboxMat);
		Entity skybox = m_registry.create();
		m_registry.addComponent<MeshComponent>(skybox, Renderer::getSkyboxVa(), skyboxMat, std::string("skybox"));
		m_registry.addComponent<TransformComponent>(skybox);
	}

	void Scene::clear()
	{
		m_camControl.clear();
		m_matLib.clear();
		m_registry.clear();
	}

	void Scene::onUpdate(timestep& ts, const bool rotateCam)
	{
		m_camControl.onUpdate(ts, rotateCam);//update the camera according to user input
		Engine::Renderer::setCam(m_camControl.getCam());//submit the camera to the renderer for the next frame
		m_sceneGraph->calculateGlobalTransforms();//calculate the transforms for all objects in the scene
		m_matLib.updateDynamic();//update the dynamic materials
	}

	void Scene::onRender()
	{
		m_oss->sub();//submit all of the scene's meshes using the ObjectSubmitSystem
		//submit a skyboxTexture that the Renderer needs for reflections
		Engine::Renderer::setSkyboxTex(m_skyboxTex);//I'm not really happy, with how this is happening at the moment, maybe I should just store the skybox in the Renderer??
	}

	void Scene::onEvent(Event& e)
	{
		m_camControl.onEvent(e);
	}

	void Scene::onImGuiRender()
	{
		m_sceneGraph->onImGuiRender();//draw the sceneGraph ui
	}

	void Scene::addMaterial(const Ref_ptr<material>& mat)
	{
		m_matLib.add(mat);
	}

	void Scene::addMaterial(const Ref_ptr<material>& mat, const std::function<void(const void* updateValues, material* mat)>& updateFunc, const void* updateValues)
	{
		m_matLib.addDynamic(mat, updateFunc, updateValues);
	}
}