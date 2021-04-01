#pragma once
#include "Engine/Rendering/vertexArray.hpp"
#include "Engine/datatypes/include.hpp"

#include "Engine/Rendering/lights.hpp"

#include "Engine/Rendering/material.hpp"
#include "Engine/core/timeStep.hpp"

namespace Engine
{
	class MeshMovement;

	class mesh
	{
		friend class CircularMeshMovement;
	public:
		//maybe also implement a constructor, that only takes a vertexArray, and supplies a default material
		mesh(const Ref_ptr<vertexArray>& vertexArray, const Ref_ptr<material>& material, const std::string& name, const vec3& position,
			const vec3& rotation, const vec3& scale);
		virtual ~mesh() = default;
		static Ref_ptr<mesh> create(const Ref_ptr<vertexArray>& vertexArray, const Ref_ptr<material>& material, const std::string& name, const vec3& position = { 0.0f, 0.0f, 0.0f },
			const vec3& rotation = { 0.0f, 0.0f, 0.0f }, const vec3& scale = { 1.0f, 1.0f, 1.0f }) {
			return std::make_shared<mesh>(vertexArray, material, name, position, rotation, scale); }

		Ref_ptr<vertexArray> getVa() const { return m_geometry; }
		WeakRef_ptr<material> getMaterial() const { return m_material; }
		const mat4& getModelMat() const { return m_modelMat; }
		const mat3& getNormalMat() const { return m_normalMat; }
		const vec4& getPos() const { return m_position; }
		const vec3& getRot() const { return m_rotation; }
		const vec3& getScale() const { return m_scale; }
		const std::string& getName() const { return m_name; }

		inline void setPos(const vec4& newPos) { m_position = newPos; recalcModelMat(); }
		inline void setRot(const vec3& newRot) { m_rotation = newRot; recalcModelMat(); recalcNormalMat(); }
		inline void setScale(const vec3& newScale) { m_scale = newScale; recalcModelMat(); recalcNormalMat(); }
		inline void setScale(const float newScale) { m_scale = { newScale, newScale, newScale }; recalcModelMat(); recalcNormalMat(); }
		inline void setShear(const float xy, const float xz, const float yx, const float yz, const float zx, const float zy) {
			m_shear[0] = xy; m_shear[1] = xz; m_shear[2] = yx; m_shear[3] = yz;
			m_shear[4] = zx; m_shear[5] = zy; recalcModelMat();
		}
		void attachMovement(Ref_ptr<MeshMovement>& movement);
		inline void attachLight(PtrPtr<pointLight>& light)
		{
			m_attachedPointLights.push_back(light);
		}
		inline void attachLight(PtrPtr<spotLight>& light)
		{
			m_attachedSpotLights.push_back(std::make_pair(light, light->direction));//safe the light's initial direction, to rotate it according to the model's rotationMatrix
		}
		void onUpdate(timestep& ts);

	protected:
		//assets
		Ref_ptr<vertexArray> m_geometry;
		WeakRef_ptr<material> m_material;//store as weak_ptr because a mesh should never take ownership over it's material(material ownership is in hands of a class that manages materials)
		//where and how is the mesh in the world?
		vec4 m_position;
		vec3 m_rotation;
		vec3 m_scale;
		float m_shear[6] = {0};
		mat4 m_modelMat;//the meshe's model matrix -> get's calculated with respect to position, rotation, shear and scale
		mat3 m_normalMat;//the meshe's normalMat -> get's calculated with respect to rotation, shear and scale
		const std::string m_name;
		std::vector<Ref_ptr<MeshMovement>> m_movements;
		std::vector<PtrPtr<pointLight>> m_attachedPointLights;
		std::vector<std::pair<PtrPtr<spotLight>, vec4>> m_attachedSpotLights;

		void recalcModelMat();
		inline void recalcNormalMat()
		{
			m_normalMat = mat3::transposed(mat3::inverse(mat3(m_modelMat)));
		}
	};
}