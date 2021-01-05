#pragma once

#include "Engine/Rendering/texture.hpp"

namespace Engine
{
	class GLtexture2d : public texture2d
	{
	public:
		GLtexture2d(const std::string& name, const uint32_t filterMin, const uint32_t filterMag);
		~GLtexture2d();

		inline const uint32_t getHeight() const override { return m_height; }
		inline const uint32_t getWidth() const override { return m_width; }

		inline void bind(const uint8_t slot = 0) const override;
	private:
		std::string m_path;//usefull for debugging and development-> easy to reload a texture on the fly
		uint32_t m_height;
		uint32_t m_width;
		uint32_t m_renderer_id;
	};

	class GLFrameBufferTexture : public FrameBufferTexture
	{
		friend class GLFrameBuffer;//because the frameBuffer needs to be able to read the renderer_id from the texture
	public:
		GLFrameBufferTexture(const FrameBufferTextureUsage usage, const uint32_t width, const uint32_t height);
		GLFrameBufferTexture(const uint32_t width, const uint32_t height);
		~GLFrameBufferTexture();

		inline void bind(const uint8_t slot = 0) const override;

		inline const uint32_t getHeight() const override { return m_height; }
		inline const uint32_t getWidth() const override { return m_width; }
	private:
		inline const uint32_t getRenderer_id() const { return m_renderer_id; }
		uint32_t m_renderer_id;
		uint32_t m_height;
		uint32_t m_width;
	};
}