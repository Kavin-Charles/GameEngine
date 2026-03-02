#pragma once

#include "Components.h"
#include <vector>
#include <string>

#include "Engine/Core.h"

namespace Engine {

	class ENGINE_API Entity
	{
	public:
		Entity(const std::string& name = "Entity");
		~Entity();

		const std::string& GetName() const { return m_Tag.Tag; }
		void SetName(const std::string& name) { m_Tag.Tag = name; }

		TransformComponent& GetTransform() { return m_Transform; }
		const TransformComponent& GetTransform() const { return m_Transform; }

		MeshComponent& GetMesh() { return m_Mesh; }
		const MeshComponent& GetMesh() const { return m_Mesh; }

		// Hierarchy
		Entity* GetParent() const { return m_Parent; }
		const std::vector<Entity*>& GetChildren() const { return m_Children; }

		void AddChild(Entity* child);
		void RemoveChild(Entity* child); // Does not delete, just detaches

		// Components - baked in for simplicity for now
		TagComponent m_Tag;
		TransformComponent m_Transform;
		MeshComponent m_Mesh;

	private:
		Entity* m_Parent = nullptr;
		std::vector<Entity*> m_Children;

		friend class Scene;
	};

}
