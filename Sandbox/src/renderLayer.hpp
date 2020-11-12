#pragma once
#include "Engine.hpp"

#define BIND_EVENT_FN(x) std::bind(&x, this, std::placeholders::_1)

class renderLayer : public Engine::layer
{
public:
	renderLayer()//initializes the vertical fov based upon the horizontal fov and the current width&height of the window, this gets recalculated when the window gets resized
		: layer("Renderer"), m_vfov(atanf(((float)Engine::Application::Get().getWindow().getHeight() / (float)Engine::Application::Get().getWindow().getWidth())* tanf(m_hfov * 0.5f * (3.14159f / 180.0f)))* (180.0f / 3.14159f) * 2.0f)
	{
		temporarySetup();//does all the temporary stuff like setting up a vb and a shader
	}

	void onUpdate() override;
	void onImGuiRender() override;
	void onEvent(Engine::Event& e) override;
	bool onWindowResizeEvent(Engine::windowResizeEvent& e);

	void temporarySetup();
private:
	//temporary stuff
	std::unique_ptr<Engine::shader> m_shader;
	std::shared_ptr<Engine::indexBuffer> m_indexBuffer;
	std::shared_ptr<Engine::vertexBuffer> m_vertexBuffer;
	std::shared_ptr<Engine::GLvertexArray> m_vertexArray;
	//end temporary stuff

	float m_hfov = 90.0f;//we set the horizontal fov to 90 degrees by default - maybe gonna add an ImGui-Slider for fov in the future
	float m_vfov = 0;//gets initialized in constructor
};