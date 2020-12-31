#include <Engine.hpp>
#include "imgui/imgui.h"

#define PI 3.1415926f

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
		if (Engine::input::isKeyDown(ENG_KEY_SPACE))
			m_camPos.y += (m_camSpeed * ts);
		else if (Engine::input::isKeyDown(ENG_KEY_CAPS_LOCK))
			m_camPos.y -= (m_camSpeed * ts);
		if (Engine::input::isKeyDown(ENG_KEY_UP))
			m_camPos.z -= (m_camSpeed * ts);
		else if (Engine::input::isKeyDown(ENG_KEY_DOWN))
			m_camPos.z += (m_camSpeed * ts);
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
			Engine::Renderer::sub(m_mesh);
			Engine::Renderer::sub(m_cube);
		}
		Engine::Renderer::endScene();

		Engine::Renderer::Flush();
	}

	void onImGuiRender() override
	{
		ImGui::Begin("Camera");
		ImGui::Text("position:");
		ImGui::Text("x: %.1f  y: %.1f  z: %.1f", m_camPos.x, m_camPos.y, m_camPos.z);
		ImGui::Text("rotation:");
		ImGui::Text("x: %.1f  y: %.1f  z: %.1f", m_camRot.x, m_camRot.y, m_camRot.z);
		ImGui::End();
		ImGui::Begin("PointLight");
		ImGui::ColorEdit3("ambient", (float*)&(*m_lamp)->ambient);
		ImGui::ColorEdit3("diffuse", (float*)&(*m_lamp)->diffuse);
		ImGui::ColorEdit3("specular", (float*)&(*m_lamp)->specular);
		ImGui::InputFloat3("position", (float*)&(*m_lamp)->position, 3);
		ImGui::End();
		ImGui::Begin("SpotLight[0]");
		ImGui::ColorEdit3("ambient", (float*)&(*m_spotLight)->ambient);
		ImGui::ColorEdit3("diffuse", (float*)&(*m_spotLight)->diffuse);
		ImGui::ColorEdit3("specular", (float*)&(*m_spotLight)->specular);
		ImGui::SliderFloat("cutoff", &(*m_spotLight)->cutOff, cosf(80.0f * (PI / 180.0f)), 1.0f);
		ImGui::End();
		ImGui::Begin("SpotLight[1]");
		ImGui::ColorEdit3("ambient", (float*)&(*m_testSpotLight)->ambient);
		ImGui::ColorEdit3("diffuse", (float*)&(*m_testSpotLight)->diffuse);
		ImGui::ColorEdit3("specular", (float*)&(*m_testSpotLight)->specular);
		ImGui::SliderFloat("cutoff", &(*m_testSpotLight)->cutOff, cosf(80.0f * (PI / 180.0f)), 1.0f);
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
		float vFov = std::atanf(aspectRatioInverse * std::tanf(0.5f * m_hFov * (3.14159265f / 180.0f))) * 2.0f * (180.0f / PI);
		m_cam.set(m_zNear, m_zFar, m_hFov, vFov);
	}

	void temporarySetup()
	{
		/*m_vertexArray = Engine::GLvertexArray::create();

		float pVertexBuffer[5 * 4] = {
			-50.0f, -50.0f, -100.0f, 0.0f, 0.0f,
			50.0f, -50.0f, -100.0f, 1.0f, 0.0f,
			-50.0f, 50.0f, -100.0f, 0.0f, 1.0f,
			50.0f, 50.0f, -100.0f, 1.0f, 1.0f
		};
		Engine::Ref_ptr<Engine::vertexBuffer> vertexBuffer = Engine::vertexBuffer::create(sizeof(pVertexBuffer), pVertexBuffer, STATIC_DRAW);
		unsigned int pIndexBuffer[6] = {
			0, 1, 2, 1, 3, 2
		};
		Engine::Ref_ptr<Engine::indexBuffer> indexBuffer = Engine::indexBuffer::create(6, pIndexBuffer, STATIC_DRAW);

		Engine::Ref_ptr<Engine::vertexBufferLayout> VertexBufferLayout = Engine::vertexBufferLayout::create();
		VertexBufferLayout->push(Engine::ShaderDataType::vec3);//could also use "vertexBufferLayout.pushFloat(3);" to achieve the same thing
		VertexBufferLayout->push(Engine::ShaderDataType::vec2);//for tex-coords
		vertexBuffer->setLayout(VertexBufferLayout);
		m_vertexArray->addBuffer(vertexBuffer);//tie the buffer and it's layout to the vertexArray
		m_vertexArray->addBuffer(indexBuffer);//tie the indexBuffer to the vertexArray
		*/

		m_vertexArray = Engine::vertexArray::create();
		m_vertexArray->load("gun.model");
		m_vertexArray->unbind();//make sure to unbind the vertexArray before loading the next one, otherwise we would get interfearences with layout & indexBuffer
		m_cubeGeometry = Engine::vertexArray::create();
		m_cubeGeometry->load("cube.model");
		m_cubeGeometry->unbind();

		Engine::Ref_ptr<Engine::shader> lightTexShader = Engine::Renderer::getShaderLib()->load("basicPhong_one_texture.shader");
		lightTexShader->bindUniformBlock("ViewProjection", 0);
		lightTexShader->bindUniformBlock("directionalLights", 1);
		lightTexShader->bindUniformBlock("pointLights", 2);
		lightTexShader->bindUniformBlock("spotLights", 3);

		Engine::Ref_ptr<Engine::shader> lightShader = Engine::Renderer::getShaderLib()->load("basicPhong_color.shader");
		lightShader->bindUniformBlock("ViewProjection", 0);
		lightShader->bindUniformBlock("directionalLights", 1);
		lightShader->bindUniformBlock("pointLights", 2);
		lightShader->bindUniformBlock("spotLights", 3);

		m_texture = Engine::texture2d::create("Cerberus_A.tga");

		{
			Engine::Ref_ptr<Engine::material> lightMat = Engine::material::create(lightShader, "light");
			lightMat->setUniform4f("amb", 0.2f, 0.025f, 0.025f, 1.0f);
			lightMat->setUniform4f("diff", 0.7f, 0.0875f, 0.0875f, 1.0f);
			lightMat->setUniform4f("spec", 0.8f, 0.1f, 0.1f, 1.0f);
			lightMat->setUniform1f("shininess", 100.0f);
			lightMat->flushAll();
			m_matLib.add(lightMat);
		}
		{
			Engine::Ref_ptr<Engine::material> texLightMat = Engine::material::create(lightTexShader, "texturedLight");
			texLightMat->setTexture("u_texture", m_texture);
			m_matLib.add(texLightMat);
		}

		m_mesh = std::make_shared<Engine::mesh>(m_vertexArray, m_matLib.get("texturedLight"));
		m_mesh->setPos({ 0.0f, 0.0f, -10.0f });
		m_mesh->setRot({0.0f, 3.1415926f, 0.0f });//rotate 180 degrees(pi in radians)

		m_cube = std::make_shared<Engine::mesh>(m_cubeGeometry, m_matLib.get("light"));
		m_cube->setScale(0.1f);
		m_cube->setPos({ -5.0f, 0.0f, -10.0f });

		m_sun = Engine::Renderer::addStaticDirLight({ vec3(0.0f, -0.7071067f, 0.7071067f), vec3(1.0f, 1.0f, 1.0f), vec3(1.0f, 1.0f, 1.0f), vec3(1.0f, 1.0f, 1.0f) });
		m_lamp = Engine::Renderer::addDynamicPointLight({ vec3(-2.5f, 5.0f, -10.0f), vec3(0.2f, 0.8f, 0.2f), vec3(0.2f, 0.8f, 0.2f), vec3(0.1f, 0.9f, 0.1f), 1.0f, 0.01f, 0.01f });
		m_testLamp = Engine::Renderer::addDynamicPointLight({ vec3(-2.5f, 5.0f, -10.0f), vec3(0.2f, 0.2f, 0.8f), vec3(0.2f, 0.2f, 0.8f), vec3(0.1f, 0.1f, 0.9f), 1.0f, 0.01f, 0.01f });
		m_spotLight = Engine::Renderer::addDynamicSpotLight({ vec3(-2.5f, 0.0f, -10.0f), vec3(-1.0f, 0.0f, 0.0f), vec3(1.0f, 1.0f, 1.0f), vec3(1.0f, 1.0f, 1.0f), vec3(1.0f, 1.0f, 1.0f), cosf(15.0f * (PI / 180.0f)) });
		m_testSpotLight = Engine::Renderer::addDynamicSpotLight({ vec3(-2.5f, 0.0f, -10.0f), vec3(1.0f, 0.0f, 0.0f), vec3(1.0f, 1.0f, 1.0f), vec3(1.0f, 1.0f, 1.0f), vec3(1.0f, 1.0f, 1.0f), cosf(15.0f * (PI / 180.0f)) });
	}
private:
	//temporary stuff
	Engine::materialLib m_matLib;
	Engine::Ref_ptr<Engine::mesh> m_mesh;
	Engine::Ref_ptr<Engine::mesh> m_cube;
	Engine::Ref_ptr<Engine::vertexArray> m_vertexArray;
	Engine::Ref_ptr<Engine::vertexArray> m_cubeGeometry;
	Engine::Ref_ptr<Engine::texture2d> m_texture;
	Engine::directionalLight** m_sun;
	Engine::pointLight** m_lamp;
	Engine::pointLight** m_testLamp;
	Engine::spotLight** m_spotLight;
	Engine::spotLight** m_testSpotLight;
	//end temporary stuff

	Engine::perspectiveCamera m_cam;
	vec3 m_camPos;
	float m_camSpeed = 12.0f;
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