#pragma once

#include "core.hpp"
#include "Events/Event.hpp"
#include "window.hpp"

#include "Events/Event.hpp"
#include "Events/ApplicationEvent.hpp"

#include "layerStack.hpp"

namespace Engine
{

	class  ENGINE_API Application
	{
	public:
		Application();
		virtual ~Application();
	
		void Run();

		void onEvent(Event& event);

		void pushLayer(layer* layer);
		void pushOverlay(layer* overlay);
	private:
		bool onWindowCloseEvent(windowCloseEvent& e);

		std::unique_ptr<window> m_window;
		bool m_running = true;
		layerStack m_layerStack;//created on the stack->kinda inherits the lifetime of the ApplicationClass
	};

	//to be defined in client
	Application* CreateApplication();

}