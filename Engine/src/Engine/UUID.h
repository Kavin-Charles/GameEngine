#pragma once

#include <cstdint>
#include <random>
#include <functional>

namespace Engine {

	class UUID
	{
	public:
		UUID()
			: m_UUID(GenerateUUID())
		{
		}

		UUID(uint64_t uuid)
			: m_UUID(uuid)
		{
		}

		UUID(const UUID&) = default;

		operator uint64_t() const { return m_UUID; }

	private:
		static uint64_t GenerateUUID()
		{
			static std::random_device s_RandomDevice;
			static std::mt19937_64 s_Engine(s_RandomDevice());
			static std::uniform_int_distribution<uint64_t> s_Distribution;
			return s_Distribution(s_Engine);
		}

		uint64_t m_UUID;
	};
}

namespace std {

	template<>
	struct hash<Engine::UUID>
	{
		std::size_t operator()(const Engine::UUID& uuid) const
		{
			return hash<uint64_t>()(static_cast<uint64_t>(uuid));
		}
	};
}
