#pragma once

#include "core.hpp"
#include "Events/Event.hpp"

namespace Engine
{
	class ENGINE_API layer
	{
	public:
		layer(const std::string& name = "Layer");
		virtual ~layer();

		virtual void onAttach() {  }//when the layer is pushed onto the layerStack
		virtual void onDetach() {  }//when the layer is removed from the layerStack
		virtual void onUpdate() {  }//when the layer is updated
		virtual void onEvent(Event& e) {  }//when an event gets sent to the layer

		inline const std::string& getName() const { return m_DebugName; }
	protected:
		std::string m_DebugName;
	};
}