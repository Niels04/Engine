#include "Engpch.hpp"

#include "lights.hpp"

#define DirLightIndexToElementIndex(x) x * (4 + 1)//with 4 being the number of elements that make up a directional light
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
	mat4 lightMatrixCalculator::s_projDir = mat4::projMat(DIR_LIGHT_CAM);
	mat4 lightMatrixCalculator::s_projPoint = mat4::projMat(POINT_LIGHT_CAM);
	mat4 lightMatrixCalculator::s_projSpot = mat4::projMat(SPOT_LIGHT_CAM);

	void LightManager::init()
	{
		m_depthFrameBuffer = FrameBuffer::create();
		m_depthFrameBuffer->initShadow();//tell the rendering-api(opengl) that we don't intend on drawing any colours to this
		m_depthShader_dir = shader::create("shadow/toDepthMap_dir.shader");
		m_depthShader_point = shader::create("shadow/toDepthMap_point.shader");
		m_depthShader_spot = shader::create("shadow/toDepthMap_spot.shader");

		const int32_t initialLightCount = 0;//by default there is no light source
		directionalLightBuffer = globalBuffer::createUnique(sizeof(int)  * 4/*x4 because of padding*/ + MAX_LIGHTS_PER_TYPE * sizeof(std::pair<directionalLight, mat4>), STATIC_DRAW);//add an int at the beginning for the count
		directionalLightBuffer->lAddIntB();//integer for count
		for (uint8_t i = 0; i < MAX_LIGHTS_PER_TYPE; i++)
		{
			directionalLightBuffer->lAddVec4B();
			directionalLightBuffer->lAddVec4B();
			directionalLightBuffer->lAddVec4B();
			directionalLightBuffer->lAddVec4B();
			directionalLightBuffer->lAddMat4B();//toLightSpaceMatrix
		}
		directionalLightBuffer->bindToPoint(DIRECTIONAL_LIGHTS_BIND);
		directionalLightBuffer->updateElement(0, &initialLightCount);
		directionalLightBuffer->unbind();
		pointLightBuffer = globalBuffer::createUnique(sizeof(int) * 4/*x4 because of padding*/ + MAX_LIGHTS_PER_TYPE * sizeof(std::pair<pointLight, pointLightMatrices>), DYNAMIC_DRAW);
		pointLightBuffer->lAddIntB();//integer for count
		for (uint8_t i = 0; i < MAX_LIGHTS_PER_TYPE; i++)
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
		spotLightBuffer = globalBuffer::createUnique(sizeof(int)  * 4 /*x4 because of padding*/+ (MAX_LIGHTS_PER_TYPE * sizeof(std::pair<spotLight, mat4>)), DYNAMIC_DRAW);
		spotLightBuffer->lAddIntB();
		for (uint8_t i = 0; i < MAX_LIGHTS_PER_TYPE; i++)
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
		m_dirLightTextures.pop_back();
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
		m_pointLightTextures.pop_back();
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
		m_spotLightTextures.pop_back();
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
		m_dirLightTextures.pop_back();
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
		m_pointLightTextures.pop_back();
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
		m_spotLightTextures.pop_back();
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

	void LightManager::updateStaticDirLights(std::vector<std::pair<directionalLight, mat4>>::iterator it)
	{
		uint8_t index = it - m_staticDirLights.begin();
		directionalLightBuffer->bind();
		directionalLightBuffer->updateFromTo(1 + DirLightIndexToElementIndex(index), staticDirLightElementCount, &m_staticDirLights[index]);
		directionalLightBuffer->unbind();
	}

	void LightManager::updateDynamicDirLights(std::vector<std::pair<directionalLight, mat4>>::iterator it)
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
			light.second = mat4::transposed(lightMatrixCalculator::getMatrix(light.first));
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