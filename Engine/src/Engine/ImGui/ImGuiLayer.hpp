#pragma once

#include "Engine/Events/KeyEvent.hpp"
#include "Engine/Events/MouseEvent.hpp"
#include "Engine/Events/ApplicationEvent.hpp"
#include "Engine/layer.hpp"

namespace Engine
{
	class ENGINE_API imGuiLayer : public layer
	{
	public:
		imGuiLayer();
		~imGuiLayer();

		void onAttach() override;
		void onDetach() override;
		void onUpdate() override;
		void onEvent(Event& e) override;

	private:
		bool onKeyPressedEvent(keyPressedEvent& e);
		bool onKeyReleasedEvent(keyReleasedEvent& e);
		bool onKeyTypedEvent(keyTypedEvent& e);
		bool onMouseButtonPressedEvent(mouseButtonPressedEvent& e);
		bool onMouseButtonReleasedEvent(mouseButtonReleasedEvent& e);
		bool onMouseMovedEvent(mouseMoveEvent& e);
		bool onMouseScrollEvent(mouseScrollEvent& e);
		bool onWindowResizeEvent(windowResizeEvent& e);
	private:
		float m_time = 0.0f;//time is set to 0 on the first frame
	};

}