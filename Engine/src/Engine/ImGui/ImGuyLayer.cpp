#include "Engpch.hpp"
#include "ImGuiLayer.hpp"
#include "Engine/Application.hpp"

#include "imgui.h"
#include "Platform/OpenGL/imGuiOpenGLrenderer.h"

//Temporary
#include "glad/glad.h"
#include "GLFW/glfw3.h"



namespace Engine
{
	imGuiLayer::imGuiLayer()
		: layer("ImGuiLayer")
	{

	}
	imGuiLayer::~imGuiLayer()
	{

	}

	void imGuiLayer::onAttach()
	{
		ImGuiContext* context = ImGui::CreateContext();
		ImGui::SetCurrentContext(context);
		ImGui::StyleColorsDark();

		ImGuiIO& io = ImGui::GetIO();
		io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;//add some backendFlags with the or operator
		io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;

		//this is just temporary, at some point this should be using our own keycodes
		io.KeyMap[ImGuiKey_Tab] = GLFW_KEY_TAB;
		io.KeyMap[ImGuiKey_LeftArrow] = GLFW_KEY_LEFT;
		io.KeyMap[ImGuiKey_RightArrow] = GLFW_KEY_RIGHT;
		io.KeyMap[ImGuiKey_UpArrow] = GLFW_KEY_UP;
		io.KeyMap[ImGuiKey_DownArrow] = GLFW_KEY_DOWN;
		io.KeyMap[ImGuiKey_PageUp] = GLFW_KEY_PAGE_UP;
		io.KeyMap[ImGuiKey_PageDown] = GLFW_KEY_PAGE_DOWN;
		io.KeyMap[ImGuiKey_Home] = GLFW_KEY_HOME;
		io.KeyMap[ImGuiKey_End] = GLFW_KEY_END;
		io.KeyMap[ImGuiKey_Insert] = GLFW_KEY_INSERT;
		io.KeyMap[ImGuiKey_Delete] = GLFW_KEY_DELETE;
		io.KeyMap[ImGuiKey_Backspace] = GLFW_KEY_BACKSPACE;
		io.KeyMap[ImGuiKey_Space] = GLFW_KEY_SPACE;
		io.KeyMap[ImGuiKey_Enter] = GLFW_KEY_ENTER;
		io.KeyMap[ImGuiKey_Escape] = GLFW_KEY_ESCAPE;
		io.KeyMap[ImGuiKey_KeyPadEnter] = GLFW_KEY_KP_ENTER;
		io.KeyMap[ImGuiKey_A] = GLFW_KEY_A;
		io.KeyMap[ImGuiKey_C] = GLFW_KEY_C;
		io.KeyMap[ImGuiKey_V] = GLFW_KEY_V;
		io.KeyMap[ImGuiKey_X] = GLFW_KEY_X;
		io.KeyMap[ImGuiKey_Y] = GLFW_KEY_Y;
		io.KeyMap[ImGuiKey_Z] = GLFW_KEY_Z;
		
		ImGui_ImplOpenGL3_Init("#version 410");
	}

	void imGuiLayer::onDetach()
	{

	}

	void imGuiLayer::onUpdate()
	{
		ImGuiIO& io = ImGui::GetIO();
		Application& app = Application::Get();

		io.DisplaySize = ImVec2(app.getWindow().getWidth(), app.getWindow().getHeight());
		float time = (float)glfwGetTime();
		io.DeltaTime = m_time > 0.0f ? (time - m_time) : (1.0f / 60.0f);
		m_time = time;

		ImGui_ImplOpenGL3_NewFrame();
		ImGui::NewFrame();

		static bool show = true;
		ImGui::ShowDemoWindow(&show);

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}

	void imGuiLayer::onEvent(Event& e)
	{
		eventDispatcher dispatcher(e);
		dispatcher.dispatchEvent<keyPressedEvent>(ENG_BIND_EVENT_FN(imGuiLayer::onKeyPressedEvent));
		dispatcher.dispatchEvent<keyReleasedEvent>(ENG_BIND_EVENT_FN(imGuiLayer::onKeyReleasedEvent));
		dispatcher.dispatchEvent<keyTypedEvent>(ENG_BIND_EVENT_FN(imGuiLayer::onKeyTypedEvent));
		dispatcher.dispatchEvent<mouseButtonPressedEvent>(ENG_BIND_EVENT_FN(imGuiLayer::onMouseButtonPressedEvent));
		dispatcher.dispatchEvent<mouseButtonReleasedEvent>(ENG_BIND_EVENT_FN(imGuiLayer::onMouseButtonReleasedEvent));
		dispatcher.dispatchEvent<mouseMoveEvent>(ENG_BIND_EVENT_FN(imGuiLayer::onMouseMovedEvent));
		dispatcher.dispatchEvent<mouseScrollEvent>(ENG_BIND_EVENT_FN(imGuiLayer::onMouseScrollEvent));
		dispatcher.dispatchEvent<windowResizeEvent>(ENG_BIND_EVENT_FN(imGuiLayer::onWindowResizeEvent));
	}

	bool imGuiLayer::onKeyPressedEvent(keyPressedEvent& e)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.KeysDown[e.getKeyCode()] = true;

		//set modify keys:
		io.KeyCtrl = io.KeysDown[GLFW_KEY_LEFT_CONTROL] || io.KeysDown[GLFW_KEY_RIGHT_CONTROL];
		io.KeyShift = io.KeysDown[GLFW_KEY_LEFT_SHIFT] || io.KeysDown[GLFW_KEY_RIGHT_SHIFT];
		io.KeyAlt = io.KeysDown[GLFW_KEY_LEFT_ALT] || io.KeysDown[GLFW_KEY_RIGHT_ALT];
		io.KeySuper = io.KeysDown[GLFW_KEY_LEFT_SUPER] || io.KeysDown[GLFW_KEY_RIGHT_SUPER];//on win this is the win-key

		return false;
	}

	bool imGuiLayer::onKeyReleasedEvent(keyReleasedEvent& e)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.KeysDown[e.getKeyCode()] = false;

		return false;
	}

	bool imGuiLayer::onKeyTypedEvent(keyTypedEvent& e)
	{
		ImGuiIO& io = ImGui::GetIO();
		
		int keycode = e.getKeyCode();
		if(keycode > 0 && keycode < 0x10000)
			io.AddInputCharacter((unsigned short)keycode);

		return false;
	}

	bool imGuiLayer::onMouseButtonPressedEvent(mouseButtonPressedEvent& e)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.MouseDown[e.getMouseButton()] = true;

		return false;//because we want other layers to potentially handle this too
	}

	bool imGuiLayer::onMouseButtonReleasedEvent(mouseButtonReleasedEvent& e)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.MouseDown[e.getMouseButton()] = false;

		return false;//because we want other layers to potentially handle this too
	}

	bool imGuiLayer::onMouseMovedEvent(mouseMoveEvent& e)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.MousePos = ImVec2(e.getX(), e.getY());

		return false;//because we want other layers to potentially handle this too
	}

	bool imGuiLayer::onMouseScrollEvent(mouseScrollEvent& e)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.MouseWheelH += e.getXoffset();
		io.MouseWheel += e.getYoffset();

		return false;//because we want other layers to potentially handle this too
	}

	bool imGuiLayer::onWindowResizeEvent(windowResizeEvent& e)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.DisplaySize = ImVec2(e.getWidth(), e.getHeight());
		io.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);
		glViewport(0, 0, e.getWidth(), e.getHeight());

		return false;
	}
}