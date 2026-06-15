// =============================================================================
// Kivo Music Qt - audio_core_worker_snapshot.hpp
// Lightweight snapshot payload signaled from worker thread to UI controller
// =============================================================================

#pragma once

#include <QString>
#include <cstdint>

namespace kivo::qt::audio_bridge::worker {

struct WorkerPlaybackSnapshot {
    bool valid = false;
    bool isPlaying = false;
    double progress = 0.0;
    QString elapsedText;
    QString durationText;
};

struct WorkerFormatSnapshot {
    bool valid = false;
    QString formatText;
    int sampleRate = 0;
    int channels = 0;
};

struct WorkerTruthSnapshot {
    bool valid = false;
    QString bitPerfectStatus;
};

} // namespace kivo::qt::audio_bridge::worker
