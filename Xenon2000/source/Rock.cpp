#include "Rock.h"
#include <cstdlib>
#include <cmath>

// Define the sprite frame data
const Rock::SpriteFrame Rock::NARROW_FRAMES[] = {
	{288, 1472, 64, 64},  // Fourth sprite
	{288, 1408, 64, 64},  // Fifth sprite
	{288, 1344, 64, 64}   // Sixth sprite
};
const int Rock::NARROW_FRAMES_COUNT = 3;

const Rock::SpriteFrame Rock::WIDE_FRAMES[] = {
	{96, 1344, 192, 64},  // First sprite
	{64, 1408, 224, 64},  // Second sprite
	{64, 1472, 224, 64}   // Third sprite
};
const int Rock::WIDE_FRAMES_COUNT = 3;

Rock::Rock(RockType type)
	: m_type(type)
	, m_isFlipped(false)
{
	m_sprite = addComponent<SpriteComponent>();
}

void Rock::init()
{
	// Set up the sprite with the full sprite sheet
	m_sprite->setAnimatedTexture("graphics/Blocks.bmp", 8, 32);
	m_sprite->setAnimationMode(SpriteComponent::STATIC);

// 	E2_LOG(Log, "Initializing Rock - Type: %s, Before sprite selection",
// 		m_type == RockType::NARROW ? "NARROW" : "WIDE");

	selectRandomSprite();

// 	E2_LOG(Log, "After sprite selection - Width: %d, Height: %d",
// 		m_sprite->getFrameWidth(), m_sprite->getFrameHeight());

	GameObject::init();
}

void Rock::selectRandomSprite()
{
	const SpriteFrame* frames;
	int frameCount;

	if (m_type == RockType::NARROW) {
		frames = NARROW_FRAMES;
		frameCount = NARROW_FRAMES_COUNT;
	}
	else {
		frames = WIDE_FRAMES;
		frameCount = WIDE_FRAMES_COUNT;
	}

	// Select random frame
	int randomIndex = rand() % frameCount;
	const SpriteFrame& frame = frames[randomIndex];

	// Add a small padding to prevent texture bleeding
	m_sprite->setCustomFrameRect(
		frame.x + 1,     // Add 1 pixel padding from left
		frame.y + 1,     // Add 1 pixel padding from top
		frame.width - 2,  // Subtract 2 pixels for left/right padding
		frame.height - 2  // Subtract 2 pixels for top/bottom padding
	);
}

void Rock::setPosition(float x, float y)
{
	// Round the position to nearest pixel to prevent fractional positioning
	float roundedX = std::round(x);
	float roundedY = std::round(y);
	
	getTransform()->setPosition(roundedX, roundedY);
}

void Rock::setFlipped(bool flipped)
{
	m_isFlipped = flipped;
	if (m_sprite) {
		m_sprite->setFlip(flipped ? SpriteComponent::RenderFlip::HORIZONTAL : SpriteComponent::RenderFlip::NONE);
	}
}

int Rock::getWidth() const
{
	return m_type == RockType::NARROW ? 64 : 224;  // Maximum width for each type
}

bool Rock::isFlipped() const
{
	return m_isFlipped;
}