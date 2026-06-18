// =============================================================================
// Kivo Music Qt - audio_playback_controller_impl_queue.cpp
// AudioCoreBridge: Queue navigation and playback mode control
// =============================================================================

#include "audio_playback_controller_impl.hpp"
#include "../queue/playback_mode.hpp"

namespace kivo::qt::audio_bridge {

void AudioPlaybackControllerImpl::onEndOfStream() {
    if (playQueue_->isEmpty()) return;

    // Natural end of a track: advance per the active mode. nextIndex() honors ALL
    // modes — Sequential (+1, or -1 at the end), Shuffle (random), RepeatOne (same
    // index → replays), RepeatAll (wraps). This is the fix for shuffle/repeat-all
    // being silently ignored on auto-advance (the old code only ever did next()).
    const int next = playbackMode_->nextIndex(
        playQueue_->currentIndex(), playQueue_->count());
    if (next < 0) {
        // Terminal end of queue. The core is now Stopped and will not drain again,
        // so quiesce the truth surface immediately rather than letting the badge
        // linger on the last track until the next detail tick.
        {
            std::lock_guard<std::mutex> lock(stateMutex_);
            lastTruthSnapshot_ = {};  // fail-closed: drop the gold badge + rate/depth now
            bitPerfectStatus_ = lastTruthSnapshot_.lifecycleBadge();
            detailPollCounter_ = 0;
        }
        emit owner_->bitPerfectStatusChanged();
        emit owner_->truthChanged();
        emit owner_->queueFinished();
        return;
    }
    playQueue_->setCurrentIndex(next);
    openFile(playQueue_->currentTrack());
}

void AudioPlaybackControllerImpl::playNext() {
    if (playQueue_->isEmpty()) return;
    const int cur = playQueue_->currentIndex();
    const int count = playQueue_->count();
    // A user-initiated skip should ADVANCE even in RepeatOne (repeat-one only
    // auto-repeats on natural end); otherwise honor the mode (Shuffle/RepeatAll).
    const int next = playbackMode_->mode() == PlaybackMode::Mode::RepeatOne
        ? (cur + 1 < count ? cur + 1 : -1)
        : playbackMode_->nextIndex(cur, count);
    if (next < 0) return;
    playQueue_->setCurrentIndex(next);
    openFile(playQueue_->currentTrack());
}

void AudioPlaybackControllerImpl::playPrevious() {
    if (playQueue_->isEmpty()) return;
    const int cur = playQueue_->currentIndex();
    const int count = playQueue_->count();
    const int prev = playbackMode_->mode() == PlaybackMode::Mode::RepeatOne
        ? (cur > 0 ? cur - 1 : -1)
        : playbackMode_->previousIndex(cur, count);
    if (prev < 0) return;
    playQueue_->setCurrentIndex(prev);
    openFile(playQueue_->currentTrack());
}

void AudioPlaybackControllerImpl::togglePlaybackMode() {
    playbackMode_->toggleMode();
    emit owner_->playbackModeChanged();
}

void AudioPlaybackControllerImpl::toggleShuffle() {
    // Shuffle is orthogonal in intent but the backend is one 4-state enum: turning
    // shuffle on/off maps to Shuffle<->Sequential.
    const auto mode = playbackMode_->mode();
    playbackMode_->setMode(mode == PlaybackMode::Mode::Shuffle
        ? PlaybackMode::Mode::Sequential
        : PlaybackMode::Mode::Shuffle);
    emit owner_->playbackModeChanged();
}

void AudioPlaybackControllerImpl::cycleRepeat() {
    // Repeat button cycles: off -> Repeat-All -> Repeat-One -> off.
    const auto mode = playbackMode_->mode();
    PlaybackMode::Mode target;
    if (mode == PlaybackMode::Mode::RepeatAll) {
        target = PlaybackMode::Mode::RepeatOne;
    } else if (mode == PlaybackMode::Mode::RepeatOne) {
        target = PlaybackMode::Mode::Sequential;
    } else {  // Sequential or Shuffle
        target = PlaybackMode::Mode::RepeatAll;
    }
    playbackMode_->setMode(target);
    emit owner_->playbackModeChanged();
}

int AudioPlaybackControllerImpl::playbackModeValue() const {
    return static_cast<int>(playbackMode_->mode());
}

PlayQueue* AudioPlaybackControllerImpl::playQueue() const {
    return playQueue_;
}

PlaybackMode* AudioPlaybackControllerImpl::playbackMode() const {
    return playbackMode_;
}

} // namespace kivo::qt::audio_bridge
