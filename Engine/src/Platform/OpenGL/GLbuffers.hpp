#pragma once

#include "Engine/Rendering/buffers.hpp"
#include "OpenGLVertexBufferLayout.hpp"

namespace Engine
{
	//::::::::::VERTEX::BUFFER::::::::::::
	class GLvertexBuffer : public vertexBuffer
	{
	public:
		GLvertexBuffer(const uint32_t size, const void* data, const uint32_t usage);
		~GLvertexBuffer();

		virtual inline void setLayout(vertexBufferLayout* layout) override;
		virtual inline void bindLayout() const override;//not gonna use this often in gl cuz we have vertexArrays

		virtual inline void bind() const override;
		virtual inline void unbind() const override;
	private:
		unsigned int m_renderer_id;
		GLvertexBufferLayout* m_layout;
	};
	//::::::::::INDEX::BUFFER:::::::::::::
	class GLindexBuffer : public indexBuffer
	{
	private:
		unsigned int m_renderer_id;
		unsigned int m_count;
	public:
		GLindexBuffer(const uint32_t count, const uint32_t* data, const uint32_t usage);
		~GLindexBuffer();

		inline virtual uint32_t getCount() const override { return m_count; }

		virtual inline void bind() const override;
		virtual inline void unbind() const override;

	};
}