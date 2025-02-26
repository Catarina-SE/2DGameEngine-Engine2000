#include "HealthBar.h"
#include "Engine2000/Level.h"

HealthBar::HealthBar()
	: UIElement()
	, m_healthBar(nullptr)
{
}

void HealthBar::init()
{
	UIElement::init();

	m_healthBar = addComponent<HealthBarComponent>();
	m_healthBar->setDimensions(200, 20);
	
	// Position it in the UI layer (adjust these values as needed)
	setScreenPosition(0.05f, 0.05f);  // 5% from left, 5% from top
}

void HealthBar::setHealth(float current, float max)
{
	if (m_healthBar)
	{
		m_healthBar->setHealth(current, max);
	}
}

void HealthBar::setVisible(bool visible)
{
	if (m_healthBar)
	{
		m_healthBar->setVisible(visible);
	}
}
