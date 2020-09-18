#include "Engpch.hpp"
#include "glCore.hpp"
#include "GLbuffers.hpp"

namespace Engine
{
	//:::::::VERTEX::BUFFER:::::::::
    GLvertexBuffer::GLvertexBuffer(const uint32_t size, const void* data, const uint32_t usage)
    {
        GLCALL(glCreateBuffers(1, &m_renderer_id));
        GLCALL(glBindBuffer(GL_ARRAY_BUFFER, m_renderer_id));
        GLCALL(glBufferData(GL_ARRAY_BUFFER, size, data, (uint32_t)usage));
    }

    GLvertexBuffer::~GLvertexBuffer()
    {
        GLCALL(glDeleteBuffers(1, &m_renderer_id));
    }

    void GLvertexBuffer::setLayout(vertexBufferLayout* layout)
    {
        m_layout = (GLvertexBufferLayout*)layout;
    }

    void GLvertexBuffer::bind() const
    {
        GLCALL(glBindBuffer(GL_ARRAY_BUFFER, m_renderer_id));
    }

    void GLvertexBuffer::unbind() const
    {
        GLCALL(glBindBuffer(GL_ARRAY_BUFFER, 0));
    }

    void GLvertexBuffer::bindLayout() const
    {
        bind();
        const auto& layouts = m_layout->getLayouts();
        uint32_t offset = 0;
        for (uint32_t i = 0; i < layouts.size(); i++)
        {
            const auto& layout = layouts[i];
            GLCALL(glEnableVertexAttribArray(i));
            GLCALL(glVertexAttribPointer(i, layout.count, layout.type, layout.normalized, m_layout->getStride(), (const void*)offset));
            offset += layout.count * vertexBufferElement::getTypeSize(layout.type);
        }
    }

    //:::::::INDEX::BUFFER::::::::::::
    GLindexBuffer::GLindexBuffer(const uint32_t count, const uint32_t* data, const uint32_t usage)
        : m_count(count)
    {
        ENG_CORE_ASSERT(sizeof(uint32_t) == sizeof(GLuint), "Sizeof unsigned int was not the size of GLuint!");//if this isn't the case we have some platform-specific issues

        GLCALL(glCreateBuffers(1, &m_renderer_id));
        GLCALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_renderer_id));
        GLCALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(uint32_t), data, (uint32_t)usage));
    }

    GLindexBuffer::~GLindexBuffer()
    {
        GLCALL(glDeleteBuffers(1, &m_renderer_id));
    }

    void GLindexBuffer::bind() const
    {
        GLCALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_renderer_id));
    }

    void GLindexBuffer::unbind() const
    {
        GLCALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
    }
}