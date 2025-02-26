#include "GameEngine.h"
#include "EngineError.h"
#include "Window.h"
#include "Renderer.h"
#include "Level.h"
#include <SDL2/SDL.h>
#include <iostream>

GameEngine::GameEngine(const Settings& settings)
	: m_settings(settings)
	, m_isRunning(false)
	, m_window(nullptr)
	, m_currentLevel(nullptr)
{
}

GameEngine::~GameEngine()
{
	if (m_isRunning)
	{
		shutdown();
	}
}

void GameEngine::init()
{
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
	{
		throw EngineError();
	}

	m_window = new Window(m_settings.title, m_settings.width, m_settings.height, m_settings.useOpenGL);

	// Initialize renderer
	Renderer::Instance().init(m_window, m_settings.useOpenGL);

	// Initialize input
	m_input.init();

	m_isRunning = true;
	m_prevTime = SDL_GetTicks();

	// To initialize game content
	onInit();
}

void GameEngine::run()
{
	m_prevTime = SDL_GetTicks();

	while (m_isRunning)
	{
		uint32_t currentTime = SDL_GetTicks();
		m_deltaTime = (currentTime - m_prevTime) / 1000.0f;
		m_prevTime = currentTime;

		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT)
			{
				m_isRunning = false;
			}
		}
			
		m_input.update();

		// Update current level if it exists
		if (m_currentLevel)
		{
			m_currentLevel->update(m_deltaTime);
			m_currentLevel->render();
		}
		else
		{
			// Default rendering if no level is set
			Renderer::Instance().clear();
			Renderer::Instance().present();
		}
	}
}

void GameEngine::shutdown()
{
	m_isRunning = false;  // Ensure the game loop stops first
	
	if (m_currentLevel)
	{
		delete m_currentLevel;
		m_currentLevel = nullptr;
	}
	
	Renderer::Instance().cleanup();
	
	if (m_window)
	{
		delete m_window;
		m_window = nullptr;
	}
	
	SDL_Quit();
}

void* GameEngine::getRenderer()
{
	return Renderer::Instance().getRenderer();
}

void GameEngine::setCurrentLevel(Level* Level)
{
	if (m_currentLevel)
	{
		delete m_currentLevel;
	}
	m_currentLevel = Level;
}
