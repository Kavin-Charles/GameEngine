#include "CameraController.h"
#include "Engine/Input.h"

#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>

namespace Engine {

	CameraController::CameraController(float fov, float aspectRatio, float nearClip, float farClip)
		: m_Camera(fov, aspectRatio, nearClip, farClip)
	{
		m_Camera.SetPosition(m_Position);
	}

	void CameraController::OnUpdate(float deltaTime)
	{
		// --- Mouse look (right-click held) ---
		auto [mouseX, mouseY] = Input::GetMousePosition();

		if (Input::IsMouseButtonPressed(GLFW_MOUSE_BUTTON_RIGHT))
		{
			if (m_FirstMouse)
			{
				m_LastMouseX = mouseX;
				m_LastMouseY = mouseY;
				m_FirstMouse = false;
			}

			float xOffset = (mouseX - m_LastMouseX) * m_MouseSensitivity;
			float yOffset = (m_LastMouseY - mouseY) * m_MouseSensitivity; // Inverted Y

			m_Yaw += xOffset;
			m_Pitch += yOffset;

			// Clamp pitch
			if (m_Pitch > 89.0f) m_Pitch = 89.0f;
			if (m_Pitch < -89.0f) m_Pitch = -89.0f;
		}
		else
		{
			m_FirstMouse = true;
		}

		m_LastMouseX = mouseX;
		m_LastMouseY = mouseY;

		// --- Calculate front direction ---
		glm::vec3 front;
		front.x = cos(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
		front.y = sin(glm::radians(m_Pitch));
		front.z = sin(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
		front = glm::normalize(front);

		glm::vec3 right = glm::normalize(glm::cross(front, glm::vec3(0.0f, 1.0f, 0.0f)));
		glm::vec3 up = glm::normalize(glm::cross(right, front));

		// --- Update camera ---
		if (Input::IsMouseButtonPressed(GLFW_MOUSE_BUTTON_RIGHT))
		{
			// --- WASD movement ---
			float velocity = m_MoveSpeed * deltaTime;

			if (Input::IsKeyPressed(GLFW_KEY_W))
				m_Position += front * velocity;
			if (Input::IsKeyPressed(GLFW_KEY_S))
				m_Position -= front * velocity;
			if (Input::IsKeyPressed(GLFW_KEY_A))
				m_Position -= right * velocity;
			if (Input::IsKeyPressed(GLFW_KEY_D))
				m_Position += right * velocity;
			if (Input::IsKeyPressed(GLFW_KEY_E))
				m_Position += glm::vec3(0.0f, 1.0f, 0.0f) * velocity;
			if (Input::IsKeyPressed(GLFW_KEY_Q))
				m_Position -= glm::vec3(0.0f, 1.0f, 0.0f) * velocity;
		}

		// --- Update camera ---
		m_Camera.SetPositionAndTarget(m_Position, m_Position + front);
	}
}
