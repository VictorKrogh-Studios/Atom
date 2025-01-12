#pragma once
#include "Event.h"

#include <sstream>

namespace Atom
{

	class WindowCloseEvent : public Event
	{
	public:
		WindowCloseEvent()
		{
		}

		EVENT_CLASS_TYPE(WindowClose)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)
	};

}