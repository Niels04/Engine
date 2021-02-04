#pragma once
#include "Engpch.hpp"

#include "Platform/OpenGL/OpenGLVertexArray.hpp"

//forked from opengl
#define ENG_SRC_ALPHA 0x0302
#define ENG_ONE_MINUS_SRC_ALPHA 0x0303
#define ENG_ONE 1

//also forked from gl
#define ENG_NEVER 0x0200
#define ENG_LESS 0x0201
#define ENG_EQUAL 0x0202
#define ENG_LEQUAL 0x0203
#define ENG_GREATER 0x0204
#define ENG_NOTEQUAL 0x0205
#define ENG_GEQUAL 0x0206
#define ENG_ALWAYS 0x0207

//defines for culling
#define ENG_FRONT 0x0404
#define ENG_BACK 0x0405
#define ENG_FRONT_AND_BACK 0x0408

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
		virtual void setBlend(const uint32_t sfactor, const uint32_t dfactor) const = 0;
		virtual void setDepth(const uint32_t method) const = 0;
		virtual void cullFace(const uint32_t face) const = 0;
		virtual const uint32_t getMaxGlobalBuffers() const = 0;
		virtual const uint8_t getMaxTextureBinds() const = 0;
		inline static API getAPI() { return s_api; }
	private:
		//private members

		//static members
		static API s_api;
	};
}