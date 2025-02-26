#pragma once

#include "Core.h"
#include "Component.h"
#include "GameObject.h"

class ENGINE2000_API HealthBarComponent : public Component {
private:
	float m_width;
	float m_height;
	float m_maxHealth;
	float m_currentHealth;
	bool m_isVisible;

public:
	HealthBarComponent(GameObject* owner);

	void setDimensions(float width, float height);
	void setHealth(float current, float max);
	void setVisible(bool visible);

	virtual void render() override;
};