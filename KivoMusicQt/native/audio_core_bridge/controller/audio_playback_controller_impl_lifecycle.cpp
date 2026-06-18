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
    // Stop pumping BEFORE tearing down the engine the pump thread calls into.
    if (pumpWorker_) {
        pumpWorker_->stop();
        pumpWorker_.reset();
    }
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

    // Cache whether the connected core writes the ABI 1.1.0 timebase snapshot
    // tail (render rate / total frames / resample fact). On an older core the
    // flag is absent and the snapshot reader treats the tail as unknown ("--:--").
    {
        kivo_audio_capabilities_v1 caps{};
        caps.struct_size = sizeof(caps);
        caps.struct_version = KIVO_AUDIO_ABI_STRUCT_VERSION_1;
        const auto& fns = libraryGuard_->functions();
        if (fns.getCapabilities
            && fns.getCapabilities(engineHandle_->raw(), &caps)
                   == KIVO_AUDIO_RESULT_OK) {
            timebaseSnapshotSupported_ =
                (caps.capability_flags
                 & KIVO_AUDIO_CAPABILITY_PLAYBACK_TIMEBASE_SNAPSHOT) != 0u;
            truthSnapshotSupported_ =
                (caps.capability_flags
                 & KIVO_AUDIO_CAPABILITY_BIT_PERFECT_TRUTH) != 0u;
        }
    }

    snapshotTimer_ = new QTimer(owner_);
    QObject::connect(snapshotTimer_, &QTimer::timeout, [this]() {
        pollSnapshot();
    });

    // Drive the producer pump on its own thread so audio production is immune to
    // UI-thread jank (the cause of intermittent playback stutter). It is the sole
    // producer of the engine's SPSC queue; pacing is adaptive (cheap when idle).
    pumpWorker_ = std::make_unique<AudioPumpWorker>([this]() {
        return pumpStep();
    });
    pumpWorker_->start();
}

} // namespace kivo::qt::audio_bridge
