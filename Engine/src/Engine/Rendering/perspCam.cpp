#include "Engpch.hpp"
#include "perspCam.hpp"

namespace Engine
{
	perspectiveCamera::perspectiveCamera()
	{

	}
	perspectiveCamera::perspectiveCamera(float zNear, float zFar, float hFov, float vFov)
	{
		m_projMat.setProjMat(zNear, zFar, hFov, vFov);
		m_projMat.transpose();//maybe build the matrix the other way around instead of having to transpose it all the time
		m_viewProjMat = m_projMat * m_viewMat;
	}
	void perspectiveCamera::set(float zNear, float zFar, float hFov, float vFov)
	{
		m_projMat.setProjMat(zNear, zFar, hFov, vFov);
		m_projMat.transpose();//maybe build the matrix the other way around instead of having to transpose it all the time
		m_viewProjMat = m_projMat * m_viewMat;
	}
	void perspectiveCamera::recalcViewMatrix()
	{
		mat4 rotation = mat4::Rotx(-m_rot.x) * mat4::Roty(-m_rot.y) * mat4::Rotz(-m_rot.z);
		mat4 translation = mat4::transMat(-m_pos);
		m_viewMat = rotation * translation;//okay, actually do the translation first, it's logic if u think about it
		//m_viewMat = translation * rotation;//important order!!! (rotation get's done first and therefore written the last)
		m_viewProjMat = m_projMat * m_viewMat;//same here, we first transform(viewMatrix) and then project(projectionMatrix), so write the viewMat last
	}
}