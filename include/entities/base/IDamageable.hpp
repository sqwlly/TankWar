#pragma once

namespace tank {

/**
 * @brief Damageable interface - for entities that can take damage (ISP)
 */
class IDamageable {
public:
    virtual ~IDamageable() = default;

    virtual int getHealth() const = 0;
    virtual int getMaxHealth() const = 0;
    virtual int getDefense() const = 0;
    virtual void setDefense(int defense) = 0;

    virtual void takeDamage(int amount) = 0;
    virtual void heal(int amount) = 0;

    virtual bool isAlive() const = 0;
    virtual void die() = 0;
};

} // namespace tank
