#include "PhysicsWorld.h"
#include "EngineError.h"
#include "PhysicsLayerManager.h"
#include "PhysicsComponent.h"
#include "Renderer.h"
#include "E2Log.h"

const float PhysicsWorld::VELOCITY_SCALE = 60.0f;

bool PhysicsWorld::filterCallback(b2ShapeId shapeIdA, b2ShapeId shapeIdB, void* context)
{
	// Get layer names from shape user data 
	const char* layerA = static_cast<const char*>(b2Shape_GetUserData(shapeIdA));
	const char* layerB = static_cast<const char*>(b2Shape_GetUserData(shapeIdB));

	// If either shape is a sensor with events enabled, allow the detection
	bool sensorEventsA = b2Shape_AreSensorEventsEnabled(shapeIdA);
	bool sensorEventsB = b2Shape_AreSensorEventsEnabled(shapeIdB);
	
	if(!PhysicsLayerManager::getInstance().shouldLayersCollide(layerA, layerB))
	{
		if (sensorEventsA || sensorEventsB) {
			return true;  // Allow sensor detection
		}
	}

	// For physical collisions, use the layer matrix
	return PhysicsLayerManager::getInstance().shouldLayersCollide(layerA, layerB);
}

PhysicsWorld::PhysicsWorld()
	: m_timeStep(1.0f / 30.0f)
	, m_subSteps(4)
	, m_worldWidth(0)
	, m_worldHeight(0)
	, m_gravity(0.0f, 0.0f)
	, m_worldId(b2_nullWorldId)
	, m_debugDraw(false)
{
}

PhysicsWorld::~PhysicsWorld()
{
	cleanup();
}

void PhysicsWorld::init(float worldWidth, float worldHeight, const Vector2D& gravity = Vector2D(0.0f, 0.0f))
{
	m_worldWidth = worldWidth;
	m_worldHeight = worldHeight;
	m_gravity = gravity;

	b2WorldDef worldDef = b2DefaultWorldDef();
	worldDef.gravity = {m_gravity.x, m_gravity.y};

	m_worldId = b2CreateWorld(&worldDef);
	if (!b2World_IsValid(m_worldId)) {
		throw EngineError("Failed to create Box2D world");
	}

	// Set up collision filtering
	b2World_SetCustomFilterCallback(m_worldId, filterCallback, this);

	E2_LOG(Log, "PhysicsWorld initialized: %fx%f with gravity (%f, %f)",
		worldWidth, worldHeight, gravity.x, gravity.y);
}


void PhysicsWorld::cleanup()
{
	if (b2World_IsValid(m_worldId)) {
		b2DestroyWorld(m_worldId);
		m_worldId = b2_nullWorldId;
	}
}

void PhysicsWorld::update()
{
	if (b2World_IsValid(m_worldId)) {
		b2World_Step(m_worldId, m_timeStep, m_subSteps);
		processSensors();
	}
}

void PhysicsWorld::setGravity(const Vector2D& gravity) {
	m_gravity = gravity;
	if (b2World_IsValid(m_worldId)) {
		b2World_SetGravity(m_worldId, { m_gravity.x, m_gravity.y });
		E2_LOG(Log, "Updated gravity to (%f,%f)", gravity.x, gravity.y);
	}
}


b2BodyId PhysicsWorld::createBody(const Vector2D& position, bool isDynamic, bool isBullet)
{
	b2BodyDef bodyDef = b2DefaultBodyDef();
	bodyDef.position = { position.x, position.y};
	bodyDef.type = isDynamic ? b2_dynamicBody : b2_staticBody;
	bodyDef.isBullet = isBullet;

	return b2CreateBody(m_worldId, &bodyDef);
}

b2ShapeId PhysicsWorld::createBoxShape(b2BodyId bodyId, float width, float height, float density)
{
	b2ShapeDef shapeDef = b2DefaultShapeDef();
	shapeDef.density = density;

	b2Polygon box = b2MakeBox(width / 2.0f, height / 2.0f);
	return b2CreatePolygonShape(bodyId, &shapeDef, &box);
}

void PhysicsWorld::destroyBody(b2BodyId bodyId)
{
	if (b2Body_IsValid(bodyId)) {
		b2DestroyBody(bodyId);
	}
}

Vector2D PhysicsWorld::getBodyPosition(b2BodyId bodyId)
{
	if (!b2Body_IsValid(bodyId)) return { 0.0f, 0.0f };

	b2Vec2 pos = b2Body_GetPosition(bodyId);
	return { pos.x, pos.y };
}

Vector2D PhysicsWorld::getBodyVelocity(b2BodyId bodyId)
{
	if (!b2Body_IsValid(bodyId)) return { 0.0f, 0.0f };

	b2Vec2 vel = b2Body_GetLinearVelocity(bodyId);
	return { vel.x, vel.y };
}

void PhysicsWorld::setBodyPosition(b2BodyId bodyId, const Vector2D& position)
{
	if (!b2Body_IsValid(bodyId)) return;

	b2Rot currentRotation = b2Body_GetRotation(bodyId);
	b2Body_SetTransform(bodyId, { position.x, position.y }, currentRotation);
}

void PhysicsWorld::setBodyVelocity(b2BodyId bodyId, const Vector2D& velocity)
{
	if (!b2Body_IsValid(bodyId)) return;
	Vector2D scaledVelocity = velocity * VELOCITY_SCALE;
	b2Body_SetLinearVelocity(bodyId, { scaledVelocity.x, scaledVelocity.y });
}

void PhysicsWorld::processSensors()
{
	b2SensorEvents sensorEvents = b2World_GetSensorEvents(m_worldId);
	//E2_LOG(Warning, "Received sensor events: %d", sensorEvents.beginCount);

	for (int i = 0; i < sensorEvents.beginCount; ++i) {
		const b2SensorBeginTouchEvent* event = sensorEvents.beginEvents + i;

// 		E2_LOG(Warning, "Processing event with shapes: %d and %d",
// 			event->sensorShapeId.index1,
// 			event->visitorShapeId.index1);

		PhysicsComponent* sensorComponent = static_cast<PhysicsComponent*>(
			b2Body_GetUserData(b2Shape_GetBody(event->sensorShapeId)));
		PhysicsComponent* visitorComponent = static_cast<PhysicsComponent*>(
			b2Body_GetUserData(b2Shape_GetBody(event->visitorShapeId)));

		if (sensorComponent && visitorComponent) {
// 			E2_LOG(Warning, "Found components, layers: %s and %s",
// 				sensorComponent->getLayer().c_str(),
// 				visitorComponent->getLayer().c_str());

			sensorComponent->handleSensorBegin(visitorComponent->getOwner());
			visitorComponent->handleSensorBegin(sensorComponent->getOwner());
		}
		else {
			E2_LOG(Warning, "Failed to get components from bodies");
		}
	}
}