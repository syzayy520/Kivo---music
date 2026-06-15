// =============================================================================
// Kivo Music Qt - audio_core_worker_command.hpp
// Worker command types: open/play/pause/stop/seek payloads
// =============================================================================

#pragma once

#include <QString>
#include <cstdint>

namespace kivo::qt::audio_bridge::worker {

enum class WorkerCommandKind : uint8_t {
    Initialize,
    Shutdown,
    OpenFile,
    Play,
    Pause,
    Stop,
    Seek,
    Skip,
    SetVolume,
};

struct WorkerCommand {
    WorkerCommandKind kind = WorkerCommandKind::Play;
    QString filePath;
    double seekProgress = 0.0;   // 0.0-1.0 for seekTo
    int skipSeconds = 0;         // signed for skipBy
    double volume = 0.7;         // 0.0-1.0
};

} // namespace kivo::qt::audio_bridge::worker
