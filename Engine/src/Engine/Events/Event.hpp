#pragma once

#include "Engpch.hpp"
#include "Engine/core.hpp"

//right now events are processed immediately when occuring
//in the future a better approach could be to get them inside some que
//and then process them as part of some update-process(for example each frame)

namespace Engine
{
	enum EventType
	{
		NONE = 0,
		WindowClose, WindowResize, WindowFocus, WindowLostFocus, WindowMoved,
		AppTick, AppUpdate, AppRender,
		KeyPressed, KeyReleased,
		MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseScrolled
	};

	enum EventCategory
	{
		EventCategoryNONE = 0,
		EventCategoryApplication = BIT(1),
		EventCategoryInput = BIT(2),
		EventCategoryKeyboard = BIT(3),
		EventCategoryMouse = BIT(4),
		EventCategoryMouseButton = BIT(5)
	};
//these macros are used to make the implementation of certain derived classes much more easy for us
#define EVENT_CLASS_TYPE(type) static EventType getStaticType() { return EventType::##type; } \
		virtual EventType getEventType() const override { return getStaticType(); } \
		virtual const char* getName() const override { return #type; }

#define EVENT_CLASS_CATEGORY(category) virtual int getCategoryFlags() const override { return category; }

	class ENGINE_API Event//a base-class for events(many event-classes will be derived from this class(thats kinda why all those member-funcs are virtual)
	{
		friend class eventDispatcher;
	public:
		//these functions are virtual, so every eventClass has to implement them on its own
		virtual EventType getEventType() const = 0;
		virtual const char* getName() const = 0;//probably a debug only-thing
		virtual int getCategoryFlags() const = 0;
		virtual std::string toString() const { return getName(); }//return the name as a string(just for debug purposes(not very efficient))
		inline bool handled() const { return m_handled; }
		bool isInCategory(EventCategory category)//returns weather an event is in a given category
		{
			return getCategoryFlags() & category;//we mask out by using the "&"-operator
		}
	protected:
		bool m_handled = false;//we store the information weather this event already got handled
	};

	class eventDispatcher
	{
		template<class T>
		using EventFn = std::function<bool(T&)>;
	public:
		eventDispatcher(Event& event)
			: m_event(event) {  }
		template<class T>
		bool dispatchEvent(EventFn<T> func)
		{
			if (m_event.getEventType() == T::getStaticType())
			{
				m_event.m_handled = func(*(T*)&m_event);
				return true;
			}
			return false;
		}
	private:
		Event& m_event;
	};

	inline std::ostream& operator<<(std::ostream& os, const Event& event)//this just exists so we can log easier
	{
		return os << event.toString();
	}

}