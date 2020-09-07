#pragma once

#include "core.hpp"

namespace Engine
{
	class ENGINE_API input
	{
	public:
		inline static bool isKeyDown(int keycode) { return s_instance->isKeyDownImpl(keycode); }

		inline static bool isMouseButtonDown(int button) { return s_instance->isMouseButtonDownImpl(button); }
		inline static std::pair<float, float> getMousePos() { return s_instance->getMousePosImpl(); }
		inline static float getMouseX() { return s_instance->getMouseXImpl(); }
		inline static float getMouseY() { return s_instance->getMouseYImpl(); }

	protected://these functions get implemented per platform
		virtual bool isKeyDownImpl(int keycode) = 0;

		virtual bool isMouseButtonDownImpl(int button) = 0;
		virtual std::pair<float, float> getMousePosImpl() = 0;
		virtual float getMouseXImpl() = 0;
		virtual float getMouseYImpl() = 0;

	private:
		static input* s_instance;
	};
}