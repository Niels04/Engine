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

		virtual void onAttach() override;
		virtual void onDetach() override;
		virtual void onImGuiRender() override;

		void begin();
		void end();
	private:
		float m_time = 0.0f;//time is set to 0 on the first frame
	};

}