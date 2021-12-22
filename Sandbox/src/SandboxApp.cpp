#include <Engine.hpp>
#include "imgui/imgui.h"

#define PI 3.1415926f

class SandboxLayer : public Engine::layer
{
	friend class Sandbox;
public:
	SandboxLayer()
		: layer("RenderLayer")
	{
		m_scene.initialize(90.0f, 0.1f, 100.0f, static_cast<float>(Engine::Application::Get().getWindow().getWidth() / static_cast<float>(Engine::Application::Get().getWindow().getHeight())));
		Engine::Application::Get().getWindow().setDissableCursor(false);
		Engine::Application::Get().getWindow().setFullscreen(false);
	}
	~SandboxLayer()
	{
		m_scene.clear();
	}

	void onUpdate(Engine::timestep ts) override
	{
		m_scene.onUpdate(ts, Engine::Application::Get().getWindow().isDissableCursor());
	}

	void onRender() override
	{
		m_scene.onRender();
	}

	void onImGuiRender() override
	{
		if (m_sceneLoaded)
			m_scene.onImGuiRender();
		else
			showSceneSelect();
	}

	void onEvent(Engine::Event& e) override
	{
		m_scene.onEvent(e);
		Engine::eventDispatcher dispatcher(e);
		dispatcher.dispatchEvent<Engine::windowResizeEvent>(ENG_BIND_EVENT_FN(SandboxLayer::onWindowResizeEvent));
		dispatcher.dispatchEvent<Engine::keyPressedEvent>(ENG_BIND_EVENT_FN(SandboxLayer::onKeyPressedEvent));
	}
	//onEvent-functions
	bool onWindowResizeEvent(Engine::windowResizeEvent& e)
	{
		Engine::Renderer::setMainViewPort(e.getWidth(), e.getHeight());
		Engine::renderCommand::setViewport(e.getWidth(), e.getHeight());
		return false;
	}

	bool onKeyPressedEvent(Engine::keyPressedEvent& e)
	{
		int keycode = e.getKeyCode();
		switch (keycode)
		{
		case(ENG_KEY_LEFT_ALT):
		{
			Engine::Application::Get().getWindow().setDissableCursor(!Engine::Application::Get().getWindow().isDissableCursor());
		}break;
		case(ENG_KEY_F11):
		{
			Engine::Application::Get().getWindow().setFullscreen(!Engine::Application::Get().getWindow().isFullscreen());
		}break;
		case(ENG_KEY_ESCAPE):
		{
			m_scene.clear();
			m_scene.initialize(90.0f, 0.1f, 100.0f, static_cast<float>(Engine::Application::Get().getWindow().getWidth() / static_cast<float>(Engine::Application::Get().getWindow().getHeight())));
			Engine::Renderer::getShaderLib()->clear();
			Engine::Application::Get().getWindow().setDissableCursor(false);
			m_sceneLoaded = false;
		}break;
		}
		return false;
	}

	void showSceneSelect()
	{
		ImGui::Begin("Select a scene.");
		if (ImGui::Button("ECS_Test"))
		{
			ECS_Test();
			m_sceneLoaded = true;
		}
		ImGui::End();
	}
	//scenes
	void ECS_Test()
	{
		Engine::Ref_ptr<Engine::vertexArray> gun_va = Engine::vertexArray::create();
		gun_va->load("gun_tb.model");
		gun_va->unbind();
		Engine::Ref_ptr<Engine::vertexArray> plane_va = Engine::vertexArray::create();
		plane_va->load("plane.model");
		plane_va->unbind();
		Engine::Ref_ptr<Engine::vertexArray> text_va = Engine::vertexArray::create();
		text_va->load("test_text.model");
		text_va->unbind();
		Engine::Ref_ptr<Engine::vertexArray> lightsaber_va = Engine::vertexArray::create();
		lightsaber_va->load("lightsaber.model");
		lightsaber_va->unbind();

		Engine::Ref_ptr<Engine::texture2d> gunDiffTex = Engine::texture2d::create("Cerberus_A.tga", true);
		Engine::Ref_ptr<Engine::texture2d> gunNormalTex = Engine::texture2d::create("Cerberus_N.tga");//USE LINEAR INTERPOLATION FOR NORMALMAPS AND NOT SOMETHING LIKE "FILTER_NEAREST" -> mutch smoother
		Engine::Ref_ptr<Engine::texture2d> gunSpecTex = Engine::texture2d::create("Cerberus_M.tga");

		Engine::Ref_ptr<Engine::shader> shader_alb_nrm_spec = Engine::Renderer::getShaderLib()->load("deferred/geometry_pass/alb_map_nrm_map_spc_map.shader");
		shader_alb_nrm_spec->bindUniformBlock("ViewProjection", VIEWPROJ_BIND);
		Engine::Ref_ptr<Engine::shader> shader_alb = Engine::Renderer::getShaderLib()->load("deferred/geometry_pass/color.shader");
		shader_alb->bindUniformBlock("ViewProjection", VIEWPROJ_BIND);

		Engine::Ref_ptr<Engine::material> gunMat = Engine::material::create(shader_alb_nrm_spec, "gun_mat");
		gunMat->setTexture("u_texture", gunDiffTex);
		gunMat->setTexture("u_normalMap", gunNormalTex);
		gunMat->setTexture("u_specMap", gunSpecTex);
		gunMat->setUniform1f("ambCoefficient", 0.1f);
		gunMat->setUniform1f("shininess", 100.0f);
		gunMat->setUniform1f("emissiveMultiplier", 0.0f);
		gunMat->setUniform1f("reflectiveMultiplier", 0.1f);
		gunMat->flushAll();
		m_scene.addMaterial(gunMat);

		Engine::Ref_ptr<Engine::material> planeMat = Engine::material::create(shader_alb, "plane_mat", flags_default | flag_no_backface_cull);
		planeMat->setUniform4f("alb", { 0.3f, 0.3f, 0.3f, 1.0f });
		planeMat->setUniform1f("ambCoefficient", 0.1f);
		planeMat->setUniform1f("specCoefficient", 1.0f);
		planeMat->setUniform1f("shininess", 50.0f);
		planeMat->setUniform1f("emissiveMultiplier", 0.0f);
		planeMat->setUniform1f("reflectiveMultiplier", 0.1f);
		planeMat->flushAll();
		m_scene.addMaterial(planeMat);

		Engine::Ref_ptr<Engine::material> textMat = Engine::material::create(shader_alb, "neonText");
		textMat->setUniform4f("alb", 1.0f, 1.0f, 0.0195f, 1.0f);
		textMat->setUniform1f("ambCoefficient", 0.0f);
		textMat->setUniform1f("specCoefficient", 0.0f);
		textMat->setUniform1f("shininess", 1.0f);
		textMat->setUniform1f("emissiveMultiplier", 100.0f);
		textMat->setUniform1f("reflectiveMultiplier", 0.0f);
		textMat->flushAll();
		textMat->dissableFlags(flag_cast_shadow);
		m_scene.addMaterial(textMat);

		Engine::Ref_ptr<Engine::material> lightsaber_mat = Engine::material::create(shader_alb, "lightsaber_mat");
		lightsaber_mat->setUniform4f("alb", 0.0781f, 1.0f, 0.0195f, 1.0f);
		lightsaber_mat->setUniform1f("ambCoefficient", 0.0f);
		lightsaber_mat->setUniform1f("specCoefficient", 0.0f);
		lightsaber_mat->setUniform1f("shininess", 1.0f);
		lightsaber_mat->setUniform1f("emissiveMultiplier", 100.0);
		lightsaber_mat->setUniform1f("reflectiveMultiplier", 0.0f);
		lightsaber_mat->flushAll();
		lightsaber_mat->dissableFlags(flag_cast_shadow);
		m_scene.addMaterial(lightsaber_mat);

		Entity gun = m_scene.createEntity();
		m_scene.Reg().addComponent<Engine::MeshComponent>(gun, gun_va, gunMat, std::string("gun"));
		m_scene.Reg().addComponent<Engine::TransformComponent>(gun);
		Entity plane = m_scene.createEntity();
		m_scene.Reg().addComponent<Engine::MeshComponent>(plane, plane_va, planeMat, std::string("plane"));
		m_scene.Reg().addComponent<Engine::TransformComponent>(plane);
		m_scene.Reg().get<Engine::TransformComponent>(plane).setPos({ 0.0f, -5.0f, 0.0f });
		Entity text = m_scene.createEntity();
		m_scene.Reg().addComponent<Engine::MeshComponent>(text, text_va, textMat, "text");
		m_scene.Reg().addComponent<Engine::TransformComponent>(text);
		auto& textTransform = m_scene.Reg().get<Engine::TransformComponent>(text);
		textTransform.setScale(2.0f);
		textTransform.setRot({ 3.1415926f / 2.0f, 0.0f, 0.0f });
		textTransform.setPos({ 0.0f, 6.0f, -5.5f });
		Entity lightsaber = m_scene.createEntity();
		m_scene.Reg().addComponent<Engine::MeshComponent>(lightsaber, lightsaber_va, lightsaber_mat, "lightsaber");
		m_scene.Reg().addComponent<Engine::TransformComponent>(lightsaber);
		m_scene.Reg().get<Engine::TransformComponent>(lightsaber).setPos({ -2.0f, 5.0f, -5.5f });
		Entity neonGroup = m_scene.createEntity();
		m_scene.Reg().addComponent<Engine::TransformComponent>(neonGroup);
		m_scene.setParent(text, neonGroup);
		m_scene.setParent(lightsaber, neonGroup);

		Entity flashLight = m_scene.createEntity();
		m_scene.Reg().addComponent<Engine::SpotLightComponent>(flashLight, vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, -1.0f, 0.0f), vec3(0.1f, 0.1f, 0.1f), vec3(0.8f, 0.8f, 0.8f), vec3(1.0f, 1.0f, 1.0f), cosf(RAD(30.0f)));
		m_scene.Reg().addComponent<Engine::TransformComponent>(flashLight);
		m_scene.Reg().get<Engine::TransformComponent>(flashLight).setPos({ 0.0f, 4.5f, 0.0f });
		m_scene.Reg().get<Engine::TransformComponent>(flashLight).setRot({ -1.57f, 0.0f, 0.0f });
		Entity lamp = m_scene.createEntity();
		m_scene.Reg().addComponent<Engine::PointLightComponent>(lamp, vec3(0.0f, 2.0f, 0.0f), vec3(0.1f, 0.02f, 0.02f), vec3(0.5f, 0.1f, 0.1f), vec3(0.5f, 0.1f, 0.1f), 1.0f, 0.01f, 0.01f);
		m_scene.Reg().addComponent<Engine::TransformComponent>(lamp);
		m_scene.Reg().get<Engine::TransformComponent>(lamp).setPos({ 0.0f, 5.0f, 0.0f });
		m_scene.setParent(lamp, plane);
		m_scene.setParent(gun, lamp);
		Entity sun = m_scene.createEntity();
		m_scene.Reg().addComponent<Engine::DirLightComponent>(sun, vec3(0.0f, -1.0f, 0.0f), vec3(1.0f, 1.0f, 1.0f), vec3(1.0f, 1.0f, 1.0f), vec3(1.0f, 1.0f, 1.0f));
		m_scene.Reg().addComponent<Engine::TransformComponent>(sun);

		m_scene.setSkybox(Engine::texture3d_hdr::create("skyboxes/clouds_hdr/"));
	}

private:
	Engine::Scene m_scene;
	bool m_sceneLoaded = false;
};

class Sandbox : public Engine::Application
{
public:
	Sandbox()
	{
		SandboxLayer* sandboxLayer = new SandboxLayer();
		pushLayer(sandboxLayer);
	}
	~Sandbox()
	{
		
	}

};

Engine::Application* Engine::CreateApplication()
{
	return new Sandbox;
}