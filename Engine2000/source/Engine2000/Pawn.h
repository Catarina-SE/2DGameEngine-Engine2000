#pragma once

#include "Core.h"
#include "GameObject.h"
#include "SpriteComponent.h"
#include "Input.h"
#include "Level.h"

class ENGINE2000_API Pawn : public GameObject
{
protected:
	SpriteComponent* m_sprite;

public:
	Pawn();
	virtual ~Pawn() = default;

	const Input& getInput() { return m_level->getInput(); }

	const SpriteComponent* getSprite() { return m_sprite; }

	void setSprite(const char* filePath);

	void setAnimatedSprite(const char* filePath, int horizontalFrames, int verticalFrames);

	//virtual void update(float deltaTime) override;
};

