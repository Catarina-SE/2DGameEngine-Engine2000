#include "ScreenBoundsComponent.h"
#include "GameObject.h"
#include "Level.h"
#include "SpriteComponent.h"
#include "PhysicsComponent.h"
#include "E2Log.h"

ScreenBoundsComponent::ScreenBoundsComponent(GameObject* owner)
	: Component(owner)
	, m_behavior(BoundaryBehavior::DESTROY)
	, m_margin(0.0f)
	, m_isOutOfBounds(false)
	, m_isSleeping(false)
	, m_responder(nullptr)
{
}

void ScreenBoundsComponent::init()
{
	// Check if owner implements IBoundsResponder
	m_responder = dynamic_cast<IBoundsResponder*>(m_owner);
// 	if (m_responder) {
// 		E2_LOG(Warning, "GameObject implements IBoundsResponder interface");
// 	}
}

void ScreenBoundsComponent::update(float deltaTime)
{
	// Skip if sleeping
	if (m_isSleeping) {
		// Check if object is back in bounds
		if (!checkBounds()) {
			wakeUp();
		}
		return;
	}

	// Check if object is out of bounds
	if (checkBounds()) {
		handleOutOfBounds();
	}
}

bool ScreenBoundsComponent::checkBounds()
{
	if (!m_owner || !m_owner->getLevel()) return false;

	auto transform = m_owner->getTransform();
	auto sprite = m_owner->getComponent<SpriteComponent>();
	if (!transform || !sprite) return false;

	auto pos = transform->getPosition();
	float width = sprite->getFrameWidth();
	float height = sprite->getFrameHeight();
	float screenWidth = m_owner->getLevel()->getScreenWidth();
	float screenHeight = m_owner->getLevel()->getScreenHeight();

	// Check each enabled boundary
	bool outOfBounds = false;

	if (m_flags.checkTop) {
		outOfBounds |= (pos.y + height < -m_margin);
	}
	if (m_flags.checkBottom) {
		outOfBounds |= (pos.y > screenHeight + m_margin);
	}
	if (m_flags.checkLeft) {
		outOfBounds |= (pos.x + width < -m_margin);
	}
	if (m_flags.checkRight) {
		outOfBounds |= (pos.x > screenWidth + m_margin);
	}

	m_isOutOfBounds = outOfBounds;
	return outOfBounds;
}

void ScreenBoundsComponent::handleOutOfBounds()
{
	//E2_LOG(Log, "Handle out of bounds called");
	switch (m_behavior)
	{
	case BoundaryBehavior::DESTROY:
		if (m_responder)
		{
			//E2_LOG(Log, "Responder onBoundsDestroy being called");
			m_responder->onBoundsDestroy();
		}
		if (m_owner && m_owner->getLevel()) {
			//E2_LOG(Log, "GameObject destroyed due to being out of bounds");
			m_owner->getLevel()->removeGameObject(m_owner);
		}
		break;

	case BoundaryBehavior::SLEEP:
		if (!m_isSleeping) {
			m_isSleeping = true;
			// Disable physics if present
			if (auto physics = m_owner->getComponent<PhysicsComponent>()) {
				physics->setVelocity(Vector2D(0.0f, 0.0f));
			}
			if (m_responder) {
				m_responder->onBoundsSleep();
			}
			//E2_LOG(Log, "GameObject put to sleep due to being out of bounds");
		}
		break;

	case BoundaryBehavior::IGNORE:
		// Do nothing
		break;
	}
}

void ScreenBoundsComponent::wakeUp()
{
	if (m_isSleeping) {
		m_isSleeping = false;
		m_isOutOfBounds = false;
		if (m_responder) {
			m_responder->onBoundsWakeup();
		}
		//E2_LOG(Log, "GameObject woken up after returning to bounds");
	}
}