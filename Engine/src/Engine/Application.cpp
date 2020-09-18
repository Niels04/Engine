#include "Engpch.hpp"
#include "Application.hpp"

#include "input.hpp"

#include <glad/glad.h>//temporary, we are going to remove all glCode from here

namespace Engine
{
#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)//this means, that the functionPointer gets bound with the current instance of Application

	Application* Application::s_instance = nullptr;

	Application::Application()
	{
		ENG_CORE_ASSERT(!s_instance, "Application already exists!");
		s_instance = this;

		m_window = std::unique_ptr<window>(window::create());
		m_window->setEventCallbackFn(BIND_EVENT_FN(onEvent));

		m_ImGuiLayer = new imGuiLayer;//we create the imGuiLayer by default and push it as an overlay onto the layerstack
		pushOverlay(m_ImGuiLayer);
		{
			//temporary code
			m_vertexArray.reset(new GLvertexArray);//we don't use a generalized implementation here because vertexArrays only exist in openGL
			m_vertexArray->bind();

			float pVertexBuffer[7 * 3] = {
				-0.5f, -0.5f, 0.0f, 0.8f, 0.8f, 0.2f, 1.0f,
				0.5f, -0.5f, 0.0f, 0.8f, 0.2f, 0.8f, 1.0f,
				0.0f, 0.5f, 0.0f, 0.2f, 0.8f, 0.2f
			};
			m_vertexBuffer.reset(vertexBuffer::create(sizeof(pVertexBuffer), pVertexBuffer, STATIC_DRAW));
			unsigned int pIndexBuffer[3] = {
				0, 1, 2
			};
			m_indexBuffer.reset(indexBuffer::create(3, pIndexBuffer, STATIC_DRAW));

			m_vertexBufferLayout.reset(vertexBufferLayout::create());
			m_vertexBufferLayout->push(ShaderDataType::vec3);//could also use "vertexBufferLayout.pushFloat(3);" to achieve the same thing
			m_vertexBufferLayout->push(ShaderDataType::vec4);
			//m_vertexArray->addBuffer((GLvertexBuffer*)m_vertexBuffer.get(), (GLvertexBufferLayout*)m_vertexBufferLayout.get());

			m_vertexBuffer->setLayout(m_vertexBufferLayout.get());
			m_vertexBuffer->bindLayout();

			m_shader.reset(shader::create("basic.shader"));//we create a new shader with the graphicsAPI we are currently using(defined in core.hpp)

			//unbind everything
			m_vertexArray->unbind();
			m_vertexBuffer->unbind();
			m_indexBuffer->unbind();
			m_shader->unbind();
		}
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
			{
				glClearColor(0.1f, 0.1f, 0.1f, 1);
				glClear(GL_COLOR_BUFFER_BIT);

				m_shader->bind();
				m_vertexArray->bind();
				m_indexBuffer->bind();
				glDrawElements(GL_TRIANGLES, m_indexBuffer->getCount(), GL_UNSIGNED_INT, nullptr);
			}
			for (layer* layer : m_layerStack)//can use a range-based for-loop, because we implemented layerStack::begin() and layerStack::end()
				layer->onUpdate();

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