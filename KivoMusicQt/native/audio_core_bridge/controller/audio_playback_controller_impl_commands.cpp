// =============================================================================
// Kivo Music Qt - audio_playback_controller_impl_commands.cpp
// AudioCoreBridge: Host ABI command submission
// =============================================================================

#include "audio_playback_controller_impl.hpp"
#include "../command/volume_command.hpp"

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
        // Polling is now stopped, so nothing else will refresh truth — quiesce it
        // here so the Bit-Perfect badge + rate/depth don't linger on the last track.
        lastTruthSnapshot_ = {};
        bitPerfectStatus_ = lastTruthSnapshot_.lifecycleBadge();
    }
    emit owner_->playingChanged();
    emit owner_->progressChanged();
    emit owner_->elapsedTextChanged();
    emit owner_->bitPerfectStatusChanged();
    emit owner_->truthChanged();
}

void AudioPlaybackControllerImpl::seekTo(double progress) {
    // Map a 0..1 UI position onto the source's true total length. seekTo and
    // skipBy now share the render-rate / total-frames basis the core SEEK uses.
    const uint64_t totalFrames = lastPlaybackSnapshot_.totalFrames;
    if (totalFrames == 0) {
        return;  // unknown duration → a fractional seek is meaningless
    }
    progress = std::clamp(progress, 0.0, 1.0);
    uint64_t frame =
        static_cast<uint64_t>(progress * static_cast<double>(totalFrames));
    if (frame >= totalFrames) {
        frame = totalFrames - 1;  // core rejects target >= duration
    }
    (void)submitCommand(KIVO_AUDIO_COMMAND_SEEK, frame, "Seek");
}

void AudioPlaybackControllerImpl::skipBy(int seconds) {
    const uint32_t rate = lastPlaybackSnapshot_.renderSampleRate;
    if (rate == 0) {
        return;  // unknown rate → cannot translate seconds to frames
    }
    const int64_t current =
        static_cast<int64_t>(lastPlaybackSnapshot_.renderedFrames);
    const int64_t delta =
        static_cast<int64_t>(seconds) * static_cast<int64_t>(rate);
    int64_t target = std::max<int64_t>(0, current + delta);
    const uint64_t totalFrames = lastPlaybackSnapshot_.totalFrames;
    if (totalFrames > 0
        && target >= static_cast<int64_t>(totalFrames)) {
        target = static_cast<int64_t>(totalFrames) - 1;  // clamp to last frame
    }
    (void)submitCommand(
        KIVO_AUDIO_COMMAND_SEEK, static_cast<uint64_t>(target), "Skip");
}

void AudioPlaybackControllerImpl::setVolume(double volume) {
    volume = std::clamp(volume, 0.0, 1.0);
    bool changed = false;
    {
        std::lock_guard<std::mutex> lock(stateMutex_);
        changed = std::abs(volume_ - volume) > 0.001;
        volume_ = volume;
    }
    if (!changed) {
        return;
    }
    // Drive the real engine gain (audible volume). With no engine open the value
    // is still stored and applied at the next openFile via the open config, so
    // the slider and the engine never disagree.
    if (engineHandle_ && libraryGuard_) {
        (void)submitCommand(
            KIVO_AUDIO_COMMAND_SET_VOLUME,
            command::VolumeCommand::toFixedGain(volume),
            "SetVolume");
    }
    emit owner_->volumeChanged();
}

void AudioPlaybackControllerImpl::setEqBands(const QList<double>& gains) {
    // EQ support is pending in the audio engine (AudioProcessingChain does not yet
    // include a parametric filter stage). The gain values are received here so the
    // signal path is established and tested independently of the DLL update.
    Q_UNUSED(gains)
}

bool AudioPlaybackControllerImpl::pumpStep() {
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
    if (status != KIVO_AUDIO_RESULT_OK && status != KIVO_AUDIO_RESULT_BUSY) {
        return false;
    }
    // Pump again promptly only while actively producing into a non-full queue;
    // otherwise (queue full / idle / engine busy with a command) back off.
    return result.made_progress != 0u && result.backpressured == 0u;
}

} // namespace kivo::qt::audio_bridge
