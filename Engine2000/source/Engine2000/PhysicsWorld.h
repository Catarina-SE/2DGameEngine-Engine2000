#pragma once

#include "Core.h"
#include "Vector2D.h"
#include <box2d/box2d.h>
#include "SDL2/SDL_pixels.h"

class ENGINE2000_API PhysicsWorld 
{
private:
    static const float VELOCITY_SCALE;
    b2WorldId m_worldId;
    float m_timeStep;
    int m_subSteps;
    float m_worldWidth;
    float m_worldHeight;
    Vector2D m_gravity;
    bool m_debugDraw;

    static bool filterCallback(b2ShapeId shapeIdA, b2ShapeId shapeIdB, void* context);

public:
    PhysicsWorld();
    ~PhysicsWorld();

    void init(float worldWidth, float worldHeight, const Vector2D& gravity);
    void cleanup();
    void update();

    // Body and shape creation
	b2BodyId createBody(const Vector2D& position, bool isDynamic, bool isBullet = false);
	b2ShapeId createBoxShape(b2BodyId bodyId, float width, float height, float density = 1.0f);
    void destroyBody(b2BodyId bodyId);

    // Body manipulation
    Vector2D getBodyPosition(b2BodyId bodyId);
    Vector2D getBodyVelocity(b2BodyId bodyId);
    void setBodyPosition(b2BodyId bodyId, const Vector2D& position);
    void setBodyVelocity(b2BodyId bodyId, const Vector2D& velocity);
    
    // Gravity control
    void setGravity(const Vector2D& gravity);
    const Vector2D& getGravity() const { return m_gravity; }

    float getTimeStep() const { return m_timeStep; }
    
    b2WorldId getWorldId() { return m_worldId; }

    static float getVelocityScale() { return VELOCITY_SCALE; }

private:
    void processSensors();
};