#pragma once

#include "Engpch.hpp"
#include "movement.hpp"
#include "mesh.hpp"

namespace Engine
{
	CircularMeshMovement::CircularMeshMovement(const vec3& rotationNormal, const vec3& rotationCenter, const float rotationSpeed)
		: m_rotationSpeed(rotationSpeed), m_rotationCenter({rotationCenter, 1.0f})
	{
		m_rotationLookAt = mat4::lookAt({ -rotationNormal, 0.0f });
		m_rotationLookAtInverse = mat4::inverse3x3(m_rotationLookAt);
	}

	MeshRotation::MeshRotation(const float X_rotationSpeed, const float Y_rotationSpeed, const float Z_rotationSpeed)
		: m_X_rotationSpeed(X_rotationSpeed), m_Y_rotationSpeed(Y_rotationSpeed), m_Z_rotationSpeed(Z_rotationSpeed)
	{

	}

	DirLightMovement::DirLightMovement(const float X_rotationSpeed, const float Y_rotationSpeed, const float Z_rotationSpeed)
		: m_X_rotationSpeed(X_rotationSpeed), m_Y_rotationSpeed(Y_rotationSpeed), m_Z_rotationSpeed(Z_rotationSpeed)
	{
		
	}

	CircularPointLightMovement::CircularPointLightMovement(const vec3& rotationNormal, const vec3& rotationCenter, const float rotationSpeed)
		: m_rotationSpeed(rotationSpeed), m_rotationCenter({ rotationCenter, 1.0f })
	{
		ENG_CORE_WARN("Consider using the meshe's\"attachLight\"-method if you want attach a light to a mesh(position and rotation-wise). If that is not in your intend, go ahead.");
		m_rotationLookAt = mat4::lookAt({ -rotationNormal, 0.0f });
		m_rotationLookAtInverse = mat4::inverse3x3(m_rotationLookAt);
	}

	CircularSpotLightMovement::CircularSpotLightMovement(const vec3& rotationNormal, const vec3& rotationCenter, const float rotationSpeed)
		: m_rotationSpeed(rotationSpeed), m_rotationCenter({ rotationCenter, 1.0f })
	{
		ENG_CORE_WARN("Consider using the meshe's\"attachLight\"-method if you want attach a light to a mesh(position and rotation-wise). If that is not in your intend, go ahead.");
		m_rotationLookAt = mat4::lookAt({ -rotationNormal, 0.0f });
		m_rotationLookAtInverse = mat4::inverse3x3(m_rotationLookAt);
	}

	SpotLightRotation::SpotLightRotation(const float X_rotationSpeed, const float Y_rotationSpeed, const float Z_rotationSpeed)
		: m_X_rotationSpeed(X_rotationSpeed), m_Y_rotationSpeed(Y_rotationSpeed), m_Z_rotationSpeed(Z_rotationSpeed)
	{
		ENG_CORE_WARN("Consider using the meshe's\"attachLight\"-method if you want attach a light to a mesh(position and rotation-wise). If that is not in your intend, go ahead.");
	}
}