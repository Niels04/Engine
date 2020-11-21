#pragma once

#include "Engine/Rendering/texture.hpp"

namespace Engine
{
	class GLtexture2d : public texture2d
	{
	public:
		GLtexture2d(const std::string& name);
		~GLtexture2d();

		inline uint32_t getHeight() const override { return m_height; }
		inline uint32_t getWidth() const override { return m_width; }

		inline void bind(const uint8_t slot = 0) const override;
		inline void unbind() const override {  }
	private:
		std::string m_path;//usefull for debugging and development-> easy to reload a texture on the fly
		uint32_t m_height;
		uint32_t m_width;
		uint32_t m_renderer_id;
	};
}