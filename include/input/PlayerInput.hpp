#pragma once

namespace tank {

/**
 * @brief Logical player input for a single frame.
 */
struct PlayerInput {
    bool up = false;
    bool down = false;
    bool left = false;
    bool right = false;
    bool fire = false;
};

} // namespace tank

