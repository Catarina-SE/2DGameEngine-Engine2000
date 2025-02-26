#include "LonerProjectile.h"

#include "Engine2000/PhysicsComponent.h"
#include "Engine2000/Level.h"
#include "Engine2000/E2Log.h"

#include "IDamageable.h"
#include "XenonLevel.h"
#include "Explosion.h"
#include "ExplosionProjectile.h"

LonerProjectile::LonerProjectile()
	: m_damage(25.0f)
{}

void LonerProjectile::init()
{
	// First set up sprite
	auto mySprite = setSprite("graphics/EnWeap6.bmp", 8, 1);
	mySprite->setAnimationMode(SpriteComponent::LOOP);
	mySprite->setFrameDelay(0.1f);

	auto physics = getComponent<PhysicsComponent>();
	physics->init(getLevel()->getPhysicsWorld(), true, true);
	physics->createCollisionShapeFromSprite();
	physics->createSensorShapeFromSprite(this);
	physics->setLayer("EnemyProjectile");
	physics->setDebugDraw(false);	// Debug Draw
	physics->setDebugColor(PhysicsComponent::DebugColor::Yellow);

	Projectile::init();
}

void LonerProjectile::onSensorBegin(GameObject* other)
{
	if (!isActive()) return;

	//E2_LOG(Warning, "Loner Projectile Sensor Begin");

	auto physics = getComponent<PhysicsComponent>();
	auto otherPhysics = other->getComponent<PhysicsComponent>();

	if (!otherPhysics) return;

	auto damageable = dynamic_cast<IDamageable*>(other);

	if (damageable && otherPhysics->getLayer() != "Enemy" && otherPhysics->getLayer() != "EnemyProjectile")
	{
		auto explosion = getLevel()->createGameObject<ExplosionProjectile>();
		explosion->spawn(getTransform()->getPosition());

		//E2_LOG(Warning, "Loner Projectile Applying Damage");
		damageable->takeDamage(m_damage);
		deactivate();
		getLevel()->removeGameObject(this);
	}
}

void LonerProjectile::takeDamage(float amount)
{
	// Create explosion before destroying
	Vector2D explosionPos = getTransform()->getPosition();
	if (auto level = getLevel()) {
		auto explosion = level->createGameObject<Explosion>();
		explosion->spawn(explosionPos, 0.3f);
	}

	deactivate();
	getLevel()->removeGameObject(this);
}
