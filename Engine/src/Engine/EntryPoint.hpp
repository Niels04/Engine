#pragma once

#ifdef ENG_PLATFORM_WINDOWS

extern Engine::Application* Engine::CreateApplication();

int main(int argc, char** argv)
{
	Engine::Log::init();
	ENG_CORE_INFO("Initialized engine log.");
	ENG_INFO("Initialized app log.");

	auto app = Engine::CreateApplication();
	app->Run();

	delete(app);
}

#endif