#include <Engine.hpp>
#include "imgui/imgui.h"

#define PI 3.1415926f

class SandboxLayer : public Engine::layer
{
	friend class Sandbox;
public:
	SandboxLayer(const float hFov, const float zNear, const float zFar)
		: layer("RenderLayer"), m_scene(hFov, zNear, zFar, static_cast<float>(Engine::Application::Get().getWindow().getWidth() / static_cast<float>(Engine::Application::Get().getWindow().getHeight())))
	{
		Engine::Application::Get().getWindow().setDissableCursor(false);
		Engine::Application::Get().getWindow().setFullscreen(false);
	}
	~SandboxLayer()
	{
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
				Engine::Renderer::onImGuiRender();
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
			m_scene.initialize(90.0f, 0.1f, 1000.0f, static_cast<float>(Engine::Application::Get().getWindow().getWidth() / static_cast<float>(Engine::Application::Get().getWindow().getHeight())));
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
		if(ImGui::Button("plane_head_rotate"))
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
		else if (ImGui::Button("test"))
		{
			test();
			m_sceneLoaded = true;
		}
		ImGui::End();
	}
	//scenes
	void plane_head_rotate()
	{
		Engine::Ref_ptr<Engine::vertexArray> head_va = Engine::vertexArray::create();
		head_va->load("head.model");
		head_va->unbind();//make sure to unbind the vertexArray before loading the next one, otherwise we would get interfearences with layout & indexBuffer(I assume)
		Engine::Ref_ptr<Engine::vertexArray> plane_va = Engine::vertexArray::create();
		plane_va->load("plane.model");
		plane_va->unbind();
		Engine::Ref_ptr<Engine::vertexArray> helmet_va = Engine::vertexArray::create();
		helmet_va->load("helmet_tb.model");
		helmet_va->unbind();
		Engine::Ref_ptr<Engine::vertexArray> cube_va = Engine::vertexArray::create();
		cube_va->load("cube.model");
		cube_va->unbind();

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

		Engine::Ref_ptr<Engine::shader> lightTexShader_dir_normalMap = Engine::Renderer::getShaderLib()->load("additive_w_shadow/dir/normal_mapping/basicPhong_one_texture_shadow_additive_dir.shader");
		lightTexShader_dir_normalMap->bindUniformBlock("ViewProjection", VIEWPROJ_BIND);
		lightTexShader_dir_normalMap->bindUniformBlock("directionalLights", DIRECTIONAL_LIGHTS_BIND);
		lightTexShader_dir_normalMap->bindUniformBlock("directionalLights_v", DIRECTIONAL_LIGHTS_BIND);

		Engine::Ref_ptr<Engine::shader> lightTexShader_point_normalMap = Engine::Renderer::getShaderLib()->load("additive_w_shadow/point/normal_mapping/basicPhong_one_texture_shadow_additive_point.shader");
		lightTexShader_point_normalMap->bindUniformBlock("ViewProjection", VIEWPROJ_BIND);
		lightTexShader_point_normalMap->bindUniformBlock("pointLights", POINT_LIGHTS_BIND);

		Engine::Ref_ptr<Engine::shader> lightTexShader_spot_normalMap = Engine::Renderer::getShaderLib()->load("additive_w_shadow/spot/normal_mapping/basicPhong_one_texture_shadow_additive_spot.shader");
		lightTexShader_spot_normalMap->bindUniformBlock("ViewProjection", VIEWPROJ_BIND);
		lightTexShader_spot_normalMap->bindUniformBlock("spotLights", SPOT_LIGHTS_BIND);
		lightTexShader_spot_normalMap->bindUniformBlock("spotLights_v", SPOT_LIGHTS_BIND);

		Engine::Ref_ptr<Engine::texture2d> headTex = Engine::texture2d::create("head.png", true);
		Engine::Ref_ptr<Engine::texture2d> helmetTex = Engine::texture2d::create("armor_115_head_alb.png", true);
		Engine::Ref_ptr<Engine::texture2d> helmetNormalTex = Engine::texture2d::create("armor_115_head_nrm.png");
		Engine::Ref_ptr<Engine::texture2d> cubeTex = Engine::texture2d::create("checkerboard.png", true);

		Engine::Ref_ptr<Engine::material> red = Engine::material::create(lightShader_dir, "red", flags_default | flag_no_backface_cull);//just use the variant for directional lights for initialization
		red->addShader(lightShader_point);
		red->addShader(lightShader_spot);
		red->setUniform4f("amb", 0.2f, 0.025f, 0.025f, 1.0f);
		red->setUniform4f("diff", 0.7f, 0.0875f, 0.0875f, 1.0f);
		red->setUniform4f("spec", 0.8f, 0.1f, 0.1f, 1.0f);
		red->setUniform1f("shininess", 100.0f);
		red->flushAll();
		m_scene.addMaterial(red);

		Engine::Ref_ptr<Engine::material> headMat = Engine::material::create(lightTexShader_dir, "headMat");//just use the variant for directional lights for initialization
		headMat->addShader(lightTexShader_point);
		headMat->addShader(lightTexShader_spot);
		headMat->setTexture("u_texture", headTex);
		m_scene.addMaterial(headMat);

		Engine::Ref_ptr<Engine::material> helmetMat = Engine::material::create(lightTexShader_dir_normalMap, "helmetMat");
		helmetMat->addShader(lightTexShader_point_normalMap);
		helmetMat->addShader(lightTexShader_spot_normalMap);
		helmetMat->setTexture("u_texture", helmetTex);
		helmetMat->setTexture("u_normalMap", helmetNormalTex);
		m_scene.addMaterial(helmetMat);

		Engine::Ref_ptr<Engine::material> cubeMat = Engine::material::create(lightTexShader_dir, "cubeMat");
		cubeMat->addShader(lightTexShader_point);
		cubeMat->addShader(lightTexShader_spot);
		cubeMat->setTexture("u_texture", cubeTex);
		m_scene.addMaterial(cubeMat);

		Engine::Ref_ptr<Engine::mesh> head = Engine::mesh::create(head_va, headMat, "head");
		head->setPos({ 0.0f, 3.0f, 0.0f, 1.0f });
		head->setRot({ 0.0f, 3.1415926f, 0.0f });//rotate 180 degrees(pi in radians)
		head->setScale(0.1f);
		head->attachMovement(std::static_pointer_cast<Engine::MeshMovement, Engine::CircularMeshMovement>(std::make_shared<Engine::CircularMeshMovement>(vec3(1.0f, 1.0f, 1.0f), vec3(0.5773502f, 0.5773502f, 0.5773502f), 0.0174533f * 60.0f)));
		m_scene.addMesh(head);

		Engine::Ref_ptr<Engine::mesh> plane = Engine::mesh::create(plane_va, red, "plane");
		plane->setScale(10.0f);
		plane->setPos({ 0.0f, 0.0f, 0.0f, 1.0f });
		m_scene.addMesh(plane);

		Engine::Ref_ptr<Engine::mesh> helmet = Engine::mesh::create(helmet_va, helmetMat, "helmet");
		helmet->setPos({ -6.0f, 3.0f, 5.0f, 1.0f });
		helmet->setScale(5.0f);
		m_scene.addMesh(helmet);

		Engine::Ref_ptr<Engine::mesh> cube = Engine::mesh::create(cube_va, cubeMat, "cube");
		cube->setScale(0.1f);
		cube->setPos({0.0f, 1.0f, 8.0f, 1.0f});
		m_scene.addMesh(cube);

		Engine::PtrPtr<Engine::directionalLight> sun(Engine::Renderer::addDynamicDirLight({ vec3(0.0f, -0.7071067f, 0.7071067f), vec3(0.1f, 0.1f, 0.1f), vec3(0.5f, 0.5f, 0.5f), vec3(0.5f, 0.5f, 0.5f) } ));
		m_scene.addLight(sun);
		Engine::Ref_ptr<Engine::DirLightMovement> rotate = Engine::DirLightMovement::create(0.0174533f * 60.0f * 0.25f, 0.0f, 0.0f);
		rotate->attachToLight(sun);
		m_scene.addLightMovement(rotate);
		//m_moon = Engine::Renderer::addStaticDirLight({ vec3(0.0f, -0.7071067f, -0.7071067f), vec3(1.0f, 1.0f, 1.0f), vec3(1.0f, 1.0f, 1.0f), vec3(1.0f, 1.0f, 1.0f) } ); <-doesn't work because of the lookAt-function edge-case
		//m_sun_2 = Engine::Renderer::addStaticDirLight({ vec3(-0.5773502f, -0.5773502, -0.5773502f), vec3(1.0f, 1.0f, 1.0f), vec3(1.0f, 1.0f, 1.0f), vec3(1.0f, 1.0f, 1.0f) });
		Engine::PtrPtr<Engine::pointLight> lamp = Engine::Renderer::addDynamicPointLight({ vec3(2.0f, 1.0f, 0.0f), vec3(0.1f, 0.02f, 0.02f), vec3(0.5f, 0.1f, 0.1f), vec3(0.5f, 0.1f, 0.1f), 1.0f, 0.01f, 0.01f });
		m_scene.addLight(lamp);
		Engine::Ref_ptr<Engine::PointLightMovement> move_circular = Engine::CircularPointLightMovement::create(vec3(0.0f, 1.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f), 0.0174533f * 60.0f);
		move_circular->attachToLight(lamp);
		m_scene.addLightMovement(move_circular);
		Engine::PtrPtr<Engine::spotLight> spotLight =  Engine::Renderer::addDynamicSpotLight({ vec3(-6.0f, 8.0f, 4.75f), vec3(0.0f, -1.0f, 0.0f), vec3(0.05f, 0.05f, 0.5f), vec3(0.1f, 0.1f, 1.0f), vec3(0.1f, 0.1f, 1.0f), cosf(15.0f * (PI / 180.0f)) });
		m_scene.addLight(spotLight);
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

		Engine::Ref_ptr<Engine::shader> emissive_bloom = Engine::Renderer::getShaderLib()->load("emissive/emissive_bloom.shader");
		emissive_bloom->bindUniformBlock("ViewProjection", VIEWPROJ_BIND);

		Engine::Ref_ptr<Engine::material> light_grey = Engine::material::create(lightShader_dir, "light_grey");//just use the variant for directional lights for initialization
		light_grey->addShader(lightShader_point);
		light_grey->addShader(lightShader_spot);
		light_grey->setUniform4f("amb", 0.1f, 0.1f, 0.1f, 1.0f);
		light_grey->setUniform4f("diff", 0.6f, 0.6f, 0.6f, 1.0f);
		light_grey->setUniform4f("spec", 0.6f, 0.6f, 0.6f, 1.0f);
		light_grey->setUniform1f("shininess", 50.0f);
		light_grey->flushAll();
		m_scene.addMaterial(light_grey);

		Engine::Ref_ptr<Engine::material> water_mat = Engine::material::create(lightShader_dir, "water_blue");//just use the variant for directional lights for initialization
		water_mat->addShader(lightShader_point);
		water_mat->addShader(lightShader_spot);
		water_mat->setUniform4f("amb", 0.1f, 0.1f, 0.8f, 1.0f);
		water_mat->setUniform4f("diff", 0.1f, 0.1f, 0.8f, 1.0f);
		water_mat->setUniform4f("spec", 0.1f, 0.1f, 0.8f, 1.0f);
		water_mat->setUniform1f("shininess", 25.0f);
		water_mat->flushAll();
		m_scene.addMaterial(water_mat);

		Engine::Ref_ptr<Engine::material> lighthouse_mat = Engine::material::create(lightTexShader_dir, "lighthouse_mat");//just use the variant for directional lights for initialization
		lighthouse_mat->addShader(lightTexShader_point);
		lighthouse_mat->addShader(lightTexShader_spot);
		lighthouse_mat->setTexture("u_texture", lighthouse_tex);
		m_scene.addMaterial(lighthouse_mat);

		Engine::Ref_ptr<Engine::material> boat_mat = Engine::material::create(lightShader_dir, "boat_brown");//just use the variant for directional lights for initialization
		boat_mat->addShader(lightShader_point);
		boat_mat->addShader(lightShader_spot);
		boat_mat->setUniform4f("amb", 0.5451f, 0.2706f, 0.07451f, 1.0f);
		boat_mat->setUniform4f("diff", 0.5451f, 0.2706f, 0.07451f, 1.0f);
		boat_mat->setUniform4f("spec", 0.5451f, 0.2760f, 0.07451f, 1.0f);
		boat_mat->setUniform1f("shininess", 45.0f);
		boat_mat->flushAll();
		m_scene.addMaterial(boat_mat);

		Engine::Ref_ptr<Engine::material> figureMat = Engine::material::create(lightTexShader_dir, "figureMat");//just use the variant for directional lights for initialization
		figureMat->addShader(lightTexShader_point);
		figureMat->addShader(lightTexShader_spot);
		figureMat->setTexture("u_texture", headTex);
		m_scene.addMaterial(figureMat);

		Engine::Ref_ptr<Engine::material> mat_emissive = Engine::material::create(emissive_bloom, "emissive", flag_depth_test);
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

		Engine::Ref_ptr<Engine::material> light_grey = Engine::material::create(lightShader_dir, "light_grey");//just use the variant for directional lights for initialization
		light_grey->addShader(lightShader_point);
		light_grey->addShader(lightShader_spot);
		light_grey->setUniform4f("amb", 0.1f, 0.1f, 0.1f, 1.0f);
		light_grey->setUniform4f("diff", 0.6f, 0.6f, 0.6f, 1.0f);
		light_grey->setUniform4f("spec", 0.6f, 0.6f, 0.6f, 1.0f);
		light_grey->setUniform1f("shininess", 50.0f);
		light_grey->flushAll();
		m_scene.addMaterial(light_grey);

		Engine::Ref_ptr<Engine::material> black = Engine::material::create(lightShader_dir, "black");//just use the variant for directional lights for initialization
		black->addShader(lightShader_point);
		black->addShader(lightShader_spot);
		black->setUniform4f("amb", 0.01f, 0.01f, 0.01f, 1.0f);
		black->setUniform4f("diff", 0.06f, 0.06f, 0.06f, 1.0f);
		black->setUniform4f("spec", 0.1f, 0.1f, 0.1f, 1.0f);
		black->setUniform1f("shininess", 100.0f);
		black->flushAll();
		m_scene.addMaterial(black);

		Engine::Ref_ptr<Engine::material> yellow = Engine::material::create(lightShader_dir, "yellow");//just use the variant for directional lights for initialization
		yellow->addShader(lightShader_point);
		yellow->addShader(lightShader_spot);
		yellow->setUniform4f("amb", 0.08f, 0.08f, 0.01f, 1.0f);
		yellow->setUniform4f("diff", 0.8f, 0.8f, 0.1f, 1.0f);
		yellow->setUniform4f("spec", 1.0f, 1.0f, 0.1f, 1.0f);
		yellow->setUniform1f("shininess", 80.0f);
		yellow->flushAll();
		m_scene.addMaterial(yellow);

		Engine::Ref_ptr<Engine::material> spot_light_mat = Engine::material::create(lightTexShader_dir, "spot_light_mat");
		spot_light_mat->addShader(lightTexShader_point);
		spot_light_mat->addShader(lightTexShader_spot);
		spot_light_mat->setTexture("u_texture", spot_light_tex);
		m_scene.addMaterial(spot_light_mat);

		Engine::Ref_ptr<Engine::material> duck_mat = Engine::material::create(lightTexShader_dir, "duck_mat");
		duck_mat->addShader(lightTexShader_point);
		duck_mat->addShader(lightTexShader_spot);
		duck_mat->setTexture("u_texture", duck_tex);
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

		Engine::Ref_ptr<Engine::shader> lightTexShader_dir_normalMap = Engine::Renderer::getShaderLib()->load("additive_w_shadow/dir/normal_mapping/basicPhong_one_texture_shadow_additive_dir.shader");
		lightTexShader_dir_normalMap->bindUniformBlock("ViewProjection", VIEWPROJ_BIND);
		lightTexShader_dir_normalMap->bindUniformBlock("directionalLights", DIRECTIONAL_LIGHTS_BIND);
		lightTexShader_dir_normalMap->bindUniformBlock("directionalLights_v", DIRECTIONAL_LIGHTS_BIND);

		Engine::Ref_ptr<Engine::shader> lightTexShader_point_normalMap = Engine::Renderer::getShaderLib()->load("additive_w_shadow/point/normal_mapping/basicPhong_one_texture_shadow_additive_point.shader");
		lightTexShader_point_normalMap->bindUniformBlock("ViewProjection", VIEWPROJ_BIND);
		lightTexShader_point_normalMap->bindUniformBlock("pointLights", POINT_LIGHTS_BIND);

		Engine::Ref_ptr<Engine::shader> lightTexShader_spot_normalMap = Engine::Renderer::getShaderLib()->load("additive_w_shadow/spot/normal_mapping/basicPhong_one_texture_shadow_additive_spot.shader");
		lightTexShader_spot_normalMap->bindUniformBlock("ViewProjection", VIEWPROJ_BIND);
		lightTexShader_spot_normalMap->bindUniformBlock("spotLights", SPOT_LIGHTS_BIND);
		lightTexShader_spot_normalMap->bindUniformBlock("spotLights_v", SPOT_LIGHTS_BIND);

		Engine::Ref_ptr<Engine::material> gunMat = Engine::material::create(lightTexShader_dir_normalMap, "gun_mat");//just use the variant for directional lights for initialization
		gunMat->addShader(lightTexShader_point_normalMap);
		gunMat->addShader(lightTexShader_spot_normalMap);
		gunMat->setTexture("u_texture", gunDiffTex);
		gunMat->setTexture("u_normalMap", gunNormalTex);
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
		Engine::Ref_ptr<Engine::shader> lightTexShader_dir_normalMap = Engine::Renderer::getShaderLib()->load("additive_w_shadow/dir/normal_mapping/basicPhong_one_texture_shadow_additive_dir.shader");
		lightTexShader_dir_normalMap->bindUniformBlock("ViewProjection", VIEWPROJ_BIND);
		lightTexShader_dir_normalMap->bindUniformBlock("directionalLights", DIRECTIONAL_LIGHTS_BIND);
		lightTexShader_dir_normalMap->bindUniformBlock("directionalLights_v", DIRECTIONAL_LIGHTS_BIND);

		Engine::Ref_ptr<Engine::shader> lightTexShader_point_normalMap = Engine::Renderer::getShaderLib()->load("additive_w_shadow/point/normal_mapping/basicPhong_one_texture_shadow_additive_point.shader");
		lightTexShader_point_normalMap->bindUniformBlock("ViewProjection", VIEWPROJ_BIND);
		lightTexShader_point_normalMap->bindUniformBlock("pointLights", POINT_LIGHTS_BIND);

		Engine::Ref_ptr<Engine::shader> lightTexShader_spot_normalMap = Engine::Renderer::getShaderLib()->load("additive_w_shadow/spot/normal_mapping/basicPhong_one_texture_shadow_additive_spot.shader");
		lightTexShader_spot_normalMap->bindUniformBlock("ViewProjection", VIEWPROJ_BIND);
		lightTexShader_spot_normalMap->bindUniformBlock("spotLights", SPOT_LIGHTS_BIND);
		lightTexShader_spot_normalMap->bindUniformBlock("spotLights_v", SPOT_LIGHTS_BIND);

		Engine::Ref_ptr<Engine::shader> lightShader_dir = Engine::Renderer::getShaderLib()->load("additive_w_shadow/dir/basicPhong_color_shadow_additive_dir.shader");
		lightShader_dir->bindUniformBlock("ViewProjection", VIEWPROJ_BIND);
		lightShader_dir->bindUniformBlock("directionalLights", DIRECTIONAL_LIGHTS_BIND);
		lightShader_dir->bindUniformBlock("directionalLights_v", DIRECTIONAL_LIGHTS_BIND);

		Engine::Ref_ptr<Engine::shader> lightShader_point = Engine::Renderer::getShaderLib()->load("additive_w_shadow/point/basicPhong_color_shadow_additive_point.shader");
		lightShader_point->bindUniformBlock("ViewProjection", VIEWPROJ_BIND);
		lightShader_point->bindUniformBlock("pointLights", POINT_LIGHTS_BIND);

		Engine::Ref_ptr<Engine::shader> lightShader_spot = Engine::Renderer::getShaderLib()->load("additive_w_shadow/spot/basicPhong_color_shadow_additive_spot.shader");
		lightShader_spot->bindUniformBlock("ViewProjection", VIEWPROJ_BIND);
		lightShader_spot->bindUniformBlock("spotLights", POINT_LIGHTS_BIND);
		lightShader_spot->bindUniformBlock("spotLights_v", DIRECTIONAL_LIGHTS_BIND);

		Engine::Ref_ptr<Engine::shader> emissive_bloom = Engine::Renderer::getShaderLib()->load("emissive/emissive_bloom.shader");
		emissive_bloom->bindUniformBlock("ViewProjection", VIEWPROJ_BIND);

		//create materials
		Engine::Ref_ptr<Engine::material> Mat_Rock_RuinStoneBridge_A = Engine::material::create(lightTexShader_dir_normalMap, "Rock_RuinStoneBridge_A");
		Mat_Rock_RuinStoneBridge_A->addShader(lightTexShader_point_normalMap);
		Mat_Rock_RuinStoneBridge_A->addShader(lightTexShader_spot_normalMap);
		Mat_Rock_RuinStoneBridge_A->setTexture("u_texture", Rock_RuinStoneBridge_A_Alb);
		Mat_Rock_RuinStoneBridge_A->setTexture("u_normalMap", Rock_RuinStoneBridge_A_Nrm);
		m_scene.addMaterial(Mat_Rock_RuinStoneBridge_A);
		Engine::Ref_ptr<Engine::material> Mat_Rock_RuinStoneBridge_B = Engine::material::create(lightTexShader_dir_normalMap, "Rock_RuinStoneBridge_B");
		Mat_Rock_RuinStoneBridge_B->addShader(lightTexShader_point_normalMap);
		Mat_Rock_RuinStoneBridge_B->addShader(lightTexShader_spot_normalMap);
		Mat_Rock_RuinStoneBridge_B->setTexture("u_texture", Rock_RuinStoneBridge_B_Alb);
		Mat_Rock_RuinStoneBridge_B->setTexture("u_normalMap", Rock_RuinStoneBridge_B_Nrm);
		m_scene.addMaterial(Mat_Rock_RuinStoneBridge_B);
		Engine::Ref_ptr<Engine::material> Mat_Rock_RuinStoneWall_A = Engine::material::create(lightTexShader_dir_normalMap, "Rock_RuinStoneWall_A");
		Mat_Rock_RuinStoneWall_A->addShader(lightTexShader_point_normalMap);
		Mat_Rock_RuinStoneWall_A->addShader(lightTexShader_spot_normalMap);
		Mat_Rock_RuinStoneWall_A->setTexture("u_texture", Rock_RuinStoneWall_A_Alb);
		Mat_Rock_RuinStoneWall_A->setTexture("u_normalMap", Rock_RuinStoneWall_A_Nrm);
		m_scene.addMaterial(Mat_Rock_RuinStoneWall_A);
		Engine::Ref_ptr<Engine::material> Mat_Rock_RuinStoneWall_C = Engine::material::create(lightTexShader_dir_normalMap, "Rock_RuinStoneWall_C");
		Mat_Rock_RuinStoneWall_C->addShader(lightTexShader_point_normalMap);
		Mat_Rock_RuinStoneWall_C->addShader(lightTexShader_spot_normalMap);
		Mat_Rock_RuinStoneWall_C->setTexture("u_texture", Rock_RuinStoneWall_C_Alb);
		Mat_Rock_RuinStoneWall_C->setTexture("u_normalMap", Rock_RuinStoneWall_C_Nrm);
		m_scene.addMaterial(Mat_Rock_RuinStoneWall_C);
		Engine::Ref_ptr<Engine::material> Mat_Rock_RuinStoneWall_D = Engine::material::create(lightTexShader_dir_normalMap, "Rock_RuinStoneWall_D");
		Mat_Rock_RuinStoneWall_D->addShader(lightTexShader_point_normalMap);
		Mat_Rock_RuinStoneWall_D->addShader(lightTexShader_spot_normalMap);
		Mat_Rock_RuinStoneWall_D->setTexture("u_texture", Rock_RuinStoneWall_D_Alb);
		Mat_Rock_RuinStoneWall_D->setTexture("u_normalMap", Rock_RuinStoneWall_D_Nrm);
		m_scene.addMaterial(Mat_Rock_RuinStoneWall_D);
		Engine::Ref_ptr<Engine::material> green = Engine::material::create(lightShader_dir, "green");
		green->addShader(lightShader_point);
		green->addShader(lightShader_spot);
		green->setUniform4f("amb", 0.05f, 0.2f, 0.05f, 1.0f);
		green->setUniform4f("diff", 0.2f, 0.8f, 0.2f, 1.0f);
		green->setUniform4f("spec", 0.2f, 0.8f, 0.2f, 1.0f);
		green->setUniform1f("shininess", 100.0f);
		green->flushAll();
		m_scene.addMaterial(green);
		Engine::Ref_ptr<Engine::material> Mat_Boar = Engine::material::create(lightTexShader_dir_normalMap, "Boar");
		Mat_Boar->addShader(lightTexShader_point_normalMap);
		Mat_Boar->addShader(lightTexShader_spot_normalMap);
		Mat_Boar->setTexture("u_texture", Boar_Alb);
		Mat_Boar->setTexture("u_normalMap", Boar_Nrm);
		m_scene.addMaterial(Mat_Boar);
		Engine::Ref_ptr<Engine::material> Mat_Deer = Engine::material::create(lightTexShader_dir_normalMap, "Deer");
		Mat_Deer->addShader(lightTexShader_point_normalMap);
		Mat_Deer->addShader(lightTexShader_spot_normalMap);
		Mat_Deer->setTexture("u_texture", Deer_Body_Alb);
		Mat_Deer->setTexture("u_normalMap", Deer_Body_Nrm);
		m_scene.addMaterial(Mat_Deer);

		Engine::Ref_ptr<Engine::material> mat_emissive = Engine::material::create(emissive_bloom, "emissive", flag_depth_test);
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

		Engine::Ref_ptr<Engine::shader> lightTexShader_dir_normalMap = Engine::Renderer::getShaderLib()->load("additive_w_shadow/dir/nrm_map_spec_map_bloom/basicPhong_one_texture_shadow_additive_dir.shader");
		lightTexShader_dir_normalMap->bindUniformBlock("ViewProjection", VIEWPROJ_BIND);
		lightTexShader_dir_normalMap->bindUniformBlock("directionalLights", DIRECTIONAL_LIGHTS_BIND);
		lightTexShader_dir_normalMap->bindUniformBlock("directionalLights_v", DIRECTIONAL_LIGHTS_BIND);

		Engine::Ref_ptr<Engine::shader> lightTexShader_point_normalMap = Engine::Renderer::getShaderLib()->load("additive_w_shadow/point/nrm_map_spec_map_bloom/basicPhong_one_texture_shadow_additive_point.shader");
		lightTexShader_point_normalMap->bindUniformBlock("ViewProjection", VIEWPROJ_BIND);
		lightTexShader_point_normalMap->bindUniformBlock("pointLights", POINT_LIGHTS_BIND);

		Engine::Ref_ptr<Engine::shader> lightTexShader_spot_normalMap = Engine::Renderer::getShaderLib()->load("additive_w_shadow/spot/nrm_map_spec_map_bloom/basicPhong_one_texture_shadow_additive_spot.shader");
		lightTexShader_spot_normalMap->bindUniformBlock("ViewProjection", VIEWPROJ_BIND);
		lightTexShader_spot_normalMap->bindUniformBlock("spotLights", SPOT_LIGHTS_BIND);
		lightTexShader_spot_normalMap->bindUniformBlock("spotLights_v", SPOT_LIGHTS_BIND);

		Engine::Ref_ptr<Engine::material> gunMat = Engine::material::create(lightTexShader_dir_normalMap, "gun_mat");//just use the variant for directional lights for initialization
		gunMat->addShader(lightTexShader_point_normalMap);
		gunMat->addShader(lightTexShader_spot_normalMap);
		gunMat->setTexture("u_texture", gunDiffTex);
		gunMat->setTexture("u_normalMap", gunNormalTex);
		gunMat->setTexture("u_specMap", gunSpecTex);
		m_scene.addMaterial(gunMat);

		Engine::Ref_ptr<Engine::mesh> gun = Engine::mesh::create(gun_va, gunMat, "gun");
		m_scene.addMesh(gun);

		Engine::PtrPtr<Engine::directionalLight> sun = Engine::Renderer::addDynamicDirLight({ vec3(0.0f, -0.7071067f, -0.7071067f), vec3(0.1f, 0.1f, 0.1f), vec3(2.5f, 2.5f, 2.5f), vec3(3.75f, 3.75f, 3.75f) });
		m_scene.addLight(sun);
		//Engine::PtrPtr<Engine::pointLight> lamp = Engine::Renderer::addDynamicPointLight({ vec3(0.0f, 0.0f, 0.0f), vec3(0.1f, 0.1f, 0.1f), vec3(0.6f, 0.6f, 0.6f), vec3(0.9f, 0.9f, 0.9f), 1.0f, 0.00f, 1.0f });//because we use gamma-correction, only use the quadratic term
		//m_scene.addLight(lamp);
	}

	void emissive()
	{
		Engine::Ref_ptr<Engine::vertexArray> environment_va = Engine::vertexArray::create();
		environment_va->load("cube_solid.model");
		environment_va->unbind();
		Engine::Ref_ptr<Engine::vertexArray> cube_lit_va = Engine::vertexArray::create();
		cube_lit_va->load("lightbulb.model");
		cube_lit_va->unbind();

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
			
		Engine::Ref_ptr<Engine::shader> emissive_bloom = Engine::Renderer::getShaderLib()->load("emissive/emissive_bloom.shader");
		emissive_bloom->bindUniformBlock("ViewProjection", VIEWPROJ_BIND);

		Engine::Ref_ptr<Engine::material> light_grey = Engine::material::create(lightShader_dir, "light_grey");//just use the variant for directional lights for initialization
		light_grey->addShader(lightShader_point);
		light_grey->addShader(lightShader_spot);
		light_grey->setUniform4f("amb", 0.1f, 0.1f, 0.1f, 1.0f);
		light_grey->setUniform4f("diff", 0.6f, 0.6f, 0.6f, 1.0f);
		light_grey->setUniform4f("spec", 0.6f, 0.6f, 0.6f, 1.0f);
		light_grey->setUniform1f("shininess", 50.0f);
		light_grey->flushAll();
		m_scene.addMaterial(light_grey);

		Engine::Ref_ptr<Engine::material> mat_emissive = Engine::material::create(emissive_bloom, "emissive", flag_depth_test);
		mat_emissive->setUniform4f("emissiveColor", { 0.1f, 0.1f, 1.0f, 1.0f });
		mat_emissive->setUniform1f("multiplier", 5.0f);
		mat_emissive->flushAll();
		//m_scene.addMaterial(mat_emissive);

		Engine::Ref_ptr<Engine::mesh> environment = Engine::mesh::create(environment_va, light_grey, "environment");
		environment->setScale(11.1111111f);
		m_scene.addMesh(environment);

		Engine::PtrPtr<Engine::pointLight> lamp(Engine::Renderer::addDynamicPointLight({ vec3(0.0f, 0.0f, 0.0f), vec3(0.1f, 0.1f, 1.0f), vec3(0.1f, 0.1f, 1.0f), vec3(0.1f, 0.1f, 1.0f), 1.0f, 0.01f, 0.01f }));
		Engine::Ref_ptr<Engine::mesh> cube_lit = Engine::mesh::create(cube_lit_va, mat_emissive, "cube_lit");
		cube_lit->attachLight(lamp);
		m_scene.addMesh(cube_lit);
		m_scene.addLight(lamp);

		m_scene.addMaterial(mat_emissive, [] (const void* light, Engine::material* mat) {
			Engine::PtrPtr<Engine::pointLight> Light = (Engine::pointLight**)light;
			mat->setUniform4fF("emissiveColor", Light->diffuse);
			}, lamp.get());//<- an update-function and a pointer to some update-data get's supplied -> various kinds of materials could be created like this
		///////////////////////////////////////////////////////////////////////////////
		// IDEA: Remove the whole "emissiveMesh"-thing and just make being "emissive" a property of a material, cuz this is what it ultimately should be
		// ->this material get's "attached" to a light and updates it's color-values from there
		// -> think about, where I would implement this operation -> somewhere in Scene.update() -> maybe implement "material.update()"? Or just do it manually in the update-loop?
		// IDEA No2: I could implement an imgui-panel for material-editing -> the panel displays all of a material's values and properties and let's the user edit them
		///////////////////////////////////////////////////////////////////////////////
	}

	void test()
	{
		Engine::Ref_ptr<Engine::vertexArray> plane_va = Engine::vertexArray::create();
		plane_va->load("plane.model");
		plane_va->unbind();
		Engine::Ref_ptr<Engine::vertexArray> cube_va = Engine::vertexArray::create();
		cube_va->load("cube.model");
		cube_va->unbind();

		Engine::Ref_ptr<Engine::texture2d> cubeTex = Engine::texture2d::create("checkerboard.png", true);

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

		Engine::Ref_ptr<Engine::material> red = Engine::material::create(lightShader_dir, "red", flags_default | flag_no_backface_cull);//just use the variant for directional lights for initialization
		red->addShader(lightShader_point);
		red->addShader(lightShader_spot);
		red->setUniform4f("amb", 0.2f, 0.025f, 0.025f, 1.0f);
		red->setUniform4f("diff", 0.7f, 0.0875f, 0.0875f, 1.0f);
		red->setUniform4f("spec", 0.8f, 0.1f, 0.1f, 1.0f);
		red->setUniform1f("shininess", 100.0f);
		red->flushAll();
		m_scene.addMaterial(red);

		Engine::Ref_ptr<Engine::material> cubeMat = Engine::material::create(lightTexShader_dir, "cubeMat");
		cubeMat->addShader(lightTexShader_point);
		cubeMat->addShader(lightTexShader_spot);
		cubeMat->setTexture("u_texture", cubeTex);
		m_scene.addMaterial(cubeMat);

		Engine::Ref_ptr<Engine::mesh> plane = Engine::mesh::create(plane_va, red, "plane");
		plane->setScale(10.0f);
		plane->setPos({ 0.0f, 0.0f, 0.0f, 1.0f });
		m_scene.addMesh(plane);

		Engine::Ref_ptr<Engine::mesh> cube = Engine::mesh::create(cube_va, cubeMat, "cube");
		cube->setScale(0.1f);
		cube->setPos({ 0.0f, 1.0f, 8.0f, 1.0f });
		m_scene.addMesh(cube);

		Engine::PtrPtr<Engine::directionalLight> sun(Engine::Renderer::addDynamicDirLight({ vec3(0.0f, -0.7071067f, 0.7071067f), vec3(0.1f, 0.1f, 0.1f), vec3(0.5f, 0.5f, 0.5f), vec3(0.5f, 0.5f, 0.5f) }));
		m_scene.addLight(sun);
		Engine::PtrPtr<Engine::pointLight> lamp = Engine::Renderer::addDynamicPointLight({ vec3(2.0f, 1.0f, 0.0f), vec3(0.1f, 0.02f, 0.02f), vec3(0.5f, 0.1f, 0.1f), vec3(0.5f, 0.1f, 0.1f), 1.0f, 0.01f, 0.01f });
		m_scene.addLight(lamp);
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
		SandboxLayer* sandboxLayer = new SandboxLayer(90.f, 0.1f, 1000.0f);
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