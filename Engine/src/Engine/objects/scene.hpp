#pragma once
#include <Engpch.hpp>
#include "Engine/Rendering/material.hpp"
#include "Engine/perspectiveCameraController.hpp"
#include "components.hpp"
#include "Engine/ECS/Registry.hpp"
#include "Engine/Rendering/ObjectSubmitSystem.hpp"
#include "Engine/Rendering/SceneGraphSystem.hpp"
#include "Engine/Rendering/renderer.hpp"

constexpr size_t componentTypeCount = 5;
constexpr size_t systemCount = 3;

namespace Engine
{
	class Scene
	{
		//friend class NodeEditorLayer;
	public:
		Scene()
		{
			//initialize m_Registry
			m_registry.registerComponent<MeshComponent>();
			m_registry.registerComponent<DirLightComponent>();
			m_registry.registerComponent<PointLightComponent>();
			m_registry.registerComponent<SpotLightComponent>();
			m_registry.registerComponent<TransformComponent>();
			Renderer::initLightRenderingSystem(m_registry.createSystem<LightRenderingSystem>(m_registry));//initialize the renderer's lightRenderingSystem with the ECS
			m_oss = m_registry.createSystem<ObjectSubmitSystem>(m_registry);//initialize the object submission system
			m_sceneGraph = m_registry.createSystem<SceneGraphSystem>(m_registry);//initialize the sceneGraphSystem
		}
		~Scene() = default;

		void initialize(const float fov, const float nearPlane, const float farPlane, const float aspectRatio);//after the scene has been cleared, call this function to initialize a new scene
		void clear();//delete absolutely everything from the scene

		inline const perspectiveCamera& getCamera() const { return m_camControl.getCam(); }

		//entity stuff
		inline Entity createEntity() { return m_registry.create(); }
		inline void setParent(Entity e, Entity newParent) { m_sceneGraph->setParent(e, newParent); }
		inline Registry<componentTypeCount, systemCount>& Reg() { return m_registry; }//TEMPORARY
		//END entity stuff

		void addMaterial(const Ref_ptr<material>& mat);
		void addMaterial(const Ref_ptr<material>& mat, const std::function<void(const void* updateValues, material* mat)>& updateFunc, const void* updateValues);

		void setSkybox(const Ref_ptr<texture>& skybox) { m_skyboxTex = skybox; m_matLib.get("skyboxMat")->setTexture("u_skybox", m_skyboxTex); }
		inline WeakRef_ptr<texture> getSkyboxTex() const { return m_skyboxTex; }

		void onUpdate(timestep& ts, const bool rotateCam);//in this on-update-function one can implement certain routines that should be done like moving an object in a circle(or changing a light's color)
		void onRender();
		void onEvent(Event& e);//process events that the scene should react on
		void onImGuiRender();/*in this function automatically create a window that lists all of the scene's elements
		one can then click on one of the objects to get more details as well as the option to modify the values*/
	private:
		materialLib m_matLib;
		
		Registry<componentTypeCount, systemCount> m_registry;
		ObjectSubmitSystem* m_oss = nullptr;//system responsable for submitting all of the registry's component's that need to be drawn
		SceneGraphSystem* m_sceneGraph = nullptr;//system responsable for maintaining a hierarchy of everything with a transformComponent

		PerspectiveCameraController m_camControl;

		//weak reference to the skyboxTexture so that we don't always have to look it up
		WeakRef_ptr<texture> m_skyboxTex;
	};
}