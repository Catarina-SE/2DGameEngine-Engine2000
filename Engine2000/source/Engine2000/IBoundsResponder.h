#pragma once

#include "Core.h"

class ENGINE2000_API IBoundsResponder
{
public:
	virtual ~IBoundsResponder() = default;

	// Called when the object is about to be destroyed due to being out of bounds
	virtual void onBoundsDestroy() {}

	// Called when the object is going to sleep due to being out of bounds
	virtual void onBoundsSleep() {}

	// Called when the object is waking up after returning to bounds
	virtual void onBoundsWakeup() {}
};