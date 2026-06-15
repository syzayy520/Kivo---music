// =============================================================================
// Kivo Music Qt - playback_mode.cpp
// Playback Mode Implementation
// =============================================================================

#include "playback_mode.hpp"

namespace kivo::qt::audio_bridge {

PlaybackMode::PlaybackMode(QObject* parent) : QObject(parent) {
    randomGenerator_ = QRandomGenerator::securelySeeded();
}

void PlaybackMode::setMode(Mode mode) {
    if (currentMode_ != mode) {
        currentMode_ = mode;
        emit modeChanged();
    }
}

void PlaybackMode::toggleMode() {
    switch (currentMode_) {
        case Mode::Sequential:
            setMode(Mode::Shuffle);
            break;
        case Mode::Shuffle:
            setMode(Mode::RepeatOne);
            break;
        case Mode::RepeatOne:
            setMode(Mode::RepeatAll);
            break;
        case Mode::RepeatAll:
            setMode(Mode::Sequential);
            break;
    }
}

QString PlaybackMode::modeString() const {
    switch (currentMode_) {
        case Mode::Sequential:
            return tr("Sequential");
        case Mode::Shuffle:
            return tr("Shuffle");
        case Mode::RepeatOne:
            return tr("Repeat One");
        case Mode::RepeatAll:
            return tr("Repeat All");
        default:
            return tr("Unknown");
    }
}

int PlaybackMode::nextIndex(int currentIndex, int totalCount) {
    if (totalCount <= 0) {
        return -1;
    }

    switch (currentMode_) {
        case Mode::Sequential:
            if (currentIndex + 1 < totalCount) {
                return currentIndex + 1;
            }
            return -1; // End of queue

        case Mode::Shuffle:
            return shuffleIndex(totalCount);

        case Mode::RepeatOne:
            return currentIndex; // Same track

        case Mode::RepeatAll:
            return (currentIndex + 1) % totalCount;

        default:
            return -1;
    }
}

int PlaybackMode::previousIndex(int currentIndex, int totalCount) {
    if (totalCount <= 0) {
        return -1;
    }

    switch (currentMode_) {
        case Mode::Sequential:
        case Mode::Shuffle:
            if (currentIndex > 0) {
                return currentIndex - 1;
            }
            return -1;

        case Mode::RepeatOne:
            return currentIndex; // Same track

        case Mode::RepeatAll:
            return (currentIndex - 1 + totalCount) % totalCount;

        default:
            return -1;
    }
}

int PlaybackMode::shuffleIndex(int totalCount) {
    if (totalCount <= 0) {
        return -1;
    }
    return static_cast<int>(randomGenerator_.generate() % totalCount);
}

} // namespace kivo::qt::audio_bridge
