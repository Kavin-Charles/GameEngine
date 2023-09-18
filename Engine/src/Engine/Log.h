#pragma once

#include "spdlog/spdlog.h"
#include"memory"
#include "Core.h"

namespace Engine {
	class ENGINE_API Log
	{
	public:
		static void Init();
		inline static std::shared_ptr<spdlog::logger>& getCoreLogger() { return s_CoreLogger; }
		inline static std::shared_ptr<spdlog::logger>& getClientLogger() { return s_ClientLogger; }
	private:
		static std::shared_ptr<spdlog::logger> s_CoreLogger;
		static std::shared_ptr<spdlog::logger> s_ClientLogger;

	};
}

#define ENGINE_LOG_ERROR(...) ::Engine::Log::getCoreLogger()->error(__VA_ARGS__)
#define ENGINE_LOG_WARN(...) ::Engine::Log::getCoreLogger()->warn(__VA_ARGS__)
#define ENGINE_LOG_INFO(...) ::Engine::Log::getCoreLogger()->info(__VA_ARGS__)
#define ENGINE_LOG_TRACE(...) ::Engine::Log::getCoreLogger()->trace(__VA_ARGS__)

#define CLIENT_LOG_ERROR(...) ::Engine::Log::getClientLogger()->error(__VA_ARGS__)
#define CLIENT_LOG_WARN(...) ::Engine::Log::getClientLogger()->warn(__VA_ARGS__)
#define CLIENT_LOG_INFO(...) ::Engine::Log::getClientLogger()->info(__VA_ARGS__)
#define CLIENT_LOG_TRACE(...) ::Engine::Log::getClientLogger()->trace(__VA_ARGS__)

