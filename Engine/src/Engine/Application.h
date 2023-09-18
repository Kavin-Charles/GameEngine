#pragma once

#include "Core.h"

namespace Engine {

	class ENGINE_API Application
	{		
	public:
		 void run();
		 Application* createApplication();

	};
	Application* createApplication();

}

