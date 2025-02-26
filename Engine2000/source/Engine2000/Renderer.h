#pragma once

#include <cstdint>
#include "Vector4D.h"

class Window;

class Renderer {
private:
	// Singleton pattern
	Renderer();
	~Renderer();
	Renderer(const Renderer&) = delete;
	Renderer& operator=(const Renderer&) = delete;

	class RendererImpl;
	RendererImpl* pimpl;

public:
	static Renderer& Instance();

	void init(Window* window, bool useOpenGL = true);
	void cleanup();

	// Rendering methods
	void* getRenderer() const;
	bool isOpenGL() const;
	void clear();
	void present();
	void setDrawColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a);

	// OpenGL specific methods
	void drawTextureGL(unsigned int textureId, const Vector4D& texCoords, const Vector4D& screenPos);

	// Using Vector4D for both rectangle and color (x,y,w,h) and (r,g,b,a)
	void drawRect(const Vector4D& rect, const Vector4D& color);
	void fillRect(const Vector4D& rect, const Vector4D& color);
};