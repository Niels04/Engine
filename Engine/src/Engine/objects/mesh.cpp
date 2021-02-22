#include "Engpch.hpp"
#include "mesh.hpp"
#include "movement.hpp"

namespace Engine
{
	mesh::mesh(const Ref_ptr<vertexArray>& vertexArray, const Ref_ptr<material>& material, const std::string& name, const vec3& position, const vec3& rotation, const vec3& scale)
		: m_geometry(vertexArray), m_material(material), m_position(position, 1.0f), m_rotation(rotation), m_scale(scale), m_name(name), m_rotationMode(ROT_MODE_NORMAL)
	{
		recalcModelMat();
	}

	void mesh::recalcModelMat()
	{
		mat4 scale = mat4::Scale(m_scale);
		//mat4 rotation = mat4::Rotz(m_rotation.z) * mat4::Roty(m_rotation.y) * mat4::Rotx(m_rotation.x);
		mat4 rotation;
		if (!m_rotationMode)
			rotation = mat4::Rotx(m_rotation.x) * mat4::Roty(m_rotation.y) * mat4::Rotz(m_rotation.z);
		else
			rotation = mat4::inverse3x3(mat4::lookAt({ m_rotation, 0.0f }));//if the mode is set to lookAt, set a lookAt-matrix instead of an actual rotation
		mat4 translation = mat4::transMat(m_position);
		mat4 shear; shear.setShearMat(m_shear[0], m_shear[1], m_shear[2], m_shear[3], m_shear[4], m_shear[5]);
		m_modelMat = translation * rotation * scale * shear;//first scale, then rotate(I guess the order doesn't matter here) and THEN translate(very important!!!)
	}

	void mesh::attachMovement(Ref_ptr<MeshMovement>& movement)
	{
		movement->attachToMesh(this);
		m_movements.push_back(movement);
	}

	void mesh::onUpdate(timestep& ts)
	{
		for (const auto& movement : m_movements)
		{
			movement->onUpdate(ts);
		}
		//AFTER the movements have been executed, move the attached lights accordingly
		for (auto& light : m_attachedPointLights)
		{
			light->position = m_position;
		}
		for (auto& light : m_attachedSpotLights)
		{
			light.first->position = m_position;//position is easy
			light.first->direction = mat4::Rotx(m_rotation.x) * mat4::Roty(m_rotation.y) * mat4::Rotz(m_rotation.z) * light.second;//for rotation I save the light's initial rotation and rotate it with the
			//model's rotationMatrix
		}
	}
}