#pragma once
#include "Rendering/perspCam.hpp"
#include "core/timeStep.hpp"
#include "Events/ApplicationEvent.hpp"
#include "Events/KeyEvent.hpp"
#include "Events/MouseEvent.hpp"
#include "input.hpp"
#include "keyCodes.hpp"

namespace Engine
{
	class PerspectiveCameraController
	{
	public:
		PerspectiveCameraController(const float zNear, const float zFar, /*the horizontal fov*/const float Fov, const float aspectRatio)
			: m_cam(zNear, zFar, Fov, std::atanf((1.0f / aspectRatio)* std::tanf(0.5f * Fov * (3.14159265f / 180.0f))) * 2.0f * (180.0f / 3.14159265f)), m_aspectRatio(aspectRatio), m_nearPlane(zNear),
			m_farPlane(zFar), m_fov(Fov)
		{

		}
		inline const perspectiveCamera& getCam() { return m_cam; }
		inline const vec4& getPos() const { return m_camPos; }
		inline const vec3& getRot() const { return m_camRot; }
		void onUpdate(timestep& ts);
		void onEvent(Event& e);
	private:
		bool onWindowResizedEvent(windowResizeEvent& e);
		bool onMouseScrolledEvent(mouseScrollEvent& e);
		bool onMouseMovedEvent(mouseMoveEvent& e);
	private:
		float m_aspectRatio;
		float m_speed = 12.0f;
		float m_rotSpeed = 60 * 0.0174533f;//1 degree in radians
		float m_zoomLevel = 1.0f;
		vec4 m_up = { 0.0f, 1.0f, 0.0f, 0.0f };
		vec4 m_forward = { 0.0f, 0.0f, -1.0f, 0.0f };
		vec4 m_right = { 1.0f, 0.0f, 0.0f, 0.0f };
		vec4 m_camPos;
		vec3 m_camRot;
		float m_nearPlane;
		float m_farPlane;
		float m_fov;//the horizontal fov
		perspectiveCamera m_cam;
	};
}