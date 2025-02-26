#include "TextDisplay.h"
#include "Engine2000/Level.h"

TextDisplay::TextDisplay(bool useLargeFont, bool isTemporary, float displayTime)
	: m_isLargeFont(useLargeFont)
	, m_isTemporary(isTemporary)
	, m_displayTime(displayTime)
	, m_currentTime(0.0f)
{
}

void TextDisplay::init()
{
	UIElement::init();
	m_currentTime = 0.0f;
}

void TextDisplay::update(float deltaTime)
{
	if (m_isTemporary) {
		m_currentTime += deltaTime;
		if (m_currentTime >= m_displayTime) {
			if (auto level = getLevel()) {
				clearCharacters();
				level->removeGameObject(this);
			}
			return;
		}
	}

	UIElement::update(deltaTime);
}

void TextDisplay::setText(const std::string& text)
{
	if (m_text == text) return;

	m_text = text;
	createCharacters();
}

void TextDisplay::clearCharacters()
{
	for (auto& charSprite : m_characters) {
		if (charSprite.gameObject) {
			getLevel()->removeGameObject(charSprite.gameObject);
		}
	}
	m_characters.clear();
}

void TextDisplay::createCharacters()
{
	clearCharacters();

	if (!getLevel()) return;

	float fontSize = m_isLargeFont ? LARGE_FONT_SIZE : SMALL_FONT_SIZE;
	float spacing = fontSize + 1; // Add 1 pixel spacing between characters
	float xOffset = 0;

	// Get base position
	auto basePos = getTransform()->getPosition();

	for (char c : m_text) {
		// Create game object for this character
		auto charObj = getLevel()->createGameObject<GameObject>(Level::UI);

		// Add and setup sprite component
		auto sprite = charObj->addComponent<SpriteComponent>();
		if (m_isLargeFont) {
			sprite->setAnimatedTexture("graphics/font16x16.bmp", 8, 12);
		}
		else {
			sprite->setAnimatedTexture("graphics/font8x8.bmp", 8, 16);
		}
		sprite->setAnimationMode(SpriteComponent::CONTROLLED);
		sprite->setCurrentFrame(getCharacterFrame(c));

		// Position the character
		charObj->getTransform()->setPosition(basePos.x + xOffset, basePos.y);

		// Store character info
		CharacterSprite charSprite;
		charSprite.gameObject = charObj;
		charSprite.sprite = sprite;
		charSprite.xOffset = xOffset;
		m_characters.push_back(charSprite);

		xOffset += spacing;
	}
}

int TextDisplay::getCharacterFrame(char c) const
{
	if (c >= '0' && c <= '9') {
		return 16 + (c - '0');      // Numbers start at frame 16
	}
	else if (c >= 'A' && c <= 'Z') {
		return 33 + (c - 'A');      // Capitals start at frame 33
	}
	else if (c >= 'a' && c <= 'z') {
		return 65 + (c - 'a');      // Lowercase start at frame 65
	}
	return 0; // Default frame for unknown characters
}