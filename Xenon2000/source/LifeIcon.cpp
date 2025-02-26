#include "LifeIcon.h"

LifeIcon::LifeIcon()
	: m_sprite(nullptr)
{
}

void LifeIcon::init()
{
	GameObject::init();

	m_sprite = addComponent<SpriteComponent>();
	m_sprite->setTexture("graphics/PULife.bmp");
	m_sprite->setCurrentFrame(0);
}

void LifeIcon::setVisible(bool visible)
{
	if (m_sprite)
	{
		m_sprite->setVisible(visible);
	}
}
