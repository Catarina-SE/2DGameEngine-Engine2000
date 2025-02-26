#pragma once

#include "Engine2000/UIElement.h"
#include "Engine2000/SpriteComponent.h"

class LifeIcon : public UIElement
{
public:
	LifeIcon();
	virtual void init() override;
	void setVisible(bool visible);

private:
	SpriteComponent* m_sprite;
};

