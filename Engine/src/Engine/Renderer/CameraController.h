#pragma once

#include "Engine/Core.h"
#include "Engine/Renderer/Camera.h"
#include <glm/glm.hpp>

namespace Engine {

	class ENGINE_API CameraController
	{
	public:
		CameraController(float fov, float aspectRatio, float nearClip = 0.1f, float farClip = 100.0f);

		void OnUpdate(float deltaTime);

		PerspectiveCamera& GetCamera() { return m_Camera; }
		const PerspectiveCamera& GetCamera() const { return m_Camera; }

		float GetMoveSpeed() const { return m_MoveSpeed; }
		void SetMoveSpeed(float speed) { m_MoveSpeed = speed; }

		float GetMouseSensitivity() const { return m_MouseSensitivity; }
		void SetMouseSensitivity(float sensitivity) { m_MouseSensitivity = sensitivity; }

	private:
		PerspectiveCamera m_Camera;

		glm::vec3 m_Position = { 0.0f, 1.0f, 4.0f };
		float m_Yaw = -90.0f;    // Looking toward -Z
		float m_Pitch = -10.0f;

		float m_MoveSpeed = 5.0f;
		float m_MouseSensitivity = 0.1f;

		float m_LastMouseX = 0.0f;
		float m_LastMouseY = 0.0f;
		bool m_FirstMouse = true;
	};
}
