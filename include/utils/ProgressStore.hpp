#pragma once

#include "utils/Constants.hpp"
#include <string>

namespace tank {

/**
 * @brief Persisted player preferences and campaign progress.
 *
 * The store deliberately keeps the Java-era properties format. Unknown keys
 * are preserved when saving so installing the C++ game never destroys data
 * from another compatible client.
 */
struct ProgressData {
    int highScore = 0;
    int levelToPlay = 1;
    float masterVolume = 1.0f;
    GameDifficulty difficulty = GameDifficulty::Normal;
};

class ProgressStore {
public:
    explicit ProgressStore(std::string filePath = Constants::Paths::PROGRESS);

    ProgressData load() const;
    bool save(const ProgressData& progress) const;

    const std::string& getFilePath() const { return filePath_; }

private:
    std::string filePath_;
};

} // namespace tank
