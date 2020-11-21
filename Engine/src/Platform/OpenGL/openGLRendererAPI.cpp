#include "Engpch.hpp"
#include "openGLRendererAPI.hpp"

#include "glCore.hpp"

namespace Engine
{
	void OpenGLRendererAPI::init()
	{
		//setup blending
		GLCALL(glEnable(GL_BLEND));
		GLCALL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
	}

	void OpenGLRendererAPI::clear()
	{
		GLCALL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
	}

	void OpenGLRendererAPI::setClearColor(const glm::vec4& color)
	{
		GLCALL(glClearColor(color.r, color.g, color.b, color.a));
	}

	void OpenGLRendererAPI::drawIndexed(const Ref_ptr<GLvertexArray> va)
	{
		GLCALL(glDrawElements(GL_TRIANGLES, va->p_indexBuffer->getCount(), GL_UNSIGNED_INT, nullptr));
	}
}