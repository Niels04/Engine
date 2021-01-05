#pragma once
#include "Engpch.hpp"
#include "vertexBufferLayout.hpp"
#include "texture.hpp"

//defines for usage(forked from openGL)
#define STREAM_DRAW 0x88E0
#define STREAM_READ 0x88E1
#define STREAM_COPY 0x88E2
#define STATIC_DRAW 0x88E4
#define STATIC_READ 0x88E5
#define STATIC_COPY 0x88E6
#define DYNAMIC_DRAW 0x88E8
#define DYNAMIC_READ 0x88E9
#define DYNAMIC_COPY 0x88EA

namespace Engine
{

	class vertexBuffer
	{
	public:
		//usage could for example be STATIC_DRAW(not going to stream data to this buffer)
		static Ref_ptr<vertexBuffer> create(const uint32_t size, const void* data, const uint32_t usage = STATIC_DRAW);//usage is STATIC_DRAW by default
		virtual ~vertexBuffer() = default;

		virtual inline void setLayout(Ref_ptr<vertexBufferLayout> layout) = 0;
		virtual inline void bindLayout() const = 0;

		virtual inline void bind() const = 0;
		virtual inline void unbind() const = 0;

	};

	class indexBuffer
	{
	public:
		static Ref_ptr<indexBuffer> create(const uint32_t count, const uint32_t* indices, const uint32_t usage = STATIC_DRAW);//usage is STATIC_DRAW by default
		virtual ~indexBuffer() = default;

		inline virtual uint32_t getCount() const = 0;

		virtual void bind() const = 0;
		virtual void unbind() const = 0;
	};

	class globalBuffer//corresponds to uniformBuffers in OpenGL
	{
	public:
		static Ref_ptr<globalBuffer> create(const uint16_t size, const uint32_t usage);//create and bind the buffer
		static Scope_ptr<globalBuffer> createUnique(const uint16_t size, const uint32_t usage);//same
		virtual ~globalBuffer() = default;

		virtual void bind() const = 0;//bind the globalBuffer
		virtual void unbind() const = 0;//unbind the globalBuffer

		virtual void bindToPoint(const uint32_t bindingPoint) = 0;//bind the globalBuffer to a bindingPoint with index "bindingPoint"

		virtual void getData(void* target) = 0;

		virtual void laddB(const uint32_t type, const uint8_t count = 1) = 0;//add type "type" at the layout's back
		virtual void lAddBoolB(const uint8_t count = 1) = 0;//add a bool to the layout's back
		virtual void lAddIntB(const uint8_t count = 1) = 0;//add an int to the layout's back
		virtual void lAddFloatB(const uint8_t count = 1) = 0;//add a float to the layout's back
		virtual void lAddVec3B(const uint8_t count = 1) = 0;//add a vec3 to the layout's back
		virtual void lAddVec4B(const uint8_t count = 1) = 0;//add a vec4 to the layout's back
		virtual void lAddMat4B(const uint8_t count = 1) = 0;//add a mat4 to the layout's back

		virtual void lAdd(const uint32_t type, const uint16_t offset, const uint8_t count = 1) = 0;
		virtual void lAddBool(const uint16_t offset, const uint8_t count = 1) = 0;//add a bool to the layout at position "offset"
		virtual void lAddInt(const uint16_t offset, const uint8_t count = 1) = 0;//add an int to the layout at position "offset"
		virtual void lAddFloat(const uint16_t offset, const uint8_t count = 1) = 0;//add a float to the layout at position "offset"
		virtual void lAddVec3(const uint16_t offset, const uint8_t count = 1) = 0;//add a vec3 to the layout at position "offset"
		virtual void lAddVec4(const uint16_t offset, const uint8_t count = 1) = 0;//add a vec4 to the layout at position "offset"
		virtual void lAddMat4(const uint16_t offset, const uint8_t count = 1) = 0;//add a mat4 to the layout at position "offset"

		virtual void updateElement(const uint8_t index, const void* val) const = 0;//update one of the buffer's element's //index corresponds to the order in which they got added to the layout
		virtual void updateAll(const void* val) const = 0;
		virtual void updateFromTo(const uint8_t indexBegin, const uint8_t indexEnd, const void* val) const = 0;

		inline virtual uint16_t getSize() const = 0;
	};

	enum class RenderBufferUsage : uint8_t
	{
		COLOR, DEPTH, STENCIL, DEPTH_STENCIL
	};

	class RenderBuffer
	{
	public:
		static Ref_ptr<RenderBuffer> create(const RenderBufferUsage usage);
		RenderBuffer(RenderBufferUsage Usage) : usage(Usage) {  }
		virtual ~RenderBuffer() = default;

		inline virtual void bind() const = 0;
		inline virtual void unbind() const = 0;
	public:
		const RenderBufferUsage usage;//the RenderBuffer's usage
	};

	class FrameBuffer
	{
	public:
		static Ref_ptr<FrameBuffer> create();
		virtual ~FrameBuffer() = default;

		inline virtual void bind() const = 0;
		inline virtual void unbind() const = 0;

		inline virtual void attachTexture(Ref_ptr<FrameBufferTexture>& texture) = 0;//attach a frameBufferTexture for color to the FrameBuffer -> probalby the only texture that will be attached
		//to a frameBuffer cuz there are RenderBuffers, which are more efficient(but not so good for reading, thats why in the case of colors we wanna use a texture)
		inline virtual void attachRenderBuffer(Ref_ptr<RenderBuffer>& buffer) = 0;
	};
}