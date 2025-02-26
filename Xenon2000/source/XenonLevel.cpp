#include "XenonLevel.h"
#include "Engine2000/SpriteComponent.h"
#include "Engine2000/GameObject.h"
#include "Engine2000/E2Log.h"
#include "Engine2000/PhysicsLayerManager.h"
#include "Engine2000/PhysicsComponent.h"

#include "Player.h"
#include "Loner.h"
#include "Rusher.h"
#include "Drone.h"
#include "WeaponPowerUp.h"
#include "ShieldPowerUp.h"
#include "CompanionPowerUp.h"

#include <iostream>


XenonLevel::XenonLevel(const Input& input, int screenWidth, int screenHeight)
	: Level(input, screenWidth, screenHeight)
	, m_score(0.0f)
	, m_displayPlayer(nullptr)
	, m_displayScore(nullptr)
{
	setupScoreDisplay();
	setupCollisions();
	setupBackground();
	setupPlayer();

	m_waveManager = new XenonWaveManager(this);
}

XenonLevel::~XenonLevel()
{
	/* Cleanup is handled by level destructor because
	we add everything to the layers */
	delete m_waveManager;
	m_player = nullptr;
	m_enemies.clear();
}

void XenonLevel::update(float deltaTime)
{
	Level::update(deltaTime);
	m_waveManager->update(deltaTime);
}

void XenonLevel::setupCollisions()
{
	E2_LOG(Log, "Setting up Xenon collision matrix...");
	auto& layerManager = PhysicsLayerManager::getInstance();

	// Create needed layers
	layerManager.createLayer("EnemyProjectile");
	layerManager.createLayer("PlayerProjectile");
	layerManager.createLayer("PowerUp");
	layerManager.createLayer("MetalAsteroid");

	// Enemies layer set up
	layerManager.setLayerCollision("Enemy", "Enemy", false);

	// Player layer set up
	layerManager.setLayerCollision("Player", "Enemy", false);
	layerManager.setLayerCollision("Player", "Player", false);

	// Player projectiles layer set up
	layerManager.setLayerCollision("PlayerProjectile", "PlayerProjectile", false);
	layerManager.setLayerCollision("PlayerProjectile", "Player", false);
	layerManager.setLayerCollision("PlayerProjectile", "Enemy", false);

	// Enemy Projectiles layer set up
	layerManager.setLayerCollision("EnemyProjectile", "EnemyProjectile", false);
	layerManager.setLayerCollision("EnemyProjectile", "PlayerProjectile", false);
	layerManager.setLayerCollision("EnemyProjectile", "Enemy", false);
	layerManager.setLayerCollision("EnemyProjectile", "Player", false);

	// Background layer set up
	layerManager.setLayerCollision("Background", "Enemy", false);
	layerManager.setLayerCollision("Background", "Player", false);
	layerManager.setLayerCollision("Background", "Projectile", false);
	layerManager.setLayerCollision("Background", "EnemyProjectile", false);
	layerManager.setLayerCollision("Background", "PlayerProjectile", false);

	// PowerUp layer set up
	layerManager.setLayerCollision("PowerUp", "Background", false);
	layerManager.setLayerCollision("PowerUp", "PowerUp", false);
	layerManager.setLayerCollision("PowerUp", "Enemy", false);
	layerManager.setLayerCollision("PowerUp", "Player", false);
	layerManager.setLayerCollision("PowerUp", "Default", false);
	layerManager.setLayerCollision("PowerUp", "Projectile", false);
	layerManager.setLayerCollision("PowerUp", "EnemyProjectile", false);
	layerManager.setLayerCollision("PowerUp", "PlayerProjectile", false);

	layerManager.setLayerCollision("MetalAsteroid", "Background", false);
	layerManager.setLayerCollision("MetalAsteroid", "PowerUp", false);
	layerManager.setLayerCollision("MetalAsteroid", "Enemy", false);
	layerManager.setLayerCollision("MetalAsteroid", "Player", true);
	layerManager.setLayerCollision("MetalAsteroid", "Default", false);
	layerManager.setLayerCollision("MetalAsteroid", "Projectile", false);
	layerManager.setLayerCollision("MetalAsteroid", "EnemyProjectile", false);
	layerManager.setLayerCollision("MetalAsteroid", "PlayerProjectile", false);

	E2_LOG(Log, "Xenon collision matrix configured");
}

void XenonLevel::setupScoreDisplay()
{
	// Player text using 16x16 font
	m_displayPlayer = createGameObject<TextDisplay>();
	m_displayPlayer->setScreenPosition(0.02f, 0.02f);
	m_displayPlayer->setText("PLAYER ONE");

	// Score text using 8x8 font for compact number display
	m_displayScore = createGameObject<TextDisplay>(true);
	m_displayScore->setScreenPosition(0.02f, 0.05f);
	updateScore();
}

void XenonLevel::setupBackground()
{
	auto background = createGameObject<GameObject>(BACKGROUND);
	background->addComponent<SpriteComponent>()
		->setTexture("graphics/galaxy2.bmp");

	auto physics = background->addComponent<PhysicsComponent>();
	physics->init(m_physicsWorld, false, false);
	physics->setLayer("Background");
}

void XenonLevel::setupPlayer()
{
	m_player = createGameObject<Player>(Level::PLAYER);
	m_player->addCompanion();
	m_player->addCompanion();

	float playerWidth = m_player->getSprite()->getFrameWidth();
	float playerHeight = m_player->getSprite()->getFrameHeight();

	float spawnX = (m_screenWidth - playerWidth) * 0.5f;
	float spawnY = m_screenHeight - playerHeight - 20.0f;

	auto physics = m_player->getComponent<PhysicsComponent>();
	if (physics)
	{
		physics->setPosition(Vector2D(spawnX, spawnY));
	}
}

void XenonLevel::createLoner(float y, bool fromRight)
{
	auto loner = createGameObject<Loner>(m_screenWidth);
	loner->spawn(fromRight ? Loner::SpawnSide::RIGHT : Loner::SpawnSide::LEFT, y);
	m_enemies.push_back(loner);
}

void XenonLevel::createRusher(float x, bool fromTop)
{
	auto rusher = createGameObject<Rusher>(m_screenHeight);
	rusher->spawn(fromTop ? Rusher::SpawnSide::TOP : Rusher::SpawnSide::BOTTOM, x);
	m_enemies.push_back(rusher);
}

void XenonLevel::createAsteroid(float x, float y, Asteroid::Size size)
{
	auto asteroid = createGameObject<Asteroid>(size);


	if (auto physics = asteroid->getComponent<PhysicsComponent>())
	{

		physics->init(m_physicsWorld, true);
		physics->createCollisionShapeFromSprite();
		physics->createSensorShapeFromSprite(asteroid);
		physics->setLayer("Enemy");


		physics->setPosition(Vector2D(x, y));


		physics->setVelocity(Vector2D(0.0f, 0.3f));
	}

	m_enemies.push_back(asteroid);
	//E2_LOG(Log, "Created asteroid at position (%f, %f)", x, y);
}

void XenonLevel::createMetalAsteroid(float x, float y, MetalAsteroid::Size size)
{
	//E2_LOG(Warning, "XenonLevel creating MetalAsteroid with size enum value: %d", static_cast<int>(size));
	auto asteroid = createGameObject<MetalAsteroid>(size);
	if (asteroid) {
		asteroid->spawn(Vector2D(x, y));
	}
	else {
		E2_LOG(Error, "Failed to create MetalAsteroid!");
	}
}

void XenonLevel::createDrone(float x, float y, int phase)
{
	auto drone = createGameObject<Drone>();
	drone->spawn(x, y, phase);
	m_enemies.push_back(drone);
	//E2_LOG(Warning, "Created drone at x position %f", x);
}

void XenonLevel::createShieldPowerUp(float x, float y)
{
	auto powerUp = createGameObject<ShieldPowerUp>();
	powerUp->spawn(Vector2D(x, y));
	//E2_LOG(Log, "Created shield power-up at position (%f, %f)", x, y);
}

void XenonLevel::createWeaponPowerUp(float x, float y)
{
	auto powerUp = createGameObject<WeaponPowerUp>();
	powerUp->spawn(Vector2D(x, y));
	//E2_LOG(Log, "Created weapon power-up at position (%f, %f)", x, y);
}

void XenonLevel::createCompanionPowerUp(float x, float y)
{
	auto powerUp = createGameObject<CompanionPowerUp>();
	powerUp->spawn(Vector2D(x, y));
	//E2_LOG(Log, "Created companion power-up at position (%f, %f)", x, y);
}

void XenonLevel::createRock(float x, float y, Rock::RockType type, bool flipped)
{
	auto rock = createGameObject<Rock>(BACKGROUND, type);
	rock->setFlipped(flipped);

	// Set up physics for movement
	if (auto physics = rock->addComponent<PhysicsComponent>())
	{
		physics->init(m_physicsWorld, false);  // kinematic body
		physics->setLayer("Background");
		physics->setPosition(Vector2D(x, y));
		physics->setVelocity(Vector2D(0.0f, 0.15f));  // Slow downward movement
	}

// 	E2_LOG(Log, "Created rock at position (%f, %f), type: %s, flipped: %d",
// 		x, y,
// 		type == Rock::RockType::NARROW ? "NARROW" : "WIDE",
// 		flipped);
}

void XenonLevel::updateScore()
{
	if (m_displayScore)
	{
		// Format score with leading zeros to maintain consistent width
		std::string scoreText = std::to_string(m_score);
		scoreText = std::string(10 - scoreText.length(), '0') + scoreText;
		m_displayScore->setText(scoreText);
	}
	//E2_LOG(Warning, "Current score: %d", m_score);
}

void XenonLevel::addScore(int amount)
{
	if (m_score < 9999999999)
	{
		//E2_LOG(Log, "Adding to score: %d", amount);
		m_score += amount;
	}
	updateScore();
}