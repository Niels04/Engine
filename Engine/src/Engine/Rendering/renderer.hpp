#pragma once

namespace Engine
{
	enum class RenderingAPI
	{
		NONE = 0, OpenGL
	};

	class Renderer
	{
	public:
		inline static RenderingAPI getAPI() { return s_Rendering_API; }
	private:
		static RenderingAPI s_Rendering_API;
	};
}