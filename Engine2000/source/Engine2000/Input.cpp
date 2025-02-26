#include "Input.h"
#include <SDL2/SDL.h>
#include <string>
#include <iostream>
#include <unordered_map>
#include "E2Log.h"

class Input::InputImpl {
private:
	const Uint8* m_keyboardState;
	SDL_GameController* m_gameController;
	std::unordered_map<SDL_Scancode, bool> m_keyPressed;
	std::unordered_map<SDL_Scancode, bool> m_keyReleased;

	SDL_Scancode getSDLScancode(KeyCode key) const
	{
		switch (key) {
		case KeyCode::Left:    return SDL_SCANCODE_LEFT;
		case KeyCode::Right:   return SDL_SCANCODE_RIGHT;
		case KeyCode::Up:      return SDL_SCANCODE_UP;
		case KeyCode::Down:    return SDL_SCANCODE_DOWN;
		case KeyCode::Space:   return SDL_SCANCODE_SPACE;
		case KeyCode::Enter:   return SDL_SCANCODE_RETURN;
		case KeyCode::Escape:  return SDL_SCANCODE_ESCAPE;
		case KeyCode::A:       return SDL_SCANCODE_A;
		case KeyCode::B:       return SDL_SCANCODE_B;
		case KeyCode::C:       return SDL_SCANCODE_C;
		case KeyCode::D:       return SDL_SCANCODE_D;
		case KeyCode::E:       return SDL_SCANCODE_E;
		case KeyCode::F:       return SDL_SCANCODE_F;
		case KeyCode::G:       return SDL_SCANCODE_G;
		case KeyCode::H:       return SDL_SCANCODE_H;
		case KeyCode::I:       return SDL_SCANCODE_I;
		case KeyCode::J:       return SDL_SCANCODE_J;
		case KeyCode::K:       return SDL_SCANCODE_K;
		case KeyCode::L:       return SDL_SCANCODE_L;
		case KeyCode::M:       return SDL_SCANCODE_M;
		case KeyCode::N:       return SDL_SCANCODE_N;
		case KeyCode::O:       return SDL_SCANCODE_O;
		case KeyCode::P:       return SDL_SCANCODE_P;
		case KeyCode::Q:       return SDL_SCANCODE_Q;
		case KeyCode::R:       return SDL_SCANCODE_R;
		case KeyCode::S:       return SDL_SCANCODE_S;
		case KeyCode::T:       return SDL_SCANCODE_T;
		case KeyCode::U:       return SDL_SCANCODE_U;
		case KeyCode::V:       return SDL_SCANCODE_V;
		case KeyCode::W:       return SDL_SCANCODE_W;
		case KeyCode::X:       return SDL_SCANCODE_X;
		case KeyCode::Y:       return SDL_SCANCODE_Y;
		case KeyCode::Z:       return SDL_SCANCODE_Z;
		case KeyCode::NUM1:    return SDL_SCANCODE_1;
		case KeyCode::NUM2:    return SDL_SCANCODE_2;
		case KeyCode::NUM3:    return SDL_SCANCODE_3;
		case KeyCode::NUM4:    return SDL_SCANCODE_4;
		case KeyCode::NUM5:    return SDL_SCANCODE_5;
		case KeyCode::NUM6:    return SDL_SCANCODE_6;
		case KeyCode::NUM7:    return SDL_SCANCODE_7;
		case KeyCode::NUM8:    return SDL_SCANCODE_8;
		case KeyCode::NUM9:    return SDL_SCANCODE_9;
		case KeyCode::NUM0:    return SDL_SCANCODE_0;
		default:              return SDL_SCANCODE_UNKNOWN;
		}

	}

	SDL_GameControllerButton getSDLButton(Button button) const
	{
		switch (button) {
		case Button::DPadLeft:     return SDL_CONTROLLER_BUTTON_DPAD_LEFT;
		case Button::DPadRight:    return SDL_CONTROLLER_BUTTON_DPAD_RIGHT;
		case Button::DPadUp:       return SDL_CONTROLLER_BUTTON_DPAD_UP;
		case Button::DPadDown:     return SDL_CONTROLLER_BUTTON_DPAD_DOWN;
		case Button::A:            return SDL_CONTROLLER_BUTTON_A;
		case Button::B:            return SDL_CONTROLLER_BUTTON_B;
		case Button::X:            return SDL_CONTROLLER_BUTTON_X;
		case Button::Y:            return SDL_CONTROLLER_BUTTON_Y;
		case Button::Start:        return SDL_CONTROLLER_BUTTON_START;
		case Button::Select:       return SDL_CONTROLLER_BUTTON_BACK;
		case Button::Guide:        return SDL_CONTROLLER_BUTTON_GUIDE;
		case Button::LeftStick:    return SDL_CONTROLLER_BUTTON_LEFTSTICK;
		case Button::RightStick:   return SDL_CONTROLLER_BUTTON_RIGHTSTICK;
		case Button::LeftBumper:   return SDL_CONTROLLER_BUTTON_LEFTSHOULDER;
		case Button::RightBumper:  return SDL_CONTROLLER_BUTTON_RIGHTSHOULDER;
		default:                   return SDL_CONTROLLER_BUTTON_INVALID;
		}
	}

public:
	InputImpl() : m_keyboardState(nullptr), m_gameController(nullptr) {}

	void init()
	{
		m_keyboardState = SDL_GetKeyboardState(nullptr);
		if (SDL_InitSubSystem(SDL_INIT_GAMECONTROLLER) >= 0)
		{
			if (SDL_NumJoysticks() > 0)
			{
				m_gameController = SDL_GameControllerOpen(0);
			}
		}
	}

	void clearFrameStates()
	{
		m_keyPressed.clear();
		m_keyReleased.clear();
	}

	void handleEvent(const SDL_Event& event)
	{
		if (event.type == SDL_KEYDOWN)
		{
			if (!event.key.repeat)	// Detect initial key press
			{
				m_keyPressed[event.key.keysym.scancode] = true;
			}
		}
		else if (event.type == SDL_KEYUP)
		{
			m_keyReleased[event.key.keysym.scancode] = true;
		}
	}

	bool getKeyPressed(KeyCode key)
	{
		SDL_Scancode scancode = getSDLScancode(key);
		return m_keyPressed[scancode];
	}

	bool getKeyReleased(KeyCode key)
	{
		SDL_Scancode scancode = getSDLScancode(key);
		return m_keyReleased[scancode];
	}

	bool getKey(KeyCode key) const
	{
		return m_keyboardState[getSDLScancode(key)];
	}

	bool getButton(Button button) const
	{
		if (!m_gameController) return false;
		return SDL_GameControllerGetButton(m_gameController, getSDLButton(button));
	}

	void cleanup()
	{
		if (m_gameController)
		{
			SDL_GameControllerClose(m_gameController);
			m_gameController = nullptr;
		}
	}
};

void Input::init()
{
	pimpl = new InputImpl();
	pimpl->init();
}

void Input::update()
{
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		pimpl->handleEvent(event);
	}
	// Clear the states for the next frame after all input processing is done
	pimpl->clearFrameStates();
}

void Input::cleanup()
{
	if (pimpl)
	{
		pimpl->cleanup();
		delete pimpl;
		pimpl = nullptr;
	}
}

bool Input::getKey(KeyCode key) const
{
	return pimpl ? pimpl->getKey(key) : false;
}

bool Input::getKeyPressed(KeyCode key) const
{
	return pimpl ? pimpl->getKeyPressed(key) : false;
}

bool Input::getKeyReleased(KeyCode key) const
{
	return pimpl ? pimpl->getKeyReleased(key) : false;
}

bool Input::getButton(Button button) const
{
	return pimpl ? pimpl->getButton(button) : false;
}