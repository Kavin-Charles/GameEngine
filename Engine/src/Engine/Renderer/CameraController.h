#pragma once

#include "Engine/Core.h"
#include "Engine/Scene/Transform.h"
#include "Engine/InputSystem.h"
#include <glm/glm.hpp>

namespace Engine {

	class ENGINE_API CameraController
	{
	public:
		CameraController() = default;

		void OnUpdate(float dt, InputSystem& input, Transform& transform);

		float GetMoveSpeed() const { return m_MoveSpeed; }
		void SetMoveSpeed(float speed) { m_MoveSpeed = speed; }

		float GetMouseSensitivity() const { return m_MouseSensitivity; }
		void SetMouseSensitivity(float sensitivity) { m_MouseSensitivity = sensitivity; }

	private:
		float m_MoveSpeed = 5.0f;
		float m_MouseSensitivity = 0.1f;

		float m_Yaw = -90.0f;
		float m_Pitch = -10.0f;

		float m_LastMouseX = 0.0f;
		float m_LastMouseY = 0.0f;
		bool m_FirstMouse = true;
	};
}
