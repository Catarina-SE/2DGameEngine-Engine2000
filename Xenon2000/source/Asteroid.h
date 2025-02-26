#pragma once

#include "Engine2000/GameObject.h"
#include "Engine2000/SpriteComponent.h"
#include "Engine2000/PhysicsComponent.h"
#include "Engine2000/ScreenBoundsComponent.h"
#include "IDamageable.h"

class Asteroid : public GameObject, public PhysicsSensorListener, public IBoundsResponder, public IDamageable {
public:
	enum class Size {
		LARGE,
		MEDIUM,
		SMALL
	};

private:
	SpriteComponent* m_sprite;
	PhysicsComponent* m_physics;
	ScreenBoundsComponent* m_bounds;
	Size m_size;
	float m_rotationSpeed;
	float m_moveSpeed;
	const float m_damage;
	Size m_currentSize;
	int m_baseScore;
	int m_scoreValue;

public:
	Asteroid(Size size = Size::LARGE);
	virtual void init() override;
	virtual void update(float deltaTime) override;
	virtual void onSensorBegin(GameObject* other) override;
	virtual void takeDamage(float amount) override;

private:
	void spawnSmallerAsteroids();
};

