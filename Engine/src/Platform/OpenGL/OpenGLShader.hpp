#pragma once

#include "Engine/Rendering/shader.hpp"

namespace Engine
{
		struct ShaderProgramSource
		{
			std::string VertexSource;
			std::string FragmentSource;
		};

		class GLshader : public shader
		{
		private:
			uint32_t m_renderer_id;
			std::string m_filepath;
			//uniform cache
			std::unordered_map<std::string, int> m_uniformLocation_cache;

			int getUniformLocation(const std::string& name);

			static std::unordered_map<GLenum, std::string> parseShader(const std::string& filepath);//parses the shaders in the file //returns a map, with shaderType as a key and source as the second
			static uint32_t compileShader(const GLenum type, const std::string& source);
			static uint32_t createShader(const std::unordered_map<GLenum, std::string>& shaderSources);
			//void loadBinary(std::string_view name, GLint size, GLenum format);//commented that out for now, gonna implement that later
		public:
			GLshader(const char* name);//normal constructor for loading and compiling from a source-file
			//Cshader(std::string_view name, vec3_vt<bool, GLint, GLenum> params);//overloaded constructor for precompiled loading
			~GLshader();

			void bind() const override;
			void unbind() const override;
			//vec3_vt<bool, GLint, GLenum> saveBinary(std::string_view name) const;//commented that out for now, gonna implement that later

			//setuniforms
			virtual void setUniform1b(const std::string& name, const bool bValue) override;

			virtual void setUniform1i(const std::string& name, const int value) override;

			virtual void setUniform1f(const std::string& name, const float fValue) override;
			virtual void setUniform2f(const std::string& name, const float v0, const float v1) override;
			virtual void setUniform3f(const std::string& name, const float v0, const float v1, const float v2) override;
			virtual void setUniform3f(const std::string& name, const vec3& vec) override;
			virtual void setUniform4f(const std::string& name, const float v0, const float v1, const float v2, const float v3) override;
			virtual void setUniform4f(const std::string& name, const vec4& vec) override;

			virtual void setUniformMat4(const std::string& name, const mat4& mat) override;


			virtual void bindUniformBlock(const std::string& name, const uint32_t bindingPoint) override;
		};
}