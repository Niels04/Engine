#pragma once

#include "Engine/datatypes/vec4.hpp"
#include "buffers.hpp"

#define MAX_LIGHTS_PER_TYPE 10
#define DIRECTIONAL_LIGHTS_BIND 1
#define POINT_LIGHTS_BIND 2
#define SPOT_LIGHTS_BIND 3

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

		directionalLight(const vec4& lightDirection, const vec4& ambientColor, const vec4& diffuseColor, const vec3& specularColor)
			: direction(lightDirection), ambient(ambientColor), diffuse(diffuseColor), specular(specularColor)
		{

		}

		static directionalLight* create(const directionalLight& light)
		{
			return new directionalLight(light.direction, light.ambient, light.diffuse, light.specular);
		}
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

		pointLight(const vec4& lightPosition, const vec4& ambientColor, const vec4& diffuseColor, const vec4& specularColor, const vec3& INattenuation)
			: position(lightPosition), ambient(ambientColor), diffuse(diffuseColor), specular(specularColor), attenuation(INattenuation)
		{

		}

		static pointLight* create(const pointLight& light)
		{
			return new pointLight(light.position, light.ambient, light.diffuse, light.specular, light.attenuation);
		}
	};

	class spotLight
	{
		friend class LightManager;
	public:
		spotLight() = default;
		spotLight(vec3& lightPosition, vec3& spotDirection, vec3& ambientColor, vec3& diffuseColor, vec3& specularColor, float cutOffAngle)
			: position(lightPosition, 1.0f), direction(spotDirection, 0.0f), cutOff(cutOffAngle), ambient(ambientColor, 1.0f), diffuse(diffuseColor, 1.0f), specular(specularColor)
		{

		}
		vec4 position;
		vec4 direction;
		vec4 ambient;
		vec4 diffuse;
		vec3 specular;
		float cutOff;//the cosine of the spotLight's cutOff angle
	private:
		uint32_t uid;
		float padd[3];

		spotLight(const vec4& lightPosition, const vec4& lightDirection, const vec4& ambientColor, const vec4& diffuseColor, const vec3& specularColor, const float cutOffAngle)
			: position(lightPosition), direction(lightDirection), ambient(ambientColor), diffuse(diffuseColor), specular(specularColor), cutOff(cutOffAngle)
		{

		}

		static spotLight* create(const spotLight& light)
		{
			return new spotLight(light.position, light.direction, light.ambient, light.diffuse, light.specular, light.cutOff);
		}
	};

	class LightManager
	{
	public:
		void init();
		//get the index of a dynamicLight in the globalBuffer: get the index and add the amount of static lights atm
		[[nodiscard]] inline directionalLight** addStaticDirLight(const directionalLight& light) {
			m_staticDirLights.push_back(light); m_staticDirLights.back().uid = uid();
			m_staticDirLightPtrs.push_back(&m_staticDirLights.back()); updatePtrs();
			updateNewDirLight(true); updateDynamicDirLights(); updateDirLightCount();//in case of a new static light the new light itself needs to be updated and all lights after it(the dynamic ones as well)
			return &m_staticDirLightPtrs.back();
		}
		[[nodiscard]] inline pointLight** addStaticPointLight(const pointLight& light) {
			m_staticPointLights.push_back(light); m_staticPointLights.back().uid = uid();
			m_staticPointLightPtrs.push_back(&m_staticPointLights.back()); updatePtrs();
			updateNewPointLight(true); updateDynamicPointLights(); updatePointLightCount();
			return &m_staticPointLightPtrs.back();
		}
		[[nodiscard]] inline spotLight** addStaticSpotLight(const spotLight& light) {
			m_staticSpotLights.push_back(light); m_staticSpotLights.back().uid = uid();
			m_staticSpotLightPtrs.push_back(&m_staticSpotLights.back()); updatePtrs();
			updateNewSpotLight(true); updateDynamicSpotLights(); updateSpotLightCount();
			return &m_staticSpotLightPtrs.back();
		}
		
		[[nodiscard]] inline directionalLight** addDynamicDirLight(const directionalLight& light) {
			m_dynamicDirLights.push_back(light); m_dynamicDirLights.back().uid = uid();
			m_dynamicDirLightPtrs.push_back(&m_dynamicDirLights.back()); updatePtrs();
			updateNewDirLight(false); updateDirLightCount();//in case of a new dynamic light the light itself needs to be updated and the overall count
			return &m_dynamicDirLightPtrs.back();
		}
		[[nodiscard]] inline pointLight** addDynamicPointLight(const pointLight& light) {
			m_dynamicPointLights.push_back(light); m_dynamicPointLights.back().uid = uid();
			m_dynamicPointLightPtrs.push_back(&m_dynamicPointLights.back()); updatePtrs();
			updateNewPointLight(false); updatePointLightCount();
			return &m_dynamicPointLightPtrs.back();
		}
		[[nodiscard]] inline spotLight** addDynamicSpotLight(const spotLight& light) {
			m_dynamicSpotLights.push_back(light); m_dynamicSpotLights.back().uid = uid();
			m_dynamicSpotLightPtrs.push_back(&m_dynamicSpotLights.back()); updatePtrs();
			updateNewSpotLight(false); updateSpotLightCount();
			return &m_dynamicSpotLightPtrs.back();
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

		inline void LightManager::updatePtrs()
		{
			{
				auto itP = m_staticDirLightPtrs.begin();
				for (auto it = m_staticDirLights.begin(); it != m_staticDirLights.end(); it++, itP++)
				{
					*itP = &(*it);
				}
			}
			{
				auto itP = m_staticPointLightPtrs.begin();
				for (auto it = m_staticPointLights.begin(); it != m_staticPointLights.end(); it++, itP++)
				{
					*itP = &(*it);
				}
			}
			{
				auto itP = m_staticSpotLightPtrs.begin();
				for (auto it = m_staticSpotLights.begin(); it != m_staticSpotLights.end(); it++, itP++)
				{
					*itP = &(*it);
				}
			}
			{
				auto itP = m_dynamicDirLightPtrs.begin();
				for (auto it = m_dynamicDirLights.begin(); it != m_dynamicDirLights.end(); it++, itP++)
				{
					*itP = &(*it);
				}
			}
			{
				auto itP = m_dynamicPointLightPtrs.begin();
				for (auto it = m_dynamicPointLights.begin(); it != m_dynamicPointLights.end(); it++, itP++)
				{
					*itP = &(*it);
				}
			}
			{
				auto itP = m_dynamicSpotLightPtrs.begin();
				for (auto it = m_dynamicSpotLights.begin(); it != m_dynamicSpotLights.end(); it++, itP++)
				{
					*itP = &(*it);
				}
			}
		}

		//these are lists, because we don't want the elements to be reallocated all the time(cuz we wanna give out pointers to the elements(or maybe references?))
		std::list<directionalLight*> m_staticDirLightPtrs;
		std::list<pointLight*> m_staticPointLightPtrs;
		std::list<spotLight*> m_staticSpotLightPtrs;
		std::list<directionalLight*> m_dynamicDirLightPtrs;
		std::list<pointLight*> m_dynamicPointLightPtrs;
		std::list<spotLight*> m_dynamicSpotLightPtrs;

		Scope_ptr<globalBuffer> directionalLightBuffer;
		Scope_ptr<globalBuffer> pointLightBuffer;
		Scope_ptr<globalBuffer> spotLightBuffer;

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