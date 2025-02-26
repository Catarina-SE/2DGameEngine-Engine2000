#pragma once

#include <string>

class Window
{
private:
	class WindowImpl;
	WindowImpl* pimpl;

public:
	Window(const std::string& title, int windowWidth, int windowHeight, bool useOpenGL = true);
	~Window();

	void updateSurface();

	// Getters
	void* getWindow() const;
	void getSize(int& width, int& height) const;

	// OpenGL getters
	bool isOpenGL() const;
	void* getGLContext() const;
};
