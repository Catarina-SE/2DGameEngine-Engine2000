#include "Renderer.h"
#include "EngineError.h"
#include "Window.h"
#include "E2Log.h"

#include <SDL2/SDL.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glad/glad.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <direct.h> // for _getcwd

class Renderer::RendererImpl
{
private:
	Window* m_window;
	bool m_useOpenGL;

	// SDL2 specific members
	SDL_Renderer* m_sdlRenderer;

	// OpenGL specific members
	SDL_GLContext m_glContext;
	GLuint m_defaultShaderProgram;	// For textured sprites
	GLuint m_debugShaderProgram;	// For debug rectangles
	GLuint m_spriteVAO;
	GLuint m_spriteVBO;
	GLuint m_debugVAO;
	GLuint m_debugVBO;
	GLuint m_debugModelLoc;
	GLuint m_debugProjLoc;
	GLuint m_modelLoc;
	GLuint m_projectionLoc;
	GLuint m_textureLoc;
	GLuint m_colorLoc;
	glm::mat4 m_projection;

	// Add shader source strings as class members
	std::string m_vertexShaderSource;
	std::string m_fragmentShaderSource;
	std::string m_debugVertexShaderSource;
	std::string m_debugFragmentShaderSource;

public:
	RendererImpl()
		: m_window(nullptr)
		, m_useOpenGL(false)
		, m_sdlRenderer(nullptr)
		, m_glContext(nullptr)
		, m_defaultShaderProgram(0)
		, m_debugShaderProgram(0)
		, m_spriteVAO(0)
		, m_spriteVBO(0)
		, m_debugVAO(0)
		, m_debugVBO(0)
		, m_modelLoc(0)
		, m_projectionLoc(0)
		, m_textureLoc(0)
		, m_colorLoc(0)
		, m_debugModelLoc(0)
		, m_debugProjLoc(0)
		, m_projection(1.0f)
	{}

	void init(Window* window, bool useOpenGL)
	{
		m_window = window;
		m_useOpenGL = useOpenGL;

		if (m_useOpenGL)
		{
			SDL_GL_MakeCurrent(static_cast<SDL_Window*>(window->getWindow()),
				static_cast<SDL_GLContext>(window->getGLContext()));

			// Verify GLAD is initialized
			if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress))
			{
				throw EngineError("Failed to initialize GLAD in Renderer");
			}

			initOpenGL();
		}
		else
		{
			initSDL2();
		}
	}

	void cleanup()
	{
		if (m_useOpenGL)
		{
			if (m_defaultShaderProgram)
			{
				glDeleteProgram(m_defaultShaderProgram);
				m_defaultShaderProgram = 0;
			}
			if (m_debugShaderProgram)
			{
				glDeleteProgram(m_debugShaderProgram);
				m_debugShaderProgram = 0;
			}
			if (m_spriteVAO)
			{
				glDeleteVertexArrays(1, &m_spriteVAO);
				m_spriteVAO = 0;
			}
			if (m_spriteVBO)
			{
				glDeleteBuffers(1, &m_spriteVBO);
				m_spriteVBO = 0;
			}
			if (m_debugVAO)
			{
				glDeleteVertexArrays(1, &m_debugVAO);
				m_debugVAO = 0;
			}
			if (m_debugVBO)
			{
				glDeleteBuffers(1, &m_debugVBO);
				m_debugVBO = 0;
			}
			if (m_glContext)
			{
				SDL_GL_DeleteContext(m_glContext);
				m_glContext = nullptr;
			}
		}
		else
		{
			if (m_sdlRenderer)
			{
				SDL_DestroyRenderer(m_sdlRenderer);
				m_sdlRenderer = nullptr;
			}
		}
		m_window = nullptr;
	}

private:
	void checkGLError(const char* operation)
	{
		GLenum error = glGetError();
		if (error != GL_NO_ERROR) {
			E2_LOG(Error, "OpenGL Error after %s: %d", operation, error);
		}
	}

	std::string loadShaderFromFile(const char* filePath) {
		std::ifstream shaderFile(filePath);
		if (!shaderFile.is_open()) {
			E2_LOG(Error, "Failed to open shader file: %s", filePath);
			throw EngineError("Failed to open shader file");
		}

		std::stringstream buffer;
		buffer << shaderFile.rdbuf();
		return buffer.str();
	}

	void initOpenGL()
	{
		// Enable blending
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		// Load shader sources from files with correct path
		m_vertexShaderSource = loadShaderFromFile("../Engine2000/Shaders/vertexShader.glsl");
		m_fragmentShaderSource = loadShaderFromFile("../Engine2000/Shaders/fragmentShader.glsl");

		// Create default shader program
		const char* vertexSourcePtr = m_vertexShaderSource.c_str();
		GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertexShader, 1, &vertexSourcePtr, NULL);
		glCompileShader(vertexShader);
		checkShaderCompilation(vertexShader, "vertex");

		const char* fragmentSourcePtr = m_fragmentShaderSource.c_str();
		GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragmentShader, 1, &fragmentSourcePtr, NULL);
		glCompileShader(fragmentShader);
		checkShaderCompilation(fragmentShader, "fragment");

		m_defaultShaderProgram = glCreateProgram();
		glAttachShader(m_defaultShaderProgram, vertexShader);
		glAttachShader(m_defaultShaderProgram, fragmentShader);
		glLinkProgram(m_defaultShaderProgram);
		checkProgramLinking(m_defaultShaderProgram);

		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);

		glGenVertexArrays(1, &m_spriteVAO);
		glGenBuffers(1, &m_spriteVBO);

		glBindVertexArray(m_spriteVAO);

		// Create quad vertices
		float vertices[] = {
			// pos      // tex
			0.0f, 1.0f, 0.0f, 0.0f,  // top left
			1.0f, 1.0f, 1.0f, 0.0f,  // top right
			0.0f, 0.0f, 0.0f, 1.0f,  // bottom left
			1.0f, 0.0f, 1.0f, 1.0f   // bottom right
		};

		glBindBuffer(GL_ARRAY_BUFFER, m_spriteVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		// Position attribute
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		// Texture coord attribute
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
		glEnableVertexAttribArray(1);

		// Get uniform locations for sprite shader
		m_modelLoc = glGetUniformLocation(m_defaultShaderProgram, "model");
		m_projectionLoc = glGetUniformLocation(m_defaultShaderProgram, "projection");
		m_textureLoc = glGetUniformLocation(m_defaultShaderProgram, "mainTexture");
		m_colorLoc = glGetUniformLocation(m_defaultShaderProgram, "color");

		// Set up projection matrix
		int width, height;
		m_window->getSize(width, height);
		m_projection = glm::ortho(0.0f, (float)width, (float)height, 0.0f, -1.0f, 1.0f);

		// Create debug shader and buffers
		createDebugShader();
		setupDebugBuffers();

		E2_LOG(Log, "OpenGL Renderer initialized");
	}

	void initSDL2()
	{
		m_sdlRenderer = SDL_CreateRenderer(
			static_cast<SDL_Window*>(m_window->getWindow()),
			-1,
			SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
		);

		if (!m_sdlRenderer)
		{
			throw EngineError();
		}
		E2_LOG(Log, "SDL2 Renderer initialized");
	}

	void createDefaultShader()
	{
		// Use the class member variables
		const char* vertexSourcePtr = m_vertexShaderSource.c_str();
		GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertexShader, 1, &vertexSourcePtr, NULL);
		glCompileShader(vertexShader);

		// Check vertex shader compilation
		GLint success;
		char infoLog[512];
		glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
			E2_LOG(Error, "Vertex shader compilation failed:\n%s", infoLog);
			return;
		}

		// Create fragment shader
		const char* fragmentSourcePtr = m_fragmentShaderSource.c_str();
		GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragmentShader, 1, &fragmentSourcePtr, NULL);
		glCompileShader(fragmentShader);

		// Check fragment shader compilation
		glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
			E2_LOG(Error, "Fragment shader compilation failed:\n%s", infoLog);
			return;
		}

		// Create shader program
		m_defaultShaderProgram = glCreateProgram();
		glAttachShader(m_defaultShaderProgram, vertexShader);
		glAttachShader(m_defaultShaderProgram, fragmentShader);
		glLinkProgram(m_defaultShaderProgram);

		// Check program linking
		glGetProgramiv(m_defaultShaderProgram, GL_LINK_STATUS, &success);
		if (!success) {
			glGetProgramInfoLog(m_defaultShaderProgram, 512, NULL, infoLog);
			E2_LOG(Error, "Shader program linking failed:\n%s", infoLog);
			return;
		}

		// Cache all uniform locations after successful linking
		m_modelLoc = glGetUniformLocation(m_defaultShaderProgram, "model");
		m_projectionLoc = glGetUniformLocation(m_defaultShaderProgram, "projection");
		m_textureLoc = glGetUniformLocation(m_defaultShaderProgram, "mainTexture");
		m_colorLoc = glGetUniformLocation(m_defaultShaderProgram, "color");

		// Log uniform locations
		E2_LOG(Log, "Uniform locations cached - model: %d, proj: %d, tex: %d, color: %d",
			m_modelLoc, m_projectionLoc, m_textureLoc, m_colorLoc);

		// Setup projection matrix
		int width = 640;  // Your window width
		int height = 480; // Your window height
		m_projection = glm::ortho(0.0f, static_cast<float>(width),
			static_cast<float>(height), 0.0f, -1.0f, 1.0f);

		// Set initial projection matrix
		glUseProgram(m_defaultShaderProgram);
		if (m_projectionLoc != -1) {
			glUniformMatrix4fv(m_projectionLoc, 1, GL_FALSE, glm::value_ptr(m_projection));
			GLenum error = glGetError();
			if (error != GL_NO_ERROR) {
				E2_LOG(Error, "Error setting initial projection matrix: %d", error);
			}
		}

		// Cleanup shaders
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);

		E2_LOG(Log, "Shader program created successfully: %u", m_defaultShaderProgram);
	}

	void createDebugShader()
	{
		// Load debug shader sources from files with correct path
		m_debugVertexShaderSource = loadShaderFromFile("../Engine2000/Shaders/debugVertexShader.glsl");
		m_debugFragmentShaderSource = loadShaderFromFile("../Engine2000/Shaders/debugFragmentShader.glsl");

		// Create vertex shader
		const char* debugVertexSourcePtr = m_debugVertexShaderSource.c_str();
		GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertexShader, 1, &debugVertexSourcePtr, NULL);
		glCompileShader(vertexShader);
		checkShaderCompilation(vertexShader, "debug vertex");

		// Create fragment shader
		const char* debugFragmentSourcePtr = m_debugFragmentShaderSource.c_str();
		GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragmentShader, 1, &debugFragmentSourcePtr, NULL);
		glCompileShader(fragmentShader);
		checkShaderCompilation(fragmentShader, "debug fragment");

		// Create shader program
		m_debugShaderProgram = glCreateProgram();
		glAttachShader(m_debugShaderProgram, vertexShader);
		glAttachShader(m_debugShaderProgram, fragmentShader);
		glLinkProgram(m_debugShaderProgram);
		checkProgramLinking(m_debugShaderProgram);

		m_debugModelLoc = glGetUniformLocation(m_debugShaderProgram, "model");
		m_debugProjLoc = glGetUniformLocation(m_debugShaderProgram, "projection");

		if (m_debugModelLoc == -1 || m_debugProjLoc == -1) {
			E2_LOG(Error, "Failed to get uniform locations for debug shader. Model: %d, Proj: %d",
				m_debugModelLoc, m_debugProjLoc);
		}

		// Also verify the shader is using the projection matrix
		glUseProgram(m_debugShaderProgram);
		glUniformMatrix4fv(m_debugProjLoc, 1, GL_FALSE, glm::value_ptr(m_projection));
		glUseProgram(0);

		E2_LOG(Log, "Debug shader created with uniforms - Model: %d, Proj: %d",
			m_debugModelLoc, m_debugProjLoc);

		// Clean up
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);

		// Get uniform locations for debug shader
		m_debugModelLoc = glGetUniformLocation(m_debugShaderProgram, "model");
		m_debugProjLoc = glGetUniformLocation(m_debugShaderProgram, "projection");

		E2_LOG(Log, "Debug shader program created: %u", m_debugShaderProgram);
	}

	void checkShaderCompilation(GLuint shader, const char* type)
	{
		GLint success;
		char infoLog[512];
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(shader, 512, NULL, infoLog);
			E2_LOG(Error, "Shader compilation failed (%s): %s", type, infoLog);
			throw EngineError("Shader compilation failed");
		}
	}

	void checkProgramLinking(GLuint program)
	{
		GLint success;
		char infoLog[512];
		glGetProgramiv(program, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(program, 512, NULL, infoLog);
			E2_LOG(Error, "Shader program linking failed: %s", infoLog);
			throw EngineError("Shader program linking failed");
		}
	}

	void setupSpriteBuffers()
	{
		float vertices[] = {
			// pos      // tex
			0.0f, 1.0f, 0.0f, 0.0f,  // top left
			1.0f, 0.0f, 1.0f, 1.0f,  // bottom right
			0.0f, 0.0f, 0.0f, 1.0f,  // bottom left

			0.0f, 1.0f, 0.0f, 0.0f,  // top left
			1.0f, 1.0f, 1.0f, 0.0f,  // top right
			1.0f, 0.0f, 1.0f, 1.0f   // bottom right
		};

		// Generate and bind VAO first
		glGenVertexArrays(1, &m_spriteVAO);
		glBindVertexArray(m_spriteVAO);

		// Generate and bind VBO
		glGenBuffers(1, &m_spriteVBO);
		glBindBuffer(GL_ARRAY_BUFFER, m_spriteVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		// Position attribute
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		// Texture coord attribute
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
		glEnableVertexAttribArray(1);

		// Unbind VBO and VAO
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		E2_LOG(Log, "Sprite buffers created - VAO: %u, VBO: %u", m_spriteVAO, m_spriteVBO);
	}

	void setupDebugBuffers()
	{
		// Create VAO and VBO for debug rendering
		glGenVertexArrays(1, &m_debugVAO);
		glGenBuffers(1, &m_debugVBO);

		glBindVertexArray(m_debugVAO);
		glBindBuffer(GL_ARRAY_BUFFER, m_debugVBO);

		// We'll update the buffer data dynamically in drawDebugRect
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 24, nullptr, GL_DYNAMIC_DRAW);

		// Position attribute
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		// Color attribute
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(2 * sizeof(float)));
		glEnableVertexAttribArray(1);

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		E2_LOG(Log, "Debug buffers created - VAO: %u, VBO: %u", m_debugVAO, m_debugVBO);
	}

public:
	void clear()
	{
		if (m_useOpenGL)
		{
			glClear(GL_COLOR_BUFFER_BIT);
		}
		else
		{
			SDL_RenderClear(m_sdlRenderer);
		}
	}

	void present()
	{
		if (m_useOpenGL)
		{
			SDL_GL_SwapWindow(static_cast<SDL_Window*>(m_window->getWindow()));
		}
		else
		{
			SDL_RenderPresent(m_sdlRenderer);
		}
	}

	void setDrawColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
	{
		if (m_useOpenGL)
		{
			glClearColor(r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f);
		}
		else
		{
			SDL_SetRenderDrawColor(m_sdlRenderer, r, g, b, a);
		}
	}

	void* getRenderer() const
	{
		return m_useOpenGL ? (void*)m_glContext : (void*)m_sdlRenderer;
	}

	bool isOpenGL() const
	{
		return m_useOpenGL;
	}

	void drawTextureGL(GLuint textureId, const Vector4D& texCoords, const Vector4D& screenPos)
	{
		// First verify the texture exists
		if (textureId == 0)
		{
			E2_LOG(Error, "Trying to draw invalid texture (ID: 0)");
			return;
		}

		glUseProgram(m_defaultShaderProgram);
		
		// Set color uniform (white by default)
		glUniform4f(m_colorLoc, 1.0f, 1.0f, 1.0f, 1.0f);
		
		// Set up model matrix
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(screenPos.x, screenPos.y, 0.0f));
		model = glm::scale(model, glm::vec3(screenPos.w, screenPos.h, 1.0f));

		// Set uniforms
		glUniformMatrix4fv(m_modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(m_projectionLoc, 1, GL_FALSE, glm::value_ptr(m_projection));

		// Make sure the proper texture unit is active
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureId);
		glUniform1i(m_textureLoc, 0);  // Tell shader to use texture unit 0

		float vertices[] = {
			// pos      // tex
			0.0f, 0.0f, texCoords.x, texCoords.y,                    // bottom left
			1.0f, 0.0f, texCoords.x + texCoords.w, texCoords.y,      // bottom right
			0.0f, 1.0f, texCoords.x, texCoords.y + texCoords.h,      // top left
			1.0f, 1.0f, texCoords.x + texCoords.w, texCoords.y + texCoords.h  // top right
		};

		glBindVertexArray(m_spriteVAO);
		glBindBuffer(GL_ARRAY_BUFFER, m_spriteVBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

		// Draw as two triangles
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		
		// Clean up
		glBindVertexArray(0);
		glBindTexture(GL_TEXTURE_2D, 0);
		glUseProgram(0);
	}

	void drawRect(const Vector4D& rect, const Vector4D& color)
	{
		if (m_useOpenGL) {
			// Color is already in 0-1 range for OpenGL
			drawDebugRect(rect, color, false);
		}
		else {
			SDL_Rect sdlRect = {
				static_cast<int>(rect.x),
				static_cast<int>(rect.y),
				static_cast<int>(rect.w),
				static_cast<int>(rect.h)
			};
			// Color is in 0-255 range for SDL2
			SDL_SetRenderDrawColor(m_sdlRenderer,
				static_cast<uint8_t>(color.x),
				static_cast<uint8_t>(color.y),
				static_cast<uint8_t>(color.w),
				static_cast<uint8_t>(color.h));
			SDL_RenderDrawRect(m_sdlRenderer, &sdlRect);
		}
	}


	void fillRect(const Vector4D& rect, const Vector4D& color)
	{
		if (m_useOpenGL) {
			drawDebugRect(rect, color, true); // true = filled
		}
		else {
			SDL_Rect sdlRect = {
				static_cast<int>(rect.x),
				static_cast<int>(rect.y),
				static_cast<int>(rect.w),
				static_cast<int>(rect.h)
			};
			SDL_SetRenderDrawColor(m_sdlRenderer, color.x, color.y, color.w, color.h);
			SDL_RenderFillRect(m_sdlRenderer, &sdlRect);
		}
	}

	void drawDebugRect(const Vector4D& rect, const Vector4D& color, bool filled)
	{
		glUseProgram(m_debugShaderProgram);

		float normalizedColor[] = {
			color.x / 255.0f,
			color.y / 255.0f,
			color.w / 255.0f,
			color.h / 255.0f
		};

		glm::mat4 model = glm::mat4(1.0f);
		glUniformMatrix4fv(m_debugModelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(m_debugProjLoc, 1, GL_FALSE, glm::value_ptr(m_projection));

		float vertices[] = {
			rect.x, rect.y,       normalizedColor[0], normalizedColor[1], normalizedColor[2], normalizedColor[3],
			rect.x + rect.w, rect.y,   normalizedColor[0], normalizedColor[1], normalizedColor[2], normalizedColor[3],
			rect.x + rect.w, rect.y + rect.h, normalizedColor[0], normalizedColor[1], normalizedColor[2], normalizedColor[3],
			rect.x, rect.y + rect.h,   normalizedColor[0], normalizedColor[1], normalizedColor[2], normalizedColor[3]
		};

		glBindVertexArray(m_debugVAO);
		glBindBuffer(GL_ARRAY_BUFFER, m_debugVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

		glDrawArrays(filled ? GL_TRIANGLE_FAN : GL_LINE_LOOP, 0, 4);

		glBindVertexArray(0);
		glUseProgram(0);
	}
};

// Singleton instance and public method implementations
Renderer& Renderer::Instance()
{
	static Renderer instance;
	return instance;
}

Renderer::Renderer() : pimpl(new RendererImpl()) {}
Renderer::~Renderer() { delete pimpl; }

void Renderer::init(Window* window, bool useOpenGL) { pimpl->init(window, useOpenGL); }

void Renderer::cleanup() { pimpl->cleanup(); }

void* Renderer::getRenderer() const { return pimpl->getRenderer(); }

bool Renderer::isOpenGL() const { return pimpl->isOpenGL(); }

void Renderer::clear() { pimpl->clear(); }

void Renderer::present() { pimpl->present(); }

void Renderer::setDrawColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
	pimpl->setDrawColor(r, g, b, a);
}

void Renderer::drawTextureGL(unsigned int textureId, const Vector4D& texCoords, const Vector4D& screenPos)
{
	pimpl->drawTextureGL(textureId, texCoords, screenPos);
}

void Renderer::drawRect(const Vector4D& rect, const Vector4D& color)
{
	pimpl->drawRect(rect, color);
}

void Renderer::fillRect(const Vector4D& rect, const Vector4D& color)
{
	pimpl->fillRect(rect, color);
}
