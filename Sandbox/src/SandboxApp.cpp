#include <Engine.hpp>
#include "imgui/imgui.h"

class renderLayer : public Engine::layer
{
public:
	renderLayer(const float hFov, const float zNear, const float zFar)
		: layer("RenderLayer"), m_hFov(hFov), m_zNear(zNear), m_zFar(zFar)
	{
		fovRecalc(Engine::Application::Get().getWindow().getHeight(), Engine::Application::Get().getWindow().getWidth());//calculate the vertical fov and set the camera
		temporarySetup();
	}
	~renderLayer()
	{
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
			m_flatColorShader->bind();
			m_flatColorShader->setUniform4f("u_color", m_quad_color);
			m_flatColorShader->unbind();
			Engine::Renderer::sub(m_vertexArray, m_flatColorShader, mat4::transMat(0.0f, 150.0f, 0.0f));
			Engine::Renderer::sub(m_vertexArray, m_textureShader);
		}
		Engine::Renderer::endScene();

		Engine::Renderer::Flush();
	}

	void onImGuiRender() override
	{
		ImGui::Begin("RenderLayer");
		ImGui::ColorEdit4("QuadColor", &m_quad_color.x);
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

		float pVertexBuffer[5 * 4] = {
			-50.0f, -50.0f, -100.0f, 0.0f, 0.0f,
			50.0f, -50.0f, -100.0f, 1.0f, 0.0f,
			-50.0f, 50.0f, -100.0f, 0.0f, 1.0f,
			50.0f, 50.0f, -100.0f, 1.0f, 1.0f
		};
		m_vertexBuffer = Engine::vertexBuffer::create(sizeof(pVertexBuffer), pVertexBuffer, STATIC_DRAW);
		unsigned int pIndexBuffer[6] = {
			0, 1, 2, 1, 3, 2
		};
		m_indexBuffer = Engine::indexBuffer::create(6, pIndexBuffer, STATIC_DRAW);

		Engine::Ref_ptr<Engine::vertexBufferLayout> VertexBufferLayout = Engine::vertexBufferLayout::create();
		VertexBufferLayout->push(Engine::ShaderDataType::vec3);//could also use "vertexBufferLayout.pushFloat(3);" to achieve the same thing
		VertexBufferLayout->push(Engine::ShaderDataType::vec2);//for tex-coords
		m_vertexBuffer->setLayout(VertexBufferLayout);//maybe make this accept a shared_ptr, so that we don't dealocate the memory we set the layout to
		//sadly we have to do an ugly conversion here because vertexArrays don't have a generalized implementation(cuz theire openGL only)
		m_vertexArray->addBuffer(std::dynamic_pointer_cast<Engine::GLvertexBuffer, Engine::vertexBuffer>(m_vertexBuffer));//tie the buffer and it's layout to the vertexArray
		m_vertexArray->addBuffer(std::dynamic_pointer_cast<Engine::GLindexBuffer, Engine::indexBuffer>(m_indexBuffer));//tie the indexBuffer to the vertexArray

		m_flatColorShader = Engine::shader::create("flatColor.shader");
		m_flatColorShader->bindUniformBlock("ViewProjection", 0);//bind the UniformBlock "ViewProjection" to the default bindingPoint for viewProjectionMatrices, which is 0 //do this for each shader, that uses the view and the projection matrix
		m_textureShader = Engine::shader::create("textureShader.shader");
		m_textureShader->bindUniformBlock("ViewProjection", 0);

		m_texture = Engine::texture2d::create("crimson_fungus.png");
		m_texture->bind(0);
		m_textureShader->bind();
		m_textureShader->setUniform1i("u_texture", 0);
		m_textureShader->unbind();

		//unbind everything
		m_vertexArray->unbind();
		m_vertexBuffer->unbind();
		m_indexBuffer->unbind();
		m_flatColorShader->unbind();
		m_textureShader->unbind();
	}
private:
	//temporary stuff
	Engine::Ref_ptr<Engine::shader> m_flatColorShader, m_textureShader;
	Engine::Ref_ptr<Engine::indexBuffer> m_indexBuffer;
	Engine::Ref_ptr<Engine::vertexBuffer> m_vertexBuffer;
	Engine::Ref_ptr<Engine::GLvertexArray> m_vertexArray;
	Engine::Ref_ptr<Engine::globalBuffer> m_globalBuffer;
	Engine::Ref_ptr<Engine::texture2d> m_texture;
	vec4 m_quad_color = { 0.1f, 0.5f, 0.8f, 1.0f };
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