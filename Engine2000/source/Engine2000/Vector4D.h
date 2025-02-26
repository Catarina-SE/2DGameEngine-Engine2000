#pragma once

struct Vector4D {
	float x, y, w, h;
	Vector4D(float x = 0.0f, float y = 0.0f, float w = 0.0f, float h = 0.0f)
		: x(x), y(y), w(w), h(h) {}

	float getRight() const { return x + w; }
	float getBottom() const { return y + h; }
};