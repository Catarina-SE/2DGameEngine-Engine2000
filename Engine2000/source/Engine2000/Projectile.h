#pragma once
#include "GameObject.h"
#include "SpriteComponent.h"
#include "PhysicsComponent.h"
#include "ScreenBoundsComponent.h"
#include "IBoundsResponder.h"

class ENGINE2000_API Projectile : public GameObject, public PhysicsSensorListener, public IBoundsResponder
{
private:
    SpriteComponent* m_sprite;
    PhysicsComponent* m_physics;
    ScreenBoundsComponent* m_boundsComponent;

protected:
	Vector2D m_direction;
	float m_speed;
	bool m_isActive;
	SpriteComponent* getSprite() { return m_sprite; }
	PhysicsComponent* getPhysics() { return m_physics; }

public:
    Projectile();
    virtual ~Projectile() = default;

    virtual void init() override;
	virtual void update(float deltaTime) override;

	virtual void onBoundsDestroy() override;

    // Sensor methods
    virtual void onSensorBegin(GameObject* other) override {};
    virtual void onSensorEnd(GameObject* other) override {};

    void setPosition(float x, float y);
    void setDirection(const Vector2D& direction) { m_direction = direction; }
    void setSpeed(float speed) { m_speed = speed; }

    SpriteComponent* setSprite(const char* filePath, int horizontalFrames = 1, int verticalFrames = 1);
    void setFrame(int frame);

    bool isActive() const { return m_isActive; }
    void deactivate() { m_isActive = false; }
};

