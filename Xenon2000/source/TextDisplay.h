#pragma once

#include "Engine2000/UIElement.h"
#include "Engine2000/SpriteComponent.h"

#include <vector>
#include <string>

class TextDisplay : public UIElement {
private:
	struct CharacterSprite {
		GameObject* gameObject;
		SpriteComponent* sprite;
		float xOffset;
	};

	std::vector<CharacterSprite> m_characters;
	std::string m_text;
	bool m_isLargeFont;
	bool m_isTemporary;
	float m_displayTime;
	float m_currentTime;

	// Font constants
	static const int SMALL_FONT_SIZE = 8;
	static const int LARGE_FONT_SIZE = 16;

public:
	TextDisplay(bool useLargeFont = false, bool isTemporary = false, float displayTime = 2.0f);
	virtual void init() override;
	virtual void update(float deltaTime) override;

	void setText(const std::string& text);
	const std::string& getText() const { return m_text; }

private:
	void createCharacters();
	void clearCharacters();
	int getCharacterFrame(char c) const;
};