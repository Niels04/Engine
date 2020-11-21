#include "Engpch.hpp"
#include "Application.hpp"
#include "input.hpp"

#include "Rendering/renderer.hpp"

//temporary includes
#include "GLFW/glfw3.h"

namespace Engine
{
#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)//this means, that the functionPointer gets bound with the current instance of Application

	Application* Application::s_instance = nullptr;

	Application::Application()
	{
		ENG_CORE_ASSERT(!s_instance, "Application already exists!");
		s_instance = this;
		//															  height width
		m_window = std::unique_ptr<window>(window::create({ "Engine", 720U, 1080U, }));
		m_window->setEventCallbackFn(BIND_EVENT_FN(onEvent));

		Engine::Renderer::init();//setup some stuff in the renderer, for which a current graphics context is needed

		m_ImGuiLayer = new imGuiLayer;//we create the imGuiLayer by default and push it as an overlay onto the layerstack
		pushOverlay(m_ImGuiLayer);
	}

	Application::~Application()
	{
		
	}

	void Application::onEvent(Event& event)
	{
		eventDispatcher dispatcher(event);
		dispatcher.dispatchEvent<windowCloseEvent>(BIND_EVENT_FN(onWindowCloseEvent));

		for (auto it = m_layerStack.end(); it != m_layerStack.begin();)
		{
			(*--it)->onEvent(event);
			if (event.handled())
				break;
		}
	}

	void Application::Run()
	{
		while (m_running)
		{
			//calculate delta-time
			float time = (float)
				glfwGetTime();//::::::::::SHOULD::BE::PLATFORM::SPECIFIC:::: //e.g. Platform::getTime()
			timestep Timestep(time - m_lastFrameTime);
			m_lastFrameTime = time;

			for (layer* layer : m_layerStack)//can use a range-based for-loop, because we implemented layerStack::begin() and layerStack::end()
				layer->onUpdate(Timestep);

			m_ImGuiLayer->begin();//we begin a new frame in our ImGuiLayer
			for (layer* layer : m_layerStack)//we iterate through all layers and call each layer's onImGuiRender func
				layer->onImGuiRender();
			m_ImGuiLayer->end();//then we render everything and end the ImGui-Frame

			m_window->onUpdate();
		}
	}

	bool Application::onWindowCloseEvent(windowCloseEvent& e)
	{
		ENG_TRACE("{0}", e);

		m_running = false;
		return true;
	}

	void Application::pushLayer(layer* layer)
	{
		m_layerStack.pushLayer(layer);
		layer->onAttach();
	}

	void Application::pushOverlay(layer* overlay)
	{
		m_layerStack.pushOverlay(overlay);
		overlay->onAttach();
	}

}