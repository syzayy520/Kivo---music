// =============================================================================
// Kivo Music Qt - audio_playback_controller_impl_properties.cpp
// AudioCoreBridge: Thread-safe property reads
// =============================================================================

#include "audio_playback_controller_impl.hpp"

namespace kivo::qt::audio_bridge {

QString AudioPlaybackControllerImpl::title() const {
    std::lock_guard<std::mutex> lock(stateMutex_);
    return title_;
}

QString AudioPlaybackControllerImpl::artist() const {
    std::lock_guard<std::mutex> lock(stateMutex_);
    return artist_;
}

QString AudioPlaybackControllerImpl::album() const {
    std::lock_guard<std::mutex> lock(stateMutex_);
    return album_;
}

QString AudioPlaybackControllerImpl::formatText() const {
    std::lock_guard<std::mutex> lock(stateMutex_);
    return formatText_;
}

QString AudioPlaybackControllerImpl::elapsedText() const {
    std::lock_guard<std::mutex> lock(stateMutex_);
    return elapsedText_;
}

QString AudioPlaybackControllerImpl::durationText() const {
    std::lock_guard<std::mutex> lock(stateMutex_);
    return durationText_;
}

double AudioPlaybackControllerImpl::progress() const {
    std::lock_guard<std::mutex> lock(stateMutex_);
    return progress_;
}

double AudioPlaybackControllerImpl::volume() const {
    std::lock_guard<std::mutex> lock(stateMutex_);
    return volume_;
}

bool AudioPlaybackControllerImpl::isPlaying() const {
    std::lock_guard<std::mutex> lock(stateMutex_);
    return playing_;
}

bool AudioPlaybackControllerImpl::isLoading() const {
    std::lock_guard<std::mutex> lock(stateMutex_);
    return loading_;
}

QString AudioPlaybackControllerImpl::bitPerfectStatus() const {
    std::lock_guard<std::mutex> lock(stateMutex_);
    return bitPerfectStatus_;
}

QString AudioPlaybackControllerImpl::coverArtPath() const {
    std::lock_guard<std::mutex> lock(stateMutex_);
    return coverArtPath_;
}

} // namespace kivo::qt::audio_bridge
