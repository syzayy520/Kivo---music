// =============================================================================
// Kivo Music Qt - audio_playback_controller_impl_commands.cpp
// All commands delegate to worker thread — return immediately, never block GUI
// =============================================================================

#include "audio_playback_controller_impl.hpp"

#include <algorithm>
#include <cmath>

namespace kivo::qt::audio_bridge {

void AudioPlaybackControllerImpl::play() {
    if (audioWorker_) {
        QMetaObject::invokeMethod(audioWorker_, "play", Qt::QueuedConnection);
    }
}

void AudioPlaybackControllerImpl::pause() {
    if (audioWorker_) {
        QMetaObject::invokeMethod(audioWorker_, "pause", Qt::QueuedConnection);
    }
}

void AudioPlaybackControllerImpl::stop() {
    {
        std::lock_guard<std::mutex> lock(stateMutex_);
        playing_ = false;
        progress_ = 0.0;
        elapsedText_ = "0:00";
    }
    emit owner_->playingChanged();
    emit owner_->progressChanged();
    emit owner_->elapsedTextChanged();

    if (audioWorker_) {
        QMetaObject::invokeMethod(audioWorker_, "stop", Qt::QueuedConnection);
    }
}

void AudioPlaybackControllerImpl::seekTo(double progress) {
    progress = std::clamp(progress, 0.0, 1.0);
    if (audioWorker_) {
        QMetaObject::invokeMethod(audioWorker_, "seekTo",
            Qt::QueuedConnection, Q_ARG(double, progress));
    }
}

void AudioPlaybackControllerImpl::skipBy(int seconds) {
    if (audioWorker_) {
        QMetaObject::invokeMethod(audioWorker_, "skipBy",
            Qt::QueuedConnection, Q_ARG(int, seconds));
    }
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
    if (audioWorker_) {
        QMetaObject::invokeMethod(audioWorker_, "setVolume",
            Qt::QueuedConnection, Q_ARG(double, volume));
    }
}

} // namespace kivo::qt::audio_bridge
