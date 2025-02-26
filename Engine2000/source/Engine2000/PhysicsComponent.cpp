#include "PhysicsComponent.h"
#include "GameObject.h"
#include "TransformComponent.h"
#include "SpriteComponent.h"
#include "PhysicsWorld.h"
#include "PhysicsLayerManager.h"
#include "Level.h"
#include "Renderer.h"
#include "E2Log.h"
#include <box2d/box2d.h>
#include <SDL2/SDL.h>

class PhysicsComponent::PhysicsComponentImpl
{
public:
	PhysicsWorld* physicsWorld;
	b2BodyId bodyId;
	b2ShapeId collisionShapeId;
	b2ShapeId sensorShapeId;
	PhysicsSensorListener* sensorListener;
	bool sensorEventsEnabled;
	bool isDynamic;
	bool isBullet;
	std::string layer;

	// Debug
	bool debugDraw;
	bool isOverlapping;
	DebugColor debugColor;
	DebugColor overlappingColor;
	
	// Immunity
	float m_immunityDuration;
	float m_immunityTimer;
	bool m_isImmune;

	b2HexColor getBox2DColor(DebugColor color) const {
		switch (color) {
		case DebugColor::Green:  return b2_colorBox2DGreen;
		case DebugColor::Red:    return b2_colorBox2DRed;
		case DebugColor::Blue:   return b2_colorBox2DBlue;
		case DebugColor::Yellow: return b2_colorBox2DYellow;
		default:                 return b2_colorBox2DGreen;
		}
	}

	PhysicsComponentImpl()
		: physicsWorld(nullptr)
		, bodyId(b2_nullBodyId)
		, collisionShapeId(b2_nullShapeId)
		, sensorShapeId(b2_nullShapeId)
		, sensorListener(nullptr)
		, sensorEventsEnabled(false)
		, isDynamic(false)
		, isBullet(false)
		, layer("Default")
		, debugDraw(false)
		, isOverlapping(false)
		, debugColor(DebugColor::Green) // Default green
		, overlappingColor(DebugColor::Red) // Default Red
		, m_immunityDuration(0.0f)
		, m_immunityTimer(0.0f)
		, m_isImmune(false)
	{}

	b2ShapeId createShape(b2BodyId bodyId, float width, float height, bool isSensor, bool enableSensorEvents) {
		b2ShapeDef shapeDef = b2DefaultShapeDef();
		shapeDef.isSensor = isSensor;
		shapeDef.enableSensorEvents = enableSensorEvents;
		shapeDef.density = 1.0f;
		shapeDef.restitution = 0.0f;
		shapeDef.friction = 0.0f;

		// Set layer information
		shapeDef.userData = const_cast<char*>(layer.c_str());
		//E2_LOG(Log, "Creating shape - IsSensor: %d, SensorEvents: %d", shapeDef.isSensor, shapeDef.enableSensorEvents);

		b2Vec2 boxCenter = { width / 2.0f, height / 2.0f };
		b2Rot rotation = { 1.0f, 0.0f };
		b2Polygon box = b2MakeOffsetBox(width / 2.0f, height / 2.0f, boxCenter, rotation);
		
		return b2CreatePolygonShape(bodyId, &shapeDef, &box);
	}

private:
	void drawDebugShapesSDL(SDL_Renderer* renderer, const b2AABB& aabb, const b2HexColor& color)
	{
		// Convert Box2D hex color to RGB
		uint8_t r = (color >> 16) & 0xFF;
		uint8_t g = (color >> 8) & 0xFF;
		uint8_t b = color & 0xFF;

		// Draw filled shape with alpha
		SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
		SDL_SetRenderDrawColor(renderer, r, g, b, 127);

		SDL_Rect bounds = {
			static_cast<int>(aabb.lowerBound.x),
			static_cast<int>(aabb.lowerBound.y),
			static_cast<int>(aabb.upperBound.x - aabb.lowerBound.x),
			static_cast<int>(aabb.upperBound.y - aabb.lowerBound.y)
		};

		SDL_RenderFillRect(renderer, &bounds);

		// Draw outline with full alpha
		SDL_SetRenderDrawColor(renderer, r, g, b, 255);
		SDL_RenderDrawRect(renderer, &bounds);
	}

	void drawDebugShapesGL(const b2AABB& aabb, const b2HexColor& color)
	{
		// Convert Box2D hex color to normalized RGB
		float r = ((color >> 16) & 0xFF) / 255.0f;
		float g = ((color >> 8) & 0xFF) / 255.0f;
		float b = (color & 0xFF) / 255.0f;

		// Create vertices for the box
		float vertices[] = {
			// Position           // Color
			aabb.lowerBound.x, aabb.lowerBound.y,    r, g, b, 0.5f,
			aabb.upperBound.x, aabb.lowerBound.y,    r, g, b, 0.5f,
			aabb.upperBound.x, aabb.upperBound.y,    r, g, b, 0.5f,
			aabb.lowerBound.x, aabb.upperBound.y,    r, g, b, 0.5f,

			// Outline (same vertices but full alpha)
			aabb.lowerBound.x, aabb.lowerBound.y,    r, g, b, 1.0f,
			aabb.upperBound.x, aabb.lowerBound.y,    r, g, b, 1.0f,
			aabb.upperBound.x, aabb.upperBound.y,    r, g, b, 1.0f,
			aabb.lowerBound.x, aabb.upperBound.y,    r, g, b, 1.0f
		};
	}
public:
	void render(GameObject* owner)
	{
		if (!debugDraw || !b2Shape_IsValid(collisionShapeId)) return;

		// Get shape bounds
		b2AABB aabb = b2Shape_GetAABB(collisionShapeId);

		// Get Box2D color based on state
		b2HexColor box2dColor = isOverlapping ?
			getBox2DColor(overlappingColor) :
			getBox2DColor(debugColor);

		// Convert Box2D hex color to RGB
		uint8_t r = (box2dColor >> 16) & 0xFF;
		uint8_t g = (box2dColor >> 8) & 0xFF;
		uint8_t b = box2dColor & 0xFF;

		Vector4D rect(
			aabb.lowerBound.x,
			aabb.lowerBound.y,
			aabb.upperBound.x - aabb.lowerBound.x,
			aabb.upperBound.y - aabb.lowerBound.y
		);

		Vector4D color(r, g, b, 127);  // 127 for half transparency

		if (Renderer::Instance().isOpenGL()) {
			// Draw filled shape with alpha
			Renderer::Instance().fillRect(rect, color);

			// Draw outline with full opacity
			color.h = 255;  // Full alpha for outline
			Renderer::Instance().drawRect(rect, color);
		}
		else {
			auto renderer = static_cast<SDL_Renderer*>(owner->getLevel()->getRenderer());

			SDL_Rect bounds = {
				static_cast<int>(rect.x),
				static_cast<int>(rect.y),
				static_cast<int>(rect.w),
				static_cast<int>(rect.h)
			};

			// Draw filled shape with alpha
			SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
			SDL_SetRenderDrawColor(renderer, r, g, b, 127);
			SDL_RenderFillRect(renderer, &bounds);

			// Draw outline with full alpha
			SDL_SetRenderDrawColor(renderer, r, g, b, 255);
			SDL_RenderDrawRect(renderer, &bounds);
		}
	}
};

PhysicsComponent::PhysicsComponent(GameObject* owner)
	: Component(owner)
	, pimpl(new PhysicsComponentImpl())
{
}

PhysicsComponent::~PhysicsComponent()
{
	cleanup();
	delete pimpl;
}

void PhysicsComponent::init(PhysicsWorld* world, bool isDynamic, bool isBullet)
{
	if (b2Body_IsValid(pimpl->bodyId)) {
		E2_LOG(Warning, "Body already exists for this component!");
		return;
	}

	pimpl->physicsWorld = world;
	pimpl->isDynamic = isDynamic;
	pimpl->isBullet = isBullet;

	auto transform = m_owner->getTransform();
	auto position = transform->getPosition();

	// Create physics body
	pimpl->bodyId = world->createBody(position, isDynamic, isBullet);

	// Set body properties
	b2Body_SetLinearDamping(pimpl->bodyId, 0.0f);
	b2Body_SetAngularDamping(pimpl->bodyId, 0.0f);
	b2Body_SetGravityScale(pimpl->bodyId, 0.0f);
	b2Body_SetFixedRotation(pimpl->bodyId, true);

	if (!isDynamic)
	{
		b2Body_SetType(pimpl->bodyId, b2_kinematicBody);
	}

	b2Body_SetUserData(pimpl->bodyId, this);

	if (m_owner && b2Body_IsValid(pimpl->bodyId)) {
		b2Vec2 pos = b2Body_GetPosition(pimpl->bodyId);
		m_owner->getTransform()->setPosition(pos.x, pos.y);
	}
}

void PhysicsComponent::cleanup()
{
	if (pimpl->physicsWorld && b2Body_IsValid(pimpl->bodyId)) {
		if (b2Shape_IsValid(pimpl->collisionShapeId)) {
			b2DestroyShape(pimpl->collisionShapeId, false);
			pimpl->collisionShapeId = b2_nullShapeId;
		}

		if (b2Shape_IsValid(pimpl->sensorShapeId)) {
			b2DestroyShape(pimpl->sensorShapeId, false);
			pimpl->sensorShapeId = b2_nullShapeId;
		}

		pimpl->physicsWorld->destroyBody(pimpl->bodyId);
		pimpl->bodyId = b2_nullBodyId;
	}
}

void PhysicsComponent::update(float deltaTime)
{
	if (!pimpl->physicsWorld || !b2Body_IsValid(pimpl->bodyId)) return;

	if (pimpl->m_isImmune) {
		pimpl->m_immunityTimer -= deltaTime;
		if (pimpl->m_immunityTimer <= 0) {
			pimpl->m_isImmune = false;
		}
	}

	// Update GameObject position based on physics simulation
	auto position = pimpl->physicsWorld->getBodyPosition(pimpl->bodyId);
	m_owner->getTransform()->setPosition(position.x, position.y);
}

void PhysicsComponent::setLayer(const std::string& layerName)
{
	if (PhysicsLayerManager::getInstance().getLayerIndex(layerName) != -1)
	{
		pimpl->layer = layerName;
		if (b2Shape_IsValid(pimpl->collisionShapeId))
		{
			b2Shape_SetUserData(pimpl->collisionShapeId, const_cast<char*>(pimpl->layer.c_str()));
			//E2_LOG(Log, "Physics layer changed to: %s", pimpl->layer.c_str());
		}
	}
	else
	{
		E2_LOG(Warning, "Attempted to set invalid physics layer: %s", layerName.c_str());
	}
}

const std::string& PhysicsComponent::getLayer() const
{
	return pimpl->layer;
}

void PhysicsComponent::createCollisionShape(float width, float height)
{
	if (b2Shape_IsValid(pimpl->collisionShapeId)) {
		E2_LOG(Warning, "Collision shape already exists!");
		return;
	}

	pimpl->sensorShapeId = pimpl->createShape(pimpl->bodyId, width, height, false, false);
	//E2_LOG(Log, "Created collision shape: %d", pimpl->collisionShapeId.index1);
}

void PhysicsComponent::createSensorShape(float width, float height)
{
	if (b2Shape_IsValid(pimpl->sensorShapeId)) {
		E2_LOG(Warning, "Sensor shape already exists!");
		return;
	}

	pimpl->sensorShapeId = pimpl->createShape(pimpl->bodyId, width, height, true, true);
	//E2_LOG(Log, "Created sensor shape: %d", pimpl->sensorShapeId.index1);
}

void PhysicsComponent::createCollisionShapeFromSprite() {
	if (b2Shape_IsValid(pimpl->collisionShapeId)) {
		E2_LOG(Warning, "Collision shape already exists!");
		return;
	}

	float width = 1.0f;
	float height = 1.0f;

	auto sprite = m_owner->getComponent<SpriteComponent>();
	if (sprite) {
		width = static_cast<float>(sprite->getFrameWidth());
		height = static_cast<float>(sprite->getFrameHeight());

		if (width <= 0.0f) width = 1.0f;
		if (height <= 0.0f) height = 1.0f;

		//E2_LOG(Log, "Creating collision shape with sprite dimensions: %f x %f", width, height);
	}
	else {
		E2_LOG(Warning, "No sprite found for physics component, creating collision shape using default 1x1 box");
	}

	pimpl->collisionShapeId = pimpl->createShape(pimpl->bodyId, width, height, false, false);
}

void PhysicsComponent::createSensorShapeFromSprite(float scaleFactor, PhysicsSensorListener* listener) {
	if (b2Shape_IsValid(pimpl->sensorShapeId)) {
		E2_LOG(Warning, "Sensor shape already exists!");
		return;
	}

	float width = 1.0f;
	float height = 1.0f;

	auto sprite = m_owner->getComponent<SpriteComponent>();
	if (sprite) {
		width = static_cast<float>(sprite->getFrameWidth()) * scaleFactor;
		height = static_cast<float>(sprite->getFrameHeight()) * scaleFactor;

		if (width <= 0.0f) width = 1.0f * scaleFactor;
		if (height <= 0.0f) height = 1.0f * scaleFactor;

		//E2_LOG(Log, "Creating sensor shape with sprite dimensions: %f x %f", width, height);
	}
	else {
		E2_LOG(Warning, "No sprite found for physics component, using default 1x1 box with scale factor %f", scaleFactor);
		width *= scaleFactor;
		height *= scaleFactor;
	}

	pimpl->sensorShapeId = pimpl->createShape(pimpl->bodyId, width, height, true, true);
	//E2_LOG(Log, "Created sensor shape: %d", pimpl->sensorShapeId.index1);

	// Automatically enable sensor events.
	pimpl->sensorEventsEnabled = true;

	// Set the listener if provided.
	if (listener) {
		setSensorListener(listener);
	}
}

void PhysicsComponent::createSensorShapeFromSprite(PhysicsSensorListener* listener)
{
	createSensorShapeFromSprite(1.0f, listener);
}

void PhysicsComponent::enableSensorEvents()
{
	pimpl->sensorEventsEnabled = true;
	if (b2Shape_IsValid(pimpl->sensorShapeId))
	{
		b2Shape_EnableSensorEvents(pimpl->sensorShapeId, true);
	}
}

void PhysicsComponent::disableSensorEvents()
{
	pimpl->sensorEventsEnabled = false;
	if (b2Shape_IsValid(pimpl->sensorShapeId))
	{
		b2Shape_EnableSensorEvents(pimpl->sensorShapeId, false);
	}
}

void PhysicsComponent::setSensorListener(PhysicsSensorListener* listener)
{
	pimpl->sensorListener = listener;
}

void PhysicsComponent::handleSensorBegin(GameObject* other)
{
	auto otherPhysics = other->getComponent<PhysicsComponent>();
	if (isImmune() || (otherPhysics && otherPhysics->isImmune()))
	{
		return;
	}

	if (pimpl->sensorListener)
	{
		pimpl->sensorListener->onSensorBegin(other);
		setOverlapping(true);
	}

}

void PhysicsComponent::handleSensorEnd(GameObject* other)
{
	if (pimpl->sensorListener)
	{
		pimpl->sensorListener->onSensorEnd(other);
		setOverlapping(false);
	}
}

void PhysicsComponent::setDebugDraw(bool enable)
{
	pimpl->debugDraw = enable;
}

void PhysicsComponent::setDebugColor(DebugColor color)
{
	pimpl->debugColor = color;
}

void PhysicsComponent::setOverlapping(bool overlapping)
{
	pimpl->isOverlapping = overlapping;
}

void PhysicsComponent::render()
{
	pimpl->render(getOwner());
}

void PhysicsComponent::setImmunity(float duration)
{
	pimpl->m_immunityDuration = duration;
	pimpl->m_immunityTimer = duration;
	pimpl->m_isImmune = true;
}

bool PhysicsComponent::isImmune() const
{
	return pimpl->m_isImmune;
}

bool PhysicsComponent::areSensorEventsEnabled() const
{
	return pimpl->sensorEventsEnabled;
}

void PhysicsComponent::setPosition(const Vector2D& position)
{
	if (pimpl->physicsWorld && b2Body_IsValid(pimpl->bodyId))
	{
		pimpl->physicsWorld->setBodyPosition(pimpl->bodyId, position);
	}
}

void PhysicsComponent::setVelocity(const Vector2D& velocity)
{
	if (pimpl->physicsWorld && b2Body_IsValid(pimpl->bodyId))
	{
		// If it's kinematic, set linear velocity directly with scaling
		if (!pimpl->isDynamic)
		{
			Vector2D scaledVelocity = velocity * PhysicsWorld::getVelocityScale();
			b2Body_SetLinearVelocity(pimpl->bodyId, { scaledVelocity.x, scaledVelocity.y });
		}
		else
		{
			// For dynamic bodies, use PhysicsWorld's method which handles scaling
			pimpl->physicsWorld->setBodyVelocity(pimpl->bodyId, velocity);
		}
	}
}

Vector2D PhysicsComponent::getVelocity()
{
	if (pimpl->physicsWorld && b2Body_IsValid(pimpl->bodyId))
	{
		return pimpl->physicsWorld->getBodyVelocity(pimpl->bodyId);
	}
	return { 0.0f, 0.0f };
}

b2BodyId PhysicsComponent::getBodyId() const
{
	return pimpl->bodyId;
}

b2ShapeId PhysicsComponent::getCollisionShapeId() const
{
	return pimpl->collisionShapeId;
}

b2ShapeId PhysicsComponent::getSensorShapeId() const
{
	return pimpl->sensorShapeId;
}

bool PhysicsComponent::isDynamic() const
{
	return pimpl->isDynamic;
}

bool PhysicsComponent::isBullet() const
{
	return pimpl->isBullet;
}

