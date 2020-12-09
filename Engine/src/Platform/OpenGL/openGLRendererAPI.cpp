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
		//setup depth-testing
		GLCALL(glEnable(GL_DEPTH_TEST));
		GLCALL(glDepthFunc(GL_LESS));//maybe because I scale between -1 and 1 but the depthbuffer accepts values between 0 and 1
	}

	void OpenGLRendererAPI::clear()
	{
		GLCALL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
	}

	void OpenGLRendererAPI::setClearColor(const glm::vec4& color)
	{
		GLCALL(glClearColor(color.r, color.g, color.b, color.a));
	}

	void OpenGLRendererAPI::drawIndexed(const Ref_ptr<vertexArray> va)
	{
		GLCALL(glDrawElements(GL_TRIANGLES, va->getIb()->getCount(), GL_UNSIGNED_INT, nullptr));
	}

	const uint32_t OpenGLRendererAPI::getMaxGlobalBuffers() const
	{
		GLint val;
		GLCALL(glGetIntegerv(GL_MAX_UNIFORM_BUFFER_BINDINGS, &val));
		return val;
	}

	const uint8_t OpenGLRendererAPI::getMaxTextureBinds() const
	{
		GLint val;
		GLCALL(glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &val));
		return val;
	}
}