#pragma once
#include "Engine/Rendering/rendererAPI.hpp"

namespace Engine
{
	class OpenGLRendererAPI : public RendererAPI
	{
	public:
		virtual void clear() override;
		virtual void setClearColor(const glm::vec4& color) override;
		virtual void drawIndexed(const std::shared_ptr<GLvertexArray> va) override;//take in a shared_ptr here, cuz we would need to convert anyway, because we want to access it in this function
	};
}