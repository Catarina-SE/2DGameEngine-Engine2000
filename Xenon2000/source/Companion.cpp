#include "Companion.h"
#include "Player.h"
#include "PlayerProjectile.h"
#include "Engine2000/Level.h"
#include "Engine2000/E2Log.h"

// Initialize static constants
const float Companion::DAMAGE_DISPLAY_DURATION = 0.25f;
const float Companion::DEATH_FRAME_TIME = 0.1f;

Companion::Companion(Player* player, bool isLeftSide, float projectileSpeed)
	: m_player(player)
	, m_projectileSpeed(projectileSpeed)
	, m_isLeftSide(isLeftSide)
	, m_horizontalOffset(50.0f)
	, m_verticalOffset(20.0f)
	, m_isDying(false)
	, m_deathAnimFrame(0)
	, m_deathTimer(0.0f)
	, m_isBeingRemoved(false)
	, m_damageDisplayTime(0.0f)
	, m_isShowingDamage(false)
	, m_lastNormalFrame(0)
	, m_eventHandler(nullptr)
	, m_currentProjectileType(PlayerProjectile::ProjectileType::Light)
{
	// Initialize IDamageable variables
	m_maxHealth = 30.0f;
	m_health = m_maxHealth;
	m_isAlive = true;

	// Create and set up sprite component
	m_sprite = addComponent<SpriteComponent>();
	m_sprite->setAnimatedTexture("graphics/clone.bmp", 4, 5);
	m_sprite->setAnimationMode(SpriteComponent::LOOP);
	m_sprite->setFrameDelay(0.1f);
	m_sprite->setFrameRange(0, NORMAL_FRAMES - 1);

	// Create physics component
	m_physics = addComponent<PhysicsComponent>();
	m_physics->setPosition(player->getTransform()->getPosition());
}

void Companion::init()
{
	GameObject::init();

	m_physics->init(getLevel()->getPhysicsWorld(), true, false);
	m_physics->createCollisionShapeFromSprite();
	m_physics->setLayer("Player");
	m_physics->setDebugDraw(false);	// Debug Draw
	m_physics->setDebugColor(PhysicsComponent::DebugColor::Blue);
}

void Companion::update(float deltaTime)
{
	if (!m_isAlive) {
		updateDeathAnimation(deltaTime);
		return;
	}

	if (m_isShowingDamage)
	{
		m_damageDisplayTime -= deltaTime;
		if (m_damageDisplayTime <= 0)
		{
			m_isShowingDamage = false;
			m_sprite->setCurrentFrame(m_lastNormalFrame);
			m_sprite->setAnimationMode(SpriteComponent::LOOP);
		}
	}

	if (!m_player) return;

	// Update position relative to player
	updatePosition(deltaTime);

	GameObject::update(deltaTime);
}

void Companion::updateDeathAnimation(float deltaTime)
{
	if (!m_isDying || m_isBeingRemoved) return;

	m_deathTimer += deltaTime;
	if (m_deathTimer >= DEATH_FRAME_TIME) {
		m_deathTimer = 0.0f;
		if (m_deathAnimFrame < 2) {
			m_deathAnimFrame++;
			m_sprite->setCurrentFrame(FIRST_DEATH_FRAME + m_deathAnimFrame);
		}
		else {
			m_isBeingRemoved = true;
			if (auto level = getLevel()) {
				level->removeGameObject(this);
			}
		}
	}
}

void Companion::updatePosition(float deltaTime)
{
	if (!m_player || !m_physics) return;

	// Get player's position and dimensions
	auto playerPos = m_player->getTransform()->getPosition();
	auto playerSprite = m_player->getSprite();
	float playerWidth = playerSprite->getFrameWidth();
	float playerHeight = playerSprite->getFrameHeight();

	// Calculate player's center point
	float playerCenterX = playerPos.x + (playerWidth / 2.0f);
	float playerCenterY = playerPos.y + (playerHeight / 2.0f);

	// Get companion dimensions
	float companionWidth = m_sprite->getFrameWidth();
	float companionHeight = m_sprite->getFrameHeight();

	// Calculate companion target position relative to player's center
	float targetX = playerCenterX + (m_isLeftSide ? -m_horizontalOffset : m_horizontalOffset);
	targetX -= (companionWidth / 2.0f);

	float screenHeight = getLevel()->getScreenHeight();
	float targetY;

	if (playerCenterY < screenHeight * 0.3f)  // Near top
	{
		targetY = playerCenterY + m_verticalOffset;
	}
	else if (playerCenterY > screenHeight * 0.7f)  // Near bottom
	{
		targetY = playerCenterY - m_verticalOffset;
	}
	else
	{
		targetY = playerCenterY;
	}
	targetY -= (companionHeight / 2.0f);

	// Current position
	auto currentPosition = getTransform()->getPosition();

	// Interpolation factor
	float speed = 5.0f; // Adjust this value to control the speed of movement
	float interpolationFactor = speed * deltaTime;

	// Linearly interpolate the position
	float newX = currentPosition.x + (targetX - currentPosition.x);
	float newY = currentPosition.y + (targetY - currentPosition.y) * interpolationFactor;

	// Set the new position
	m_physics->setPosition(Vector2D(newX, newY));
}

void Companion::shoot()
{
	if (!m_isAlive || !getLevel()) return;

	auto projectile = getLevel()->createGameObject<PlayerProjectile>();
	auto pos = getTransform()->getPosition();

	float projectileX = pos.x + (m_sprite->getFrameWidth() * 0.5f);
	float projectileY = pos.y - (m_sprite->getFrameHeight() * 0.2f);

	projectile->setPosition(projectileX, projectileY);
	projectile->setSpeed(m_projectileSpeed);

	if (auto projectilePhysics = projectile->getComponent<PhysicsComponent>())
	{
		projectilePhysics->setImmunity(0.05f);
	}
}

void Companion::takeDamage(float amount)
{
	if (!m_isAlive) return;

	m_health = std::max(0.0f, m_health - amount);

	// Show damage frame
	if (!m_isDying) {
		m_lastNormalFrame = m_sprite->getCurrentFrame();
		m_sprite->setAnimationMode(SpriteComponent::CONTROLLED);
		m_sprite->setCurrentFrame(DAMAGE_FRAME);
		m_isShowingDamage = true;
		m_damageDisplayTime = DAMAGE_DISPLAY_DURATION;
	}

	if (m_health <= 0) {
		die();
	}
}

void Companion::upgradeWeapon()
{
	if (m_currentProjectileType < PlayerProjectile::ProjectileType::Heavy)
	{
		m_currentProjectileType = static_cast<PlayerProjectile::ProjectileType>(static_cast<int>(m_currentProjectileType) + 1);
	}
}

void Companion::die()
{
	if (m_isDying) return;
	
	m_isAlive = false;
	m_isDying = true;
	m_deathAnimFrame = 0;
	m_deathTimer = 0.0f;
	m_sprite->setAnimationMode(SpriteComponent::CONTROLLED);
	m_sprite->setCurrentFrame(FIRST_DEATH_FRAME);

	// Notify handler about death
	if (m_eventHandler) {
		m_eventHandler->onCompanionDeath(m_isLeftSide);
	}
}