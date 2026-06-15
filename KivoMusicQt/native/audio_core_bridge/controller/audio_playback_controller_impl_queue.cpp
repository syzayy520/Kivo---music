// =============================================================================
// Kivo Music Qt - audio_playback_controller_impl_queue.cpp
// AudioCoreBridge: Queue navigation and playback mode control
// =============================================================================

#include "audio_playback_controller_impl.hpp"
#include "../queue/playback_mode.hpp"

namespace kivo::qt::audio_bridge {

void AudioPlaybackControllerImpl::onEndOfStream() {
    if (playQueue_->isEmpty()) return;

    const auto mode = playbackMode_->mode();
    
    if (mode == PlaybackMode::Mode::RepeatOne) {
        openFile(playQueue_->currentTrack());
        return;
    }

    if (playQueue_->hasNext()) {
        playQueue_->next();
        openFile(playQueue_->currentTrack());
    } else {
        if (mode == PlaybackMode::Mode::RepeatAll) {
            playQueue_->setCurrentIndex(0);
            openFile(playQueue_->currentTrack());
        } else {
            emit owner_->queueFinished();
        }
    }
}

void AudioPlaybackControllerImpl::playNext() {
    if (playQueue_->isEmpty()) return;
    if (playQueue_->hasNext()) {
        playQueue_->next();
        openFile(playQueue_->currentTrack());
    }
}

void AudioPlaybackControllerImpl::playPrevious() {
    if (playQueue_->isEmpty()) return;
    if (playQueue_->hasPrevious()) {
        playQueue_->previous();
        openFile(playQueue_->currentTrack());
    }
}

void AudioPlaybackControllerImpl::togglePlaybackMode() {
    playbackMode_->toggleMode();
    emit owner_->playbackModeChanged();
}

PlayQueue* AudioPlaybackControllerImpl::playQueue() const {
    return playQueue_;
}

PlaybackMode* AudioPlaybackControllerImpl::playbackMode() const {
    return playbackMode_;
}

} // namespace kivo::qt::audio_bridge
