#pragma once

#include "Event.hpp"

namespace Engine
{
	class ENGINE_API mouseMoveEvent : public Event
	{
	public:
		mouseMoveEvent(float x, float y)
			: m_mouseX(x), m_mouseY(y) {  }
		inline float getX() const { return m_mouseX; }
		inline float getY() const { return m_mouseY; }

		std::string toString() const override
		{
			std::stringstream ss;
			ss << "MouseMoveEvent: X = " << m_mouseX << " Y = " << m_mouseY;
			return ss.str();
		}

		EVENT_CLASS_TYPE(MouseMoved)
		EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)
	private:
		float m_mouseX, m_mouseY;
	};

	class ENGINE_API mouseScrollEvent : public Event
	{
	public:
		mouseScrollEvent(float Xoffset, float Yoffset)
			: m_Xoffset(Xoffset), m_Yoffset(Yoffset) {  }

		inline float getXoffset() const { return m_Xoffset; }
		inline float getYoffset() const { return m_Yoffset; }

		std::string toString() const override
		{
			std::stringstream ss;
			ss << "MouseScollEvent: Xoffset = " << m_Xoffset << " Yoffset = " << m_Yoffset;
			return ss.str();
		}
		EVENT_CLASS_TYPE(MouseScrolled)
		EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)

	private:
		float m_Xoffset, m_Yoffset;
	};

	class ENGINE_API mouseButtonEvent : public Event
	{
	public:
		inline int getMouseButton() const { return m_button; }

		EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryMouseButton)
	protected:
		mouseButtonEvent(int button)
			: m_button(button) {  }

		int m_button;
	};

	class ENGINE_API mouseButtonPressedEvent : public mouseButtonEvent
	{
	public:
		mouseButtonPressedEvent(int button)
			: mouseButtonEvent(button) {  }

		std::string toString() const override
		{
			std::stringstream ss;
			ss << "MouseButtonPressedEvent: " << m_button;
			return ss.str();
		}
		EVENT_CLASS_TYPE(MouseButtonPressed)
	};

	class ENGINE_API mouseButtonReleasedEvent : public mouseButtonEvent
	{
	public:
		mouseButtonReleasedEvent(int button)
			: mouseButtonEvent(button) {  }

		std::string toString() const override
		{
			std::stringstream ss;
			ss << "MouseButtonReleasedEvent:" << m_button;
			return ss.str();
		}
		EVENT_CLASS_TYPE(MouseButtonReleased)
	};

}