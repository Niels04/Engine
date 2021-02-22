#include <Engpch.hpp>
#include "light_effects.hpp"

namespace Engine
{
	void dirLightColorFade::onUpdate()
	{
		if (m_color.x > 0.0f && m_color.z == 0.0f)
		{
			m_light->ambient.x -= m_incr * m_ambFactor; m_light->ambient.y += m_incr * m_ambFactor;
			m_light->diffuse.x -= m_incr * m_diffFactor; m_light->diffuse.y += m_incr * m_diffFactor;//if the light is only red(and maybe green) -> decrease red and increase green
			m_light->specular.x -= m_incr * m_specFactor; m_light->specular.y += m_incr * m_specFactor;
			m_color.x -= m_incr; m_color.y += m_incr;
		}
		if (m_color.y > 0.0f && m_color.x == 0.0f)
		{
			m_light->ambient.y -= m_incr * m_ambFactor; m_light->ambient.z += m_incr * m_ambFactor;
			m_light->diffuse.y -= m_incr * m_diffFactor; m_light->diffuse.z += m_incr * m_diffFactor;//if the light is only green(and maybe red) -> decrease green and increase blue
			m_light->specular.y -= m_incr * m_specFactor; m_light->specular.z += m_incr * m_specFactor;
			m_color.y -= m_incr; m_color.z += m_incr;
		}
		if (m_color.z > 0.0f && m_color.y == 0.0f)
		{
			m_light->ambient.z -= m_incr * m_ambFactor; m_light->ambient.x += m_incr * m_ambFactor;
			m_light->diffuse.z -= m_incr * m_diffFactor; m_light->diffuse.x += m_incr * m_diffFactor;//if the light is only blue(and maybe green), subtract from the blue-channel and add to the red one
			m_light->specular.z -= m_incr * m_specFactor; m_light->specular.x += m_incr * m_specFactor;
			m_color.z -= m_incr; m_color.x += m_incr;
		}
		m_color.x = std::clamp(m_color.x, 0.0f, 1.0f);
		m_color.y = std::clamp(m_color.y, 0.0f, 1.0f);
		m_color.z = std::clamp(m_color.z, 0.0f, 1.0f);
	}

	void pointLightColorFade::onUpdate()
	{
		if (m_color.x > 0.0f && m_color.z == 0.0f)
		{
			m_light->ambient.x -= m_incr * m_ambFactor; m_light->ambient.y += m_incr * m_ambFactor;
			m_light->diffuse.x -= m_incr * m_diffFactor; m_light->diffuse.y += m_incr * m_diffFactor;//if the light is only red(and maybe green) -> decrease red and increase green
			m_light->specular.x -= m_incr * m_specFactor; m_light->specular.y += m_incr * m_specFactor;
			m_color.x -= m_incr; m_color.y += m_incr;
		}
		if (m_color.y > 0.0f && m_color.x == 0.0f)
		{
			m_light->ambient.y -= m_incr * m_ambFactor; m_light->ambient.z += m_incr * m_ambFactor;
			m_light->diffuse.y -= m_incr * m_diffFactor; m_light->diffuse.z += m_incr * m_diffFactor;//if the light is only green(and maybe red) -> decrease green and increase blue
			m_light->specular.y -= m_incr * m_specFactor; m_light->specular.z += m_incr * m_specFactor;
			m_color.y -= m_incr; m_color.z += m_incr;
		}
		if (m_color.z > 0.0f && m_color.y == 0.0f)
		{
			m_light->ambient.z -= m_incr * m_ambFactor; m_light->ambient.x += m_incr * m_ambFactor;
			m_light->diffuse.z -= m_incr * m_diffFactor; m_light->diffuse.x += m_incr * m_diffFactor;//if the light is only blue(and maybe green), subtract from the blue-channel and add to the red one
			m_light->specular.z -= m_incr * m_specFactor; m_light->specular.x += m_incr * m_specFactor;
			m_color.z -= m_incr; m_color.x += m_incr;
		}
		m_color.x = std::clamp(m_color.x, 0.0f, 1.0f);
		m_color.y = std::clamp(m_color.y, 0.0f, 1.0f);
		m_color.z = std::clamp(m_color.z, 0.0f, 1.0f);
	}

	void spotLightColorFade::onUpdate()
	{
		if (m_color.x > 0.0f && m_color.z == 0.0f)
		{
			m_light->ambient.x -= m_incr * m_ambFactor; m_light->ambient.y += m_incr * m_ambFactor;
			m_light->diffuse.x -= m_incr * m_diffFactor; m_light->diffuse.y += m_incr * m_diffFactor;//if the light is only red(and maybe green) -> decrease red and increase green
			m_light->specular.x -= m_incr * m_specFactor; m_light->specular.y += m_incr * m_specFactor;
			m_color.x -= m_incr; m_color.y += m_incr;
		}
		if (m_color.y > 0.0f && m_color.x == 0.0f)
		{
			m_light->ambient.y -= m_incr * m_ambFactor; m_light->ambient.z += m_incr * m_ambFactor;
			m_light->diffuse.y -= m_incr * m_diffFactor; m_light->diffuse.z += m_incr * m_diffFactor;//if the light is only green(and maybe red) -> decrease green and increase blue
			m_light->specular.y -= m_incr * m_specFactor; m_light->specular.z += m_incr * m_specFactor;
			m_color.y -= m_incr; m_color.z += m_incr;
		}
		if (m_color.z > 0.0f && m_color.y == 0.0f)
		{
			m_light->ambient.z -= m_incr * m_ambFactor; m_light->ambient.x += m_incr * m_ambFactor;
			m_light->diffuse.z -= m_incr * m_diffFactor; m_light->diffuse.x += m_incr * m_diffFactor;//if the light is only blue(and maybe green), subtract from the blue-channel and add to the red one
			m_light->specular.z -= m_incr * m_specFactor; m_light->specular.x += m_incr * m_specFactor;
			m_color.z -= m_incr; m_color.x += m_incr;
		}
		m_color.x = std::clamp(m_color.x, 0.0f, 1.0f);
		m_color.y = std::clamp(m_color.y, 0.0f, 1.0f);
		m_color.z = std::clamp(m_color.z, 0.0f, 1.0f);
	}
}