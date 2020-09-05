#pragma once

#include "Event.hpp"

namespace Engine
{
	class ENGINE_API windowResizeEvent : public Event
	{
	public:
		windowResizeEvent(int height, int width)
			: m_height(height), m_width(width) {  }
		inline int getHeight() const { return m_height; }
		inline int getWidth() const { return m_width; }

		std::string toString() const override
		{
			std::stringstream ss;
			ss << "WindowResizeEvent: Height = " << m_height << " Width = " << m_width;
			return ss.str();
		}
		EVENT_CLASS_TYPE(WindowResize)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)
	private:
		int m_height, m_width;
	};

	class ENGINE_API windowCloseEvent : public Event
	{
	public:
		windowCloseEvent() {  }

		EVENT_CLASS_TYPE(WindowClose)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)
	};

	class ENGINE_API appTickEvent : public Event
	{
	public:
		appTickEvent() {  }

		EVENT_CLASS_TYPE(AppTick)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)
	};

	class ENGINE_API appUpdateEvent : public Event
	{
	public:
		appUpdateEvent() {  }

		EVENT_CLASS_TYPE(AppUpdate)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)
	};

	class ENGINE_API appRenderEvent : public Event
	{
	public:
		appRenderEvent() {  }

		EVENT_CLASS_TYPE(AppRender)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)
	};

}