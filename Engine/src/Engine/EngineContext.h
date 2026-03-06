#pragma once

#include "Engine/Core.h"

namespace Engine {

	class Renderer;
	class InputSystem;
	class Window;

	struct ENGINE_API EngineContext
	{
		Renderer* RendererInstance = nullptr;
		InputSystem* Input = nullptr;
		Window* WindowInstance = nullptr;
	};
}
