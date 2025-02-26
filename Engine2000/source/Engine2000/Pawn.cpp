#include "Pawn.h"
#include "Level.h"

Pawn::Pawn()
{
	m_sprite = addComponent<SpriteComponent>();
}

void Pawn::setSprite(const char* filePath)
{
	m_sprite->setTexture(filePath);
}

void Pawn::setAnimatedSprite(const char* filePath, int horizontalFrames, int verticalFrames)
{
	m_sprite->setAnimatedTexture(filePath, horizontalFrames, verticalFrames);
}
