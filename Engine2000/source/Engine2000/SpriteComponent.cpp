#include "SpriteComponent.h"
#include "GameObject.h"
#include "TransformComponent.h"
#include "Texture.h"

#include <SDL2/SDL.h>

SpriteComponent::SpriteComponent(GameObject* owner)
	: Component(owner)
	, m_texture(std::make_unique<Texture>())
	, m_textureHandle(nullptr)
	, m_frameTime(0)
	, m_frameDelay(0.1f)
	, m_isAnimated(false)
	, m_currentFrame(0)
	, m_totalFrames(1)
	, m_animMode(STATIC)
	, m_isPaused(false)
	, m_isVisible(true)
	, m_textureWidth(0)
	, m_textureHeight(0)
	, m_frameWidth(1)
	, m_frameHeight(1)
	, m_flip(RenderFlip::NONE)
	, m_startFrame(0)
	, m_endFrame(0)
	, m_hasFrameRange(false)
	, m_useCustomFrameRect(false)
{
}

SpriteComponent::~SpriteComponent()
{
	m_textureHandle = nullptr;
}

void SpriteComponent::setTexture(const char* filePath)
{
	m_textureHandle = m_texture->loadFromFile(filePath);

	// Query texture size
	m_textureWidth = m_texture->getWidth();
	m_textureHeight = m_texture->getHeight();

	// For static sprites, frame is the entire texture
	m_frameRect = Vector4D(0, 0, m_textureWidth, m_textureHeight);
	m_positionRect = Vector4D(0, 0, m_textureWidth, m_textureHeight);

	m_frameWidth = m_textureWidth;
	m_frameHeight = m_textureHeight;

	m_isAnimated = false;
	m_animMode = STATIC;
	m_totalFrames = 1;
}

void SpriteComponent::setAnimatedTexture(const char* filePath, int horizontalFrames, int verticalFrames)
{
	m_textureHandle = m_texture->loadFromFile(filePath);

	// Query texture size
	m_textureWidth = m_texture->getWidth();
	m_textureHeight = m_texture->getHeight();

	// Calculate frame size
	m_frameWidth = m_textureWidth / horizontalFrames;
	m_frameHeight = m_textureHeight / verticalFrames;
	m_totalFrames = horizontalFrames * verticalFrames;

	// Set initial frame
	m_frameRect = Vector4D(0, 0, m_frameWidth, m_frameHeight);
	m_positionRect = Vector4D(0, 0, m_frameWidth, m_frameHeight);

	m_isAnimated = true;
	m_animMode = LOOP;

	// Initialize frame range to full sprite sheet by default
	m_startFrame = 0;
	m_endFrame = m_totalFrames - 1;
	m_hasFrameRange = false;
	m_currentFrame = 0;
}

void SpriteComponent::setAnimationMode(AnimationMode mode)
{
	m_animMode = mode;
}

void SpriteComponent::setCurrentFrame(int frame)
{
	if (!m_isAnimated) return;

	m_currentFrame = frame % m_totalFrames;
	int framesPerRow = (m_frameWidth > 0) ? m_textureWidth / m_frameWidth : 1;

	m_frameRect.x = (m_currentFrame % framesPerRow) * m_frameWidth;
	m_frameRect.y = (m_currentFrame / framesPerRow) * m_frameHeight;
}

void SpriteComponent::setFrameRange(int startFrame, int endFrame)
{
	m_startFrame = startFrame;
	m_endFrame = endFrame;
	m_hasFrameRange = true;
	m_currentFrame = startFrame;
}

void SpriteComponent::clearFrameRange()
{
	m_hasFrameRange = false;
	m_startFrame = 0;
	m_endFrame = m_totalFrames - 1;
}

void SpriteComponent::update(float deltaTime)
{
	// Update position based on transform
	const Vector2D& pos = m_owner->getTransform()->getPosition();
	m_positionRect.x = pos.x;
	m_positionRect.y = pos.y;

	// Handle animation if needed
	if (!m_isAnimated || m_isPaused || m_animMode == STATIC) return;

	if (m_animMode == LOOP) {
		m_frameTime += deltaTime;
		if (m_frameTime >= m_frameDelay) {
			m_frameTime = 0;
			m_currentFrame++;

			// Handle frame range
			int lastFrame = m_hasFrameRange ? m_endFrame : (m_totalFrames - 1);
			int firstFrame = m_hasFrameRange ? m_startFrame : 0;

			if (m_currentFrame > lastFrame) {
				m_currentFrame = firstFrame;
			}

			// Update frame rectangle
			int framesPerRow = m_textureWidth / m_frameWidth;
			m_frameRect.x = (m_currentFrame % framesPerRow) * m_frameWidth;
			m_frameRect.y = (m_currentFrame / framesPerRow) * m_frameHeight;
		}
	}
}

void SpriteComponent::render()
{
	if (!m_texture || !m_isVisible) return;

	// Apply transform scale
	const Vector2D& scale = m_owner->getTransform()->getScale();
	m_positionRect.w = m_frameRect.w * scale.x;
	m_positionRect.h = m_frameRect.h * scale.y;

	// Convert RenderFlip to SDL_RendererFlip
	SDL_RendererFlip sdlFlip = SDL_FLIP_NONE;
	switch (m_flip) {
	case RenderFlip::HORIZONTAL:
		sdlFlip = SDL_FLIP_HORIZONTAL;
		break;
	case RenderFlip::VERTICAL:
		sdlFlip = SDL_FLIP_VERTICAL;
		break;
	default:
		sdlFlip = SDL_FLIP_NONE;
	}

	// Draw using the Texture class
	m_texture->draw(m_frameRect, m_positionRect, sdlFlip);
}

void SpriteComponent::setCustomFrameRect(int x, int y, int width, int height)
{
	m_customFrameRect = Vector4D(x, y, width, height);
	m_useCustomFrameRect = true;
	m_frameWidth = width;
	m_frameHeight = height;

	m_positionRect.w = width;
	m_positionRect.h = height;

	m_frameRect.x = x;
	m_frameRect.y = y;
	m_frameRect.w = width;
	m_frameRect.h = height;
}
