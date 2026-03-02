#pragma once

#include "Entity.h"
#include <vector>
#include <glm/glm.hpp>
#include "Engine/Core.h"

namespace Engine {

	class ENGINE_API Scene
	{
	public:
		Scene();
		~Scene();

		Entity* CreateEntity(const std::string& name = "Entity");
		void DestroyEntity(Entity* entity);
		void ReparentEntity(Entity* entity, Entity* parent);

#include <memory>

		// Editor camera rendering
		void OnUpdateEditor(float ts, const std::shared_ptr<class Shader>& shader, const glm::mat4& view, const glm::mat4& projection);

		// Runtime rendering with Scene Camera (TODO)
		void OnUpdateRuntime(float ts); 

		const std::vector<Entity*>& GetRootEntities() const { return m_RootEntities; }

	private:
		std::vector<Entity*> m_RootEntities;

		// Recursive helper
		void DrawEntity(Entity* entity, const std::shared_ptr<class Shader>& shader, const glm::mat4& view, const glm::mat4& projection, const glm::mat4& parentTransform);
	};

}
