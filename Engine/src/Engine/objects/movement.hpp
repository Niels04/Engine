#pragma once
#include "Engine/core/timeStep.hpp"
#include "mesh.hpp"
#include "Engine/Rendering/lights.hpp"

namespace Engine
{

	class MeshMovement
	{
		friend class mesh;
	public:
		virtual ~MeshMovement() = default;
		virtual void onUpdate(timestep& ts) const = 0;
	protected:
		mesh* m_mesh;
	private:
		inline void attachToMesh(mesh* Mesh) { m_mesh = Mesh; }
	};

	class CircularMeshMovement : public MeshMovement
	{
	public:
		CircularMeshMovement(const vec3& rotationNormal, const vec3& rotationCenter, const float rotationSpeed);
		~CircularMeshMovement() = default;
		static Ref_ptr<MeshMovement> create(const vec3& rotationNormal, const vec3& rotationCenter, const float rotationSpeed) {
			return std::make_shared<CircularMeshMovement>(rotationNormal, rotationCenter, rotationSpeed); }
		inline virtual void onUpdate(timestep& ts) const
		{
			vec4 relativePos = m_mesh->m_position - m_rotationCenter;
			relativePos = m_rotationLookAtInverse * mat4::Rotz(m_rotationSpeed * ts) * m_rotationLookAt * relativePos;
			m_mesh->setPos(relativePos + m_rotationCenter);
		}
	private:
		mat4 m_rotationLookAt;
		mat4 m_rotationLookAtInverse;
		vec4 m_rotationCenter;
		const float m_rotationSpeed;
	};

	class MeshRotation : public MeshMovement
	{
	public:
		MeshRotation(const float X_rotationSpeed, const float Y_rotationSpeed, const float Z_rotationSpeed);
		~MeshRotation() = default;
		static Ref_ptr<MeshMovement> create(const float X_rotationSpeed, const float Y_rotationSpeed, const float Z_rotationSpeed) {
			return std::make_shared<MeshRotation>(X_rotationSpeed, Y_rotationSpeed, Z_rotationSpeed); }
		inline void onUpdate(timestep & ts) const
		{
			vec4 rotation(m_mesh->getRot(), 0.0f);
			m_mesh->setRot({ rotation.x + m_X_rotationSpeed * ts, rotation.y + m_Y_rotationSpeed * ts, rotation.z + m_Z_rotationSpeed * ts });
		}
	private:
		const float m_X_rotationSpeed;
		const float m_Y_rotationSpeed;
		const float m_Z_rotationSpeed;
	};
	//for lights it makes most sense to store a reference to the light in the movement rather than to store the movement in the light
	class DirLightMovement
	{
	public:
		DirLightMovement(const float X_rotationSpeed, const float Y_rotationSpeed, const float Z_rotationSpeed);
		~DirLightMovement() = default;
		static Ref_ptr<DirLightMovement> create(const float X_rotationSpeed, const float Y_rotationspeed, const float Z_rotationSpeed) {
			return std::make_shared<DirLightMovement>(X_rotationSpeed, Y_rotationspeed, Z_rotationSpeed); }
		inline void onUpdate(timestep& ts) const
		{
			m_light->direction = mat4::Rotx(m_X_rotationSpeed * ts) * mat4::Roty(m_Y_rotationSpeed * ts) * mat4::Rotz(m_Z_rotationSpeed * ts) * m_light->direction;
		}
		inline void attachToLight(PtrPtr<directionalLight>& light) { m_light = light; }
	private:
		const float m_X_rotationSpeed;
		const float m_Y_rotationSpeed;
		const float m_Z_rotationSpeed;
		PtrPtr<directionalLight> m_light;
	};

	class PointLightMovement
	{
	public:
		virtual ~PointLightMovement() = default;
		virtual void onUpdate(timestep& ts) const = 0;
		virtual void attachToLight(PtrPtr<pointLight>& light) { m_light = light; }
	protected:
		PtrPtr<pointLight> m_light;
	};

	class CircularPointLightMovement : public PointLightMovement
	{
	public:
		CircularPointLightMovement(const vec3& rotationNormal, const vec3& rotationCenter, const float rotationSpeed);
		~CircularPointLightMovement() = default;
		static Ref_ptr<PointLightMovement> create(const vec3& rotationNormal, const vec3& rotationCenter, const float rotationSpeed) {
			return std::make_shared<CircularPointLightMovement>(rotationNormal, rotationCenter, rotationSpeed); }
		inline virtual void onUpdate(timestep& ts) const
		{
			vec4 relativePos = m_light->position - m_rotationCenter;
			relativePos = m_rotationLookAtInverse * mat4::Rotz(m_rotationSpeed * ts) * m_rotationLookAt * relativePos;
			m_light->position = relativePos + m_rotationCenter;
		}
	private:
		mat4 m_rotationLookAt;
		mat4 m_rotationLookAtInverse;
		vec4 m_rotationCenter;
		const float m_rotationSpeed;
	};

	class SpotLightMovement
	{
	public:
		virtual ~SpotLightMovement() = default;
		virtual void onUpdate(timestep& ts) const = 0;
		virtual void attachToLight(PtrPtr<spotLight>& light) { m_light = light; }
	protected:
		PtrPtr<spotLight> m_light;
	};

	class CircularSpotLightMovement : public SpotLightMovement
	{
	public:
		CircularSpotLightMovement(const vec3& rotationNormal, const vec3& rotationCenter, const float rotationSpeed);
		~CircularSpotLightMovement() = default;
		static Ref_ptr<SpotLightMovement> create(const vec3& rotationNormal, const vec3& rotationCenter, const float rotationSpeed) {
			return std::make_shared<CircularSpotLightMovement>(rotationNormal, rotationCenter, rotationSpeed);
		}
		inline virtual void onUpdate(timestep& ts) const
		{
			vec4 relativePos = m_light->position - m_rotationCenter;
			relativePos = m_rotationLookAtInverse * mat4::Rotz(m_rotationSpeed * ts) * m_rotationLookAt * relativePos;
			m_light->position = relativePos + m_rotationCenter;
		}
	private:
		mat4 m_rotationLookAt;
		mat4 m_rotationLookAtInverse;
		vec4 m_rotationCenter;
		const float m_rotationSpeed;
	};

	class SpotLightRotation : public SpotLightMovement
	{
	public:
		SpotLightRotation(const float X_rotationSpeed, const float Y_rotationSpeed, const float Z_rotationSpeed);
		~SpotLightRotation() = default;
		static Ref_ptr<SpotLightMovement> create(const float X_rotationSpeed, const float Y_rotationspeed, const float Z_rotationSpeed) {
			return std::make_shared<SpotLightRotation>(X_rotationSpeed, Y_rotationspeed, Z_rotationSpeed);
		}
		inline void onUpdate(timestep& ts) const
		{
			m_light->direction = mat4::Rotx(m_X_rotationSpeed * ts) * mat4::Roty(m_Y_rotationSpeed * ts) * mat4::Rotz(m_Z_rotationSpeed * ts) * m_light->direction;
		}
		inline void attachToLight(PtrPtr<spotLight>& light) { m_light = light; }
	private:
		const float m_X_rotationSpeed;
		const float m_Y_rotationSpeed;
		const float m_Z_rotationSpeed;
		PtrPtr<spotLight> m_light;
	};
}