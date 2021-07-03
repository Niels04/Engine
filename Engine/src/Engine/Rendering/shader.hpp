#pragma once
#include "Engpch.hpp"
#include "LightMatrixStructs.hpp"

namespace Engine
{
	class shader
	{
	public:
		struct uniformProps
		{
			uint8_t index;
			uint16_t offset;
			uint32_t type;
		};

		virtual ~shader() {  }

		virtual void bind() const = 0;
		virtual void unbind() const = 0;

		static Ref_ptr<shader> create(const std::string& fileName);
		static Ref_ptr<shader> create(const std::string& fileName, const std::string& name);

		virtual const std::string& getName() const = 0;
		virtual void setName(const std::string& nameNew) = 0;

		virtual const std::unordered_map<std::string, uniformProps>& getMaterialUniforms() const = 0;
		virtual const uint16_t getMaterialUniformsSize() const = 0;
		virtual const std::vector<std::string>& getTextures() const = 0;
		virtual const bool hasTextures() const = 0;

		//maybe remove all of these in the future, because they only apply for openGL
		virtual void setUniform1b(const std::string& name, const bool bValue) = 0;

		virtual void setUniform1i(const std::string& name, const int value) = 0;

		virtual void setUniform1ui(const std::string& name, const uint32_t value) = 0;

		virtual void setUniform1f(const std::string& name, const float fValue) = 0;
		virtual void setUniform2f(const std::string& name, const float v0, const float v1) = 0;
		virtual void setUniform3f(const std::string& name, const float v0, const float v1, const float v2) = 0;
		virtual void setUniform3f(const std::string& name, const vec3& vec) = 0;
		virtual void setUniform4f(const std::string& name, const float v0, const float v1, const float v2, const float v3) = 0;
		virtual void setUniform4f(const std::string& name, const vec4& vec) = 0;

		virtual void setUniform3fArr(const std::string& name, const uint32_t count, const vec3 vecs[]) = 0;

		virtual void setUniformMat4(const std::string& name, const mat4& mat, const uint8_t transpose = 0) = 0;
		virtual void setUniformMat4_3(const std::string& name, const cascadedDirLightMatrices& matrices, const uint8_t transpose = 0) = 0;
		virtual void setUniformMat4_6(const std::string& name, const pointLightMatrices& matrices, const uint8_t transpose = 0) = 0;
		virtual void setUniformMat3(const std::string& name, const mat3& mat, const uint8_t transpose = 0) = 0;

		virtual void bindUniformBlock(const std::string& name, const uint32_t bindingPoint) = 0;
	};

	class shaderLib
	{
	public:
		void add(const Ref_ptr<shader>& shader);//take the smartPointer in as reference, because it will get copied when it is pushed into the map(so we don't make an extra ref-count for no reason)
		void add(const Ref_ptr<shader>& shader, const std::string& name);//inserts the shader into the lib under keyVal "name" (does not change the shader's internal name)
		Ref_ptr<shader> get(const std::string& name);//get a shader by keyVal "name" (it is not guarranted that the shader's internal name corresponds to the keyVal it's saved under)
		Ref_ptr<shader> load(const std::string& fileName);
		void name(const std::string& nameOld, const std::string& nameNew);//save the shader that is currently stored with keyVal "nameOld" with the new keyval "nameNew" (does not change the shader's internal name)
		bool exists(const std::string& name);
		void clear();//remove all shaders from the shaderLib
	private:
		std::unordered_map<std::string, Ref_ptr<shader>> m_shaders;//first is name, second is corresponding shader
	};
}