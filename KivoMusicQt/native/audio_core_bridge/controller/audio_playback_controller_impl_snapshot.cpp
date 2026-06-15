// =============================================================================
// Kivo Music Qt - audio_playback_controller_impl_snapshot.cpp
// Reacts to worker snapshot signals — projects to Qt property emissions
// =============================================================================

#include "audio_playback_controller_impl.hpp"

#include <cmath>

namespace kivo::qt::audio_bridge {

void AudioPlaybackControllerImpl::onWorkerPlaybackSnapshot(
    const worker::WorkerPlaybackSnapshot& snap) {
    if (!snap.valid) return;

    bool stateChanged = false;
    bool progressChanged = false;

    {
        std::lock_guard<std::mutex> lock(stateMutex_);
        const bool wasPlaying = playing_;
        playing_ = snap.isPlaying;
        stateChanged = (wasPlaying != playing_);

        const double oldProgress = progress_;
        progress_ = snap.progress;
        progressChanged = (std::abs(oldProgress - progress_) > 0.001);

        elapsedText_ = snap.elapsedText;
        durationText_ = snap.durationText;
    }

    if (stateChanged) emit owner_->playingChanged();
    if (progressChanged) {
        emit owner_->progressChanged();
        emit owner_->elapsedTextChanged();
    }
}

void AudioPlaybackControllerImpl::onWorkerFormatSnapshot(
    const worker::WorkerFormatSnapshot& snap) {
    if (!snap.valid) return;

    QString newFormat;
    {
        std::lock_guard<std::mutex> lock(stateMutex_);
        newFormat = snap.formatText;
        if (formatText_ != newFormat) {
            formatText_ = newFormat;
        } else {
            newFormat.clear(); // no change
        }
    }
    if (!newFormat.isEmpty()) emit owner_->formatTextChanged();
}

void AudioPlaybackControllerImpl::onWorkerTruthSnapshot(
    const worker::WorkerTruthSnapshot& snap) {
    if (!snap.valid) return;

    bool changed = false;
    {
        std::lock_guard<std::mutex> lock(stateMutex_);
        changed = (bitPerfectStatus_ != snap.bitPerfectStatus);
        if (changed) bitPerfectStatus_ = snap.bitPerfectStatus;
    }
    if (changed) emit owner_->bitPerfectStatusChanged();
}

void AudioPlaybackControllerImpl::onWorkerError(const QString& message) {
    notifyError(message);
}

void AudioPlaybackControllerImpl::onWorkerLoadingChanged(bool loading) {
    {
        std::lock_guard<std::mutex> lock(stateMutex_);
        loading_ = loading;
    }
    emit owner_->loadingChanged();
}

} // namespace kivo::qt::audio_bridge
