#include "InputSystem.h"
#include "Window.h"

#include <GLFW/glfw3.h>

namespace Engine {

	InputSystem::InputSystem(Window& window)
		: m_Window(window)
	{
	}

	bool InputSystem::IsKeyPressed(int keycode) const
	{
		auto* glfwWindow = static_cast<GLFWwindow*>(m_Window.GetNativeWindow());
		auto state = glfwGetKey(glfwWindow, keycode);
		return state == GLFW_PRESS || state == GLFW_REPEAT;
	}

	bool InputSystem::IsMouseButtonPressed(int button) const
	{
		auto* glfwWindow = static_cast<GLFWwindow*>(m_Window.GetNativeWindow());
		auto state = glfwGetMouseButton(glfwWindow, button);
		return state == GLFW_PRESS;
	}

	std::pair<float, float> InputSystem::GetMousePosition() const
	{
		auto* glfwWindow = static_cast<GLFWwindow*>(m_Window.GetNativeWindow());
		double xpos, ypos;
		glfwGetCursorPos(glfwWindow, &xpos, &ypos);
		return { (float)xpos, (float)ypos };
	}

	float InputSystem::GetMouseX() const
	{
		auto [x, y] = GetMousePosition();
		return x;
	}

	float InputSystem::GetMouseY() const
	{
		auto [x, y] = GetMousePosition();
		return y;
	}
}
