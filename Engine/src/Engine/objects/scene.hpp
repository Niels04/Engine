#pragma once
#include <Engpch.hpp>
#include "mesh.hpp"
#include "Engine/Rendering/material.hpp"
#include "Engine/Rendering/lights.hpp"
#include "Engine/perspectiveCameraController.hpp"
#include "movement.hpp"
#include "light_effects.hpp"

namespace Engine
{
	class Scene
	{
		friend class NodeEditorLayer;
	public:
		Scene() = default;
		~Scene() = default;

		void initialize(const float fov, const float nearPlane, const float farPlane, const float aspectRatio);//after the scene has been cleared, call this function to initialize a new scene
		void clear(LightManager* lightManager);//delete absolutely everything from the scene

		inline const perspectiveCamera& getCamera() const { return m_camControl.getCam(); }

		void addMaterial(const Ref_ptr<material>& mat);
		void addMaterial(const Ref_ptr<material>& mat, const std::function<void(const void* updateValues, material* mat)>& updateFunc, const void* updateValues);
		void addMesh(Ref_ptr<mesh>& Mesh);
		void addLight(PtrPtr<directionalLight> light);
		void addLight(PtrPtr<pointLight> light);
		void addLight(PtrPtr<spotLight> light);

		void addLightMovement(const Ref_ptr<DirLightMovement>& movement) { m_dirLightMovements.push_back(movement); }
		void addLightMovement(const Ref_ptr<PointLightMovement>& movement) { m_pointLightMovements.push_back(movement); }
		void addLightMovement(const Ref_ptr<SpotLightMovement>& movement) { m_spotLightMovements.push_back(movement); }

		void addLightEffect(const Ref_ptr<dirLightEffect>& effect) { m_dirLightEffects.push_back(effect); }
		void addLightEffect(const Ref_ptr<pointLightEffect>& effect) { m_pointLightEffects.push_back(effect); }
		void addLightEffect(const Ref_ptr<spotLightEffect>& effect) { m_spotLightEffects.push_back(effect); }

		void addLightAnimation(const Ref_ptr<DirLightAnimation>& animation) { m_dirLightAnimations.push_back(animation); }
		void addLightAnimation(const Ref_ptr<PointLightAnimation>& animation) { m_pointLightAnimations.push_back(animation); }
		void addLightAnimation(const Ref_ptr<SpotLightAnimation>& animation) { m_spotLightAnimations.push_back(animation); }

		void setSkybox(const Ref_ptr<texture>& skybox) { m_skyboxTex = skybox; m_matLib.get("skyboxMat")->setTexture("u_skybox", m_skyboxTex); }
		inline WeakRef_ptr<texture> getSkyboxTex() const { return m_skyboxTex; }

		void onUpdate(timestep& ts, const bool rotateCam);//in this on-update-function one can implement certain routines that should be done like moving an object in a circle(or changing a light's color)
		void onEvent(Event& e);//process events that the scene should react on
		void onImGuiRender();/*in this function automatically create a window that lists all of the scene's elements
		one can then click on one of the objects to get more details as well as the option to modify the values*/

		inline std::vector<Ref_ptr<mesh>>::const_iterator meshes_begin() const { return m_meshes.begin(); }
		inline std::vector<Ref_ptr<mesh>>::const_iterator meshes_end() const { return m_meshes.end(); }
	private:
		materialLib m_matLib;
		std::vector<Ref_ptr<mesh>> m_meshes;
		
		std::vector<PtrPtr<directionalLight>> m_dirLights;
		std::vector<PtrPtr<pointLight>> m_pointLights;
		std::vector<PtrPtr<spotLight>> m_spotLights;
		//movements for lights
		std::vector<Ref_ptr<DirLightMovement>> m_dirLightMovements;
		std::vector<Ref_ptr<PointLightMovement>> m_pointLightMovements;
		std::vector<Ref_ptr<SpotLightMovement>> m_spotLightMovements;
		//light-effects
		std::vector<Ref_ptr<dirLightEffect>> m_dirLightEffects;
		std::vector<Ref_ptr<pointLightEffect>> m_pointLightEffects;
		std::vector<Ref_ptr<spotLightEffect>> m_spotLightEffects;
		//light-animations
		std::vector<Ref_ptr<DirLightAnimation>> m_dirLightAnimations;
		std::vector<Ref_ptr<PointLightAnimation>> m_pointLightAnimations;
		std::vector<Ref_ptr<SpotLightAnimation>> m_spotLightAnimations;

		PerspectiveCameraController m_camControl;

		//weak reference to the skyboxTexture so that we don't always have to look it up
		WeakRef_ptr<texture> m_skyboxTex;
	};
}