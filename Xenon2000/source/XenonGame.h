#pragma once

#include <Engine2000.h>
#include "Engine2000/Level.h"

class XenonGame : public GameEngine
{
private:
	float m_moveSpeed;
	Level* m_level;

public:
	XenonGame();
	~XenonGame() = default;

	virtual void onInit() override;
};