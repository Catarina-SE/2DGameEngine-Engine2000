#pragma once

#include "Core.h"
#include "Component.h"
#include "TransformComponent.h"
#include "E2Log.h"
#include <vector>
#include <typeinfo>

struct SDL_Rect;
struct SDL_Renderer;
class Sprite;
class Level;

class ENGINE2000_API GameObject {
private:
	TransformComponent* m_transform;
	std::vector<Component*> m_components;

protected:
	Level* m_level;

public:
	GameObject();
	~GameObject();

	void setPosition(const Vector2D& pos);

	virtual void init();;
	virtual void update(float deltaTime);
	void render();

	void setLevel(Level* level);
	const Level* getLevel() const;
	Level* getLevel();
	TransformComponent* getTransform() { return m_transform; }

	template<typename T> T* addComponent()
	{
		static_assert(std::is_base_of<Component, T>::value, "T must inherit from Component");

		// Check if component of this type already exists
		if (getComponent<T>() != nullptr)
		{
			E2_LOG(Error, "Failed to add component: GameObject already has a %s", typeid(T).name());
			return nullptr;
		}

		auto component = new T(this);
		T* componentPtr = component;
		m_components.push_back(component);
		return componentPtr;
	}

	template<typename T> T* getComponent()
	{
		static_assert(std::is_base_of<Component, T>::value, "T must inherit from Component");
		for (auto component : m_components)
		{
			if (T* castComponent = dynamic_cast<T*>(component))
				return castComponent;
		}
		return nullptr;
	}

};