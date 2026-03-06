#pragma once

#include <memory>

// --- Platform Detection ---
#ifdef ENGINE_BUILD_DLL
	#define ENGINE_API __declspec(dllexport)
#else
	#define ENGINE_API __declspec(dllimport)
#endif

// --- Utility Macros ---
#define BIT(X) (1 << X)

// --- Asserts (Debug only) ---
#ifdef ENGINE_DEBUG
	#define ENGINE_CORE_ASSERT(x, ...) { if(!(x)) { ENGINE_LOG_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
	#define ENGINE_ASSERT(x, ...) { if(!(x)) { CLIENT_LOG_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
#else
	#define ENGINE_CORE_ASSERT(x, ...)
	#define ENGINE_ASSERT(x, ...)
#endif

// --- Smart Pointer Aliases ---
namespace Engine {

	template<typename T>
	using Scope = std::unique_ptr<T>;

	template<typename T, typename ... Args>
	constexpr Scope<T> CreateScope(Args&& ... args)
	{
		return std::make_unique<T>(std::forward<Args>(args)...);
	}

	template<typename T>
	using Ref = std::shared_ptr<T>;

	template<typename T, typename ... Args>
	constexpr Ref<T> CreateRef(Args&& ... args)
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	}
}
