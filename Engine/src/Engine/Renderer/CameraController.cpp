#include "CameraController.h"
#include "Engine/InputSystem.h"

#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>

namespace Engine {

	void CameraController::OnUpdate(float dt, InputSystem& input, Transform& transform)
	{
		// Sync internal yaw/pitch from transform on first call
		if (m_FirstMouse)
		{
			m_Yaw = transform.Rotation.y;
			m_Pitch = transform.Rotation.x;
		}

		auto [mouseX, mouseY] = input.GetMousePosition();

		// --- Mouse look (right-click only) ---
		if (input.IsMouseButtonPressed(GLFW_MOUSE_BUTTON_RIGHT))
		{
			if (m_FirstMouse)
			{
				m_LastMouseX = mouseX;
				m_LastMouseY = mouseY;
				m_FirstMouse = false;
			}

			float xOffset = (mouseX - m_LastMouseX) * m_MouseSensitivity;
			float yOffset = (m_LastMouseY - mouseY) * m_MouseSensitivity;

			m_Yaw += xOffset;
			m_Pitch += yOffset;

			if (m_Pitch > 89.0f) m_Pitch = 89.0f;
			if (m_Pitch < -89.0f) m_Pitch = -89.0f;

			transform.Rotation.x = m_Pitch;
			transform.Rotation.y = m_Yaw;
		}
		else
		{
			m_FirstMouse = true;
		}

		m_LastMouseX = mouseX;
		m_LastMouseY = mouseY;

		// --- WASD movement (always active) ---
		glm::vec3 front;
		front.x = cos(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
		front.y = sin(glm::radians(m_Pitch));
		front.z = sin(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
		front = glm::normalize(front);

		glm::vec3 right = glm::normalize(glm::cross(front, glm::vec3(0.0f, 1.0f, 0.0f)));

		float velocity = m_MoveSpeed * dt;

		if (input.IsKeyPressed(GLFW_KEY_W))
			transform.Position += front * velocity;
		if (input.IsKeyPressed(GLFW_KEY_S))
			transform.Position -= front * velocity;
		if (input.IsKeyPressed(GLFW_KEY_A))
			transform.Position -= right * velocity;
		if (input.IsKeyPressed(GLFW_KEY_D))
			transform.Position += right * velocity;
		if (input.IsKeyPressed(GLFW_KEY_E))
			transform.Position += glm::vec3(0.0f, 1.0f, 0.0f) * velocity;
		if (input.IsKeyPressed(GLFW_KEY_Q))
			transform.Position -= glm::vec3(0.0f, 1.0f, 0.0f) * velocity;
	}
}
