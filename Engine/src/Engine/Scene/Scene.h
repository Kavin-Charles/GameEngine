#pragma once

#include "Entity.h"
#include "Registry.h"
#include <vector>
#include <string>
#include <memory>
#include "Engine/Core.h"

namespace Engine {

	class ENGINE_API Scene
	{
	public:
		Scene() = default;
		~Scene() = default;

		// Entity management
		Entity CreateEntity(const std::string& name = "Entity");
		void DestroyEntity(Entity entity);

		// Update
		void OnUpdate(float dt);

		// Viewport resize
		void OnSceneViewResize(uint32_t width, uint32_t height);
		void OnGameViewResize(uint32_t width, uint32_t height);

		// Camera queries — return entity IDs (0 = not found)
		uint32_t FindGameCameraID();
		uint32_t FindPrimaryCameraID();

		// Registry access
		Registry& GetRegistry() { return m_Registry; }
		const Registry& GetRegistry() const { return m_Registry; }

	private:
		Registry m_Registry;
	};
}
