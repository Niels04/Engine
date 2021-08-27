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
		m_scene.clear(Engine::Renderer::getLightManager());
	}

	void onUpdate(Engine::timestep ts) override
	{
		//update
		m_scene.onUpdate(ts, Engine::Application::Get().getWindow().isDissableCursor());

		//render
		//Engine::renderCommand::setClearColor({ 0.0171f, 0.0171f, 0.0171f, 1.0f });
		Engine::renderCommand::setClearColor({ 0.0f, 0.0f, 0.0f, 1.0f });
		Engine::renderCommand::clear();

		Engine::Renderer::beginScene(m_scene.getCamera());
		{
			for (auto& it = m_scene.meshes_begin(); it != m_scene.meshes_end(); it++)
			{
				Engine::Renderer::sub(*it);
			}
		}
		Engine::Renderer::endScene();
		//first render to the depth maps
		Engine::Renderer::RenderDepthMaps();
		//then render the scene with the shadows applied
		Engine::Renderer::Flush();
	}

	void onImGuiRender() override
	{
		if (!m_showNodeEditor)
		{
			if (!m_sceneLoaded)
				showSceneSelect();
			else
			{
				m_scene.onImGuiRender();
				Engine::Renderer::onImGuiRender();//this should probably not be called by Sandbox
			}
		}
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
			if (Engine::Application::Get().getWindow().isDissableCursor())
			{
				Engine::Application::Get().getWindow().setDissableCursor(false);
			}
			else
			{
				Engine::Application::Get().getWindow().setDissableCursor(true);
			}
		}break;
		case(ENG_KEY_F11):
		{
			if (Engine::Application::Get().getWindow().isFullscreen())
				Engine::Application::Get().getWindow().setFullscreen(false);
			else
				Engine::Application::Get().getWindow().setFullscreen(true);
		}break;
		case(ENG_KEY_ESCAPE):
		{
			m_scene.clear(Engine::Renderer::getLightManager());
			m_scene.initialize(90.0f, 0.1f, 100.0f, static_cast<float>(Engine::Application::Get().getWindow().getWidth() / static_cast<float>(Engine::Application::Get().getWindow().getHeight())));
			Engine::Renderer::getShaderLib()->clear();
			Engine::Application::Get().getWindow().setDissableCursor(false);
			m_sceneLoaded = false;
		}break;
		case(ENG_KEY_N):
		{
			m_showNodeEditor = !m_showNodeEditor;
		}break;
		}
		return false;
	}

	void showSceneSelect()
	{
		ImGui::Begin("Select a scene.");
		if (ImGui::Button("plane_head_rotate"))
		{
			plane_head_rotate();
			m_sceneLoaded = true;
		}
		else if (ImGui::Button("boat_water"))
		{
			boat_water();
			m_sceneLoaded = true;
		}
		else if (ImGui::Button("experiment"))
		{
			experiment();
			m_sceneLoaded = true;
		}
		else if (ImGui::Button("normalMapping"))
		{
			normalMapping();
			m_sceneLoaded = true;
		}
		else if (ImGui::Button("bridge"))
		{
			bridge();
			m_sceneLoaded = true;
		}
		else if (ImGui::Button("norm_map_spc_map"))
		{
			nrm_map_spc_map();
			m_sceneLoaded = true;
		}
		else if (ImGui::Button("emissive"))
		{
			emissive();
			m_sceneLoaded = true;
		}
		else if (ImGui::Button("deferred_test"))
		{
			deferred_test();
			m_sceneLoaded = true;
		}
		else if (ImGui::Button("hylia_bridge"))
		{
			hylia_bridge();
			m_sceneLoaded = true;
		}
		else if (ImGui::Button("bloom_demo"))
		{
			bloom_demo();
			m_sceneLoaded = true;
		}
		ImGui::End();
	}
	//scenes
	void plane_head_rotate()
	{
		/*Engine::Ref_ptr<Engine::vertexArray> head_va = Engine::vertexArray::create();
		head_va->load("head.model");
		head_va->unbind();//make sure to unbind the vertexArray before loading the next one, otherwise we would get interfearences with layout & indexBuffer(I assume)*/
		Engine::Ref_ptr<Engine::vertexArray> plane_va = Engine::vertexArray::create();
		plane_va->load("plane.model");
		plane_va->unbind();
		Engine::Ref_ptr<Engine::vertexArray> helmet_va = Engine::vertexArray::create();
		helmet_va->load("helmet_tb.model");
		helmet_va->unbind();
		Engine::Ref_ptr<Engine::vertexArray> cube_va = Engine::vertexArray::create();
		cube_va->load("cube.model");
		cube_va->unbind();

		Engine::Ref_ptr<Engine::shader> alb_map_shader = Engine::Renderer::getShaderLib()->load("deferred/geometry_pass/alb_map.shader");
		alb_map_shader->bindUniformBlock("ViewProjection", VIEWPROJ_BIND);

		Engine::Ref_ptr<Engine::shader> color_shader = Engine::Renderer::getShaderLib()->load("deferred/geometry_pass/color.shader");
		color_shader->bindUniformBlock("ViewProjection", VIEWPROJ_BIND);

		Engine::Ref_ptr<Engine::shader> alb_nrm_map_shader = Engine::Renderer::getShaderLib()->load("deferred/geometry_pass/alb_map_nrm_map.shader");
		alb_nrm_map_shader->bindUniformBlock("ViewProjection", VIEWPROJ_BIND);

		//Engine::Ref_ptr<Engine::texture2d> headTex = Engine::texture2d::create("head.png", true);
		Engine::Ref_ptr<Engine::texture2d> helmetTex = Engine::texture2d::create("armor_115_head_alb.png", true);
		Engine::Ref_ptr<Engine::texture2d> helmetNormalTex = Engine::texture2d::create("armor_115_head_nrm.png");
		Engine::Ref_ptr<Engine::texture2d> cubeTex = Engine::texture2d::create("checkerboard.png", true);

		Engine::Ref_ptr<Engine::material> red = Engine::material::create(color_shader, "red", flags_default | flag_no_backface_cull);
		red->setUniform4f("alb", 0.7f, 0.0875f, 0.0875f, 1.0f);
		red->setUniform1f("ambCoefficient", 0.285f);
		red->setUniform1f("specCoefficient", 0.5f);
		red->setUniform1f("shininess", 100.0f);
		red->setUniform1f("emissiveMultiplier", 0.0f);
		red->flushAll();
		m_scene.addMaterial(red);

		/*Engine::Ref_ptr<Engine::material> headMat = Engine::material::create(alb_map_shader, "headMat");
		headMat->setTexture("u_texture", headTex);
		headMat->setUniform1f("ambCoefficient", 0.1f);
		headMat->setUniform1f("specCoefficient", 0.1f);
		headMat->setUniform1f("shininess", 10.0f);
		headMat->flushAll();
		m_scene.addMaterial(headMat);*/

		Engine::Ref_ptr<Engine::material> helmetMat = Engine::material::create(alb_nrm_map_shader, "helmetMat");
		helmetMat->setTexture("u_texture", helmetTex);
		helmetMat->setTexture("u_normalMap", helmetNormalTex);
		helmetMat->setUniform1f("ambCoefficient", 0.1f);
		helmetMat->setUniform1f("specCoefficient", 1.0f);
		helmetMat->setUniform1f("shininess", 100.0f);
		helmetMat->setUniform1f("emissiveMultiplier", 0.0f);
		helmetMat->flushAll();
		m_scene.addMaterial(helmetMat);

		Engine::Ref_ptr<Engine::material> cubeMat = Engine::material::create(alb_map_shader, "cubeMat");
		cubeMat->setTexture("u_texture", cubeTex);
		cubeMat->setUniform1f("ambCoefficient", 0.1f);
		cubeMat->setUniform1f("specCoefficient", 1.0f),
		cubeMat->setUniform1f("shininess", 80.0f);
		cubeMat->setUniform1f("emissiveMultiplier", 0.0f);
		cubeMat->flushAll();
		m_scene.addMaterial(cubeMat);

		/*Engine::Ref_ptr<Engine::mesh> head = Engine::mesh::create(head_va, headMat, "head");
		head->setPos({ 0.0f, 3.0f, -20.0f, 1.0f });
		head->setRot({ 0.0f, 3.1415926f, 0.0f });//rotate 180 degrees(pi in radians)
		head->setScale(0.1f);
		//head->attachMovement(std::static_pointer_cast<Engine::MeshMovement, Engine::CircularMeshMovement>(std::make_shared<Engine::CircularMeshMovement>(vec3(1.0f, 1.0f, 1.0f), vec3(0.5773502f, 0.5773502f, 0.5773502f), 0.0174533f * 60.0f)));
		m_scene.addMesh(head);*/

		Engine::Ref_ptr<Engine::mesh> plane = Engine::mesh::create(plane_va, red, "plane");
		plane->setScale(10.0f);
		plane->setPos({ 0.0f, 0.0f, -15.0f, 1.0f });
		plane->setRot({ 3.1415926f / 2.0f, 0.0f, 0.0f });
		m_scene.addMesh(plane);

		Engine::Ref_ptr<Engine::mesh> helmet = Engine::mesh::create(helmet_va, helmetMat, "helmet");
		helmet->setPos({ 0.0f, 0.0f, -10.0f, 1.0f });
		helmet->setScale(5.0f);
		m_scene.addMesh(helmet);

		Engine::Ref_ptr<Engine::mesh> cube = Engine::mesh::create(cube_va, cubeMat, "cube");
		cube->setScale(0.1f);
		cube->setPos({-60.0f, -30.0f, -10.0f, 1.0f});
		m_scene.addMesh(cube);

		//Engine::PtrPtr<Engine::directionalLight> sun(Engine::Renderer::addDynamicDirLight({ vec3(0.0f, -0.7071067f, 0.7071067f), vec3(0.1f, 0.1f, 0.1f), vec3(0.5f, 0.5f, 0.5f), vec3(0.5f, 0.5f, 0.5f) } ));
		//m_scene.addLight(sun);
		//Engine::Ref_ptr<Engine::DirLightMovement> rotate = Engine::DirLightMovement::create(0.0174533f * 60.0f * 0.25f, 0.0f, 0.0f);
		//rotate->attachToLight(sun);
		//m_scene.addLightMovement(rotate);
		Engine::PtrPtr<Engine::directionalLight> testSun(Engine::Renderer::addDynamicDirLight({ vec3(0.0f, 0.0f, -1.0f), vec3(0.1f, 0.1f, 0.1f), vec3(0.5f, 0.5f, 0.5f), vec3(0.5f, 0.5f, 0.5f) }));
		m_scene.addLight(testSun);
		
		//m_moon = Engine::Renderer::addStaticDirLight({ vec3(0.0f, -0.7071067f, -0.7071067f), vec3(1.0f, 1.0f, 1.0f), vec3(1.0f, 1.0f, 1.0f), vec3(1.0f, 1.0f, 1.0f) } ); <-doesn't work because of the lookAt-function edge-case
		//m_sun_2 = Engine::Renderer::addStaticDirLight({ vec3(-0.5773502f, -0.5773502, -0.5773502f), vec3(1.0f, 1.0f, 1.0f), vec3(1.0f, 1.0f, 1.0f), vec3(1.0f, 1.0f, 1.0f) });
		/*Engine::PtrPtr<Engine::pointLight> lamp = Engine::Renderer::addDynamicPointLight({ vec3(2.0f, 1.0f, 0.0f), vec3(0.1f, 0.02f, 0.02f), vec3(0.5f, 0.1f, 0.1f), vec3(0.5f, 0.1f, 0.1f), 1.0f, 0.01f, 0.01f });
		m_scene.addLight(lamp);
		Engine::Ref_ptr<Engine::PointLightMovement> move_circular = Engine::CircularPointLightMovement::create(vec3(0.0f, 1.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f), 0.0174533f * 60.0f);
		move_circular->attachToLight(lamp);
		m_scene.addLightMovement(move_circular);
		Engine::PtrPtr<Engine::spotLight> spotLight =  Engine::Renderer::addDynamicSpotLight({ vec3(-6.0f, 8.0f, 4.75f), vec3(0.0f, -1.0f, 0.0f), vec3(0.05f, 0.05f, 0.5f), vec3(0.1f, 0.1f, 1.0f), vec3(0.1f, 0.1f, 1.0f), cosf(15.0f * (PI / 180.0f)) });
		m_scene.addLight(spotLight);*/
	}

	void boat_water()
	{
		Engine::Ref_ptr<Engine::vertexArray> environment_va = Engine::vertexArray::create();
		environment_va->load("cube_solid.model");
		environment_va->unbind();
		Engine::Ref_ptr<Engine::vertexArray> figure_va = Engine::vertexArray::create();
		figure_va->load("figure.model");
		figure_va->unbind();
		Engine::Ref_ptr<Engine::vertexArray> boat_va = Engine::vertexArray::create();
		boat_va->load("boat.model");
		boat_va->unbind();
		Engine::Ref_ptr<Engine::vertexArray> water_va = Engine::vertexArray::create();
		water_va->load("water.model");
		water_va->unbind();
		Engine::Ref_ptr<Engine::vertexArray> lighthouse_va = Engine::vertexArray::create();
		lighthouse_va->load("lighthouse_textured.model");
		lighthouse_va->unbind();
		Engine::Ref_ptr<Engine::vertexArray> lightbulb_va = Engine::vertexArray::create();
		lightbulb_va->load("lightbulb.model");
		lightbulb_va->unbind();

		Engine::Ref_ptr<Engine::texture2d> duck_tex = Engine::texture2d::create("09-Default_albedo.jpg", true);
		Engine::Ref_ptr<Engine::texture2d> lighthouse_tex = Engine::texture2d::create("lighthouse_tex.png", true);
		Engine::Ref_ptr<Engine::texture2d> headTex = Engine::texture2d::create("figure_tex.png", true);

		Engine::Ref_ptr<Engine::shader> alb_map_shader = Engine::Renderer::getShaderLib()->load("deferred/geometry_pass/alb_map.shader");
		alb_map_shader->bindUniformBlock("ViewProjection", VIEWPROJ_BIND);

		Engine::Ref_ptr<Engine::shader> color_shader = Engine::Renderer::getShaderLib()->load("deferred/geometry_pass/color.shader");
		color_shader->bindUniformBlock("ViewProjection", VIEWPROJ_BIND);

		Engine::Ref_ptr<Engine::shader> emissive_bloom = Engine::Renderer::getShaderLib()->load("emissive/emissive_bloom.shader");
		emissive_bloom->bindUniformBlock("ViewProjection", VIEWPROJ_BIND);

		Engine::Ref_ptr<Engine::material> light_grey = Engine::material::create(color_shader, "light_grey");
		light_grey->setUniform4f("alb", 0.6f, 0.6f, 0.6f, 1.0f);
		light_grey->setUniform1f("ambCoefficient", 0.1666f);
		light_grey->setUniform1f("shininess", 50.0f);
		light_grey->setUniform1f("emissiveMultiplier", 0.0f);
		light_grey->flushAll();
		m_scene.addMaterial(light_grey);

		Engine::Ref_ptr<Engine::material> water_mat = Engine::material::create(color_shader, "water_blue");
		water_mat->setUniform4f("alb", 0.1f, 0.1f, 0.8f, 1.0f);
		water_mat->setUniform1f("ambCoefficient", 0.1f);
		water_mat->setUniform1f("specCoefficient", 1.0f);
		water_mat->setUniform1f("shininess", 25.0f);
		water_mat->setUniform1f("emissiveMultiplier", 0.0f);
		water_mat->flushAll();
		m_scene.addMaterial(water_mat);

		Engine::Ref_ptr<Engine::material> lighthouse_mat = Engine::material::create(alb_map_shader, "lighthouse_mat");
		lighthouse_mat->setTexture("u_texture", lighthouse_tex);
		lighthouse_mat->setUniform1f("ambCoefficient", 0.1f);
		lighthouse_mat->setUniform1f("specCoefficient", 1.0f);
		lighthouse_mat->setUniform1f("shininess", 100.0f);
		lighthouse_mat->setUniform1f("emissiveMultiplier", 0.0f);
		lighthouse_mat->flushAll();
		m_scene.addMaterial(lighthouse_mat);

		Engine::Ref_ptr<Engine::material> boat_mat = Engine::material::create(color_shader, "boat_brown");
		boat_mat->setUniform4f("alb", 0.5451f, 0.2706f, 0.07451f, 1.0f);
		boat_mat->setUniform1f("ambCoefficient", 0.25f);
		boat_mat->setUniform1f("shininess", 45.0f);
		boat_mat->setUniform1f("emissiveMultiplier", 0.0f);
		boat_mat->flushAll();
		m_scene.addMaterial(boat_mat);

		Engine::Ref_ptr<Engine::material> figureMat = Engine::material::create(alb_map_shader, "figureMat");
		figureMat->setTexture("u_texture", headTex);
		figureMat->setUniform1f("ambCoefficient", 0.1f);
		figureMat->setUniform1f("specCoefficient", 1.0f);
		figureMat->setUniform1f("shininess", 50.0f);
		figureMat->setUniform1f("emissiveMultiplier", 0.0f);
		figureMat->flushAll();
		m_scene.addMaterial(figureMat);

		Engine::Ref_ptr<Engine::material> mat_emissive = Engine::material::create(emissive_bloom, "emissive", flag_depth_test | flag_no_deferred);
		mat_emissive->setUniform4f("emissiveColor", { 0.1f, 0.1f, 1.0f, 1.0f });
		mat_emissive->setUniform1f("multiplier", 5.0f);
		mat_emissive->flushAll();

		Engine::Ref_ptr<Engine::mesh> environment = Engine::mesh::create(environment_va, light_grey, "environment");
		environment->setScale(11.1111111f);
		m_scene.addMesh(environment);

		Engine::Ref_ptr<Engine::mesh> boat = Engine::mesh::create(boat_va, boat_mat, "boat");
		boat->setScale(0.5f);
		boat->setPos({-6.5f, -8.8f, 0.0f, 1.0f});
		boat->attachMovement(Engine::CircularMeshMovement::create(vec3(0.0f, 1.0f, 0.0f), vec3(0.0f, 0.0f, 0.0f), 0.0174533f * 60.0f));
		boat->attachMovement(Engine::MeshRotation::create(0.0f, 1.05f, 0.0f));
		m_scene.addMesh(boat);

		Engine::Ref_ptr<Engine::mesh> figure = Engine::mesh::create(figure_va, figureMat, "figure");
		figure->setPos({ -7.6f, -8.0f, 1.0f, 1.0f });
		figure->setRot({0.0f, -PI / 2.0f, 0.0f});
		figure->attachMovement(Engine::CircularMeshMovement::create(vec3(0.0f, 1.0f, 0.0f), vec3(0.0f, 0.0f, 0.0f), 0.0174533f * 60.0f));
		figure->attachMovement(Engine::MeshRotation::create(0.0f, 1.05f, 0.0f));
		m_scene.addMesh(figure);

		Engine::Ref_ptr<Engine::mesh> water = Engine::mesh::create(water_va, water_mat, "water");
		water->setPos({ 0.0f, -9.5f, 0.0f, 1.0f });
		m_scene.addMesh(water);

		Engine::Ref_ptr<Engine::mesh> lighthouse = Engine::mesh::create(lighthouse_va, lighthouse_mat, "lighthouse");
		lighthouse->setPos({0.0f, 0.0f, 0.0f, 1.0f});
		m_scene.addMesh(lighthouse);

		Engine::Ref_ptr<Engine::mesh> lightbulb = Engine::mesh::create(lightbulb_va, mat_emissive, "lightbulb");
		lightbulb->setScale(0.5f);
		m_scene.addMesh(lightbulb);

		Engine::PtrPtr<Engine::pointLight> lamp(Engine::Renderer::addDynamicPointLight({ vec3(6.0f, 5.0f, 3.0f), vec3(0.14f, 0.14f, 0.14f), vec3(0.14f, 0.14f, 0.14f), vec3(0.14f, 0.14f, 0.14f), 1.0f, 0.00f, 1.0f }));//only use the quadratic component
		m_scene.addLight(lamp);
		Engine::PtrPtr<Engine::spotLight> spot_0(Engine::Renderer::addDynamicSpotLight({ vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, -0.242542f, -0.970167f), vec3(0.45f, 0.45f, 0.3f), vec3(0.45f, 0.45f, 0.3f), vec3(0.45f, 0.45f, 0.3), cosf(15.0f * (PI / 180.0f)) }));
		m_scene.addLight(spot_0);
		Engine::Ref_ptr<Engine::SpotLightMovement> rotation_0 = Engine::SpotLightRotation::create(0.0f, -0.9f, 0.0f);
		rotation_0->attachToLight(spot_0);
		m_scene.addLightMovement(rotation_0);
		uint32_t animationSteps[] = { 0, 0, 0, 1, 2, 0, 2, 2, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 1 };
		Engine::Ref_ptr<Engine::SpotLightAnimation> flicker = Engine::SpotLightAnimation::create(3, 10, 20, 0.1f, 1.0f, 1.0f, animationSteps,
			vec3(0.315f, 0.315f, 0.21f), vec3(0.252f, 0.252f, 0.168f), vec3(0.0f, 0.0f, 0.0f));
		flicker->attachToLight(spot_0);
		m_scene.addLightAnimation(flicker);

		//now add the material that is attached to the light
		m_scene.addMaterial(mat_emissive, [](const void* light, Engine::material* mat) {
			Engine::PtrPtr<Engine::spotLight> Light = (Engine::spotLight**)light;
			mat->setUniform4fF("emissiveColor", Light->diffuse);
			}, spot_0.get());
	}

	void experiment()
	{
		Engine::Ref_ptr<Engine::vertexArray> environment_va = Engine::vertexArray::create();
		environment_va->load("cube_solid.model");
		environment_va->unbind();
		Engine::Ref_ptr<Engine::vertexArray> spot_light_va = Engine::vertexArray::create();
		spot_light_va->load("spot_light.model");
		spot_light_va->unbind();
		Engine::Ref_ptr<Engine::vertexArray> stage_va = Engine::vertexArray::create();
		stage_va->load("stage.model");
		stage_va->unbind();
		Engine::Ref_ptr<Engine::vertexArray> duck_va = Engine::vertexArray::create();
		duck_va->load("duck.model");
		duck_va->unbind();
		Engine::Ref_ptr<Engine::vertexArray> text_va = Engine::vertexArray::create();
		text_va->load("test_text.model");
		text_va->unbind();

		Engine::Ref_ptr<Engine::texture2d> spot_light_tex = Engine::texture2d::create("spotlight_bc.png", true);
		Engine::Ref_ptr<Engine::texture2d> duck_tex = Engine::texture2d::create("09-Default_albedo.jpg", true);

		Engine::Ref_ptr<Engine::shader> alb_map_shader = Engine::Renderer::getShaderLib()->load("deferred/geometry_pass/alb_map.shader");
		alb_map_shader->bindUniformBlock("ViewProjection", VIEWPROJ_BIND);

		Engine::Ref_ptr<Engine::shader> color_shader = Engine::Renderer::getShaderLib()->load("deferred/geometry_pass/color.shader");
		color_shader->bindUniformBlock("ViewProjection", VIEWPROJ_BIND);

		Engine::Ref_ptr<Engine::material> light_grey = Engine::material::create(color_shader, "light_grey");
		light_grey->setUniform4f("alb", 0.6f, 0.6f, 0.6f, 1.0f);
		light_grey->setUniform1f("ambCoefficient", 0.1666f);
		light_grey->setUniform1f("specCoefficient", 1.0f);
		light_grey->setUniform1f("shininess", 50.0f);
		light_grey->setUniform1f("emissiveMultiplier", 0.0f);
		light_grey->flushAll();
		m_scene.addMaterial(light_grey);

		Engine::Ref_ptr<Engine::material> black = Engine::material::create(color_shader, "black");
		black->setUniform4f("alb", 0.06f, 0.06f, 0.06f, 1.0f);
		black->setUniform1f("ambCoefficient", 0.1666f);
		black->setUniform1f("specCoefficient", 1.0f);
		black->setUniform1f("shininess", 100.0f);
		black->setUniform1f("emissiveMultiplier", 0.0f);
		black->flushAll();
		m_scene.addMaterial(black);

		Engine::Ref_ptr<Engine::material> yellow = Engine::material::create(color_shader, "yellow");
		yellow->setUniform4f("alb", 0.8f, 0.8f, 0.1f, 1.0f);
		yellow->setUniform1f("ambCoefficient", 0.1f);
		yellow->setUniform1f("specCoefficient", 1.0f);
		yellow->setUniform1f("shininess", 80.0f);
		yellow->setUniform1f("emissiveMultiplier", 0.0f);
		yellow->flushAll();
		m_scene.addMaterial(yellow);

		Engine::Ref_ptr<Engine::material> spot_light_mat = Engine::material::create(alb_map_shader, "spot_light_mat");
		spot_light_mat->setTexture("u_texture", spot_light_tex);
		spot_light_mat->setUniform1f("ambCoefficient", 0.1f);
		spot_light_mat->setUniform1f("specCoefficient", 1.0f);
		spot_light_mat->setUniform1f("shininess", 100.0f);
		spot_light_mat->setUniform1f("emissiveMultiplier", 0.0f);
		spot_light_mat->flushAll();
		m_scene.addMaterial(spot_light_mat);

		Engine::Ref_ptr<Engine::material> duck_mat = Engine::material::create(alb_map_shader, "duck_mat");
		duck_mat->setTexture("u_texture", duck_tex);
		duck_mat->setUniform1f("ambCoefficient", 0.1f);
		duck_mat->setUniform1f("specCoefficient", 1.0f);
		duck_mat->setUniform1f("shininess", 80.0f);
		duck_mat->setUniform1f("emissiveMultiplier", 0.0f);
		duck_mat->flushAll();
		m_scene.addMaterial(duck_mat);

		Engine::Ref_ptr<Engine::mesh> environment = Engine::mesh::create(environment_va, light_grey, "environment");
		environment->setScale(11.1111111f);
		m_scene.addMesh(environment);

		Engine::Ref_ptr<Engine::mesh> stage = Engine::mesh::create(stage_va, black, "stage");
		stage->setScale(0.9f);
		stage->setRot({ 0.0f, 3.1415926f / 2.0f , 0.0f });
		stage->setPos({0.0f, 0.0f, -6.0f, 1.0f});
		m_scene.addMesh(stage);

		Engine::Ref_ptr<Engine::mesh> duck = Engine::mesh::create(duck_va, duck_mat, "duck");
		duck->setScale(0.5f);
		duck->setPos({ -4.5f, 0.7f, -2.5f, 1.0f });
		m_scene.addMesh(duck);

		Engine::Ref_ptr<Engine::mesh> text = Engine::mesh::create(text_va, yellow, "text");
		text->setScale(2.0f);
		text->setRot({ 3.1415926f / 2.0f, 0.0f, 0.0f });
		text->setPos({ 0.0f, 6.0f, -5.5f, 1.0f });
		m_scene.addMesh(text);

		for (uint8_t i = 0; i < 8; i++)
		{
			char name[] = "spot_light_X";
			std::to_chars(name + 11, name + strlen(name), i);
			Engine::Ref_ptr<Engine::mesh> spot_light_mesh = Engine::mesh::create(spot_light_va, spot_light_mat, name);
			spot_light_mesh->setPos({ -5.0f + static_cast<float>(2*i), 9.5f, 0.0f, 1.0f });
			spot_light_mesh->attachMovement(Engine::MeshRotation::create(0.6f, 0.6f, 0.0f));
			m_scene.addMesh(spot_light_mesh);
			Engine::PtrPtr<Engine::spotLight> spot_light(Engine::Renderer::addDynamicSpotLight({ vec3(0.0f, -2.0f, 0.0f), vec3(0.0f, 0.0f, -1.0f), vec3(0.1f, 0.1f, 0.4f), vec3(0.2f, 0.2f, 1.0f), vec3(0.2f, 0.2f, 1.0f), cosf(15.0f * (PI / 180.0f)) }));
			m_scene.addLight(spot_light);
			spot_light_mesh->attachLight(spot_light);
			Engine::Ref_ptr<Engine::spotLightEffect> rgb_fade = Engine::spotLightColorFade::create(0.05f, 0.4f, 0.5f, 5.0f);
			rgb_fade->attachToLight(spot_light);
			rgb_fade->onInit();//AFTER the light has been attached one has to initialize
			m_scene.addLightEffect(rgb_fade);
		}

		Engine::PtrPtr<Engine::directionalLight> sun(Engine::Renderer::addDynamicDirLight({ vec3(0.0f, -0.7071067f, 0.7071067f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 0.0f, 0.0f) }));
		m_scene.addLight(sun);
		Engine::PtrPtr<Engine::pointLight> lamp(Engine::Renderer::addDynamicPointLight({ vec3(0.0f, 0.0f, 0.0f), vec3(0.2f, 0.2f, 0.2f), vec3(0.2f, 0.2f, 0.2f), vec3(0.2f, 0.2f, 0.2f), 1.0f, 0.01f, 0.01f }));
		m_scene.addLight(lamp);
	}

	void normalMapping()
	{
		Engine::Ref_ptr<Engine::vertexArray> gun_va = Engine::vertexArray::create();
		gun_va->load("gun_tb.model");
		gun_va->unbind();

		Engine::Ref_ptr<Engine::texture2d> gunDiffTex = Engine::texture2d::create("Cerberus_A.tga", true);
		Engine::Ref_ptr<Engine::texture2d> gunNormalTex = Engine::texture2d::create("Cerberus_N.tga");//USE LINEAR INTERPOLATION FOR NORMALMAPS AND NOT SOMETHING LIKE "FILTER_NEAREST" -> mutch smoother

		Engine::Ref_ptr<Engine::shader> alb_nrm_map_shader = Engine::Renderer::getShaderLib()->load("deferred/geometry_pass/alb_map_nrm_map.shader");
		alb_nrm_map_shader->bindUniformBlock("ViewProjection", VIEWPROJ_BIND);

		Engine::Ref_ptr<Engine::material> gunMat = Engine::material::create(alb_nrm_map_shader, "gun_mat");
		gunMat->setTexture("u_texture", gunDiffTex);
		gunMat->setTexture("u_normalMap", gunNormalTex);
		gunMat->setUniform1f("ambCoefficient", 0.1f);
		gunMat->setUniform1f("specCoefficient", 1.0f);
		gunMat->setUniform1f("shininess", 100.0f);
		gunMat->setUniform1f("emissiveMultiplier", 0.0f);
		gunMat->flushAll();
		m_scene.addMaterial(gunMat);

		Engine::Ref_ptr<Engine::mesh> gun = Engine::mesh::create(gun_va, gunMat, "gun");
		m_scene.addMesh(gun);

		Engine::PtrPtr<Engine::directionalLight> sun = Engine::Renderer::addDynamicDirLight({ vec3(0.0f, -0.7071067f, -0.7071067f), vec3(0.1f, 0.1f, 0.1f), vec3(0.8f, 0.8f, 0.8f), vec3(1.0f, 1.0f, 1.0f) });
		m_scene.addLight(sun);
		Engine::PtrPtr<Engine::pointLight> lamp = Engine::Renderer::addDynamicPointLight({ vec3(0.0f, 0.0f, 0.0f), vec3(0.1f, 0.1f, 0.1f), vec3(0.6f, 0.6f, 0.6f), vec3(0.9f, 0.9f, 0.9f), 1.0f, 0.01f, 0.01f });
		m_scene.addLight(lamp);
	}

	void bridge()
	{
		//load geometry
		Engine::Ref_ptr<Engine::vertexArray> bridge_a_va = Engine::vertexArray::create();
		bridge_a_va->load("bridge/bridge_a_tb.model");
		bridge_a_va->unbind();
		Engine::Ref_ptr<Engine::vertexArray> bridge_b_va = Engine::vertexArray::create();
		bridge_b_va->load("bridge/bridge_b_tb.model");
		bridge_b_va->unbind();
		Engine::Ref_ptr<Engine::vertexArray> wall_a_va = Engine::vertexArray::create();
		wall_a_va->load("bridge/wall_a_tb.model");
		wall_a_va->unbind();
		Engine::Ref_ptr<Engine::vertexArray> wall_c_va = Engine::vertexArray::create();
		wall_c_va->load("bridge/wall_c_tb.model");
		wall_c_va->unbind();
		Engine::Ref_ptr<Engine::vertexArray> wall_d_va = Engine::vertexArray::create();
		wall_d_va->load("bridge/wall_d_tb.model");
		wall_d_va->unbind();
		Engine::Ref_ptr<Engine::vertexArray> plane_va = Engine::vertexArray::create();
		plane_va->load("plane.model");
		plane_va->unbind();
		Engine::Ref_ptr<Engine::vertexArray> boar_va = Engine::vertexArray::create();
		boar_va->load("boar_tb.model");
		boar_va->unbind();
		Engine::Ref_ptr<Engine::vertexArray> deer_va = Engine::vertexArray::create();
		deer_va->load("deer_tb.model");
		deer_va->unbind();
		Engine::Ref_ptr<Engine::vertexArray> lightbulb_va = Engine::vertexArray::create();
		lightbulb_va->load("lightbulb.model");
		lightbulb_va->unbind();

		//load textures
		Engine::Ref_ptr<Engine::texture2d> Rock_RuinStoneBridge_A_Alb = Engine::texture2d::create("bridge/Rock_RuinStoneBridge_A_Alb.png", true);
		Engine::Ref_ptr<Engine::texture2d> Rock_RuinStoneBridge_B_Alb = Engine::texture2d::create("bridge/Rock_RuinStoneBridge_B_Alb.png", true);
		Engine::Ref_ptr<Engine::texture2d> Rock_RuinStoneWall_A_Alb = Engine::texture2d::create("bridge/Rock_RuinStoneWall_A_Alb.png", true);
		Engine::Ref_ptr<Engine::texture2d> Rock_RuinStoneWall_C_Alb = Engine::texture2d::create("bridge/Rock_RuinStoneWall_C_Alb.png", true);
		Engine::Ref_ptr<Engine::texture2d> Rock_RuinStoneWall_D_Alb = Engine::texture2d::create("bridge/Rock_RuinStoneWall_D_Alb.png", true);
		Engine::Ref_ptr<Engine::texture2d> Boar_Alb = Engine::texture2d::create("Boar_Alb_1.png", true);
		Engine::Ref_ptr<Engine::texture2d> Deer_Body_Alb = Engine::texture2d::create("Deer_Body_Alb.png", true);

		Engine::Ref_ptr<Engine::texture2d> Rock_RuinStoneBridge_A_Nrm = Engine::texture2d::create("bridge/Rock_RuinStoneBridge_A_Nrm_fixed.png");
		Engine::Ref_ptr<Engine::texture2d> Rock_RuinStoneBridge_B_Nrm = Engine::texture2d::create("bridge/Rock_RuinStoneBridge_B_Nrm_fixed.png");
		Engine::Ref_ptr<Engine::texture2d> Rock_RuinStoneWall_A_Nrm = Engine::texture2d::create("bridge/Rock_RuinStoneWall_A_Nrm_fixed.png");
		Engine::Ref_ptr<Engine::texture2d> Rock_RuinStoneWall_C_Nrm = Engine::texture2d::create("bridge/Rock_RuinStoneWall_C_Nrm_fixed.png");
		Engine::Ref_ptr<Engine::texture2d> Rock_RuinStoneWall_D_Nrm = Engine::texture2d::create("bridge/Rock_RuinStoneWall_D_Nrm_fixed.png");
		Engine::Ref_ptr<Engine::texture2d> Boar_Nrm = Engine::texture2d::create("Boar_Nrm.png");
		Engine::Ref_ptr<Engine::texture2d> Deer_Body_Nrm = Engine::texture2d::create("Deer_Body_Nrm.png");

		//load shaders
		Engine::Ref_ptr<Engine::shader> alb_nrm_map_shader = Engine::Renderer::getShaderLib()->load("deferred/geometry_pass/alb_map_nrm_map.shader");
		alb_nrm_map_shader->bindUniformBlock("ViewProjection", VIEWPROJ_BIND);

		Engine::Ref_ptr<Engine::shader> color_shader = Engine::Renderer::getShaderLib()->load("deferred/geometry_pass/color.shader");
		color_shader->bindUniformBlock("ViewProjection", VIEWPROJ_BIND);

		Engine::Ref_ptr<Engine::shader> emissive_bloom = Engine::Renderer::getShaderLib()->load("emissive/emissive_bloom.shader");
		emissive_bloom->bindUniformBlock("ViewProjection", VIEWPROJ_BIND);

		//create materials
		Engine::Ref_ptr<Engine::material> Mat_Rock_RuinStoneBridge_A = Engine::material::create(alb_nrm_map_shader, "Rock_RuinStoneBridge_A");
		Mat_Rock_RuinStoneBridge_A->setTexture("u_texture", Rock_RuinStoneBridge_A_Alb);
		Mat_Rock_RuinStoneBridge_A->setTexture("u_normalMap", Rock_RuinStoneBridge_A_Nrm);
		Mat_Rock_RuinStoneBridge_A->setUniform1f("ambCoefficient", 0.1f);
		Mat_Rock_RuinStoneBridge_A->setUniform1f("specCoefficient", 0.5f);
		Mat_Rock_RuinStoneBridge_A->setUniform1f("shininess", 100.0f);
		Mat_Rock_RuinStoneBridge_A->setUniform1f("emissiveMultiplier", 0.0f);
		Mat_Rock_RuinStoneBridge_A->flushAll();
		m_scene.addMaterial(Mat_Rock_RuinStoneBridge_A);
		Engine::Ref_ptr<Engine::material> Mat_Rock_RuinStoneBridge_B = Engine::material::create(alb_nrm_map_shader, "Rock_RuinStoneBridge_B");
		Mat_Rock_RuinStoneBridge_B->setTexture("u_texture", Rock_RuinStoneBridge_B_Alb);
		Mat_Rock_RuinStoneBridge_B->setTexture("u_normalMap", Rock_RuinStoneBridge_B_Nrm);
		Mat_Rock_RuinStoneBridge_B->setUniform1f("ambCoefficient", 0.1f);
		Mat_Rock_RuinStoneBridge_B->setUniform1f("specCoefficient", 0.5f);
		Mat_Rock_RuinStoneBridge_B->setUniform1f("shininess", 100.0f);
		Mat_Rock_RuinStoneBridge_B->setUniform1f("emissiveMultiplier", 0.0f);
		Mat_Rock_RuinStoneBridge_B->flushAll();
		m_scene.addMaterial(Mat_Rock_RuinStoneBridge_B);
		Engine::Ref_ptr<Engine::material> Mat_Rock_RuinStoneWall_A = Engine::material::create(alb_nrm_map_shader, "Rock_RuinStoneWall_A");
		Mat_Rock_RuinStoneWall_A->setTexture("u_texture", Rock_RuinStoneWall_A_Alb);
		Mat_Rock_RuinStoneWall_A->setTexture("u_normalMap", Rock_RuinStoneWall_A_Nrm);
		Mat_Rock_RuinStoneWall_A->setUniform1f("ambCoefficient", 0.1f);
		Mat_Rock_RuinStoneWall_A->setUniform1f("specCoefficient", 0.5f);
		Mat_Rock_RuinStoneWall_A->setUniform1f("shininess", 100.0f);
		Mat_Rock_RuinStoneWall_A->setUniform1f("emissiveMultiplier", 0.0f);
		Mat_Rock_RuinStoneWall_A->flushAll();
		m_scene.addMaterial(Mat_Rock_RuinStoneWall_A);
		Engine::Ref_ptr<Engine::material> Mat_Rock_RuinStoneWall_C = Engine::material::create(alb_nrm_map_shader, "Rock_RuinStoneWall_C");
		Mat_Rock_RuinStoneWall_C->setTexture("u_texture", Rock_RuinStoneWall_C_Alb);
		Mat_Rock_RuinStoneWall_C->setTexture("u_normalMap", Rock_RuinStoneWall_C_Nrm);
		Mat_Rock_RuinStoneWall_C->setUniform1f("ambCoefficient", 0.1f);
		Mat_Rock_RuinStoneWall_C->setUniform1f("specCoefficient", 0.5f);
		Mat_Rock_RuinStoneWall_C->setUniform1f("shininess", 100.0f);
		Mat_Rock_RuinStoneWall_C->setUniform1f("emissiveMultiplier", 0.0f);
		Mat_Rock_RuinStoneWall_C->flushAll();
		m_scene.addMaterial(Mat_Rock_RuinStoneWall_C);
		Engine::Ref_ptr<Engine::material> Mat_Rock_RuinStoneWall_D = Engine::material::create(alb_nrm_map_shader, "Rock_RuinStoneWall_D");
		Mat_Rock_RuinStoneWall_D->setTexture("u_texture", Rock_RuinStoneWall_D_Alb);
		Mat_Rock_RuinStoneWall_D->setTexture("u_normalMap", Rock_RuinStoneWall_D_Nrm);
		Mat_Rock_RuinStoneWall_D->setUniform1f("ambCoefficient", 0.1f);
		Mat_Rock_RuinStoneWall_D->setUniform1f("specCoefficient", 0.5f);
		Mat_Rock_RuinStoneWall_D->setUniform1f("shininess", 100.0f);
		Mat_Rock_RuinStoneWall_D->setUniform1f("emissiveMultiplier", 0.0f);
		Mat_Rock_RuinStoneWall_D->flushAll();
		m_scene.addMaterial(Mat_Rock_RuinStoneWall_D);
		Engine::Ref_ptr<Engine::material> green = Engine::material::create(color_shader, "green");
		green->setUniform4f("alb", 0.2f, 0.8f, 0.2f, 1.0f);
		green->setUniform1f("ambCoefficient", 0.25f);
		green->setUniform1f("specCoefficient", 0.1f);
		green->setUniform1f("shininess", 100.0f);
		green->setUniform1f("emissiveMultiplier", 0.0f);
		green->flushAll();
		m_scene.addMaterial(green);
		Engine::Ref_ptr<Engine::material> Mat_Boar = Engine::material::create(alb_nrm_map_shader, "Boar");
		Mat_Boar->setTexture("u_texture", Boar_Alb);
		Mat_Boar->setTexture("u_normalMap", Boar_Nrm);
		Mat_Boar->setUniform1f("ambCoefficient", 0.1f);
		Mat_Boar->setUniform1f("specCoefficient", 0.01f);
		Mat_Boar->setUniform1f("shininess", 20.0f);
		Mat_Boar->setUniform1f("emissiveMultiplier", 0.0f);
		Mat_Boar->flushAll();
		m_scene.addMaterial(Mat_Boar);
		Engine::Ref_ptr<Engine::material> Mat_Deer = Engine::material::create(alb_nrm_map_shader, "Deer");
		Mat_Deer->setTexture("u_texture", Deer_Body_Alb);
		Mat_Deer->setTexture("u_normalMap", Deer_Body_Nrm);
		Mat_Deer->setUniform1f("ambCoefficient", 0.1f);
		Mat_Deer->setUniform1f("specCoefficient", 0.01f);
		Mat_Deer->setUniform1f("shininess", 20.0f);
		Mat_Deer->setUniform1f("emissiveMultiplier", 0.0f);
		Mat_Deer->flushAll();
		m_scene.addMaterial(Mat_Deer);

		Engine::Ref_ptr<Engine::material> mat_emissive = Engine::material::create(emissive_bloom, "emissive", flag_depth_test | flag_no_deferred);
		mat_emissive->setUniform4f("emissiveColor", { 0.1f, 0.1f, 0.1f, 1.0f });
		mat_emissive->setUniform1f("multiplier", 20.0f);
		mat_emissive->flushAll();

		//create meshes
		Engine::Ref_ptr<Engine::mesh> bridge_a = Engine::mesh::create(bridge_a_va, Mat_Rock_RuinStoneBridge_A, "bridge_a");
		bridge_a->setPos({ 0.0f, 0.0f, 10.0f, 1.0f });
		m_scene.addMesh(bridge_a);
		Engine::Ref_ptr<Engine::mesh> bridge_b = Engine::mesh::create(bridge_b_va, Mat_Rock_RuinStoneBridge_B, "bridge_b");
		bridge_b->setPos({ 0.0f, 0.0f, 10.0f, 1.0f });
		m_scene.addMesh(bridge_b);
		Engine::Ref_ptr<Engine::mesh> wall_a = Engine::mesh::create(wall_a_va, Mat_Rock_RuinStoneWall_A, "wall_a");
		wall_a->setPos({ 0.0f, 0.0f, 10.0f, 1.0f });
		m_scene.addMesh(wall_a);
		Engine::Ref_ptr<Engine::mesh> wall_c = Engine::mesh::create(wall_c_va, Mat_Rock_RuinStoneWall_C, "wall_c");
		wall_c->setPos({ 0.0f, 0.0f, 10.0f, 1.0f });
		m_scene.addMesh(wall_c);
		Engine::Ref_ptr<Engine::mesh> wall_d = Engine::mesh::create(wall_d_va, Mat_Rock_RuinStoneWall_D, "wall_d");
		wall_d->setPos({ 0.0f, 0.0f, 10.0f, 1.0f });
		m_scene.addMesh(wall_d);
		Engine::Ref_ptr<Engine::mesh> plane = Engine::mesh::create(plane_va, green, "plane");
		plane->setScale(10.0f);
		plane->setPos({0.0f, -8.0f, 0.0f, 1.0f});
		m_scene.addMesh(plane);
		Engine::Ref_ptr<Engine::mesh> boar = Engine::mesh::create(boar_va, Mat_Boar, "boar");
		boar->setPos({0.0f, -0.1f, 0.0f, 1.0f});
		m_scene.addMesh(boar);
		Engine::Ref_ptr<Engine::mesh> deer = Engine::mesh::create(deer_va, Mat_Deer, "deer");
		deer->setPos({ 0.0f, -0.1f, 5.0f, 1.0f });
		m_scene.addMesh(deer);
		Engine::Ref_ptr<Engine::mesh> lightbulb = Engine::mesh::create(lightbulb_va, mat_emissive, "lightbulb");
		lightbulb->setScale(0.1f);

		//add lights
		Engine::PtrPtr<Engine::pointLight> lamp = Engine::Renderer::addDynamicPointLight({ vec3(0.0f, 0.0f, 0.0f), vec3(0.1f, 0.1f, 0.1f), vec3(0.6f, 0.6f, 0.6f), vec3(0.9f, 0.9f, 0.9f), 1.0f, 0.01f, 0.01f });
		lightbulb->attachLight(lamp);
		m_scene.addMesh(lightbulb);
		m_scene.addLight(lamp);

		m_scene.addMaterial(mat_emissive, [](const void* light, Engine::material* mat) {
			Engine::PtrPtr<Engine::pointLight> Light = (Engine::pointLight**)light;
			mat->setUniform4fF("emissiveColor", Light->diffuse);
			}, lamp.get());

		Engine::PtrPtr<Engine::directionalLight> sun = Engine::Renderer::addDynamicDirLight({ vec3(0.0f, -0.7071067f, -0.7071067f), vec3(0.1f, 0.1f, 0.1f), vec3(2.5f, 2.5f, 2.5f), vec3(3.75f, 3.75f, 3.75f) });
		m_scene.addLight(sun);
}

	void nrm_map_spc_map()
	{
		Engine::Ref_ptr<Engine::vertexArray> gun_va = Engine::vertexArray::create();
		gun_va->load("gun_tb.model");
		gun_va->unbind();

		Engine::Ref_ptr<Engine::texture2d> gunDiffTex = Engine::texture2d::create("Cerberus_A.tga", true);
		Engine::Ref_ptr<Engine::texture2d> gunNormalTex = Engine::texture2d::create("Cerberus_N.tga");//USE LINEAR INTERPOLATION FOR NORMALMAPS AND NOT SOMETHING LIKE "FILTER_NEAREST" -> mutch smoother
		Engine::Ref_ptr<Engine::texture2d> gunSpecTex = Engine::texture2d::create("Cerberus_R.tga");

		Engine::Ref_ptr<Engine::shader> shader_alb_nrm_spec = Engine::Renderer::getShaderLib()->load("deferred/geometry_pass/alb_map_nrm_map_spc_map.shader");
		shader_alb_nrm_spec->bindUniformBlock("ViewProjection", VIEWPROJ_BIND);

		Engine::Ref_ptr<Engine::material> gunMat = Engine::material::create(shader_alb_nrm_spec, "gun_mat");//just use the variant for directional lights for initialization
		gunMat->setTexture("u_texture", gunDiffTex);
		gunMat->setTexture("u_normalMap", gunNormalTex);
		gunMat->setTexture("u_specMap", gunSpecTex);
		gunMat->setUniform1f("ambCoefficient", 0.1f);
		gunMat->setUniform1f("shininess", 100.0f);
		gunMat->setUniform1f("emissiveMultiplier", 0.0f);
		gunMat->flushAll();
		m_scene.addMaterial(gunMat);

		Engine::Ref_ptr<Engine::mesh> gun = Engine::mesh::create(gun_va, gunMat, "gun");
		m_scene.addMesh(gun);

		Engine::PtrPtr<Engine::directionalLight> sun = Engine::Renderer::addDynamicDirLight({ vec3(0.0f, -0.7071067f, -0.7071067f), vec3(0.1f, 0.1f, 0.1f), vec3(2.5f, 2.5f, 2.5f), vec3(3.75f, 3.75f, 3.75f) });
		m_scene.addLight(sun);

		//m_scene.setSkybox(Engine::texture3d::create("skyboxes/stadium/"));
	}

	void emissive()
	{
		Engine::Ref_ptr<Engine::vertexArray> environment_va = Engine::vertexArray::create();
		environment_va->load("cube_solid.model");
		environment_va->unbind();
		Engine::Ref_ptr<Engine::vertexArray> lightbulb_va = Engine::vertexArray::create();
		lightbulb_va->load("lightbulb.model");
		lightbulb_va->unbind();

		Engine::Ref_ptr<Engine::shader> color_shader = Engine::Renderer::getShaderLib()->load("deferred/geometry_pass/color.shader");
		color_shader->bindUniformBlock("ViewProjection", VIEWPROJ_BIND);
			
		Engine::Ref_ptr<Engine::shader> emissive_bloom = Engine::Renderer::getShaderLib()->load("emissive/emissive_bloom.shader");
		emissive_bloom->bindUniformBlock("ViewProjection", VIEWPROJ_BIND);

		Engine::Ref_ptr<Engine::material> light_grey = Engine::material::create(color_shader, "light_grey");
		light_grey->setUniform4f("alb", 0.6f, 0.6f, 0.6f, 1.0f);
		light_grey->setUniform1f("ambCoefficient", 0.2f);
		light_grey->setUniform1f("specCoefficient", 1.0f);
		light_grey->setUniform1f("shininess", 50.0f);
		light_grey->setUniform1f("emissiveMultiplier", 0.0f);
		light_grey->flushAll();
		m_scene.addMaterial(light_grey);

		Engine::Ref_ptr<Engine::material> mat_emissive = Engine::material::create(emissive_bloom, "emissive", flag_depth_test | flag_no_deferred);
		mat_emissive->setUniform4f("emissiveColor", { 0.1f, 0.1f, 1.0f, 1.0f });
		mat_emissive->setUniform1f("multiplier", 5.0f);
		mat_emissive->flushAll();

		Engine::Ref_ptr<Engine::mesh> environment = Engine::mesh::create(environment_va, light_grey, "environment");
		environment->setScale(11.1111111f);
		m_scene.addMesh(environment);

		Engine::PtrPtr<Engine::pointLight> lamp(Engine::Renderer::addDynamicPointLight({ vec3(0.0f, 0.0f, 0.0f), vec3(0.1f, 0.1f, 1.0f), vec3(0.1f, 0.1f, 1.0f), vec3(0.1f, 0.1f, 1.0f), 1.0f, 0.01f, 0.01f }));
		Engine::Ref_ptr<Engine::mesh> lightbulb = Engine::mesh::create(lightbulb_va, mat_emissive, "lightbulb");
		lightbulb->attachLight(lamp);
		m_scene.addMesh(lightbulb);
		m_scene.addLight(lamp);

		m_scene.addMaterial(mat_emissive, [] (const void* light, Engine::material* mat) {
			Engine::PtrPtr<Engine::pointLight> Light = (Engine::pointLight**)light;
			mat->setUniform4fF("emissiveColor", Light->diffuse);
			}, lamp.get());//<- an update-function and a pointer to some update-data get's supplied -> various kinds of materials could be created like this
	}

	void deferred_test()
	{
		Engine::Ref_ptr<Engine::vertexArray> gun_va = Engine::vertexArray::create();
		gun_va->load("gun_tb.model");
		gun_va->unbind();
		Engine::Ref_ptr<Engine::vertexArray> lightbulb_va = Engine::vertexArray::create();
		lightbulb_va->load("lightbulb.model");
		lightbulb_va->unbind();
		Engine::Ref_ptr<Engine::vertexArray> cube_va = Engine::vertexArray::create();
		cube_va->load("cube.model");
		cube_va->unbind();

		Engine::Ref_ptr<Engine::texture2d> gunDiffTex = Engine::texture2d::create("Cerberus_A.tga", true);
		Engine::Ref_ptr<Engine::texture2d> gunNormalTex = Engine::texture2d::create("Cerberus_N.tga");//USE LINEAR INTERPOLATION FOR NORMALMAPS AND NOT SOMETHING LIKE "FILTER_NEAREST" -> mutch smoother
		Engine::Ref_ptr<Engine::texture2d> gunSpecTex = Engine::texture2d::create("Cerberus_R.tga");
		Engine::Ref_ptr<Engine::texture2d> cubeTex = Engine::texture2d::create("checkerboard.png");

		Engine::Ref_ptr<Engine::shader> shader_alb_nrm_spec = Engine::Renderer::getShaderLib()->load("deferred/geometry_pass/alb_map_nrm_map_spc_map.shader");
		shader_alb_nrm_spec->bindUniformBlock("ViewProjection", VIEWPROJ_BIND);

		Engine::Ref_ptr<Engine::shader> emissive_bloom = Engine::Renderer::getShaderLib()->load("emissive/emissive_bloom.shader");
		emissive_bloom->bindUniformBlock("ViewProjection", VIEWPROJ_BIND);

		Engine::Ref_ptr<Engine::shader> shader_alb_map = Engine::Renderer::getShaderLib()->load("deferred/geometry_pass/alb_map.shader");
		shader_alb_map->bindUniformBlock("ViewProjection", VIEWPROJ_BIND);

		Engine::Ref_ptr<Engine::material> gunMat = Engine::material::create(shader_alb_nrm_spec, "gun_mat");
		gunMat->setUniform1f("ambCoefficient", 0.1f);
		gunMat->setUniform1f("shininess", 100.0f);
		gunMat->setUniform1f("emissiveMultiplier", 0.0f);
		gunMat->flushAll();
		gunMat->setTexture("u_texture", gunDiffTex);
		gunMat->setTexture("u_normalMap", gunNormalTex);
		gunMat->setTexture("u_specMap", gunSpecTex);
		m_scene.addMaterial(gunMat);

		Engine::Ref_ptr<Engine::material> mat_emissive = Engine::material::create(emissive_bloom, "emissive", flag_depth_test | flag_no_deferred);
		mat_emissive->setUniform4f("emissiveColor", { 0.1f, 0.1f, 1.0f, 1.0f });
		mat_emissive->setUniform1f("multiplier", 5.0f);
		mat_emissive->flushAll();

		Engine::Ref_ptr<Engine::material> cubeMat = Engine::material::create(shader_alb_map, "cube_mat");
		cubeMat->setUniform1f("ambCoefficient", 0.1f);
		cubeMat->setUniform1f("specCoefficient", 1.0f);
		cubeMat->setUniform1f("shininess", 100.0f);
		cubeMat->setUniform1f("emissiveMultiplier", 0.0f);
		cubeMat->flushAll();
		cubeMat->setTexture("u_texture", cubeTex);
		m_scene.addMaterial(cubeMat);

		Engine::Ref_ptr<Engine::mesh> gun = Engine::mesh::create(gun_va, gunMat, "gun");
		m_scene.addMesh(gun);

		Engine::PtrPtr<Engine::pointLight> lamp(Engine::Renderer::addDynamicPointLight({ vec3(0.0f, 0.0f, 0.0f), vec3(0.1f, 0.1f, 1.0f), vec3(0.1f, 0.1f, 1.0f), vec3(0.1f, 0.1f, 1.0f), 1.0f, 0.01f, 0.01f }));
		Engine::Ref_ptr<Engine::mesh> lightbulb = Engine::mesh::create(lightbulb_va, mat_emissive, "lightbulb");
		lightbulb->setScale(0.1f);
		lightbulb->setPos({ 0.0f, -2.0f, 0.0f, 1.0f });
		lightbulb->attachLight(lamp);
		m_scene.addMesh(lightbulb);
		m_scene.addLight(lamp);

		Engine::Ref_ptr<Engine::mesh> cube = Engine::mesh::create(cube_va, cubeMat, "cube");
		m_scene.addMesh(cube);

		m_scene.addMaterial(mat_emissive, [](const void* light, Engine::material* mat) {
			Engine::PtrPtr<Engine::pointLight> Light = (Engine::pointLight**)light;
			mat->setUniform4fF("emissiveColor", Light->diffuse);
			}, lamp.get());//<- an update-function and a pointer to some update-data get's supplied -> various kinds of materials could be created like this

		Engine::PtrPtr<Engine::directionalLight> sun = Engine::Renderer::addDynamicDirLight({ vec3(0.0f, -0.7071067f, -0.7071067f), vec3(0.1f, 0.1f, 0.1f), vec3(2.5f, 2.5f, 2.5f), vec3(3.75f, 3.75f, 3.75f) });
		m_scene.addLight(sun);
	}

	void hylia_bridge()
	{
		//load geometry
		Engine::Ref_ptr<Engine::vertexArray> Arch_0_Mt_Wall_HyliaStucco_A_Va = Engine::vertexArray::create();
		Arch_0_Mt_Wall_HyliaStucco_A_Va->load("hylia_bridge/Arch_0_Mt_Wall_HyliaStucco_A_tb.model");
		Arch_0_Mt_Wall_HyliaStucco_A_Va->unbind();
		Engine::Ref_ptr<Engine::vertexArray> Arch_0_Mt_Wall_SmallBrick_A_Va = Engine::vertexArray::create();
		Arch_0_Mt_Wall_SmallBrick_A_Va->load("hylia_bridge/Arch_0_Mt_Wall_SmallBrick_A_tb.model");
		Arch_0_Mt_Wall_SmallBrick_A_Va->unbind();
		Engine::Ref_ptr<Engine::vertexArray> Basis_0_Mt_Builparts_HyliaOrnament_C_Va = Engine::vertexArray::create();
		Basis_0_Mt_Builparts_HyliaOrnament_C_Va->load("hylia_bridge/Basis_0_Mt_Builparts_HyliaOrnament_C_tb.model");
		Basis_0_Mt_Builparts_HyliaOrnament_C_Va->unbind();
		Engine::Ref_ptr<Engine::vertexArray> Center_Deco_0_Mt_Builparts_HyliaOrnament_B_Va = Engine::vertexArray::create();
		Center_Deco_0_Mt_Builparts_HyliaOrnament_B_Va->load("hylia_bridge/Center_Deco_0_Mt_Builparts_HyliaOrnament_B_tb.model");
		Center_Deco_0_Mt_Builparts_HyliaOrnament_B_Va->unbind();
		Engine::Ref_ptr<Engine::vertexArray> Center_Deco_3_Mt_Wall_TOTTile01_G_Va = Engine::vertexArray::create();
		Center_Deco_3_Mt_Wall_TOTTile01_G_Va->load("hylia_bridge/Center_Deco_3_Mt_Wall_TOTTile01_G_tb.model");
		Center_Deco_3_Mt_Wall_TOTTile01_G_Va->unbind();
		Engine::Ref_ptr<Engine::vertexArray> Center_Floor_0_Mt_Builparts_HyliaStoneBrick_A_Va = Engine::vertexArray::create();
		Center_Floor_0_Mt_Builparts_HyliaStoneBrick_A_Va->load("hylia_bridge/Center_Floor_0_Mt_Builparts_HyliaStoneBrick_A_tb.model");
		Center_Floor_0_Mt_Builparts_HyliaStoneBrick_A_Va->unbind();
		Engine::Ref_ptr<Engine::vertexArray> Circle_Paving_1_Mt_Wall_FloorBrick_A_Va = Engine::vertexArray::create();
		Circle_Paving_1_Mt_Wall_FloorBrick_A_Va->load("hylia_bridge/Circle_Paving_1_Mt_Wall_FloorBrick_A_tb.model");
		Circle_Paving_1_Mt_Wall_FloorBrick_A_Va->unbind();
		Engine::Ref_ptr<Engine::vertexArray> Circle_Paving_2_Mt_Wall_ReliefStone_D_Va = Engine::vertexArray::create();
		Circle_Paving_2_Mt_Wall_ReliefStone_D_Va->load("hylia_bridge/Circle_Paving_2_Mt_Wall_ReliefStone_D_tb.model");
		Circle_Paving_2_Mt_Wall_ReliefStone_D_Va->unbind();
		Engine::Ref_ptr<Engine::vertexArray> Circle_Paving_3_Mt_Plant_GreenGrassField_B_Va = Engine::vertexArray::create();//<- the one without texture
		Circle_Paving_3_Mt_Plant_GreenGrassField_B_Va->load("hylia_bridge/Circle_Paving_3_Mt_Plant_GreenGrassField_B.model");
		Circle_Paving_3_Mt_Plant_GreenGrassField_B_Va->unbind();
		Engine::Ref_ptr<Engine::vertexArray> Circle_Paving_4_10_Mt_Wall_FloorBrick_B_Va = Engine::vertexArray::create();
		Circle_Paving_4_10_Mt_Wall_FloorBrick_B_Va->load("hylia_bridge/Circle_Paving_4_10_Mt_Wall_FloorBrick_B_tb.model");
		Circle_Paving_4_10_Mt_Wall_FloorBrick_B_Va->unbind();
		Engine::Ref_ptr<Engine::vertexArray> Crack_10_Mt_Wall_Crack_A_Va = Engine::vertexArray::create();
		Crack_10_Mt_Wall_Crack_A_Va->load("hylia_bridge/Crack_10_Mt_Wall_Crack_A_tb.model");
		Crack_10_Mt_Wall_Crack_A_Va->unbind();
		Engine::Ref_ptr<Engine::vertexArray> Decoration_1_4_Mt_Wall_TOTTileCutsurface_Va = Engine::vertexArray::create();
		Decoration_1_4_Mt_Wall_TOTTileCutsurface_Va->load("hylia_bridge/Decoration_1_4_Mt_Wall_TOTTileCutsurface_tb.model");
		Decoration_1_4_Mt_Wall_TOTTileCutsurface_Va->unbind();
		Engine::Ref_ptr<Engine::vertexArray> Fence_0_Mt_Stone_HyliaBridgeBreak_A_Va = Engine::vertexArray::create();
		Fence_0_Mt_Stone_HyliaBridgeBreak_A_Va->load("hylia_bridge/Fence_0_Mt_Stone_HyliaBridgeBreak_A_tb.model");
		Fence_0_Mt_Stone_HyliaBridgeBreak_A_Va->unbind();
		Engine::Ref_ptr<Engine::vertexArray> Main_Body_1_Mt_Wall_LargeBrick_A_Va = Engine::vertexArray::create();
		Main_Body_1_Mt_Wall_LargeBrick_A_Va->load("hylia_bridge/Main_Body_1_Mt_Wall_LargeBrick_A_tb.model");
		Main_Body_1_Mt_Wall_LargeBrick_A_Va->unbind();
		Engine::Ref_ptr<Engine::vertexArray> Ornament_3_Mt_Builparts_Ornament_A_Va = Engine::vertexArray::create();
		Ornament_3_Mt_Builparts_Ornament_A_Va->load("hylia_bridge/Ornament_3_Mt_Builparts_Ornament_A_tb.model");
		Ornament_3_Mt_Builparts_Ornament_A_Va->unbind();
		Engine::Ref_ptr<Engine::vertexArray> Pedestral_Center_2_Mt_Wall_TempleOfTime_A_Va = Engine::vertexArray::create();
		Pedestral_Center_2_Mt_Wall_TempleOfTime_A_Va->load("hylia_bridge/Pedestral_Center_2_Mt_Wall_TempleOfTime_A_tb.model");
		Pedestral_Center_2_Mt_Wall_TempleOfTime_A_Va->unbind();
		Engine::Ref_ptr<Engine::vertexArray> Stone_Paving_1_Mt_Wall_StoneRoad_D_Va = Engine::vertexArray::create();
		Stone_Paving_1_Mt_Wall_StoneRoad_D_Va->load("hylia_bridge/Stone_Paving_1_Mt_Wall_StoneRoad_D_tb.model");
		Stone_Paving_1_Mt_Wall_StoneRoad_D_Va->unbind();
		Engine::Ref_ptr<Engine::vertexArray> lightbulb_va = Engine::vertexArray::create();
		lightbulb_va->load("lightbulb.model");
		lightbulb_va->unbind();

		//load textures:
		//::::::::ALB::::::::::
		Engine::Ref_ptr<Engine::texture2d> CmnTex_Wall_HyliaStucco_A_Alb = Engine::texture2d::create("hylia_bridge/CmnTex_Wall_HyliaStucco_A_Alb.png", true);
		Engine::Ref_ptr<Engine::texture2d> CmnTex_Wall_SmallBrick_A_Alb = Engine::texture2d::create("hylia_bridge/CmnTex_Wall_SmallBrick_A_Alb.png", true);
		Engine::Ref_ptr<Engine::texture2d> Builparts_HyliaOrnament_C_Alb = Engine::texture2d::create("hylia_bridge/Builparts_HyliaOrnament_C_Alb.png", true);
		Engine::Ref_ptr<Engine::texture2d> CmnTex_Builparts_HyliaOrnament_B_Alb = Engine::texture2d::create("hylia_bridge/CmnTex_Builparts_HyliaOrnament_B_Alb.png", true);
		Engine::Ref_ptr<Engine::texture2d> CmnTex_Wall_TOTTile01_G_Alb = Engine::texture2d::create("hylia_bridge/CmnTex_Wall_TOTTile01_G_Alb.png", true);
		Engine::Ref_ptr<Engine::texture2d> Builparts_HyliaStoneBrick_A_Alb = Engine::texture2d::create("hylia_bridge/Builparts_HyliaStoneBrick_A_Alb.png", true);
		Engine::Ref_ptr<Engine::texture2d> Wall_FloorBrick_A_Alb = Engine::texture2d::create("hylia_bridge/Wall_FloorBrick_A_Alb.png", true);
		Engine::Ref_ptr<Engine::texture2d> CmnTex_Wall_ReliefStone_D_Alb = Engine::texture2d::create("hylia_bridge/CmnTex_Wall_ReliefStone_D_Alb.png", true);
		//here would be the one without texture atm
		//next one uses the texture "Wall_FloorBrick_A_Alb" which is already loaded
		Engine::Ref_ptr<Engine::texture2d> CmnTex_Wall_Crack_A_Alb = Engine::texture2d::create("hylia_bridge/CmnTex_Wall_Crack_A_Alb.png", true);
		Engine::Ref_ptr<Engine::texture2d> CmnTex_Wall_TOTTileCutsurface_Alb = Engine::texture2d::create("hylia_bridge/CmnTex_Wall_TOTTileCutsurface_Alb.png", true);
		Engine::Ref_ptr<Engine::texture2d> CmnTex_Stone_HyliaBridgeBreak_A_Alb = Engine::texture2d::create("hylia_bridge/CmnTex_Stone_HyliaBridgeBreak_A_Alb.png", true);
		Engine::Ref_ptr<Engine::texture2d> CmnTex_Wall_LargeBrick_A_Alb = Engine::texture2d::create("hylia_bridge/CmnTex_Wall_LargeBrick_A_Alb.png", true);
		Engine::Ref_ptr<Engine::texture2d> Builparts_Ornament_A_Alb = Engine::texture2d::create("hylia_bridge/Builparts_Ornament_A_Alb.png", true);
		Engine::Ref_ptr<Engine::texture2d> CmnTex_Wall_TempleOfTime_A_Alb = Engine::texture2d::create("hylia_bridge/CmnTex_Wall_TempleOfTime_A_Alb.png", true);
		Engine::Ref_ptr<Engine::texture2d> CmnTex_Wall_StoneRoad_D_Alb = Engine::texture2d::create("hylia_bridge/CmnTex_Wall_StoneRoad_D_Alb.png", true);
		//:::::::::NRM:::::::::::
		Engine::Ref_ptr<Engine::texture2d> CmnTex_Wall_HyliaStucco_A_Nrm = Engine::texture2d::create("hylia_bridge/CmnTex_Wall_HyliaStucco_A_Nrm.png");
		Engine::Ref_ptr<Engine::texture2d> CmnTex_Wall_SmallBrick_A_Nrm = Engine::texture2d::create("hylia_bridge/CmnTex_Wall_SmallBrick_A_Nrm.png");
		Engine::Ref_ptr<Engine::texture2d> Builparts_HyliaOrnament_C_Nrm = Engine::texture2d::create("hylia_bridge/Builparts_HyliaOrnament_C_Nrm.png");
		Engine::Ref_ptr<Engine::texture2d> CmnTex_Builparts_HyliaOrnament_B_Nrm = Engine::texture2d::create("hylia_bridge/CmnTex_Builparts_HyliaOrnament_B_Nrm.png");
		Engine::Ref_ptr<Engine::texture2d> CmnTex_Wall_TOTTile01_G_Nrm = Engine::texture2d::create("hylia_bridge/CmnTex_Wall_TOTTile01_G_Nrm.png");
		Engine::Ref_ptr<Engine::texture2d> Builparts_HyliaStoneBrick_A_Nrm = Engine::texture2d::create("hylia_bridge/Builparts_HyliaStoneBrick_A_Nrm.png");
		Engine::Ref_ptr<Engine::texture2d> Wall_FloorBrick_A_Nrm = Engine::texture2d::create("hylia_bridge/Wall_FloorBrick_A_Nrm.png");
		Engine::Ref_ptr<Engine::texture2d> CmnTex_Wall_ReliefStone_D_Nrm = Engine::texture2d::create("hylia_bridge/CmnTex_Wall_ReliefStone_D_Nrm.png");
		//here would be the one without texture atm
		//next one uses the texture "Wall_FloorBrick_A_Nrm" which is already loaded
		Engine::Ref_ptr<Engine::texture2d> CmnTex_Wall_Crack_A_Nrm = Engine::texture2d::create("hylia_bridge/CmnTex_Wall_Crack_A_Nrm.png");
		Engine::Ref_ptr<Engine::texture2d> CmnTex_Wall_TOTTileCutsurface_Nrm = Engine::texture2d::create("hylia_bridge/CmnTex_Wall_TOTTileCutsurface_Nrm.png");
		Engine::Ref_ptr<Engine::texture2d> CmnTex_Stone_HyliaBridgeBreak_A_Nrm = Engine::texture2d::create("hylia_bridge/CmnTex_Stone_HyliaBridgeBreak_A_Nrm.png");
		Engine::Ref_ptr<Engine::texture2d> CmnTex_Wall_LargeBrick_A_Nrm = Engine::texture2d::create("hylia_bridge/CmnTex_Wall_LargeBrick_A_Nrm.png");
		Engine::Ref_ptr<Engine::texture2d> Builparts_Ornament_A_Nrm = Engine::texture2d::create("hylia_bridge/Builparts_Ornament_A_Nrm.png");
		Engine::Ref_ptr<Engine::texture2d> CmnTex_Wall_TempleOfTime_A_Nrm = Engine::texture2d::create("hylia_bridge/CmnTex_Wall_TempleOfTime_A_Nrm.png");
		Engine::Ref_ptr<Engine::texture2d> CmnTex_Wall_StoneRoad_D_Nrm = Engine::texture2d::create("hylia_bridge/CmnTex_Wall_StoneRoad_D_Nrm.png");

		//load shaders
		Engine::Ref_ptr<Engine::shader> alb_nrm_map_shader = Engine::Renderer::getShaderLib()->load("deferred/geometry_pass/alb_map_nrm_map.shader");
		alb_nrm_map_shader->bindUniformBlock("ViewProjection", VIEWPROJ_BIND);

		Engine::Ref_ptr<Engine::shader> color_shader = Engine::Renderer::getShaderLib()->load("deferred/geometry_pass/color.shader");
		color_shader->bindUniformBlock("ViewProjection", VIEWPROJ_BIND);

		Engine::Ref_ptr<Engine::shader> emissive_bloom = Engine::Renderer::getShaderLib()->load("emissive/emissive_bloom.shader");
		emissive_bloom->bindUniformBlock("ViewProjection", VIEWPROJ_BIND);

		//create materials(named after their textures, not after the models that they are assigned to)
		Engine::Ref_ptr<Engine::material> Mat_CmnTex_Wall_HyliaStucco_A = Engine::material::create(alb_nrm_map_shader, "Mat_CmnTex_Wall_HyliaStucco_A");
		Mat_CmnTex_Wall_HyliaStucco_A->setTexture("u_texture", CmnTex_Wall_HyliaStucco_A_Alb);
		Mat_CmnTex_Wall_HyliaStucco_A->setTexture("u_normalMap", CmnTex_Wall_HyliaStucco_A_Nrm);
		Mat_CmnTex_Wall_HyliaStucco_A->setUniform1f("ambCoefficient", 0.1f);
		Mat_CmnTex_Wall_HyliaStucco_A->setUniform1f("specCoefficient", 0.05f);
		Mat_CmnTex_Wall_HyliaStucco_A->setUniform1f("shininess", 80.0f);
		Mat_CmnTex_Wall_HyliaStucco_A->setUniform1f("emissiveMultiplier", 0.0f);
		Mat_CmnTex_Wall_HyliaStucco_A->flushAll();
		m_scene.addMaterial(Mat_CmnTex_Wall_HyliaStucco_A);
		Engine::Ref_ptr<Engine::material> Mat_CmnTex_Wall_SmallBrick_A = Engine::material::create(alb_nrm_map_shader, "Mat_CmnTex_Wall_SmallBrick_A");
		Mat_CmnTex_Wall_SmallBrick_A->setTexture("u_texture", CmnTex_Wall_SmallBrick_A_Alb);
		Mat_CmnTex_Wall_SmallBrick_A->setTexture("u_normalMap", CmnTex_Wall_SmallBrick_A_Nrm);
		Mat_CmnTex_Wall_SmallBrick_A->setUniform1f("ambCoefficient", 0.1f);
		Mat_CmnTex_Wall_SmallBrick_A->setUniform1f("specCoefficient", 0.05f);
		Mat_CmnTex_Wall_SmallBrick_A->setUniform1f("shininess", 80.0f);
		Mat_CmnTex_Wall_SmallBrick_A->setUniform1f("emissiveMultiplier", 0.0f);
		Mat_CmnTex_Wall_SmallBrick_A->flushAll();
		m_scene.addMaterial(Mat_CmnTex_Wall_SmallBrick_A);
		Engine::Ref_ptr<Engine::material> Mat_Builparts_HyliaOrnament_C = Engine::material::create(alb_nrm_map_shader, "Mat_Builparts_HyliaOrnament_C");
		Mat_Builparts_HyliaOrnament_C->setTexture("u_texture", Builparts_HyliaOrnament_C_Alb);
		Mat_Builparts_HyliaOrnament_C->setTexture("u_normalMap", Builparts_HyliaOrnament_C_Nrm);
		Mat_Builparts_HyliaOrnament_C->setUniform1f("ambCoefficient", 0.1f);
		Mat_Builparts_HyliaOrnament_C->setUniform1f("specCoefficient", 0.05f);
		Mat_Builparts_HyliaOrnament_C->setUniform1f("shininess", 80.0f);
		Mat_Builparts_HyliaOrnament_C->setUniform1f("emissiveMultiplier", 0.0f);
		Mat_Builparts_HyliaOrnament_C->flushAll();
		m_scene.addMaterial(Mat_Builparts_HyliaOrnament_C);
		Engine::Ref_ptr<Engine::material> Mat_CmnTex_Builparts_HyliaOrnament_B = Engine::material::create(alb_nrm_map_shader, "Mat_CmnTex_Builparts_HyliaOrnament_B");
		Mat_CmnTex_Builparts_HyliaOrnament_B->setTexture("u_texture", CmnTex_Builparts_HyliaOrnament_B_Alb);
		Mat_CmnTex_Builparts_HyliaOrnament_B->setTexture("u_normalMap", CmnTex_Builparts_HyliaOrnament_B_Nrm);
		Mat_CmnTex_Builparts_HyliaOrnament_B->setUniform1f("ambCoefficient", 0.1f);
		Mat_CmnTex_Builparts_HyliaOrnament_B->setUniform1f("specCoefficient", 0.05f);
		Mat_CmnTex_Builparts_HyliaOrnament_B->setUniform1f("shininess", 80.0f);
		Mat_CmnTex_Builparts_HyliaOrnament_B->setUniform1f("emissiveMultiplier", 0.0f);
		Mat_CmnTex_Builparts_HyliaOrnament_B->flushAll();
		m_scene.addMaterial(Mat_CmnTex_Builparts_HyliaOrnament_B);
		Engine::Ref_ptr<Engine::material> Mat_CmnTex_Wall_TOTTile01_G = Engine::material::create(alb_nrm_map_shader, "Mat_CmnTex_Wall_TOTTile01_G");
		Mat_CmnTex_Wall_TOTTile01_G->setTexture("u_texture", CmnTex_Wall_TOTTile01_G_Alb);
		Mat_CmnTex_Wall_TOTTile01_G->setTexture("u_normalMap", CmnTex_Wall_TOTTile01_G_Nrm);
		Mat_CmnTex_Wall_TOTTile01_G->setUniform1f("ambCoefficient", 0.1f);
		Mat_CmnTex_Wall_TOTTile01_G->setUniform1f("specCoefficient", 0.05f);
		Mat_CmnTex_Wall_TOTTile01_G->setUniform1f("shininess", 80.0f);
		Mat_CmnTex_Wall_TOTTile01_G->setUniform1f("emissiveMultiplier", 0.0f);
		Mat_CmnTex_Wall_TOTTile01_G->flushAll();
		m_scene.addMaterial(Mat_CmnTex_Wall_TOTTile01_G);
		Engine::Ref_ptr<Engine::material> Mat_Builparts_HyliaStoneBrick_A = Engine::material::create(alb_nrm_map_shader, "Mat_Builparts_HyliaStoneBrick_A");
		Mat_Builparts_HyliaStoneBrick_A->setTexture("u_texture", Builparts_HyliaStoneBrick_A_Alb);
		Mat_Builparts_HyliaStoneBrick_A->setTexture("u_normalMap", Builparts_HyliaStoneBrick_A_Nrm);
		Mat_Builparts_HyliaStoneBrick_A->setUniform1f("ambCoefficient", 0.1f);
		Mat_Builparts_HyliaStoneBrick_A->setUniform1f("specCoefficient", 0.05f);
		Mat_Builparts_HyliaStoneBrick_A->setUniform1f("shininess", 80.0f);
		Mat_Builparts_HyliaStoneBrick_A->setUniform1f("emissiveMultiplier", 0.0f);
		Mat_Builparts_HyliaStoneBrick_A->flushAll();
		m_scene.addMaterial(Mat_Builparts_HyliaStoneBrick_A);
		Engine::Ref_ptr<Engine::material> Mat_Wall_FloorBrick_A = Engine::material::create(alb_nrm_map_shader, "Mat_Mat_Wall_FloorBrick_A");
		Mat_Wall_FloorBrick_A->setTexture("u_texture", Wall_FloorBrick_A_Alb);
		Mat_Wall_FloorBrick_A->setTexture("u_normalMap", Wall_FloorBrick_A_Nrm);
		Mat_Wall_FloorBrick_A->setUniform1f("ambCoefficient", 0.1f);
		Mat_Wall_FloorBrick_A->setUniform1f("specCoefficient", 0.05f);
		Mat_Wall_FloorBrick_A->setUniform1f("shininess", 80.0f);
		Mat_Wall_FloorBrick_A->setUniform1f("emissiveMultiplier", 0.0f);
		Mat_Wall_FloorBrick_A->flushAll();
		m_scene.addMaterial(Mat_Wall_FloorBrick_A);
		Engine::Ref_ptr<Engine::material> Mat_CmnTex_Wall_ReliefStone_D = Engine::material::create(alb_nrm_map_shader, "Mat_CmnTex_Wall_ReliefStone_D");
		Mat_CmnTex_Wall_ReliefStone_D->setTexture("u_texture", CmnTex_Wall_ReliefStone_D_Alb);
		Mat_CmnTex_Wall_ReliefStone_D->setTexture("u_normalMap", CmnTex_Wall_ReliefStone_D_Nrm);
		Mat_CmnTex_Wall_ReliefStone_D->setUniform1f("ambCoefficient", 0.1f);
		Mat_CmnTex_Wall_ReliefStone_D->setUniform1f("specCoefficient", 0.05f);
		Mat_CmnTex_Wall_ReliefStone_D->setUniform1f("shininess", 80.0f);
		Mat_CmnTex_Wall_ReliefStone_D->setUniform1f("emissiveMultiplier", 0.0f);
		Mat_CmnTex_Wall_ReliefStone_D->flushAll();
		m_scene.addMaterial(Mat_CmnTex_Wall_ReliefStone_D);
		Engine::Ref_ptr<Engine::material> green = Engine::material::create(color_shader, "green");
		green->setUniform4f("alb", 0.2f, 0.8f, 0.2f, 1.0f);
		green->setUniform1f("ambCoefficient", 0.25f);
		green->setUniform1f("specCoefficient", 0.1f);
		green->setUniform1f("shininess", 80.0f);
		green->setUniform1f("emissiveMultiplier", 0.0f);
		green->flushAll();
		m_scene.addMaterial(green);
		Engine::Ref_ptr<Engine::material> Mat_CmnTex_Wall_Crack_A = Engine::material::create(alb_nrm_map_shader, "Mat_CmnTex_Wall_Crack_A");
		Mat_CmnTex_Wall_Crack_A->dissableFlags(flag_cast_shadow);
		Mat_CmnTex_Wall_Crack_A->setTexture("u_texture", CmnTex_Wall_Crack_A_Alb);
		Mat_CmnTex_Wall_Crack_A->setTexture("u_normalMap", CmnTex_Wall_Crack_A_Nrm);
		Mat_CmnTex_Wall_Crack_A->setUniform1f("ambCoefficient", 0.1f);
		Mat_CmnTex_Wall_Crack_A->setUniform1f("specCoefficient", 0.05f);
		Mat_CmnTex_Wall_Crack_A->setUniform1f("shininess", 80.0f);
		Mat_CmnTex_Wall_Crack_A->setUniform1f("emissiveMultiplier", 0.0f);
		Mat_CmnTex_Wall_Crack_A->flushAll();
		m_scene.addMaterial(Mat_CmnTex_Wall_Crack_A);
		Engine::Ref_ptr<Engine::material> Mat_CmnTex_Wall_TOTTileCutsurface = Engine::material::create(alb_nrm_map_shader, "Mat_CmnTex_Wall_TOTTileCutsurface");
		Mat_CmnTex_Wall_TOTTileCutsurface->setTexture("u_texture", CmnTex_Wall_TOTTileCutsurface_Alb);
		Mat_CmnTex_Wall_TOTTileCutsurface->setTexture("u_normalMap", CmnTex_Wall_TOTTileCutsurface_Nrm);
		Mat_CmnTex_Wall_TOTTileCutsurface->setUniform1f("ambCoefficient", 0.1f);
		Mat_CmnTex_Wall_TOTTileCutsurface->setUniform1f("specCoefficient", 0.05f);
		Mat_CmnTex_Wall_TOTTileCutsurface->setUniform1f("shininess", 80.0f);
		Mat_CmnTex_Wall_TOTTileCutsurface->setUniform1f("emissiveMultiplier", 0.0f);
		Mat_CmnTex_Wall_TOTTileCutsurface->flushAll();
		m_scene.addMaterial(Mat_CmnTex_Wall_TOTTileCutsurface);
		Engine::Ref_ptr<Engine::material> Mat_CmnTex_Stone_HyliaBridgeBreak_A = Engine::material::create(alb_nrm_map_shader, "Mat_CmnTex_Stone_HyliaBridgeBreak_A");
		Mat_CmnTex_Stone_HyliaBridgeBreak_A->setTexture("u_texture", CmnTex_Stone_HyliaBridgeBreak_A_Alb);
		Mat_CmnTex_Stone_HyliaBridgeBreak_A->setTexture("u_normalMap", CmnTex_Stone_HyliaBridgeBreak_A_Nrm);
		Mat_CmnTex_Stone_HyliaBridgeBreak_A->setUniform1f("ambCoefficient", 0.1f);
		Mat_CmnTex_Stone_HyliaBridgeBreak_A->setUniform1f("specCoefficient", 0.05f);
		Mat_CmnTex_Stone_HyliaBridgeBreak_A->setUniform1f("shininess", 80.0f);
		Mat_CmnTex_Stone_HyliaBridgeBreak_A->setUniform1f("emissiveMultiplier", 0.0f);
		Mat_CmnTex_Stone_HyliaBridgeBreak_A->flushAll();
		m_scene.addMaterial(Mat_CmnTex_Stone_HyliaBridgeBreak_A);
		Engine::Ref_ptr<Engine::material> Mat_CmnTex_Wall_LargeBrick_A = Engine::material::create(alb_nrm_map_shader, "Mat_CmnTex_Wall_LargeBrick_A");
		Mat_CmnTex_Wall_LargeBrick_A->setTexture("u_texture", CmnTex_Wall_LargeBrick_A_Alb);
		Mat_CmnTex_Wall_LargeBrick_A->setTexture("u_normalMap", CmnTex_Wall_LargeBrick_A_Nrm);
		Mat_CmnTex_Wall_LargeBrick_A->setUniform1f("ambCoefficient", 0.1f);
		Mat_CmnTex_Wall_LargeBrick_A->setUniform1f("specCoefficient", 0.05f);
		Mat_CmnTex_Wall_LargeBrick_A->setUniform1f("shininess", 80.0f);
		Mat_CmnTex_Wall_LargeBrick_A->setUniform1f("emissiveMultiplier", 0.0f);
		Mat_CmnTex_Wall_LargeBrick_A->flushAll();
		m_scene.addMaterial(Mat_CmnTex_Wall_LargeBrick_A);
		Engine::Ref_ptr<Engine::material> Mat_Builparts_Ornament_A = Engine::material::create(alb_nrm_map_shader, "Mat_Builparts_Ornament_A");
		Mat_Builparts_Ornament_A->setTexture("u_texture", Builparts_Ornament_A_Alb);
		Mat_Builparts_Ornament_A->setTexture("u_normalMap", Builparts_Ornament_A_Nrm);
		Mat_Builparts_Ornament_A->setUniform1f("ambCoefficient", 0.1f);
		Mat_Builparts_Ornament_A->setUniform1f("specCoefficient", 0.05f);
		Mat_Builparts_Ornament_A->setUniform1f("shininess", 80.0f);
		Mat_Builparts_Ornament_A->setUniform1f("emissiveMultiplier", 0.0f);
		Mat_Builparts_Ornament_A->flushAll();
		m_scene.addMaterial(Mat_Builparts_Ornament_A);
		Engine::Ref_ptr<Engine::material> Mat_CmnTex_Wall_TempleOfTime_A = Engine::material::create(alb_nrm_map_shader, "Mat_CmnTex_Wall_TempleOfTime_A");
		Mat_CmnTex_Wall_TempleOfTime_A->setTexture("u_texture", CmnTex_Wall_TempleOfTime_A_Alb);
		Mat_CmnTex_Wall_TempleOfTime_A->setTexture("u_normalMap", CmnTex_Wall_TempleOfTime_A_Nrm);
		Mat_CmnTex_Wall_TempleOfTime_A->setUniform1f("ambCoefficient", 0.1f);
		Mat_CmnTex_Wall_TempleOfTime_A->setUniform1f("specCoefficient", 0.05f);
		Mat_CmnTex_Wall_TempleOfTime_A->setUniform1f("shininess", 80.0f);
		Mat_CmnTex_Wall_TempleOfTime_A->setUniform1f("emissiveMultiplier", 0.0f);
		Mat_CmnTex_Wall_TempleOfTime_A->flushAll();
		m_scene.addMaterial(Mat_CmnTex_Wall_TempleOfTime_A);
		Engine::Ref_ptr<Engine::material> Mat_CmnTex_Wall_StoneRoad_D = Engine::material::create(alb_nrm_map_shader, "Mat_CmnTex_Wall_StoneRoad_D");
		Mat_CmnTex_Wall_StoneRoad_D->setTexture("u_texture", CmnTex_Wall_StoneRoad_D_Alb);
		Mat_CmnTex_Wall_StoneRoad_D->setTexture("u_normalMap", CmnTex_Wall_StoneRoad_D_Nrm);
		Mat_CmnTex_Wall_StoneRoad_D->setUniform1f("ambCoefficient", 0.1f);
		Mat_CmnTex_Wall_StoneRoad_D->setUniform1f("specCoefficient", 0.05f);
		Mat_CmnTex_Wall_StoneRoad_D->setUniform1f("shininess", 80.0f);
		Mat_CmnTex_Wall_StoneRoad_D->setUniform1f("emissiveMultiplier", 0.0f);
		Mat_CmnTex_Wall_StoneRoad_D->flushAll();
		m_scene.addMaterial(Mat_CmnTex_Wall_StoneRoad_D);
		Engine::Ref_ptr<Engine::material> mat_emissive = Engine::material::create(emissive_bloom, "emissive", flag_depth_test | flag_no_deferred);
		mat_emissive->setUniform4f("emissiveColor", { 0.1f, 0.1f, 0.1f, 1.0f });
		mat_emissive->setUniform1f("multiplier", 20.0f);
		mat_emissive->flushAll();
		m_scene.addMaterial(mat_emissive);

		Engine::Ref_ptr<Engine::mesh> Arch_0_Mt_Wall_HyliaStucco_A = Engine::mesh::create(Arch_0_Mt_Wall_HyliaStucco_A_Va, Mat_CmnTex_Wall_HyliaStucco_A, "Arch_0_Mt_Wall_HyliaStucco_A");
		m_scene.addMesh(Arch_0_Mt_Wall_HyliaStucco_A);
		Engine::Ref_ptr<Engine::mesh> Arch_0_Mt_Wall_SmallBrick_A = Engine::mesh::create(Arch_0_Mt_Wall_SmallBrick_A_Va, Mat_CmnTex_Wall_SmallBrick_A, "Arch_0_Mt_Wall_SmallBrick_A");
		m_scene.addMesh(Arch_0_Mt_Wall_SmallBrick_A);
		Engine::Ref_ptr<Engine::mesh> Basis_0_Mt_Builparts_HyliaOrnament_C = Engine::mesh::create(Basis_0_Mt_Builparts_HyliaOrnament_C_Va, Mat_Builparts_HyliaOrnament_C, "Basis_0_Mt_Builparts_HyliaOrnament_C");
		m_scene.addMesh(Basis_0_Mt_Builparts_HyliaOrnament_C);
		Engine::Ref_ptr<Engine::mesh> Center_Deco_0_Mt_Builparts_HyliaOrnament_B = Engine::mesh::create(Center_Deco_0_Mt_Builparts_HyliaOrnament_B_Va, Mat_CmnTex_Builparts_HyliaOrnament_B, "Center_Deco_0_Mt_Builparts_HyliaOrnament_B");
		m_scene.addMesh(Center_Deco_0_Mt_Builparts_HyliaOrnament_B);
		Engine::Ref_ptr<Engine::mesh> Center_Deco_3_Mt_Wall_TOTTile01_G = Engine::mesh::create(Center_Deco_3_Mt_Wall_TOTTile01_G_Va, Mat_CmnTex_Wall_TOTTile01_G, "Center_Deco_3_Mt_Wall_TOTTile01_G");
		m_scene.addMesh(Center_Deco_3_Mt_Wall_TOTTile01_G);
		Engine::Ref_ptr<Engine::mesh> Center_Floor_0_Mt_Builparts_HyliaStoneBrick_A = Engine::mesh::create(Center_Floor_0_Mt_Builparts_HyliaStoneBrick_A_Va, Mat_Builparts_HyliaStoneBrick_A, "Center_Floor_0_Mt_Builparts_HyliaStoneBrick_A");
		m_scene.addMesh(Center_Floor_0_Mt_Builparts_HyliaStoneBrick_A);
		Engine::Ref_ptr<Engine::mesh> Circle_Paving_1_Mt_Wall_FloorBrick_A = Engine::mesh::create(Circle_Paving_1_Mt_Wall_FloorBrick_A_Va, Mat_Wall_FloorBrick_A, "Circle_Paving_1_Mt_Wall_FloorBrick_A");
		m_scene.addMesh(Circle_Paving_1_Mt_Wall_FloorBrick_A);
		Engine::Ref_ptr<Engine::mesh> Circle_Paving_2_Mt_Wall_ReliefStone_D = Engine::mesh::create(Circle_Paving_2_Mt_Wall_ReliefStone_D_Va, Mat_CmnTex_Wall_ReliefStone_D, "Circle_Paving_2_Mt_Wall_ReliefStone_D");
		m_scene.addMesh(Circle_Paving_2_Mt_Wall_ReliefStone_D);
		Engine::Ref_ptr<Engine::mesh> Circle_Paving_3_Mt_Plant_GreenGrassField_B = Engine::mesh::create(Circle_Paving_3_Mt_Plant_GreenGrassField_B_Va, green, "Circle_Paving_3_Mt_Plant_GreenGrassField_B");
		m_scene.addMesh(Circle_Paving_3_Mt_Plant_GreenGrassField_B);
		Engine::Ref_ptr<Engine::mesh> Circle_Paving_4_10_Mt_Wall_FloorBrick_B = Engine::mesh::create(Circle_Paving_4_10_Mt_Wall_FloorBrick_B_Va, Mat_Wall_FloorBrick_A, "Circle_Paving_4_10_Mt_Wall_FloorBrick_B");
		m_scene.addMesh(Circle_Paving_4_10_Mt_Wall_FloorBrick_B);
		Engine::Ref_ptr<Engine::mesh> Crack_10_Mt_Wall_Crack_A = Engine::mesh::create(Crack_10_Mt_Wall_Crack_A_Va, Mat_CmnTex_Wall_Crack_A, "Crack_10_Mt_Wall_Crack_A");
		Crack_10_Mt_Wall_Crack_A->setPos({ 0.0f, 0.002f, 0.0f, 1.0f });//to at least avoid z-fighting when close to camera(still occurs when further away)
		m_scene.addMesh(Crack_10_Mt_Wall_Crack_A);
		Engine::Ref_ptr<Engine::mesh> Decoration_1_4_Mt_Wall_TOTTileCutsurface = Engine::mesh::create(Decoration_1_4_Mt_Wall_TOTTileCutsurface_Va, Mat_CmnTex_Wall_TOTTileCutsurface , "Decoration_1_4_Mt_Wall_TOTTileCutsurface");
		m_scene.addMesh(Decoration_1_4_Mt_Wall_TOTTileCutsurface);
		Engine::Ref_ptr<Engine::mesh> Fence_0_Mt_Stone_HyliaBridgeBreak_A = Engine::mesh::create(Fence_0_Mt_Stone_HyliaBridgeBreak_A_Va, Mat_CmnTex_Stone_HyliaBridgeBreak_A, "Fence_0_Mt_Stone_HyiaBridgeBreak_A");
		m_scene.addMesh(Fence_0_Mt_Stone_HyliaBridgeBreak_A);
		Engine::Ref_ptr<Engine::mesh> Main_Body_1_Mt_Wall_LargeBrick_A = Engine::mesh::create(Main_Body_1_Mt_Wall_LargeBrick_A_Va, Mat_CmnTex_Wall_LargeBrick_A, "Main_Body_1_Mt_Wall_LargeBrick_A");
		m_scene.addMesh(Main_Body_1_Mt_Wall_LargeBrick_A);
		Engine::Ref_ptr<Engine::mesh> Ornament_3_Mt_Builparts_Ornament_A = Engine::mesh::create(Ornament_3_Mt_Builparts_Ornament_A_Va, Mat_Builparts_Ornament_A, "Ornament_3_Mt_Builparts_Ornament_A");
		m_scene.addMesh(Ornament_3_Mt_Builparts_Ornament_A);
		Engine::Ref_ptr<Engine::mesh> Pedestral_Center_2_Mt_Wall_TempleOfTime_A = Engine::mesh::create(Pedestral_Center_2_Mt_Wall_TempleOfTime_A_Va, Mat_CmnTex_Wall_TempleOfTime_A, "Pedestral_Center_2_Mt_Wall_TempleOfTime_A");
		m_scene.addMesh(Pedestral_Center_2_Mt_Wall_TempleOfTime_A);
		Engine::Ref_ptr<Engine::mesh> Stone_Paving_1_Mt_Wall_StoneRoad_D = Engine::mesh::create(Stone_Paving_1_Mt_Wall_StoneRoad_D_Va, Mat_CmnTex_Wall_StoneRoad_D, "Stone_Paving_1_Mt_Wall_StoneRoad_D");
		m_scene.addMesh(Stone_Paving_1_Mt_Wall_StoneRoad_D);
		Engine::Ref_ptr<Engine::mesh> lightbulb = Engine::mesh::create(lightbulb_va, mat_emissive, "lightbulb");
		lightbulb->setScale(0.1f);
		m_scene.addMesh(lightbulb);

		Engine::PtrPtr<Engine::directionalLight> sun = Engine::Renderer::addDynamicDirLight({ vec3(0.0f, -0.7071067f, -0.7071067f), vec3(0.1f, 0.1f, 0.1f), vec3(2.5f, 2.5f, 2.5f), vec3(3.75f, 3.75f, 3.75f) });
		m_scene.addLight(sun);
		Engine::Ref_ptr<Engine::DirLightMovement> rotate = Engine::DirLightMovement::create(0.0174533f * 60.0f * 0.25f * 0.0f , 0.0174533f * 60.0f * 0.25f, 0.0f);
		rotate->attachToLight(sun);
		m_scene.addLightMovement(rotate);
	}

	void bloom_demo()
	{
		Engine::Ref_ptr<Engine::vertexArray> plane_va = Engine::vertexArray::create();
		plane_va->load("plane.model");
		plane_va->unbind();
		Engine::Ref_ptr<Engine::vertexArray> text_va = Engine::vertexArray::create();
		text_va->load("test_text.model");
		text_va->unbind();
		Engine::Ref_ptr<Engine::vertexArray> lightsaber_va = Engine::vertexArray::create();
		lightsaber_va->load("lightsaber.model");
		lightsaber_va->unbind();

		Engine::Ref_ptr<Engine::shader> color_shader = Engine::Renderer::getShaderLib()->load("deferred/geometry_pass/color.shader");
		color_shader->bindUniformBlock("ViewProjection", VIEWPROJ_BIND);
		Engine::Ref_ptr<Engine::shader> emissive_bloom = Engine::Renderer::getShaderLib()->load("emissive/emissive_bloom.shader");
		emissive_bloom->bindUniformBlock("ViewProjection", VIEWPROJ_BIND);

		Engine::Ref_ptr<Engine::material> plane_mat = Engine::material::create(color_shader, "planeMat", flags_default | flag_no_backface_cull);
		plane_mat->setUniform4f("alb", 0.1f, 0.1f, 0.1f, 1.0f);
		plane_mat->setUniform1f("ambCoefficient", 0.285f);
		plane_mat->setUniform1f("specCoefficient", 0.8f);
		plane_mat->setUniform1f("shininess", 100.0f);
		plane_mat->setUniform1f("emissiveMultiplier", 0.0f);
		plane_mat->flushAll();
		m_scene.addMaterial(plane_mat);

		Engine::Ref_ptr<Engine::material> mat_emissive = Engine::material::create(emissive_bloom, "emissive", flag_depth_test | flag_no_deferred);
		mat_emissive->setUniform4f("emissiveColor", { 1.0f, 0.95f, 0.05f, 1.0f });
		mat_emissive->setUniform1f("multiplier", 100.0f);
		mat_emissive->flushAll();
		m_scene.addMaterial(mat_emissive);

		Engine::Ref_ptr<Engine::material> lightsaber_mat = Engine::material::create(emissive_bloom, "lightsaber_mat", flag_depth_test | flag_no_deferred);
		lightsaber_mat->setUniform4f("emissiveColor", { 0.05f, 1.0f, 0.05f, 1.0f });
		lightsaber_mat->setUniform1f("multiplier", 100.0f);
		lightsaber_mat->flushAll();
		m_scene.addMaterial(lightsaber_mat);

		Engine::Ref_ptr<Engine::mesh> plane = Engine::mesh::create(plane_va, plane_mat, "plane");
		m_scene.addMesh(plane);
		Engine::Ref_ptr<Engine::mesh> text = Engine::mesh::create(text_va, mat_emissive, "text");
		text->setScale(2.0f);
		text->setRot({ 3.1415926f / 2.0f, 0.0f, 0.0f });
		text->setPos({ 0.0f, 6.0f, -5.5f, 1.0f });
		m_scene.addMesh(text);
		Engine::Ref_ptr<Engine::mesh> lightsaber = Engine::mesh::create(lightsaber_va, lightsaber_mat, "lightsaber");
		lightsaber->setPos({ -2.0f, 5.0f, -5.5f, 1.0f });
		m_scene.addMesh(lightsaber);

		Engine::PtrPtr<Engine::directionalLight> sun = Engine::Renderer::addDynamicDirLight({ vec3(0.0f, -0.7071067f, -0.7071067f), vec3(0.1f, 0.1f, 0.1f), vec3(10.0f, 10.0f, 10.0f), vec3(100.0f, 100.0f, 100.0f) });
		m_scene.addLight(sun);
	}

	void newMatProperties()
	{
		Engine::Ref_ptr<Engine::vertexArray> plane_va = Engine::vertexArray::create();
		plane_va->load("plane.model");
		plane_va->unbind();
		Engine::Ref_ptr<Engine::vertexArray> text_va = Engine::vertexArray::create();
		text_va->load("test_text.model");
		text_va->unbind();
		Engine::Ref_ptr<Engine::vertexArray> lightsaber_va = Engine::vertexArray::create();
		lightsaber_va->load("lightsaber.model");
		lightsaber_va->unbind();
		Engine::Ref_ptr<Engine::vertexArray> sphere_va = Engine::vertexArray::create();
		sphere_va->load("sphere.model");
		sphere_va->unbind();

		Engine::Ref_ptr<Engine::shader> color_shader = Engine::Renderer::getShaderLib()->load("deferred/geometry_pass/color.shader");
		color_shader->bindUniformBlock("ViewProjection", VIEWPROJ_BIND);
		Engine::Ref_ptr<Engine::shader> emissive_bloom = Engine::Renderer::getShaderLib()->load("deferred/geometry_pass/emissive.shader");
		emissive_bloom->bindUniformBlock("ViewProjection", VIEWPROJ_BIND);

		Engine::Ref_ptr<Engine::material> plane_mat = Engine::material::create(color_shader, "planeMat", flags_default | flag_no_backface_cull);
		plane_mat->setUniform4f("alb", 0.1f, 0.1f, 0.1f, 1.0f);
		plane_mat->setUniform1f("ambCoefficient", 0.285f);
		plane_mat->setUniform1f("specCoefficient", 0.8f);
		plane_mat->setUniform1f("shininess", 100.0f);
		plane_mat->flushAll();
		m_scene.addMaterial(plane_mat);

		Engine::Ref_ptr<Engine::material> mat_emissive = Engine::material::create(emissive_bloom, "emissive", flag_depth_test | flag_no_deferred);
		mat_emissive->setUniform4f("emissiveColor", { 1.0f, 0.95f, 0.05f, 1.0f });
		mat_emissive->setUniform1f("multiplier", 100.0f);
		mat_emissive->flushAll();
		m_scene.addMaterial(mat_emissive);

		Engine::Ref_ptr<Engine::material> lightsaber_mat = Engine::material::create(emissive_bloom, "lightsaber_mat", flag_depth_test | flag_no_deferred);
		lightsaber_mat->setUniform4f("emissiveColor", { 0.05f, 1.0f, 0.05f, 1.0f });
		lightsaber_mat->setUniform1f("multiplier", 100.0f);
		lightsaber_mat->flushAll();
		m_scene.addMaterial(lightsaber_mat);

		Engine::Ref_ptr<Engine::mesh> plane = Engine::mesh::create(plane_va, plane_mat, "plane");
		m_scene.addMesh(plane);
		Engine::Ref_ptr<Engine::mesh> text = Engine::mesh::create(text_va, mat_emissive, "text");
		text->setScale(2.0f);
		text->setRot({ 3.1415926f / 2.0f, 0.0f, 0.0f });
		text->setPos({ 0.0f, 6.0f, -5.5f, 1.0f });
		m_scene.addMesh(text);
		Engine::Ref_ptr<Engine::mesh> lightsaber = Engine::mesh::create(lightsaber_va, lightsaber_mat, "lightsaber");
		lightsaber->setPos({ -2.0f, 5.0f, -5.5f, 1.0f });
		m_scene.addMesh(lightsaber);

		Engine::PtrPtr<Engine::directionalLight> sun = Engine::Renderer::addDynamicDirLight({ vec3(0.0f, -0.7071067f, -0.7071067f), vec3(0.1f, 0.1f, 0.1f), vec3(10.0f, 10.0f, 10.0f), vec3(100.0f, 100.0f, 100.0f) });
		m_scene.addLight(sun);
	}

private:
	inline Engine::Scene* getScene() { return &m_scene; }
	Engine::Scene m_scene;
	bool m_sceneLoaded = false;
	bool m_showNodeEditor = false;
};

class Sandbox : public Engine::Application
{
public:
	Sandbox()
	{
		SandboxLayer* sandboxLayer = new SandboxLayer();
		pushLayer(sandboxLayer);
		pushOverlay(new Engine::NodeEditorLayer(sandboxLayer->getScene()));
	}
	~Sandbox()
	{
		
	}

};

Engine::Application* Engine::CreateApplication()
{
	return new Sandbox;
}