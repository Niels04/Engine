#pragma once

#include "Engine/window.hpp"

namespace Engine
{
	class graphicsContext
	{
	public:
		virtual void init() = 0;
		virtual void swapBuffers() const  = 0;

		static graphicsContext* create(window* window);
	};
}