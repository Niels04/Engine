#include "Engpch.hpp"

#include "glCore.hpp" //for glm and makros
#include <stb_image.hpp>//for texture loading

#include "OpenGLTexture.hpp"

namespace Engine
{
	GLtexture2d::GLtexture2d(const std::string& name, const bool sRGB, const uint32_t filterMin, const uint32_t filterMag)
		: m_path("res/textures/")
	{
		m_path.append(name);
		int width, height, channels;
		stbi_set_flip_vertically_on_load(1);
		stbi_uc* data = stbi_load(m_path.c_str(), &width, &height, &channels, 0);
		ENG_CORE_ASSERT(data, "Error, when loading image, \"stbi_load\" returned a nullptr!");
		m_width = width; m_height = height;

		GLenum internalFormat = 0/*how openGL stores the texture*/, dataFormat = 0;
		if (channels == 4)
		{
			if (sRGB)
				internalFormat = GL_SRGB8_ALPHA8;
			else
				internalFormat = GL_RGBA8;
			dataFormat = GL_RGBA;
		}
		else if (channels == 3)
		{
			if (sRGB)
				internalFormat = GL_SRGB8;
			else
				internalFormat = GL_RGB8;
			dataFormat = GL_RGB;
		}
		ENG_CORE_ASSERT(internalFormat && dataFormat, "Eighter \"internalFormat\" or \"dataFormat\" (or both)was 0 when loading a texture.");

		//create the texture in OpenGL:
		GLCALL(glCreateTextures(GL_TEXTURE_2D, 1, &m_renderer_id));
		GLCALL(glTextureStorage2D(m_renderer_id, /*if we had mulitple mipmaps then we would put something other than 1 in for levels*/1, /*define how openGL internally
		stores the texture*/internalFormat, m_width, m_height));
		GLCALL(glTextureParameteri(m_renderer_id, GL_TEXTURE_MIN_FILTER, filterMin));
		GLCALL(glTextureParameteri(m_renderer_id, GL_TEXTURE_MAG_FILTER, filterMag));
		GLCALL(glTextureSubImage2D(m_renderer_id, /*index of the level, we only defined one, so index 0(again, usefull when dealing with mipmaps)*/0, /*offsets are usefull when changing a region of an already uploaded
		texture but we set them to 0 here*/0, 0, m_width, m_height, /*here we supply the format of our data*/dataFormat, /*form in which our data comes(stb_image gives unsigned bytes)*/GL_UNSIGNED_BYTE, data));
		
		stbi_image_free(data);//deallocate the memory returned form stbi_load again
	}

	GLtexture2d::GLtexture2d(const uint32_t width, const uint32_t height, const uint32_t format, const uint32_t filterMin, const uint32_t filterMag, const uint32_t texWrap)
	{
		GLenum internalFormat = format;
		m_width = width; m_height = height;
		GLCALL(glCreateTextures(GL_TEXTURE_2D, 1, &m_renderer_id));
		GLCALL(glTextureStorage2D(m_renderer_id, /*if we had mulitple mipmaps then we would put something other than 1 in for levels*/1, /*define how openGL internally
		stores the texture*/internalFormat, m_width, m_height));
		GLCALL(glTextureParameteri(m_renderer_id, GL_TEXTURE_MIN_FILTER, filterMin));
		GLCALL(glTextureParameteri(m_renderer_id, GL_TEXTURE_MAG_FILTER, filterMag));
		GLCALL(glTextureParameteri(m_renderer_id, GL_TEXTURE_WRAP_S, texWrap));
		GLCALL(glTextureParameteri(m_renderer_id, GL_TEXTURE_WRAP_T, texWrap));
	}

	GLtexture2d::GLtexture2d(const uint32_t width, const uint32_t height, const uint32_t format, const uint32_t filterMin, const uint32_t filterMag, const uint32_t textureWrap, const void* data)
	{
		GLenum internalFormat = format;
		m_width = width; m_height = height;
		GLCALL(glCreateTextures(GL_TEXTURE_2D, 1, &m_renderer_id));
		GLCALL(glBindTexture(GL_TEXTURE_2D, m_renderer_id));
		GLCALL(glTexStorage2D(GL_TEXTURE_2D, /*if we had mulitple mipmaps then we would put something other than 1 in for levels*/1, /*define how openGL internally
		stores the texture*/internalFormat, m_width, m_height));
		GLCALL(glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_width, m_height, GL_RGBA, GL_FLOAT, data));//test if this function actually behaves the way it should
		GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filterMin));
		GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filterMag));
		GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, textureWrap));
		GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, textureWrap));
	}

	GLtexture2d::~GLtexture2d()
	{
		GLCALL(glDeleteTextures(1, &m_renderer_id));
	}

	void GLtexture2d::bind(const uint8_t slot) const
	{
		GLCALL(glBindTextureUnit(slot, m_renderer_id));//binding textures at multiple slots is only usefull when rendering objects, that use multiple textures(because all of the object's textures have to be
		//accessible in the shader at the same time
	}

	void GLtexture2d::clear(const vec4& color) const
	{
		GLCALL(glClearTexImage(m_renderer_id, 0, GL_RGBA, GL_FLOAT, &color));
	}
	
	//SHADOW_MAP_2D::::::::::
	GLShadowMap2d::GLShadowMap2d(const uint32_t width, const uint32_t height)
		: m_width(width), m_height(height)
	{
		GLCALL(glGenTextures(1, &m_renderer_id));
		GLCALL(glBindTexture(GL_TEXTURE_2D, m_renderer_id));
		GLCALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, m_width, m_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr));//maybe switch this with glTexStorage2D
		GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
		GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
		GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER));
		GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER));
		float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };//set border color, so that stuff outside the light's frustum is never in shadow
		GLCALL(glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor));
	}
	GLShadowMap2d::~GLShadowMap2d()
	{
		GLCALL(glDeleteTextures(1, &m_renderer_id));
	}

	void GLShadowMap2d::bind(const uint8_t slot) const
	{
		GLCALL(glBindTextureUnit(slot, m_renderer_id));
	}

	void GLShadowMap2d::clear(const vec4& color) const
	{
		GLCALL(glClearTexImage(m_renderer_id, 0, GL_RGBA, GL_FLOAT, &color));
	}

	//SHADOW_MAP_3D:::::::
	GLShadowMap3d::GLShadowMap3d(const uint32_t width, const uint32_t height)
		: m_width(width), m_height(height)
	{
		GLCALL(glGenTextures(1, &m_renderer_id));
		GLCALL(glBindTexture(GL_TEXTURE_CUBE_MAP, m_renderer_id));
		for (uint8_t i = 0; i < 6; i++)
		{
			GLCALL(glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, m_width, m_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr));
		}
		GLCALL(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
		GLCALL(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
		GLCALL(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
		GLCALL(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
		GLCALL(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE));
	}
	GLShadowMap3d::~GLShadowMap3d()
	{
		GLCALL(glDeleteTextures(1, &m_renderer_id));
	}

	void GLShadowMap3d::bind(const uint8_t slot) const
	{
		GLCALL(glBindTextureUnit(slot, m_renderer_id));
	}

	void GLShadowMap3d::clear(const vec4& color) const
	{
		GLCALL(glClearTexImage(m_renderer_id, 0, GL_RGBA, GL_FLOAT, &color));
	}
	//ShadowMap2dArray::::::::::::
	GLShadowMap2dArray::GLShadowMap2dArray(const uint32_t width, const uint32_t height, const uint8_t layerCount)
	{
		GLCALL(glGenTextures(1, &m_renderer_id));
		GLCALL(glBindTexture(GL_TEXTURE_2D_ARRAY, m_renderer_id));
		GLCALL(glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, GL_DEPTH_COMPONENT32, width, height, static_cast<uint32_t>(layerCount)));//<-maybe use GL_DEPTH_COMPONENT without the 32, or with the F in the back
		GLCALL(glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
		GLCALL(glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
		GLCALL(glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER));
		GLCALL(glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER));
		float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };//set border color, so that stuff outside the light's frustum is never in shadow
		GLCALL(glTexParameterfv(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_BORDER_COLOR, borderColor));
	}
	GLShadowMap2dArray::~GLShadowMap2dArray()
	{
		GLCALL(glDeleteTextures(1, &m_renderer_id));
	}

	void GLShadowMap2dArray::bind(const uint8_t slot) const
	{
		GLCALL(glBindTextureUnit(slot, m_renderer_id));
	}
	//ShadowMap3dArray::::::::::::
	GLShadowMap3dArray::GLShadowMap3dArray(const uint32_t size, const uint8_t layerCount)
	{
		GLCALL(glGenTextures(1, &m_renderer_id));
		GLCALL(glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, m_renderer_id));
		GLCALL(glTexStorage3D(GL_TEXTURE_CUBE_MAP_ARRAY, 1, GL_DEPTH_COMPONENT32, size, size, layerCount * 6));//<-maybe use GL_DEPTH_COMPONENT without the 32, or with the F in the back
		GLCALL(glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
		GLCALL(glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
		GLCALL(glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
		GLCALL(glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
		GLCALL(glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE));
	}
	GLShadowMap3dArray::~GLShadowMap3dArray()
	{
		GLCALL(glDeleteTextures(1, &m_renderer_id));
	}

	void GLShadowMap3dArray::bind(const uint8_t slot) const
	{
		GLCALL(glBindTextureUnit(slot, m_renderer_id));
	}
}