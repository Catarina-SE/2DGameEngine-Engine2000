#pragma once

#include <SDL2/SDL.h>
#include "Vector4D.h"

struct SDL_Rect;

class Texture {
private:
	class TextureImpl;
	TextureImpl* pimpl;

public:
    Texture();
    ~Texture();

    void* loadFromFile(const char* filePath);
    void draw(const Vector4D& srcRect, const Vector4D& dstRect, SDL_RendererFlip flip = SDL_FLIP_NONE);

    // Getters
    void* getTexture() const;
	int getWidth() const;
	int getHeight() const;
};

