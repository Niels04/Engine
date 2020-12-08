#pragma once

#include "Engine/Rendering/shader.hpp"

namespace Engine
{
		struct ShaderProgramSource
		{
			std::string VertexSource;
			std::string FragmentSource;
		};

		struct UniformElement;

		class GLshader : public shader
		{
		public:
			GLshader(const std::string& fileName);//normal constructor for loading and compiling from a source-file
			GLshader(const std::string& fileName, const std::string& name);//same as above, but used if one want's to assign a custom name to the shader
			//Cshader(std::string_view name, vec3_vt<bool, GLint, GLenum> params);//overloaded constructor for precompiled loading
			~GLshader();

			void bind() const override;
			void unbind() const override;

			virtual const std::string& getName() const override { return m_name; }
			virtual void setName(const std::string& nameNew) override { m_name = nameNew; }

			virtual const std::unordered_map<std::string, uniformProps>& getMaterialUniforms() const override { return m_materialUniforms; }//for each materialUniform that can get set, this get's an entry with first being the uniformName and second being its offset on the cpu-buffer
			virtual const uint16_t getMaterialUniformsSize() const override { return m_materialUniformsSize; }
			virtual const std::vector<std::string>& getTextures() const override { return m_textures; }
			virtual const bool hasTextures() const override { return m_textured; }

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

		private:
			uint32_t m_renderer_id;
			std::string m_filepath;
			std::string m_name;
			//uniform cache
			std::unordered_map<std::string, int> m_uniformLocation_cache;
			std::unordered_map<std::string, int> m_uniformBlockLocation_cache;

			int getUniformLocation(const std::string& name);
			int getUniformBlockLocation(const std::string& name);

			std::unordered_map<GLenum, std::string> parseShader(const std::string& filepath);//parses the shaders in the file //returns a map, with shaderType as a key and source as the second

			std::unordered_map<std::string, uniformProps> m_materialUniforms;//the uniform elements the fragmentShader expects, THE VERTEX SHADER USUALLY DOESN'T ACCEPT UNIFORMS THAT HAVE SOMETHING TO DO WITH MATERIALS
			uint16_t m_materialUniformsSize;//size of all materialUniforms added in bytes
			bool m_textured;//true if the shader expects any textures
			std::vector<std::string> m_textures;
			static uint32_t compileShader(const GLenum type, const std::string& source);
			static uint32_t createShader(const std::unordered_map<GLenum, std::string>& shaderSources);
			//void loadBinary(std::string_view name, GLint size, GLenum format);//commented that out for now, gonna implement that later
		};
}