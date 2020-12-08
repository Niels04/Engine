#pragma once
#include "Engine/Rendering/vertexArray.hpp"
#include "Engine/datatypes/include.hpp"

#include "Engine/Rendering/material.hpp"

namespace Engine
{
	class mesh
	{
	public:
		//maybe also implement a constructor, that only takes a vertexArray, and supplies a default material
		mesh(const Ref_ptr<vertexArray>& vertexArray, const Ref_ptr<material>& material, const vec3& position = { 0.0f, 0.0f, 0.0f },
			const vec3& rotation = { 0.0f, 0.0f, 0.0f }, const vec3& scale = { 1.0f, 1.0f, 1.0f });

		Ref_ptr<vertexArray> getVa() const { return m_geometry; }
		WeakRef_ptr<material> getMaterial() const { return m_material; }
		const mat4& getModelMat() const { return m_modelMat; }

		inline void setPos(const vec3& newPos) { m_position = newPos; recalcModelMat(); }
		inline void setRot(const vec3& newRot) { m_rotation = newRot; recalcModelMat(); }
		inline void setScale(const vec3& newScale) { m_scale = newScale; recalcModelMat(); }
		inline void setScale(const float newScale) { m_scale = { newScale, newScale, newScale }; recalcModelMat(); }
	private:
		//assets
		Ref_ptr<vertexArray> m_geometry;
		WeakRef_ptr<material> m_material;//store as weak_ptr because a mesh should never take ownership over it's material(material ownership is in hands of a class that manages materials)
		//where and how is the mesh in the world?
		vec3 m_position;
		vec3 m_rotation;
		vec3 m_scale;
		mat4 m_modelMat;//the meshe's model matrix -> get's calculated with respect to position, rotation and scale once per frame

		void recalcModelMat();
	};
}