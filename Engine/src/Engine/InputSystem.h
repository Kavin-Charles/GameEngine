#pragma once

#include "Engine/Core.h"
#include <utility>

namespace Engine {

	class Window;

	class ENGINE_API InputSystem
	{
	public:
		InputSystem(Window& window);

		bool IsKeyPressed(int keycode) const;
		bool IsMouseButtonPressed(int button) const;
		std::pair<float, float> GetMousePosition() const;
		float GetMouseX() const;
		float GetMouseY() const;

	private:
		Window& m_Window;
	};
}
