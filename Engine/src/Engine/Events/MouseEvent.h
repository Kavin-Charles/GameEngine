#pragma once

#include "Event.h"
#include "sstream"
namespace Engine {

	class ENGINE_API MouseMovedEvent : public Event {
	public:
		MouseMovedEvent(float x, float y) : m_MouseX(x), m_MouseY(y) {}
		inline float getX() { return m_MouseX; }
		inline float getY() { return m_MouseY; }

		std::string ToString() {
			std::stringstream ss;
			ss << "MousedMovedEvent (X:" << m_MouseX << " Y:" << m_MouseY << ")";
			return ss.str();
		}

		EVENT_CLASS_CATEGORY(EventCategoryMouse |EventCategoryInput)
		EVENT_CLASS_TYPE(MouseMoved)
	private:
		float m_MouseX, m_MouseY;
	};
}