#include "Engpch.hpp"
#include "material.hpp"
#include "renderer.hpp"

#include "imgui.h"

namespace Engine
{
	Ref_ptr<material> material::create(Ref_ptr<shader>& shader, const std::string& name, uint32_t flags, uint32_t blend_src, uint32_t blend_dest)
	{
		return std::make_shared<material>(shader, name, flags, blend_src, blend_dest);
	}

	material::material(Ref_ptr<shader>& shader, const std::string& name, uint32_t flags, uint32_t blend_src, uint32_t blend_dest)
		: m_name(name), m_properties(flags, blend_src, blend_dest)
	{
		m_shaderRef = shader;
		//reserve memory for the materialUniforms
		if (m_shaderRef->getMaterialUniformsSize())//if there are any uniforms to be set
		{
			m_data = malloc(m_shaderRef->getMaterialUniformsSize());
			m_globalBuffer = globalBuffer::createUnique(m_shaderRef->getMaterialUniformsSize(), STATIC_DRAW);//because the shader-class reads the size directly from shader-source-code we can pass it in here without worries
			const std::unordered_map<std::string, shader::uniformProps>& temp = m_shaderRef->getMaterialUniforms();
			for (const std::pair<const std::string, shader::uniformProps>& it : temp)//add all the layouts to the globalBuffer
			{
				m_globalBuffer->lAdd(it.second.type, it.second.offset);
			}
		}
		if (m_shaderRef->hasTextures())//if the shader has any textures
		{
			const std::vector<std::string>& textures = m_shaderRef->getTextures();
			ENG_CORE_ASSERT(textures.size() <= materialLib::getMaxTexSlots(), "The shader expects more textures than this graphics card supports slots.");
			for (const std::string& it : textures)
			{
				m_textures[it] = nullptr;//initialize the map with nullptrs
			}
		}
	}

	material::~material()
	{
		if(m_shaderRef->getMaterialUniformsSize())//don't free memory that hasn't be allocated!!!(if size is 0 no memory was allocated)
			free(m_data);
	}

	void material::bind(const uint16_t slot)
	{
		//materialUniforms in the "material"-block
		m_shaderRef->bind();
		if (m_shaderRef->getMaterialUniformsSize())//if there even are uniforms to bind(there may be a material, which is just composed of textures)
		{
			m_shaderRef->bindUniformBlock("material", slot);
			m_globalBuffer->bindToPoint(slot);
			m_globalBuffer->unbind();
		}
		//textures
		uint8_t texSlot = 0;
		for (const auto& it : m_shaderRef->getTextures())//range-based for-loop should work with function that returns reference to vector(tested it)
		{
			m_textures[it]->bind(texSlot);
			m_shaderRef->setUniform1i(it, texSlot);
			texSlot++;
		}
	}

	void material::setUniform1bF(const std::string& name, const bool bValue)
	{
		ENG_CORE_ASSERT(m_shaderRef->getMaterialUniforms().count(name), "Tried to update materialUniform but there is no uniform with this name in the shader.");
		//::::::::test this:::::::::
		m_globalBuffer->bind();
		m_globalBuffer->updateElement(m_shaderRef->getMaterialUniforms().find(name)->second.index, (int*)(&bValue));//this may not work because we cast a *bool to an *int
		m_globalBuffer->unbind();
		*(int*)((char*)m_data + m_shaderRef->getMaterialUniforms().find(name)->second.offset) = (int)bValue;
	}
	void material::setUniform1iF(const std::string& name, const int iValue)
	{
		//test this!!!
		ENG_CORE_ASSERT(m_shaderRef->getMaterialUniforms().count(name), "Tried to update materialUniform but there is no uniform with this name in the shader.");
		m_globalBuffer->bind();
		m_globalBuffer->updateElement(m_shaderRef->getMaterialUniforms().find(name)->second.index, &iValue);
		m_globalBuffer->unbind();
		*(int*)((char*)m_data + m_shaderRef->getMaterialUniforms().find(name)->second.offset) = iValue;
	}
	void material::setUniform1fF(const std::string& name, const float fValue)
	{
		//test this!!!
		ENG_CORE_ASSERT(m_shaderRef->getMaterialUniforms().count(name), "Tried to update materialUniform but there is no uniform with this name in the shader.");
		m_globalBuffer->bind();
		m_globalBuffer->updateElement(m_shaderRef->getMaterialUniforms().find(name)->second.index, &fValue);
		m_globalBuffer->unbind();
		*(float*)((char*)m_data + m_shaderRef->getMaterialUniforms().find(name)->second.offset) = fValue;
	}
	void material::setUniform2fF(const std::string& name, const float v0, const float v1)
	{
		//test this!!!
		ENG_CORE_ASSERT(m_shaderRef->getMaterialUniforms().count(name), "Tried to update materialUniform but there is no uniform with this name in the shader.");
		float* temp = (float*)((char*)m_data + m_shaderRef->getMaterialUniforms().find(name)->second.offset);
		*temp = v0; temp++;
		*temp = v1;
		m_globalBuffer->bind();
		m_globalBuffer->updateElement(m_shaderRef->getMaterialUniforms().find(name)->second.index, temp);
		m_globalBuffer->unbind();
	}
	void material::setUniform3fF(const std::string& name, const float v0, const float v1, const float v2)
	{
		//test this!!!
		ENG_CORE_ASSERT(m_shaderRef->getMaterialUniforms().count(name), "Tried to update materialUniform but there is no uniform with this name in the shader.");
		float* temp = (float*)((char*)m_data + m_shaderRef->getMaterialUniforms().find(name)->second.offset);
		*temp = v0; temp++;
		*temp = v1; temp++;
		*temp = v2;
		m_globalBuffer->bind();
		m_globalBuffer->updateElement(m_shaderRef->getMaterialUniforms().find(name)->second.index, temp);
		m_globalBuffer->unbind();
	}
	void material::setUniform3fF(const std::string& name, const vec3& vec)
	{
		//test this!!!
		ENG_CORE_ASSERT(m_shaderRef->getMaterialUniforms().count(name), "Tried to update materialUniform but there is no uniform with this name in the shader.");
		m_globalBuffer->bind();
		m_globalBuffer->updateElement(m_shaderRef->getMaterialUniforms().find(name)->second.index, &vec);
		m_globalBuffer->unbind();
		memcpy(((char*)m_data + m_shaderRef->getMaterialUniforms().find(name)->second.offset), &vec, sizeof(vec3));//copy the vector's memory into the buffer
	}
	void material::setUniform4fF(const std::string& name, const float v0, const float v1, const float v2, const float v3)
	{
		ENG_CORE_ASSERT(m_shaderRef->getMaterialUniforms().count(name), "Tried to update materialUniform but there is no uniform with this name in the shader.");
		float* temp = (float*)((char*)m_data + m_shaderRef->getMaterialUniforms().find(name)->second.offset);
		*temp = v0; temp++;
		*temp = v1; temp++;
		*temp = v2; temp++;
		*temp = v3;
		//probably an error somewhere here
		m_globalBuffer->bind();
		m_globalBuffer->updateElement(m_shaderRef->getMaterialUniforms().find(name)->second.index, temp);
		m_globalBuffer->unbind();
	}
	void material::setUniform4fF(const std::string& name, const vec4& vec)
	{
		//test this!!!
		ENG_CORE_ASSERT(m_shaderRef->getMaterialUniforms().count(name), "Tried to update materialUniform but there is no uniform with this name in the shader.");
		m_globalBuffer->bind();
		m_globalBuffer->updateElement(m_shaderRef->getMaterialUniforms().find(name)->second.index, &vec);
		m_globalBuffer->unbind();
		memcpy(((char*)m_data + m_shaderRef->getMaterialUniforms().find(name)->second.offset), &vec, sizeof(vec4));//copy the vector's memory into the buffer
	}
	void material::setUniformMat4F(const std::string& name, const mat4& mat)
	{
		//test this!!!
		ENG_CORE_ASSERT(m_shaderRef->getMaterialUniforms().count(name), "Tried to update materialUniform but there is no uniform with this name in the shader.");
		m_globalBuffer->bind();
		m_globalBuffer->updateElement(m_shaderRef->getMaterialUniforms().find(name)->second.index, &mat);
		m_globalBuffer->unbind();
		memcpy(((char*)m_data + m_shaderRef->getMaterialUniforms().find(name)->second.offset), &mat, sizeof(mat4));//copy the vector's memory into the buffer
	}
	void material::setUniform1b(const std::string& name, const bool bValue)
	{
		ENG_CORE_ASSERT(m_shaderRef->getMaterialUniforms().count(name), "Tried to update materialUniform but there is no uniform with this name in the shader.");
		//::::::::test this:::::::::
		*(int*)((char*)m_data + m_shaderRef->getMaterialUniforms().find(name)->second.offset) = (int)bValue;
	}
	void material::setUniform1i(const std::string& name, const int iValue)
	{
		ENG_CORE_ASSERT(m_shaderRef->getMaterialUniforms().count(name), "Tried to update materialUniform but there is no uniform with this name in the shader.");
		//::::::::test this:::::::::
		*(int*)((char*)m_data + m_shaderRef->getMaterialUniforms().find(name)->second.offset) = iValue;
	}
	void material::setUniform1f(const std::string& name, const float fValue)
	{
		ENG_CORE_ASSERT(m_shaderRef->getMaterialUniforms().count(name), "Tried to update materialUniform but there is no uniform with this name in the shader.");
		//::::::::test this:::::::::
		*(float*)((char*)m_data + m_shaderRef->getMaterialUniforms().find(name)->second.offset) = fValue;
	}
	void material::setUniform2f(const std::string& name, const float v0, const float v1)
	{
		ENG_CORE_ASSERT(m_shaderRef->getMaterialUniforms().count(name), "Tried to update materialUniform but there is no uniform with this name in the shader.");
		//::::::::test this:::::::::
		float* temp = (float*)((char*)m_data + m_shaderRef->getMaterialUniforms().find(name)->second.offset);
		*temp = v0; temp++;
		*temp = v1;
	}
	void material::setUniform3f(const std::string& name, const vec3& vec)
	{
		ENG_CORE_ASSERT(m_shaderRef->getMaterialUniforms().count(name), "Tried to update materialUniform but there is no uniform with this name in the shader.");
		//::::::::test this:::::::::
		memcpy(((char*)m_data + m_shaderRef->getMaterialUniforms().find(name)->second.offset), &vec, sizeof(vec3));//copy the vector's memory into the buffer
	}
	void material::setUniform3f(const std::string& name, const float v0, const float v1, const float v2)
	{
		ENG_CORE_ASSERT(m_shaderRef->getMaterialUniforms().count(name), "Tried to update materialUniform but there is no uniform with this name in the shader.");
		//::::::::test this:::::::::
		float* temp = (float*)((char*)m_data + m_shaderRef->getMaterialUniforms().find(name)->second.offset);
		*temp = v0; temp++;
		*temp = v1; temp++;
		*temp = v2;
	}
	void material::setUniform4f(const std::string& name, const vec4& vec)
	{
		ENG_CORE_ASSERT(m_shaderRef->getMaterialUniforms().count(name), "Tried to update materialUniform but there is no uniform with this name in the shader.");
		memcpy(((char*)m_data + m_shaderRef->getMaterialUniforms().find(name)->second.offset), &vec, sizeof(vec4));//copy the vector's memory into the buffer
	}
	void material::setUniform4f(const std::string& name, const float v0, const float v1, const float v2, const float v3)
	{
		ENG_CORE_ASSERT(m_shaderRef->getMaterialUniforms().count(name), "Tried to update materialUniform but there is no uniform with this name in the shader.");
		float* temp = (float*)((char*)m_data + m_shaderRef->getMaterialUniforms().find(name)->second.offset);
		*temp = v0; temp++;
		*temp = v1; temp++;
		*temp = v2; temp++;
		*temp = v3;
	}
	void material::setUniformMat4(const std::string& name, const mat4& mat)
	{
		ENG_CORE_ASSERT(m_shaderRef->getMaterialUniforms().count(name), "Tried to update materialUniform but there is no uniform with this name in the shader.");
		//::::::::test this:::::::::
		memcpy(((char*)m_data + m_shaderRef->getMaterialUniforms().find(name)->second.offset), &mat, sizeof(mat4));//copy the matrix' memory into the buffer
	}

	void material::setTexture(const std::string& name, const Ref_ptr<texture>& tex)
	{
		ENG_CORE_ASSERT(m_textures.count(name), "Tried to set texture that the shader doesn't expect.");//make sure the shader even accepts this texture
		m_textures[name] = tex;
	}

	void material::flushAll() const
	{
		m_globalBuffer->bind();
		m_globalBuffer->updateAll(m_data);
		m_globalBuffer->unbind();
	}

	//:::::::::MATERIAL::LIB:::::::::::::
	unsigned int materialLib::s_maxMats = 0;
	uint8_t materialLib::s_maxTexSlots = 0;

	void materialLib::init()
	{
		s_maxMats = renderCommand::getMaxGlobalBuffers();//renderCommand directly accesses the static instance of RendererAPI and gets the max number of GlobalBuffers from it
		s_maxTexSlots = renderCommand::getMaxTextureBinds();//get the maximum number of textures that can be bind simultaneously
	}

	void materialLib::clear()
	{
		m_dynamicMats.clear();
		m_materials.clear();
	}

	void materialLib::add(const Ref_ptr<material>& material)
	{
		if (!m_materials.count(material->getName()))//if no shader with this name is currently loaded
		{
			m_materials[material->getName()] = material;
			return;
		}
		ENG_CORE_WARN("Material \"{0}\" was already in the library.", material->getName());
	}

	void materialLib::add(const Ref_ptr<material>& material, const std::string& name)
	{
		if (!m_materials.count(name))
		{
			m_materials[name] = material;
			return;
		}
		ENG_CORE_WARN("Material \"{0}\" was already in the library.", name);
	}

	void materialLib::addDynamic(const Ref_ptr<material>& Material, const std::function<void(const void* updateValues, material* mat)>& updateFunc, const void* updateValues)
	{
		if (!m_materials.count(Material->getName()))//if no shader with this name is currently loaded
		{
			m_materials[Material->getName()] = Material;
			m_dynamicMats[Material->getName()].mat = Material;
			m_dynamicMats[Material->getName()].updateValues = updateValues;
			m_dynamicMats[Material->getName()].updateFunc = updateFunc;
			return;
		}
		ENG_CORE_WARN("Material \"{0}\" was already in the library.", Material->getName());
	}

	void materialLib::updateDynamic()
	{
		for (auto& mat : m_dynamicMats)
		{
			mat.second.updateFunc(mat.second.updateValues, mat.second.mat.get());
		}
	}

	Ref_ptr<material> materialLib::get(const std::string& name)
	{
		ENG_CORE_ASSERT(m_materials.count(name), "Error when trying to get material \"{0}\". Was not loaded.", name);
		return m_materials[name];
	}

	void materialLib::name(const std::string& nameOld, const std::string& nameNew)
	{
		ENG_CORE_ASSERT(m_materials.count(nameOld), "Tried to rename material that was not present in the library.");
		auto temp = m_materials.extract(nameOld);
		temp.key() = nameNew;
		m_materials.insert(std::move(temp));
		if (m_dynamicMats.count(nameOld))
		{
			auto temp_2 = m_dynamicMats.extract(nameOld);
			temp_2.key() = nameNew;
			m_dynamicMats.insert(std::move(temp_2));
		}
	}

	bool materialLib::exists(const std::string& name)
	{
		return (bool)m_materials.count(name);//since this is no multimap, count() can only return 0 or 1, so perfect for a bool
	}

	void materialLib::remove(const std::string& name)
	{
		ENG_CORE_ASSERT(m_materials.count(name), "Tried to remove a material that wasn't in the library.");
		m_materials.erase(name);
		if (m_dynamicMats.count(name))
			m_dynamicMats.erase(name);
	}

	void materialLib::onImGuiRender()
	{
		ImGui::Begin("Materials");
		//ImVec2 p = ImGui::GetCursorScreenPos();
		//ImVec2 dest(p.x + 300, p.y + 150);
		//ImGui::GetWindowDrawList()->AddBezierCurve(p, ImVec2((5 * p.x + dest.x) / 6, (p.y + dest.y) / 2), ImVec2((p.x + 5 * dest.x) / 6, (p.y + dest.y) / 2), dest, IM_COL32(100, 100, 200, 255), 3.0f);
		for (const auto& mat : m_materials)
		{
			bool isOpen = ImGui::TreeNode(mat.second->getName().c_str());
			if (isOpen)
			{
				if (mat.second->getShader()->getMaterialUniformsSize())
				{
					const unsigned char* data = (unsigned char*)mat.second->getData();
					const std::unordered_map<std::string, shader::uniformProps> uniforms = mat.second->getShader()->getMaterialUniforms();
					for (const auto& uniform : uniforms)
					{
						renderComponent(uniform, data);
					}
					mat.second->flushAll();//update the material after all the values have been set in the rendering functions
				}
				uint32_t flags_packed = mat.second->getFlags();
				ImGui::CheckboxFlags("depth_test", &flags_packed, flag_depth_test);
				ImGui::CheckboxFlags("cast_shadow", &flags_packed, flag_cast_shadow);
				ImGui::CheckboxFlags("no_backface_cull", &flags_packed, flag_no_backface_cull);
				mat.second->setFlags(flags_packed);
				ImGui::TreePop();
			}
		}
		ImGui::End();
	}

	void materialLib::renderComponent(const std::pair<std::string, shader::uniformProps>& props, const unsigned char* data)
	{
		switch (props.second.type)
		{
		case(ENG_FLOAT):
		{
			ImGui::DragFloat(props.first.c_str(), (float*)(data + props.second.offset), 2.0f, 0.0f, 100.0f);
		}break;
		case(ENG_INT):
		{
			ImGui::DragInt(props.first.c_str(), (int*)(data + props.second.offset), 2.0f, -100.0f, 100.0f);
		}break;
		case(ENG_BOOL):
		{
			ImGui::Checkbox(props.first.c_str(), (bool*)(data + props.second.offset));
		}break;
		case(ENG_FLOAT_VEC2):
		{
			ImGui::DragFloat2(props.first.c_str(), (float*)(data + props.second.offset), 2.0f, -100.0f, 100.0f);
		}break;
		case(ENG_FLOAT_VEC3):
		{
			ImGui::ColorEdit3(props.first.c_str(), (float*)(data + props.second.offset));
		}break;
		case(ENG_FLOAT_VEC4):
		{
			ImGui::ColorEdit4(props.first.c_str(), (float*)(data + props.second.offset));
		}break;
		default:
		{
			ImGui::Text("Unknown material property type.");
		}break;
		}
	}
}