#pragma once

#include "Core.h"
#include "Vector2D.h"

class ENGINE2000_API TransformComponent : public Component 
{
private: 
	Vector2D m_position;
	Vector2D m_scale;

public:
	TransformComponent(GameObject* owner)
		: Component(owner)
		, m_position(0.0f, 0.0f)
		, m_scale(1.0f,1.0f) {}

	void setPosition(float x, float y) { m_position = Vector2D(x,y); }
	void setPosition(Vector2D position) { m_position = position; }
	const Vector2D& getPosition() const { return m_position; }

	void setScale(float x, float y) { m_scale = Vector2D(x, y); }
	void setScale(float uniform) { m_scale = Vector2D(uniform, uniform); }
	const Vector2D& getScale() const { return m_scale; }
};
