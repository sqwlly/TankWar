#include "entities/base/Entity.hpp"

namespace tank {

std::atomic<int> Entity::nextId_{0};

Entity::Entity(const Vector2& position, float width, float height)
    : id_(nextId_++)
    , position_(position)
    , width_(width)
    , height_(height)
{
}

void Entity::update(float deltaTime) {
    if (!active_) return;
    onUpdate(deltaTime);
}

void Entity::render(IRenderer& renderer) {
    if (!active_) return;
    onRender(renderer);
}

} // namespace tank
