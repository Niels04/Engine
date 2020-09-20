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

		void addBuffer(GLvertexBuffer* buffer, GLvertexBufferLayout* Clayout);//deprecated
		void addBuffer(std::weak_ptr<GLvertexBuffer> buffer);//add a vertexBuffer and it's layout
		void addBuffer(std::weak_ptr<GLindexBuffer> buffer);//add an indexBuffer to the vertexArray
		void bind() const;
		void unbind() const;

		//these can be public, because we will want to access their member funcs
		std::shared_ptr<GLvertexBuffer> p_vertexBuffer;//use p to indicate that these are public members
		std::shared_ptr<GLindexBuffer> p_indexBuffer;//use p to indicate that these are public members
	private:
		unsigned int m_renderer_id;
	};
}