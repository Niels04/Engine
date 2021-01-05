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
		virtual void init() const = 0;
		virtual void clear() const = 0;
		virtual void clearDepth() const = 0;
		virtual void setClearColor(const glm::vec4& color) const = 0;
		virtual void drawIndexed(const Ref_ptr<vertexArray> va) const = 0;//drawIndexed with a vertexArray is what we are going to use for now since we are dealing with openGL
		virtual void setViewport(const uint32_t width, const uint32_t height) const = 0;
		virtual const uint32_t getMaxGlobalBuffers() const = 0;
		virtual const uint8_t getMaxTextureBinds() const = 0;
		inline static API getAPI() { return s_api; }
	private:
		//private members

		//static members
		static API s_api;
	};
}