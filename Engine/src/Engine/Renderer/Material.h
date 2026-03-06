#pragma once

#include "Engine/Renderer/Shader.h"
#include "Engine/Renderer/Texture.h"
#include <glm/glm.hpp>
#include <memory>

namespace Engine {

	struct Material
	{
		std::shared_ptr<Shader> Shader = nullptr;
		std::shared_ptr<Texture2D> AlbedoTexture = nullptr;
		glm::vec4 Color = { 1.0f, 1.0f, 1.0f, 1.0f };
		float Metallic = 0.0f;
		float Roughness = 0.5f;
		float AmbientOcclusion = 1.0f;

		Material() = default;
		Material(const Material&) = default;
	};
}
