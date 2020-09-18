#pragma once

#include "glCore.hpp"

#include "Engine/Rendering/vertexBufferLayout.hpp"

namespace Engine
{
	struct vertexBufferElement
	{
		unsigned int type;
		unsigned int count;
		unsigned char normalized;

		static unsigned int getTypeSize(unsigned int type)
		{
			switch (type)
			{
			case(GL_FLOAT):
				return 4;
			case(GL_UNSIGNED_INT):
				return 4;
			case(GL_UNSIGNED_BYTE):
				return 1;
			}
			ENG_CORE_ASSERT(false, "Type was not GL_FLOAT, GL_UNSIGNED_INT or GL_UNSIGNED_BYTE.");//this shouldn't happen
			return 0;
		}
	};

	class GLvertexBufferLayout : public vertexBufferLayout
	{
	private:
		unsigned int m_stride;
		std::vector<vertexBufferElement> m_layouts;
	public:
		GLvertexBufferLayout() :
			m_stride(0)
		{

		}
		~GLvertexBufferLayout() {  }

		virtual void pushFloat(const uint32_t count) override;
		virtual void pushUint(const uint32_t count) override;
		virtual void pushUchar(const uint32_t count) override;

		virtual void push(const ShaderDataType type) override;

		inline const std::vector<vertexBufferElement>& getLayouts() const { return m_layouts; }
		inline unsigned int getStride() const { return m_stride; }
	};
}