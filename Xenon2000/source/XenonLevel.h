#pragma once

#include "Engine2000/Level.h"
#include "Player.h"
#include <vector>
#include "XenonWaveManager.h"
#include "Asteroid.h"
#include "MetalAsteroid.h"
#include "Rock.h"
#include "TextDisplay.h"

class XenonLevel : public Level
{
private:
	Player* m_player;
	std::vector<GameObject*> m_enemies;
	XenonWaveManager* m_waveManager;

	TextDisplay* m_displayPlayer;
	TextDisplay* m_displayScore;
	int m_score;

public:
	XenonLevel(const Input& input, int screenWidth, int screenHeight);
	~XenonLevel();

	virtual void update(float deltaTime) override;


	XenonWaveManager* getWaveManager() { return m_waveManager; }
	std::vector<GameObject*>& getEnemies() { return m_enemies; }

	// GameObject creation methods
	void createLoner(float y, bool fromRight);
	void createRusher(float x, bool fromTop);
	void createAsteroid(float x, float y, Asteroid::Size size = Asteroid::Size::LARGE);
	void createMetalAsteroid(float x, float y, MetalAsteroid::Size size);
	void createDrone(float x, float y, int phase);
	void createShieldPowerUp(float x, float y);
	void createWeaponPowerUp(float x, float y);
	void createCompanionPowerUp(float x, float y);
	void createRock(float x, float y, Rock::RockType type, bool flipped);

	// Score related methods
	void addScore(int amount);

private:
	void setupCollisions();
	void setupScoreDisplay();
	void setupBackground();
	void setupPlayer();
	
	// Score related methods
	void updateScore();
};

