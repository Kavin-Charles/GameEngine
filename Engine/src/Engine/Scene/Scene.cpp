#include "Scene.h"
#include "Components.h"
#include "Entity.inl"

namespace Engine {

	Entity Scene::CreateEntity(const std::string& name)
	{
		uint32_t id = m_Registry.CreateEntity();
		Entity entity(id, this);
		m_Registry.AddComponent<TagComponent>(id, name);
		m_Registry.AddComponent<TransformComponent>(id);
		return entity;
	}

	void Scene::DestroyEntity(Entity entity)
	{
		m_Registry.DestroyEntity(entity.GetID());
	}

	void Scene::OnUpdate(float dt)
	{
	}

	void Scene::OnSceneViewResize(uint32_t width, uint32_t height)
	{
		m_Registry.Each<CameraComponent>([&](uint32_t id, CameraComponent& cam) {
			if (cam.Primary && !cam.IsGameCamera)
				cam.CameraData.SetViewportSize(width, height);
		});
	}

	void Scene::OnGameViewResize(uint32_t width, uint32_t height)
	{
		m_Registry.Each<CameraComponent>([&](uint32_t id, CameraComponent& cam) {
			if (cam.IsGameCamera)
				cam.CameraData.SetViewportSize(width, height);
		});
	}

	uint32_t Scene::FindGameCameraID()
	{
		uint32_t found = 0;
		m_Registry.Each<CameraComponent>([&](uint32_t id, CameraComponent& cam) {
			if (cam.IsGameCamera && found == 0)
				found = id;
		});
		return found;
	}

	uint32_t Scene::FindPrimaryCameraID()
	{
		uint32_t found = 0;
		m_Registry.Each<CameraComponent>([&](uint32_t id, CameraComponent& cam) {
			if (cam.Primary && found == 0)
				found = id;
		});
		return found;
	}
}
