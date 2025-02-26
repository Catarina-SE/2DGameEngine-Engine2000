#pragma once
#include "Engine2000/Projectile.h"

class PlayerProjectile : public Projectile
{
public:
	enum class ProjectileType
	{
		Light,
		Medium,
		Heavy
	};

private:
	float m_damage;
	ProjectileType m_type;
public:
	PlayerProjectile() : PlayerProjectile(ProjectileType::Light) {}
	PlayerProjectile(ProjectileType Type);
	virtual void init() override;
	virtual void onSensorBegin(GameObject* other) override;
	void setProjectileType(ProjectileType type);
	void updateProjectileType();
};