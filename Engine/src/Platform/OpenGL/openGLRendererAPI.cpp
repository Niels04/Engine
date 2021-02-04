#include "Engpch.hpp"
#include "openGLRendererAPI.hpp"

#include "glCore.hpp"

namespace Engine
{
	void OpenGLRendererAPI::init() const
	{
		//setup blending
		GLCALL(glEnable(GL_BLEND));
		GLCALL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
		//setup depth-testing
		GLCALL(glEnable(GL_DEPTH_TEST));
		GLCALL(glDepthFunc(GL_LESS));//maybe because I scale between -1 and 1 but the depthbuffer accepts values between 0 and 1
		GLCALL(glEnable(GL_CULL_FACE));
		GLCALL(glCullFace(GL_BACK));
	}

	void OpenGLRendererAPI::clear() const
	{
		GLCALL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
	}

	void OpenGLRendererAPI::clearDepth() const
	{
		GLCALL(glClear(GL_DEPTH_BUFFER_BIT));
	}

	void OpenGLRendererAPI::setClearColor(const glm::vec4& color) const
	{
		GLCALL(glClearColor(color.r, color.g, color.b, color.a));
	}

	void OpenGLRendererAPI::drawIndexed(const Ref_ptr<vertexArray> va) const
	{
		GLCALL(glDrawElements(GL_TRIANGLES, va->getIb()->getCount(), GL_UNSIGNED_INT, nullptr));
	}

	void OpenGLRendererAPI::setViewport(const uint32_t width, const uint32_t height) const
	{
		GLCALL(glViewport(0, 0, width, height));
	}

	void OpenGLRendererAPI::setBlend(const uint32_t sfactor, const uint32_t dfactor) const
	{
		GLCALL(glBlendFunc(sfactor, dfactor));
	}

	void OpenGLRendererAPI::setDepth(const uint32_t method) const
	{
		GLCALL(glDepthFunc(method));
	}

	void OpenGLRendererAPI::cullFace(uint32_t face) const
	{
		GLCALL(glCullFace(face));
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