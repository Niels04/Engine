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

    void GLvertexBuffer::setLayout(Ref_ptr<vertexBufferLayout> layout)
    {
        m_layout = std::dynamic_pointer_cast<GLvertexBufferLayout, vertexBufferLayout>(layout);
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
        ENG_CORE_ASSERT(layouts.size(), "Tried to bind vertexBufferLayout but size was 0.");
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

    //:::::::GLOBAL::BUFFER::::(Uniform_Buffer)::::
    GLglobalBuffer::GLglobalBuffer(const uint16_t size, const uint32_t usage)
        : m_size(size)
    {
        GLCALL(glCreateBuffers(1, &m_renderer_id));
        bind();
        GLCALL(glBufferData(GL_UNIFORM_BUFFER, m_size, nullptr, usage));
    }
    GLglobalBuffer::~GLglobalBuffer()
    {
        GLCALL(glDeleteBuffers(1, &m_renderer_id));
    }
    void GLglobalBuffer::bind() const
    {
        GLCALL(glBindBuffer(GL_UNIFORM_BUFFER, m_renderer_id));
    }
    void GLglobalBuffer::unbind() const
    {
        GLCALL(glBindBuffer(GL_UNIFORM_BUFFER, 0));
    }
    void GLglobalBuffer::bindToPoint(const uint32_t bindingPoint)
    {
        glBindBufferBase(GL_UNIFORM_BUFFER, bindingPoint, m_renderer_id);
    }
    void GLglobalBuffer::lAddBool(const uint8_t count)
    {
        m_elements.push_back({ GL_BOOL, count, m_stack });
        m_stack += uniformBufferElement::getTypeSize(GL_BOOL) * count;
        _ASSERT(m_stack <= m_size);
    }
    void GLglobalBuffer::lAddInt(const uint8_t count)
    {
        m_elements.push_back({ GL_INT, count, m_stack });
        m_stack += uniformBufferElement::getTypeSize(GL_INT) * count;
        _ASSERT(m_stack <= m_size);
    }
    void GLglobalBuffer::lAddFloat(const uint8_t count)
    {
        m_elements.push_back({GL_FLOAT, count, m_stack});
        m_stack += uniformBufferElement::getTypeSize(GL_FLOAT) * count;
        _ASSERT(m_stack <= m_size);
    }
    void GLglobalBuffer::lAddVec3(const uint8_t count)
    {
        m_elements.push_back({ GL_FLOAT_VEC3, count, m_stack });
        m_stack += uniformBufferElement::getTypeSize(GL_FLOAT_VEC3) * count;
        _ASSERT(m_stack <= m_size);
    }
    void GLglobalBuffer::lAddVec4(const uint8_t count)
    {
        m_elements.push_back({ GL_FLOAT_VEC4, count, m_stack });
        m_stack += uniformBufferElement::getTypeSize(GL_FLOAT_VEC4) * count;
        _ASSERT(m_stack <= m_size);
    }
    void GLglobalBuffer::lAddMat4(const uint8_t count)
    {
        m_elements.push_back({ GL_FLOAT_MAT4, count, m_stack });
        m_stack += uniformBufferElement::getTypeSize(GL_FLOAT_MAT4) * count;
        _ASSERT(m_stack <= m_size);
    }
    void GLglobalBuffer::updateElement(const uint8_t index, const void* val)
    {
        GLCALL(glBufferSubData(GL_UNIFORM_BUFFER, m_elements[index].offset, uniformBufferElement::getTypeSize(m_elements[index].type) * m_elements[index].count, val));
    }
}