#pragma once

#include "Engine2000/UIElement.h"
#include "Engine2000/HealthBarComponent.h"

class HealthBar : public UIElement {
private:
	HealthBarComponent* m_healthBar;

public:
	HealthBar();

	virtual void init() override;

	void setHealth(float current, float max);

	void setVisible(bool visible);
};