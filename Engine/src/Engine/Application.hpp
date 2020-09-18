#pragma once

#include "core.hpp"
#include "Events/Event.hpp"
#include "window.hpp"

#include "Events/Event.hpp"
#include "Events/ApplicationEvent.hpp"

#include "layerStack.hpp"
#include "ImGui/ImGuiLayer.hpp"

#include "Rendering/buffers.hpp"
#include "Rendering/vertexBufferLayout.hpp"
#include "Rendering/shader.hpp"
//openGL specific includes(because OpenGL has some features other APIs don't have
#include "Platform/OpenGL/OpenGLVertexArray.hpp"

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

		inline static Application& Get() { return *s_instance; }

		inline window& getWindow() { return *m_window; }
	private:
		bool onWindowCloseEvent(windowCloseEvent& e);

		std::unique_ptr<window> m_window;
		imGuiLayer* m_ImGuiLayer;
		bool m_running = true;
		layerStack m_layerStack;//created on the stack->kinda inherits the lifetime of the ApplicationClass
		//these are just temporary
		std::unique_ptr<shader> m_shader;
		std::unique_ptr<indexBuffer> m_indexBuffer;
		std::unique_ptr<vertexBuffer> m_vertexBuffer;
		std::unique_ptr<vertexBufferLayout> m_vertexBufferLayout;
		std::unique_ptr<GLvertexArray> m_vertexArray;
	private:
		static Application* s_instance;
	};

	//to be defined in client
	Application* CreateApplication();
}