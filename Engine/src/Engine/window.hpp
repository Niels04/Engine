#pragma once

#include "Engpch.hpp"

#include "core.hpp"
#include "Events/Event.hpp"

namespace Engine
{
	struct windowProps
	{
		std::string title;
		unsigned int height;
		unsigned int width;

		windowProps(const std::string& Title = "GameEngine", unsigned int Height = 720, unsigned int Width = 1080)
			: title(Title), height(Height), width(Width)
		{

		}
	};

	//representation of a desktop-based-window
	//this is pure virtual and no data stored here, everything needs to be derived platform-specific from here
	class ENGINE_API window
	{
	public:
		using EventCallbackFn = std::function<void(Event&)>;

		virtual ~window() {  }

		virtual void onUpdate() = 0;

		virtual unsigned int getHeight() const = 0;
		virtual unsigned int getWidth() const = 0;

		//window attribures:
		virtual void setEventCallbackFn(const EventCallbackFn& callback) = 0;
		virtual void setVsync(const bool enabled) = 0;
		virtual bool isVsync() const = 0;
		virtual void setDissableCursor(bool enabled) = 0;
		virtual bool isDissableCursor() const = 0;
		virtual void setFullscreen(const bool enabled) = 0;
		virtual bool isFullscreen() const = 0;
		virtual void setRawMouseInput(const bool enabled) = 0;
		virtual bool isRawMouseInput() const = 0;
		
		virtual void* getNativeWindow() const = 0;

		static window* create(const windowProps& windowProps = windowProps());
	};
}