#pragma once

#include "Core.h"

enum class KeyCode {
	Left, Right, Up, Down,
	A, B, C, D, E, F, G, H, I, J, K, L, M,
	N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
	Space, Enter, Escape, Return, 
	NUM1, NUM2, NUM3, NUM4, NUM5, 
	NUM6, NUM7, NUM8, NUM9, NUM0
};

enum class Button {
	DPadLeft, DPadRight, DPadUp, DPadDown,
	A, B, X, Y,
	Start, Select, Guide,
	LeftStick, RightStick, LeftBumper, RightBumper
};

class ENGINE2000_API Input {
private:
	Input(const Input&) = delete;
	Input& operator=(const Input&) = delete;

	class InputImpl;
	InputImpl* pimpl;

public:
	Input() = default;
	~Input() = default;

	void init();
	void update();
	void cleanup();

	bool getKey(KeyCode key) const;
	bool getKeyPressed(KeyCode Key) const;
	bool getKeyReleased(KeyCode key) const;
	bool getButton(Button button) const;
};
