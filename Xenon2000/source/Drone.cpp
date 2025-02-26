#include "Drone.h"
#include "Engine2000/E2Log.h"

#include "Explosion.h"
#include "XenonLevel.h"
#include "ExplosionProjectile.h"
#include "TextDisplay.h"

#include <cmath>

Drone::Drone()
	: m_descendSpeed(1.30f)
	, m_oscillateSpeed(2.0f)
	, m_oscillateWidth(30.0f)
	, m_time(0.0f)
	, m_startX(0.0f)
	, m_damage(25.0f)
	, m_timeBetweenDamage(0.1f)
	, m_canDamage(true)
	, m_baseScore(5000)
	, m_scoreValue(0)
{
	// Initialize IDamageable variables
	m_maxHealth = 50.0f;
	m_health = m_maxHealth;
	m_isAlive = true;

	// Create components
	m_sprite = addComponent<SpriteComponent>();
	m_sprite->setAnimatedTexture("graphics/drone.bmp", 8, 2);
	m_sprite->setAnimationMode(SpriteComponent::LOOP);
	m_sprite->setFrameDelay(0.1f);

	m_boundsComponent = addComponent<ScreenBoundsComponent>();
	m_physics = addComponent<PhysicsComponent>();
	m_physics->setLayer("Enemy");
	m_physics->setDebugDraw(false);	// Debug Draw
	m_physics->setDebugColor(PhysicsComponent::DebugColor::Blue);
}

void Drone::init()
{
	m_physics->init(getLevel()->getPhysicsWorld(), true, false);
	m_physics->createSensorShapeFromSprite(this);
	m_physics->createCollisionShapeFromSprite();

	GameObject::init();
}

void Drone::update(float deltaTime)
{
	if (!m_isAlive) return;

	m_time += deltaTime;

	
	auto currentPos = getTransform()->getPosition();
	float newX = m_startX + sin(m_time * m_oscillateSpeed) * m_oscillateWidth;
	float newY = currentPos.y + (m_descendSpeed * deltaTime * 60.0f);

	
	m_physics->setPosition(Vector2D(newX, newY));

	
	if (!m_canDamage)
	{
		m_doDamageTimer -= deltaTime;
		if (m_doDamageTimer <= 0)
		{
			m_canDamage = true;
			if (m_sprite)
			{
				m_sprite->setVisible(true);
			}
		}
	}

	GameObject::update(deltaTime);
}

void Drone::onSensorBegin(GameObject* other)
{
	auto otherPhysics = other->getComponent<PhysicsComponent>();
	if (!otherPhysics || otherPhysics->isImmune()) return;

	auto damageable = dynamic_cast<IDamageable*>(other);
	if (damageable && otherPhysics->getLayer() != "Enemy" && otherPhysics->getLayer() != "EnemyProjectile")
	{
		m_doDamageTimer = m_timeBetweenDamage;
		m_canDamage = false;
		damageable->takeDamage(m_damage);
	}
}

void Drone::spawn(float x, float y, int phase)
{
	m_startX = x;
	m_scoreValue = m_baseScore * phase;
	//E2_LOG(Error, "Drone phase: %d ; Score Value: %d", phase, m_scoreValue);

	if (auto physics = getComponent<PhysicsComponent>())
	{
		physics->setPosition(Vector2D(x, y));

		auto pos = getTransform()->getPosition();
		//E2_LOG(Log, "Drone spawned and positioned at (%f, %f)", pos.x, pos.y);
	}
}

void Drone::takeDamage(float amount)
{
	if (!m_isAlive) return;

	m_health = std::max(0.0f, m_health - amount);
	if (m_health <= 0)
	{
		m_isAlive = false;

		// Create explosion before destroying
		Vector2D explosionPos = getTransform()->getPosition();
		if (auto level = getLevel()) {
			auto explosion = level->createGameObject<Explosion>();
			explosion->spawn(explosionPos, 0.5f);
		}

		if (auto xenonLevel = dynamic_cast<XenonLevel*>(m_level))
		{
			auto scorePopup = xenonLevel->createGameObject<TextDisplay>(false, true, 1.0f);
			scorePopup->setPosition(getTransform()->getPosition());
			scorePopup->setText(std::to_string(m_scoreValue));
			xenonLevel->addScore(m_scoreValue);
			xenonLevel->removeGameObject(this);
		}
	}
}