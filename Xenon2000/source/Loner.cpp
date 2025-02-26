#include "Loner.h"
#include "Engine2000/E2Log.h"

#include "LonerProjectile.h"
#include "XenonLevel.h"
#include "Explosion.h"
#include "TextDisplay.h"

Loner::Loner(int screenWidth)
	: m_moveSpeed(0.5f)
	, m_movingRight(true)
	, m_projectileSpeed(1.0f)
	, m_fireDelay(2.0f)
	, m_timeSinceLastShot(0.0f)
	, m_screenWidth(screenWidth)
	, m_damage(25.0f)
	, m_timeBetweenDamage(0.1f)
	, m_canDamage(true)
	, m_scoreValue(10000)
{
	// Initialize IDamageable variables
	m_maxHealth = 50.0f;
	m_health = m_maxHealth;
	m_isAlive = true;

	// Create and set up sprite component
	m_sprite = addComponent<SpriteComponent>();
	m_sprite->setAnimatedTexture("graphics/LonerA.bmp", 4, 4);
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
void Loner::init()
{
	m_physics->init(getLevel()->getPhysicsWorld(), true, false);
	m_physics->createCollisionShapeFromSprite();
	m_physics->createSensorShapeFromSprite(this);

	GameObject::init();
}

void Loner::update(float deltaTime)
{
	auto physics = getComponent<PhysicsComponent>();
	if (!physics) return;

	
	m_timeSinceLastShot += deltaTime;
	if (m_timeSinceLastShot >= m_fireDelay)
	{
		m_timeSinceLastShot = 0.0f;
		shoot();
	}

	
	m_timeSinceLastShot += deltaTime;
	if (m_timeSinceLastShot >= m_fireDelay)
	{
		m_timeSinceLastShot = 0.0f;
		shoot();
	}

	
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

void Loner::shoot()
{
	Level* myLevel = getLevel();
	if (!myLevel) return;

	auto projectile = myLevel->createGameObject<LonerProjectile>();
	auto pos = getTransform()->getPosition();

	float projectileX = pos.x + (m_sprite->getFrameWidth() * 0.5f);
	float projectileY = pos.y + m_sprite->getFrameHeight();

	projectile->setPosition(projectileX, projectileY);
	projectile->setDirection(Vector2D(0.0f, 1.0f));
	projectile->setSpeed(m_projectileSpeed);

	if (auto projectilePhysics = projectile->getComponent<PhysicsComponent>())
	{
		projectilePhysics->setImmunity(0.05f);
	}
}

void Loner::onSensorBegin(GameObject* other)
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

void Loner::spawn(SpawnSide side, float y)
{
	float spawnX;
	if (side == SpawnSide::LEFT)
	{
		spawnX = -m_sprite->getFrameWidth();
		m_movingRight = true;
	}
	else
	{
		spawnX = m_screenWidth;
		m_movingRight = false;
	}

	// Set initial position through physics
	auto physics = getComponent<PhysicsComponent>();
	if (physics)
	{
		physics->setPosition(Vector2D(spawnX, y));

		// Set initial velocity
		float velocityX = m_movingRight ? m_moveSpeed : -m_moveSpeed;
		physics->setVelocity(Vector2D(velocityX, 0.0f));

	}
}

void Loner::takeDamage(float amount)
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
			explosion->spawn(explosionPos);
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
