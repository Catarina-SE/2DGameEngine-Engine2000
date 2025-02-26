#include "UIElement.h"

#include "Level.h"

UIElement::UIElement()
	: m_screenPercentX(0.0f)
	, m_screenPercentY(0.0f)
{
}

void UIElement::init()
{
	GameObject::init();
}

void UIElement::update(float deltaTime)
{
	GameObject::update(deltaTime);
}

void UIElement::setScreenPosition(float percentX, float percentY)
{
	m_screenPercentX = percentX;
	m_screenPercentY = percentY;

	if (auto level = getLevel())
	{
		float screenWidth = level->getScreenWidth();
		float screenHeight = level->getScreenHeight();

		float x = screenWidth * percentX;
		float y = screenHeight * percentY;

		getTransform()->setPosition(x, y);
	}
}

void UIElement::setUIScale(float scale)
{
	m_uiScale = scale;
	if (auto transform = getComponent<TransformComponent>())
	{
		transform->setScale(scale);
	}
}
