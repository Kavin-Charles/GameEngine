#include "Renderer.h"
#include "Engine/Scene/Scene.h"
#include "Engine/Scene/Components.h"
#include "Engine/Scene/Transform.h"

namespace Engine {

	void Renderer::Init()
	{
		RenderCommand::Init();
	}

	void Renderer::Shutdown()
	{
	}

	void Renderer::BeginFrame(const glm::vec4& clearColor)
	{
		RenderCommand::SetClearColor(clearColor);
		RenderCommand::Clear();
	}

	void Renderer::EndFrame()
	{
	}

	void Renderer::RenderScene(Scene& scene, Shader& shader)
	{
		// Find primary camera entity
		uint32_t camID = scene.FindPrimaryCameraID();
		if (camID == 0) return;

		auto& reg = scene.GetRegistry();
		auto& cam = reg.GetComponent<CameraComponent>(camID);
		auto& transform = reg.GetComponent<TransformComponent>(camID);

		RenderSceneWithCamera(scene, shader, cam.CameraData, transform.TransformData);
	}

	void Renderer::RenderSceneWithCamera(Scene& scene, Shader& shader,
		const Camera& camera, const Transform& cameraTransform)
	{
		glm::mat4 view = camera.GetViewMatrix(cameraTransform);
		glm::mat4 projection = camera.GetProjectionMatrix();
		m_ViewProjectionMatrix = projection * view;

		// Iterate all entities with MeshRendererComponent
		auto& reg = scene.GetRegistry();
		reg.Each<MeshRendererComponent>([&](uint32_t id, MeshRendererComponent& meshRenderer) {
			if (!meshRenderer.MeshAsset) return;
			if (!reg.HasComponent<TransformComponent>(id)) return;

			auto& transform = reg.GetComponent<TransformComponent>(id);

			shader.Bind();
			shader.SetInt("u_EntityID", (int)id);
			shader.SetBool("u_Selected", id == m_SelectedEntityID);
			shader.SetFloat3("u_ObjectColor", glm::vec3(meshRenderer.MaterialData.Color));

			if (meshRenderer.MaterialData.AlbedoTexture)
			{
				meshRenderer.MaterialData.AlbedoTexture->Bind(0);
				shader.SetInt("u_AlbedoMap", 0);
				shader.SetBool("u_HasAlbedoMap", true);
			}
			else
			{
				shader.SetBool("u_HasAlbedoMap", false);
			}

			Submit(shader, meshRenderer.MeshAsset->GetVertexArray(), transform.GetMatrix());
		});
	}

	void Renderer::Submit(Shader& shader,
		const std::shared_ptr<VertexArray>& vertexArray,
		const glm::mat4& transform)
	{
		shader.Bind();
		shader.SetMat4("u_ViewProjection", m_ViewProjectionMatrix);
		shader.SetMat4("u_Model", transform);

		RenderCommand::DrawIndexed(vertexArray);
	}

	void Renderer::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
	{
		RenderCommand::SetViewport(x, y, width, height);
	}
}
