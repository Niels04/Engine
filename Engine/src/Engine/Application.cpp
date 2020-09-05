#include "Engpch.hpp"
#include "Application.hpp"

#include <glad/glad.h>

namespace Engine
{
#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)//this means, that the functionPointer gets bound with the current instance of Application

	Application::Application()
	{
		m_window = std::unique_ptr<window>(window::create());
		m_window->setEventCallbackFn(BIND_EVENT_FN(onEvent));
	}

	Application::~Application()
	{

	}

	int testfunc(int testparam)
	{
		return testparam;
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
			glClearColor(1, 0, 1, 1);
			glClear(GL_COLOR_BUFFER_BIT);

			for (layer* layer : m_layerStack)//can use a range-based for-loop, because we implemented layerStack::begin() and layerStack::end()
				layer->onUpdate();

			m_window->onUpdate();
		}
	}

	bool Application::onWindowCloseEvent(windowCloseEvent& e)
	{
		m_running = false;
		return true;
	}

	void Application::pushLayer(layer* layer)
	{
		m_layerStack.pushLayer(layer);
	}

	void Application::pushOverlay(layer* overlay)
	{
		m_layerStack.pushOverlay(overlay);
	}

}