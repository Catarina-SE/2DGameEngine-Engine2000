#pragma once
#include "Engine2000/GameObject.h"
#include "Engine2000/Vector2D.h"
#include "Engine2000/SpriteComponent.h"
#include "Engine2000/ScreenBoundsComponent.h"
#include "Engine2000/PhysicsComponent.h"

#include "IDamageable.h"

class Rusher : public GameObject, public PhysicsSensorListener, public IBoundsResponder, public IDamageable
{
private:
	const float m_damage;
	float m_doDamageTimer;
	float m_timeBetweenDamage;
	bool m_canDamage;
	int m_scoreValue;

private:
	SpriteComponent* m_sprite;
	PhysicsComponent* m_physics;
	ScreenBoundsComponent* m_boundsComponent;
	float m_moveSpeed;
	bool m_movingDown;
	float m_projectileSpeed;
	float m_fireDelay;
	float m_timeSinceLastShot;
	int m_screenHeight;

public:
	enum class SpawnSide
	{
		TOP,
		BOTTOM
	};

	Rusher(int screenHeight);
	virtual void init() override;
	virtual void update(float deltaTime) override;

	virtual void onSensorBegin(GameObject* other) override;

	// Setters
	void setMoveSpeed(float speed) { m_moveSpeed = speed; }
	void spawn(SpawnSide side, float y);

public:
	virtual void takeDamage(float amount) override;
	virtual float getHealth() const override { return m_health; }
	virtual bool isAlive() const override { return m_isAlive; }
};

