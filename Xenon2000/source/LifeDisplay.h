#pragma once

#include "Engine2000/UIElement.h"
#include "Engine2000/SpriteComponent.h"
#include "LifeIcon.h"
#include <vector>

class LifeDisplay : public UIElement {
private:
	static const int MAX_LIVES = 3;
	static const float ICON_SPACING;
	static const float DEFAULT_UI_SCALE;
	std::vector<LifeIcon*> m_lifeIcons;

public:
	LifeDisplay();
	virtual void init() override;
	virtual void update(float deltaTime) override;

	void setLifeCount(int count);
};
