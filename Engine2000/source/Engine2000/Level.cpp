#include "Level.h"
#include "GameEngine.h"
#include "GameObject.h"
#include "Renderer.h"
#include "PhysicsWorld.h"
#include <algorithm>
#include <iostream>
#include <SDL2/SDL.h>

inline Level::Level(const Input& input, int screenWidth, int screenHeight)
	: m_input(input)
	, m_screenWidth(screenWidth)
	, m_screenHeight(screenHeight)
{
    // Initialize physics world with screen dimensions
    m_physicsWorld = new PhysicsWorld;
    m_physicsWorld->init(screenWidth, screenHeight, Vector2D(0.0f, 0.0f));
}

Level::~Level() {
    // Clean up pending objects
    m_pendingAdds.clear();
    m_pendingRemoves.clear();
    
    // Clean up all layers
    for (int i = 0; i < TOTAL_LAYERS; i++)
    {
        for (auto obj : m_layers[i])
        {
            if (obj!=nullptr)
            {
                delete obj;
            }
        }
        m_layers[i].clear();
    }

}

void Level::processLists() {
    // Process removals
    for (auto obj : m_pendingRemoves)
    {
        for (int i = 0; i < TOTAL_LAYERS; i++)
        {
            auto& layer = m_layers[i];
            auto it = std::find(layer.begin(), layer.end(), obj);
            if (it != layer.end())
            {
                layer.erase(it);
                break;
            }
        }
    }

    for (auto obj : m_pendingRemoves)
    {
        delete obj;
    }
    m_pendingRemoves.clear();

    // Process additions
    for (const auto& pending : m_pendingAdds)
    {
        if (pending.obj)
        {
            m_layers[pending.layer].push_back(pending.obj);
        }
    }
    m_pendingAdds.clear();
}

void Level::setGravity(const Vector2D& gravity)
{
	if (m_physicsWorld) {
		m_physicsWorld->setGravity(gravity);
	}
}

void Level::update(float deltaTime)
{
    if (m_physicsWorld) {
        m_physicsWorld->update();
    }

    // Process any pending additions/removals first
    processLists();

    // Update all objects
    for (int i = 0; i < TOTAL_LAYERS; i++) {
        auto& currentLayer = m_layers[i];
        
        // Remove null objects and update remaining ones
        currentLayer.erase(
            std::remove_if(currentLayer.begin(), currentLayer.end(),
                [deltaTime](GameObject* obj) {
                    if (obj == nullptr) return true;
                    obj->update(deltaTime);
                    return false;
                }),
            currentLayer.end()
        );
    }
}

void Level::render() {
    Renderer::Instance().clear();

    // Background color (works for both OpenGL and SDL2)
    Renderer::Instance().setDrawColor(64, 0, 64, 255);

    // Render all layers in order
    for (auto obj : m_layers[BACKGROUND]) {
        if (obj) obj->render();
    }

    for (auto obj : m_layers[GAME]) {
        if (obj) obj->render();
    }

    for (auto obj : m_layers[FOREGROUND]) {
        if (obj) obj->render();
    }

    // Add PLAYER layer rendering
    for (auto obj : m_layers[PLAYER]) {
        if (obj) obj->render();
    }

    for (auto obj : m_layers[UI]) {
        if (obj) obj->render();
    }

    Renderer::Instance().present();
}

void* Level::getRenderer() const
{
    return Renderer::Instance().getRenderer();
}

void Level::addGameObject(GameObject* obj, Layer layer) {
    if (obj)
    {
        PendingObject pending;
        pending.obj = obj;
        pending.layer = layer;
        m_pendingAdds.push_back(pending);
    }
}

void Level::removeGameObject(GameObject* obj)
{
    if (obj)
    {
        m_pendingRemoves.push_back(obj);
    }
}