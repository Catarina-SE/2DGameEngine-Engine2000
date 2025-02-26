#include "PhysicsLayerManager.h"
#include "E2Log.h"

PhysicsLayerManager* PhysicsLayerManager::s_instance = nullptr;

PhysicsLayerManager& PhysicsLayerManager::getInstance() {
	if (!s_instance) {
		s_instance = new PhysicsLayerManager();
	}
	return *s_instance;
}

void PhysicsLayerManager::destroy() {
	delete s_instance;
	s_instance = nullptr;
}

PhysicsLayerManager::PhysicsLayerManager() {
	// Set up built-in layers
	m_layerNames[0] = "Default";      // For objects with no specific layer needs
	m_layerNames[1] = "Background";   // Non-colliding background elements
	m_layerNames[2] = "Environment";  // Static world elements (walls, platforms)
	m_layerNames[3] = "Player";       // Player characters
	m_layerNames[4] = "Enemy";        // Enemy characters
	m_layerNames[5] = "Projectile";   // Bullets, missiles, etc.
	m_layerNames[6] = "Trigger";      // Trigger zones (no physical collision)
	m_layerNames[7] = "UI";           // UI elements (no physical collision)

	// Initialize layer name mapping
	for (int i = 0; i < BUILT_IN_LAYERS; ++i) {
		m_layerNameToIndex[m_layerNames[i]] = i;
	}

	// By default, everything collides with everything
	for (int i = 0; i < MAX_LAYERS; ++i) {
		for (int j = 0; j < MAX_LAYERS; ++j) {
			m_collisionMatrix[i][j] = true;
		}
	}

	initializeDefaultCollisions();
	E2_LOG(Log, "PhysicsLayerManager initialized with default layers");
}

void PhysicsLayerManager::initializeDefaultCollisions() {
	// Background doesn't collide with anything
	for (int i = 0; i < MAX_LAYERS; ++i) {
		setLayerCollision("Background", m_layerNames[i], false);
	}

	// UI doesn't collide with anything
	for (int i = 0; i < MAX_LAYERS; ++i) {
		setLayerCollision("UI", m_layerNames[i], false);
	}

	// Triggers don't physically collide but still detect overlap
	for (int i = 0; i < MAX_LAYERS; ++i) {
		setLayerCollision("Trigger", m_layerNames[i], false);
	}

	// Environment collides with everything except Background, UI, and Trigger
	setLayerCollision("Environment", "Background", false);
	setLayerCollision("Environment", "UI", false);
	setLayerCollision("Environment", "Trigger", false);
}

bool PhysicsLayerManager::createLayer(const std::string& name, int* outIndex) {
	if (m_layerNameToIndex.find(name) != m_layerNameToIndex.end()) {
		E2_LOG(Warning, "Failed to create layer '%s': name already exists", name.c_str());
		return false;
	}

	for (int i = BUILT_IN_LAYERS; i < MAX_LAYERS; ++i) {
		if (m_layerNames[i].empty()) {
			m_layerNames[i] = name;
			m_layerNameToIndex[name] = i;
			if (outIndex) *outIndex = i;
			E2_LOG(Log, "Created new layer '%s' at index %d", name.c_str(), i);
			return true;
		}
	}

	E2_LOG(Error, "Failed to create layer '%s': no free slots available", name.c_str());
	return false;
}

bool PhysicsLayerManager::renameLayer(int index, const std::string& newName) {
	if (index < BUILT_IN_LAYERS || index >= MAX_LAYERS) {
		E2_LOG(Error, "Cannot rename built-in layer or invalid index: %d", index);
		return false;
	}

	if (m_layerNameToIndex.find(newName) != m_layerNameToIndex.end()) {
		E2_LOG(Error, "Cannot rename layer: name '%s' already exists", newName.c_str());
		return false;
	}

	m_layerNameToIndex.erase(m_layerNames[index]);
	m_layerNames[index] = newName;
	m_layerNameToIndex[newName] = index;
	E2_LOG(Log, "Renamed layer at index %d to '%s'", index, newName.c_str());
	return true;
}

void PhysicsLayerManager::setLayerCollision(const std::string& layer1, const std::string& layer2, bool shouldCollide) {
	auto it1 = m_layerNameToIndex.find(layer1);
	auto it2 = m_layerNameToIndex.find(layer2);

	if (it1 != m_layerNameToIndex.end() && it2 != m_layerNameToIndex.end()) {
		int i = it1->second;
		int j = it2->second;
		m_collisionMatrix[i][j] = shouldCollide;
		m_collisionMatrix[j][i] = shouldCollide;
		E2_LOG(Log, "Set collision between '%s' and '%s' to %s",
			layer1.c_str(), layer2.c_str(), shouldCollide ? "true" : "false");
	}
	else {
		E2_LOG(Warning, "Failed to set collision: invalid layer names");
	}
}

bool PhysicsLayerManager::shouldLayersCollide(const std::string& layer1, const std::string& layer2) const {
	auto it1 = m_layerNameToIndex.find(layer1);
	auto it2 = m_layerNameToIndex.find(layer2);

	if (it1 != m_layerNameToIndex.end() && it2 != m_layerNameToIndex.end()) {
		return m_collisionMatrix[it1->second][it2->second];
	}
	return false;
}

const std::string& PhysicsLayerManager::getLayerName(int index) const {
	if (index >= 0 && index < MAX_LAYERS) {
		return m_layerNames[index];
	}
	static std::string emptyString;
	return emptyString;
}

int PhysicsLayerManager::getLayerIndex(const std::string& name) const {
	auto it = m_layerNameToIndex.find(name);
	return (it != m_layerNameToIndex.end()) ? it->second : -1;
}