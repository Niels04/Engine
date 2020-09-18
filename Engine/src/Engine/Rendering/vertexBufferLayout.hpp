#pragma once

namespace Engine
{
	enum class ShaderDataType : uint8_t//defines the datatypes that shaders can receive and that can thererfore be stored in vertexBuffers
	{
		NONE = 0, uChar = 1U, Float = 4U, vec2 = 8U, vec3 = 12U, vec4 = 16U,
		mat3 = 36U, mat4 = 64U//datatypes are defined as their sizes in bytes, so we don't need to convert between types and typesizes
	};

	class vertexBufferLayout
	{
	public:
		virtual ~vertexBufferLayout() {  }

		virtual void pushFloat(const uint32_t count) = 0;
		virtual void pushUint(const uint32_t count) = 0;
		virtual void pushUchar(const uint32_t count) = 0;

		virtual void push(const ShaderDataType type) = 0;

		static vertexBufferLayout* create();
	private:
	};
}