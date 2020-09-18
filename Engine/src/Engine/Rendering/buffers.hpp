#pragma once
#include "Engpch.hpp"
#include "vertexBufferLayout.hpp"

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
		static vertexBuffer* create(const uint32_t size, const void* data, const uint32_t usage = STATIC_DRAW);//usage is STATIC_DRAW by default
		virtual ~vertexBuffer() {  }

		virtual inline void setLayout(vertexBufferLayout* layout) = 0;
		virtual inline void bindLayout() const = 0;

		virtual inline void bind() const = 0;
		virtual inline void unbind() const = 0;

	};

	class indexBuffer
	{
	public:
		static indexBuffer* create(const uint32_t count, const uint32_t* indices, const uint32_t usage = STATIC_DRAW);//usage is STATIC_DRAW by default
		virtual ~indexBuffer() {  }

		inline virtual uint32_t getCount() const = 0;

		virtual void bind() const = 0;
		virtual void unbind() const = 0;
	};
}