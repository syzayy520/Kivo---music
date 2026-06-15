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
    kivo_audio_handle engine) {

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
    snapshot.position = std::chrono::milliseconds(
        static_cast<int64_t>((abiSnapshot.rendered_position_frames * 1000ull) / 48000ull));
    snapshot.duration = std::chrono::milliseconds(0);
    snapshot.hasValidPosition = abiSnapshot.rendered_position_frames > 0;

    if (abiSnapshot.decoded_frames > 0) {
        snapshot.progress =
            static_cast<double>(abiSnapshot.rendered_position_frames)
            / static_cast<double>(abiSnapshot.decoded_frames);
        snapshot.progress = std::clamp(snapshot.progress, 0.0, 1.0);
    }

    return error::BridgeResult<PlaybackSnapshot>::ok(snapshot);
}

PlaybackState PlaybackSnapshotReader::mapState(int abiState) {
    // Map Host ABI state codes to our enum
    // TODO: get actual state codes from Host ABI header
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
