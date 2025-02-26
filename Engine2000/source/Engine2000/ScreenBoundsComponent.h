#pragma once

#include "Core.h"
#include "Component.h"
#include "Vector2D.h"
#include "IBoundsResponder.h"

class ENGINE2000_API ScreenBoundsComponent : public Component {
public:
	// How the GameObject should behave when going out of bounds
	enum class BoundaryBehavior {
		DESTROY,	// Destroy the GameObject when it goes out of bounds
		SLEEP,		// Make the GameObject dormant when out of bounds
		IGNORE		// Do nothing when out of bounds
	};

	// Which boundaries to check
	struct BoundaryFlags {
		bool checkTop = true;
		bool checkBottom = true;
		bool checkLeft = true;
		bool checkRight = true;
	};

private:
	BoundaryBehavior m_behavior;
	BoundaryFlags m_flags;
	float m_margin;
	bool m_isOutOfBounds;
	bool m_isSleeping;
	IBoundsResponder* m_responder;

public:
	ScreenBoundsComponent(GameObject* owner);
	virtual void init() override;
	virtual void update(float deltaTime) override;

	// Configuration
	void setBehavior(BoundaryBehavior behavior) { m_behavior = behavior; }
	void setBoundaryFlags(const BoundaryFlags& flags) { m_flags = flags; }
	void setMargin(float margin) { m_margin = margin; }

	// Status checks
	bool isOutOfBounds() const { return m_isOutOfBounds; }
	bool isSleeping() const { return m_isSleeping; }

private:
	bool checkBounds();
	void handleOutOfBounds();
	void wakeUp();
};
