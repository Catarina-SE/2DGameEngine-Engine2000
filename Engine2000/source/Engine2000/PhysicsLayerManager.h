#pragma once

#include "Core.h"
#include <string>
#include <array>
#include <unordered_map>

class ENGINE2000_API PhysicsLayerManager {
public:
	static constexpr int MAX_LAYERS = 32;
	static constexpr int BUILT_IN_LAYERS = 8;

private:
	std::unordered_map<std::string, int> m_layerNameToIndex;
	std::array<std::string, MAX_LAYERS> m_layerNames;
	bool m_collisionMatrix[MAX_LAYERS][MAX_LAYERS];
	static PhysicsLayerManager* s_instance;

public:
	static PhysicsLayerManager& getInstance();
	static void destroy();

	// Initialize built-in layers appropriate for 2D games
	PhysicsLayerManager();

	// Initialize default collision rules
	void initializeDefaultCollisions();

	// Layer management
	bool createLayer(const std::string& name, int* outIndex = nullptr);
	bool renameLayer(int index, const std::string& newName);

	// Collision management
	void setLayerCollision(const std::string& layer1, const std::string& layer2, bool shouldCollide);
	bool shouldLayersCollide(const std::string& layer1, const std::string& layer2) const;

	// Getters
	const std::string& getLayerName(int index) const;
	int getLayerIndex(const std::string& name) const;
};