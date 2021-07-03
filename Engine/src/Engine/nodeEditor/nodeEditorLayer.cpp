#include "Engpch.hpp"
#include "nodeEditorLayer.hpp"

#include "imgui.h"
#include "Engine/Application.hpp"

namespace Engine
{
	NodeEditorLayer::NodeEditorLayer(Scene* scene)
		: layer("NodeEditorLayer"), m_scene(scene)
	{

	}

	void NodeEditorLayer::onImGuiRender()
	{
		if (m_displayed)
		{
            static bool opt_fullscreen = true;
            static bool opt_padding = false;
            static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

            // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
            // because it would be confusing to have two docking targets within each others.
            ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
            if (opt_fullscreen)
            {
                ImGuiViewport* viewport = ImGui::GetMainViewport();
                ImGui::SetNextWindowPos(viewport->GetWorkPos());
                ImGui::SetNextWindowSize(viewport->GetWorkSize());
                ImGui::SetNextWindowViewport(viewport->ID);
                ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
                ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
                window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
                window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
            }
            else
            {
                dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
            }

            // When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
            // and handle the pass-thru hole, so we ask Begin() to not render a background.
            if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
                window_flags |= ImGuiWindowFlags_NoBackground;

            // Important: note that we proceed even if Begin() returns false (aka window is collapsed).
            // This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
            // all active windows docked into it will lose their parent and become undocked.
            // We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
            // any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
            if (!opt_padding)
                ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
            ImGui::Begin("NodeEditorDockspace", &m_displayed, window_flags);
            if (!opt_padding)
                ImGui::PopStyleVar();

            if (opt_fullscreen)
                ImGui::PopStyleVar(2);

            // DockSpace
            ImGuiIO& io = ImGui::GetIO();
            if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
            {
                ImGuiID dockspace_id = ImGui::GetID("NodeEditorDockSpace");
                ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
            }
            else
            {
                _ASSERT(false);
            }

            uint32_t index = 0;
            for (const auto& mesh : m_scene->m_meshes)
                DisplayMeshNode(mesh);
            for (const auto& light : m_scene->m_dirLights)
            {
                DisplayDirLightNode(light, index);
                index++;
            }
            index = 0;
            for (const auto& light : m_scene->m_pointLights)
            {
                DisplayPointLightNode(light, index);
                index++;
            }
            index = 0;
            for (const auto& light : m_scene->m_spotLights)
            {
                DisplaySpotLightNode(light, index);
                index++;
            }
            for (const auto& mat : m_scene->m_matLib.m_materials)
                DisplayMaterialNode(mat);

            ImGui::End();
		}
	}

	void NodeEditorLayer::onEvent(Event& e)
	{
		eventDispatcher dispatcher(e);
		dispatcher.dispatchEvent<keyPressedEvent>(ENG_BIND_EVENT_FN(NodeEditorLayer::onKeyPressedEvent));
	}

	bool NodeEditorLayer::onKeyPressedEvent(keyPressedEvent& e)
	{
		if (e.getKeyCode() == ENG_KEY_N)
		{
			m_displayed = !m_displayed;
		}
		return false;
	}

    void NodeEditorLayer::DisplayMeshNode(const WeakRef_ptr<mesh> Mesh)
    {
        static bool closeButton = NULL;
        ImGui::PushStyleColor(ImGuiCol_TitleBg, { 0.219607f, 0.141176f, 0.301961f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_TitleBgActive, { 0.307450f, 0.197646f, 0.422745f, 1.0f });
        ImGui::Begin(Mesh->getName().c_str(), (bool*)closeButton, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse);
        ImGui::PopStyleColor();
        ImGui::PopStyleColor();
        vec3 scale = Mesh->getScale();
        vec4 pos = Mesh->getPos();
        vec3 rot = Mesh->getRot();
        ImGui::SliderFloat3("scale", (float*)&scale, 0.1f, 10.0f);
        ImGui::DragFloat3("position", (float*)&pos, 0.25f, -100.0f, 100.0f);
        ImGui::DragFloat3("rotation", (float*)&rot, 0.25f, -2.0f * 3.1415926f, 2.0f * 3.1415926f);
        Mesh->setScale(scale);
        Mesh->setPos(pos);
        Mesh->setRot(rot);
        ImGui::End();
    }

    void NodeEditorLayer::DisplayDirLightNode(const PtrPtr<directionalLight> light, uint32_t index)
    {
        char dirLight_str[] = "directional light[   ]";
        std::to_chars(dirLight_str + 18U, dirLight_str + strlen(dirLight_str), index);
        static bool closeButton = NULL;
        ImGui::PushStyleColor(ImGuiCol_TitleBg, { 0.490196f, 0.560784f, 0.090196f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_TitleBgActive, { 0.686274f, 0.785098f, 0.126274f, 1.0f });
        ImGui::Begin(dirLight_str, (bool*)closeButton, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse);
        ImGui::PopStyleColor();
        ImGui::PopStyleColor();
        ImGui::ColorEdit3("ambient", (float*)&light->ambient);
        ImGui::ColorEdit3("diffuse", (float*)&light->diffuse);
        ImGui::ColorEdit3("specular", (float*)&light->specular);
        vec4 direction = light->direction;
        ImGui::DragFloat3("direction", (float*)&direction, 0.25f, -1000.0f, 1000.0f);
        light->direction = vec4::normalized(direction);
        ImGui::End();
    }

    void NodeEditorLayer::DisplayPointLightNode(const PtrPtr<pointLight> light, uint32_t index)
    {
        char pointLight_str[] = "point light[   ]";
        std::to_chars(pointLight_str + 12U, pointLight_str + strlen(pointLight_str), index);
        static bool closeButton = NULL;
        ImGui::PushStyleColor(ImGuiCol_TitleBg, { 0.490196f, 0.560784f, 0.090196f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_TitleBgActive, { 0.686274f, 0.785098f, 0.126274f, 1.0f });
        ImGui::Begin(pointLight_str, (bool*)closeButton, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse);
        ImGui::PopStyleColor();
        ImGui::PopStyleColor();
        ImGui::ColorEdit3("ambient", (float*)&light->ambient);
        ImGui::ColorEdit3("diffuse", (float*)&light->diffuse);
        ImGui::ColorEdit3("specular", (float*)&light->specular);
        ImGui::DragFloat3("position", (float*)&light->position, 0.25f, -100.0f, 100.0f);
        ImGui::End();
    }

    void NodeEditorLayer::DisplaySpotLightNode(const PtrPtr<spotLight> light, uint32_t index)
    {
        char spotLight_str[] = "spot light[   ]";
        std::to_chars(spotLight_str + 11U, spotLight_str + strlen(spotLight_str), index);
        static bool closeButton = NULL;
        ImGui::PushStyleColor(ImGuiCol_TitleBg, { 0.490196f, 0.560784f, 0.090196f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_TitleBgActive, { 0.686274f, 0.785098f, 0.126274f, 1.0f });
        ImGui::Begin(spotLight_str, (bool*)closeButton, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse);
        ImGui::PopStyleColor();
        ImGui::PopStyleColor();
        ImGui::ColorEdit3("ambient", (float*)&light->ambient);
        ImGui::ColorEdit3("diffuse", (float*)&light->diffuse);
        ImGui::ColorEdit3("specular", (float*)&light->specular);
        ImGui::DragFloat3("position", (float*)&light->position, 0.25f, -100.0f, 100.0f);
        vec4 direction = light->direction;
        ImGui::DragFloat3("direction", (float*)&direction, 0.25f, -1000.0f, 1000.0f);
        light->direction = vec4::normalized(direction);
        ImGui::SliderFloat("cutoff", &light->cutOff, cosf(80.0f * (3.1415926f / 180.0f)), 1.0f);
        ImGui::End();
    }

    void NodeEditorLayer::DisplayMaterialNode(const std::pair<std::string, Ref_ptr<material>> mat)
    {
        static bool closeButton = NULL;
        ImGui::PushStyleColor(ImGuiCol_TitleBg, { 0.156863f, 0.415686f, 0.109804f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_TitleBgActive, { 0.219608f, 0.581960f, 0.153726f, 1.0f });
        ImGui::Begin(mat.first.c_str(), (bool*)closeButton, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse);
        ImGui::PopStyleColor();
        ImGui::PopStyleColor();
        if (mat.second->getShader()->getMaterialUniformsSize())
        {
            const unsigned char* data = (unsigned char*)mat.second->getData();
            const std::unordered_map<std::string, shader::uniformProps> uniforms = mat.second->getShader()->getMaterialUniforms();
            for (const auto& uniform : uniforms)
            {
                DisplayMaterialComponent(uniform, data);
            }
            mat.second->flushAll();//update the material after all the values have been set in the rendering functions
        }
        for (const auto& texture : mat.second->m_textures)
        {
            ImGui::Text(texture.first.c_str());
            ImGui::Image((void*)texture.second->getRenderer_id(), { 128, 128 });
        }
        uint32_t flags_packed = mat.second->getFlags();
        ImGui::CheckboxFlags("depth_test", &flags_packed, flag_depth_test);
        ImGui::CheckboxFlags("cast_shadow", &flags_packed, flag_cast_shadow);
        ImGui::CheckboxFlags("no_backface_cull", &flags_packed, flag_no_backface_cull);
        mat.second->setFlags(flags_packed);
        ImGui::End();
    }

    void NodeEditorLayer::DisplayMaterialComponent(const std::pair<std::string, shader::uniformProps>& props, const unsigned char* data)
    {
        switch (props.second.type)
        {
        case(ENG_FLOAT):
        {
            ImGui::DragFloat(props.first.c_str(), (float*)(data + props.second.offset), 2.0f, -100.0f, 100.0f);
        }break;
        case(ENG_INT):
        {
            ImGui::DragInt(props.first.c_str(), (int*)(data + props.second.offset), 2.0f, -100.0f, 100.0f);
        }break;
        case(ENG_BOOL):
        {
            ImGui::Checkbox(props.first.c_str(), (bool*)(data + props.second.offset));
        }break;
        case(ENG_FLOAT_VEC2):
        {
            ImGui::DragFloat2(props.first.c_str(), (float*)(data + props.second.offset), 2.0f, -100.0f, 100.0f);
        }break;
        case(ENG_FLOAT_VEC3):
        {
            ImGui::ColorEdit3(props.first.c_str(), (float*)(data + props.second.offset));
        }break;
        case(ENG_FLOAT_VEC4):
        {
            ImGui::ColorEdit4(props.first.c_str(), (float*)(data + props.second.offset));
        }break;
        default:
        {
            ImGui::Text("Unknown material property type.");
        }break;
        }
    }
}