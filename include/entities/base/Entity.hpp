#pragma once

#include "entities/base/IEntity.hpp"
#include "entities/base/IRenderable.hpp"
#include "utils/Vector2.hpp"
#include "utils/Rectangle.hpp"
#include "utils/Constants.hpp"
#include <atomic>

namespace tank {

/**
 * @brief Base entity class implementing common functionality
 * Template Method Pattern: defines skeleton of algorithms
 */
class Entity : public IEntity, public IRenderable {
public:
    Entity(const Vector2& position, float width, float height);
    ~Entity() override = default;

    // IEntity implementation
    int getId() const override { return id_; }

    Vector2 getPosition() const override { return position_; }
    void setPosition(const Vector2& pos) override { position_ = pos; }

    Rectangle getBounds() const override {
        return Rectangle(position_.x, position_.y, width_, height_);
    }

    bool isActive() const override { return active_; }
    void setActive(bool active) override { active_ = active; }

    void update(float deltaTime) override;

    // IRenderable implementation
    void render(IRenderer& renderer) override;
    RenderLayer getRenderLayer() const override { return renderLayer_; }

    // Dimensions
    float getWidth() const { return width_; }
    float getHeight() const { return height_; }
    void setSize(float width, float height) { width_ = width; height_ = height; }

protected:
    // Subclass hooks (Template Method)
    virtual void onUpdate(float deltaTime) {}
    virtual void onRender(IRenderer& renderer) {}

    int id_;
    Vector2 position_;
    float width_;
    float height_;
    bool active_ = true;
    RenderLayer renderLayer_ = RenderLayer::Terrain;

private:
    static std::atomic<int> nextId_;
};

} // namespace tank
