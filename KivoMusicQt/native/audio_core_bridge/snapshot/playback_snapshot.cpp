// =============================================================================
// Kivo Music Qt - playback_snapshot.cpp
// AudioCoreBridge: Playback snapshot implementation
// =============================================================================

#include "playback_snapshot.hpp"

namespace kivo::qt::audio_bridge::snapshot {

QString PlaybackSnapshot::positionText() const {
    if (!hasValidPosition) {
        return QString("--:--");
    }

    int totalSeconds = static_cast<int>(position.count() / 1000);
    int minutes = totalSeconds / 60;
    int seconds = totalSeconds % 60;

    return QString("%1:%2")
        .arg(minutes)
        .arg(seconds, 2, 10, QChar('0'));
}

QString PlaybackSnapshot::durationText() const {
    if (duration.count() == 0) {
        return QString("--:--");
    }

    int totalSeconds = static_cast<int>(duration.count() / 1000);
    int minutes = totalSeconds / 60;
    int seconds = totalSeconds % 60;

    return QString("%1:%2")
        .arg(minutes)
        .arg(seconds, 2, 10, QChar('0'));
}

error::BridgeResult<PlaybackSnapshot> PlaybackSnapshotReader::read(
    const loader::AudioCoreFunctions& functions,
    kivo_audio_handle engine,
    bool timebaseSupported) {

    if (!functions.getSnapshot) {
        return error::BridgeResult<PlaybackSnapshot>::failed(
            error::BridgeError{error::BridgeErrorCategory::Snapshot,
                               "getSnapshot function not available", 0});
    }

    if (engine == KIVO_AUDIO_INVALID_HANDLE) {
        return error::BridgeResult<PlaybackSnapshot>::failed(
            error::BridgeError::invalidState("Engine handle is null"));
    }

    kivo_audio_snapshot_v1 abiSnapshot{};
    abiSnapshot.struct_size = sizeof(kivo_audio_snapshot_v1);
    abiSnapshot.struct_version = KIVO_AUDIO_ABI_STRUCT_VERSION_1;
    kivo_audio_result result = functions.getSnapshot(engine, &abiSnapshot);

    if (result != KIVO_AUDIO_RESULT_OK) {
        return error::BridgeResult<PlaybackSnapshot>::failed(
            error::BridgeError{error::BridgeErrorCategory::Snapshot,
                               "Failed to read snapshot",
                               static_cast<int>(result)});
    }

    PlaybackSnapshot snapshot;
    snapshot.state = mapState(abiSnapshot.state);
    snapshot.renderedFrames = abiSnapshot.rendered_position_frames;
    snapshot.decodedFrames = abiSnapshot.decoded_frames;
    // Natural end-of-stream marker: incremented once per completed drain (the
    // ABI v1 field is already populated; the reader requests the full v1 struct).
    snapshot.successfulDrains = abiSnapshot.successful_drains;
    // Real timebase (ABI 1.1.0 tail). Gate on the capability so a mismatched
    // older core (which zero-fills the tail) reports "unknown" instead of a
    // bogus rate. rate > 0 is the universal guard for both position & duration.
    const uint32_t rate =
        timebaseSupported ? abiSnapshot.render_sample_rate : 0u;
    const uint64_t rendered = abiSnapshot.rendered_position_frames;
    snapshot.renderSampleRate = rate;

    snapshot.position = rate > 0u
        ? std::chrono::milliseconds(
              static_cast<int64_t>((rendered * 1000ull) / rate))
        : std::chrono::milliseconds(0);
    snapshot.hasValidPosition = rate > 0u && rendered > 0u;

    snapshot.totalFrames = timebaseSupported ? abiSnapshot.total_frames : 0ull;
    snapshot.hasDuration = timebaseSupported
        && abiSnapshot.total_frames_known != 0u
        && rate > 0u
        && snapshot.totalFrames > 0ull;
    snapshot.duration = snapshot.hasDuration
        ? std::chrono::milliseconds(
              static_cast<int64_t>((snapshot.totalFrames * 1000ull) / rate))
        : std::chrono::milliseconds(0);

    // Progress is rendered / true-total (not the moving decoded_frames counter,
    // which drifts). 0 when the duration is unknown.
    snapshot.progress = snapshot.hasDuration
        ? std::clamp(static_cast<double>(rendered)
                         / static_cast<double>(snapshot.totalFrames),
                     0.0, 1.0)
        : 0.0;

    return error::BridgeResult<PlaybackSnapshot>::ok(snapshot);
}

PlaybackState PlaybackSnapshotReader::mapState(int abiState) {
    switch (abiState) {
        case KIVO_AUDIO_STATE_PLAYING: return PlaybackState::Playing;
        case KIVO_AUDIO_STATE_PAUSED: return PlaybackState::Paused;
        case KIVO_AUDIO_STATE_SEEKING:
        case KIVO_AUDIO_STATE_RECOVERING: return PlaybackState::Buffering;
        case KIVO_AUDIO_STATE_STOPPED:
        case KIVO_AUDIO_STATE_CLOSED: return PlaybackState::Stopped;
        case KIVO_AUDIO_STATE_FAILED: return PlaybackState::Error;
        default: return PlaybackState::Stopped;
    }
}

} // namespace kivo::qt::audio_bridge::snapshot
