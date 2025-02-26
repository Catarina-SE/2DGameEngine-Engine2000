#include "ExplosionProjectile.h"

ExplosionProjectile::ExplosionProjectile()
{
	auto sprite = getComponent<SpriteComponent>();
	sprite->setAnimatedTexture("graphics/explode16.bmp", 5, 2);
}
