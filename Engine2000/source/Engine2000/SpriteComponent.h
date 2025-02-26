#pragma once

#include "Core.h"
#include "Component.h"
#include "Vector4D.h"
#include <memory>

struct SDL_Rect;
struct SDL_Texture;

class Texture;

class ENGINE2000_API SpriteComponent : public Component {
public:
	enum AnimationMode {
		STATIC,
		LOOP,
		CONTROLLED
	};

	enum class RenderFlip {
		NONE,
		HORIZONTAL,
		VERTICAL
	};

private:
	std::unique_ptr<Texture> m_texture;
	Vector4D m_frameRect;       // Current frame rectangle
	Vector4D m_positionRect;    // Where to render on screen
	void* m_textureHandle;      // SDL_Texture* or GLuint
	RenderFlip m_flip;

	bool m_isVisible;
	bool m_isAnimated;
	int m_currentFrame;
	int m_totalFrames;
	float m_frameTime;
	float m_frameDelay;
	AnimationMode m_animMode;
	bool m_isPaused;
	int m_textureWidth;
	int m_textureHeight;
	int m_frameWidth;
	int m_frameHeight;

	// Frame range control
	int m_startFrame;
	int m_endFrame;
	bool m_hasFrameRange;

	Vector4D m_customFrameRect;
	bool m_useCustomFrameRect;

public:
	SpriteComponent(GameObject* owner);
	~SpriteComponent();

	// Visibility
	void setVisible(bool visible) { m_isVisible = visible; }
	bool isVisible() const { return m_isVisible; }

	// Setters
	void setTexture(const char* filePath);
	void setFlip(RenderFlip flip) { m_flip = flip; }
	void setAnimatedTexture(const char* filePath, int horizontalFrames, int verticalFrames);
	void setAnimationMode(AnimationMode mode);
	void setCustomFrameRect(int x, int y, int width, int height);

	// Frame control
	void setFrameDelay(float delay) { m_frameDelay = delay; }
	void setCurrentFrame(int frame);
	void setFrameRange(int startFrame, int endFrame);
	void clearFrameRange();

	void pause() { m_isPaused = true; }
	void resume() { m_isPaused = false; }

	// Getters
	int getCurrentFrame() const { return m_currentFrame; }
	int getTotalFrames() const { return m_totalFrames; }
	int getFrameWidth() const { return m_frameWidth; }
	int getFrameHeight() const { return m_frameHeight; }
	int getStartFrame() const { return m_startFrame; }
	int getEndFrame() const { return m_endFrame; }

	virtual void update(float deltaTime) override;
	virtual void render() override;
};