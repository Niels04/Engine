#pragma once
#include "Engine/layer.hpp"
#include "Engine/objects/scene.hpp"

namespace Engine
{
	class NodeEditorLayer : public layer
	{
	public:
		NodeEditorLayer(Scene* scene);

		virtual void onImGuiRender() override;
		virtual void onEvent(Event& e) override;
		bool onKeyPressedEvent(keyPressedEvent& e);

		void DisplayMeshNode(const WeakRef_ptr<mesh> Mesh);
		void DisplayDirLightNode(const PtrPtr<directionalLight> light, uint32_t index);
		void DisplayPointLightNode(const PtrPtr<pointLight> light, uint32_t index);
		void DisplaySpotLightNode(const PtrPtr<spotLight> light, uint32_t index);
		void DisplayMaterialNode(const std::pair<std::string, Ref_ptr<material>> mat);
		void DisplayMaterialComponent(const std::pair<std::string, shader::uniformProps>& props, const unsigned char* data);
	private:
		Scene* m_scene;
		bool m_displayed = false;
	};
}