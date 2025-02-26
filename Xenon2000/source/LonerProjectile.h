#pragma once

#include "Engine2000/Projectile.h"
#include "IDamageable.h"

class LonerProjectile : public Projectile, public IDamageable
{
private:
	const float m_damage;
public:
	LonerProjectile();
	virtual void init() override;
	virtual void onSensorBegin(GameObject* other) override;

	virtual void takeDamage(float amount) override;
};
