#include <Engine.hpp>
#include "imgui/imgui.h"

#define PI 3.1415926f

class SandboxLayer : public Engine::layer
{
public:
	SandboxLayer(const float hFov, const float zNear, const float zFar)
		: layer("RenderLayer"), m_scene(hFov, zNear, zFar, static_cast<float>(Engine::Application::Get().getWindow().getWidth() / static_cast<float>(Engine::Application::Get().getWindow().getHeight())))
	{
		Engine::Application::Get().getWindow().setDissableCursor(false);
		Engine::Application::Get().getWindow().setFullscreen(false);
		//for now just call the different functions here, that load the scenes
		//plane_head_rotate();
		boat_water();
		//experiment();
		//normalMapping();
	}
	~SandboxLayer()
	{
	}

	void onUpdate(Engine::timestep ts) override
	{
		//update
		m_scene.onUpdate(ts, Engine::Application::Get().getWindow().isDissableCursor());

		//render
		Engine::renderCommand::setClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
		Engine::renderCommand::clear();

		Engine::Renderer::beginScene(m_scene.getCamera());
		{
			for (const auto& Mesh : m_scene)
			{
				Engine::Renderer::sub(Mesh);
			}
		}
		Engine::Renderer::endScene();
		//first render to the depth maps
		Engine::Renderer::RenderDepthMaps();
		//then render the scene with the shadows
		Engine::Renderer::Flush();
	}

	void onImGuiRender() override
	{
		m_scene.onImGuiRender();
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
		}
		return false;
	}

	void plane_head_rotate()
	{
		Engine::Ref_ptr<Engine::vertexArray> head_va = Engine::vertexArray::create();
		head_va->load("head.model");
		head_va->unbind();//make sure to unbind the vertexArray before loading the next one, otherwise we would get interfearences with layout & indexBuffer(I assume)
		Engine::Ref_ptr<Engine::vertexArray> plane_va = Engine::vertexArray::create();
		plane_va->load("plane.model");
		plane_va->unbind();
		Engine::Ref_ptr<Engine::vertexArray> helmet_va = Engine::vertexArray::create();
		helmet_va->load("helmet.model");
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

		//m_gunTex = Engine::texture2d::create("Cerberus_A.tga");
		Engine::Ref_ptr<Engine::texture2d> headTex = Engine::texture2d::create("head.png");
		Engine::Ref_ptr<Engine::texture2d> helmetTex = Engine::texture2d::create("armor_115_head_alb.png");
		//m_cubeTex = Engine::texture2d::create("chiseled_stone_bricks.png", FILTER_NEAREST);<-work on rgba-textures
		Engine::Ref_ptr<Engine::texture2d> cubeTex = Engine::texture2d::create("checkerboard.png");

		Engine::Ref_ptr<Engine::material> red = Engine::material::create(lightShader_dir, "red");//just use the variant for directional lights for initialization
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

		Engine::Ref_ptr<Engine::material> helmetMat = Engine::material::create(lightTexShader_dir, "helmetMat");
		helmetMat->addShader(lightTexShader_point);
		helmetMat->addShader(lightTexShader_spot);
		helmetMat->setTexture("u_texture", helmetTex);
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

		Engine::PtrPtr<Engine::directionalLight> sun(Engine::Renderer::addDynamicDirLight({ vec3(0.0f, -0.7071067f, 0.7071067f), vec3(0.5f, 0.5f, 0.5f), vec3(0.5f, 0.5f, 0.5f), vec3(0.5f, 0.5f, 0.5f) } ));
		m_scene.addLight(sun);
		Engine::Ref_ptr<Engine::DirLightMovement> rotate = Engine::DirLightMovement::create(0.0174533f * 60.0f * 0.25f, 0.0f, 0.0f);
		rotate->attachToLight(sun);
		m_scene.addLightMovement(rotate);
		//m_moon = Engine::Renderer::addStaticDirLight({ vec3(0.0f, -0.7071067f, -0.7071067f), vec3(1.0f, 1.0f, 1.0f), vec3(1.0f, 1.0f, 1.0f), vec3(1.0f, 1.0f, 1.0f) } ); <-doesn't work because of the lookAt-function edge-case
		//m_sun_2 = Engine::Renderer::addStaticDirLight({ vec3(-0.5773502f, -0.5773502, -0.5773502f), vec3(1.0f, 1.0f, 1.0f), vec3(1.0f, 1.0f, 1.0f), vec3(1.0f, 1.0f, 1.0f) });
		Engine::PtrPtr<Engine::pointLight> lamp = Engine::Renderer::addDynamicPointLight({ vec3(2.0f, 1.0f, 0.0f), vec3(0.5f, 0.1f, 0.1f), vec3(0.5f, 0.1f, 0.1f), vec3(0.5f, 0.1f, 0.1f), 1.0f, 0.01f, 0.01f });
		m_scene.addLight(lamp);
		Engine::Ref_ptr<Engine::PointLightMovement> move_circular = Engine::CircularPointLightMovement::create(vec3(0.0f, 1.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f), 0.0174533f * 60.0f);
		move_circular->attachToLight(lamp);
		m_scene.addLightMovement(move_circular);
		Engine::PtrPtr<Engine::spotLight> spotLight =  Engine::Renderer::addDynamicSpotLight({ vec3(-6.0f, 8.0f, 4.75f), vec3(0.0f, -1.0f, 0.0f), vec3(0.1f, 0.1f, 1.0f), vec3(0.1f, 0.1f, 1.0f), vec3(0.1f, 0.1f, 1.0f), cosf(15.0f * (PI / 180.0f)) });
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

		Engine::Ref_ptr<Engine::texture2d> spot_light_tex = Engine::texture2d::create("spotlight_bc.png");
		Engine::Ref_ptr<Engine::texture2d> duck_tex = Engine::texture2d::create("09-Default_albedo.jpg");
		Engine::Ref_ptr<Engine::texture2d> lighthouse_tex = Engine::texture2d::create("lighthouse_tex.png");
		Engine::Ref_ptr<Engine::texture2d> headTex = Engine::texture2d::create("figure_tex.png");

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

		Engine::Ref_ptr<Engine::material> water_mat = Engine::material::create(lightShader_dir, "water_blue");//just use the variant for directional lights for initialization
		water_mat->addShader(lightShader_point);
		water_mat->addShader(lightShader_spot);
		water_mat->setUniform4f("amb", 0.1f, 0.1f, 0.8f, 1.0f);
		water_mat->setUniform4f("diff", 0.1f, 0.1f, 0.8f, 1.0f);
		water_mat->setUniform4f("spec", 0.1f, 0.1f, 0.8f, 1.0f);
		water_mat->setUniform1f("shininess", 25.0f);
		water_mat->flushAll();
		m_scene.addMaterial(water_mat);

		Engine::Ref_ptr<Engine::material> lighthouse_mat = Engine::material::create(lightTexShader_dir, "lighthouse");//just use the variant for directional lights for initialization
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

		Engine::Ref_ptr<Engine::mesh> environment = Engine::mesh::create(environment_va, light_grey, "environment");
		environment->setScale(11.1111111f);
		m_scene.addMesh(environment);

		Engine::Ref_ptr<Engine::mesh> boat = Engine::mesh::create(boat_va, boat_mat, "boat");
		boat->setScale(0.5f);
		boat->setPos({-6.5f, -8.8f, 0.0f, 1.0f});
		boat->attachMovement(Engine::CircularMeshMovement::create(vec3(0.0f, 1.0f, 0.0f), vec3(0.0f, 0.0f, 0.0f), 0.0174533f * 60.0f));
		boat->attachMovement(Engine::MeshRotation::create(0.0f, 1.05f, 0.0f));
		m_scene.addMesh(boat);

		Engine::Ref_ptr<Engine::material> figureMat = Engine::material::create(lightTexShader_dir, "figureMat");//just use the variant for directional lights for initialization
		figureMat->addShader(lightTexShader_point);
		figureMat->addShader(lightTexShader_spot);
		figureMat->setTexture("u_texture", headTex);
		m_scene.addMaterial(figureMat);

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

		Engine::PtrPtr<Engine::directionalLight> sun(Engine::Renderer::addDynamicDirLight({ vec3(0.0f, -0.7071067f, 0.7071067f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 0.0f, 0.0f) }));
		m_scene.addLight(sun);
		Engine::PtrPtr<Engine::pointLight> lamp(Engine::Renderer::addDynamicPointLight({ vec3(6.0f, 5.0f, 3.0f), vec3(0.2f, 0.2f, 0.2f), vec3(0.2f, 0.2f, 0.2f), vec3(0.2f, 0.2f, 0.2f), 1.0f, 0.01f, 0.01f }));
		m_scene.addLight(lamp);
		Engine::PtrPtr<Engine::spotLight> spot_0(Engine::Renderer::addDynamicSpotLight({ vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, -0.242542f, -0.970167f), vec3(0.9f, 0.9f, 0.6f), vec3(0.9f, 0.9f, 0.6f), vec3(0.9f, 0.9f, 0.6f), cosf(15.0f * (PI / 180.0f)) }));
		m_scene.addLight(spot_0);
		Engine::Ref_ptr<Engine::SpotLightMovement> rotation_0 = Engine::SpotLightRotation::create(0.0f, -0.9f, 0.0f);
		rotation_0->attachToLight(spot_0);
		m_scene.addLightMovement(rotation_0);
		uint32_t animationSteps[] = { 0, 0, 0, 1, 2, 0, 2, 2, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 1 };
		Engine::Ref_ptr<Engine::SpotLightAnimation> flicker = Engine::SpotLightAnimation::create(3, 10, 20, 0.3f, 1.0f, 1.0f, animationSteps,
			vec3(0.9f, 0.9f, 0.6f), vec3(0.72f, 0.72f, 0.48f), vec3(0.0f, 0.0f, 0.0f));
		flicker->attachToLight(spot_0);
		m_scene.addLightAnimation(flicker);
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

		Engine::Ref_ptr<Engine::texture2d> spot_light_tex = Engine::texture2d::create("spotlight_bc.png");
		Engine::Ref_ptr<Engine::texture2d> duck_tex = Engine::texture2d::create("09-Default_albedo.jpg");

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
		stage->setRot({ 0.0f, 3.1415926f / 2.0f , 0.0f });//rotate 180 degrees(pi in radians)
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
			Engine::Ref_ptr<Engine::mesh> spot_light_mesh = Engine::mesh::create(spot_light_va, spot_light_mat, "spot_light_0");
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
		Engine::Ref_ptr<Engine::vertexArray> plane_va = Engine::vertexArray::create();
		plane_va->load("gun_tb.model");
		plane_va->unbind();
		//for testing manually assemble the va:
		/*vec3 pos1(-1.0f, 1.0f, 0.0f);
		vec3 pos2(-1.0f, -1.0f, 0.0f);
		vec3 pos3(1.0f, -1.0f, 0.0f);
		vec3 pos4(1.0f, 1.0f, 0.0f);
		std::pair<float, float> uv1(0.0f, 1.0f);
		std::pair<float, float> uv2(0.0f, 0.0f);
		std::pair<float, float> uv3(1.0f, 0.0f);
		std::pair<float, float> uv4(1.0f, 1.0f);
		vec3 normal(0.0f, 0.0f, 1.0f);

		vec3 edge1 = pos2 - pos1;
		vec3 edge2 = pos3 - pos1;
		std::pair<float, float> deltauv1(uv2.first - uv1.first, uv2.second - uv1.second);
		std::pair<float, float> deltauv2(uv3.first - uv1.first, uv3.second - uv1.second);
		float inverseDet = 1.0f / (deltauv1.first * deltauv2.second - deltauv2.first * deltauv1.second);
		vec3 tang;
		vec3 bitang;
		tang.x = inverseDet * (deltauv2.second * edge1.x - deltauv1.second * edge2.x);
		tang.y = inverseDet * (deltauv2.second * edge1.y - deltauv1.second * edge2.y);
		tang.z = inverseDet * (deltauv2.second * edge1.z - deltauv1.second * edge2.z);
		bitang.x = inverseDet * (-deltauv2.first * edge1.x + deltauv1.first * edge2.x);
		bitang.y = inverseDet * (-deltauv2.first * edge1.y + deltauv1.first * edge2.y);
		bitang.z = inverseDet * (-deltauv2.first * edge1.z + deltauv1.first * edge2.z);

		float vb[] = { -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, tang.x, tang.y, tang.z, bitang.x, bitang.y, bitang.z,
					   -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, tang.x, tang.y, tang.z, bitang.x, bitang.y, bitang.z,
						1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, tang.x, tang.y, tang.z, bitang.x, bitang.y, bitang.z,
						1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, tang.x, tang.y, tang.z, bitang.x, bitang.y, bitang.z, };
		Engine::Ref_ptr<Engine::vertexBuffer> plane_vb = Engine::vertexBuffer::create(sizeof(vb), vb);
		Engine::Ref_ptr<Engine::vertexBufferLayout> plane_layout = Engine::vertexBufferLayout::create();
		plane_layout->pushFloat(3); plane_layout->pushFloat(2); plane_layout->pushFloat(3); plane_layout->pushFloat(3); plane_layout->pushFloat(3);
		plane_vb->setLayout(plane_layout);
		plane_va->addBuffer(plane_vb);
		uint32_t indices[] = { 0, 1, 2, 2, 3, 0 };
		Engine::Ref_ptr<Engine::indexBuffer> plane_ib = Engine::indexBuffer::create(6, indices);
		plane_va->addBuffer(plane_ib);
		plane_va->unbind();*/

		Engine::Ref_ptr<Engine::texture2d> planeTex = Engine::texture2d::create("Cerberus_A.tga");
		Engine::Ref_ptr<Engine::texture2d> planeNormal = Engine::texture2d::create("Cerberus_N.tga");//USE LINEAR INTERPOLATION FOR NORMALMAPS AND NOT SOMETHING LIKE "FILTER_NEAREST" -> mutch smoother

		Engine::Ref_ptr<Engine::shader> lightTexShader_dir = Engine::Renderer::getShaderLib()->load("additive_w_shadow/dir/normal_mapping/basicPhong_one_texture_shadow_additive_dir.shader");
		lightTexShader_dir->bindUniformBlock("ViewProjection", VIEWPROJ_BIND);
		lightTexShader_dir->bindUniformBlock("directionalLights", DIRECTIONAL_LIGHTS_BIND);
		lightTexShader_dir->bindUniformBlock("directionalLights_v", DIRECTIONAL_LIGHTS_BIND);

		Engine::Ref_ptr<Engine::shader> lightTexShader_point = Engine::Renderer::getShaderLib()->load("additive_w_shadow/point/normal_mapping/basicPhong_one_texture_shadow_additive_point.shader");
		lightTexShader_point->bindUniformBlock("ViewProjection", VIEWPROJ_BIND);
		lightTexShader_point->bindUniformBlock("pointLights", POINT_LIGHTS_BIND);

		Engine::Ref_ptr<Engine::shader> lightTexShader_spot = Engine::Renderer::getShaderLib()->load("additive_w_shadow/spot/normal_mapping/basicPhong_one_texture_shadow_additive_spot.shader");
		lightTexShader_spot->bindUniformBlock("ViewProjection", VIEWPROJ_BIND);
		lightTexShader_spot->bindUniformBlock("spotLights", SPOT_LIGHTS_BIND);
		lightTexShader_spot->bindUniformBlock("spotLights_v", SPOT_LIGHTS_BIND);

		Engine::Ref_ptr<Engine::material> wallMat = Engine::material::create(lightTexShader_dir, "brick_mat");//just use the variant for directional lights for initialization
		wallMat->addShader(lightTexShader_point);
		wallMat->addShader(lightTexShader_spot);
		wallMat->setTexture("u_texture", planeTex);
		wallMat->setTexture("u_normalMap", planeNormal);
		m_scene.addMaterial(wallMat);

		Engine::Ref_ptr<Engine::mesh> brick_wall = Engine::mesh::create(plane_va, wallMat, "brick_wall");
		//brick_wall->setRot({PI/2.0f, 0.0f, 0.0f});
		m_scene.addMesh(brick_wall);

		Engine::PtrPtr<Engine::directionalLight> sun = Engine::Renderer::addDynamicDirLight({ vec3(0.0f, -0.7071067f, -0.7071067f), vec3(0.1f, 0.1f, 0.1f), vec3(0.8f, 0.8f, 0.8f), vec3(1.0f, 1.0f, 1.0f) });
		m_scene.addLight(sun);
	}

private:
	Engine::Scene m_scene;
};

class Sandbox : public Engine::Application
{
public:
	Sandbox()
	{
		pushLayer(new SandboxLayer(90.0f, 0.1f, 1000.0f));
	}
	~Sandbox()
	{
		
	}

};

Engine::Application* Engine::CreateApplication()
{
	return new Sandbox;
}