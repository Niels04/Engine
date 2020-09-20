#include "Engpch.hpp"
#include "openGLRendererAPI.hpp"

#include <glad/glad.h>

namespace Engine
{
	void OpenGLRendererAPI::clear()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void OpenGLRendererAPI::setClearColor(const glm::vec4& color)
	{
		glClearColor(color.r, color.g, color.b, color.a);
	}

	void OpenGLRendererAPI::drawIndexed(const std::shared_ptr<GLvertexArray> va)
	{
		glDrawElements(GL_TRIANGLES, va->p_indexBuffer->getCount(), GL_UNSIGNED_INT, nullptr);
	}
}