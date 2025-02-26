#pragma once

#include "Core.h"
#include "Input.h"
#include <string>

struct SDL_Renderer;

class Window;
class Renderer;
class Level;

class ENGINE2000_API GameEngine
{
protected:
	struct Settings
	{
		std::string title;
		int width;
		int height;
		bool useOpenGL;
		Settings(const std::string& t = "Engine 2000", int w = 640, int h = 480, bool gl = true)
			: title(t), width(w), height(h), useOpenGL(gl) {}
	};

private:
	Settings m_settings;
	bool m_isRunning;
	Window* m_window;
	Level* m_currentLevel;
	float m_deltaTime;
	uint32_t m_prevTime;
	Input m_input;

public:
	GameEngine(const Settings& settings = Settings());
	virtual ~GameEngine();

	virtual void onInit() {}

	void init();
	void run();
	void shutdown();
	void* getRenderer();

	// Level management
	void setCurrentLevel(Level* Level);
	Level* getCurrentLevel() const { return m_currentLevel; }

	float getDeltaTime() const { return m_deltaTime; }

	const Input& getInput() const { return m_input; }

	int getWindowWidth() const { return m_settings.width; }
	int getWindowHeight() const { return m_settings.height; }
};

// To be defined in client
GameEngine* CreateApplication();
