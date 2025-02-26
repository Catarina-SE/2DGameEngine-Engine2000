#include "XenonGame.h"
#include "Engine2000/GameEngine.h"
#include "XenonLevel.h"
#include <iostream>

XenonGame::XenonGame()
	: GameEngine(Settings("Xenon 2000", 640, 480))
{
}

void XenonGame::onInit()
{
	setCurrentLevel(new XenonLevel(getInput(), getWindowWidth(), getWindowHeight()));
}

GameEngine* CreateApplication()
{
	return new XenonGame();
}
