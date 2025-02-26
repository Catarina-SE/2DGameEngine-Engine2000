#pragma once

#ifdef E2000_PLATFORM_WINDOWS

extern GameEngine* CreateApplication();

int main(int argc, char** argv)
{
	printf("Engine2000\n");
	auto app = CreateApplication();
	app->init();
	app->run();
	delete app;
}

#endif