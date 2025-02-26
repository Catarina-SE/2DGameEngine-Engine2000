#pragma once

#include "Core.h"
#include "E2Log.h"
#include "Vector2d.h"
#include <vector>

struct SDL_Renderer;

class GameEngine;
class GameObject;
class Input;
class PhysicsWorld;

class ENGINE2000_API Level {
public:
    enum Layer {
        BACKGROUND = 0,
        GAME,
        FOREGROUND,
        PLAYER,     // New dedicated layer for player and companions
        UI,
        TOTAL_LAYERS
    };

protected:
    struct PendingObject {
        GameObject* obj;
        Layer layer;
    };
    
	const Input& m_input;
    std::vector<GameObject*> m_layers[TOTAL_LAYERS];
    std::vector<PendingObject> m_pendingAdds;  // Change type from GameObject* to PendingObject
    std::vector<GameObject*> m_pendingRemoves;
	int m_screenWidth;
	int m_screenHeight;
    PhysicsWorld* m_physicsWorld;

public:
    Level(const Input& input, int screenWidth, int screenHeight);
    virtual ~Level();

	template<typename T, typename... Args>
	T* createGameObject(Args&&... args) {
		T* obj = new T(std::forward<Args>(args)...);
        obj->setLevel(this);
		addGameObject(obj);
		return obj;
	}

	template<typename T, typename... Args>
	T* createGameObject(Layer layer, Args&&... args) {
		T* obj = new T(std::forward<Args>(args)...);
        obj->setLevel(this);
		addGameObject(obj, layer);
		return obj;
	}

    virtual void update(float deltaTime);
	void render();

	void* getRenderer() const;

    void addGameObject(GameObject* obj, Layer layer = GAME);
    void removeGameObject(GameObject* obj);

    const Input& getInput() const { return m_input; }

    void processLists(); // Process pending additions and removals

    void setGravity(const Vector2D& gravity);

	int getScreenWidth() const { return m_screenWidth; }
	int getScreenHeight() const { return m_screenHeight; }
    PhysicsWorld* getPhysicsWorld() { return m_physicsWorld; }

};
