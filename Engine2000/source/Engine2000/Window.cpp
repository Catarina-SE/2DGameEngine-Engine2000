#include "Window.h"
#include "EngineError.h"
#include "E2Log.h"

#include <SDL2/SDL.h>
#include "glad/glad.h"

class Window::WindowImpl
{
private:
	SDL_Window* m_window;
	SDL_GLContext m_glContext;
	bool m_useOpenGL;
	int m_width;
	int m_height;

public:
	WindowImpl(const std::string& title, int windowWidth, int windowHeight, bool useOpenGL)
		: m_window(nullptr)
		, m_useOpenGL(useOpenGL)
		, m_width(windowWidth)
		, m_height(windowHeight)
	{
		if (m_useOpenGL)
		{
			initOpenGL(title);
		}
		else
		{
			initSDL2(title);
		}
	}

	~WindowImpl()
	{
		if (m_useOpenGL && m_glContext)
		{
			SDL_GL_DeleteContext(m_glContext);
			m_glContext = nullptr;
		}
		
		if (m_window)
		{
			SDL_DestroyWindow(m_window);
			m_window = nullptr;
		}
	}

private:
	void initOpenGL(const std::string& title)
	{
		// Set OpenGL attributes before window creation
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

		// Create window with OpenGL support
		m_window = SDL_CreateWindow(
			title.c_str(),
			SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
			m_width, m_height,
			SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL
		);

		if (!m_window)
		{
			throw EngineError("Failed to create SDL window with OpenGL support");
		}

		// Create OpenGL context
		m_glContext = SDL_GL_CreateContext(m_window);
		if (!m_glContext)
		{
			SDL_DestroyWindow(m_window);
			throw EngineError("Failed to create OpenGL context");
		}

		// Initialize GLAD
		if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress))
		{
			SDL_GL_DeleteContext(m_glContext);
			SDL_DestroyWindow(m_window);
			throw EngineError("Failed to initialize GLAD");
		}

		// Log OpenGL version
		E2_LOG(Log, "OpenGL Context created - Version: %s", glGetString(GL_VERSION));
	}

	void initSDL2(const std::string& title)
	{
		m_window = SDL_CreateWindow(
			title.c_str(),
			SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
			m_width, m_height,
			SDL_WINDOW_SHOWN
		);

		if (!m_window)
		{
			throw EngineError();
		}

		E2_LOG(Log, "SDL2 Window Created: %dx%d", m_width, m_height);
	}

public:
	void updateSurface()
	{
		if (m_useOpenGL)
		{
			// For OpenGL, this is handled by the renderer
		}
		else
		{
			SDL_UpdateWindowSurface(m_window);
		}
	}

	void* getWindow() const
	{
		return m_window;
	}

	void getSize(int& width, int& height) const
	{
		width = m_width;
		height = m_height;
	}

	bool isOpenGL() const
	{
		return m_useOpenGL;
	}

	void* getGLContext() const
	{
		return m_glContext;
	}
};

Window::Window(const std::string& title, int windowWidth, int windowHeight, bool useOpenGL)
	: pimpl(new WindowImpl(title, windowWidth, windowHeight, useOpenGL))
{
}

Window::~Window()
{
	delete pimpl;
}

void Window::updateSurface()
{
	pimpl->updateSurface();
}

void* Window::getWindow() const
{
	return pimpl->getWindow();
}

void Window::getSize(int& width, int& height) const
{
	pimpl->getSize(width, height);
}

bool Window::isOpenGL() const
{
	return pimpl->isOpenGL();
}

void* Window::getGLContext() const
{
	return pimpl->getGLContext();
}
