#include <Engpch.hpp>
#include "perspectiveCameraController.hpp"

#define sensitivity_x 0.5f
#define sensitivit_y 0.5f

namespace Engine
{
	void PerspectiveCameraController::onUpdate(timestep& ts, const bool rotate)
	{
		//update rot
		if (rotate)
		{
			std::pair<int, int> mouse_new = input::getMousePos();
			float mouse_delta[2] = { static_cast<float>(mouse_new.first - mouse_last[0]), static_cast<float>(mouse_new.second - mouse_last[1]) };
			mouse_last[0] = static_cast<float>(mouse_new.first); mouse_last[1] = static_cast<float>(mouse_new.second);
			m_camRot.y -= mouse_delta[0] * ts * sensitivit_y; //m_camRot.x -= mouse_delta[1] * ts * 0.5f;
			m_camRot.x = std::clamp(m_camRot.x - mouse_delta[1] * ts * sensitivity_x, -0.5f * 3.14159265f, 0.5f * 3.14159265f);//clamp this so that one can fully rotate around the x-axis
			if (input::isKeyDown(ENG_KEY_R))
				m_camRot.z += (m_rotSpeed * ts);
			else if (input::isKeyDown(ENG_KEY_T))
				m_camRot.z -= (m_rotSpeed * ts);
			m_cam.setRot(m_camRot);
			m_forward = mat4::Rotx(m_camRot.x) * mat4::Roty(m_camRot.y) * vec4(0.0f, 0.0f, -1.0f, 0.0f);
			//m_up = mat4::Rotx(m_camRot.x) * mat4::Rotz(m_camRot.z) * vec4(0.0f, 1.0f, 0.0f, 0.0f); <- don't change the up and down-vectors
			m_right = mat4::Roty(m_camRot.y) * mat4::Rotz(m_camRot.z) * vec4(1.0f, 0.0f, 0.0f, 0.0f);
		}
		//update pos
		if (input::isKeyDown(ENG_KEY_A))
			m_camPos -= (m_right * m_speed * ts);//multiply by delta time, in order to not tie movement-speeds to framerate
		else if (input::isKeyDown(ENG_KEY_D))
			m_camPos += (m_right * m_speed * ts);
		if (input::isKeyDown(ENG_KEY_SPACE))
			m_camPos += (m_up * m_speed * ts);
		else if (input::isKeyDown(ENG_KEY_CAPS_LOCK))
			m_camPos -= (m_up * m_speed * ts);
		if (input::isKeyDown(ENG_KEY_W))
			m_camPos += (m_forward * m_speed * ts);
		else if (input::isKeyDown(ENG_KEY_S))
			m_camPos -= (m_forward * m_speed * ts);
		m_cam.setPos(m_camPos.xyz());
	}

	void PerspectiveCameraController::initialize(const float zNear, const float zFar, const float Fov, const float aspectRatio)
	{
		m_cam.initialize(zNear, zFar, Fov, std::atanf((1.0f / aspectRatio) * std::tanf(0.5f * Fov * (3.14159265f / 180.0f))) * 2.0f * (180.0f / 3.14159265f));
		m_aspectRatio = aspectRatio;
		m_nearPlane = zNear; m_farPlane = zFar;
		m_fov = Fov;
	}

	void PerspectiveCameraController::clear()
	{
		//set everything back to original values
		m_cam.clear();
		m_camPos = { 0.0f, 0.0f, 0.0f, 1.0f };
		m_camRot = { 0.0f, 0.0f, 0.0f };
		m_up = { 0.0f, 1.0f, 0.0f, 0.0f };
		m_forward = { 0.0f, 0.0f, -1.0f, 0.0f };
		m_right = { 1.0f, 0.0f, 0.0f, 0.0f };
		m_zoomLevel = 1.0f;
		mouse_last[0] = 0.0f; mouse_last[1] = 0.0f;
		m_speed = 12.0f;
		m_rotSpeed = 60 * 0.0174533f;//1 degree in radians
	}

	void PerspectiveCameraController::onEvent(Event& e)
	{
		eventDispatcher dispatcher(e);
		dispatcher.dispatchEvent<mouseScrollEvent>(ENG_BIND_EVENT_FN(PerspectiveCameraController::onMouseScrolledEvent));;
		dispatcher.dispatchEvent<windowResizeEvent>(ENG_BIND_EVENT_FN(PerspectiveCameraController::onWindowResizedEvent));
	}

	bool PerspectiveCameraController::onWindowResizedEvent(windowResizeEvent& e)
	{
		float aspectRatioInverse = static_cast<float>(e.getHeight()) / static_cast<float>(e.getWidth());
		m_cam.initialize(m_nearPlane, m_farPlane, m_fov, std::atanf(aspectRatioInverse * std::tanf(0.5f * m_fov * (3.14159265f / 180.0f))) * 2.0f * (180.0f / 3.14159265f));
		return false;
	}
	bool PerspectiveCameraController::onMouseScrolledEvent(mouseScrollEvent& e)
	{
		//implement something that zooms here(probably manipulate the fov somehow
		return false;
	}
}