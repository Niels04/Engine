#include <Engine.hpp>
#include "imgui/imgui.h"

#define PI 3.1415926f

class renderLayer : public Engine::layer
{
public:
	renderLayer(const float hFov, const float zNear, const float zFar)
		: layer("RenderLayer"), m_cam(zNear, zFar, hFov, static_cast<float>(Engine::Application::Get().getWindow().getWidth() / static_cast<float>(Engine::Application::Get().getWindow().getHeight())))
	{
		temporarySetup();
	}
	~renderLayer()
	{
	}

	void onUpdate(Engine::timestep ts) override
	{
		//update
		m_cam.onUpdate(ts);

		//render
		Engine::renderCommand::setClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
		Engine::renderCommand::clear();

		Engine::Renderer::beginScene(m_cam.getCam());
		{
			Engine::Renderer::sub(m_gun);
			Engine::Renderer::sub(m_plane);
			Engine::Renderer::sub(m_helmet);
			Engine::Renderer::sub(m_cube);
		}
		Engine::Renderer::endScene();
		//first render to the depth map(this is all temporary and will be worked on)
		Engine::Renderer::RenderDepthMaps();
		//then render the scene with the shadows
		Engine::Renderer::Flush();
	}

	void onImGuiRender() override
	{
		const vec4& camPos = m_cam.getPos();
		const vec3& camRot = m_cam.getRot();
		ImGui::Begin("Camera");
		ImGui::Text("position:");
		ImGui::Text("x: %.1f  y: %.1f  z: %.1f", camPos.x, camPos.y, camPos.z);
		ImGui::Text("rotation:");
		ImGui::Text("x: %.1f  y: %.1f  z: %.1f", camRot.x, camRot.y, camRot.z);
		ImGui::End();
		ImGui::Begin("PointLight[0]");
		ImGui::ColorEdit3("ambient", (float*)&m_lamp->ambient);
		ImGui::ColorEdit3("diffuse", (float*)&m_lamp->diffuse);
		ImGui::ColorEdit3("specular", (float*)&m_lamp->specular);
		ImGui::SliderFloat3("position", (float*)&m_lamp->position, -10.0f, 10.0f);
		ImGui::End();
		/*ImGui::Begin("PointLight[1]");
		ImGui::ColorEdit3("ambient", (float*)&m_testLamp->ambient);
		ImGui::ColorEdit3("diffuse", (float*)&m_testLamp->diffuse);
		ImGui::ColorEdit3("specular", (float*)&m_testLamp->specular);
		ImGui::InputFloat3("position", (float*)&m_testLamp->position, 3);
		ImGui::End();
		ImGui::Begin("SpotLight[0]");
		ImGui::ColorEdit3("ambient", (float*)&m_spotLight->ambient);
		ImGui::ColorEdit3("diffuse", (float*)&m_spotLight->diffuse);
		ImGui::ColorEdit3("specular", (float*)&m_spotLight->specular);
		ImGui::SliderFloat("cutoff", &m_spotLight->cutOff, cosf(80.0f * (PI / 180.0f)), 1.0f);
		ImGui::End();*/
		ImGui::Begin("SpotLight[1]");
		ImGui::ColorEdit3("ambient", (float*)&m_testSpotLight->ambient);
		ImGui::ColorEdit3("diffuse", (float*)&m_testSpotLight->diffuse);
		ImGui::ColorEdit3("specular", (float*)&m_testSpotLight->specular);
		ImGui::SliderFloat("cutoff", &m_testSpotLight->cutOff, cosf(80.0f * (PI / 180.0f)), 1.0f);
		ImGui::SliderFloat3("position", (float*)&m_testSpotLight->position, -50.0f, 50.0f);
		ImGui::End();
	}

	void onEvent(Engine::Event& e) override
	{
		m_cam.onEvent(e);
		Engine::eventDispatcher dispatcher(e);
		dispatcher.dispatchEvent<Engine::windowResizeEvent>(ENG_BIND_EVENT_FN(renderLayer::onWindowResizeEvent));
	}
	//onEvent-functions
	bool onWindowResizeEvent(Engine::windowResizeEvent& e)
	{
		Engine::Renderer::setMainViewPort(e.getWidth(), e.getHeight());
		Engine::renderCommand::setViewport(e.getWidth(), e.getHeight());
		return false;
	}

	void temporarySetup()
	{
		Engine::Ref_ptr<Engine::vertexArray> gun = Engine::vertexArray::create();
		gun->load("gun.model");
		gun->unbind();//make sure to unbind the vertexArray before loading the next one, otherwise we would get interfearences with layout & indexBuffer
		Engine::Ref_ptr<Engine::vertexArray> plane = Engine::vertexArray::create();
		plane->load("plane.model");
		plane->unbind();
		Engine::Ref_ptr<Engine::vertexArray> helmet = Engine::vertexArray::create();
		helmet->load("helmet.model");
		helmet->unbind();
		Engine::Ref_ptr<Engine::vertexArray> cube = Engine::vertexArray::create();
		cube->load("cube.model");
		cube->unbind();

		Engine::Ref_ptr<Engine::shader> lightTexShader_dir = Engine::Renderer::getShaderLib()->load("additive_w_shadow/dir/basicPhong_one_texture_shadow_additive_dir.shader");
		lightTexShader_dir->bindUniformBlock("ViewProjection", VIEWPROJ_BIND);
		lightTexShader_dir->bindUniformBlock("directionalLights", DIRECTIONAL_LIGHTS_BIND);
		lightTexShader_dir->bindUniformBlock("directionalLights_v", DIRECTIONAL_LIGHTS_BIND);

		Engine::Ref_ptr<Engine::shader> lightTexShader_point = Engine::Renderer::getShaderLib()->load("additive_w_shadow/point/basicPhong_one_texture_shadow_additive_point.shader");
		lightTexShader_point->bindUniformBlock("ViewProjection", VIEWPROJ_BIND);
		lightTexShader_point->bindUniformBlock("pointLights", POINT_LIGHTS_BIND);

		Engine::Ref_ptr<Engine::shader> lightTexShader_spot = Engine::Renderer::getShaderLib()->load("additive_w_shadow/spot/basicPhong_one_texture_shadow_additive_spot.shader");
		lightTexShader_spot->bindUniformBlock("ViewProjection", VIEWPROJ_BIND);
		lightTexShader_spot->bindUniformBlock("spotLights", SPOT_LIGHTS_BIND);
		lightTexShader_spot->bindUniformBlock("spotLights_v", SPOT_LIGHTS_BIND);

		Engine::Ref_ptr<Engine::shader> lightShader_dir = Engine::Renderer::getShaderLib()->load("additive_w_shadow/dir/basicPhong_color_shadow_additive_dir.shader");
		lightShader_dir->bindUniformBlock("ViewProjection", VIEWPROJ_BIND);
		lightShader_dir->bindUniformBlock("directionalLights", DIRECTIONAL_LIGHTS_BIND);
		lightShader_dir->bindUniformBlock("directionalLights_v", DIRECTIONAL_LIGHTS_BIND);

		Engine::Ref_ptr<Engine::shader> lightShader_point = Engine::Renderer::getShaderLib()->load("additive_w_shadow/point/basicPhong_color_shadow_additive_point.shader");
		lightShader_point->bindUniformBlock("ViewProjection", VIEWPROJ_BIND);
		lightShader_point->bindUniformBlock("pointLights", POINT_LIGHTS_BIND);

		Engine::Ref_ptr<Engine::shader> lightShader_spot = Engine::Renderer::getShaderLib()->load("additive_w_shadow/spot/basicPhong_color_shadow_additive_spot.shader");
		lightShader_spot->bindUniformBlock("ViewProjection", VIEWPROJ_BIND);
		lightShader_spot->bindUniformBlock("spotLights", SPOT_LIGHTS_BIND);
		lightShader_spot->bindUniformBlock("spotLights_v", SPOT_LIGHTS_BIND);

		m_gunTex = Engine::texture2d::create("Cerberus_A.tga");
		
		m_helmetTex = Engine::texture2d::create("armor_115_head_alb.png");
		//m_cubeTex = Engine::texture2d::create("chiseled_stone_bricks.png", FILTER_NEAREST);<-work on rgba-textures
		m_cubeTex = Engine::texture2d::create("checkerboard.png");

		{
			Engine::Ref_ptr<Engine::material> red = Engine::material::create(lightShader_dir, "red");//just use the variant for directional lights for initialization
			red->addShader(lightShader_point);
			red->addShader(lightShader_spot);
			red->setUniform4f("amb", 0.2f, 0.025f, 0.025f, 1.0f);
			red->setUniform4f("diff", 0.7f, 0.0875f, 0.0875f, 1.0f);
			red->setUniform4f("spec", 0.8f, 0.1f, 0.1f, 1.0f);
			red->setUniform1f("shininess", 100.0f);
			red->flushAll();
			m_matLib.add(red);
		}
		{
			Engine::Ref_ptr<Engine::material> gunMat = Engine::material::create(lightTexShader_dir, "gunMat");//just use the variant for directional lights for initialization
			gunMat->addShader(lightTexShader_point);
			gunMat->addShader(lightTexShader_spot);
			gunMat->setTexture("u_texture", m_gunTex);
			m_matLib.add(gunMat);
		}
		{
			Engine::Ref_ptr<Engine::material> helmetMat = Engine::material::create(lightTexShader_dir, "helmetMat");
			helmetMat->addShader(lightTexShader_point);
			helmetMat->addShader(lightTexShader_spot);
			helmetMat->setTexture("u_texture", m_helmetTex);
			m_matLib.add(helmetMat);
		}
		{
			Engine::Ref_ptr<Engine::material> cubeMat = Engine::material::create(lightTexShader_dir, "cubeMat");
			cubeMat->addShader(lightTexShader_point);
			cubeMat->addShader(lightTexShader_spot);
			cubeMat->setTexture("u_texture", m_cubeTex);
			m_matLib.add(cubeMat);
		}

		m_gun = std::make_shared<Engine::mesh>(gun, m_matLib.get("gunMat"));
		m_gun->setPos({ 0.0f, 3.0f, 0.0f });
		m_gun->setRot({0.0f, 3.1415926f, 0.0f });//rotate 180 degrees(pi in radians)

		m_plane = std::make_shared<Engine::mesh>(plane, m_matLib.get("red"));
		m_plane->setScale(10.0f);
		m_plane->setPos({ 0.0f, 0.0f, 0.0f });

		m_helmet = std::make_shared<Engine::mesh>(helmet, m_matLib.get("helmetMat"));
		m_helmet->setPos({ -6.0f, 3.0f, 5.0f });
		m_helmet->setScale(5.0f);

		m_cube = std::make_shared<Engine::mesh>(cube, m_matLib.get("cubeMat"));
		m_cube->setScale(0.1f);
		m_cube->setPos({0.0f, 1.0f, 8.0f});

		m_sun = Engine::Renderer::addStaticDirLight({ vec3(0.0f, -0.7071067f, 0.7071067f), vec3(0.5f, 0.5f, 0.5f), vec3(0.5f, 0.5f, 0.5f), vec3(0.5f, 0.5f, 0.5f) } );
		//m_moon = Engine::Renderer::addStaticDirLight({ vec3(0.0f, -0.7071067f, -0.7071067f), vec3(1.0f, 1.0f, 1.0f), vec3(1.0f, 1.0f, 1.0f), vec3(1.0f, 1.0f, 1.0f) } ); <-doesn't work because of the lookAt-function edge-case
		//m_sun_2 = Engine::Renderer::addStaticDirLight({ vec3(-0.5773502f, -0.5773502, -0.5773502f), vec3(1.0f, 1.0f, 1.0f), vec3(1.0f, 1.0f, 1.0f), vec3(1.0f, 1.0f, 1.0f) });
		m_lamp = Engine::Renderer::addDynamicPointLight({ vec3(0.0f, 1.0f, 0.0f), vec3(0.5f, 0.1f, 0.1f), vec3(0.5f, 0.1f, 0.1f), vec3(0.5f, 0.1f, 0.1f), 1.0f, 0.01f, 0.01f });
		m_testSpotLight = Engine::Renderer::addDynamicSpotLight({ vec3(-6.0f, 8.0f, 4.75f), vec3(0.0f, -1.0f, 0.0f), vec3(0.1f, 0.1f, 1.0f), vec3(0.1f, 0.1f, 1.0f), vec3(0.1f, 0.1f, 1.0f), cosf(15.0f * (PI / 180.0f)) });
	}

private:
	Engine::materialLib m_matLib;
	Engine::Ref_ptr<Engine::mesh> m_gun;
	Engine::Ref_ptr<Engine::mesh> m_plane;
	Engine::Ref_ptr<Engine::mesh> m_helmet;
	Engine::Ref_ptr<Engine::mesh> m_cube;
	Engine::Ref_ptr<Engine::texture2d> m_gunTex;
	Engine::Ref_ptr<Engine::texture2d> m_helmetTex;
	Engine::Ref_ptr<Engine::texture2d> m_cubeTex;
	Engine::PtrPtr<Engine::directionalLight> m_sun;
	Engine::PtrPtr<Engine::pointLight> m_lamp;
	Engine::PtrPtr<Engine::spotLight> m_testSpotLight;

	Engine::PerspectiveCameraController m_cam;
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