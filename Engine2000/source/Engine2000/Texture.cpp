#include "Texture.h"
#include "Renderer.h"
#include "EngineError.h"
#include "E2Log.h"

#include <glad/glad.h>
#include <unordered_map>
#include <memory>
#include <string>
#include "stb_image.h"

class Texture::TextureImpl
{
private:
	// Backend-agnostic members
	int m_width;
	int m_height;
	bool m_useOpenGL;

	// SDL2 specific members
	std::shared_ptr<SDL_Texture> m_sdlTexture;
	SDL_Renderer* m_sdlRenderer;

	// OpenGL specific members
	GLuint m_glTextureId;

	// Shared texture cache for both backends
	static std::unordered_map<std::string, std::shared_ptr<void>> s_textureCache;

public:
	TextureImpl()
		: m_width(0)
		, m_height(0)
		, m_useOpenGL(false)
		, m_sdlTexture(nullptr)
		, m_sdlRenderer(nullptr)
		, m_glTextureId(0)
	{
		// Check renderer type
		m_useOpenGL = Renderer::Instance().isOpenGL();
		if (!m_useOpenGL)
		{
			m_sdlRenderer = static_cast<SDL_Renderer*>(Renderer::Instance().getRenderer());
		}
	}

	~TextureImpl()
	{
		if (m_useOpenGL && m_glTextureId)
		{
			glDeleteTextures(1, &m_glTextureId);
		}
	}

	void* loadFromFile(const char* filePath)
	{
		std::string path(filePath);

		// Check cache first
		auto it = s_textureCache.find(path);
		if (it != s_textureCache.end())
		{
			if (m_useOpenGL)
			{
				m_glTextureId = *static_cast<GLuint*>(it->second.get());
				return &m_glTextureId;
			}
			m_sdlTexture = std::static_pointer_cast<SDL_Texture>(it->second);
			return m_sdlTexture.get();
		}

		SDL_Surface* surface = nullptr;

		// Get file extension
		std::string ext = path.substr(path.find_last_of(".") + 1);

		// Load based on file type
		if (ext == "bmp" || ext == "BMP")
		{
			surface = SDL_LoadBMP(filePath);
			if (surface && SDL_SetColorKey(surface, SDL_TRUE, SDL_MapRGB(surface->format, 255, 0, 255)) < 0)
			{
				SDL_FreeSurface(surface);
				throw EngineError("Failed to set color key");
			}
		}
		else
		{
			// For PNG, JPG, TGA
			int width, height, channels;
			stbi_set_flip_vertically_on_load(m_useOpenGL);
			unsigned char* data = stbi_load(filePath, &width, &height, &channels, 4); // Force RGBA

			if (data)
			{
				surface = SDL_CreateRGBSurface(0, width, height, 32,
					0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000);

				if (surface)
				{
					SDL_LockSurface(surface);
					memcpy(surface->pixels, data, width * height * 4);
					SDL_UnlockSurface(surface);
				}
				stbi_image_free(data);
			}
		}

		if (!surface) throw EngineError("Failed to load image");

		m_width = surface->w;
		m_height = surface->h;

		void* result = m_useOpenGL ? createGLTexture(surface) : createSDLTexture(surface);
		SDL_FreeSurface(surface);
		return result;
	}

	void draw(const Vector4D& srcRect, const Vector4D& dstRect, SDL_RendererFlip flip)
	{
		if (m_useOpenGL)
		{
			drawWithOpenGL(srcRect, dstRect, flip);
		}
		else
		{
			drawWithSDL(srcRect, dstRect, flip);
		}
	}

private:
	void* createGLTexture(SDL_Surface* surface)
	{
		GLuint textureId;
		glGenTextures(1, &textureId);
		glBindTexture(GL_TEXTURE_2D, textureId);

		// Log texture creation
// 		E2_LOG(Log, "Creating GL texture %u from surface %dx%d",
// 			textureId, surface->w, surface->h);

		// Set texture parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		// Convert surface to RGBA format if needed
		SDL_Surface* rgbaSurface = SDL_ConvertSurfaceFormat(surface,
			SDL_PIXELFORMAT_RGBA32, 0);

		if (!rgbaSurface) {
			E2_LOG(Error, "Failed to convert surface to RGBA");
			return nullptr;
		}

		// Upload texture data
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
			rgbaSurface->w, rgbaSurface->h, 0,
			GL_RGBA, GL_UNSIGNED_BYTE, rgbaSurface->pixels);

		GLenum error = glGetError();
		if (error != GL_NO_ERROR) {
			E2_LOG(Error, "Error creating texture: %d", error);
		}

		SDL_FreeSurface(rgbaSurface);

		m_glTextureId = textureId;
		return &m_glTextureId;
	}

	void* createSDLTexture(SDL_Surface* surface)
	{
		SDL_Texture* newTexture = SDL_CreateTextureFromSurface(m_sdlRenderer, surface);
		if (!newTexture)
		{
			throw EngineError();
		}

		m_sdlTexture = std::shared_ptr<SDL_Texture>(newTexture, SDL_DestroyTexture);
		s_textureCache[std::to_string(surface->w)] = m_sdlTexture;

		return m_sdlTexture.get();
	}

	void drawWithOpenGL(const Vector4D& srcRect, const Vector4D& dstRect, SDL_RendererFlip flip)
	{
		// Calculate normalized texture coordinates
		float texLeft = srcRect.x / static_cast<float>(m_width);
		float texRight = (srcRect.x + srcRect.w) / static_cast<float>(m_width);
		float texTop = srcRect.y / static_cast<float>(m_height);
		float texBottom = (srcRect.y + srcRect.h) / static_cast<float>(m_height);

		if (flip & SDL_FLIP_HORIZONTAL)
		{
			std::swap(texLeft, texRight);
		}
		if (flip & SDL_FLIP_VERTICAL)
		{
			std::swap(texTop, texBottom);
		}

		// Create texture coordinates vector
		Vector4D texCoords(texLeft, texTop, texRight - texLeft, texBottom - texTop);

		// Draw using the renderer
		Renderer::Instance().drawTextureGL(m_glTextureId, texCoords, dstRect);
	}

	void drawWithSDL(const Vector4D& srcRect, const Vector4D& dstRect, SDL_RendererFlip flip)
	{
		// Convert Vector4D to SDL_Rect
		SDL_Rect sdlSrcRect = {
			static_cast<int>(srcRect.x),
			static_cast<int>(srcRect.y),
			static_cast<int>(srcRect.w),
			static_cast<int>(srcRect.h)
		};

		SDL_Rect sdlDstRect = {
			static_cast<int>(dstRect.x),
			static_cast<int>(dstRect.y),
			static_cast<int>(dstRect.w),
			static_cast<int>(dstRect.h)
		};

		SDL_RenderCopyEx(m_sdlRenderer, m_sdlTexture.get(),
			&sdlSrcRect, &sdlDstRect, 0.0, nullptr, flip);
	}

public:
	void* getTexture() const
	{
		return m_useOpenGL ? (void*)&m_glTextureId : (void*)m_sdlTexture.get();
	}

	int getWidth() const { return m_width; }
	int getHeight() const { return m_height; }
};

// Initialize static member
std::unordered_map<std::string, std::shared_ptr<void>> Texture::TextureImpl::s_textureCache;

// Texture.cpp - Implementation of public methods
Texture::Texture() : pimpl(new TextureImpl()) {}
Texture::~Texture() { delete pimpl; }

void* Texture::loadFromFile(const char* filePath)
{
	void* texture = pimpl->loadFromFile(filePath);
	if (texture == nullptr) {
		E2_LOG(Warning, "Failed to load texture: %s", filePath);
	}
	return texture;
}

void Texture::draw(const Vector4D& srcRect, const Vector4D& dstRect, SDL_RendererFlip flip)
{
	pimpl->draw(srcRect, dstRect, flip);
}

void* Texture::getTexture() const
{
	return pimpl->getTexture();
}

int Texture::getWidth() const
{
	return pimpl->getWidth();
}

int Texture::getHeight() const
{
	return pimpl->getHeight();
}
