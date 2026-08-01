#include "utils/ProgressStore.hpp"

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <vector>

namespace tank {
namespace {

std::string trim(const std::string& value) {
    const auto first = value.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) {
        return {};
    }
    const auto last = value.find_last_not_of(" \t\r\n");
    return value.substr(first, last - first + 1);
}

int parseInt(const std::string& value, int fallback) {
    try {
        size_t consumed = 0;
        const int parsed = std::stoi(trim(value), &consumed);
        return consumed == trim(value).size() ? parsed : fallback;
    } catch (...) {
        return fallback;
    }
}

float parseFloat(const std::string& value, float fallback) {
    try {
        size_t consumed = 0;
        const std::string trimmed = trim(value);
        const float parsed = std::stof(trimmed, &consumed);
        return consumed == trimmed.size() ? parsed : fallback;
    } catch (...) {
        return fallback;
    }
}

ProgressData normalized(ProgressData progress) {
    progress.highScore = std::max(0, progress.highScore);
    progress.levelToPlay = std::clamp(progress.levelToPlay, 1, Constants::MAX_LEVEL);
    progress.masterVolume = std::clamp(progress.masterVolume, 0.0f, 1.0f);
    const int difficulty = std::clamp(static_cast<int>(progress.difficulty), 0, 2);
    progress.difficulty = static_cast<GameDifficulty>(difficulty);
    return progress;
}

bool isOwnedKey(const std::string& key) {
    return key == "highScore" || key == "levelToPlay" || key == "masterVolume" || key == "difficulty";
}

std::string valueFor(const std::string& key, const ProgressData& progress) {
    if (key == "highScore") {
        return std::to_string(progress.highScore);
    }
    if (key == "levelToPlay") {
        return std::to_string(progress.levelToPlay);
    }
    if (key == "masterVolume") {
        std::ostringstream value;
        value << std::fixed << std::setprecision(2) << progress.masterVolume;
        return value.str();
    }
    return std::to_string(static_cast<int>(progress.difficulty));
}

} // namespace

ProgressStore::ProgressStore(std::string filePath)
    : filePath_(std::move(filePath)) {
}

ProgressData ProgressStore::load() const {
    ProgressData progress;
    std::ifstream input(filePath_);
    if (!input.is_open()) {
        return progress;
    }

    std::string line;
    while (std::getline(input, line)) {
        const size_t separator = line.find('=');
        if (separator == std::string::npos) {
            continue;
        }

        const std::string key = trim(line.substr(0, separator));
        const std::string value = line.substr(separator + 1);
        if (key == "highScore") {
            progress.highScore = parseInt(value, progress.highScore);
        } else if (key == "levelToPlay") {
            progress.levelToPlay = parseInt(value, progress.levelToPlay);
        } else if (key == "masterVolume") {
            progress.masterVolume = parseFloat(value, progress.masterVolume);
        } else if (key == "difficulty") {
            progress.difficulty = static_cast<GameDifficulty>(parseInt(value, static_cast<int>(progress.difficulty)));
        }
    }

    return normalized(progress);
}

bool ProgressStore::save(const ProgressData& progressToSave) const {
    const ProgressData progress = normalized(progressToSave);
    std::vector<std::string> lines;
    std::ifstream input(filePath_);
    std::string line;
    while (std::getline(input, line)) {
        lines.push_back(line);
    }

    std::filesystem::path path(filePath_);
    if (const auto parent = path.parent_path(); !parent.empty()) {
        std::error_code error;
        std::filesystem::create_directories(parent, error);
        if (error) {
            return false;
        }
    }

    std::ofstream output(filePath_, std::ios::trunc);
    if (!output.is_open()) {
        return false;
    }

    bool writtenHighScore = false;
    bool writtenLevel = false;
    bool writtenVolume = false;
    bool writtenDifficulty = false;
    for (const std::string& originalLine : lines) {
        const size_t separator = originalLine.find('=');
        const std::string key = separator == std::string::npos ? "" : trim(originalLine.substr(0, separator));
        if (!isOwnedKey(key)) {
            output << originalLine << '\n';
            continue;
        }

        output << key << '=' << valueFor(key, progress) << '\n';
        writtenHighScore |= key == "highScore";
        writtenLevel |= key == "levelToPlay";
        writtenVolume |= key == "masterVolume";
        writtenDifficulty |= key == "difficulty";
    }

    if (!writtenHighScore) output << "highScore=" << progress.highScore << '\n';
    if (!writtenLevel) output << "levelToPlay=" << progress.levelToPlay << '\n';
    if (!writtenVolume) output << "masterVolume=" << valueFor("masterVolume", progress) << '\n';
    if (!writtenDifficulty) output << "difficulty=" << static_cast<int>(progress.difficulty) << '\n';
    return output.good();
}

} // namespace tank
