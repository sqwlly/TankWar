#pragma once

#include "utils/Constants.hpp"
#include <string>

namespace tank {

/**
 * @brief Abstract audio player interface (DIP)
 */
class IAudioPlayer {
public:
    virtual ~IAudioPlayer() = default;

    // Lifecycle
    virtual bool initialize() = 0;
    virtual void shutdown() = 0;

    // Sound effects
    virtual void playSound(SoundId id) = 0;
    virtual void stopSound(SoundId id) = 0;

    // Music
    virtual void playMusic(const std::string& path, bool loop = true) = 0;
    virtual void stopMusic() = 0;
    virtual void pauseMusic() = 0;
    virtual void resumeMusic() = 0;

    // Volume control
    virtual void setMasterVolume(float volume) = 0;
    virtual void setSoundVolume(float volume) = 0;
    virtual void setMusicVolume(float volume) = 0;

    virtual float getMasterVolume() const = 0;
};

} // namespace tank
