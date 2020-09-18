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
	//::::::::::Remove the vertexArray-interface completely, since we only use it for OpenGL
	void GLvertexArray::addBuffer(GLvertexBuffer* buffer, GLvertexBufferLayout* Clayout)
	{
		bind();
		buffer->bind();
		const auto& layouts = Clayout->getLayouts();
		unsigned int offset = 0;
		for (unsigned int i = 0; i < layouts.size(); i++)
		{
			const auto& layout = layouts[i];
			GLCALL(glEnableVertexAttribArray(i));
			GLCALL(glVertexAttribPointer(i, layout.count, layout.type, layout.normalized, Clayout->getStride(), (const void*)offset));
			offset += layout.count * vertexBufferElement::getTypeSize(layout.type);
		}
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