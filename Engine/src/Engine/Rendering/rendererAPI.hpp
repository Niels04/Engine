#pragma once
#include "Engpch.hpp"

#include "Platform/OpenGL/OpenGLVertexArray.hpp"

//blending -> forked from gl
#define ENG_SRC_ALPHA 0x0302
#define ENG_ONE_MINUS_SRC_ALPHA 0x0303
#define ENG_ONE 1
#define ENG_ZERO 0

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

//defines for colorBuffer attachments
#define ENG_COLOR_ATTACHMENT0 0x8CE0
#define ENG_COLOR_ATTACHMENT1 0x8CE1
#define ENG_COLOR_ATTACHMENT2 0x8CE2
#define ENG_COLOR_ATTACHMENT3 0x8CE3
#define ENG_COLOR_ATTACHMENT4 0x8CE4
#define ENG_COLOR_ATTACHMENT5 0x8CE5
#define ENG_COLOR_ATTACHMENT6 0x8CE6
#define ENG_COLOR_ATTACHMENT7 0x8CE7
#define ENG_COLOR_ATTACHMENT8 0x8CE8
#define ENG_COLOR_ATTACHMENT9 0x8CE9
#define ENG_COLOR_ATTACHMENT10 0x8CEA
#define ENG_COLOR_ATTACHMENT11 0x8CEB
#define ENG_COLOR_ATTACHMENT12 0x8CEC
#define ENG_COLOR_ATTACHMENT13 0x8CED
#define ENG_COLOR_ATTACHMENT14 0x8CEE
#define ENG_COLOR_ATTACHMENT15 0x8CEF
#define ENG_COLOR_ATTACHMENT16 0x8CF0
#define ENG_COLOR_ATTACHMENT17 0x8CF1
#define ENG_COLOR_ATTACHMENT18 0x8CF2
#define ENG_COLOR_ATTACHMENT19 0x8CF3
#define ENG_COLOR_ATTACHMENT20 0x8CF4
#define ENG_COLOR_ATTACHMENT21 0x8CF5
#define ENG_COLOR_ATTACHMENT22 0x8CF6
#define ENG_COLOR_ATTACHMENT23 0x8CF7
#define ENG_COLOR_ATTACHMENT24 0x8CF8
#define ENG_COLOR_ATTACHMENT25 0x8CF9
#define ENG_COLOR_ATTACHMENT26 0x8CFA
#define ENG_COLOR_ATTACHMENT27 0x8CFB
#define ENG_COLOR_ATTACHMENT28 0x8CFC
#define ENG_COLOR_ATTACHMENT29 0x8CFD
#define ENG_COLOR_ATTACHMENT30 0x8CFE
#define ENG_COLOR_ATTACHMENT31 0x8CFF

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
		virtual void enableBlend(const bool enabled) const = 0;
		virtual void setDepth(const uint32_t method) const = 0;
		virtual void enableDepth(const bool enabled) const = 0;
		virtual void cullFace(const uint32_t face) const = 0;
		virtual void drawToBuffers(const uint32_t count, va_list params) const = 0;
		virtual const uint32_t getMaxGlobalBuffers() const = 0;
		virtual const uint8_t getMaxTextureBinds() const = 0;
		inline static API getAPI() { return s_api; }
	private:
		//private members

		//static members
		static API s_api;
	};
}