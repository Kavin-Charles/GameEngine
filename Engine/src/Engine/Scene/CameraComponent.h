#pragma once

#include "Engine/Renderer/Camera.h"

namespace Engine {

	struct CameraComponent
	{
		Camera CameraData;
		bool Primary = true;
		bool IsGameCamera = false;  // If true, this is the Game View camera

		CameraComponent() = default;
		CameraComponent(const CameraComponent&) = default;
	};
}
