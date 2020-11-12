#include <Engine.hpp>
#include "imgui/imgui.h"

class renderLayer : public Engine::layer
{
public:
	renderLayer(const float hFov, const float zNear, const float zFar)
		: layer("RenderLayer"), m_hFov(hFov), m_zNear(zNear), m_zFar(zFar)
	{
		temporarySetup();
		fovRecalc(Engine::Application::Get().getWindow().getHeight(), Engine::Application::Get().getWindow().getWidth());//calculate the vertical fov and set the camera
	}

	void onUpdate(Engine::timestep ts) override
	{
		//temp
		//update pos
		if (Engine::input::isKeyDown(ENG_KEY_LEFT))
			m_camPos.x -= (m_camSpeed * ts);//multiply by delta time, in order to not tie movement-speeds to framerate
		else if (Engine::input::isKeyDown(ENG_KEY_RIGHT))
			m_camPos.x += (m_camSpeed * ts);
		if (Engine::input::isKeyDown(ENG_KEY_UP))
			m_camPos.y += (m_camSpeed * ts);
		else if (Engine::input::isKeyDown(ENG_KEY_DOWN))
			m_camPos.y -= (m_camSpeed * ts);
		m_cam.setPos({m_camPos.x, m_camPos.y, m_camPos.z});
		//update rot
		if (Engine::input::isKeyDown(ENG_KEY_Q))
			m_camRot.x += (m_camRotSpeed * ts);
		else if (Engine::input::isKeyDown(ENG_KEY_A))
			m_camRot.x -= (m_camRotSpeed * ts);
		if (Engine::input::isKeyDown(ENG_KEY_W))
			m_camRot.y += (m_camRotSpeed * ts);
		else if (Engine::input::isKeyDown(ENG_KEY_S))
			m_camRot.y -= (m_camRotSpeed * ts);
		if (Engine::input::isKeyDown(ENG_KEY_E))
			m_camRot.z += (m_camRotSpeed * ts);
		else if (Engine::input::isKeyDown(ENG_KEY_D))
			m_camRot.z -= (m_camRotSpeed * ts);
		m_cam.setRot(m_camRot);
		//end temp
		Engine::renderCommand::setClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
		Engine::renderCommand::clear();

		Engine::Renderer::beginScene(m_cam);
		{
			Engine::Renderer::sub(m_vertexArray, m_shader);
		}
		Engine::Renderer::endScene();

		Engine::Renderer::Flush();
	}

	void onImGuiRender() override
	{
		ImGui::Begin("RenderLayer");
		ImGui::End();
	}

	void onEvent(Engine::Event& e) override
	{
		Engine::eventDispatcher dispatcher(e);
		dispatcher.dispatchEvent<Engine::windowResizeEvent>(ENG_BIND_EVENT_FN(renderLayer::onWindowResizeEvent));
	}
	//onEvent-functions
	bool onWindowResizeEvent(Engine::windowResizeEvent& e)
	{
		fovRecalc(e.getHeight(), e.getWidth());
		return false;
	}

	void fovRecalc(unsigned int height, unsigned int width)
	{
		//calculate the verticalFov and initialize the camera
		float aspectRatioInverse = static_cast<float>(height) / static_cast<float>(width);
		float vFov = std::atanf(aspectRatioInverse * std::tanf(0.5f * m_hFov * (3.14159265f / 180.0f))) * 2.0f * (180.0f / 3.14159265f);
		m_cam.set(m_zNear, m_zFar, m_hFov, vFov);
	}

	void temporarySetup()
	{
		m_vertexArray.reset(new Engine::GLvertexArray);//we don't use a generalized implementation here because vertexArrays only exist in openGL

		float pVertexBuffer[7 * 3] = {
			-50.0f, -50.0f, -100.0f, 0.8f, 0.8f, 0.2f, 1.0f,
			50.0f, -50.0f, -100.0f, 0.8f, 0.2f, 0.8f, 1.0f,
			0.0f, 50.0f, -100.0f, 0.2f, 0.8f, 0.2f, 1.0f
		};
		m_vertexBuffer.reset(Engine::vertexBuffer::create(sizeof(pVertexBuffer), pVertexBuffer, STATIC_DRAW));
		unsigned int pIndexBuffer[3] = {
			0, 1, 2
		};
		m_indexBuffer.reset(Engine::indexBuffer::create(3, pIndexBuffer, STATIC_DRAW));

		std::shared_ptr<Engine::vertexBufferLayout> vertexBufferLayout;
		vertexBufferLayout.reset(Engine::vertexBufferLayout::create());
		vertexBufferLayout->push(Engine::ShaderDataType::vec3);//could also use "vertexBufferLayout.pushFloat(3);" to achieve the same thing
		vertexBufferLayout->push(Engine::ShaderDataType::vec4);//for color
		m_vertexBuffer->setLayout(vertexBufferLayout);//maybe make this accept a shared_ptr, so that we don't dealocate the memory we set the layout to
		//sadly we have to do an ugly conversion here because vertexArrays don't have a generalized implementation(cuz theire openGL only)
		m_vertexArray->addBuffer(std::dynamic_pointer_cast<Engine::GLvertexBuffer, Engine::vertexBuffer>(m_vertexBuffer));//tie the buffer and it's layout to the vertexArray
		m_vertexArray->addBuffer(std::dynamic_pointer_cast<Engine::GLindexBuffer, Engine::indexBuffer>(m_indexBuffer));//tie the indexBuffer to the vertexArray

		m_shader.reset(Engine::shader::create("basic.shader"));

		//unbind everything
		m_vertexArray->unbind();
		m_vertexBuffer->unbind();
		m_indexBuffer->unbind();
		m_shader->unbind();
	}
private:
	//temporary stuff
	std::shared_ptr<Engine::shader> m_shader;
	std::shared_ptr<Engine::indexBuffer> m_indexBuffer;
	std::shared_ptr<Engine::vertexBuffer> m_vertexBuffer;
	std::shared_ptr<Engine::GLvertexArray> m_vertexArray;
	//end temporary stuff

	Engine::perspectiveCamera m_cam;
	vec3 m_camPos;
	float m_camSpeed = 120.0f;
	vec3 m_camRot;
	float m_camRotSpeed = 60 * 0.0174533f;//1 degree in radians
	float m_zNear, m_zFar;
	float m_hFov;
};

class Sandbox : public Engine::Application
{
public:
	Sandbox()
	{
		pushLayer(new renderLayer(90.0f, 0.1f, 1000.0f));
	}
	~Sandbox()
	{
		
	}

};

Engine::Application* Engine::CreateApplication()
{
	return new Sandbox;
}