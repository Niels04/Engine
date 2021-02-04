#include <Engpch.hpp>
#include "perspectiveCameraController.hpp"

namespace Engine
{
	void PerspectiveCameraController::onUpdate(timestep& ts)
	{
		m_forward = mat4::Rotx(m_camRot.x) * mat4::Roty(m_camRot.y) * vec4( 0.0f, 0.0f, -1.0f, 0.0f );
		m_up = mat4::Rotx(m_camRot.x) * mat4::Rotz(m_camRot.z) * vec4(0.0f, 1.0f, 0.0f, 0.0f);
		m_right = mat4::Roty(m_camRot.y) * mat4::Rotz(m_camRot.z) * vec4(1.0f, 0.0f, 0.0f, 0.0f);
		//update rot
		if (input::isKeyDown(ENG_KEY_Q))
			m_camRot.x += (m_rotSpeed * ts);
		else if (input::isKeyDown(ENG_KEY_A))
			m_camRot.x -= (m_rotSpeed * ts);
		if (input::isKeyDown(ENG_KEY_W))
			m_camRot.y += (m_rotSpeed * ts);
		else if (input::isKeyDown(ENG_KEY_S))
			m_camRot.y -= (m_rotSpeed * ts);
		if (input::isKeyDown(ENG_KEY_E))
			m_camRot.z += (m_rotSpeed * ts);
		else if (input::isKeyDown(ENG_KEY_D))
			m_camRot.z -= (m_rotSpeed * ts);
		m_cam.setRot(m_camRot);
		//update pos
		if (input::isKeyDown(ENG_KEY_LEFT))
			m_camPos -= (m_right * m_speed * ts);//multiply by delta time, in order to not tie movement-speeds to framerate
		else if (input::isKeyDown(ENG_KEY_RIGHT))
			m_camPos += (m_right * m_speed * ts);
		if (input::isKeyDown(ENG_KEY_SPACE))
			m_camPos += (m_up * m_speed * ts);
		else if (input::isKeyDown(ENG_KEY_CAPS_LOCK))
			m_camPos -= (m_up * m_speed * ts);
		if (input::isKeyDown(ENG_KEY_UP))
			m_camPos += (m_forward * m_speed * ts);
		else if (input::isKeyDown(ENG_KEY_DOWN))
			m_camPos -= (m_forward * m_speed * ts);
		m_cam.setPos(m_camPos.xyz());
	}
	void PerspectiveCameraController::onEvent(Event& e)
	{
		eventDispatcher dispatcher(e);
		dispatcher.dispatchEvent<mouseScrollEvent>(ENG_BIND_EVENT_FN(PerspectiveCameraController::onMouseScrolledEvent));;
		dispatcher.dispatchEvent<mouseMoveEvent>(ENG_BIND_EVENT_FN(PerspectiveCameraController::onMouseMovedEvent));
		dispatcher.dispatchEvent<windowResizeEvent>(ENG_BIND_EVENT_FN(PerspectiveCameraController::onWindowResizedEvent));
	}

	bool PerspectiveCameraController::onWindowResizedEvent(windowResizeEvent& e)
	{
		float aspectRatioInverse = static_cast<float>(e.getHeight()) / static_cast<float>(e.getWidth());
		m_cam.set(m_nearPlane, m_farPlane, m_fov, std::atanf(aspectRatioInverse * std::tanf(0.5f * m_fov * (3.14159265f / 180.0f))) * 2.0f * (180.0f / 3.14159265f));
		return false;
	}
	bool PerspectiveCameraController::onMouseScrolledEvent(mouseScrollEvent& e)
	{
		//implement something that zooms here(probably manipulate the fov somehow
		return false;
	}
	bool PerspectiveCameraController::onMouseMovedEvent(mouseMoveEvent& e)
	{
		//implement something that rotates the camera
		return false;
	}
}