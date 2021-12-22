#pragma once

#include "core.hpp"
#include "Events/Event.hpp"

#include "Engine/core/timeStep.hpp"

namespace Engine
{
	class ENGINE_API layer
	{
	public:
		layer(const std::string& name = "Layer");
		virtual ~layer() = default;

		virtual void onAttach() {  }//when the layer is pushed onto the layerStack
		virtual void onDetach() {  }//when the layer is removed from the layerStack
		virtual void onUpdate(timestep ts) {  }//when the layer is updated
		virtual void onImGuiRender() {  }//in this function we specify what we want this layer to render in ImGui (the rest gets done in imGuiLayer::begin() and imGuiLayer::end()
		virtual void onRender() {  }//in this function we specify what we want the layer to render -> inside the function the layer can submit meshes with transforms to the renderer
		virtual void onEvent(Event& e) {  }//when an event gets sent to the layer

		inline const std::string& getName() const { return m_DebugName; }
	protected:
		std::string m_DebugName;
	};
}