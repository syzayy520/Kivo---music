// =============================================================================
// Kivo Music Qt - audio_playback_controller_impl_lifecycle.cpp
// AudioCoreBridge: Construction, destruction, engine initialization
// =============================================================================

#include "audio_playback_controller_impl.hpp"
#include "../queue/play_queue.hpp"
#include "../queue/playback_mode.hpp"

#include <QTimer>

namespace kivo::qt::audio_bridge {

AudioPlaybackControllerImpl::AudioPlaybackControllerImpl(
    AudioPlaybackController* owner)
    : owner_(owner) {
    playQueue_ = new PlayQueue(owner);
    playbackMode_ = new PlaybackMode(owner);
    
    QObject::connect(owner_, &AudioPlaybackController::endOfStream, [this]() {
        onEndOfStream();
    });
}

AudioPlaybackControllerImpl::~AudioPlaybackControllerImpl() {
    stopSnapshotPolling();
    engineHandle_.reset();
}

void AudioPlaybackControllerImpl::initialize() {
    libraryGuard_ = std::make_unique<loader::AudioCoreLibraryGuard>();

    auto initResult = libraryGuard_->initialize();
    if (initResult.isFailed()) {
        {
            std::lock_guard<std::mutex> lock(stateMutex_);
            formatText_ = "Engine Offline";
            bitPerfectStatus_ = "Offline";
        }
        libraryGuard_.reset();
        emit owner_->formatTextChanged();
        emit owner_->bitPerfectStatusChanged();
        return;
    }

    auto engineResult = handle::EngineHandle::create(libraryGuard_->functions());
    if (engineResult.isFailed()) {
        {
            std::lock_guard<std::mutex> lock(stateMutex_);
            formatText_ = "Engine Offline";
            bitPerfectStatus_ = "Offline";
        }
        libraryGuard_.reset();
        emit owner_->formatTextChanged();
        emit owner_->bitPerfectStatusChanged();
        return;
    }

    engineHandle_ = std::move(engineResult.value());
    snapshotTimer_ = new QTimer(owner_);
    QObject::connect(snapshotTimer_, &QTimer::timeout, [this]() {
        pollSnapshot();
    });
}

} // namespace kivo::qt::audio_bridge
