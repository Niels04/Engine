#include "Engpch.hpp"

#include "lights.hpp"

#define DirLightIndexToElementIndex(x) x * (7 + 1)//with 7 being the number of elements that make up a directional light
#define PointLightIndexToElementIndex(x) x * (5 + 6/*cuz a pointLight has 6 transformation-matrices*/)
#define SpotLightIndexToElementIndex(x) x * (6 + 1)//+1 cuz of the toLightSpaceMatrix

#define staticDirLightElementCount DirLightIndexToElementIndex(m_staticDirLights.size())
#define dynamicDirLightElementCount DirLightIndexToElementIndex(m_dynamicDirLights.size())
#define staticPointLightElementCount PointLightIndexToElementIndex(m_staticPointLights.size())
#define dynamicPointLightElementCount PointLightIndexToElementIndex(m_dynamicPointLights.size())
#define staticSpotLightElementCount SpotLightIndexToElementIndex(m_staticSpotLights.size())
#define dynamicSpotLightElementCount SpotLightIndexToElementIndex(m_dynamicSpotLights.size())

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
		/*mat4 reflect;
		reflect.setReflectMat(mat4::reflectType::z);
		for (uint8_t i = 0; i < 3; i++)
		{
			for (uint8_t j = 0; j < 8; j++)
			{
				frustaPoints_viewSpace[i][j] = (reflect * vec4(frustaPoints_viewSpace[i][j], 1.0f)).xyz();
			}
		}*/
	}

	void LightManager::init()
	{
		m_depthFrameBuffer = FrameBuffer::create();
		m_depthFrameBuffer->initShadow();//tell the rendering-api(opengl) that we don't intend on drawing any colours to this
		m_depthShader_dir = shader::create("shadow/toDepthMap_dir.shader");
		m_depthShader_point = shader::create("shadow/toDepthMap_point.shader");
		m_depthShader_spot = shader::create("shadow/toDepthMap_spot.shader");

		const int32_t initialLightCount = 0;//by default there is no light source
		directionalLightBuffer = globalBuffer::createUnique(sizeof(int)  * 4/*x4 because of padding*/ + MAX_DIR_LIGHTS * sizeof(std::pair<directionalLight, cascadedDirLightMatrices>), DYNAMIC_DRAW);//add an int at the beginning for the count
		directionalLightBuffer->lAddIntB();//integer for count
		for (uint8_t i = 0; i < MAX_DIR_LIGHTS; i++)
		{
			directionalLightBuffer->lAddVec4B();
			directionalLightBuffer->lAddVec4B();
			directionalLightBuffer->lAddVec4B();
			directionalLightBuffer->lAddVec4B();
			//cascaded toLightSpace Matrices
			directionalLightBuffer->lAddMat4B();
			directionalLightBuffer->lAddMat4B();
			directionalLightBuffer->lAddMat4B();
		}
		directionalLightBuffer->bindToPoint(DIRECTIONAL_LIGHTS_BIND);
		directionalLightBuffer->updateElement(0, &initialLightCount);
		directionalLightBuffer->unbind();
		pointLightBuffer = globalBuffer::createUnique(sizeof(int) * 4/*x4 because of padding*/ + MAX_POINT_LIGHTS * sizeof(std::pair<pointLight, pointLightMatrices>), DYNAMIC_DRAW);
		pointLightBuffer->lAddIntB();//integer for count
		for (uint8_t i = 0; i < MAX_POINT_LIGHTS; i++)
		{
			pointLightBuffer->lAddVec4B();
			pointLightBuffer->lAddVec4B();
			pointLightBuffer->lAddVec4B();
			pointLightBuffer->lAddVec4B();
			pointLightBuffer->lAddVec3B();
			//toLightSpaceMatrix
			pointLightBuffer->lAddMat4B();
			pointLightBuffer->lAddMat4B();
			pointLightBuffer->lAddMat4B();
			pointLightBuffer->lAddMat4B();
			pointLightBuffer->lAddMat4B();
			pointLightBuffer->lAddMat4B();
		}
		pointLightBuffer->bindToPoint(POINT_LIGHTS_BIND);
		pointLightBuffer->updateElement(0, &initialLightCount);
		pointLightBuffer->unbind();
		spotLightBuffer = globalBuffer::createUnique(sizeof(int)  * 4 /*x4 because of padding*/+ (MAX_SPOT_LIGHTS * sizeof(std::pair<spotLight, mat4>)), DYNAMIC_DRAW);
		spotLightBuffer->lAddIntB();
		for (uint8_t i = 0; i < MAX_SPOT_LIGHTS; i++)
		{
			spotLightBuffer->lAddVec4B();
			spotLightBuffer->lAddVec4B();
			spotLightBuffer->lAddVec4B();
			spotLightBuffer->lAddVec4B();
			spotLightBuffer->lAddVec4B();
			spotLightBuffer->lAddVec4B();//for padding
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

	void LightManager::clear()
	{
		const int32_t light_count = 0;
		directionalLightBuffer->bind();
		directionalLightBuffer->updateElement(0, &light_count);
		pointLightBuffer->bind();
		pointLightBuffer->updateElement(0, &light_count);
		spotLightBuffer->bind();
		spotLightBuffer->updateElement(0, &light_count);
		spotLightBuffer->unbind();
		m_staticDirLights.clear();
		m_staticPointLights.clear();
		m_staticSpotLights.clear();
		m_dynamicDirLights.clear();
		m_dynamicPointLights.clear();
		m_dynamicSpotLights.clear();
		m_staticDirLightPtrs.clear();
		m_staticPointLightPtrs.clear();
		m_staticSpotLightPtrs.clear();
		m_dynamicDirLightPtrs.clear();
		m_dynamicPointLightPtrs.clear();
		m_dynamicSpotLightPtrs.clear();
		//could initialize all gpu-memory to 0 here, but im not goin to bother with that, since there is no reason, the old data will just be overwritten
	}

	void LightManager::rmStaticDirLight(directionalLight* toRemove)
	{
		auto it = m_staticDirLights.begin();
		auto itP = m_staticDirLightPtrs.begin();
		for (; it != m_staticDirLights.end(); it++, itP++)
		{
			if (it->first.uid == toRemove->uid)
			{
				it = m_staticDirLights.erase(it);
				m_staticDirLightPtrs.erase(itP);
				goto ELEMENT_FOUND;
			}
		}
		//output some error
		return;
	ELEMENT_FOUND:
		updateStaticDirLights(it);
		updateDynamicDirLights();
		toRemove = NULL;
		return;
	}

	void LightManager::rmStaticPointLight(pointLight* toRemove)
	{
		auto it = m_staticPointLights.begin();
		auto itP = m_staticPointLightPtrs.begin();
		for (; it != m_staticPointLights.end(); it++, itP++)
		{
			if (it->first.uid == toRemove->uid)
			{
				it = m_staticPointLights.erase(it);
				m_staticPointLightPtrs.erase(itP);
				goto ELEMENT_FOUND;
			}
		}
		//output some error
		return;
	ELEMENT_FOUND:
		//::::::UPDATE ALL STATIC & DYNAMIC ELEMENTS
		updateStaticPointLights(it);
		updateDynamicPointLights();
		toRemove = NULL;
		return;
	}

	void LightManager::rmStaticSpotLight(spotLight* toRemove)
	{
		auto it = m_staticSpotLights.begin();
		auto itP = m_staticSpotLightPtrs.begin();
		for (; it != m_staticSpotLights.end(); it++, itP++)
		{
			if (it->first.uid == toRemove->uid)
			{
				it = m_staticSpotLights.erase(it);
				m_staticSpotLightPtrs.erase(itP);
				goto ELEMENT_FOUND;
			}
		}
		//output some error
		return;
	ELEMENT_FOUND:
		//::::::UPDATE ALL STATIC & DYNAMIC ELEMENTS
		updateStaticSpotLights(it);
		updateDynamicSpotLights();
		toRemove = NULL;
		return;
	}

	void LightManager::rmDynamicDirLight(directionalLight* toRemove)
	{
		auto it = m_dynamicDirLights.begin();
		auto itP = m_dynamicDirLightPtrs.begin();
		for (; it != m_dynamicDirLights.end(); it++, itP++)
		{
			if (it->first.uid == toRemove->uid)
			{
				it = m_dynamicDirLights.erase(it);
				m_dynamicDirLightPtrs.erase(itP);
				goto ELEMENT_FOUND;
			}
		}
		//output some error
		return;
	ELEMENT_FOUND:
		//:::::::UPDATE ALL DYNAMIC ELEMENTS
		updateDynamicDirLights(it);
		toRemove = NULL;
		return;
	}

	void LightManager::rmDynamicPointLight(pointLight* toRemove)
	{
		auto it = m_dynamicPointLights.begin();
		auto itP = m_dynamicPointLightPtrs.begin();
		for (; it != m_dynamicPointLights.end(); it++, itP++)
		{
			if (it->first.uid == toRemove->uid)
			{
				it = m_dynamicPointLights.erase(it);
				m_dynamicPointLightPtrs.erase(itP);
				goto ELEMENT_FOUND;
			}
		}
		//output some error
		return;
	ELEMENT_FOUND:
		//:::::::UPDATE ALL DYNAMIC ELEMENTS
		updateDynamicPointLights(it);
		toRemove = NULL;
		return;
	}

	void LightManager::rmDynamicSpotLight(spotLight* toRemove)
	{
		auto it = m_dynamicSpotLights.begin();
		auto itP = m_dynamicSpotLightPtrs.begin();
		for (; it != m_dynamicSpotLights.end(); it++, itP++)
		{
			if (it->first.uid == toRemove->uid)
			{
				it = m_dynamicSpotLights.erase(it);
				m_dynamicSpotLightPtrs.erase(itP);
				goto ELEMENT_FOUND;
			}
		}
		//output some error
		return;
	ELEMENT_FOUND:
		//:::::::UPDATE ALL DYNAMIC ELEMENTS
		updateDynamicSpotLights(it);
		toRemove = NULL;
		return;
	}

	void LightManager::updateNewDirLight(bool Static)
	{
		directionalLightBuffer->bind();
		if (Static)
		{
			directionalLightBuffer->updateFromTo((1 + staticDirLightElementCount) - DirLightIndexToElementIndex(1)/*first element of the last static dirLight*/,
				staticDirLightElementCount/*last element of the last static dirLight <- don't add 1 because one would also have to subract 1 when getting last index from elementCount*/,
				&m_staticDirLights.back());//1+ because the first index always is the number of lights per type
			directionalLightBuffer->unbind();
			return;
		}
		directionalLightBuffer->updateFromTo((1 + staticDirLightElementCount + dynamicDirLightElementCount) - DirLightIndexToElementIndex(1),
			staticDirLightElementCount + dynamicDirLightElementCount, &m_dynamicDirLights.back());
		directionalLightBuffer->unbind();
	}

	void LightManager::updateDynamicDirLights()
	{
		if (m_dynamicDirLights.size() == 0)
			return;
		directionalLightBuffer->bind();
		directionalLightBuffer->updateFromTo(1 + staticDirLightElementCount, staticDirLightElementCount + dynamicDirLightElementCount, &m_dynamicDirLights.front());
		directionalLightBuffer->unbind();
	}

	void LightManager::updateStaticDirLights(std::vector<std::pair<directionalLight, cascadedDirLightMatrices>>::iterator it)
	{
		uint8_t index = it - m_staticDirLights.begin();
		directionalLightBuffer->bind();
		directionalLightBuffer->updateFromTo(1 + DirLightIndexToElementIndex(index), staticDirLightElementCount, &m_staticDirLights[index]);
		directionalLightBuffer->unbind();
	}

	void LightManager::updateDynamicDirLights(std::vector<std::pair<directionalLight, cascadedDirLightMatrices>>::iterator it)
	{
		uint8_t index = it - m_dynamicDirLights.begin();
		directionalLightBuffer->bind();
		directionalLightBuffer->updateFromTo(1 + staticDirLightElementCount + DirLightIndexToElementIndex(index), staticDirLightElementCount + dynamicDirLightElementCount,
			&m_dynamicDirLights[index]);
		directionalLightBuffer->unbind();
	}
	//pointLight
	void LightManager::updateNewPointLight(bool Static)
	{
		pointLightBuffer->bind();
		if (Static)
		{
			pointLightBuffer->updateFromTo((1 + staticPointLightElementCount) - PointLightIndexToElementIndex(1),
				staticPointLightElementCount, &m_staticPointLights.back());
			pointLightBuffer->unbind();
			return;
		}
		pointLightBuffer->updateFromTo((1 + staticPointLightElementCount + dynamicPointLightElementCount) - PointLightIndexToElementIndex(1),
			staticPointLightElementCount + dynamicPointLightElementCount, &m_dynamicPointLights.back());
		pointLightBuffer->unbind();
	}

	void LightManager::updateDynamicPointLights()
	{
		if (m_dynamicPointLights.size() == 0)
			return;
		pointLightBuffer->bind();
		pointLightBuffer->updateFromTo(1 + staticPointLightElementCount, staticPointLightElementCount + dynamicPointLightElementCount, &m_dynamicPointLights.front());
		pointLightBuffer->unbind();
	}

	void LightManager::updateStaticPointLights(std::vector<std::pair<pointLight, pointLightMatrices>>::iterator it)
	{
		uint8_t index = it - m_staticPointLights.begin();
		pointLightBuffer->bind();
		pointLightBuffer->updateFromTo(1 + PointLightIndexToElementIndex(index), staticPointLightElementCount, &m_staticPointLights[index]);
		pointLightBuffer->unbind();
	}

	void LightManager::updateDynamicPointLights(std::vector<std::pair<pointLight, pointLightMatrices>>::iterator it)
	{
		uint8_t index = it - m_dynamicPointLights.begin();
		pointLightBuffer->bind();
		pointLightBuffer->updateFromTo(1 + staticPointLightElementCount + PointLightIndexToElementIndex(index), staticPointLightElementCount + dynamicPointLightElementCount,
			&m_dynamicPointLights[index]);
		pointLightBuffer->unbind();
	}
	//spotLight
	void LightManager::updateNewSpotLight(bool Static)
	{
		spotLightBuffer->bind();
		if (Static)
		{
			spotLightBuffer->updateFromTo((1 + staticSpotLightElementCount) - SpotLightIndexToElementIndex(1),
				staticSpotLightElementCount, &m_staticSpotLights.back());
			spotLightBuffer->unbind();
			return;
		}
		spotLightBuffer->updateFromTo((1 + staticSpotLightElementCount + dynamicSpotLightElementCount) - SpotLightIndexToElementIndex(1),
			staticSpotLightElementCount + dynamicSpotLightElementCount, &m_dynamicSpotLights.back());
		spotLightBuffer->unbind();
	}

	void LightManager::updateDynamicSpotLights()
	{
		if (m_dynamicSpotLights.size() == 0)
			return;
		spotLightBuffer->bind();
		spotLightBuffer->updateFromTo(1 + staticSpotLightElementCount, staticSpotLightElementCount + dynamicSpotLightElementCount, &m_dynamicSpotLights.front());
		spotLightBuffer->unbind();
	}

	void LightManager::updateStaticSpotLights(std::vector<std::pair<spotLight, mat4>>::iterator it)
	{
		uint8_t index = it - m_staticSpotLights.begin();
		spotLightBuffer->bind();
		spotLightBuffer->updateFromTo(1 + SpotLightIndexToElementIndex(index), staticSpotLightElementCount, &m_staticSpotLights[index]);
		spotLightBuffer->unbind();
	}

	void LightManager::updateDynamicSpotLights(std::vector<std::pair<spotLight, mat4>>::iterator it)
	{
		uint8_t index = it - m_dynamicSpotLights.begin();
		spotLightBuffer->bind();
		spotLightBuffer->updateFromTo(1 + staticSpotLightElementCount + SpotLightIndexToElementIndex(index), staticSpotLightElementCount + dynamicSpotLightElementCount,
			&m_dynamicSpotLights[index]);
		spotLightBuffer->unbind();
	}

	void LightManager::updateDynamicLightMatrices()
	{
		for (auto& light : m_dynamicDirLights)
		{
			light.second = lightMatrixCalculator::getMatrix(light.first);
		}
		for (auto& light : m_dynamicPointLights)
		{
			light.second = lightMatrixCalculator::getMatrix(light.first);
		}
		for (auto& light : m_dynamicSpotLights)
		{
			light.second = mat4::transposed(lightMatrixCalculator::getMatrix(light.first));
		}
	}
}