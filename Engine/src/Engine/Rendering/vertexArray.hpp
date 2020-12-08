#pragma once

#include "buffers.hpp"

namespace Engine
{
	class vertexArray
	{
	public:
		static Ref_ptr<vertexArray> create();
		virtual ~vertexArray() = default;

		virtual void addBuffer(Ref_ptr<vertexBuffer> vertexBuffer) = 0;
		virtual void addBuffer(Ref_ptr<indexBuffer> indexBuffer) = 0;

		virtual vertexBuffer* getVb() const = 0;//these return no smart pointers, because I don't want to give ownership of these members to anything other than the actual vertexArray they're associated to
		virtual indexBuffer* getIb() const = 0;

		virtual void bind() const = 0;
		virtual void unbind() const = 0;
	};
}