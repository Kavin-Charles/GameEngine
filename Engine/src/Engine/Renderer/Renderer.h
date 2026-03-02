#pragma once

#include "Engine/Core.h"
#include "Camera.h"
#include "Shader.h"
#include "VertexArray.h"

#include <glad/gl.h>
#include <glm/glm.hpp>
#include <memory>

namespace Engine {

	class ENGINE_API Renderer
	{
	public:
		static void Init();
		static void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height);
		static void SetClearColor(const glm::vec4& color);
		static void Clear();

		static void BeginScene(const PerspectiveCamera& camera);
		static void BeginScene(const glm::mat4& view, const glm::mat4& projection); // new
		static void EndScene();

		static void Submit(const std::shared_ptr<Shader>& shader,
			const std::shared_ptr<VertexArray>& vertexArray,
			const glm::mat4& transform = glm::mat4(1.0f));

	private:
		struct SceneData
		{
			glm::mat4 ViewProjectionMatrix;
		};

		static SceneData* s_SceneData;
	};
}
