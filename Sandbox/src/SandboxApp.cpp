#include <Engine.hpp>

class exampleLayer : public Engine::layer
{
public:
	exampleLayer()
		: layer("example") {  }
	void onUpdate() override
	{
		//ENG_INFO("ExampleLayer::Update");
	}
	void onEvent(Engine::Event& e) override
	{
		ENG_TRACE("{0}", e);
	}
};

class Sandbox : public Engine::Application
{
public:
	Sandbox()
	{
		pushLayer(new exampleLayer);
		pushOverlay(new Engine::imGuiLayer);
	}
	~Sandbox()
	{
		
	}

};

Engine::Application* Engine::CreateApplication()
{
	return new Sandbox;
}