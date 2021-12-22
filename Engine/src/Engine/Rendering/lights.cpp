#include "Engpch.hpp"

#include "lights.hpp"

#include "imgui.h"

#define DirLightIndexToElementIndex(x) (x * (4 + 3)) + 1//+1 because of the first element that stores count
#define PointLightIndexToElementIndex(x) (x * (5 + 6/*cuz a pointLight has 6 transformation-matrices*/)) + 1//same
#define SpotLightIndexToElementIndex(x) (x * (5 + 1/*+1 because of the toLightSpace mat*/)) + 1//same

#define DirLightLastElementIndex DirLightIndexToElementIndex(m_registry.getAllOf<DirLightComponent>().size()) - 1
#define PointLightLastElementIndex PointLightIndexToElementIndex(m_registry.getAllOf<PointLightComponent>().size()) - 1
#define SpotLightLastElementIndex SpotLightIndexToElementIndex(m_registry.getAllOf<SpotLightComponent>().size()) - 1

namespace Engine
{
	mat4 lightMatrixCalculator::s_projPoint = mat4::projMat(POINT_LIGHT_CAM);
	mat4 lightMatrixCalculator::s_projSpot = mat4::projMat(SPOT_LIGHT_CAM);
	float lightMatrixCalculator::s_fovs[2] = { 0.0f };
	mat4 lightMatrixCalculator::s_INV_viewMat = mat4::identity();
	vec3 lightMatrixCalculator::frustaPoints_viewSpace[3][8] = { {} };
	vec3 lightMatrixCalculator::frustaPoints_worldSpace[3][8] = { {} };

	void lightMatrixCalculator::init(const float hFov, const float vFov, const float nearPlane, const float farPlane)
	{
		//initialize cascaded shadowMap boundingboxes
		s_fovs[0] = hFov; s_fovs[1] = vFov;
		const float Nn = nearPlane;//the near cascade's nearPlanes is the camera's nearPlane
		const float Nf = farPlane * 0.3333333f;
		const float Mn = Nf;//middle cascade's nearPlane
		const float Mf = farPlane * 0.6666666f;
		const float Fn = Mf;
		const float Ff = farPlane;
		const float tanFovH = tanf(RAD(hFov * 0.5f));
		const float tanFovV = tanf(RAD(vFov * 0.5f));
		frustaPoints_viewSpace[0][0] = { Nn * tanFovH, 0.0f, -Nn };//near-cascade | nearPlane | positive x
		frustaPoints_viewSpace[0][1] = { -frustaPoints_viewSpace[0][0].x, 0.0f, -Nn };//near-cascade | nearPlane | negative x
		frustaPoints_viewSpace[0][2] = { 0.0f, Nn * tanFovV, -Nn };//near-cascade | nearPlane | positive y
		frustaPoints_viewSpace[0][3] = { 0.0f, -frustaPoints_viewSpace[0][2].y, -Nn };//near-cascade | nearPlane | negative y
		frustaPoints_viewSpace[0][4] = { Nf * tanFovH, 0.0f, -Nf };//near-cascade | farPlane | positive x
		frustaPoints_viewSpace[0][5] = { -frustaPoints_viewSpace[0][4].x, 0.0f, -Nf };//near-cascade | farPlane | negative x
		frustaPoints_viewSpace[0][6] = { 0.0f, Nf * tanFovV, -Nf };//near-cascade | farPlane | positive y
		frustaPoints_viewSpace[0][7] = { 0.0f, -frustaPoints_viewSpace[0][6].y, -Nf };//near-cascade | farPlane | negative y
		frustaPoints_viewSpace[1][0] = { Mn * tanFovH, 0.0f, -Mn };//middle-cascade | nearPlane | positive x
		frustaPoints_viewSpace[1][1] = { -frustaPoints_viewSpace[1][0].x, 0.0f, -Mn };//middle-cascade | nearPlane | negative x
		frustaPoints_viewSpace[1][2] = { 0.0f, Mn * tanFovV, -Mn };//middle-cascade | nearPlane | positive y
		frustaPoints_viewSpace[1][3] = { 0.0f, -frustaPoints_viewSpace[1][2].y, -Mn };//middle-cascade | nearPlane | negative y
		frustaPoints_viewSpace[1][4] = { Mf * tanFovH, 0.0f, -Mf };//middle-cascade | farPlane | positive x
		frustaPoints_viewSpace[1][5] = { -frustaPoints_viewSpace[1][4].x, 0.0f, -Mf };//middle-cascade | farPlane | negative x
		frustaPoints_viewSpace[1][6] = { 0.0f, Mf * tanFovV, -Mf };//middle-cascade | farPlane | positive y
		frustaPoints_viewSpace[1][7] = { 0.0f, -frustaPoints_viewSpace[1][6].y, -Mf };//middle-cascade | farPlane | negative y
		frustaPoints_viewSpace[2][0] = { Fn * tanFovH, 0.0f, -Fn };//far-cascade | nearPlane | positive x
		frustaPoints_viewSpace[2][1] = { -frustaPoints_viewSpace[2][0].x, 0.0f, -Fn };//far-cascade | nearPlane | negative x
		frustaPoints_viewSpace[2][2] = { 0.0f, Fn * tanFovV, -Fn };//far-cascade | nearPlane | positive y
		frustaPoints_viewSpace[2][3] = { 0.0f, -frustaPoints_viewSpace[2][2].y, -Fn };//far-cascade | nearPlane | negative y
		frustaPoints_viewSpace[2][4] = { Ff * tanFovH, 0.0f, -Ff };//far-cascade | farPlane | positive x
		frustaPoints_viewSpace[2][5] = { -frustaPoints_viewSpace[2][4].x, 0.0f, -Ff };//far-cascade | farPlane | negative x
		frustaPoints_viewSpace[2][6] = { 0.0f, Ff * tanFovV, -Ff };//far-cascade | farPlane | positive y
		frustaPoints_viewSpace[2][7] = { 0.0f, -frustaPoints_viewSpace[2][6].y, -Ff };//far-cascade | farPlane | negative y
	}

	void LightRenderingSystem::init()
	{
		m_depthFrameBuffer = FrameBuffer::create();
		m_depthFrameBuffer->initShadow();//tell the rendering-api(opengl) that we don't intend on drawing any colours to this
		m_depthShader_dir = shader::create("shadow/toDepthMap_dir.shader");
		m_depthShader_point = shader::create("shadow/toDepthMap_point.shader");
		m_depthShader_spot = shader::create("shadow/toDepthMap_spot.shader");

		const int32_t initialLightCount = 0;//by default there is no light source
		directionalLightBuffer = globalBuffer::createUnique(sizeof(int32_t)  * 4/*x4 because of padding*/ + MAX_DIR_LIGHTS * sizeof(DirLightComponent), DYNAMIC_DRAW);//add an int at the beginning for the count
		directionalLightBuffer->lAddIntB();//integer for count
		for (uint8_t i = 0; i < MAX_DIR_LIGHTS; i++)
		{
			directionalLightBuffer->lAddVec4B();//direction
			directionalLightBuffer->lAddVec4B();//ambient
			directionalLightBuffer->lAddVec4B();//diffuse
			directionalLightBuffer->lAddVec4B();//specular
			//cascaded toLightSpace Matrices
			directionalLightBuffer->lAddMat4B();//nearCascade
			directionalLightBuffer->lAddMat4B();//middleCascade
			directionalLightBuffer->lAddMat4B();//farCascade
		}
		directionalLightBuffer->bindToPoint(DIRECTIONAL_LIGHTS_BIND);
		directionalLightBuffer->updateElement(0, &initialLightCount);
		directionalLightBuffer->unbind();
		pointLightBuffer = globalBuffer::createUnique(sizeof(int32_t) * 4/*x4 because of padding*/ + MAX_POINT_LIGHTS * sizeof(PointLightComponent), DYNAMIC_DRAW);
		pointLightBuffer->lAddIntB();//integer for count
		for (uint8_t i = 0; i < MAX_POINT_LIGHTS; i++)
		{
			pointLightBuffer->lAddVec4B();//position
			pointLightBuffer->lAddVec4B();//ambient
			pointLightBuffer->lAddVec4B();//diffuse
			pointLightBuffer->lAddVec4B();//specular
			pointLightBuffer->lAddVec4B();//attenuation(w is padding)
			//toLightSpaceMatrices
			pointLightBuffer->lAddMat4B();//positiveX
			pointLightBuffer->lAddMat4B();//negativeX
			pointLightBuffer->lAddMat4B();//positiveY
			pointLightBuffer->lAddMat4B();//negativeY
			pointLightBuffer->lAddMat4B();//positiveZ
			pointLightBuffer->lAddMat4B();//negativeZ
		}
		pointLightBuffer->bindToPoint(POINT_LIGHTS_BIND);
		pointLightBuffer->updateElement(0, &initialLightCount);
		pointLightBuffer->unbind();
		spotLightBuffer = globalBuffer::createUnique(sizeof(int32_t) * 4 /*x4 because of padding*/+ (MAX_SPOT_LIGHTS * sizeof(SpotLightComponent)), DYNAMIC_DRAW);
		spotLightBuffer->lAddIntB();
		for (uint8_t i = 0; i < MAX_SPOT_LIGHTS; i++)
		{
			spotLightBuffer->lAddVec4B();//position
			spotLightBuffer->lAddVec4B();//direction
			spotLightBuffer->lAddVec4B();//ambient
			spotLightBuffer->lAddVec4B();//diffuse
			spotLightBuffer->lAddVec4B();//specular(w is cutOff)
			spotLightBuffer->lAddMat4B();//toLightSpaceMatrix
		}
		spotLightBuffer->bindToPoint(SPOT_LIGHTS_BIND);
		spotLightBuffer->updateElement(0, &initialLightCount);
		spotLightBuffer->unbind();
		//now allocate memory for the shadow maps:::
		m_dirLightMapArray = ShadowMap2dArray::create(Shadow_Width, Shadow_Height, MAX_DIR_LIGHTS * 3/*with 3 being the number of cascades*/);//<- will have to be carefull to not over-allocate here, cuz this takes a whole lot of memory
		m_pointLightMapArray = ShadowMap3dArray::create(Shadow_Width, MAX_POINT_LIGHTS);
		m_spotLightMapArray = ShadowMap2dArray::create(Shadow_Width, Shadow_Height, MAX_SPOT_LIGHTS);
	}

	void LightRenderingSystem::clear()
	{
		const int32_t light_count = 0;
		directionalLightBuffer->bind();
		directionalLightBuffer->updateElement(0, &light_count);
		pointLightBuffer->bind();
		pointLightBuffer->updateElement(0, &light_count);
		spotLightBuffer->bind();
		spotLightBuffer->updateElement(0, &light_count);
		spotLightBuffer->unbind();
		//could initialize all gpu-memory to 0 here, but im not goin to bother with that, since there is no reason, the old data will just be overwritten
	}

	void LightRenderingSystem::onImGuiRender()
	{
		ImGui::Begin("Lights");
		ImGui::Text("Directional lights:");
		for (auto& light : m_registry.getAllOf<DirLightComponent>())
		{
			char name[10] = { ' ' };
			sprintf(name, "%u", (uint32_t)m_registry.getOwner(light));
			bool isOpen = ImGui::TreeNode(name);
			if (isOpen)
			{
				ImGui::ColorEdit3("ambient", (float*)&light.ambient);
				ImGui::ColorEdit3("diffuse", (float*)&light.diffuse);
				ImGui::ColorEdit3("specular", (float*)&light.specular);
				ImGui::Text("direction: %.2f|%.2f|%.2f", light.direction.x, light.direction.y, light.direction.z);
				ImGui::TreePop();
			}
		}
		ImGui::Text("Point lights:");
		for (auto& light : m_registry.getAllOf<PointLightComponent>())
		{
			char name[10] = { ' ' };
			sprintf(name, "%u", (uint32_t)m_registry.getOwner(light));
			bool isOpen = ImGui::TreeNode(name);
			if (isOpen)
			{
				ImGui::ColorEdit3("ambient", (float*)&light.ambient);
				ImGui::ColorEdit3("diffuse", (float*)&light.diffuse);
				ImGui::ColorEdit3("specular", (float*)&light.specular);
				ImGui::Text("position: %.2f|%.2f|%.2f", light.position.x, light.position.y, light.position.z);
				ImGui::TreePop();
			}
		}
		ImGui::Text("Spot lights:");
		for (auto& light : m_registry.getAllOf<SpotLightComponent>())
		{
			char name[10] = { ' ' };
			sprintf(name, "%u", (uint32_t)m_registry.getOwner(light));
			bool isOpen = ImGui::TreeNode(name);
			if (isOpen)
			{
				ImGui::ColorEdit3("ambient", (float*)&light.ambient);
				ImGui::ColorEdit3("diffuse", (float*)&light.diffuse);
				ImGui::ColorEdit3("specular", (float*)&light.specular);
				ImGui::SliderFloat("cutoff", &light.cutOff, cosf(80.0f * (3.1415926f / 180.0f)), 1.0f);
				ImGui::Text("direction: %.2f|%.2f|%.2f", light.direction.x, light.direction.y, light.direction.z);
				ImGui::Text("position: %.2f|%.2f|%.2f", light.position.x, light.position.y, light.position.z);
				ImGui::TreePop();
			}
		}
		ImGui::End();
	}

	void LightRenderingSystem::flushDirLights()
	{
		if (m_registry.getAllOf<DirLightComponent>().size() == 0)
			return;
		directionalLightBuffer->bind();
		directionalLightBuffer->updateFromTo(1, DirLightLastElementIndex, (const void*)m_registry.getAllOf<DirLightComponent>().data());
		directionalLightBuffer->unbind();
	}

	void LightRenderingSystem::flushPointLights()
	{
		if (m_registry.getAllOf<PointLightComponent>().size() == 0)
			return;
		pointLightBuffer->bind();
		pointLightBuffer->updateFromTo(1, PointLightLastElementIndex, (const void*)m_registry.getAllOf<PointLightComponent>().data());
		pointLightBuffer->unbind();
	}

	void LightRenderingSystem::flushSpotLights()
	{
		if (m_registry.getAllOf<SpotLightComponent>().size() == 0)
			return;
		spotLightBuffer->bind();
		spotLightBuffer->updateFromTo(1, SpotLightLastElementIndex, (const void*)m_registry.getAllOf<SpotLightComponent>().data());
		spotLightBuffer->unbind();
	}

	void LightRenderingSystem::updateLightMatrices()
	{
		for (auto& light : m_registry.getAllOf<DirLightComponent>())//does this create copies of the vectors?
		{
			light.shadowMatrices = lightMatrixCalculator::getMatrix(light);
		}
		for (auto& light : m_registry.getAllOf<PointLightComponent>())
		{
			light.shadowMatrices = lightMatrixCalculator::getMatrix(light);
		}
		for (auto& light : m_registry.getAllOf<SpotLightComponent>())
		{
			light.shadowMat = mat4::transposed(lightMatrixCalculator::getMatrix(light));
		}
	}

	void LightRenderingSystem::updateLightCount()
	{
		int32_t dirLightCount = m_registry.getAllOf<DirLightComponent>().size();
		int32_t pointLightCount = m_registry.getAllOf<PointLightComponent>().size();
		int32_t spotLightCount = m_registry.getAllOf<SpotLightComponent>().size();
		directionalLightBuffer->bind();
		directionalLightBuffer->updateElement(0, &dirLightCount);
		directionalLightBuffer->unbind();
		pointLightBuffer->bind();
		pointLightBuffer->updateElement(0, &pointLightCount);
		pointLightBuffer->unbind();
		spotLightBuffer->bind();
		spotLightBuffer->updateElement(0, &spotLightCount);
		spotLightBuffer->unbind();
	}

	void LightRenderingSystem::applyTransforms()
	{
		for (auto& light : m_registry.getAllOf<DirLightComponent>())
		{
			auto& transform = m_registry.get<TransformComponent>(m_registry.getOwner(light));
			light.direction = transform.global * vec4(0.0f, 0.0f, -1.0f, 0.0f);
		}
		for (auto& light : m_registry.getAllOf<PointLightComponent>())
		{
			auto& transform = m_registry.get<TransformComponent>(m_registry.getOwner(light));
			light.position = transform.global * vec4(0.0f, 0.0f, 0.0f, 1.0f);
		}
		for (auto& light : m_registry.getAllOf<SpotLightComponent>())
		{
			auto& transform = m_registry.get<TransformComponent>(m_registry.getOwner(light));//for every entity with a spotLightComponent get it's transformComponent
			light.position = transform.global * vec4(0.0f, 0.0f, 0.0f, 1.0f);
			light.direction = transform.global * vec4(0.0f, 0.0f, -1.0f, 0.0f);
		}
	}
}