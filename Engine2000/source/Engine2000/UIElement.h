#pragma once

#include "Core.h"
#include "GameObject.h"

class ENGINE2000_API UIElement : public GameObject
{
protected:
	float m_screenPercentX;
	float m_screenPercentY;
	float m_uiScale;

public:
	UIElement();
	virtual void init() override;
	virtual void update(float deltaTime) override;


	void setScreenPosition(float percentX, float percentY);
	void setUIScale(float scale);
};