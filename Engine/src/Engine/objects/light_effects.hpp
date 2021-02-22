#pragma once

#include "Engine/Rendering/lights.hpp"

namespace Engine
{
	class dirLightEffect
	{
	public:
		virtual ~dirLightEffect() = default;
		virtual void onUpdate() = 0;//dont take in a timestep here -> this should advance a stage with every frame
		virtual void onInit() {  }//every child is free to implement this if it needs such a function
		inline void attachToLight(PtrPtr<directionalLight>& color) { m_light = color; }
	protected:
		PtrPtr<directionalLight> m_light;
	};

	class dirLightColorFade : public dirLightEffect
	{
	public:
		dirLightColorFade(const float ambientFactor, const float diffuseFactor, const float specularFactor, const float increment, const vec4 startingColor)
			: m_ambFactor(ambientFactor), m_diffFactor(diffuseFactor), m_specFactor(specularFactor), m_incr(increment / 255.0f), m_color(startingColor)
		{
		}
		static Ref_ptr<dirLightEffect> create(float ambientFactor, float diffuseFactor, float specularFactor, const float increment = 1.0f, const vec4 startingColor = { 1.0f, 0.0f, 0.0f, 1.0f }) {
			return std::make_shared<dirLightColorFade>(ambientFactor, diffuseFactor, specularFactor, increment, startingColor); }
		virtual void onInit() override { m_light->ambient = m_color * m_ambFactor; m_light->diffuse = m_color * m_diffFactor; m_light->specular = (m_color * m_specFactor).xyz(); }
		virtual void onUpdate() override;
	private:
		vec4 m_color;
		float m_incr;
		float m_ambFactor;
		float m_diffFactor;
		float m_specFactor;
	};

	class pointLightEffect
	{
	public:
		virtual ~pointLightEffect() = default;
		virtual void onUpdate() = 0;//dont take in a timestep here -> this should advance a stage with every frame
		virtual void onInit() {  }//every child is free to implement this if it needs such a function
		inline void attachToLight(PtrPtr<pointLight>& color) { m_light = color; }
	protected:
		PtrPtr<pointLight> m_light;
	};

	class pointLightColorFade : public pointLightEffect
	{
	public:
		pointLightColorFade(const float ambientFactor, const float diffuseFactor, const float specularFactor, const float increment, const vec4 startingColor)
			: m_ambFactor(ambientFactor), m_diffFactor(diffuseFactor), m_specFactor(specularFactor), m_incr(increment / 255.0f), m_color(startingColor)
		{
		}
		static Ref_ptr<pointLightEffect> create(float ambientFactor, float diffuseFactor, float specularFactor, const float increment = 1.0f, const vec4 startingColor = { 1.0f, 0.0f, 0.0f, 1.0f }) {
			return std::make_shared<pointLightColorFade>(ambientFactor, diffuseFactor, specularFactor, increment, startingColor);
		}
		virtual void onInit() override { m_light->ambient = m_color * m_ambFactor; m_light->diffuse = m_color * m_diffFactor; m_light->specular = m_color * m_specFactor; }
		virtual void onUpdate() override;
	private:
		vec4 m_color;
		float m_incr;
		float m_ambFactor;
		float m_diffFactor;
		float m_specFactor;
	};

	class spotLightEffect
	{
	public:
		virtual ~spotLightEffect() = default;
		virtual void onUpdate() = 0;//dont take in a timestep here -> this should advance a stage with every frame
		virtual void onInit() {  }//every child is free to implement this if it needs such a function
		inline void attachToLight(PtrPtr<spotLight>& color) { m_light = color; }
	protected:
		PtrPtr<spotLight> m_light;
	};

	class spotLightColorFade : public spotLightEffect
	{
	public:
		spotLightColorFade(const float ambientFactor, const float diffuseFactor, const float specularFactor, const float increment, const vec4 startingColor)
			: m_ambFactor(ambientFactor), m_diffFactor(diffuseFactor), m_specFactor(specularFactor), m_incr(increment / 255.0f), m_color(startingColor)
		{
		}
		static Ref_ptr<spotLightEffect> create(float ambientFactor, float diffuseFactor, float specularFactor, const float increment = 1.0f, const vec4 startingColor = { 1.0f, 0.0f, 0.0f, 1.0f }) {
			return std::make_shared<spotLightColorFade>(ambientFactor, diffuseFactor, specularFactor, increment, startingColor);
		}
		virtual void onInit() override { m_light->ambient = m_color * m_ambFactor; m_light->diffuse = m_color * m_diffFactor; m_light->specular = (m_color * m_specFactor).xyz(); }
		virtual void onUpdate() override;
	private:
		vec4 m_color;
		float m_incr;
		float m_ambFactor;
		float m_diffFactor;
		float m_specFactor;
	};

	class DirLightAnimation
	{
	public:
		DirLightAnimation(const uint32_t colorCount, const uint32_t switch_interval, const uint32_t stepCount, const float ambFactor, const float diffFactor, const float specFactor, const uint32_t* animationSteps, va_list params)
			: m_switch_interval(switch_interval), m_stepCount(stepCount), m_ambFactor(ambFactor), m_diffFactor(diffFactor), m_specFactor(specFactor), m_colors(NULL), m_steps(NULL)
		{
			m_colors = (vec4*)malloc(colorCount * sizeof(vec4));
			vec3 val;
			for (int i = 0; i < colorCount; i++)
			{
				val = va_arg(params, vec3);
				*(m_colors + i) = { val, 1.0f };
			}
			m_steps = (uint32_t*)malloc(stepCount * sizeof(uint32_t));
			memcpy(m_steps, animationSteps, stepCount * sizeof(uint32_t));//copy the array of animationSteps
		}
		~DirLightAnimation()
		{
			_ASSERT(m_colors != NULL);
			free(m_colors);
			_ASSERT(m_steps != NULL);
			free(m_steps);
		}
		static Ref_ptr<DirLightAnimation> create(const uint32_t colorCount, const uint32_t switch_interval, const uint32_t stepCount, const float ambFactor, const float diffFactor, const float specFactor, const uint32_t* animationSteps, ...) {
			va_list params;
			va_start(params, animationSteps);//pass in the last fixed parameter to get a ptr to the begin of the variatic parameters
			Ref_ptr<DirLightAnimation> ptr = std::make_shared<DirLightAnimation>(colorCount, switch_interval, stepCount, ambFactor, diffFactor, specFactor, animationSteps, params);
			va_end(params);
			return  ptr;
		}
		inline void onUpdate()
		{
			m_light->ambient = m_colors[m_steps[index_curr]] * m_ambFactor;
			m_light->diffuse = m_colors[m_steps[index_curr]] * m_diffFactor;
			m_light->specular = (m_colors[m_steps[index_curr]] * m_specFactor).xyz();
			time_in_step_count++;
			if (time_in_step_count == m_switch_interval)
			{
				time_in_step_count = 0;
				index_curr++;
				if (index_curr == m_stepCount)
					index_curr = 0;
			}
		}
		void attachToLight(PtrPtr<directionalLight> light) { m_light = light; }
	private:
		const uint32_t m_stepCount;
		vec4* m_colors;//array of the animation-colors
		uint32_t* m_steps;//array of the animation-steps
		const uint32_t m_switch_interval;
		//for managing looping
		uint32_t index_curr = 0;//current index, at which the animation is
		uint32_t time_in_step_count = 0;//gets set to 2 if the light has been in an animation-step for 2 frames
		//light and factors
		PtrPtr<directionalLight> m_light;
		const float m_ambFactor;
		const float m_diffFactor;
		const float m_specFactor;
	};

	class PointLightAnimation
	{
	public:
		PointLightAnimation(const uint32_t colorCount, const uint32_t switch_interval, const uint32_t stepCount, const float ambFactor, const float diffFactor, const float specFactor, const uint32_t* animationSteps, va_list params)
			: m_switch_interval(switch_interval), m_stepCount(stepCount), m_ambFactor(ambFactor), m_diffFactor(diffFactor), m_specFactor(specFactor), m_colors(NULL), m_steps(NULL)
		{
			m_colors = (vec4*)malloc(colorCount * sizeof(vec4));
			vec3 val;
			for (int i = 0; i < colorCount; i++)
			{
				val = va_arg(params, vec3);
				*(m_colors + i) = { val, 1.0f };
			}
			m_steps = (uint32_t*)malloc(stepCount * sizeof(uint32_t));
			memcpy(m_steps, animationSteps, stepCount * sizeof(uint32_t));//copy the array of animationSteps
		}
		~PointLightAnimation()
		{
			_ASSERT(m_colors != NULL);
			free(m_colors);
			_ASSERT(m_steps != NULL);
			free(m_steps);
		}
		static Ref_ptr<PointLightAnimation> create(const uint32_t colorCount, const uint32_t switch_interval, const uint32_t stepCount, const float ambFactor, const float diffFactor, const float specFactor, const uint32_t* animationSteps, ...) {
			va_list params;
			va_start(params, animationSteps);//pass in the last fixed parameter to get a ptr to the begin of the variatic parameters
			Ref_ptr<PointLightAnimation> ptr = std::make_shared<PointLightAnimation>(colorCount, switch_interval, stepCount, ambFactor, diffFactor, specFactor, animationSteps, params);
			va_end(params);
			return  ptr;
		}
		inline void onUpdate()
		{
			m_light->ambient = m_colors[m_steps[index_curr]] * m_ambFactor;
			m_light->diffuse = m_colors[m_steps[index_curr]] * m_diffFactor;
			m_light->specular = m_colors[m_steps[index_curr]] * m_specFactor;
			time_in_step_count++;
			if (time_in_step_count == m_switch_interval)
			{
				time_in_step_count = 0;
				index_curr++;
				if (index_curr == m_stepCount)
					index_curr = 0;
			}
		}
		void attachToLight(PtrPtr<pointLight> light) { m_light = light; }
	private:
		const uint32_t m_stepCount;
		vec4* m_colors;//array of the animation-colors
		uint32_t* m_steps;//array of the animation-steps
		const uint32_t m_switch_interval;
		//for managing looping
		uint32_t index_curr = 0;//current index, at which the animation is
		uint32_t time_in_step_count = 0;//gets set to 2 if the light has been in an animation-step for 2 frames
		//light and factors
		PtrPtr<pointLight> m_light;
		const float m_ambFactor;
		const float m_diffFactor;
		const float m_specFactor;
	};

	class SpotLightAnimation
	{
	public:
		SpotLightAnimation(const uint32_t colorCount, const uint32_t switch_interval, const uint32_t stepCount, const float ambFactor, const float diffFactor, const float specFactor, const uint32_t* animationSteps, va_list params)
			: m_switch_interval(switch_interval), m_stepCount(stepCount), m_ambFactor(ambFactor), m_diffFactor(diffFactor), m_specFactor(specFactor), m_colors(NULL), m_steps(NULL)
		{
			m_colors = (vec4*)malloc(colorCount * sizeof(vec4));
			vec3 val;
			for (int i = 0; i < colorCount; i++)
			{
				val = va_arg(params, vec3);
				*(m_colors + i) = { val, 1.0f };
			}
			m_steps = (uint32_t*)malloc(stepCount * sizeof(uint32_t));
			memcpy(m_steps, animationSteps, stepCount * sizeof(uint32_t));//copy the array of animationSteps
		}
		~SpotLightAnimation()
		{
			_ASSERT(m_colors != NULL);
			free(m_colors);
			_ASSERT(m_steps != NULL);
			free(m_steps);
		}
		static Ref_ptr<SpotLightAnimation> create(const uint32_t colorCount, const uint32_t switch_interval, const uint32_t stepCount, const float ambFactor, const float diffFactor, const float specFactor, const uint32_t* animationSteps, ...) {
			va_list params;
			va_start(params, animationSteps);//pass in the last fixed parameter to get a ptr to the begin of the variatic parameters
			Ref_ptr<SpotLightAnimation> ptr = std::make_shared<SpotLightAnimation>(colorCount, switch_interval, stepCount, ambFactor, diffFactor, specFactor, animationSteps, params);
			va_end(params);
			return  ptr;
		}
		inline void onUpdate()
		{
			m_light->ambient = m_colors[m_steps[index_curr]] * m_ambFactor;
			m_light->diffuse = m_colors[m_steps[index_curr]] * m_diffFactor;
			m_light->specular = (m_colors[m_steps[index_curr]] * m_specFactor).xyz();
			time_in_step_count++;
			if (time_in_step_count == m_switch_interval)
			{
				time_in_step_count = 0;
				index_curr++;
				if (index_curr == m_stepCount)
					index_curr = 0;
			}
		}
		void attachToLight(PtrPtr<spotLight> light) { m_light = light; }
	private:
		const uint32_t m_stepCount;
		vec4* m_colors;//array of the animation-colors
		uint32_t* m_steps;//array of the animation-steps
		const uint32_t m_switch_interval;
		//for managing looping
		uint32_t index_curr = 0;//current index, at which the animation is
		uint32_t time_in_step_count = 0;//gets set to 2 if the light has been in an animation-step for 2 frames
		//light and factors
		PtrPtr<spotLight> m_light;
		const float m_ambFactor;
		const float m_diffFactor;
		const float m_specFactor;
	};
}