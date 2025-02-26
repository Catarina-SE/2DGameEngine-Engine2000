#include "CompanionPowerUp.h"

#include "Player.h"
#include "Companion.h"
#include "Engine2000/E2Log.h"

void CompanionPowerUp::init()
{
	auto sprite = getComponent<SpriteComponent>();
	sprite->setAnimatedTexture("graphics/clone.bmp", 4, 5);
	sprite->setFrameRange(0, 15);

	PowerUp::init();
}

void CompanionPowerUp::applyEffect(GameObject* target)
{
	auto player = dynamic_cast<Player*>(target);
	player->addCompanion();
}