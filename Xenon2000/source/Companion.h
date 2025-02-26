#pragma once

#include "Engine2000/GameObject.h"
#include "Engine2000/SpriteComponent.h"
#include "Engine2000/PhysicsComponent.h"
#include "IDamageable.h"
#include "PlayerProjectile.h"

class Player;

// Companion event handler
class CompanionEventHandler {
public:
	virtual ~CompanionEventHandler() = default;
	virtual void onCompanionDeath(bool isLeftSide) = 0;
};

class Companion : public GameObject, public IDamageable {
private:
	SpriteComponent* m_sprite;
	PhysicsComponent* m_physics;
	Player* m_player;
	float m_projectileSpeed;
	bool m_isLeftSide;
	float m_horizontalOffset;
	float m_verticalOffset;
	
	// Death animation variables
	bool m_isDying;
	int m_deathAnimFrame;
	float m_deathTimer;
	bool m_isBeingRemoved;
	
	// Damage display variables
	float m_damageDisplayTime;
	bool m_isShowingDamage;
	int m_lastNormalFrame;  // Last frame before damage frames
	
	CompanionEventHandler* m_eventHandler;
	PlayerProjectile::ProjectileType m_currentProjectileType;

	// Animation constants
	static const int NORMAL_FRAMES = 16;  // 4x4 grid for normal animation
	static const int FIRST_DEATH_FRAME = 16;  // First frame in last row
	static const int DAMAGE_FRAME = 19;  // Last frame in the sprite sheet
	static const float DAMAGE_DISPLAY_DURATION;  // How long to show damage frame
	static const float DEATH_FRAME_TIME;  // Time between death animation frames

public:
	Companion(Player* player, bool isLeftSide, float projectileSpeed);
	virtual void init() override;
	virtual void update(float deltaTime) override;

	// Combat
	void shoot();
	virtual void takeDamage(float amount) override;
	void setEventHandler(CompanionEventHandler* handler) { m_eventHandler = handler; }

	void upgradeWeapon();

private:
	void updatePosition(float deltaTime);
	void updateDeathAnimation(float deltaTime);
	void die();
};