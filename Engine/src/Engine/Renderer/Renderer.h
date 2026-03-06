#pragma once

#include "Engine/Core.h"
#include "Shader.h"
#include "VertexArray.h"
#include "RenderCommand.h"
#include "Camera.h"

#include <glm/glm.hpp>
#include <memory>

namespace Engine {

	class Scene;
	struct Transform;

	class ENGINE_API Renderer
	{
	public:
		void Init();
		void Shutdown();

		void BeginFrame(const glm::vec4& clearColor = { 0.1f, 0.1f, 0.1f, 1.0f });
		void EndFrame();

		// Render scene using the scene's primary camera
		void RenderScene(Scene& scene, Shader& shader);

		// Render scene from an explicit camera + transform (used for editor/game views)
		void RenderSceneWithCamera(Scene& scene, Shader& shader,
			const Camera& camera, const Transform& cameraTransform);

		void Submit(Shader& shader,
			const std::shared_ptr<VertexArray>& vertexArray,
			const glm::mat4& transform = glm::mat4(1.0f));

		void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height);

		static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }

	private:
		glm::mat4 m_ViewProjectionMatrix = glm::mat4(1.0f);
	};
}
