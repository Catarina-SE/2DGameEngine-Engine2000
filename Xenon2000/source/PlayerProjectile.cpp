#include "PlayerProjectile.h"

#include "Engine2000/PhysicsComponent.h"
#include "Engine2000/Level.h"
#include "Engine2000/E2Log.h"

#include "IDamageable.h"
#include "Explosion.h"
#include "ExplosionProjectile.h"

PlayerProjectile::PlayerProjectile(ProjectileType type)
	: m_damage(25.0f)
	, m_type(type)
{
	// Initialize the current projectile type
	updateProjectileType();
}

void PlayerProjectile::init()
{
	auto physics = getComponent<PhysicsComponent>();
	physics->init(getLevel()->getPhysicsWorld(), true, true);
	physics->createSensorShapeFromSprite(this);
	physics->setLayer("PlayerProjectile");
	physics->setDebugDraw(false);	// Debug Draw
	physics->setDebugColor(PhysicsComponent::DebugColor::Yellow);

	Projectile::init();
}

void PlayerProjectile::onSensorBegin(GameObject* other)
{
	if (!isActive()) return;

	auto physics = getComponent<PhysicsComponent>();
	auto otherPhysics = other->getComponent<PhysicsComponent>();

	if (!otherPhysics) return;
	auto damageable = dynamic_cast<IDamageable*>(other);

	if (damageable && otherPhysics->getLayer() != "Player")
	{
		//E2_LOG(Warning, "Player Projectile Applying Damage");
		damageable->takeDamage(m_damage);
		auto explosion = getLevel()->createGameObject<ExplosionProjectile>();
		explosion->spawn(getTransform()->getPosition());
		deactivate();
		getLevel()->removeGameObject(this);
	}
	if (otherPhysics->getLayer() == "MetalAsteroid")
	{
		auto explosion = getLevel()->createGameObject<ExplosionProjectile>();
		explosion->spawn(getTransform()->getPosition());
		deactivate();
		getLevel()->removeGameObject(this);
	}
}

void PlayerProjectile::setProjectileType(ProjectileType type)
{
	m_type = type;
	updateProjectileType();
}

void PlayerProjectile::updateProjectileType()
{
	auto mySprite = setSprite("graphics/missile.bmp", 2, 3);
	switch(m_type)
	{
	case ProjectileType::Light:
		m_damage = 25.0f;
		mySprite->setFrameRange(0, 1); // First two frames
		break;
	case ProjectileType::Medium:
		m_damage = 50.0f;
		mySprite->setFrameRange(2, 3); // Second two frames
		break;
	case ProjectileType::Heavy:
		m_damage = 100.0f;
		mySprite->setFrameRange(4, 5); // Last two frames
		break;
	}
}