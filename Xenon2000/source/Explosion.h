#pragma once

#include "Engine2000/GameObject.h"
#include "Engine2000/SpriteComponent.h"

class Explosion : public GameObject
{
private:
	SpriteComponent* m_sprite;
	float m_animationTime;
	float m_currentTime;

public:
	Explosion();
	virtual void update(float deltaTime) override;

	void spawn(const Vector2D& position, Vector2D scale);
	void spawn(const Vector2D& position, float scale);
	void spawn(const Vector2D& position);
};
