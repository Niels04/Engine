#pragma once

#include "Engine/Rendering/material.hpp"
#include "Engine/Rendering/vertexArray.hpp"
#include "Engine/datatypes/include.hpp"

#include "Engine/ECS/Component.hpp"

class SceneGraphSystem;

namespace Engine
{
	class TransformComponent : public Component<TransformComponent>
	{
		friend class SceneGraphSystem;
	public:
		TransformComponent() : m_translation({ 0.0f, 0.0f, 0.0f }), m_rotation(0.0f, 0.0f, 0.0f), m_scale({ 1.0f, 1.0f, 1.0f }) {  }

		const vec3& getPos() const { return m_translation; }
		const vec3& getRot() const { return m_rotation; }
		const vec3& getScale() const { return m_scale; }

		inline void setPos(const vec3& newPos) { m_translation = newPos; recalcLocalTransform(); }
		inline void setRot(const vec3& newRot) { m_rotation = newRot; recalcLocalTransform(); }
		inline void setScale(const vec3& newScale) { m_scale = newScale; recalcLocalTransform(); }
		inline void setScale(const float newScale) { m_scale = { newScale, newScale, newScale }; recalcLocalTransform(); }
		inline void setShear(const float xy, const float xz, const float yx, const float yz, const float zx, const float zy);
		operator mat4() const { return global; }//this operator makes it possible to use the TransformComponent as if it were a mat4(returns the global transform)

		mat4 local;//gets set by the "user"
		mat4 global;//gets set by the SceneGraphSystem
	private:
		vec3 m_translation;
		vec3 m_rotation;
		vec3 m_scale;
		float m_shear[6] = { 0.0f };

		inline void recalcLocalTransform()
		{
			mat4 scale = mat4::Scale(m_scale);
			mat4 rotation = mat4::Rotx(m_rotation.x) * mat4::Roty(m_rotation.y) * mat4::Rotz(m_rotation.z);
			mat4 translation = mat4::transMat(m_translation);
			mat4 shear = mat4::shearMat(m_shear[0], m_shear[1], m_shear[2], m_shear[3], m_shear[4], m_shear[5]);
			local = translation * rotation * scale * shear;//first scale, then rotate(I guess the order doesn't matter here) and THEN translate(very important!!!)
		}
	};

	class MeshComponent : public Component<MeshComponent>
	{
	public:
		//maybe also implement a constructor, that only takes a vertexArray, and supplies a default material
		MeshComponent(const Ref_ptr<vertexArray>& vertexArray, const Ref_ptr<material>& material, const std::string& name)
			: m_geometry(vertexArray), m_material(material), m_name(name)
		{

		}
		MeshComponent(const MeshComponent& rhs) = delete;//don't want any copies
		MeshComponent& operator =(MeshComponent& rhs) = delete;//don't want any copies
		MeshComponent(MeshComponent&& other) noexcept//move constructor
		{
			//printf("Move-constructed a MeshComponent\n");
			if (this != &other)
			{
				m_geometry = other.m_geometry;
				other.m_geometry.reset();
				m_material = other.m_material;
				other.m_material.reset();
				m_name = std::move(other.m_name);
			}
		}
		MeshComponent& operator =(MeshComponent&& other) noexcept//assignment move operator
		{
			printf("Move-assigned a MeshComponent\n");
			if (this != &other)//make sure the objects aren't the same object
			{
				m_geometry = other.m_geometry;
				other.m_geometry.reset();
				m_material = other.m_material;
				other.m_material.reset();
				m_name = std::move(other.m_name);
			}
			return *this;
		}
		//void operator =(const MeshComponent& rhs) { m_geometry = rhs.m_geometry; m_material = rhs.m_material; m_name = rhs.m_name; }
		virtual ~MeshComponent() = default;
		//static Ref_ptr<MeshComponent> create(const Ref_ptr<vertexArray>&vertexArray, const Ref_ptr<material>&material, const std::string & name) {
			//return std::make_shared<MeshComponent>(vertexArray, material, name);
		//}

		Ref_ptr<vertexArray> getVa() const { return m_geometry; }
		WeakRef_ptr<material> getMaterial() const { return m_material; }
		const std::string& getName() const { return m_name; }
	private:
		//assets
		Ref_ptr<vertexArray> m_geometry;
		WeakRef_ptr<material> m_material;//store as weak_ptr because a mesh should never take ownership over it's material(material ownership is in hands of a class that manages materials)
		std::string m_name;

		/*inline void recalcNormalMat()
		{
			m_normalMat = mat3::transposed(mat3::inverse(mat3(m_modelMat)));
		}*/
	};

	class DirLightComponent : public Component<DirLightComponent>
	{
	public:
		DirLightComponent() = default;
		DirLightComponent(vec3 & lightDirection, vec3 & ambientColor, vec3 & diffuseColor, vec3 & specularColor)
			: direction(lightDirection, 1.0f), ambient(ambientColor, 1.0f), diffuse(diffuseColor, 1.0f), specular(specularColor, 1.0f)
		{

		}
		vec4 direction;
		vec4 ambient;
		vec4 diffuse;
		vec4 specular;
		cascadedDirLightMatrices shadowMatrices;
	};

	class PointLightComponent : public Component<PointLightComponent>
	{
	public:
		PointLightComponent() = default;
		PointLightComponent(vec3 & lightPosition, vec3 & ambientColor, vec3 & diffuseColor, vec3 & specularColor, vec3 & INattenuation)
			: position(lightPosition, 1.0f), ambient(ambientColor, 1.0f), diffuse(diffuseColor, 1.0f), specular(specularColor, 1.0f), attenuation(INattenuation, 0.0f)
		{

		}
		PointLightComponent(vec3 & lightPosition, vec3 & ambientColor, vec3 & diffuseColor, vec3 & specularColor, float Constant, float LinearFactor, float QuadraticFactor)
			: position(lightPosition, 1.0f), ambient(ambientColor, 1.0f), diffuse(diffuseColor, 1.0f), specular(specularColor, 1.0f), attenuation({ Constant, LinearFactor, QuadraticFactor, 0.0f })
		{

		}
		vec4 position;
		vec4 ambient;
		vec4 diffuse;
		vec4 specular;
		vec4 attenuation;//first is constant, second is linear, third is quadratic, fourth is padding
		pointLightMatrices shadowMatrices;
	};

	class SpotLightComponent : public Component<SpotLightComponent>
	{
	public:
		SpotLightComponent() = default;
		SpotLightComponent(vec3 & lightPosition, vec3 & spotDirection, vec3 & ambientColor, vec3 & diffuseColor, vec3 & specularColor, float cutOffAngle/*cosine of the light's cutoff angle*/)
			: position(lightPosition, 1.0f), direction(spotDirection, 0.0f), cutOff(cutOffAngle), ambient(ambientColor, 1.0f), diffuse(diffuseColor, 1.0f), specular(specularColor)
		{

		}
		vec4 position;
		vec4 direction;
		vec4 ambient;
		vec4 diffuse;
		vec3 specular;
		float cutOff;//the cosine of the spotLight's cutOff angle
		mat4 shadowMat;
	};
}