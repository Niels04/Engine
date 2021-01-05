#include "Engpch.hpp"
#include "mesh.hpp"

namespace Engine
{
	mesh::mesh(const Ref_ptr<vertexArray>& vertexArray, const Ref_ptr<material>& material, const vec3& position, const vec3& rotation, const vec3& scale)
		: m_geometry(vertexArray), m_material(material), m_position(position), m_rotation(rotation), m_scale(scale)
	{
		recalcModelMat();
	}

	void mesh::recalcModelMat()
	{
		mat4 scale = mat4::Scale(m_scale);
		mat4 rotation = mat4::Rotx(m_rotation.x) * mat4::Roty(m_rotation.y) * mat4::Rotz(m_rotation.z);
		mat4 translation = mat4::transMat(m_position);
		mat4 shear; shear.setShearMat(m_shear[0], m_shear[1], m_shear[2], m_shear[3], m_shear[4], m_shear[5]);
		m_modelMat = translation * rotation * scale * shear;//first scale, then rotate(I guess the order doesn't matter here) and THEN translate(very important!!!)
	}
}