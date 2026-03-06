#pragma once

// Entity template implementations — include this AFTER Scene.h
// Both Engine.dll and client projects include this so templates are instantiated locally

#include "Entity.h"
#include "Scene.h"
#include "Components.h"

namespace Engine {

	template<typename T, typename... Args>
	T& Entity::AddComponent(Args&&... args)
	{
		return m_Scene->GetRegistry().AddComponent<T>(m_EntityID, std::forward<Args>(args)...);
	}

	template<typename T>
	T& Entity::GetComponent()
	{
		return m_Scene->GetRegistry().GetComponent<T>(m_EntityID);
	}

	template<typename T>
	const T& Entity::GetComponent() const
	{
		return m_Scene->GetRegistry().GetComponent<T>(m_EntityID);
	}

	template<typename T>
	bool Entity::HasComponent() const
	{
		return m_Scene->GetRegistry().HasComponent<T>(m_EntityID);
	}

	template<typename T>
	void Entity::RemoveComponent()
	{
		m_Scene->GetRegistry().RemoveComponent<T>(m_EntityID);
	}
}
