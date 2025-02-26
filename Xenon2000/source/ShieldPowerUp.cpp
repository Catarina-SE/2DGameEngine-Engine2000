#include "ShieldPowerUp.h"

#include "Player.h"
#include "Companion.h"
#include "Engine2000/E2Log.h"

void ShieldPowerUp::init()
{
	auto sprite = getComponent<SpriteComponent>();
	sprite->setAnimatedTexture("graphics/PUShield.bmp", 4, 2);

	PowerUp::init();
}

void ShieldPowerUp::applyEffect(GameObject* target)
{
	E2_LOG(Warning, "ShieldPowerUp applyEffect called");

	if (auto damageable = dynamic_cast<IDamageable*>(target)) {
		E2_LOG(Warning, "Target is damageable, applying health restore");
		damageable->gainHealth(50.0f);
	}
	else {
		E2_LOG(Warning, "Target is not damageable");
	}
}
