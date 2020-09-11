#include <Engine.hpp>

#include "imgui/imgui.h"

class exampleLayer : public Engine::layer
{
public:
	exampleLayer()
		: layer("example") {  }
	void onUpdate() override
	{
		
	}

	void onImGuiRender() override
	{
		ImGui::Begin("Example layer window!");
		ImGui::Text("Example layer window text!");
		ImGui::End();
	}

	void onEvent(Engine::Event& e) override
	{
		
	}
};

class Sandbox : public Engine::Application
{
public:
	Sandbox()
	{
		pushLayer(new exampleLayer);
	}
	~Sandbox()
	{
		
	}

};

Engine::Application* Engine::CreateApplication()
{
	return new Sandbox;
}