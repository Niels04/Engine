#pragma once

namespace Engine
{
	class perspectiveCamera
	{
	public:
		perspectiveCamera() = default;
		perspectiveCamera(const float zNear, const float zFar, const float hFov, const float vFov);
		void initialize(const float zNear, const float zFar, const float hFov, const float vFov);//used for initialization after a call to "clear"
		void clear();//clear the camera -> before usage make a call to "initialize"
		inline void setPos(const vec3& pos) { m_pos = pos; recalcViewMatrix(); }
		inline void setRot(const vec3& rot) { m_rot = rot; recalcViewMatrix(); }
		inline const vec3& getPos() const { return m_pos; }
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
		vec3 m_pos;
		vec3 m_rot;//rotation in eulerAngles
	};
}