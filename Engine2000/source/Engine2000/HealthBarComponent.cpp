#include "HealthBarComponent.h"

#include "GameObject.h"
#include "Level.h"
#include "Renderer.h"
#include "Vector4D.h"

#include <SDL2/SDL.h>
#include <glad/glad.h>
#include <gl/GL.h>

HealthBarComponent::HealthBarComponent(GameObject* owner)
	: Component(owner)
	, m_width(200.0f)
	, m_height(20.0f)
	, m_maxHealth(100.0f)
	, m_currentHealth(100.0f)
	, m_isVisible(true)
{
}

void HealthBarComponent::setDimensions(float width, float height)
{
	m_width = width;
	m_height = height;
}

void HealthBarComponent::setHealth(float current, float max)
{
	m_currentHealth = current;
	m_maxHealth = max;
}

void HealthBarComponent::setVisible(bool visible)
{
	m_isVisible = visible;
}

void HealthBarComponent::render()
{
	if (!m_isVisible) return;

	auto level = m_owner->getLevel();
	if (!level) return;

	// Get transform position
	auto pos = m_owner->getTransform()->getPosition();

	// Calculate health percentage
	float healthPercentage = m_currentHealth / m_maxHealth;

	// Create rectangles for border and fill
	Vector4D borderRect(
		pos.x,
		pos.y,
		m_width,
		m_height
	);

	Vector4D fillRect(
		pos.x + 2,
		pos.y + 2,
		(m_width - 4) * healthPercentage,
		m_height - 4
	);

	if (Renderer::Instance().isOpenGL()) {
		Vector4D borderColor(0, 0, 0, 255);    // Black
		Vector4D fillColor(0, 255, 0, 255);    // Green

		Renderer::Instance().drawRect(borderRect, borderColor);
		if (healthPercentage > 0) {
			Renderer::Instance().fillRect(fillRect, fillColor);
		}
	}
	else {
		// SDL2 path remains unchanged
		auto renderer = static_cast<SDL_Renderer*>(level->getRenderer());

		SDL_Rect sdlBorderRect = {
			static_cast<int>(borderRect.x),
			static_cast<int>(borderRect.y),
			static_cast<int>(borderRect.w),
			static_cast<int>(borderRect.h)
		};

		SDL_Rect sdlFillRect = {
			static_cast<int>(fillRect.x),
			static_cast<int>(fillRect.y),
			static_cast<int>(fillRect.w),
			static_cast<int>(fillRect.h)
		};

		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);		// Black
		SDL_RenderFillRect(renderer, &sdlBorderRect);

		SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);	// Green
		SDL_RenderFillRect(renderer, &sdlFillRect);
	}
}