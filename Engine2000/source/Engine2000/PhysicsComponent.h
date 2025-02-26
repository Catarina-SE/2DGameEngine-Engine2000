#pragma once

#include "Core.h"
#include "Component.h"
#include "Vector2D.h"
#include <string>

struct b2BodyId;
struct b2ShapeId;
struct SDL_Color;

class PhysicsWorld;
class GameObject;

// Define the listener interface outside the component
class ENGINE2000_API PhysicsSensorListener
{
public:
	virtual ~PhysicsSensorListener() = default;
	virtual void onSensorBegin(GameObject* other) {}
	virtual void onSensorEnd(GameObject* other) {}
};

class ENGINE2000_API PhysicsComponent : public Component
{
private:
	class PhysicsComponentImpl;
	PhysicsComponentImpl* pimpl;

public:
	// Color abstraction for debug visualization
	enum class DebugColor {
		Green,
		Red,
		Blue,
		Yellow
	};

	PhysicsComponent(GameObject* owner);
	~PhysicsComponent();

	void init(PhysicsWorld* world, bool isDynamic = true, bool isBullet = false);
	void cleanup();

	virtual void update(float deltaTime) override;

	// Layer management
	void setLayer(const std::string& layerName);
	const std::string& getLayer() const;

	// Physics properties
	void setPosition(const Vector2D& position);
	void initializeBody();
	void setVelocity(const Vector2D& velocity);
	Vector2D getVelocity();
	b2BodyId getBodyId() const;
	b2ShapeId getCollisionShapeId() const;
	b2ShapeId getSensorShapeId() const;

	bool isDynamic() const;
	bool isBullet() const;

	// Sensor management
	void createCollisionShape(float width, float height);
	void createSensorShape(float width, float height);
	void createCollisionShapeFromSprite();
	void createSensorShapeFromSprite(float scaleFactor, PhysicsSensorListener* listener);
	void createSensorShapeFromSprite(PhysicsSensorListener* listener);
	void enableSensorEvents();
	void disableSensorEvents();
	void setSensorListener(PhysicsSensorListener* listener);
	bool areSensorEventsEnabled() const;
	void handleSensorBegin(GameObject* other);
	void handleSensorEnd(GameObject* other);

	// Debug visualization
	void setDebugDraw(bool enable);
	void setDebugColor(DebugColor color);
	void setOverlapping(bool overlapping);
	virtual void render() override;

	void setImmunity(float duration);
	bool isImmune() const;
};