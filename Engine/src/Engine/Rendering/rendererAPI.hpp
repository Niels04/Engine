#pragma once
#include "Engpch.hpp"

#include "Platform/OpenGL/OpenGLVertexArray.hpp"

namespace Engine
{
	class RendererAPI
	{
	public:
		//types
		enum class API : uint8_t
		{
			NONE = 0, OpenGL = 1
		};
	public:
		//public functions
		virtual void init() = 0;
		virtual void clear() = 0;
		virtual void setClearColor(const glm::vec4& color) = 0;
		virtual void drawIndexed(const Ref_ptr<GLvertexArray> va) = 0;//drawIndexed with a vertexArray is what we are going to use for now since we are dealing with openGL
		inline static API getAPI() { return s_api; }
	private:
		//private members

		//static members
		static API s_api;
	};
}