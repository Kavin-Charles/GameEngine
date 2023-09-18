#include "Application.h"
#include "stdio.h"
#include "Events/KeyEvent.h"
namespace Engine {


	void Application::run()
	{
		KeyPressedEvent e();
		ENGINE_LOG_INFO(e);

		while (true)
		{
			//printf("LMAO");
		}
	}
}
