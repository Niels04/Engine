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

		static shader* create(const char* name);

		virtual void setUniform1f(const std::string& name, float fValue) = 0;
		virtual void setUniform2f(const std::string& name, float v0, float v1) = 0;
		virtual void setUniform3f(const std::string& name, float v0, float v1, float v2) = 0;
		virtual void setUniform4f(const std::string& name, float v0, float v1, float v2, float v3) = 0;

		virtual void setUniform1i(const std::string& name, int value) = 0;
	};
}