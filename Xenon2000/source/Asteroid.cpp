#include "Asteroid.h"
#include "Engine2000/SpriteComponent.h"
#include "Engine2000/E2Log.h"

#include "XenonLevel.h"
#include "TextDisplay.h"

Asteroid::Asteroid(Size size)
	: m_size(size)
	, m_rotationSpeed((rand() % 100) / 100.0f - 0.5f)
	, m_moveSpeed(0.1f)
	, m_damage(25.0f)
	, m_currentSize(size)
	, m_baseScore(5000)
	, m_scoreValue(0)
{
	// Health based on size
	switch (size) {
	case Size::LARGE:   m_maxHealth = 100.0f; break;
	case Size::MEDIUM:  m_maxHealth = 50.0f; break;
	case Size::SMALL:   m_maxHealth = 25.0f; break;
	}
	m_health = m_maxHealth;
	m_isAlive = true;

	// Setup components
	m_sprite = addComponent<SpriteComponent>();
	m_bounds = addComponent<ScreenBoundsComponent>();
	m_physics = addComponent<PhysicsComponent>();

	// Set sprite based on size, with correct frame configuration
	const char* spriteFile{};
	int horizontalFrames = 1, verticalFrames = 1;

	switch (m_currentSize) {
	case Size::LARGE:
		spriteFile = "graphics/SAster96.bmp";
		horizontalFrames = 5;
		verticalFrames = 5;
		m_moveSpeed = 0.1f;
		m_scoreValue = m_baseScore * 2;
		break;
	case Size::MEDIUM:
		spriteFile = "graphics/SAster64.bmp";
		horizontalFrames = 8;
		verticalFrames = 3;
		m_moveSpeed = 0.25f;
		m_scoreValue = m_baseScore * 1.5f;
		break;
	case Size::SMALL:
		spriteFile = "graphics/SAster32.bmp";
		horizontalFrames = 8;
		verticalFrames = 2;
		m_moveSpeed = 0.5f;
		m_scoreValue = m_baseScore;
		break;
	}

	m_sprite->setAnimatedTexture(spriteFile, horizontalFrames, verticalFrames);
	m_sprite->setAnimationMode(SpriteComponent::LOOP);
	m_sprite->setFrameDelay(0.1f);

	// Set random initial frame based on sprite sheet configuration
	int totalFrames = horizontalFrames * verticalFrames;
	m_sprite->setCurrentFrame(rand() % totalFrames);
}

void Asteroid::init()
{
	GameObject::init();

	m_physics->init(getLevel()->getPhysicsWorld(), true);
	m_physics->createCollisionShapeFromSprite();
	m_physics->createSensorShapeFromSprite(this);
	m_physics->setLayer("Enemy");

	// Set downward movement
	Vector2D velocity(0.0f, m_moveSpeed);
	m_physics->setVelocity(velocity);
}

void Asteroid::update(float deltaTime)
{
	if (!m_isAlive) return;

	// Update animation frame
	GameObject::update(deltaTime);
}

void Asteroid::onSensorBegin(GameObject* other)
{
	if (!m_isAlive) return;

	auto otherPhysics = other->getComponent<PhysicsComponent>();
	if (!otherPhysics || otherPhysics->isImmune()) return;

	// Check if the other object can take damage
	auto damageable = dynamic_cast<IDamageable*>(other);
	if (damageable && otherPhysics->getLayer() != "Enemy" && otherPhysics->getLayer() != "EnemyProjectile")
	{
		E2_LOG(Warning, "Asteroid applying damage");
		damageable->takeDamage(m_damage);
	}
}

void Asteroid::takeDamage(float amount)
{
	if (!m_isAlive) return;

	m_health = std::max(0.0f, m_health - amount);
	if (m_health <= 0)
	{
		m_isAlive = false;

		// Spawn smaller asteroids if not already at smallest size
		if (m_size != Size::SMALL)
		{
			spawnSmallerAsteroids();
		}

		// Handle removal
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

void Asteroid::spawnSmallerAsteroids()
{
	// Reduce size of the asteroids
	if (m_currentSize < Size::SMALL)
	{
		m_currentSize = static_cast<Size>(static_cast<int>(m_currentSize) + 1);
	}

	// Get the exact center position of the current asteroid
	Vector2D originalPos = getTransform()->getPosition();
	float currentWidth = m_sprite->getFrameWidth();
	float currentHeight = m_sprite->getFrameHeight();

	// Adjust position to be center of the current asteroid
	originalPos.x += currentWidth / 2;
	originalPos.y += currentHeight / 2;

	// Use consistent spread
	float spread = 40.0f;
	// Define consistent horizontal speeds for child asteroids
	float horizontalSpeed = 0.1f;  // Reduced from 0.1f

	if (auto level = getLevel())
	{
		// Create asteroids
		auto leftAsteroid = level->createGameObject<Asteroid>(m_currentSize);
		auto centerAsteroid = level->createGameObject<Asteroid>(m_currentSize);
		auto rightAsteroid = level->createGameObject<Asteroid>(m_currentSize);

		// Get dimensions of new asteroids
		float newWidth = leftAsteroid->getComponent<SpriteComponent>()->getFrameWidth();
		float newHeight = leftAsteroid->getComponent<SpriteComponent>()->getFrameHeight();

		// Adjust spawn positions to account for the new asteroid sizes
		Vector2D leftPos(originalPos.x - spread - newWidth / 2, originalPos.y - newHeight / 2);
		Vector2D centerPos(originalPos.x - newWidth / 2, originalPos.y - newHeight / 2);
		Vector2D rightPos(originalPos.x + spread - newWidth / 2, originalPos.y - newHeight / 2);

		// Set positions and velocities, ensuring consistent vertical speed
		if (auto physics = leftAsteroid->getComponent<PhysicsComponent>())
		{
			physics->setPosition(leftPos);
			// Use the asteroid's defined move speed instead of inheriting
			physics->setVelocity(Vector2D(-horizontalSpeed, leftAsteroid->m_moveSpeed));
		}

		if (auto physics = centerAsteroid->getComponent<PhysicsComponent>())
		{
			physics->setPosition(centerPos);
			physics->setVelocity(Vector2D(0.0f, centerAsteroid->m_moveSpeed));
		}

		if (auto physics = rightAsteroid->getComponent<PhysicsComponent>())
		{
			physics->setPosition(rightPos);
			physics->setVelocity(Vector2D(horizontalSpeed, rightAsteroid->m_moveSpeed));
		}
	}
}