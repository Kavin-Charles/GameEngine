#include "Scene.h"
#include "Components.h"
#include "Engine/Renderer/Renderer.h"
#include "Engine/Renderer/Shader.h"
#include "Engine/Renderer/Mesh.h"

#include <algorithm>

namespace Engine {

	Scene::Scene()
	{
	}

	Scene::~Scene()
	{
		for (auto entity : m_RootEntities)
			delete entity;
	}

	Entity* Scene::CreateEntity(const std::string& name)
	{
		Entity* entity = new Entity(name);
		m_RootEntities.push_back(entity);
		return entity;
	}

	void Scene::DestroyEntity(Entity* entity)
	{
		if (!entity)
			return;

		if (entity->GetParent())
		{
			entity->GetParent()->RemoveChild(entity);
		}
		else
		{
			auto it = std::find(m_RootEntities.begin(), m_RootEntities.end(), entity);
			if (it != m_RootEntities.end())
				m_RootEntities.erase(it);
		}

		delete entity;
	}

	void Scene::OnUpdateEditor(float ts, const std::shared_ptr<Shader>& shader, const glm::mat4& view, const glm::mat4& projection)
	{
		// Render
		Renderer::BeginScene(view, projection); // We need an overload for ViewProjection, or construct camera
		
		// Actually Renderer::BeginScene takes Camera&.
		// We should add an overload to Renderer to take ViewProjection matrices directly for Editor camera.
		// Or construct a temp Camera. But Camera is PerspectiveCamera.
		// Let's check Renderer.h. It has BeginScene(PerspectiveCamera&).
		// I'll add BeginScene(view, projection) to Renderer.
		// For now, I'll assume it exists or I'll add it.
		// Wait, I can't assume. I should check or add it.
		// I'll add it to Renderer.h quickly.
		
		// For now let's just use the shader bind manually here? No, Renderer manages scene data (ViewProjection).
		// So I must have Renderer::BeginScene(view, projection).

		for (auto entity : m_RootEntities)
		{
			DrawEntity(entity, shader, view, projection, glm::mat4(1.0f));
		}

		Renderer::EndScene();
	}

	void Scene::DrawEntity(Entity* entity, const std::shared_ptr<Shader>& shader, const glm::mat4& view, const glm::mat4& projection, const glm::mat4& parentTransform)
	{
		auto& transform = entity->GetTransform();
		glm::mat4 localTransform = transform.GetTransform();
		glm::mat4 worldTransform = parentTransform * localTransform;

		if (entity->GetMesh().Mesh)
		{
			// Bind shader and set uniforms
			// Note: Renderer::Submit binds the shader.
			// But we need to set color uniform.
			shader->Bind();
			shader->SetFloat3("u_ObjectColor", glm::vec3(entity->GetMesh().Color));
			
			// Submit
			// We need to get VertexArray from Mesh.
			// The Mesh class has GetVertexArray().
			Renderer::Submit(shader, entity->GetMesh().Mesh->GetVertexArray(), worldTransform);
		}

		for (auto child : entity->GetChildren())
		{
			DrawEntity(child, shader, view, projection, worldTransform);
		}
	}

	void Scene::ReparentEntity(Entity* entity, Entity* parent)
	{
		if (entity->GetParent())
		{
			entity->GetParent()->RemoveChild(entity);
		}
		else
		{
			// Was a root entity
			auto it = std::find(m_RootEntities.begin(), m_RootEntities.end(), entity);
			if (it != m_RootEntities.end())
				m_RootEntities.erase(it);
		}

		if (parent)
		{
			parent->AddChild(entity);
		}
		else
		{
			m_RootEntities.push_back(entity);
		}
	}

}
