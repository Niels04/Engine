#include "Engpch.hpp"
#include "shader.hpp"
#include "renderer.hpp"

#include "Platform/OpenGL/OpenGLShader.hpp"

namespace Engine
{
	Ref_ptr<shader> shader::create(const std::string& fileName)
	{
		switch (Renderer::getAPI())
		{
		case RendererAPI::API::OpenGL:
		{
			return std::make_shared<GLshader>(fileName);
		}break;
		case RendererAPI::API::NONE:
		{
			ENG_CORE_WARN("Tried to create vertexBuffer while RenderingAPI was set to \"NONE\". Returning nullptr.");
			return nullptr;
		}break;
		default:
		{
			ENG_CORE_ASSERT(false, "Unknown rendering-API.");
		}break;
		}
	}

	Ref_ptr<shader> shader::create(const std::string& fileName, const std::string& name)
	{
		switch (Renderer::getAPI())
		{
		case RendererAPI::API::OpenGL:
		{
			return std::make_shared<GLshader>(fileName, name);
		}break;
		case RendererAPI::API::NONE:
		{
			ENG_CORE_WARN("Tried to create vertexBuffer while RenderingAPI was set to \"NONE\". Returning nullptr.");
			return nullptr;
		}break;
		default:
		{
			ENG_CORE_ASSERT(false, "Unknown rendering-API.");
		}break;
		}
	}

	void shaderLib::add(const Ref_ptr<shader>& shader)
	{
		if (!m_shaders.count(shader->getName()))//if no shader with this name is currently loaded
		{
			m_shaders[shader->getName()] = shader;
			return;
		}
		ENG_CORE_WARN("Shader \"{0}\" was already in the library.", shader->getName());
	}

	void shaderLib::add(const Ref_ptr<shader>& shader, const std::string& name)
	{
		if (!m_shaders.count(name))
		{
			m_shaders[name] = shader;
			return;
		}
		ENG_CORE_WARN("Shader\"{0}\" was already in the library.", name);
	}

	Ref_ptr<shader> shaderLib::get(const std::string& name)
	{
		ENG_CORE_ASSERT(m_shaders.count(name), "Error when trying to get shader \"{0}\". Was not loaded.", name);
		return m_shaders[name];
	}

	void shaderLib::clear()
	{	
		m_shaders.clear();
	}

	Ref_ptr<shader> shaderLib::load(const std::string& fileName)
	{
		std::string name = fileName.substr(0, fileName.rfind(".") == std::string::npos ? fileName.size() - 1 : fileName.rfind("."));//find out what the shader's default name should be
		if (!m_shaders.count(name))//if no shader with this name is currently loaded
		{
			Ref_ptr<shader> temp = shader::create(fileName, name);
			m_shaders[temp->getName()] = temp;
			return m_shaders[temp->getName()];
		}
		ENG_CORE_WARN("Shader \"{0}\" was already in the library.", name);
		return m_shaders[name];
	}

	void shaderLib::name(const std::string& nameOld, const std::string& nameNew)
	{
		ENG_CORE_ASSERT(m_shaders.count(nameOld), "Tried to rename shader that was not present in the library.");
		auto temp = m_shaders.extract(nameOld);
		temp.key() = nameNew;
		m_shaders.insert(std::move(temp));
	}

	bool shaderLib::exists(const std::string& name)
	{
		return (bool)m_shaders.count(name);//since this is no multimap, count() can only return 0 or 1, so perfect for a bool
	}
}