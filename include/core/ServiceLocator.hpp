#pragma once

#include "rendering/IRenderer.hpp"
#include "input/InputManager.hpp"
#include "audio/IAudioPlayer.hpp"
#include <memory>
#include <stdexcept>

namespace tank {

/**
 * @brief Service Locator pattern for global service access
 * Provides dependency injection without tight coupling
 * (DIP - Depend on abstractions via interfaces)
 */
class ServiceLocator {
public:
    // Delete copy/move operations
    ServiceLocator() = delete;
    ServiceLocator(const ServiceLocator&) = delete;
    ServiceLocator& operator=(const ServiceLocator&) = delete;

    // Provide services
    static void provide(std::shared_ptr<IRenderer> renderer) {
        renderer_ = std::move(renderer);
    }

    static void provide(std::shared_ptr<InputManager> input) {
        input_ = std::move(input);
    }

    static void provide(std::shared_ptr<IAudioPlayer> audio) {
        audio_ = std::move(audio);
    }

    // Get services
    static IRenderer& getRenderer() {
        if (!renderer_) {
            throw std::runtime_error("Renderer service not provided");
        }
        return *renderer_;
    }

    static InputManager& getInput() {
        if (!input_) {
            throw std::runtime_error("Input service not provided");
        }
        return *input_;
    }

    static IAudioPlayer& getAudio() {
        if (!audio_) {
            throw std::runtime_error("Audio service not provided");
        }
        return *audio_;
    }

    // Check if services are available
    static bool hasRenderer() { return renderer_ != nullptr; }
    static bool hasInput() { return input_ != nullptr; }
    static bool hasAudio() { return audio_ != nullptr; }

    // Cleanup
    static void reset() {
        renderer_.reset();
        input_.reset();
        audio_.reset();
    }

private:
    static inline std::shared_ptr<IRenderer> renderer_ = nullptr;
    static inline std::shared_ptr<InputManager> input_ = nullptr;
    static inline std::shared_ptr<IAudioPlayer> audio_ = nullptr;
};

} // namespace tank
