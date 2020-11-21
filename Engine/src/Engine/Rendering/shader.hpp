#pragma once
#include "Engpch.hpp"

namespace Engine
{
	class shader
	{
	public:
		virtual ~shader() {  }

		virtual void bind() const = 0;
		virtual void unbind() const = 0;

		static Ref_ptr<shader> create(const char* name);

		virtual void setUniform1b(const std::string& name, const bool bValue) = 0;

		virtual void setUniform1i(const std::string& name, const int value) = 0;

		virtual void setUniform1f(const std::string& name, const float fValue) = 0;
		virtual void setUniform2f(const std::string& name, const float v0, const float v1) = 0;
		virtual void setUniform3f(const std::string& name, const float v0, const float v1, const float v2) = 0;
		virtual void setUniform3f(const std::string& name, const vec3& vec) = 0;
		virtual void setUniform4f(const std::string& name, const float v0, const float v1, const float v2, const float v3) = 0;
		virtual void setUniform4f(const std::string& name, const vec4& vec) = 0;

		virtual void setUniformMat4(const std::string& name, const mat4& mat) = 0;


		virtual void bindUniformBlock(const std::string& name, const uint32_t bindingPoint) = 0;
	};
}