#include "MetalAsteroid.h"
#include "Engine2000/SpriteComponent.h"
#include "Engine2000/PhysicsComponent.h"
#include "Engine2000/ScreenBoundsComponent.h"
#include "Engine2000/Level.h"
#include "Engine2000/E2Log.h"

MetalAsteroid::MetalAsteroid(Size size)
	: m_size(size)
	, m_moveSpeed(0.3f)
	, m_animationTimer(0.0f)
	, m_frameDelay(0.1f)
{
	// Setup components
	m_sprite = addComponent<SpriteComponent>();
	m_bounds = addComponent<ScreenBoundsComponent>();
	m_physics = addComponent<PhysicsComponent>();

	// Set sprite based on size, with correct frame configuration
	const char* spriteFile{};
	int horizontalFrames, verticalFrames;
	int totalAnimationFrames = 0;

	switch (size) {
	case Size::LARGE:
		spriteFile = "graphics/MAster96.bmp";
		horizontalFrames = 5;
		verticalFrames = 5;
		totalAnimationFrames = 25;
		break;
	case Size::MEDIUM:
		spriteFile = "graphics/MAster64.bmp";
		horizontalFrames = 8;
		verticalFrames = 3;
		totalAnimationFrames = 24;
		break;
	case Size::SMALL:
		spriteFile = "graphics/MAster32.bmp";
		horizontalFrames = 8;
		verticalFrames = 2;
		totalAnimationFrames = 16;
		break;
	}

	m_sprite->setAnimatedTexture(spriteFile, horizontalFrames, verticalFrames);
	m_sprite->setAnimationMode(SpriteComponent::LOOP);
	m_sprite->setFrameDelay(m_frameDelay);

	// Set random initial frame 
	int totalFrames = horizontalFrames * verticalFrames;
	m_sprite->setCurrentFrame(rand() % totalFrames);
	m_sprite->setFrameRange(0, totalFrames - 1);

	//E2_LOG(Log, "Created metal asteroid with size %d", static_cast<int>(size));
}

void MetalAsteroid::init()
{
	m_physics->init(getLevel()->getPhysicsWorld(), false);
	m_physics->createCollisionShapeFromSprite();
	m_physics->createSensorShapeFromSprite(this);
	m_physics->setLayer("MetalAsteroid");
	
	GameObject::init();

	// Downward movement
	Vector2D velocity(0.0f, m_moveSpeed);
	m_physics->setVelocity(velocity);

	//E2_LOG(Log, "Initialized metal asteroid physics with speed %f", m_moveSpeed);
}

void MetalAsteroid::spawn(const Vector2D& position)
{
	if (auto physics = getComponent<PhysicsComponent>()) {
		physics->setPosition(position);
		//E2_LOG(Log, "Spawned metal asteroid at position (%f, %f)", position.x, position.y);
	}
}

void MetalAsteroid::setMoveSpeed(float speed)
{
	m_moveSpeed = speed;
	if (auto physics = getComponent<PhysicsComponent>()) {
		physics->setVelocity(Vector2D(0.0f, m_moveSpeed));
		//E2_LOG(Log, "Updated metal asteroid speed to %f", speed);
	}
}

void MetalAsteroid::onSensorBegin(GameObject* other)
{
	auto otherPhysics = other->getComponent<PhysicsComponent>();
	if (!otherPhysics) return;

	// Check if it's a player that can take damage
	if (auto damageable = dynamic_cast<IDamageable*>(other))
	{
		if (otherPhysics->getLayer() == "Player" && damageable->isAlive())
		{
			//E2_LOG(Warning, "Metal Asteroid applying damage to player");
			damageable->takeDamage(m_damage);
		}
	}
}