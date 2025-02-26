#pragma once

struct Vector2D
{
	float x, y;
	Vector2D(float x = 0.0f, float y = 0.0f) : x(x), y(y) {}

	// Scalar multiplication
	Vector2D operator*(float scalar) const {
		return Vector2D(x * scalar, y * scalar);
	}

	// Friend function for scalar * vector
	friend Vector2D operator*(float scalar, const Vector2D& vec) {
		return vec * scalar;
	}
};