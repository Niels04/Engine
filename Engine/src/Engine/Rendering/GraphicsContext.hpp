#pragma once

namespace Engine
{
	class graphicsContext
	{
	public:
		virtual void init() = 0;
		virtual void swapBuffers() const  = 0;
	protected:
	};
}