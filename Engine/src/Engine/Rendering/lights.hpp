#pragma once

#include "Engine/datatypes/vec4.hpp"
#include "buffers.hpp"

#define MAX_LIGHTS_PER_TYPE 10

namespace Engine
{
	struct directionalLight
	{
		directionalLight() = default;
		directionalLight(vec3& lightDirection, vec3& ambientColor, vec3& diffuseColor, vec3& specularColor)
			: direction(lightDirection, 0.0f), ambient(ambientColor, 1.0f), diffuse(diffuseColor, 1.0f), specular(specularColor, 1.0f)
		{

		}
		vec4 direction;
		vec4 ambient;
		vec4 diffuse;
		vec4 specular;
	};

	struct pointLight
	{
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
	};

	struct spotLight
	{
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
	};

	class LightManager
	{
	public:
		void init();
		//get the index of a dynamicLight in the globalBuffer: get the index and add the amount of static lights atm
		inline uint32_t addStaticDirLight(Ref_ptr<directionalLight>& light) {
			m_staticDirLights.push_back({ static_cast<uint8_t>(m_staticDirLights.size()), uid(), light });
			updateNewDirLight(true); updateDynamicDirLights(); updateDirLightCount();//in case of a new static light the new light itself needs to be updated and all lights after it(the dynamic ones) as well
			return m_next_uid - 1;
		}
		inline uint32_t addStaticPointLight(Ref_ptr<pointLight>& light) {
			m_staticPointLights.push_back({ static_cast<uint8_t>(m_staticPointLights.size()), uid(), light });
			updateNewPointLight(true); updateDynamicPointLights(); updatePointLightCount();
			return m_next_uid - 1;
		}
		inline uint32_t addStaticSpotLight(Ref_ptr<spotLight>& light) {
			m_staticSpotLights.push_back({ static_cast<uint8_t>(m_staticSpotLights.size()), uid(), light });
			updateNewSpotLight(true); updateDynamicSpotLights(); updateSpotLightCount();
			return m_next_uid - 1;
		}
		
		inline uint32_t addDynamicDirLight(Ref_ptr<directionalLight>& light) {
			m_dynamicDirLights.push_back({ static_cast<uint8_t>(m_dynamicDirLights.size()), uid(), light });
			updateNewDirLight(false); updateDirLightCount();//in case of a new dynamic light the light itself needs to be updated and the overall count
			return m_next_uid - 1;
		}
		inline uint32_t addDynamicPointLight(Ref_ptr<pointLight>& light) {
			m_dynamicPointLights.push_back({ static_cast<uint8_t>(m_dynamicPointLights.size()), uid(), light });
			updateNewPointLight(false); updatePointLightCount();
			return m_next_uid - 1;
		}
		inline uint32_t addDynamicSpotLight(Ref_ptr<spotLight>& light) {
			m_dynamicSpotLights.push_back({ static_cast<uint8_t>(m_dynamicSpotLights.size()), uid(), light });
			updateNewSpotLight(false); updateSpotLightCount();
			return m_next_uid - 1;
		}

		void rmStaticDirLight(const uint32_t uid);
		void rmStaticPointLight(const uint32_t uid);
		void rmStaticSpotLight(const uint32_t uid);

		void rmDynamicDirLight(const uint32_t uid);
		void rmDynamicPointLight(const uint32_t uid);
		void rmDynamicSpotLight(const uint32_t uid);

		void updateLights() { updateDynamicDirLights(); updateDynamicPointLights(); updateDynamicSpotLights(); }
	private:
		struct DirLightElement;
		struct PointLightElement;
		struct SpotLightElement;
		std::list<DirLightElement> m_staticDirLights;
		std::list<PointLightElement> m_staticPointLights;
		std::list<SpotLightElement> m_staticSpotLights;
		std::list<DirLightElement> m_dynamicDirLights;
		std::list<PointLightElement> m_dynamicPointLights;
		std::list<SpotLightElement> m_dynamicSpotLights;

		Ref_ptr<globalBuffer> directionalLightBuffer;
		Ref_ptr<globalBuffer> pointLightBuffer;
		Ref_ptr<globalBuffer> spotLightBuffer;

		//in case of adding a new staticLight-> the new staticLight gets updated + all dynamicLights get updated
		void updateNewDirLight(bool Static);//one new light to update(if its a static one all dynamic ones will have to be updated as well)
		void updateDynamicDirLights();//every frame, cuz theire dynamic but also when a static light got added/removed
		void updateStaticDirLights(std::list<DirLightElement>::iterator it);//in case a static light gets removed update all static elements after it and call updateDynamicLights() as well
		void updateDynamicDirLights(std::list<DirLightElement>::iterator it);//in case a dynamic light gets removed update all dynamic elements after it

		void updateNewPointLight(bool Static);
		void updateDynamicPointLights();
		void updateStaticPointLights(std::list<PointLightElement>::iterator it);
		void updateDynamicPointLights(std::list<PointLightElement>::iterator it);

		void updateNewSpotLight(bool Static);
		void updateDynamicSpotLights();
		void updateStaticSpotLights(std::list<SpotLightElement>::iterator it);
		void updateDynamicSpotLights(std::list<SpotLightElement>::iterator it);

		inline void updateDirLightCount() { int count = m_staticDirLights.size() + m_dynamicDirLights.size(); directionalLightBuffer->bind(); 
			directionalLightBuffer->updateElement(0, &count); directionalLightBuffer->unbind(); }
		inline void updatePointLightCount() { int count = m_staticPointLights.size() + m_dynamicPointLights.size();  pointLightBuffer->bind();
			pointLightBuffer->updateElement(0, &count); pointLightBuffer->unbind(); }
		inline void updateSpotLightCount() { int count = m_staticSpotLights.size() + m_dynamicSpotLights.size(); spotLightBuffer->bind();
		spotLightBuffer->updateElement(0, &count); spotLightBuffer->unbind(); }

		uint32_t m_next_uid = 0;
		inline uint32_t uid() { m_next_uid++; return m_next_uid - 1; }

		struct DirLightElement
		{
			uint8_t index;
			uint32_t uid;
			Ref_ptr<directionalLight> light;
		};
		struct PointLightElement
		{
			uint8_t index;
			uint32_t uid;
			Ref_ptr<pointLight> light;
		};
		struct SpotLightElement
		{
			uint8_t index;
			uint32_t uid;
			Ref_ptr<spotLight> light;
		};
	};
}