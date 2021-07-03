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
    void GLglobalBuffer::getData(void* target)
    {
        bind();
        void* temp = malloc(m_size);
        GLCALL(glGetBufferSubData(GL_UNIFORM_BUFFER, 0, m_size, temp));
        unbind();
        memcpy(target, temp, m_size);
        free(temp);
    }

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
    void GLglobalBuffer::laddB(const uint32_t type, const uint8_t count)
    {
        m_elements.push_back({ type, count, m_stack });
        m_stack += uniformBufferElement::getTypeSize(type) * count;
        _ASSERT(m_stack <= m_size);
    }
    void GLglobalBuffer::lAddBoolB(const uint8_t count)
    {
        m_elements.push_back({ GL_BOOL, count, m_stack });
        m_stack += uniformBufferElement::getTypeSize(GL_BOOL) * count;
        _ASSERT(m_stack <= m_size);
    }
    void GLglobalBuffer::lAddIntB(const uint8_t count)//temporary!!!
    {
        m_elements.push_back({ GL_INT, count, m_stack });
        m_stack += uniformBufferElement::getTypeSize(GL_INT) * count;
        _ASSERT(m_stack <= m_size);
    }
    void GLglobalBuffer::lAddFloatB(const uint8_t count)
    {
        m_elements.push_back({GL_FLOAT, count, m_stack});
        m_stack += uniformBufferElement::getTypeSize(GL_FLOAT) * count;
        _ASSERT(m_stack <= m_size);
    }
    void GLglobalBuffer::lAddVec3B(const uint8_t count)
    {
        m_elements.push_back({ GL_FLOAT_VEC3, count, m_stack });
        m_stack += uniformBufferElement::getTypeSize(GL_FLOAT_VEC3) * count;
        _ASSERT(m_stack <= m_size);
    }
    void GLglobalBuffer::lAddVec4B(const uint8_t count)
    {
        m_elements.push_back({ GL_FLOAT_VEC4, count, m_stack });
        m_stack += uniformBufferElement::getTypeSize(GL_FLOAT_VEC4) * count;
        _ASSERT(m_stack <= m_size);
    }
    void GLglobalBuffer::lAddMat4B(const uint8_t count)
    {
        m_elements.push_back({ GL_FLOAT_MAT4, count, m_stack });
        m_stack += uniformBufferElement::getTypeSize(GL_FLOAT_MAT4) * count;
        _ASSERT(m_stack <= m_size);
    }
    ////////////////////////////////////////////////////////////////////////
    void GLglobalBuffer::lAdd(const uint32_t type, const uint16_t offset, const uint8_t count)
    {
        m_elements.push_back({ type, count, offset });
        _ASSERT(offset + uniformBufferElement::getTypeSize(type) <= m_size);
    }
    void GLglobalBuffer::lAddBool(const uint16_t offset, const uint8_t count)
    {
        m_elements.push_back({ GL_BOOL, count, offset });
        _ASSERT(offset + uniformBufferElement::getTypeSize(GL_BOOL) <= m_size);
    }
    void GLglobalBuffer::lAddInt(const uint16_t offset, const uint8_t count)
    {
        m_elements.push_back({ GL_INT, count, offset });
        _ASSERT(offset + uniformBufferElement::getTypeSize(GL_INT) <= m_size);
    }
    void GLglobalBuffer::lAddFloat(const uint16_t offset, const uint8_t count)
    {
        m_elements.push_back({ GL_FLOAT, count, offset });
        _ASSERT(offset + uniformBufferElement::getTypeSize(GL_FLOAT) <= m_size);
    }
    void GLglobalBuffer::lAddVec3(const uint16_t offset, const uint8_t count)
    {
        m_elements.push_back({ GL_FLOAT_VEC3, count, offset });
        _ASSERT(offset + uniformBufferElement::getTypeSize(GL_FLOAT_VEC3) <= m_size);
    }
    void GLglobalBuffer::lAddVec4(const uint16_t offset, const uint8_t count)
    {
        m_elements.push_back({ GL_FLOAT_VEC4, count, offset });
        _ASSERT(offset + uniformBufferElement::getTypeSize(GL_FLOAT_VEC4) <= m_size);
    }
    void GLglobalBuffer::lAddMat4(const uint16_t offset, const uint8_t count)
    {
        m_elements.push_back({ GL_FLOAT_MAT4, count, offset });
        _ASSERT(offset + uniformBufferElement::getTypeSize(GL_FLOAT_MAT4) <= m_size);
    }
    void GLglobalBuffer::updateElement(const uint8_t index, const void* val) const
    {
        GLCALL(glBufferSubData(GL_UNIFORM_BUFFER, m_elements[index].offset, uniformBufferElement::getUpdateSize(m_elements[index].type) * m_elements[index].count, val));
    }
    void GLglobalBuffer::updateAll(const void* val) const
    {
        GLCALL(glBufferSubData(GL_UNIFORM_BUFFER, 0, m_size, val));//update all values by specifying 0 as offset and m_size as the size
    }
    void GLglobalBuffer::updateFromTo(const uint8_t indexBegin, const uint8_t indexEnd, const void* val) const
    {
        uint16_t offset = 0;
        uint16_t size = 0;
        for (uint8_t i = 0; i < indexBegin; i++)
        {
            offset += uniformBufferElement::getTypeSize(m_elements[i].type);
        }
        for (uint8_t i = indexBegin; i <= indexEnd; i++)
        {
            size += uniformBufferElement::getTypeSize(m_elements[i].type);
        }
        GLCALL(glBufferSubData(GL_UNIFORM_BUFFER, offset, size, val));
    }
    //:::::::::FRAME_BUFFER::::::::::::::
    GLFrameBuffer::GLFrameBuffer()
    {
        GLCALL(glGenFramebuffers(1, &m_renderer_id));
    }
    GLFrameBuffer::~GLFrameBuffer()
    {
        GLCALL(glDeleteFramebuffers(1, &m_renderer_id));
    }

    void GLFrameBuffer::bind() const
    {
        GLCALL(glBindFramebuffer(GL_FRAMEBUFFER, m_renderer_id));
    }

    void GLFrameBuffer::unbind() const
    {
        GLCALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
    }

    void GLFrameBuffer::setReadBuffer() const
    {
        GLCALL(glBindFramebuffer(GL_READ_FRAMEBUFFER, m_renderer_id));
    }

    void GLFrameBuffer::setDrawBuffer() const
    {
        GLCALL(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_renderer_id));
    }

    void GLFrameBuffer::initShadow() const
    {
        bind();
        GLCALL(glDrawBuffer(GL_NONE));//tell openGl that we don't intend on drawing any colors
        GLCALL(glReadBuffer(GL_NONE));
        unbind();
    }

    void GLFrameBuffer::checkStatus() const
    {
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE)
            ENG_CORE_INFO("Framebuffer object complete.");
        else
            ENG_CORE_ERROR("Framebuffer object incomplete.");
    }

    void GLFrameBuffer::attachTexture(const Ref_ptr<ShadowMap2d>& map) const
    {
        GLCALL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, map->getRenderer_id(), 0));
    }
    void GLFrameBuffer::attachTexture(const Ref_ptr<ShadowMap3d>& map) const
    {
        //GLCALL(glFramebufferTexture3D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_CUBE_MAP, map->getRenderer_id(), 0, 0));//potential cause of errors, maybe use glFramebufferTexture() instead
        GLCALL(glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, map->getRenderer_id(), 0));
    }
    void GLFrameBuffer::attachTexture(const Ref_ptr<ShadowMap2dArray> & map, const uint8_t layer) const
    {
        GLCALL(glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, map->getRenderer_id(), 0, layer));//->maybe this 0 should be a 1
    }
    void GLFrameBuffer::attachTexture(const Ref_ptr<ShadowMap3dArray>& map) const
    {
        GLCALL(glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, map->getRenderer_id(), 0));//->maybe this 0 should be a 1
    }

    void GLFrameBuffer::attachTexture(const Ref_ptr<texture2d>& tex, const uint32_t slot) const
    {
        GLCALL(glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + slot, tex->getRenderer_id(), 0));
    }

   void GLFrameBuffer::attachRenderBuffer(const Ref_ptr<RenderBuffer>& buffer) const
    {
        switch (buffer->usage)
        {
        case(RenderBufferUsage::COLOR):
        {
            GLCALL(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, buffer->getRenderer_id()));
        }break;
        case(RenderBufferUsage::DEPTH):
        {
            GLCALL(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, buffer->getRenderer_id()));
        }break;
        case(RenderBufferUsage::STENCIL):
        {
            //not supported atm(I heard only with new nvidia drivers)
            //GLCALL(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_renderBuffer->getRenderer_id()));
            ENG_CORE_ASSERT(false, "Stencil-only renderbuffers are not supported at the moment. Consider using DEPTH_STENCIL.");
        }break;
        case(RenderBufferUsage::DEPTH_STENCIL):
        {
            GLCALL(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, buffer->getRenderer_id()));
        }break;
        default:
        {
            ENG_CORE_ASSERT(false, "Error attaching renderBuffer. \"usage\" was none of the accepted values.");
        }break;
        }
    }
    //::::::::RenderBuffer::::::::
    GLRenderBuffer::GLRenderBuffer(RenderBufferUsage usage, uint32_t width, uint32_t height)
        : RenderBuffer(usage)
    {
        GLCALL(glGenRenderbuffers(1, &m_renderer_id));
        GLCALL(glBindRenderbuffer(GL_RENDERBUFFER, m_renderer_id));
        switch (usage)
        {
        case(RenderBufferUsage::COLOR):
        {
            GLCALL(glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA8, width, height));
        }break;
        case(RenderBufferUsage::DEPTH):
        {
            GLCALL(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height));
        }break;
        case(RenderBufferUsage::STENCIL):
        {
            //not supported atm(I heard only with new nvidia drivers)
            //GLCALL(glRenderbufferStorage(GL_RENDERBUFFER, GL_STENCIL_INDEX8, width, height));
            ENG_CORE_ASSERT(false, "Stencil-only renderbuffers are not supported at the moment. Consider using DEPTH_STENCIL.");
        }break;
        case(RenderBufferUsage::DEPTH_STENCIL):
        {
            GLCALL(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height));
        }break;
        default:
        {
            ENG_CORE_ASSERT(false, "Error creating GLRenderBuffer. \"usage\" was none of the accepted values.");
        }break;
        }
        GLCALL(glBindRenderbuffer(GL_RENDERBUFFER, 0));
    }
    GLRenderBuffer::~GLRenderBuffer()
    {
        glDeleteRenderbuffers(1, &m_renderer_id);
    }

    void GLRenderBuffer::bind() const
    {
        GLCALL(glBindRenderbuffer(GL_RENDERBUFFER, m_renderer_id));
    }

    void GLRenderBuffer::unbind() const
    {
        GLCALL(glBindRenderbuffer(GL_RENDERBUFFER, 0));
    }
}