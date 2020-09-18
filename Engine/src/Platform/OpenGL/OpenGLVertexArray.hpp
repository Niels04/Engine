#pragma once

#include "GLbuffers.hpp"
#include "OpenGLVertexBufferLayout.hpp"

namespace Engine
{
	class GLvertexArray
	{
	public:
		GLvertexArray();
		~GLvertexArray();

		void addBuffer(GLvertexBuffer* buffer, GLvertexBufferLayout* Clayout);
		void bind() const;
		void unbind() const;
	private:
		unsigned int m_renderer_id;
	};
}