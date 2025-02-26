#include "GameObject.h"
#include "PhysicsComponent.h"


GameObject::GameObject()
	: m_transform(nullptr)
{
	// Create default transform component
	m_transform = addComponent<TransformComponent>();
}

GameObject::~GameObject()
{
	for (auto component : m_components)
	{
		delete component;
	}
	m_components.clear();
	m_transform = nullptr;
}

void GameObject::setPosition(const Vector2D& pos)
{
	getTransform()->setPosition(pos);
}

void GameObject::init()
{
	// Initialize components
	for (auto& component : m_components) {
		component->init();
	}
}

void GameObject::update(float deltaTime)
{
	// Update components
	for (auto& component : m_components) {
		component->update(deltaTime);
	}
}

void GameObject::render()
{
	for (auto& component : m_components) {
		component->render();
	}
}

void GameObject::setLevel(Level* level)
{
	m_level = level;
	if(m_level)
	{
		init();
	}
}

const Level* GameObject::getLevel() const
{
	return m_level;
}

Level* GameObject::getLevel()
{
	return m_level;
}
