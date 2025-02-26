#pragma once

#include "Engine2000/Pawn.h"

#include "IDamageable.h"
#include "Companion.h"
#include "PlayerProjectile.h"
#include "LifeDisplay.h"
#include "HealthBar.h"

// Structure to hold companion pointers
struct CompanionPair {
	Companion* left = nullptr;
	Companion* right = nullptr;
};

class Player : public Pawn, public IDamageable, public CompanionEventHandler
{
	enum class Direction {
		LEFT,
		RIGHT,
		NEUTRAL
	};
	static const int NEUTRAL_FRAME = 3;
	static const int LEFT_START_FRAME = 2;   // Start from frame 2 when going left
	static const int RIGHT_START_FRAME = 4;  // Start from frame 4 when going right
	static const int LEFT_END_FRAME = 0;     // End at frame 0 when going left
	static const int RIGHT_END_FRAME = 6;    // End at frame 6 when going right
	static const int FIRST_DEATH_FRAME = 13;  // First frame in last row
	static const int LAST_DEATH_FRAME = 20;
	static const float DEATH_FRAME_TIME;

	float m_moveSpeed;
	Direction m_currentDirection;
	Direction m_lastDirection;
	float m_animationTimer;
	int m_currentAnimFrame;
	bool m_isAnimating;
	float m_fireDelay;
	float m_projectileSpeed;
	float m_timeSinceLastShot;
	float m_respawnTime;
	float m_respawnTimer;
	bool m_isVisible;
	CompanionPair m_companions;
	PlayerProjectile::ProjectileType m_currentProjectileType;
	int m_lifes;
	bool m_isDying;
	int m_deathAnimFrame;
	float m_deathTimer;
	LifeDisplay* m_lifeDisplay;
	HealthBar* m_healthBar;

public:
	Player();
	virtual void init() override;
	virtual void update(float deltaTime) override;
	void shoot();

	// Companion management
	void addCompanion();
	void removeCompanions();
	virtual void onCompanionDeath(bool isLeftSide) override;

	virtual void takeDamage(float amount) override;
	virtual void gainHealth(float amount) override;

	void upgradeWeapon();
private:
	void updateAnimation(float deltaTime);
	void updateDeathAnimation(float deltaTime);
	void startDirectionalAnimation(Direction newDirection);
	void hide();
	void die();
};