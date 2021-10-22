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
		m_meshes.push_back(mesh::create(Renderer::getSkyboxVa(), skyboxMat, "skybox"));
	}

	void Scene::clear(LightManager* lightManager)
	{
		m_camControl.clear();
		m_meshes.clear();//is it actually safe to delete like this?
		m_dirLightMovements.clear();
		m_pointLightMovements.clear();
		m_spotLightMovements.clear();
		m_dirLightEffects.clear();
		m_pointLightEffects.clear();
		m_spotLightEffects.clear();
		m_dirLightAnimations.clear();
		m_pointLightAnimations.clear();
		m_spotLightAnimations.clear();
		m_matLib.clear();
		m_dirLights.clear();
		m_pointLights.clear();
		m_spotLights.clear();
		lightManager->clear();
	}

	void Scene::onUpdate(timestep& ts, const bool rotateCam)
	{
		m_camControl.onUpdate(ts, rotateCam);
		for (auto& mesh : m_meshes)
		{
			mesh->onUpdate(ts);
		}
		for (const auto& movement : m_dirLightMovements)
		{
			movement->onUpdate(ts);
		}
		for (const auto& movement : m_pointLightMovements)
		{
			movement->onUpdate(ts);
		}
		for (const auto& movement : m_spotLightMovements)
		{
			movement->onUpdate(ts);
		}
		for (auto& effect : m_dirLightEffects)
		{
			effect->onUpdate();
		}
		for (auto& effect : m_pointLightEffects)
		{
			effect->onUpdate();
		}
		for (auto& effect : m_spotLightEffects)
		{
			effect->onUpdate();
		}
		for (auto& animation : m_dirLightAnimations)
		{
			animation->onUpdate();
		}
		for (auto& animation : m_pointLightAnimations)
		{
			animation->onUpdate();
		}
		for (auto& animation : m_spotLightAnimations)
		{
			animation->onUpdate();
		}
		m_matLib.updateDynamic();//update the dynamicMaterials
	}

	void Scene::onEvent(Event& e)
	{
		m_camControl.onEvent(e);
	}

	void Scene::onImGuiRender()
	{
		ImGui::Begin("Meshes");
		for (auto& Mesh : m_meshes)
		{
			bool isOpen = ImGui::TreeNode(Mesh->getName().c_str());
			if (isOpen)
			{
				vec3 scale = Mesh->getScale();
				vec4 pos = Mesh->getPos();
				vec3 rot = Mesh->getRot();
				ImGui::SliderFloat3("scale", (float*)&scale, 0.1f, 10.0f);
				ImGui::DragFloat3("position", (float*)&pos, 0.25f, -100.0f, 100.0f);
				ImGui::DragFloat3("rotation", (float*)&rot, 0.25f, -2.0f * 3.1415926f, 2.0f * 3.1415926f);
				ImGui::TreePop();
				Mesh->setScale(scale);
				Mesh->setPos(pos);
				Mesh->setRot(rot);
			}
		}
		ImGui::End();
		ImGui::Begin("Lights");
		uint8_t index = 0;
		char dirLight_str[] = "directional light[   ]";
		for (const auto& light : m_dirLights)
		{
			std::to_chars(dirLight_str + 18U, dirLight_str + strlen(dirLight_str), index);
			bool isOpen = ImGui::TreeNode(dirLight_str);
			if (isOpen)
			{
				ImGui::ColorEdit3("ambient", (float*)&light->ambient);
				ImGui::ColorEdit3("diffuse", (float*)&light->diffuse);
				ImGui::ColorEdit3("specular", (float*)&light->specular);
				vec4 direction = light->direction;
				ImGui::DragFloat3("direction", (float*)&direction, 0.25f, -1000.0f, 1000.0f);
				light->direction = vec4::normalized(direction);
				ImGui::TreePop();
			}
			index++;
		}
		index = 0;
		char pointLight_str[] = "point light[   ]";
		for (const auto& light : m_pointLights)
		{
			std::to_chars(pointLight_str + 12U, pointLight_str + strlen(pointLight_str), index);
			bool isOpen = ImGui::TreeNode(pointLight_str);
			if (isOpen)
			{
				ImGui::ColorEdit3("ambient", (float*)&light->ambient);
				ImGui::ColorEdit3("diffuse", (float*)&light->diffuse);
				ImGui::ColorEdit3("specular", (float*)&light->specular);
				ImGui::DragFloat3("position", (float*)&light->position, 0.25f, -100.0f, 100.0f);
				ImGui::TreePop();
			}
			index++;
		}
		index = 0;
		char spotLight_str[] = "spot light[   ]";
		for (const auto& light : m_spotLights)
		{
			std::to_chars(spotLight_str + 11U, spotLight_str + strlen(spotLight_str), index);
			bool isOpen = ImGui::TreeNode(spotLight_str);
			if (isOpen)
			{
				ImGui::ColorEdit3("ambient", (float*)&light->ambient);
				ImGui::ColorEdit3("diffuse", (float*)&light->diffuse);
				ImGui::ColorEdit3("specular", (float*)&light->specular);
				ImGui::DragFloat3("position", (float*)&light->position, 0.25f, -100.0f, 100.0f);
				vec4 direction = light->direction;
				ImGui::DragFloat3("direction", (float*)&direction, 0.25f, -1000.0f, 1000.0f);
				light->direction = vec4::normalized(direction);
				ImGui::SliderFloat("cutoff", &light->cutOff, cosf(80.0f * (3.1415926f / 180.0f)), 1.0f);
				ImGui::TreePop();
			}
			index++;
		}
		ImGui::End();
		const vec4& camPos = m_camControl.getPos();
		const vec3& camRot = m_camControl.getRot();
		ImGui::Begin("Camera");
		ImGui::Text("position:");
		ImGui::Text("x: %.1f  y: %.1f  z: %.1f", camPos.x, camPos.y, camPos.z);
		ImGui::Text("rotation:");
		ImGui::Text("x: %.1f  y: %.1f  z: %.1f", camRot.x, camRot.y, camRot.z);
		ImGui::End();

		//Render Materials
		m_matLib.onImGuiRender();
	}

	void Scene::addMaterial(const Ref_ptr<material>& mat)
	{
		m_matLib.add(mat);
	}

	void Scene::addMaterial(const Ref_ptr<material>& mat, const std::function<void(const void* updateValues, material* mat)>& updateFunc, const void* updateValues)
	{
		m_matLib.addDynamic(mat, updateFunc, updateValues);
	}

	void Scene::addMesh(Ref_ptr<mesh>& Mesh)
	{
		m_meshes.push_back(Mesh);
	}

	void Scene::addLight(PtrPtr<directionalLight> light)
	{
		m_dirLights.push_back(light);
	}
	void Scene::addLight(PtrPtr<pointLight> light)
	{
		m_pointLights.push_back(light);
	}
	void Scene::addLight(PtrPtr<spotLight> light)
	{
		m_spotLights.push_back(light);
	}
}