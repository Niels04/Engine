#pragma once

#include "Engine/layer.hpp"
#include "Engine/Rendering/renderer.hpp"

namespace Engine
{
	class ENGINE_API RenderLayer : public layer
	{
	public:
		RenderLayer();
		~RenderLayer() = default;

		virtual void onAttach() override {  }
		virtual void onDetach() override {  }
		virtual void onImGuiRender() override;
		virtual void onRender() override {  }

		void begin();
		void end();
	private:
		float m_time = 0.0f;//time is set to 0 on the first frame
	};

}