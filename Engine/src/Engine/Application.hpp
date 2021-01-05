#pragma once

#include "core.hpp"
#include "Events/Event.hpp"
#include "window.hpp"

#include "Events/Event.hpp"
#include "Events/ApplicationEvent.hpp"

#include "layerStack.hpp"
#include "ImGui/ImGuiLayer.hpp"

#include "Engine/core/timeStep.hpp"

namespace Engine
{
	class Application
	{
	public:
		Application();
		virtual ~Application();
	
		void Run();

		void onEvent(Event& event);

		void pushLayer(layer* layer);
		void pushOverlay(layer* overlay);

		inline static Application& Get() { return *s_instance; }

		inline window& getWindow() { return *m_window; }
	private:
		bool onWindowCloseEvent(windowCloseEvent& e);
	private:
		std::unique_ptr<window> m_window;
		imGuiLayer* m_ImGuiLayer;
		bool m_running = true;
		layerStack m_layerStack;//created on the stack-> inherits the lifetime of the ApplicationClass
		float m_lastFrameTime = 0.0f;
	private:
		static Application* s_instance;
	};

	//to be defined in client
	Application* CreateApplication();
}