#pragma once

#include "Engine/Rendering/buffers.hpp"
#include "OpenGLVertexBufferLayout.hpp"

namespace Engine
{
	//::::::::::VERTEX::BUFFER::::::::::::
	class GLvertexBuffer : public vertexBuffer
	{
	public:
		GLvertexBuffer(const uint32_t size, const void* data, const uint32_t usage);
		~GLvertexBuffer();

		virtual inline void setLayout(Ref_ptr<vertexBufferLayout> layout) override;
		virtual inline void bindLayout() const override;//not gonna use this often in gl cuz we have vertexArrays

		virtual inline void bind() const override;
		virtual inline void unbind() const override;
	private:
		unsigned int m_renderer_id;
		Ref_ptr<GLvertexBufferLayout> m_layout;
	};
	//::::::::::INDEX::BUFFER:::::::::::::
	class GLindexBuffer : public indexBuffer
	{
	private:
		unsigned int m_renderer_id;
		unsigned int m_count;
	public:
		GLindexBuffer(const uint32_t count, const uint32_t* data, const uint32_t usage);
		~GLindexBuffer();

		inline virtual uint32_t getCount() const override { return m_count; }

		virtual inline void bind() const override;
		virtual inline void unbind() const override;

	};
	//::::::::::uniformBufferElement::::::::::::::::::::
	struct uniformBufferElement
	{
		unsigned int type;
		uint8_t count;
		uint16_t offset;//warning, allows for a maximum offset of 65,535!!!

		uniformBufferElement(const unsigned int inType, const uint8_t inCount, const uint16_t inOffset)
			: type(inType), count(inCount), offset(inOffset)
		{
		}
		~uniformBufferElement()
		{
		}

		static unsigned int getTypeSize(unsigned int type)//return the typesize in the uniformBuffer when using std140 memory layout
		{
			switch (type)
			{
			case(GL_FLOAT):
				return 4;
			case(GL_INT):
				return 4;
			case(GL_BOOL):
				return 4;//bools also take 4bytes in uniform buffers when using std140 memory layout
			case(GL_FLOAT_VEC2):
				return 8;
			case(GL_FLOAT_VEC3):
				return 16;//also vec3s take 16 bytes when using std140 memory layout instead of 12bytes
			case(GL_FLOAT_VEC4):
				return 16;
			case(GL_FLOAT_MAT4):
				return 64;
			}
			ENG_CORE_ASSERT(false, "Type was unknown(uniformBufferElement).");//this shouldn't happen
			return 0;
		}
	};
	//::::::::::UNIFORM::BUFFER:::::::(GLOBAL_BUFFER):::::
	class GLglobalBuffer : public globalBuffer
	{
	public:
		GLglobalBuffer(const uint16_t size, const uint32_t usage);//create and bind the buffer
		~GLglobalBuffer();

		inline virtual void bind() const override;
		inline virtual void unbind() const override;

		inline virtual void bindToPoint(const uint32_t bindingPoint) override;

		virtual void getData(void *target) override;

		inline virtual void laddB(const uint32_t type, const uint8_t count = 1) override;
		inline virtual void lAddBoolB(const uint8_t count = 1) override;
		inline virtual void lAddIntB(const uint8_t count = 1) override;
		inline virtual void lAddFloatB(const uint8_t count = 1) override;
		inline virtual void lAddVec3B(const uint8_t count = 1) override;
		inline virtual void lAddVec4B(const uint8_t count = 1) override;
		inline virtual void lAddMat4B(const uint8_t count = 1) override;

		inline virtual void lAdd(const uint32_t type, const uint16_t offset, const uint8_t count = 1) override;
		inline virtual void lAddBool(const uint16_t offset, const uint8_t count = 1) override;
		inline virtual void lAddInt(const uint16_t offset, const uint8_t count = 1) override;
		inline virtual void lAddFloat(const uint16_t offset, const uint8_t count = 1) override;
		inline virtual void lAddVec3(const uint16_t offset, const uint8_t count = 1) override;
		inline virtual void lAddVec4(const uint16_t offset, const uint8_t count = 1) override;
		inline virtual void lAddMat4(const uint16_t offset, const uint8_t count = 1) override;

		inline virtual void updateElement(const uint8_t index, const void* val) const override;
		inline virtual void updateAll(const void* val) const override;

		inline virtual uint16_t getSize() const override { return m_size; }//return the buffer's size in bytes
	private:
		unsigned int m_renderer_id;
		uint16_t m_size = 0;//maximum size of 65,535
		uint16_t m_stack = 0;//points to the offset, at which the next element will be inserted into the layout
		std::vector<uniformBufferElement> m_elements;
	};
}