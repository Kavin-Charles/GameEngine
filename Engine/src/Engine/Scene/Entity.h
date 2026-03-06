#pragma once

#include "Registry.h"
#include "Components.h"
#include <cstdint>
#include <string>
#include "Engine/Core.h"

namespace Engine {

	class Scene;

	class ENGINE_API Entity
	{
	public:
		Entity() = default;
		Entity(uint32_t id, Scene* scene) : m_EntityID(id), m_Scene(scene) {}
		Entity(const Entity& other) = default;

		template<typename T, typename... Args>
		T& AddComponent(Args&&... args);

		template<typename T>
		T& GetComponent();

		template<typename T>
		const T& GetComponent() const;

		template<typename T>
		bool HasComponent() const;

		template<typename T>
		void RemoveComponent();

		uint32_t GetID() const { return m_EntityID; }

		operator bool() const { return m_EntityID != 0 && m_Scene != nullptr; }
		operator uint32_t() const { return m_EntityID; }

		bool operator==(const Entity& other) const
		{
			return m_EntityID == other.m_EntityID && m_Scene == other.m_Scene;
		}

		bool operator!=(const Entity& other) const
		{
			return !(*this == other);
		}

	private:
		uint32_t m_EntityID = 0;
		Scene* m_Scene = nullptr;
	};
}
