#include "Engpch.hpp"
#include "OpenGLVertexBufferLayout.hpp"

namespace Engine
{
	void GLvertexBufferLayout::pushFloat(const uint32_t count)
	{
		m_layouts.push_back({ GL_FLOAT, count, GL_FALSE }); //normalized is hardcoded to false, maybe have to change that later
		m_stride += vertexBufferElement::getTypeSize(GL_FLOAT) * count;
	}

	void GLvertexBufferLayout::pushUint(const uint32_t count)
	{
		m_layouts.push_back({ GL_UNSIGNED_INT, count, GL_FALSE }); //normalized is hardcoded to false, maybe have to change that later
		m_stride += vertexBufferElement::getTypeSize(GL_UNSIGNED_INT) * count;
	}

	void GLvertexBufferLayout::pushUchar(const uint32_t count)
	{
		m_layouts.push_back({ GL_UNSIGNED_BYTE, count, GL_FALSE }); //normalized is hardcoded to false, maybe have to change that later
		m_stride += vertexBufferElement::getTypeSize(GL_UNSIGNED_BYTE) * count;
	}

	void GLvertexBufferLayout::push(const ShaderDataType type)
	{
		switch (type)
		{
		case ShaderDataType::uChar:
		{
			m_layouts.push_back({ GL_UNSIGNED_BYTE, 1, GL_FALSE });//normalized is hardcoded to false, maybe have to change that later
			m_stride += (uint8_t)ShaderDataType::uChar;
			return;
		}
		case ShaderDataType::Float:
		{
			m_layouts.push_back({ GL_FLOAT, 1, GL_FALSE });//normalized is hardcoded to false, maybe have to change that later
			m_stride += (uint8_t)ShaderDataType::Float;
			return;
		}
		case ShaderDataType::vec2:
		{
			m_layouts.push_back({ GL_FLOAT, 2, GL_FALSE });//normalized is hardcoded to false, maybe have to change that later
			m_stride += ((uint8_t)ShaderDataType::Float * 2);
			return;
		}
		case ShaderDataType::vec3:
		{
			m_layouts.push_back({ GL_FLOAT, 3, GL_FALSE });//normalized is hardcoded to false, maybe have to change that later
			m_stride += ((uint8_t)ShaderDataType::Float * 3);
			return;
		}
		case ShaderDataType::vec4:
		{
			m_layouts.push_back({ GL_FLOAT, 4, GL_FALSE });//normalized is hardcoded to false, maybe have to change that later
			m_stride += ((uint8_t)ShaderDataType::Float * 4);
			return;
		}
		case ShaderDataType::NONE:
		{
			ENG_CORE_WARN("Trying to create vertexBufferLayout-element with datatype set to \"NONE\"(0). Created nothing.");
			return;
		}
		default:
		{
			ENG_CORE_ASSERT(false, "Trying to create vertexBufferLayout-element with undefined datatype(uint8_t: {0}).", type);
			return;
		}
		}
	}
}