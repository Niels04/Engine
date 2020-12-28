#pragma once

#include "Engine/datatypes/vec4.hpp"
#include "buffers.hpp"

#define MAX_LIGHTS_PER_TYPE 10

namespace Engine
{
	class directionalLight
	{
		friend class LightManager;
	public:
		directionalLight() = default;
		directionalLight(vec3& lightDirection, vec3& ambientColor, vec3& diffuseColor, vec3& specularColor)
			: direction(lightDirection, 0.0f), ambient(ambientColor, 0.0f), diffuse(diffuseColor, 0.0f), specular(specularColor)
		{

		}
		vec4 direction;
		vec4 ambient;
		vec4 diffuse;
		vec3 specular;
	private:
		uint32_t uid;
	};

	class pointLight
	{
		friend class LightManager;
	public:
		pointLight() = default;
		pointLight(vec3& lightPosition, vec3& ambientColor, vec3& diffuseColor, vec3& specularColor, vec3& INattenuation)
			: position(lightPosition, 1.0f), ambient(ambientColor, 1.0f), diffuse(diffuseColor, 1.0f), specular(specularColor, 1.0f), attenuation(INattenuation)
		{

		}
		pointLight(vec3& lightPosition, vec3& ambientColor, vec3& diffuseColor, vec3& specularColor, float Constant, float LinearFactor, float QuadraticFactor)
			: position(lightPosition, 1.0f), ambient(ambientColor, 1.0f), diffuse(diffuseColor, 1.0f), specular(specularColor, 1.0f), attenuation({Constant, LinearFactor, QuadraticFactor})
		{

		}
		vec4 position;
		vec4 ambient;
		vec4 diffuse;
		vec4 specular;
		vec3 attenuation;//first is constant, second is linear, third is quadratic
	private:
		uint32_t uid;
	};

	class spotLight
	{
		friend class LightManager;
	public:
		spotLight() = default;
		spotLight(vec3& lightPosition, vec3& spotDirection, vec3& ambientColor, vec3& diffuseColor, vec3& specularColor, float cutOffAngle)
			: position(lightPosition, 1.0f), direction(spotDirection, 0.0f), cutOff(cutOffAngle), ambient(ambientColor, 1.0f), diffuse(diffuseColor, 1.0f), specular(specularColor, 1.0f)
		{

		}
		vec4 position;
		vec4 direction;
		vec4 ambient;
		vec4 diffuse;
		vec4 specular;
		float cutOff;//the cosine of the spotLight's cutOff angle
	private:
		uint32_t uid;
		float padd[2];
	};

	class LightManager
	{
	public:
		void init();
		//get the index of a dynamicLight in the globalBuffer: get the index and add the amount of static lights atm
		inline directionalLight* addStaticDirLight(const directionalLight& light) {
			m_staticDirLights.push_back(light); m_staticDirLights.back().uid = uid();
			updateNewDirLight(true); updateDynamicDirLights(); updateDirLightCount();//in case of a new static light the new light itself needs to be updated and all lights after it(the dynamic ones) as well
			return &m_staticDirLights.back();
		}
		inline pointLight* addStaticPointLight(const pointLight& light) {
			m_staticPointLights.push_back(light); m_staticPointLights.back().uid = uid();
			updateNewPointLight(true); updateDynamicPointLights(); updatePointLightCount();
			return &m_staticPointLights.back();
		}
		inline spotLight* addStaticSpotLight(const spotLight& light) {
			m_staticSpotLights.push_back(light); m_staticSpotLights.back().uid = uid();
			updateNewSpotLight(true); updateDynamicSpotLights(); updateSpotLightCount();
			return &m_staticSpotLights.back();
		}
		
		inline directionalLight* addDynamicDirLight(const directionalLight& light) {
			m_dynamicDirLights.push_back(light); m_dynamicDirLights.back().uid = uid();
			updateNewDirLight(false); updateDirLightCount();//in case of a new dynamic light the light itself needs to be updated and the overall count
			return &m_dynamicDirLights.back();
		}
		inline pointLight* addDynamicPointLight(const pointLight& light) {
			m_dynamicPointLights.push_back(light); m_dynamicPointLights.back().uid = uid();
			updateNewPointLight(false); updatePointLightCount();
			return &m_dynamicPointLights.back();
		}
		inline spotLight* addDynamicSpotLight(const spotLight& light) {
			m_dynamicSpotLights.push_back(light); m_dynamicSpotLights.back().uid = uid();
			updateNewSpotLight(false); updateSpotLightCount();
			return &m_dynamicSpotLights.back();
		}

		void rmStaticDirLight(directionalLight* toRemove);
		void rmStaticPointLight(pointLight* toRemove);
		void rmStaticSpotLight(spotLight* toRemove);

		void rmDynamicDirLight(directionalLight* toRemove);
		void rmDynamicPointLight(pointLight* toRemove);
		void rmDynamicSpotLight(spotLight* toRemove);

		void updateLights() { updateDynamicDirLights(); updateDynamicPointLights(); updateDynamicSpotLights(); }
	private:
		std::vector<directionalLight> m_staticDirLights;
		std::vector<pointLight> m_staticPointLights;
		std::vector<spotLight> m_staticSpotLights;
		std::vector<directionalLight> m_dynamicDirLights;
		std::vector<pointLight> m_dynamicPointLights;
		std::vector<spotLight> m_dynamicSpotLights;

		Ref_ptr<globalBuffer> directionalLightBuffer;
		Ref_ptr<globalBuffer> pointLightBuffer;
		Ref_ptr<globalBuffer> spotLightBuffer;

		//in case of adding a new staticLight-> the new staticLight gets updated + all dynamicLights get updated
		void updateNewDirLight(bool Static);//one new light to update(if its a static one all dynamic ones will have to be updated as well)
		void updateDynamicDirLights();//every frame, cuz theire dynamic but also when a static light got added/removed
		void updateStaticDirLights(std::vector<directionalLight>::iterator it);//in case a static light gets removed update all static elements after it and call updateDynamicLights() as well
		void updateDynamicDirLights(std::vector<directionalLight>::iterator it);//in case a dynamic light gets removed update all dynamic elements after it

		void updateNewPointLight(bool Static);
		void updateDynamicPointLights();
		void updateStaticPointLights(std::vector<pointLight>::iterator it);
		void updateDynamicPointLights(std::vector<pointLight>::iterator it);

		void updateNewSpotLight(bool Static);
		void updateDynamicSpotLights();
		void updateStaticSpotLights(std::vector<spotLight>::iterator it);
		void updateDynamicSpotLights(std::vector<spotLight>::iterator it);

		inline void updateDirLightCount() { int count = m_staticDirLights.size() + m_dynamicDirLights.size(); directionalLightBuffer->bind(); 
			directionalLightBuffer->updateElement(0, &count); directionalLightBuffer->unbind(); }
		inline void updatePointLightCount() { int count = m_staticPointLights.size() + m_dynamicPointLights.size();  pointLightBuffer->bind();
			pointLightBuffer->updateElement(0, &count); pointLightBuffer->unbind(); }
		inline void updateSpotLightCount() { int count = m_staticSpotLights.size() + m_dynamicSpotLights.size(); spotLightBuffer->bind();
		spotLightBuffer->updateElement(0, &count); spotLightBuffer->unbind(); }

		uint32_t m_next_uid = 0;
		inline uint32_t uid() { m_next_uid++; return m_next_uid - 1; }
	};
}