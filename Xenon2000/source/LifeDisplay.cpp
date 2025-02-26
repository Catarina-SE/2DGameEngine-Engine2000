#include "LifeDisplay.h"

#include "Engine2000/Level.h"

const float LifeDisplay::ICON_SPACING = 40.0f;
const float LifeDisplay::DEFAULT_UI_SCALE = 1.0f;

LifeDisplay::LifeDisplay()
	: UIElement()
{
	m_lifeIcons.reserve(MAX_LIVES);
}

void LifeDisplay::init()
{
	UIElement::init();

	getTransform()->setScale(DEFAULT_UI_SCALE);

	// Create life icons as separate GameObjects
	if (auto level = getLevel())
	{
		for (int i = 0; i < MAX_LIVES; ++i)
		{
			auto icon = level->createGameObject<LifeIcon>();
			icon->getTransform()->setScale(DEFAULT_UI_SCALE);
			m_lifeIcons.push_back(icon);
		}
	}

	setScreenPosition(0.05f, 0.87f);
}

void LifeDisplay::update(float deltaTime)
{
	UIElement::update(deltaTime);

	// Update icon positions relative to display position
	auto displayPos = getTransform()->getPosition();
	for (int i = 0; i < m_lifeIcons.size(); ++i)
	{
		if (auto icon = m_lifeIcons[i])
		{
			float xOffset = i * (ICON_SPACING * DEFAULT_UI_SCALE);
			icon->getTransform()->setPosition(
				displayPos.x + xOffset,
				displayPos.y
			);
		}
	}
}

void LifeDisplay::setLifeCount(int count)
{
	count = std::min(std::max(count, 0), MAX_LIVES);
	for (int i = 0; i < m_lifeIcons.size(); ++i)
	{
		if (auto icon = m_lifeIcons[i])
		{
			icon->setVisible(i < count);
		}
	}
}
