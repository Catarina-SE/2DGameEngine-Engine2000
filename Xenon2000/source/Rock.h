#pragma once
#include "Engine2000/GameObject.h"
#include "Engine2000/SpriteComponent.h"

class Rock : public GameObject {
public:
	enum class RockType {
		NARROW,  // 64x64 rocks
		WIDE    // Wider formations (192x64 and 224x64)
	};

private:
	struct SpriteFrame {
		int x, y;           // Top-left position in sprite sheet
		int width, height;  // Dimensions of the sprite
	};

	SpriteComponent* m_sprite;
	RockType m_type;
	bool m_isFlipped;

	// Sprite frame data
	static const SpriteFrame NARROW_FRAMES[];
	static const int NARROW_FRAMES_COUNT;
	static const SpriteFrame WIDE_FRAMES[];
	static const int WIDE_FRAMES_COUNT;

public:
	Rock(RockType type = RockType::NARROW);
	virtual void init() override;

	void setPosition(float x, float y);
	void setFlipped(bool flipped);

	int getWidth() const;
	bool isFlipped() const;
private:
	void selectRandomSprite();
};