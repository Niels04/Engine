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

	void GLvertexArray::addBuffer(GLvertexBuffer* buffer, GLvertexBufferLayout* Clayout)//warning, deprecated
	{
		bind();
		buffer->bind();
		const auto& layouts = Clayout->getLayouts();
		ENG_CORE_ASSERT(layouts.size(), "Tried to tie vertexBuffer to vertexArray, but layout wasn't specified.");
		unsigned int offset = 0;
		for (unsigned int i = 0; i < layouts.size(); i++)
		{
			const auto& layout = layouts[i];
			GLCALL(glEnableVertexAttribArray(i));
			GLCALL(glVertexAttribPointer(i, layout.count, layout.type, layout.normalized, Clayout->getStride(), (const void*)offset));
			offset += layout.count * vertexBufferElement::getTypeSize(layout.type);
		}
	}

	void GLvertexArray::addBuffer(std::weak_ptr<GLvertexBuffer> buffer)
	{
		p_vertexBuffer = buffer.lock();
		bind();
		p_vertexBuffer->bind();
		p_vertexBuffer->bindLayout();
	}

	void GLvertexArray::addBuffer(std::weak_ptr<GLindexBuffer> buffer)
	{
		p_indexBuffer = buffer.lock();
		bind();
		p_indexBuffer->bind();
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