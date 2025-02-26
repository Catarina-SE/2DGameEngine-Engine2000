#include "Player.h"
#include "Engine2000/Input.h"
#include "Engine2000/Level.h"
#include "Engine2000/Pawn.h"
#include "Engine2000/E2Log.h"
#include "Engine2000/PhysicsComponent.h"

const float Player::DEATH_FRAME_TIME = 0.1f;

Player::Player()
	: m_moveSpeed(1.5f)
	, m_fireDelay(0.3f)
	, m_projectileSpeed(3.0f)
	, m_timeSinceLastShot(0.0f)
	, m_currentDirection(Direction::NEUTRAL)
	, m_lastDirection(Direction::NEUTRAL)
	, m_animationTimer(0.0f)
	, m_currentAnimFrame(NEUTRAL_FRAME)
	, m_isAnimating(false)
	, m_respawnTime(0.1f)
	, m_currentProjectileType(PlayerProjectile::ProjectileType::Light)
	, m_lifes(3)
	, m_isDying(false)
	, m_deathAnimFrame(0)
	, m_deathTimer(0.0f)
{
	// Initialize IDamageable variables
	m_maxHealth = 100.0f;
	m_health = m_maxHealth;
	m_isAlive = true;
	m_healthBar = nullptr;

	setAnimatedSprite("graphics/Ship2.bmp", 7, 3);
	m_sprite->setAnimationMode(SpriteComponent::CONTROLLED);
	m_sprite->setFrameDelay(0.1f);
	m_sprite->setCurrentFrame(NEUTRAL_FRAME);
}

void Player::init()
{
	Pawn::init();

	// Create life display
	if (auto level = getLevel()) {
		m_lifeDisplay = level->createGameObject<LifeDisplay>();
		m_lifeDisplay->setLifeCount(m_lifes);
		m_healthBar = level->createGameObject<HealthBar>(Level::UI);
		m_healthBar->getTransform()->setPosition(10, level->getScreenHeight() - 30);
	}

	// Set up physics component
	auto physics = addComponent<PhysicsComponent>();
	physics->init(getLevel()->getPhysicsWorld(), true, false);
	physics->createCollisionShapeFromSprite();
	physics->setLayer("Player");
	physics->setDebugDraw(false);	// Debug Draw
	physics->setDebugColor(PhysicsComponent::DebugColor::Green);
}

void Player::update(float deltaTime)
{
	if (!m_isAlive) {
		updateDeathAnimation(deltaTime);
		return;
	}

	auto physics = getComponent<PhysicsComponent>();
	if (!physics) return;

	m_timeSinceLastShot += deltaTime;

	// Input attribution
	const bool shootInput = getInput().getKey(KeyCode::Space) ||
		getInput().getButton(Button::A);

	const bool moveLeftInput = getInput().getKey(KeyCode::Left) ||
		getInput().getKey(KeyCode::A) ||
		getInput().getButton(Button::DPadLeft);

	const bool moveRightInput = getInput().getKey(KeyCode::Right) ||
		getInput().getKey(KeyCode::D) ||
		getInput().getButton(Button::DPadRight);

	const bool moveUpInput = getInput().getKey(KeyCode::Up) ||
		getInput().getKey(KeyCode::W) ||
		getInput().getButton(Button::DPadUp);

	const bool moveDownInput = getInput().getKey(KeyCode::Down) ||
		getInput().getKey(KeyCode::S) ||
		getInput().getButton(Button::DPadDown);

	// Calculate desired velocity based on input
	Vector2D velocity(0.0f, 0.0f);

	if (moveLeftInput)
	{
		velocity.x = -m_moveSpeed;
		startDirectionalAnimation(Direction::LEFT);
	}
	else if (moveRightInput)
	{
		velocity.x = m_moveSpeed;
		startDirectionalAnimation(Direction::RIGHT);
	}
	else
	{
		startDirectionalAnimation(Direction::NEUTRAL);
	}

	if (moveUpInput)
	{
		velocity.y = -m_moveSpeed;
	}
	else if (moveDownInput)
	{
		velocity.y = m_moveSpeed;
	}

	// Update physics velocity
	physics->setVelocity(velocity);

	// Handle shooting
	if (shootInput && m_timeSinceLastShot >= m_fireDelay) {
		m_timeSinceLastShot = 0.0f;
		shoot();

		// Make companions shoot too
		if (m_companions.left && m_companions.left->isAlive()) {
			m_companions.left->shoot();
		}
		if (m_companions.right && m_companions.right->isAlive()) {
			m_companions.right->shoot();
		}
	}

	// Update animation
	updateAnimation(deltaTime);

	// Screen bounds checking
	auto pos = getTransform()->getPosition();
	float screenWidth = m_level->getScreenWidth();
	float screenHeight = m_level->getScreenHeight();
	float spriteWidth = m_sprite->getFrameWidth();
	float spriteHeight = m_sprite->getFrameHeight();

	// Clamp position to screen bounds
	if (pos.x < 0) {
		physics->setPosition(Vector2D(0, pos.y));
		physics->setVelocity(Vector2D(0, velocity.y));
	}
	else if (pos.x > screenWidth - spriteWidth) {
		physics->setPosition(Vector2D(screenWidth - spriteWidth, pos.y));
		physics->setVelocity(Vector2D(0, velocity.y));
	}

	if (pos.y < 0) {
		physics->setPosition(Vector2D(pos.x, 0));
		physics->setVelocity(Vector2D(velocity.x, 0));
	}
	else if (pos.y > screenHeight - spriteHeight) {
		physics->setPosition(Vector2D(pos.x, screenHeight - spriteHeight));
		physics->setVelocity(Vector2D(velocity.x, 0));
	}

	// Visibility timer after taking damage
	if (!m_isVisible)
	{
		m_respawnTimer -= deltaTime;
		if (m_respawnTimer <= 0)
		{
			m_isVisible = true;
			if (m_sprite)
			{
				m_sprite->setVisible(true);
			}
		}
	}

	Pawn::update(deltaTime);
}

void Player::shoot()
{
	Level* myLevel = getLevel();
	if (!myLevel) return;

	auto projectile = myLevel->createGameObject<PlayerProjectile>(m_currentProjectileType);
	auto pos = getTransform()->getPosition();

	float projectileX = pos.x + (m_sprite->getFrameWidth() * 0.4f);
	float projectileY = pos.y + (m_sprite->getFrameHeight() * 0.2f);

	projectile->setPosition(projectileX, projectileY);
	projectile->setSpeed(m_projectileSpeed);

	if (auto projectilePhysics = projectile->getComponent<PhysicsComponent>())
	{
		projectilePhysics->setImmunity(0.05f);
	}
}

void Player::updateAnimation(float deltaTime)
{
	m_animationTimer += deltaTime;

	if (m_animationTimer >= 0.05f) {
		m_animationTimer = 0.0f;

		if (m_isAnimating) {
			if (m_currentDirection == Direction::LEFT) {
				if (m_currentAnimFrame > LEFT_END_FRAME) {
					m_currentAnimFrame--;
				}
				else {
					m_isAnimating = false;
				}
			}
			else if (m_currentDirection == Direction::RIGHT) {
				if (m_currentAnimFrame < RIGHT_END_FRAME) {
					m_currentAnimFrame++;
				}
				else {
					m_isAnimating = false;
				}
			}
			else { // Returning to NEUTRAL
				m_currentAnimFrame = NEUTRAL_FRAME;
				m_isAnimating = false;
			}

			m_sprite->setCurrentFrame(m_currentAnimFrame);
		}
	}
}

void Player::updateDeathAnimation(float deltaTime)
{
	if (m_isDying) {
		m_deathTimer += deltaTime;
		if (m_deathTimer >= DEATH_FRAME_TIME)
		{
			m_deathTimer = 0.0f;
			if (m_deathAnimFrame < (LAST_DEATH_FRAME - FIRST_DEATH_FRAME))
			{
				m_deathAnimFrame++;
				m_sprite->setCurrentFrame(FIRST_DEATH_FRAME + m_deathAnimFrame);
			}
			else
			{
				// Create explosion effect here if desired
				if (auto level = getLevel())
				{
					level->removeGameObject(this);
				}
			}
		}
	}
}

void Player::startDirectionalAnimation(Direction newDirection)
{
	if (m_currentDirection != newDirection) {
		m_lastDirection = m_currentDirection;
		m_currentDirection = newDirection;
		m_isAnimating = true;

		// Set initial frame based on transition
		if (newDirection == Direction::LEFT) {
			m_currentAnimFrame = LEFT_START_FRAME;
		}
		else if (newDirection == Direction::RIGHT) {
			m_currentAnimFrame = RIGHT_START_FRAME;
		}
		else { // NEUTRAL
			m_currentAnimFrame = NEUTRAL_FRAME;
		}
	}
}

void Player::hide()
{
	m_isVisible = false;
	m_respawnTimer = m_respawnTime;
	if (m_sprite) {
		m_sprite->setVisible(false);
	}
}

void Player::takeDamage(float amount)
{
	if (!m_isAlive || m_isDying) return;

	m_health = std::max(0.0f, m_health - amount);

	// Update health bar
	if (m_healthBar) {
		m_healthBar->setHealth(m_health, m_maxHealth);
	}

	if (m_health <= 0)
	{
		m_lifes--;

		if (m_lifeDisplay) {
			m_lifeDisplay->setLifeCount(m_lifes);
		}

		if (m_lifes <= 0)
		{
			die();
		}
		else
		{
			// Reset health and make temporarily invulnerable
			m_health = m_maxHealth;
			if (m_healthBar) {
				m_healthBar->setHealth(m_health, m_maxHealth);
			}
			hide();
		}
	}
	else
	{
		hide();
	}
}

void Player::onCompanionDeath(bool isLeftSide)
{
	if (isLeftSide)
	{
		m_companions.left = nullptr;
	}
	else
	{
		m_companions.right = nullptr;
	}
}

void Player::addCompanion()
{
	if (!m_companions.right)
	{
		auto companion = getLevel()->createGameObject<Companion>(Level::PLAYER, this, false, m_projectileSpeed);
		companion->setEventHandler(this);
		m_companions.right = companion;
		return;
	}
	else if (!m_companions.left)
	{
		auto companion = getLevel()->createGameObject<Companion>(Level::PLAYER, this, true, m_projectileSpeed);
		companion->setEventHandler(this);
		m_companions.left = companion;
		return;
	}
}

void Player::removeCompanions()
{
	if (m_companions.right)
	{
		getLevel()->removeGameObject(m_companions.right);
		m_companions.right = nullptr;
	}
	if (m_companions.left)
	{
		getLevel()->removeGameObject(m_companions.left);
		m_companions.left = nullptr;
	}
}

void Player::upgradeWeapon()
{
	if (m_currentProjectileType < PlayerProjectile::ProjectileType::Heavy)
	{
		m_currentProjectileType = static_cast<PlayerProjectile::ProjectileType>(static_cast<int>(m_currentProjectileType) + 1);
	}
}

void Player::gainHealth(float amount)
{
	if (!m_isAlive) return;

	// Add health but don't exceed max
	m_health = std::min(m_health + amount, m_maxHealth);

	// Make sure player is visible
	m_isVisible = true;
	if (m_sprite)
	{
		m_sprite->setVisible(true);
	}

	// Update health bar
	if (m_healthBar)
	{
		m_healthBar->setHealth(m_health, m_maxHealth);
	}
}

void Player::die()
{
	m_isAlive = false;
	m_isDying = true;
	m_deathAnimFrame = 0;
	m_sprite->setAnimationMode(SpriteComponent::CONTROLLED);
	m_sprite->setCurrentFrame(FIRST_DEATH_FRAME);
	removeCompanions();
}