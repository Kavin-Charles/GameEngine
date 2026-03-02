#include "Entity.h"

#include <algorithm>

namespace Engine {

	Entity::Entity(const std::string& name)
		: m_Tag(name)
	{
	}

	Entity::~Entity()
	{
		for (auto child : m_Children)
			delete child; // Scene owns root entities, entities own children. Simple ownership model.
	}

	void Entity::AddChild(Entity* child)
	{
		if (child->m_Parent)
			child->m_Parent->RemoveChild(child);

		child->m_Parent = this;
		m_Children.push_back(child);
	}

	void Entity::RemoveChild(Entity* child)
	{
		auto it = std::find(m_Children.begin(), m_Children.end(), child);
		if (it != m_Children.end())
		{
			child->m_Parent = nullptr;
			m_Children.erase(it);
		}
	}

}
