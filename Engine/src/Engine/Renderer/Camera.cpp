#include "Camera.h"
#include "Engine/Scene/Transform.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Engine {

	void Camera::SetPerspective(float fov, float aspectRatio, float nearClip, float farClip)
	{
		m_FOV = fov;
		m_AspectRatio = aspectRatio;
		m_NearClip = nearClip;
		m_FarClip = farClip;
		RecalculateProjection();
	}

	void Camera::SetViewportSize(uint32_t width, uint32_t height)
	{
		if (height == 0) return;
		m_AspectRatio = (float)width / (float)height;
		RecalculateProjection();
	}

	glm::mat4 Camera::GetViewMatrix(const Transform& transform) const
	{
		// Build front vector from rotation (Rotation.x = pitch, Rotation.y = yaw)
		float pitch = glm::radians(transform.Rotation.x);
		float yaw = glm::radians(transform.Rotation.y);

		glm::vec3 front;
		front.x = cos(yaw) * cos(pitch);
		front.y = sin(pitch);
		front.z = sin(yaw) * cos(pitch);
		front = glm::normalize(front);

		glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

		return glm::lookAt(transform.Position, transform.Position + front, up);
	}

	void Camera::RecalculateProjection()
	{
		m_Projection = glm::perspective(glm::radians(m_FOV), m_AspectRatio, m_NearClip, m_FarClip);
	}
}
