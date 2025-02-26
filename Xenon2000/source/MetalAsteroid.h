#pragma once

#include "Engine2000/GameObject.h"
#include "Engine2000/Vector2D.h"
#include "Engine2000/PhysicsComponent.h"
#include "IDamageable.h"

class SpriteComponent;
class PhysicsComponent;
class ScreenBoundsComponent;

class MetalAsteroid : public GameObject, public PhysicsSensorListener {
public:
	enum class Size {
		LARGE,   // 5x5 sprite sheet
		MEDIUM,  // 8x3 sprite sheet
		SMALL    // 8x2 sprite sheet
	};

private:
	SpriteComponent* m_sprite;
	PhysicsComponent* m_physics;
	ScreenBoundsComponent* m_bounds;
	Size m_size;
	float m_moveSpeed;
	const float m_damage = 100.0f;
	float m_animationTimer;
	float m_frameDelay;

public:
	MetalAsteroid(Size size = Size::LARGE);
	virtual void init() override;

	// Spawn the asteroid at a specific position
	void spawn(const Vector2D& position);

	// Adjust movement speed
	void setMoveSpeed(float speed);
	float getMoveSpeed() const { return m_moveSpeed; }

	// Get components
	SpriteComponent* getSprite() { return m_sprite; }
	PhysicsComponent* getPhysics() { return m_physics; }
	ScreenBoundsComponent* getBounds() { return m_bounds; }

	// Get size
	Size getSize() const { return m_size; }

	virtual void onSensorBegin(GameObject* other) override;
};