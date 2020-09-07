#pragma once

#include "Engine/input.hpp"

namespace Engine
{
	class windowsInput : public input
	{
	protected:
		virtual bool isKeyDownImpl(int keycode) override;

		virtual bool isMouseButtonDownImpl(int button) override;
		virtual std::pair<float, float> getMousePosImpl() override;
		virtual float getMouseXImpl() override;
		virtual float getMouseYImpl() override;
	public:

	private:
	};
}