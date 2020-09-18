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

			static ShaderProgramSource parseShader(const std::string& filepath);
			static uint32_t compileShader(unsigned int type, const std::string& source);
			static uint32_t createShader(const std::string vertexShader, const std::string fragmentShader);
			//void loadBinary(std::string_view name, GLint size, GLenum format);//commented that out for now, gonna implement that later
		public:
			GLshader(const char* name);//normal constructor for loading and compiling from a source-file
			//Cshader(std::string_view name, vec3_vt<bool, GLint, GLenum> params);//overloaded constructor for precompiled loading
			~GLshader();

			void bind() const override;
			void unbind() const override;
			//vec3_vt<bool, GLint, GLenum> saveBinary(std::string_view name) const;//commented that out for now, gonna implement that later

			//void setprojMat(mat44f& projMat);
			//void setOffset(vec3<float> model, vec3<float> cam);
			//void setRot(vec2<float> camRot);

			//setuniforms
			virtual void setUniform1f(const std::string& name, float fValue) override;
			virtual void setUniform2f(const std::string& name, float v0, float v1) override;
			virtual void setUniform3f(const std::string& name, float v0, float v1, float v2) override;
			virtual void setUniform4f(const std::string& name, float v0, float v1, float v2, float v3) override;

			virtual void setUniform1i(const std::string& name, int value) override;

			//void setUniformMat4f(const std::string& name, const mat44f& mat);//maybe gonna implement that later, but with a glm-mat
		};
}