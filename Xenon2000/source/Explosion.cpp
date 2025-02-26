#include "Explosion.h"
#include "Engine2000/Level.h"
#include "Engine2000/SpriteComponent.h"

Explosion::Explosion()
	: m_animationTime(0.6f)
	, m_currentTime(0.0f)
{
	m_sprite = addComponent<SpriteComponent>();
	m_sprite->setAnimatedTexture("graphics/explode64.bmp", 5, 2); // 10 frames explosion sheed
	m_sprite->setAnimationMode(SpriteComponent::CONTROLLED);
	m_sprite->setFrameDelay(m_animationTime / 10.0f); // Divide animation time by total frames
	m_sprite->setCurrentFrame(0);
}

void Explosion::update(float deltaTime)
{
	m_currentTime += deltaTime;

	// Calculate current frame based on time
	int frame = static_cast<int>((m_currentTime / m_animationTime) * 10);
	if (frame >= 10)
	{
		// Animation complete, remove explosion
		auto level = getLevel();
		if (level)
		{
			level->removeGameObject(this);
		}
		return;
	}

	m_sprite->setCurrentFrame(frame);
	GameObject::update(deltaTime);
}

void Explosion::spawn(const Vector2D& position, Vector2D scale)
{
	getTransform()->setPosition(position.x, position.y);
	getTransform()->setScale(scale.x, scale.y);
}

void Explosion::spawn(const Vector2D& position, float scale)
{
	spawn(position, Vector2D(scale, scale));
}

void Explosion::spawn(const Vector2D& position)
{
	spawn(position, getTransform()->getScale());
}

