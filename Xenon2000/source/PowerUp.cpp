#include "PowerUp.h"

#include "Player.h"

PowerUp::PowerUp()
	:m_timeInLevel(30.0f)
	, m_moveSpeed(0.3f)
{
	m_sprite = addComponent<SpriteComponent>();

	m_physics = addComponent<PhysicsComponent>();
}

void PowerUp::update(float deltaTime)
{
	m_timeInLevel -= deltaTime;
	if (m_timeInLevel <= 0)
	{
		getLevel()->removeGameObject(this);
	}

	if (m_physics)
	{
		auto currentPos = getTransform()->getPosition();
		float newY = currentPos.y + (m_moveSpeed * deltaTime * 60.0f);
		m_physics->setPosition(Vector2D(currentPos.x, newY));
	}

	GameObject::update(deltaTime);
}

void PowerUp::init()
{
	m_physics->init(getLevel()->getPhysicsWorld(), true);

	m_physics->createSensorShapeFromSprite(this);
	m_physics->setLayer("PowerUp");
	m_physics->setDebugDraw(true);	// Debug Draw
	m_physics->setDebugColor(PhysicsComponent::DebugColor::Yellow);

	m_physics->setVelocity(Vector2D(0.0f, m_moveSpeed));

	if (m_boundsComponent)
	{
		ScreenBoundsComponent::BoundaryFlags flags;
		flags.checkTop = false;     // Don't check top boundary
		flags.checkLeft = false;    // Don't check left boundary
		flags.checkRight = false;   // Don't check right boundary
		flags.checkBottom = true;   // Only check bottom boundary
		m_boundsComponent->setBoundaryFlags(flags);
		m_boundsComponent->setBehavior(ScreenBoundsComponent::BoundaryBehavior::DESTROY);
	}


	GameObject::init();
}

void PowerUp::spawn(const Vector2D& position)
{
	getTransform()->setPosition(position);
}

void PowerUp::onSensorBegin(GameObject* other)
{
	auto otherPhysics = other->getComponent<PhysicsComponent>();
	if (!otherPhysics || otherPhysics->isImmune()) {
		return;
	}

	if (otherPhysics->getLayer() == "Player")
	{
		auto player = dynamic_cast<Player*>(other);
		auto companion = dynamic_cast<Companion*>(other);

		if (player) {
			applyEffect(other);
		}
		else if (companion) {
			applyEffect(other);
		}
		getLevel()->removeGameObject(this);
	}
}
