#pragma once

#ifdef ENGINE_PLATFORM_WINDOWS

extern Engine::Application* Engine::createApplication();

int main(int argc, char** argv)
{
	Engine::Log::Init();
	ENGINE_LOG_INFO("Engine Initialized!");

	auto app = Engine::createApplication();
	app->Run();
	delete app;

	return 0;
}

#endif
