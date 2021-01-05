#pragma once

#include "Engine/datatypes/include.hpp"

namespace Engine
{
	class OrthographicCamera
	{
	public:
		OrthographicCamera() = default;
		OrthographicCamera(const float zNear, const float zFar, const float hFov, const float vFov);
		void set(const float left, const float right, const float bottom, const float top, const float Znear, const float Zfar);
		inline void setPos(const point& pos) { m_pos = pos; recalcViewMatrix(); }
		inline void setRot(const vec3& rot) { m_rot = rot; recalcViewMatrix(); }
		inline const point& getPos() const { return m_pos; }
		inline const vec3& getRot() const { return m_rot; }
		inline const mat4& getProjMat() const { return m_projMat; }
		inline const mat4& getViewMat() const { return m_viewMat; }
		inline const mat4& getViewProjMat() const { return m_viewProjMat; }
		//temporary function, as long as we don't have onUpdate()
		void recalcViewMatrix();

	private:
		mat4 m_projMat;
		mat4 m_viewMat;
		mat4 m_viewProjMat;
		point m_pos;
		vec3 m_rot;//rotation in eulerAngles
	};
}