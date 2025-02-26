#include "WeaponPowerUp.h"

#include "Player.h"
#include "Companion.h"

void WeaponPowerUp::init()
{
	auto sprite = getComponent<SpriteComponent>();
	sprite->setAnimatedTexture("graphics/PUWeapon.bmp", 4, 2);

	PowerUp::init();
}

void WeaponPowerUp::applyEffect(GameObject* target)
{
	E2_LOG(Warning, "WeaponPowerUp applyEffect called");

	if (auto player = dynamic_cast<Player*>(target)) {
		E2_LOG(Warning, "Target is Player, calling upgradeWeapon");
		player->upgradeWeapon();
	}
	else if (auto companion = dynamic_cast<Companion*>(target)) {
		E2_LOG(Warning, "Target is Companion, calling upgradeWeapon");
		companion->upgradeWeapon();
	}
	else {
		E2_LOG(Warning, "Target is neither Player nor Companion");
	}
}
