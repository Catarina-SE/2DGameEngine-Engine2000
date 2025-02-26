#pragma once

#include "PowerUp.h"

class WeaponPowerUp : public PowerUp
{
public:
	void init() override;

private:
	virtual void applyEffect(GameObject* target) override;

};

