#pragma once

#include "audio/IAudioPlayer.hpp"
#include <SDL2/SDL_mixer.h>
#include <unordered_map>
#include <string>

namespace tank {

/**
 * @brief SDL_mixer implementation of IAudioPlayer
 */
class SDLAudioPlayer : public IAudioPlayer {
public:
    SDLAudioPlayer();
    ~SDLAudioPlayer() override;

    bool initialize() override;
    void shutdown() override;

    void playSound(SoundId id) override;
    void stopSound(SoundId id) override;

    void playMusic(const std::string& path, bool loop = true) override;
    void stopMusic() override;
    void pauseMusic() override;
    void resumeMusic() override;

    void setMasterVolume(float volume) override;
    void setSoundVolume(float volume) override;
    void setMusicVolume(float volume) override;

    float getMasterVolume() const override { return masterVolume_; }

private:
    bool initialized_;
    float masterVolume_;
    float soundVolume_;
    float musicVolume_;

    std::unordered_map<SoundId, Mix_Chunk*> soundCache_;
    Mix_Music* currentMusic_;

    std::string getSoundPath(SoundId id) const;
    void loadSounds();
    void clearSoundCache();
};

} // namespace tank
