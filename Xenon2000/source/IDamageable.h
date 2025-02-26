#pragma once

class IDamageable
{
protected:
    float m_maxHealth;
    float m_health;
    bool m_isAlive;
public:
    virtual ~IDamageable() = default;
    virtual void takeDamage(float amount) {}
    virtual void gainHealth(float amount) {}
    virtual float getHealth() const { return m_health; }
    virtual float getMaxHealth() const { return m_maxHealth; }
    virtual bool isAlive() const { return m_isAlive; }
};