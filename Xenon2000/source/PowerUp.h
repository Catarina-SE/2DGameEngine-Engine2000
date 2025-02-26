#pragma once

#include "Engine2000/GameObject.h"
#include "Engine2000/SpriteComponent.h"
#include "Engine2000/PhysicsComponent.h"
#include "Engine2000/ScreenBoundsComponent.h"


class PowerUp : public GameObject, public PhysicsSensorListener
{
private:
	SpriteComponent* m_sprite;
	PhysicsComponent* m_physics;
	float m_timeInLevel;
	float m_moveSpeed;
	ScreenBoundsComponent* m_boundsComponent;

public:
	PowerUp();
	void update(float deltaTime) override;
	void init() override;
	void spawn(const Vector2D& position);

	virtual void onSensorBegin(GameObject* other) override;

	virtual void applyEffect(GameObject* target) = 0;
};

