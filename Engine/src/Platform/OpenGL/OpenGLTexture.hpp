#pragma once

#include "Engine/Rendering/texture.hpp"

namespace Engine
{
	class GLtexture2d : public texture2d
	{
	public:
		GLtexture2d(const std::string& name, const bool sRGB, const uint32_t filterMin, const uint32_t filterMag);
		GLtexture2d(const uint32_t width, const uint32_t height, const uint32_t format, const uint32_t filterMin, const uint32_t filterMag, const uint32_t texWrap);
		GLtexture2d(const uint32_t width, const uint32_t height, const uint32_t format, const uint32_t filterMin, const uint32_t filterMag, const uint32_t textureWrap, const void* data);
		~GLtexture2d();

		inline const uint32_t getHeight() const override { return m_height; }
		inline const uint32_t getWidth() const override { return m_width; }

		inline void bind(const uint8_t slot = 0) const override;
		inline void clear(const vec4& color) const override;
	private:
		std::string m_path;//usefull for debugging and development-> easy to reload a texture on the fly
		uint32_t m_height, m_width;
		uint32_t m_renderer_id;

		inline const uint32_t getRenderer_id() const override { return m_renderer_id; }
	};

	class GLShadowMap2d : public ShadowMap2d
	{
		friend class GLFrameBuffer;
	public:
		GLShadowMap2d(const uint32_t width, const uint32_t height);
		~GLShadowMap2d();

		inline virtual void bind(const uint8_t slot = 0) const override;
		inline void clear(const vec4& color) const override;

		inline const uint32_t getWidth() const override { return m_width; }
		inline const uint32_t getHeight() const override { return m_height; }
	private:
		uint32_t m_renderer_id;
		inline const uint32_t getRenderer_id() const override { return m_renderer_id; }

		uint32_t m_width, m_height;
	};

	class GLShadowMap3d : public ShadowMap3d
	{
		friend class GLFrameBuffer;
	public:
		GLShadowMap3d(const uint32_t width, const uint32_t height);
		~GLShadowMap3d();

		inline virtual void bind(const uint8_t slot = 0) const override;
		inline void clear(const vec4& color) const override;

		inline const uint32_t getWidth() const override { return m_width; }
		inline const uint32_t getHeight() const override { return m_height; }
	private:
		uint32_t m_renderer_id;
		inline const uint32_t getRenderer_id() const override { return m_renderer_id; }

		uint32_t m_width, m_height;
	};

	class GLShadowMap2dArray : public ShadowMap2dArray
	{
		friend class GLFrameBuffer;
	public:
		GLShadowMap2dArray(const uint32_t width, const uint32_t height, const uint8_t layerCount);
		~GLShadowMap2dArray();
		inline virtual void bind(const uint8_t slot = 0) const override;
		inline virtual void clear(const vec4& color) const override {  }

		inline virtual const uint32_t getWidth() const override { return m_width; }
		inline virtual const uint32_t getHeight() const override { return m_height; }
		inline const uint32_t getLayerCount() const { return m_layerCount; }
	private:
		uint32_t m_renderer_id;
		inline const uint32_t getRenderer_id() const override { return m_renderer_id; }

		uint32_t m_width, m_height;
		uint32_t m_layerCount;
	};

	class GLShadowMap3dArray : public ShadowMap3dArray
	{
		friend class GLFrameBuffer;
	public:
		GLShadowMap3dArray(const uint32_t size, const uint8_t layerCount);
		~GLShadowMap3dArray();
		inline virtual void bind(const uint8_t slot = 0) const override;
		inline virtual void clear(const vec4& color) const override {  }

		inline virtual const uint32_t getWidth() const override { return m_size; }
		inline virtual const uint32_t getHeight() const override { return m_size; }
		inline const uint32_t getLayerCount() const { return m_layerCount; }
	private:
		uint32_t m_renderer_id;
		inline const uint32_t getRenderer_id() const override { return m_renderer_id; };

		uint32_t m_size;
		uint32_t m_layerCount;
	};
}