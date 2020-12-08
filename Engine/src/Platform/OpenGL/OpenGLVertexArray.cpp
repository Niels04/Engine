#include "Engpch.hpp"
#include "glCore.hpp"
#include "OpenGLVertexArray.hpp"

namespace Engine
{
	GLvertexArray::GLvertexArray()
	{
		GLCALL(glGenVertexArrays(1, &m_renderer_id));
	}

	GLvertexArray::~GLvertexArray()
	{
		GLCALL(glDeleteVertexArrays(1, &m_renderer_id));
	}

	void GLvertexArray::addBuffer(Ref_ptr<vertexBuffer> buffer)
	{
		m_vertexBuffer = std::static_pointer_cast<GLvertexBuffer, vertexBuffer>(buffer);
		bind();
		m_vertexBuffer->bind();
		m_vertexBuffer->bindLayout();
	}

	void GLvertexArray::addBuffer(Ref_ptr<indexBuffer> buffer)
	{
		m_indexBuffer = std::static_pointer_cast<GLindexBuffer, indexBuffer>(buffer);
		bind();
		m_indexBuffer->bind();
	}

	void GLvertexArray::bind() const
	{
		GLCALL(glBindVertexArray(m_renderer_id));
	}

	void GLvertexArray::unbind() const
	{
		GLCALL(glBindVertexArray(0));
	}
}