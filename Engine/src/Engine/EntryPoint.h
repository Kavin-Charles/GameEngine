#pragma once
#include "../Engine.h"

extern Engine::Application* Engine::createApplication();


int main(int arc, char** argv) {

	Engine::Log::Init();

	ENGINE_LOG_INFO("Atleast it works!");


	auto app = Engine::createApplication();
	app->run();
	delete app;
}

