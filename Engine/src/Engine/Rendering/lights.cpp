#include "Engpch.hpp"

#include "lights.hpp"

namespace Engine
{
	void LightManager::init()
	{
		const int32_t initialLightCount = 0;//by default there is no light source
		directionalLightBuffer = globalBuffer::create(sizeof(int)  * 4/*x4 because of padding*/ + (MAX_LIGHTS_PER_TYPE * sizeof(directionalLight)), STATIC_DRAW);//add an int at the beginning for the count
		directionalLightBuffer->lAddIntB();//integer for count
		for (uint8_t i = 0; i < MAX_LIGHTS_PER_TYPE; i++)//a rather laborious way of specifying a layout -> maybe add some function to add a user-defined datatype to the layout
		{
			directionalLightBuffer->lAddVec4B();
			directionalLightBuffer->lAddVec4B();
			directionalLightBuffer->lAddVec4B();
			directionalLightBuffer->lAddVec4B();
		}
		directionalLightBuffer->bindToPoint(1);
		directionalLightBuffer->updateElement(0, &initialLightCount);
		directionalLightBuffer->unbind();
		pointLightBuffer = globalBuffer::create(sizeof(int) * 4/*x4 because of padding*/ + (MAX_LIGHTS_PER_TYPE * (sizeof(pointLight) + 4)), DYNAMIC_DRAW);
		pointLightBuffer->lAddIntB();//integer for count
		for (uint8_t i = 0; i < MAX_LIGHTS_PER_TYPE; i++)
		{
			pointLightBuffer->lAddVec4B();
			pointLightBuffer->lAddVec4B();
			pointLightBuffer->lAddVec4B();
			pointLightBuffer->lAddVec4B();
			pointLightBuffer->lAddVec3B();
		}
		pointLightBuffer->bindToPoint(2);
		pointLightBuffer->updateElement(0, &initialLightCount);
		pointLightBuffer->unbind();
		spotLightBuffer = globalBuffer::create(sizeof(int)  * 4 /*x4 because of padding*/+ (MAX_LIGHTS_PER_TYPE * sizeof(spotLight)), DYNAMIC_DRAW);
		spotLightBuffer->lAddIntB();
		for (uint8_t i = 0; i < MAX_LIGHTS_PER_TYPE; i++)
		{
			spotLightBuffer->lAddVec4B();
			spotLightBuffer->lAddVec4B();
			spotLightBuffer->lAddVec4B();
			spotLightBuffer->lAddVec4B();
			spotLightBuffer->lAddVec4B();
			spotLightBuffer->lAddFloatB();
		}
		spotLightBuffer->bindToPoint(3);
		spotLightBuffer->updateElement(0, &initialLightCount);
		spotLightBuffer->unbind();
	}

	void LightManager::rmStaticDirLight(const uint32_t uid)
	{
		auto it = m_staticDirLights.begin();
		for (; it != m_staticDirLights.end(); it++)
		{
			if (it->uid == uid)
			{
				it = m_staticDirLights.erase(it);
				goto ELEMENT_FOUND;
			}
		}
		//output some error
		return;
	ELEMENT_FOUND:
		for (auto i = it; i != m_staticDirLights.end(); i++)
		{
			i->index--;
		}
		//::::::UPDATE ALL STATIC & DYNAMIC ELEMENTS
		updateStaticDirLights(it);
		updateDynamicDirLights();
		return;
	}

	void LightManager::rmStaticPointLight(const uint32_t uid)
	{
		auto it = m_staticPointLights.begin();
		for (; it != m_staticPointLights.end(); it++)
		{
			if (it->uid == uid)
			{
				it = m_staticPointLights.erase(it);
				goto ELEMENT_FOUND;
			}
		}
		//output some error
		return;
	ELEMENT_FOUND:
		for (; it != m_staticPointLights.end(); it++)
		{
			it->index--;
		}
		//::::::UPDATE ALL STATIC & DYNAMIC ELEMENTS
		return;
	}

	void LightManager::rmStaticSpotLight(const uint32_t uid)
	{
		auto it = m_staticSpotLights.begin();
		for (; it != m_staticSpotLights.end(); it++)
		{
			if (it->uid == uid)
			{
				it = m_staticSpotLights.erase(it);
				goto ELEMENT_FOUND;
			}
		}
		//output some error
		return;
	ELEMENT_FOUND:
		for (; it != m_staticSpotLights.end(); it++)
		{
			it->index--;
		}
		//::::::UPDATE ALL STATIC & DYNAMIC ELEMENTS
		return;
	}

	void LightManager::rmDynamicDirLight(const uint32_t uid)
	{
		auto it = m_dynamicDirLights.begin();
		for (; it != m_dynamicDirLights.end(); it++)
		{
			if (it->uid == uid)
			{
				it = m_dynamicDirLights.erase(it);
				goto ELEMENT_FOUND;
			}
		}
		//output some error
		return;
	ELEMENT_FOUND:
		for (auto i = it; i != m_dynamicDirLights.end(); i++)
		{
			i->index--;
		}
		//:::::::UPDATE ALL DYNAMIC ELEMENTS
		updateDynamicDirLights(it);
		return;
	}

	void LightManager::rmDynamicPointLight(const uint32_t uid)
	{
		auto it = m_dynamicPointLights.begin();
		for (; it != m_dynamicPointLights.end(); it++)
		{
			if (it->uid == uid)
			{
				it = m_dynamicPointLights.erase(it);
				goto ELEMENT_FOUND;
			}
		}
		//output some error
		return;
	ELEMENT_FOUND:
		for (; it != m_dynamicPointLights.end(); it++)
		{
			it->index--;
		}
		//:::::::UPDATE ALL DYNAMIC ELEMENTS
		return;
	}

	void LightManager::rmDynamicSpotLight(const uint32_t uid)
	{
		auto it = m_dynamicSpotLights.begin();
		for (; it != m_dynamicSpotLights.end(); it++)
		{
			if (it->uid == uid)
			{
				it = m_dynamicSpotLights.erase(it);
				goto ELEMENT_FOUND;
			}
		}
		//output some error
		return;
	ELEMENT_FOUND:
		for (; it != m_dynamicSpotLights.end(); it++)
		{
			it->index--;
		}
		//:::::::UPDATE ALL DYNAMIC ELEMENTS
		return;
	}

	void LightManager::updateNewDirLight(bool Static)
	{
		directionalLightBuffer->bind();
		if (Static)
		{
			directionalLightBuffer->updateElement(1 + m_staticDirLights.back().index * 4 + 0, &m_staticDirLights.back().light->direction);//1+ because the first index always is the number of lights per type
			directionalLightBuffer->updateElement(1 + m_staticDirLights.back().index * 4 + 1, &m_staticDirLights.back().light->ambient);
			directionalLightBuffer->updateElement(1 + m_staticDirLights.back().index * 4 + 2, &m_staticDirLights.back().light->diffuse);
			directionalLightBuffer->updateElement(1 + m_staticDirLights.back().index * 4 + 3, &m_staticDirLights.back().light->specular);
			directionalLightBuffer->unbind();
			return;
		}
		directionalLightBuffer->updateElement(1 + m_dynamicDirLights.back().index * 4 + m_staticDirLights.size() * 4 + 0, &m_dynamicDirLights.back().light->direction);//1+ because the first index always is the number of lights per type
		directionalLightBuffer->updateElement(1 + m_dynamicDirLights.back().index * 4 + m_staticDirLights.size() * 4 + 1, &m_dynamicDirLights.back().light->ambient);
		directionalLightBuffer->updateElement(1 + m_dynamicDirLights.back().index * 4 + m_staticDirLights.size() * 4 + 2, &m_dynamicDirLights.back().light->diffuse);
		directionalLightBuffer->updateElement(1 + m_dynamicDirLights.back().index * 4 + m_staticDirLights.size() * 4 + 3, &m_dynamicDirLights.back().light->specular);
		directionalLightBuffer->unbind();
	}

	void LightManager::updateDynamicDirLights()
	{
		directionalLightBuffer->bind();
		for (auto it = m_dynamicDirLights.begin(); it != m_dynamicDirLights.end(); it++)
		{
			directionalLightBuffer->updateElement(1 + it->index * 4 + m_staticDirLights.size() * 4 + 0, &it->light->direction);
			directionalLightBuffer->updateElement(1 + it->index * 4 + m_staticDirLights.size() * 4 + 1, &it->light->ambient);
			directionalLightBuffer->updateElement(1 + it->index * 4 + m_staticDirLights.size() * 4 + 2, &it->light->diffuse);
			directionalLightBuffer->updateElement(1 + it->index * 4 + m_staticDirLights.size() * 4 + 3, &it->light->specular);
		}
		directionalLightBuffer->unbind();
	}

	void LightManager::updateStaticDirLights(std::list<DirLightElement>::iterator it)
	{
		directionalLightBuffer->bind();
		for (; it != m_staticDirLights.end(); it++)
		{
			directionalLightBuffer->updateElement(1 + it->index * 4 + 0, &it->light->direction);
			directionalLightBuffer->updateElement(1 + it->index * 4 + 1, &it->light->ambient);
			directionalLightBuffer->updateElement(1 + it->index * 4 + 2, &it->light->diffuse);
			directionalLightBuffer->updateElement(1 + it->index * 4 + 3, &it->light->diffuse);
		}
		directionalLightBuffer->unbind();
	}

	void LightManager::updateDynamicDirLights(std::list<DirLightElement>::iterator it)
	{
		directionalLightBuffer->bind();
		for (; it != m_dynamicDirLights.end(); it++)
		{
			directionalLightBuffer->updateElement(1 + it->index * 4 + m_staticDirLights.size() * 4 + 0, &it->light->direction);
			directionalLightBuffer->updateElement(1 + it->index * 4 + m_staticDirLights.size() * 4 + 1, &it->light->ambient);
			directionalLightBuffer->updateElement(1 + it->index * 4 + m_staticDirLights.size() * 4 + 2, &it->light->diffuse);
			directionalLightBuffer->updateElement(1 + it->index * 4 + m_staticDirLights.size() * 4 + 3, &it->light->diffuse);
		}
		directionalLightBuffer->unbind();
	}
	//pointLight
	void LightManager::updateNewPointLight(bool Static)
	{
		pointLightBuffer->bind();
		if (Static)
		{
			pointLightBuffer->updateElement(1 + m_staticPointLights.back().index + 0, &m_staticPointLights.back().light->position);//1+ because the first index always is the number of lights per type
			pointLightBuffer->updateElement(1 + m_staticPointLights.back().index + 1, &m_staticPointLights.back().light->ambient);
			pointLightBuffer->updateElement(1 + m_staticPointLights.back().index + 2, &m_staticPointLights.back().light->diffuse);
			pointLightBuffer->updateElement(1 + m_staticPointLights.back().index + 3, &m_staticPointLights.back().light->specular);
			pointLightBuffer->updateElement(1 + m_staticPointLights.back().index + 4, &m_staticPointLights.back().light->attenuation);
			pointLightBuffer->unbind();
			return;
		}
		pointLightBuffer->updateElement(1 + m_dynamicPointLights.back().index + 0, &m_dynamicPointLights.back().light->position);//1+ because the first index always is the number of lights per type
		pointLightBuffer->updateElement(1 + m_dynamicPointLights.back().index + 1, &m_dynamicPointLights.back().light->ambient);
		pointLightBuffer->updateElement(1 + m_dynamicPointLights.back().index + 2, &m_dynamicPointLights.back().light->diffuse);
		pointLightBuffer->updateElement(1 + m_dynamicPointLights.back().index + 3, &m_dynamicPointLights.back().light->specular);
		pointLightBuffer->updateElement(1 + m_dynamicPointLights.back().index + 4, &m_dynamicPointLights.back().light->attenuation);
		pointLightBuffer->unbind();
	}

	void LightManager::updateDynamicPointLights()
	{
		pointLightBuffer->bind();
		for (auto it = m_dynamicPointLights.begin(); it != m_dynamicPointLights.end(); it++)
		{
			pointLightBuffer->updateElement(1 + it->index + m_staticPointLights.size() + 0, &it->light->position);
			pointLightBuffer->updateElement(1 + it->index + m_staticPointLights.size() + 1, &it->light->ambient);
			pointLightBuffer->updateElement(1 + it->index + m_staticPointLights.size() + 2, &it->light->diffuse);
			pointLightBuffer->updateElement(1 + it->index + m_staticPointLights.size() + 3, &it->light->specular);
			pointLightBuffer->updateElement(1 + it->index + m_staticPointLights.size() + 4, &it->light->attenuation);
		}
		pointLightBuffer->unbind();
	}

	void LightManager::updateStaticPointLights(std::list<PointLightElement>::iterator it)
	{
		pointLightBuffer->bind();
		for (; it != m_staticPointLights.end(); it++)
		{
			pointLightBuffer->updateElement(1 + it->index + 0, &it->light->position);
			pointLightBuffer->updateElement(1 + it->index + 1, &it->light->ambient);
			pointLightBuffer->updateElement(1 + it->index + 2, &it->light->diffuse);
			pointLightBuffer->updateElement(1 + it->index + 3, &it->light->specular);
			pointLightBuffer->updateElement(1 + it->index + 4, &it->light->attenuation);
		}
		pointLightBuffer->unbind();
	}

	void LightManager::updateDynamicPointLights(std::list<PointLightElement>::iterator it)
	{
		pointLightBuffer->bind();
		for (; it != m_dynamicPointLights.end(); it++)
		{
			pointLightBuffer->updateElement(1 + it->index + m_staticPointLights.size() + 0, &it->light->position);
			pointLightBuffer->updateElement(1 + it->index + m_staticPointLights.size() + 1, &it->light->ambient);
			pointLightBuffer->updateElement(1 + it->index + m_staticPointLights.size() + 2, &it->light->diffuse);
			pointLightBuffer->updateElement(1 + it->index + m_staticPointLights.size() + 3, &it->light->specular);
			pointLightBuffer->updateElement(1 + it->index + m_staticPointLights.size() + 4, &it->light->attenuation);
		}
		pointLightBuffer->unbind();
	}
	//spotLight
	void LightManager::updateNewSpotLight(bool Static)
	{
		spotLightBuffer->bind();
		if (Static)
		{
			spotLightBuffer->updateElement(1 + m_staticSpotLights.back().index + 0, &m_staticSpotLights.back().light->position);//1+ because the first index always is the number of lights per type
			spotLightBuffer->updateElement(1 + m_staticSpotLights.back().index + 1, &m_staticSpotLights.back().light->direction);
			spotLightBuffer->updateElement(1 + m_staticSpotLights.back().index + 2, &m_staticSpotLights.back().light->ambient);
			spotLightBuffer->updateElement(1 + m_staticSpotLights.back().index + 3, &m_staticSpotLights.back().light->diffuse);
			spotLightBuffer->updateElement(1 + m_staticSpotLights.back().index + 4, &m_staticSpotLights.back().light->specular);
			spotLightBuffer->updateElement(1 + m_staticSpotLights.back().index + 5, &m_staticSpotLights.back().light->cutOff);
			spotLightBuffer->unbind();
			return;
		}
		spotLightBuffer->updateElement(1 + m_dynamicSpotLights.back().index + 0, &m_dynamicSpotLights.back().light->position);//1+ because the first index always is the number of lights per type
		spotLightBuffer->updateElement(1 + m_dynamicSpotLights.back().index + 1, &m_dynamicSpotLights.back().light->direction);
		spotLightBuffer->updateElement(1 + m_dynamicSpotLights.back().index + 2, &m_dynamicSpotLights.back().light->ambient);
		spotLightBuffer->updateElement(1 + m_dynamicSpotLights.back().index + 3, &m_dynamicSpotLights.back().light->diffuse);
		spotLightBuffer->updateElement(1 + m_dynamicSpotLights.back().index + 4, &m_dynamicSpotLights.back().light->specular);
		spotLightBuffer->updateElement(1 + m_dynamicSpotLights.back().index + 5, &m_dynamicSpotLights.back().light->cutOff);
		spotLightBuffer->unbind();
	}

	void LightManager::updateDynamicSpotLights()
	{
		spotLightBuffer->bind();
		for (auto it = m_dynamicSpotLights.begin(); it != m_dynamicSpotLights.end(); it++)
		{
			spotLightBuffer->updateElement(1 + it->index + m_staticSpotLights.size() + 0, &it->light->position);
			spotLightBuffer->updateElement(1 + it->index + m_staticSpotLights.size() + 1, &it->light->direction);
			spotLightBuffer->updateElement(1 + it->index + m_staticSpotLights.size() + 2, &it->light->ambient);
			spotLightBuffer->updateElement(1 + it->index + m_staticSpotLights.size() + 3, &it->light->diffuse);
			spotLightBuffer->updateElement(1 + it->index + m_staticSpotLights.size() + 4, &it->light->specular);
			spotLightBuffer->updateElement(1 + it->index + m_staticSpotLights.size() + 5, &it->light->cutOff);
		}
		spotLightBuffer->unbind();
	}

	void LightManager::updateStaticSpotLights(std::list<SpotLightElement>::iterator it)
	{
		spotLightBuffer->bind();
		for (; it != m_staticSpotLights.end(); it++)
		{
			spotLightBuffer->updateElement(1 + it->index + 0, &it->light->position);
			spotLightBuffer->updateElement(1 + it->index + 1, &it->light->direction);
			spotLightBuffer->updateElement(1 + it->index + 2, &it->light->ambient);
			spotLightBuffer->updateElement(1 + it->index + 3, &it->light->diffuse);
			spotLightBuffer->updateElement(1 + it->index + 4, &it->light->specular);
			spotLightBuffer->updateElement(1 + it->index + 5, &it->light->cutOff);
		}
		spotLightBuffer->unbind();
	}

	void LightManager::updateDynamicSpotLights(std::list<SpotLightElement>::iterator it)
	{
		spotLightBuffer->bind();
		for (; it != m_dynamicSpotLights.end(); it++)
		{
			spotLightBuffer->updateElement(1 + it->index + m_staticSpotLights.size() + 0, &it->light->position);
			spotLightBuffer->updateElement(1 + it->index + m_staticSpotLights.size() + 1, &it->light->direction);
			spotLightBuffer->updateElement(1 + it->index + m_staticSpotLights.size() + 2, &it->light->ambient);
			spotLightBuffer->updateElement(1 + it->index + m_staticSpotLights.size() + 3, &it->light->diffuse);
			spotLightBuffer->updateElement(1 + it->index + m_staticSpotLights.size() + 4, &it->light->specular);
			spotLightBuffer->updateElement(1 + it->index + m_staticSpotLights.size() + 5, &it->light->cutOff);
		}
		spotLightBuffer->unbind();
	}
}