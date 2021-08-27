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
		PerspectiveCameraController() = default;
		void initialize(const float zNear, const float zFar, /*the horizontal fov*/const float Fov, const float aspectRatio);//used for initialization after a call to "clear"
		void clear();//clear the cameraController -> make a call to "initialize" before using it again
		inline const perspectiveCamera& getCam() const { return m_cam; }
		inline const vec4& getPos() const { return m_camPos; }
		inline const vec3& getRot() const { return m_camRot; }
		void onUpdate(timestep& ts, const bool rotate);
		void onEvent(Event& e);
		void DEBUG_setCamPos(const vec4& pos);
		void DEBUG_setCAmRot(const vec3& rot);
	private:
		bool onWindowResizedEvent(windowResizeEvent& e);
		bool onMouseScrolledEvent(mouseScrollEvent& e);
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
		float mouse_last[2] = { 0.0f };
	};
}