// =============================================================================
// Kivo Music Qt - playback_snapshot.hpp
// AudioCoreBridge: Playback state snapshot
// =============================================================================

#pragma once

#include "../loader/audio_core_function_resolver.hpp"
#include "../error/bridge_error.hpp"
#include <chrono>

namespace kivo::qt::audio_bridge::snapshot {

// Playback state enum
enum class PlaybackState {
    Stopped,
    Playing,
    Paused,
    Buffering,
    EndOfStream,
    Error
};

// Playback position and timing information
struct PlaybackSnapshot {
    PlaybackState state = PlaybackState::Stopped;
    std::chrono::milliseconds position{0};
    std::chrono::milliseconds duration{0};
    double progress = 0.0;  // 0.0 to 1.0
    bool hasValidPosition = false;
    uint64_t renderedFrames = 0;
    uint64_t decodedFrames = 0;
    uint64_t successfulDrains = 0;  // cumulative natural-EOS drain completions (engine lifetime)
    uint32_t renderSampleRate = 0;  // processed/negotiated render rate, Hz (0 = unknown)
    uint64_t totalFrames = 0;       // full source length in frames (0 = unknown)
    bool hasDuration = false;       // true only when a real total duration is known

    // Format as user-friendly strings
    QString positionText() const;
    QString durationText() const;

    // Check if actively playing
    bool isPlaying() const { return state == PlaybackState::Playing; }
    bool isPaused() const { return state == PlaybackState::Paused; }
    bool isEnded() const { return state == PlaybackState::EndOfStream; }
};

// Reads playback state from Host ABI snapshot
class PlaybackSnapshotReader {
public:
    // Read snapshot from engine
    static error::BridgeResult<PlaybackSnapshot> read(
        const loader::AudioCoreFunctions& functions,
        kivo_audio_handle engine,
        bool timebaseSupported = false);

private:
    static PlaybackState mapState(int abiState);
};

} // namespace kivo::qt::audio_bridge::snapshot
