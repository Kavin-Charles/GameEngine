#pragma once

#include "Engine/Core.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Engine {

	class ENGINE_API Camera
	{
	public:
		Camera() { RecalculateProjection(); }

		void SetPerspective(float fov, float aspectRatio, float nearClip, float farClip);
		void SetViewportSize(uint32_t width, uint32_t height);

		glm::mat4 GetViewMatrix(const struct Transform& transform) const;
		glm::mat4 GetProjectionMatrix() const { return m_Projection; }

		float GetFOV() const { return m_FOV; }
		void SetFOV(float fov) { m_FOV = fov; RecalculateProjection(); }

		float GetNearClip() const { return m_NearClip; }
		float GetFarClip() const { return m_FarClip; }

	private:
		void RecalculateProjection();

		glm::mat4 m_Projection = glm::mat4(1.0f);

		float m_FOV = 45.0f;
		float m_AspectRatio = 16.0f / 9.0f;
		float m_NearClip = 0.01f;
		float m_FarClip = 1000.0f;
	};
}
