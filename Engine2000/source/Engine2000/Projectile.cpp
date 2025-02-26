#include "Projectile.h"
#include "Projectile.h"
#include "PhysicsComponent.h"
#include "Level.h"
#include "E2Log.h"
#include <iostream>

Projectile::Projectile()
    : m_speed(0.0f)
    , m_direction(0.0f, -1.0f) // Default direction is up
    , m_isActive(true)
{
	// Sprite component
    m_sprite = addComponent<SpriteComponent>();

	// Physics component
	m_physics = addComponent<PhysicsComponent>();

	// Screen bounds component
	m_boundsComponent = addComponent<ScreenBoundsComponent>();
}

void Projectile::init()
{
	if (m_physics->getLayer() == "Default")
	{
		m_physics->setLayer("Projectile");
	}

	// Set initial velocity based on direction and speed
	Vector2D velocity = m_direction;
	velocity.x *= m_speed;
	velocity.y *= m_speed;
	m_physics->setVelocity(velocity);
}

void Projectile::update(float deltaTime)
{
	if (!m_isActive) return;

	// Set physics velocity based on direction and speed
	Vector2D velocity;
	velocity.x = m_direction.x * m_speed;
	velocity.y = m_direction.y * m_speed;
	m_physics->setVelocity(velocity);

	GameObject::update(deltaTime);
}

void Projectile::onBoundsDestroy()
{
	if (m_isActive && m_level) {
        m_isActive = false;
        m_level->removeGameObject(this);
        E2_LOG(Log, "Projectile destroyed due to leaving bounds");
    }
}

void Projectile::setPosition(float x, float y)
{
	getTransform()->setPosition(x, y);
	m_physics->setPosition(Vector2D(x, y));
}

SpriteComponent* Projectile::setSprite(const char* filePath, int horizontalFrames, int verticalFrames)
{
    if (horizontalFrames == 1 && verticalFrames == 1)
	{
        m_sprite->setTexture(filePath);
    }
    else
	{
        m_sprite->setAnimatedTexture(filePath, horizontalFrames, verticalFrames);
    }
	return m_sprite;
}

void Projectile::setFrame(int frame)
{
    m_sprite->setCurrentFrame(frame);
}