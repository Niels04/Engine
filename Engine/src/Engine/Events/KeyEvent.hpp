#pragma once

#include "Event.hpp"

namespace Engine
{
	//this is really just an abstract event, that can only be constructed, by a class, that derives from it(protected constructor)->this is just a base class
	class ENGINE_API keyEvent : public Event
	{
	public:
		inline int getKeyCode() { return m_keycode; }

		EVENT_CLASS_CATEGORY(EventCategoryKeyboard | EventCategoryInput)//we "or" them together(a keyboardEvent is also an inputEvent)
	protected:
		keyEvent(int keycode)
			: m_keycode(keycode) {  }
		int m_keycode;
	};

	class ENGINE_API keyPressedEvent : public keyEvent
	{
	public:
		keyPressedEvent(int keycode, int repeatCount)
			: keyEvent(keycode)/*we call the protected constructor of the base class here*/, m_repeatCount(repeatCount) {  }
		inline int getRepeatCount() const { return m_repeatCount; }
		std::string toString() const override
		{
			std::stringstream ss;
			ss << "KeyPressedEvent: " << m_keycode << " (" << m_repeatCount << " repeats)";
			return ss.str();
		}
		EVENT_CLASS_TYPE(KeyPressed)
	private:
		int m_repeatCount;
	};

	class ENGINE_API keyReleasedEvent : public keyEvent
	{
	public:
		keyReleasedEvent(int keycode)
			: keyEvent(keycode) {  }
		std::string toString() const override
		{
			std::stringstream ss;
			ss << "KeyReleasedEvent: " << m_keycode;
			return ss.str();
		}
		EVENT_CLASS_TYPE(KeyReleased)
	};
}