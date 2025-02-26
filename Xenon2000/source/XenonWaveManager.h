#pragma once
#include <unordered_map>
#include "Rock.h"

class XenonLevel;
class Rock;

class XenonWaveManager {
private:

	static constexpr float DRONE_SPAWN_INTERVAL = 0.5f;

	// Enum for different types of entities that can spawn
	enum class SpawnType {
		Loner,
		Rusher,
		DroneGroup,
		Asteroid,
		MetalAsteroid,
		ShieldPowerUp,
		WeaponPowerUp,
		CompanionPowerUp,
		Rock
	};


	struct SpawnConfig {
		float interval;
		float chance;
	};

	XenonLevel* m_level;
	std::unordered_map<SpawnType, SpawnConfig> m_spawnConfigs;
	std::unordered_map<SpawnType, float> m_spawnTimers;


	bool m_isSpawningDrones;
	int m_dronesToSpawn;
	int m_dronesRemaining;
	float m_droneSpawnTimer;
	float m_droneGroupX;

	bool m_isSpawningRocks;
	int m_rocksRemaining;
	float m_rockSpawnTimer;
	bool m_rockSpawnSide;     // true = left, false = right
	Rock::RockType m_rockSpawnType;
	static constexpr float ROCK_SPAWN_INTERVAL = 0.3f;

	void spawnEntity(SpawnType type);
	void spawnLoner();
	void spawnRusher();
	void spawnAsteroid();
	void startDroneGroup();
	void updateDroneGroupSpawning(float deltaTime);
	void spawnMetalAsteroid();
	void startRockGroup();
	void updateRockSpawning(float deltaTime);

	void spawnShieldPowerUp();
	void spawnWeaponPowerUp();
	void spawnCompanionPowerUp();
public:
	XenonWaveManager(XenonLevel* level);
	void update(float deltaTime);
};