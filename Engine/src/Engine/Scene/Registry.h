#pragma once

#include <unordered_map>
#include <typeindex>
#include <memory>
#include <functional>
#include <cassert>
#include <cstdint>

namespace Engine {

	// Type-erased base for component storage
	class IComponentPool
	{
	public:
		virtual ~IComponentPool() = default;
		virtual void Remove(uint32_t entityID) = 0;
		virtual bool Has(uint32_t entityID) const = 0;
	};

	// Typed component pool — stores components of type T indexed by entity ID
	template<typename T>
	class ComponentPool : public IComponentPool
	{
	public:
		template<typename... Args>
		T& Add(uint32_t entityID, Args&&... args)
		{
			m_Data[entityID] = T(std::forward<Args>(args)...);
			return m_Data[entityID];
		}

		T& Get(uint32_t entityID)
		{
			auto it = m_Data.find(entityID);
			assert(it != m_Data.end() && "Entity does not have this component");
			return it->second;
		}

		const T& Get(uint32_t entityID) const
		{
			auto it = m_Data.find(entityID);
			assert(it != m_Data.end() && "Entity does not have this component");
			return it->second;
		}

		bool Has(uint32_t entityID) const override
		{
			return m_Data.find(entityID) != m_Data.end();
		}

		void Remove(uint32_t entityID) override
		{
			m_Data.erase(entityID);
		}

		// Iterate all entities with this component
		void Each(std::function<void(uint32_t, T&)> fn)
		{
			for (auto& [id, component] : m_Data)
			{
				fn(id, component);
			}
		}

		size_t Size() const { return m_Data.size(); }

	private:
		std::unordered_map<uint32_t, T> m_Data;
	};

	// Registry — manages entities and their components
	class Registry
	{
	public:
		uint32_t CreateEntity()
		{
			uint32_t id = m_NextEntityID++;
			m_Entities.push_back(id);
			return id;
		}

		void DestroyEntity(uint32_t entityID)
		{
			// Remove from all component pools
			for (auto& [type, pool] : m_Pools)
			{
				if (pool->Has(entityID))
					pool->Remove(entityID);
			}

			// Remove from entity list
			auto it = std::find(m_Entities.begin(), m_Entities.end(), entityID);
			if (it != m_Entities.end())
				m_Entities.erase(it);
		}

		bool IsValid(uint32_t entityID) const
		{
			return std::find(m_Entities.begin(), m_Entities.end(), entityID) != m_Entities.end();
		}

		template<typename T, typename... Args>
		T& AddComponent(uint32_t entityID, Args&&... args)
		{
			auto& pool = GetOrCreatePool<T>();
			return pool.Add(entityID, std::forward<Args>(args)...);
		}

		template<typename T>
		T& GetComponent(uint32_t entityID)
		{
			return GetOrCreatePool<T>().Get(entityID);
		}

		template<typename T>
		const T& GetComponent(uint32_t entityID) const
		{
			auto it = m_Pools.find(std::type_index(typeid(T)));
			assert(it != m_Pools.end());
			return static_cast<const ComponentPool<T>&>(*it->second).Get(entityID);
		}

		template<typename T>
		bool HasComponent(uint32_t entityID) const
		{
			auto it = m_Pools.find(std::type_index(typeid(T)));
			if (it == m_Pools.end()) return false;
			return it->second->Has(entityID);
		}

		template<typename T>
		void RemoveComponent(uint32_t entityID)
		{
			auto it = m_Pools.find(std::type_index(typeid(T)));
			if (it != m_Pools.end())
				it->second->Remove(entityID);
		}

		template<typename T>
		void Each(std::function<void(uint32_t, T&)> fn)
		{
			GetOrCreatePool<T>().Each(fn);
		}

		const std::vector<uint32_t>& GetEntities() const { return m_Entities; }
		size_t GetEntityCount() const { return m_Entities.size(); }

	private:
		template<typename T>
		ComponentPool<T>& GetOrCreatePool()
		{
			auto typeIdx = std::type_index(typeid(T));
			auto it = m_Pools.find(typeIdx);
			if (it == m_Pools.end())
			{
				auto pool = std::make_shared<ComponentPool<T>>();
				m_Pools[typeIdx] = pool;
				return *pool;
			}
			return static_cast<ComponentPool<T>&>(*it->second);
		}

		uint32_t m_NextEntityID = 1; // 0 = invalid
		std::vector<uint32_t> m_Entities;
		std::unordered_map<std::type_index, std::shared_ptr<IComponentPool>> m_Pools;
	};
}
