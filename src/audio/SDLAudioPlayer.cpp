#include "audio/SDLAudioPlayer.hpp"
#include <iostream>
#include <vector>

namespace tank {

SDLAudioPlayer::SDLAudioPlayer()
    : initialized_(false)
    , masterVolume_(1.0f)
    , soundVolume_(1.0f)
    , musicVolume_(0.7f)
    , currentMusic_(nullptr)
{
}

SDLAudioPlayer::~SDLAudioPlayer() {
    shutdown();
}

bool SDLAudioPlayer::initialize() {
    if (initialized_) return true;

    // Initialize SDL_mixer
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        std::cerr << "SDL_mixer initialization failed: " << Mix_GetError() << std::endl;
        return false;
    }

    // Allocate mixing channels
    Mix_AllocateChannels(16);

    loadSounds();
    initialized_ = true;

    std::cout << "SDL Audio Player initialized successfully" << std::endl;
    return true;
}

void SDLAudioPlayer::shutdown() {
    if (!initialized_) return;

    stopMusic();
    clearSoundCache();

    if (currentMusic_) {
        Mix_FreeMusic(currentMusic_);
        currentMusic_ = nullptr;
    }

    Mix_CloseAudio();
    initialized_ = false;
}

void SDLAudioPlayer::loadSounds() {
    // Pre-load common sounds
    std::vector<SoundId> sounds = {
        SoundId::BulletShot,
        SoundId::BulletHitSteel,
        SoundId::Explosion,
        SoundId::PlayerMove,
        SoundId::EnemyMove,
        SoundId::GetBonus,
        SoundId::BonusLife,
        SoundId::Star,
        SoundId::GameOver,
        SoundId::StageStart,
        SoundId::Pause
    };

    for (SoundId id : sounds) {
        std::string path = getSoundPath(id);
        Mix_Chunk* chunk = Mix_LoadWAV(path.c_str());
        if (chunk) {
            soundCache_[id] = chunk;
        } else {
            std::cerr << "Failed to load sound: " << path << " - " << Mix_GetError() << std::endl;
        }
    }
}

void SDLAudioPlayer::clearSoundCache() {
    for (auto& [id, chunk] : soundCache_) {
        if (chunk) {
            Mix_FreeChunk(chunk);
        }
    }
    soundCache_.clear();
}

std::string SDLAudioPlayer::getSoundPath(SoundId id) const {
    std::string basePath = "assets/audio/";

    switch (id) {
        case SoundId::BulletShot:     return basePath + "bullet_shot.wav";
        case SoundId::BulletHitSteel: return basePath + "bullet_hit_steel.wav";
        case SoundId::Explosion:      return basePath + "explosion_2.wav";
        case SoundId::PlayerMove:     return basePath + "player.move.wav";
        case SoundId::EnemyMove:      return basePath + "enemy.move.wav";
        case SoundId::GetBonus:       return basePath + "getBonus.wav";
        case SoundId::BonusLife:      return basePath + "bonus.life.wav";
        case SoundId::Star:           return basePath + "star.wav";
        case SoundId::GameOver:       return basePath + "game_over.wav";
        case SoundId::StageStart:     return basePath + "stage_start.wav";
        case SoundId::Pause:          return basePath + "menu.wav";
    }
    return basePath + "bullet_shot.wav";
}

void SDLAudioPlayer::playSound(SoundId id) {
    if (!initialized_) return;

    auto it = soundCache_.find(id);
    if (it != soundCache_.end() && it->second) {
        int volume = static_cast<int>(MIX_MAX_VOLUME * masterVolume_ * soundVolume_);
        Mix_VolumeChunk(it->second, volume);
        Mix_PlayChannel(-1, it->second, 0);
    }
}

void SDLAudioPlayer::stopSound(SoundId id) {
    // SDL_mixer doesn't easily support stopping specific sounds
    // Would need to track channels
}

void SDLAudioPlayer::playMusic(const std::string& path, bool loop) {
    if (!initialized_) return;

    stopMusic();

    currentMusic_ = Mix_LoadMUS(path.c_str());
    if (!currentMusic_) {
        std::cerr << "Failed to load music: " << path << " - " << Mix_GetError() << std::endl;
        return;
    }

    int loops = loop ? -1 : 1;
    Mix_PlayMusic(currentMusic_, loops);
    Mix_VolumeMusic(static_cast<int>(MIX_MAX_VOLUME * masterVolume_ * musicVolume_));
}

void SDLAudioPlayer::stopMusic() {
    if (Mix_PlayingMusic()) {
        Mix_HaltMusic();
    }
}

void SDLAudioPlayer::pauseMusic() {
    if (Mix_PlayingMusic()) {
        Mix_PauseMusic();
    }
}

void SDLAudioPlayer::resumeMusic() {
    if (Mix_PausedMusic()) {
        Mix_ResumeMusic();
    }
}

void SDLAudioPlayer::setMasterVolume(float volume) {
    masterVolume_ = std::max(0.0f, std::min(1.0f, volume));
    // Update music volume
    Mix_VolumeMusic(static_cast<int>(MIX_MAX_VOLUME * masterVolume_ * musicVolume_));
}

void SDLAudioPlayer::setSoundVolume(float volume) {
    soundVolume_ = std::max(0.0f, std::min(1.0f, volume));
}

void SDLAudioPlayer::setMusicVolume(float volume) {
    musicVolume_ = std::max(0.0f, std::min(1.0f, volume));
    Mix_VolumeMusic(static_cast<int>(MIX_MAX_VOLUME * masterVolume_ * musicVolume_));
}

} // namespace tank
