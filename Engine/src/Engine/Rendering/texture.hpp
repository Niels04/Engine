#pragma once

#include "Engine/core.hpp"

#include <string>

//forked from OpenGL
#define FILTER_NEAREST 0x2600
#define FILTER_LINEAR 0x2601
//formats
#define ENG_RGBA8 0x8058
#define ENG_SRGB8_ALPHA8 0x8C43
#define ENG_RGB8 0x8051
#define ENG_SRGB8 0x8C41
//used for floating-point-buffers
#define ENG_RGBA32F 0x8814
#define ENG_RGB32F 0x8815
#define ENG_RGBA16F 0x881A
#define ENG_RGB16F 0x881B

namespace Engine
{
	class texture//purely virtual, represents any kind of texture
	{//mostly exist, because sometimes one might want to work with a texture, regardless wether it's 2d of 3d
		friend class GLFrameBuffer;
	public:
		virtual ~texture() = default;

		virtual const uint32_t getHeight() const = 0;
		virtual const uint32_t getWidth() const = 0;

		virtual void bind(const uint8_t slot = 0) const = 0;

	private:
		virtual const uint32_t getRenderer_id() const = 0;
	};

	class texture2d : public texture//represents a 2d-texture, again purely virtual and implemented per graphics-API
	{
	public:
		virtual ~texture2d() = default;
		static Ref_ptr<texture2d> create(const std::string& path, const bool sRGB = false, const uint32_t filterMin = FILTER_LINEAR, const uint32_t filterMag = FILTER_LINEAR);
		static Ref_ptr<texture2d> create(const uint32_t width, const uint32_t height, const uint32_t format, const uint32_t filterMin = FILTER_LINEAR, const uint32_t filterMag = FILTER_LINEAR);
	};

	class ShadowMap2d : public texture
	{
	public:
		virtual ~ShadowMap2d() = default;
		static Ref_ptr<ShadowMap2d> create(const uint32_t width, const uint32_t height);
	};

	class ShadowMap3d : public texture
	{
	public:
		virtual ~ShadowMap3d() = default;
		static Ref_ptr<ShadowMap3d> create(const uint32_t width, const uint32_t height);
	};
}