#include "XenonWaveManager.h"
#include "XenonLevel.h"
#include "Engine2000/E2Log.h"
#include "Asteroid.h"
#include "MetalAsteroid.h"
#include "Drone.h"

XenonWaveManager::XenonWaveManager(XenonLevel* level)
	: m_level(level)
	, m_spawnTimers{}
	, m_isSpawningDrones(false)
	, m_dronesToSpawn(6)
	, m_dronesRemaining(0)
	, m_droneSpawnTimer(0.0f)
	, m_droneGroupX(0.0f)
	, m_isSpawningRocks(false)
	, m_rocksRemaining(0)
	, m_rockSpawnTimer(0.0f)
	, m_rockSpawnSide(false)
	, m_rockSpawnType(Rock::RockType::NARROW)
{

	m_spawnConfigs = {
	   {SpawnType::Loner, {1.0f, 0.40f}},      // Checks every % seconds, Chance to Spawn
	   {SpawnType::Rusher, {1.2f, 0.60f}},    // Checks every % seconds, Chance to Spawn
	   {SpawnType::DroneGroup, {2.0f, 0.30f}},      // Checks every % seconds, Chance to Spawn
	   {SpawnType::Asteroid, {2.0f, 0.30f}},      // Checks every % seconds, Chance to Spawn
	   {SpawnType::MetalAsteroid, {2.0f, 0.20f}}, // Checks every % seconds, Chance to Spawn
	   {SpawnType::ShieldPowerUp, {5.0f, 0.20f}}, // Checks every % seconds, Chance to Spawn
	   {SpawnType::WeaponPowerUp, {7.0f, 0.15f}},  // Checks every % seconds, Chance to Spawn
	   {SpawnType::CompanionPowerUp, {7.0f, 0.15f}},  // Checks every % seconds, Chance to Spawn
	   {SpawnType::Rock, {1.5f, 0.50f}}  // Checks every % seconds, Chance to Spawn
	};
}

void XenonWaveManager::update(float deltaTime)
{
	if (!m_level) return;

	// Handle ongoing spawns
	if (m_isSpawningDrones)
	{
		updateDroneGroupSpawning(deltaTime);
	}
	if (m_isSpawningRocks)
	{
		updateRockSpawning(deltaTime);
	}

	// Update spawn timers and checks
	for (auto& [type, config] : m_spawnConfigs)
	{
		m_spawnTimers[type] += deltaTime;

		if (m_spawnTimers[type] >= config.interval)
		{
			m_spawnTimers[type] = 0.0f;
			float roll = static_cast<float>(rand()) / RAND_MAX;
			if (roll < config.chance)
			{
				spawnEntity(type);
			}
		}
	}
}

void XenonWaveManager::startDroneGroup()
{
	float screenWidth = static_cast<float>(m_level->getScreenWidth());
	float margin = 64.0f;
	m_droneGroupX = margin + (static_cast<float>(rand()) / RAND_MAX) * (screenWidth - 2.0f * margin);

	m_isSpawningDrones = true;
	m_dronesRemaining = m_dronesToSpawn;
	m_droneSpawnTimer = 0.0f;
	//E2_LOG(Warning, "Starting drone group spawn sequence at x position: %f", m_droneGroupX);
}

void XenonWaveManager::updateDroneGroupSpawning(float deltaTime)
{
	if (!m_isSpawningDrones || m_dronesRemaining <= 0) return;

	m_droneSpawnTimer += deltaTime;

	if (m_droneSpawnTimer >= DRONE_SPAWN_INTERVAL)
	{
		m_droneSpawnTimer = 0.0f;

		// Calculate Y position based on remaining drones
		float baseY = -64.0f;
		float spacing = 150.0f;
		float spawnY = baseY - ((m_dronesToSpawn - m_dronesRemaining) * spacing);

		m_dronesRemaining--;
		
		int phase = m_dronesToSpawn - m_dronesRemaining;

		m_level->createDrone(m_droneGroupX, spawnY, phase);

		if (m_dronesRemaining <= 0)
		{
			m_isSpawningDrones = false;
			//E2_LOG(Warning, "Drone spawn sequence complete");
		}
	}
}

void XenonWaveManager::spawnEntity(SpawnType type)
{
	switch (type)
	{
	case SpawnType::Loner:
		spawnLoner();
		break;

	case SpawnType::Rusher:
		spawnRusher();
		break;

	case SpawnType::DroneGroup:
		if (!m_isSpawningDrones)
		{
			startDroneGroup();
		}
		break;

	case SpawnType::Asteroid:
		spawnAsteroid();
		break;
	
	case SpawnType::MetalAsteroid:
		spawnMetalAsteroid();
		break;

	case SpawnType::ShieldPowerUp:
		spawnShieldPowerUp();
		break;

	case SpawnType::WeaponPowerUp:
		spawnWeaponPowerUp();
		break;

	case SpawnType::CompanionPowerUp:
		spawnCompanionPowerUp();
		break;
		
	case SpawnType::Rock:
		if (!m_isSpawningRocks)
		{
			startRockGroup();
		}
		break;

	default:
		E2_LOG(Warning, "Unknown spawn type: %d", static_cast<int>(type));
		break;
	}
}

void XenonWaveManager::spawnLoner()
{
	float y = static_cast<float>(rand() % (m_level->getScreenHeight() / 2));
	bool fromRight = (rand() % 2 == 0);
	m_level->createLoner(y, fromRight);
	//E2_LOG(Log, "Spawned Loner at y: %f, from %s", y, fromRight ? "right" : "left");
}

void XenonWaveManager::spawnRusher()
{
	float x = static_cast<float>(rand() % m_level->getScreenWidth());
	bool fromTop = (rand() % 2 == 0);
	m_level->createRusher(x, fromTop);
	//E2_LOG(Log, "Spawned Rusher at x: %f, from %s", x, fromTop ? "top" : "bottom");
}

void XenonWaveManager::spawnAsteroid()
{
	float screenWidth = static_cast<float>(m_level->getScreenWidth());
	float margin = 96.0f;
	float x = margin + (static_cast<float>(rand()) / RAND_MAX) * (screenWidth - 2 * margin);
	float y = 0.0f;

	// Random size selection
	float sizeRoll = static_cast<float>(rand()) / RAND_MAX;
	Asteroid::Size size;
	if (sizeRoll < 0.33f) {
		size = Asteroid::Size::LARGE;
	}
	else if (sizeRoll < 0.66f) {
		size = Asteroid::Size::MEDIUM;
	}
	else {
		size = Asteroid::Size::SMALL;
	}

	m_level->createAsteroid(x, y, size);
	//E2_LOG(Log, "Spawned Asteroid at position (%f, %f)", x, y);
}


void XenonWaveManager::spawnMetalAsteroid()
{
	float screenWidth = static_cast<float>(m_level->getScreenWidth());
	float margin = 96.0f;
	float x = margin + (static_cast<float>(rand()) / RAND_MAX) * (screenWidth - 2 * margin);
	float y = 0.0f;

	// Random size selection
	float sizeRoll = static_cast<float>(rand()) / RAND_MAX;
	MetalAsteroid::Size size;
	if (sizeRoll < 0.33f) {
		size = MetalAsteroid::Size::LARGE;
	}
	else if (sizeRoll < 0.66f) {
		size = MetalAsteroid::Size::MEDIUM;
	}
	else {
		size = MetalAsteroid::Size::SMALL;
	}

	m_level->createMetalAsteroid(x, y, size);
	//E2_LOG(Log, "Spawned Metal Asteroid at position (%f, %f)", x, y);
}

void XenonWaveManager::startRockGroup()
{
	m_isSpawningRocks = true;
	m_rocksRemaining = 2 + (rand() % 4); // Random between 2 and 5 rocks
	m_rockSpawnTimer = 0.0f;
	m_rockSpawnSide = (rand() % 2) == 0;  // Random side
	m_rockSpawnType = (rand() % 2 == 0) ? Rock::RockType::NARROW : Rock::RockType::WIDE;

// 	E2_LOG(Log, "Starting rock group spawn sequence - Count: %d, Side: %s, Type: %s",
// 		m_rocksRemaining,
// 		m_rockSpawnSide ? "LEFT" : "RIGHT",
// 		m_rockSpawnType == Rock::RockType::NARROW ? "NARROW" : "WIDE");
}

void XenonWaveManager::updateRockSpawning(float deltaTime)
{
	if (!m_isSpawningRocks || m_rocksRemaining <= 0) return;

	m_rockSpawnTimer += deltaTime;
	if (m_rockSpawnTimer >= ROCK_SPAWN_INTERVAL)
	{
		m_rockSpawnTimer = 0.0f;

		float screenWidth = static_cast<float>(m_level->getScreenWidth());
		float x;
		bool shouldFlip;

		if (m_rockSpawnType == Rock::RockType::NARROW) {
			if (m_rockSpawnSide) {  // Left side
				x = 0.0f;
				shouldFlip = false;
			}
			else {  // Right side
				x = screenWidth - 64.0f;
				shouldFlip = true;
			}
		}
		else {  // WIDE type
			if (m_rockSpawnSide) {  // Left side
				x = -224.0f;
				shouldFlip = true;
			}
			else {  // Right side
				x = screenWidth - 180.0f;
				shouldFlip = false;
			}
		}

		// Add some vertical spacing between rocks
		float baseY = -64.0f;
		float spacing = 100.0f;
		float spawnY = baseY - ((m_rocksRemaining - 1) * spacing);

		m_level->createRock(x, spawnY, m_rockSpawnType, shouldFlip);
		//E2_LOG(Log, "Spawned rock %d at position (%f, %f)", m_rocksRemaining, x, spawnY);

		m_rocksRemaining--;
		if (m_rocksRemaining <= 0)
		{
			m_isSpawningRocks = false;
			//E2_LOG(Log, "Rock spawn sequence complete");
		}
	}
}

void XenonWaveManager::spawnShieldPowerUp()
{
	float screenWidth = static_cast<float>(m_level->getScreenWidth());
	float margin = 32.0f;
	float x = margin + (static_cast<float>(rand()) / RAND_MAX) * (screenWidth - 2 * margin);
	m_level->createShieldPowerUp(x, -32.0f);
	//E2_LOG(Log, "Spawned Shield PowerUp at x: %f", x);
}

void XenonWaveManager::spawnWeaponPowerUp()
{
	float screenWidth = static_cast<float>(m_level->getScreenWidth());
	float margin = 32.0f;
	float x = margin + (static_cast<float>(rand()) / RAND_MAX) * (screenWidth - 2 * margin);
	m_level->createWeaponPowerUp(x, -32.0f);
	//E2_LOG(Log, "Spawned Weapon PowerUp at x: %f", x);
}

void XenonWaveManager::spawnCompanionPowerUp()
{
	float screenWidth = static_cast<float>(m_level->getScreenWidth());
	float margin = 32.0f;
	float x = margin + (static_cast<float>(rand()) / RAND_MAX) * (screenWidth - 2 * margin);
	m_level->createCompanionPowerUp(x, -32.0f);
	//E2_LOG(Log, "Spawned Companion PowerUp at x: %f", x);
}
