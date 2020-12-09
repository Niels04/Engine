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
			/*auto& /*don't copy to avoid unnessecary refcount incrementing*//*quadMat = m_matLib.get("Red_flat");
			quadMat->bind(1);//always bind materials to slot 1 for now because slot 0 is used for projectionMat and viewMat
			quadMat->setUniform4fF("u_color", m_quad_color);//when only updating a single uniform use setUniform<type>F() to instantly flush it-> real materials will probably not use this often because they won't be dynamic
			Engine::Renderer::sub(m_vertexArray, Engine::Renderer::getShaderLib()->get("flatColor"), mat4::transMat(0.0f, 150.0f, 0.0f));*/
			Engine::Renderer::sub(m_mesh);
			//Engine::Renderer::sub(m_fungus);
		}
		Engine::Renderer::endScene();

		Engine::Renderer::Flush();
	}

	void onImGuiRender() override
	{

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
		m_vertexArray->load("skull.model");

		Engine::Ref_ptr<Engine::shader> flatColorShader = Engine::Renderer::getShaderLib()->load("flatColor.shader");
		flatColorShader->bindUniformBlock("ViewProjection", 0);//bind the UniformBlock "ViewProjection" to the default bindingPoint for viewProjectionMatrices, which is 0 //do this for each shader, that uses the view and the projection matrix
		Engine::Ref_ptr<Engine::shader> textureShader = Engine::Renderer::getShaderLib()->load("texture.shader");
		textureShader->bindUniformBlock("ViewProjection", 0);

		m_texture = Engine::texture2d::create("Skull.png");

		{
			Engine::Ref_ptr<Engine::material> redMat = Engine::material::create(flatColorShader, "Red_flat");
			redMat->setUniform4f("u_color", 0.8f, 0.1f, 0.1f, 1.0f);//if a material is non-dynamic(which a material is probably gonna be in most cases) we just set it's uniforms when loading it
			redMat->flushAll();//set all the uniforms on CPU-side first and then flush them over in one gl-call when creating a material
			m_matLib.add(redMat);
		}
		{
			Engine::Ref_ptr<Engine::material> skullMat = Engine::material::create(textureShader, "SkullMat");
			skullMat->setTexture("u_texture", m_texture);
			m_matLib.add(skullMat);
		}

		m_mesh = std::make_shared<Engine::mesh>(m_vertexArray, m_matLib.get("SkullMat"));
		m_mesh->setPos({ 0.0f, 0.0f, -150.0f });
		m_mesh->setRot({ 0.0f, 3.1415926f, 0.0f });//rotate 180 degrees(pi in radians)

		//unbind everything
		m_vertexArray->unbind();
		//indexBuffer->unbind();
		//vertexBuffer->unbind();
		flatColorShader->unbind();
		textureShader->unbind();
	}
private:
	//temporary stuff
	Engine::materialLib m_matLib;
	//Engine::Ref_ptr<Engine::mesh> m_fungus;
	Engine::Ref_ptr<Engine::mesh> m_mesh;
	Engine::Ref_ptr<Engine::vertexArray> m_vertexArray;
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