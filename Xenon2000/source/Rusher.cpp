#include "Rusher.h"
#include "Engine2000/PhysicsComponent.h"
#include "Engine2000/E2Log.h"

#include "LonerProjectile.h"
#include "XenonLevel.h"
#include "Explosion.h"
#include "TextDisplay.h"

Rusher::Rusher(int screenHeight)
	: m_moveSpeed(0.5f)
	, m_movingDown(true)
	, m_projectileSpeed(1.0f)
	, m_fireDelay(2.0f)
	, m_timeSinceLastShot(0.0f)
	, m_screenHeight(screenHeight)
	, m_damage(25.0f)
	, m_timeBetweenDamage(0.1f)
	, m_canDamage(true)
	, m_scoreValue(10000)
{
	//Initialize IDamageable variables
	m_maxHealth = 75.0f;
	m_health = m_maxHealth;
	m_isAlive = true;

	// Create and set up sprite component
	m_sprite = addComponent<SpriteComponent>();
	m_sprite->setAnimatedTexture("graphics/rusher.bmp", 4, 6);
	m_sprite->setAnimationMode(SpriteComponent::LOOP);
	m_sprite->setFrameDelay(0.1f);

	// Create screen bounds component
	m_boundsComponent = addComponent<ScreenBoundsComponent>();

	// Create physics component
	m_physics = addComponent<PhysicsComponent>();
	m_physics->setLayer("Enemy");
	m_physics->setDebugDraw(false);	// Debug Draw
	m_physics->setDebugColor(PhysicsComponent::DebugColor::Blue);
}

void Rusher::init()
{
	m_physics->init(getLevel()->getPhysicsWorld(), true, false);
	m_physics->createCollisionShapeFromSprite();
	m_physics->createSensorShapeFromSprite(this);

	GameObject::init();
	
}

void Rusher::update(float deltaTime)
{
	auto physics = getComponent<PhysicsComponent>();
	if (!physics) return;

	// Timer for taking damage
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

void Rusher::onSensorBegin(GameObject* other)
{
	auto otherPhysics = other->getComponent<PhysicsComponent>();

	if (!otherPhysics) return;

	auto damageable = dynamic_cast<IDamageable*>(other);

	if (damageable && otherPhysics->getLayer() != "Enemy" && otherPhysics->getLayer() != "EnemyProjectile")
	{
		//E2_LOG(Warning, "Other is Damageable");
		m_doDamageTimer = m_timeBetweenDamage;
		m_canDamage = false;
		damageable->takeDamage(m_damage);
	}
}

void Rusher::spawn(SpawnSide side, float x)
{
	float spawnY;
	if (side == SpawnSide::TOP)
	{
		spawnY = -m_sprite->getFrameHeight();
		m_movingDown = true;
	}
	else
	{
		spawnY = m_screenHeight;
		m_movingDown = false;
	}

	// Set initial position through physics
	auto physics = getComponent<PhysicsComponent>();
	if (physics)
	{
		physics->setPosition(Vector2D(x, spawnY));

		// Set initial velocity
		float velocityY = m_movingDown ? m_moveSpeed : -m_moveSpeed;
		physics->setVelocity(Vector2D(0.0f, velocityY));

		//E2_LOG(Log, "Rusher spawned at position (%f, %f) with velocity %f", x, spawnY, velocityY);
	}
}

void Rusher::takeDamage(float amount)
{
	if (!m_isAlive) return;

	m_health = std::max(0.0f, m_health - amount);
	if (m_health <= 0) {
		m_isAlive = false;

		// Create explosion before destroying
		Vector2D explosionPos = getTransform()->getPosition();
		if (auto level = getLevel()) {
			auto explosion = level->createGameObject<Explosion>();
			explosion->spawn(explosionPos, 0.8f);
		}

		// Handle death
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
