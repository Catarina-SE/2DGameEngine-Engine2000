#pragma once

#include "Core.h"

class GameObject;

class ENGINE2000_API Component
{
protected:
	GameObject* m_owner;

public:
	Component(GameObject* owner) : m_owner(owner) {}
	virtual ~Component() = default;

	virtual void init() {} 
	virtual void update(float deltaTime) {}
	virtual void render() {}
	GameObject* getOwner() { return m_owner; }
};