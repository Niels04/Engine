#pragma once

#include "Engine/Rendering/vertexArray.hpp"

#include "GLbuffers.hpp"

namespace Engine
{
	class GLvertexArray : public vertexArray
	{
	public:
		GLvertexArray();
		~GLvertexArray();

		virtual void addBuffer(Ref_ptr<vertexBuffer> buffer) override;//add a vertexBuffer and it's layout
		virtual void addBuffer(Ref_ptr<indexBuffer> buffer) override;//add an indexBuffer to the vertexArray

		virtual inline vertexBuffer* getVb() const override { return m_vertexBuffer.get(); }
		virtual inline indexBuffer* getIb() const override { return m_indexBuffer.get(); }

		inline virtual void bind() const override;
		inline virtual void unbind() const override;

	private:
		unsigned int m_renderer_id;
		Ref_ptr<GLvertexBuffer> m_vertexBuffer;
		Ref_ptr<GLindexBuffer> m_indexBuffer;
	};
}