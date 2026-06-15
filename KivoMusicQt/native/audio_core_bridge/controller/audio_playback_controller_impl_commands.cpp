// =============================================================================
// Kivo Music Qt - audio_playback_controller_impl_commands.cpp
// AudioCoreBridge: Host ABI command submission
// =============================================================================

#include "audio_playback_controller_impl.hpp"

#include <algorithm>
#include <cmath>

namespace kivo::qt::audio_bridge {

bool AudioPlaybackControllerImpl::submitCommand(
    uint32_t kind,
    uint64_t requestedFrame,
    const char* label) {
    if (!engineHandle_ || !libraryGuard_) {
        return false;
    }

    kivo_audio_command_v1 command{};
    command.struct_size = sizeof(kivo_audio_command_v1);
    command.struct_version = KIVO_AUDIO_ABI_STRUCT_VERSION_1;
    command.command_id = sessionClock_.nextCommandId();
    command.session_generation = sessionClock_.sessionGeneration();
    command.kind = kind;
    command.requested_frame = requestedFrame;

    const auto result = libraryGuard_->functions().submitCommand(
        engineHandle_->raw(),
        &command);
    if (result != KIVO_AUDIO_RESULT_OK) {
        notifyError(QString("%1 failed: %2")
            .arg(QString::fromUtf8(label))
            .arg(static_cast<int>(result)));
        return false;
    }
    return true;
}

void AudioPlaybackControllerImpl::play() {
    if (submitCommand(KIVO_AUDIO_COMMAND_RESUME, 0, "Play")) {
        startSnapshotPolling();
    }
}

void AudioPlaybackControllerImpl::pause() {
    (void)submitCommand(KIVO_AUDIO_COMMAND_PAUSE, 0, "Pause");
}

void AudioPlaybackControllerImpl::stop() {
    stopSnapshotPolling();
    (void)submitCommand(KIVO_AUDIO_COMMAND_STOP, 0, "Stop");
    {
        std::lock_guard<std::mutex> lock(stateMutex_);
        playing_ = false;
        progress_ = 0.0;
        elapsedText_ = "0:00";
    }
    emit owner_->playingChanged();
    emit owner_->progressChanged();
    emit owner_->elapsedTextChanged();
}

void AudioPlaybackControllerImpl::seekTo(double progress) {
    progress = std::clamp(progress, 0.0, 1.0);
    const auto frame = static_cast<uint64_t>(progress * static_cast<double>(
        lastPlaybackSnapshot_.decodedFrames));
    (void)submitCommand(KIVO_AUDIO_COMMAND_SEEK, frame, "Seek");
}

void AudioPlaybackControllerImpl::skipBy(int seconds) {
    const int sampleRate = 48000;
    const int64_t current = static_cast<int64_t>(lastPlaybackSnapshot_.renderedFrames);
    const int64_t delta = static_cast<int64_t>(seconds) * sampleRate;
    const auto target = static_cast<uint64_t>(std::max<int64_t>(0, current + delta));
    (void)submitCommand(KIVO_AUDIO_COMMAND_SEEK, target, "Skip");
}

void AudioPlaybackControllerImpl::setVolume(double volume) {
    volume = std::clamp(volume, 0.0, 1.0);
    bool changed = false;
    {
        std::lock_guard<std::mutex> lock(stateMutex_);
        changed = std::abs(volume_ - volume) > 0.001;
        volume_ = volume;
    }
    if (changed) {
        emit owner_->volumeChanged();
    }
}

bool AudioPlaybackControllerImpl::pumpOnce() {
    if (!engineHandle_ || !libraryGuard_) {
        return false;
    }
    kivo_audio_pump_request_v1 request{};
    request.struct_size = sizeof(kivo_audio_pump_request_v1);
    request.struct_version = KIVO_AUDIO_ABI_STRUCT_VERSION_1;
    request.maximum_steps = 4;

    kivo_audio_pump_result_v1 result{};
    result.struct_size = sizeof(kivo_audio_pump_result_v1);
    result.struct_version = KIVO_AUDIO_ABI_STRUCT_VERSION_1;

    const auto status = libraryGuard_->functions().pump(
        engineHandle_->raw(),
        &request,
        &result);
    return status == KIVO_AUDIO_RESULT_OK
        || status == KIVO_AUDIO_RESULT_BUSY;
}

} // namespace kivo::qt::audio_bridge
