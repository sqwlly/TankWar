#pragma once

#include "rendering/IRenderer.hpp"
#include "utils/Constants.hpp"

namespace tank {

/**
 * @brief Renderable interface - for entities that can be drawn (ISP)
 */
class IRenderable {
public:
    virtual ~IRenderable() = default;

    virtual void render(IRenderer& renderer) = 0;
    virtual RenderLayer getRenderLayer() const = 0;
};

} // namespace tank
