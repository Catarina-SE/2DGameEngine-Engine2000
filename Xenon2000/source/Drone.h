#pragma once

#include "Engine2000/GameObject.h"
#include "Engine2000/Vector2D.h"
#include "Engine2000/SpriteComponent.h"
#include "Engine2000/ScreenBoundsComponent.h"
#include "Engine2000/PhysicsComponent.h"
#include "IDamageable.h"

class Drone : public GameObject, public PhysicsSensorListener, public IBoundsResponder, public IDamageable
{
protected:
	SpriteComponent* m_sprite;
	PhysicsComponent* m_physics;
	ScreenBoundsComponent* m_boundsComponent;

	float m_descendSpeed;    
	float m_oscillateSpeed;  
	float m_oscillateWidth;   
	float m_time;            
	float m_startX;
	int m_baseScore;
	int m_scoreValue;

	const float m_damage;     
	float m_doDamageTimer;
	float m_timeBetweenDamage;
	bool m_canDamage;

public:
	Drone();
	virtual ~Drone() = default;

	virtual void init() override;
	virtual void update(float deltaTime) override;
	virtual void onSensorBegin(GameObject* other) override;
	void spawn(float x, float y, int phase);
	virtual void takeDamage(float amount) override;
};