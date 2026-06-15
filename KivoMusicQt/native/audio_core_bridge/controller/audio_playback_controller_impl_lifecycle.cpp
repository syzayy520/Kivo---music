// =============================================================================
// Kivo Music Qt - audio_playback_controller_impl_lifecycle.cpp
// Creates worker thread; controller only touches UI state
// =============================================================================

#include "audio_playback_controller_impl.hpp"
#include "../queue/play_queue.hpp"
#include "../queue/playback_mode.hpp"

namespace kivo::qt::audio_bridge {

AudioPlaybackControllerImpl::AudioPlaybackControllerImpl(
    AudioPlaybackController* owner)
    : QObject(nullptr)
    , owner_(owner) {
    playQueue_ = new PlayQueue(owner);
    playbackMode_ = new PlaybackMode(owner);

    QObject::connect(owner_, &AudioPlaybackController::endOfStream, [this]() {
        onEndOfStream();
    });
}

AudioPlaybackControllerImpl::~AudioPlaybackControllerImpl() {
    if (audioWorker_) {
        QMetaObject::invokeMethod(audioWorker_, "shutdown",
            Qt::QueuedConnection);
    }
    if (workerThread_) {
        workerThread_->quit();
        workerThread_->wait(3000);
    }
}

void AudioPlaybackControllerImpl::initialize() {
    // Create worker on its own thread
    workerThread_ = new QThread(this);
    audioWorker_ = new worker::AudioCoreWorker();
    audioWorker_->moveToThread(workerThread_);

    // Connect worker signals → controller slots (auto queued across threads)
    QObject::connect(audioWorker_, &worker::AudioCoreWorker::playbackSnapshot,
        this, &AudioPlaybackControllerImpl::onWorkerPlaybackSnapshot);
    QObject::connect(audioWorker_, &worker::AudioCoreWorker::formatSnapshot,
        this, &AudioPlaybackControllerImpl::onWorkerFormatSnapshot);
    QObject::connect(audioWorker_, &worker::AudioCoreWorker::truthSnapshot,
        this, &AudioPlaybackControllerImpl::onWorkerTruthSnapshot);
    QObject::connect(audioWorker_, &worker::AudioCoreWorker::workerError,
        this, &AudioPlaybackControllerImpl::onWorkerError);
    QObject::connect(audioWorker_, &worker::AudioCoreWorker::loadingChanged,
        this, &AudioPlaybackControllerImpl::onWorkerLoadingChanged);

    // Forward worker events to owner
    QObject::connect(audioWorker_, &worker::AudioCoreWorker::engineOffline,
        owner_, [this](const QString& reason) {
            {
                std::lock_guard<std::mutex> lock(stateMutex_);
                formatText_ = QString("Engine Offline: %1").arg(reason);
                bitPerfectStatus_ = "Offline";
            }
            emit owner_->formatTextChanged();
            emit owner_->bitPerfectStatusChanged();
        });

    QObject::connect(audioWorker_, &worker::AudioCoreWorker::fileOpened,
        owner_, &AudioPlaybackController::fileOpened);

    // Cleanup on thread finish
    QObject::connect(workerThread_, &QThread::finished,
        audioWorker_, &QObject::deleteLater);

    workerThread_->start();

    // Trigger DLL load + engine create on worker
    QMetaObject::invokeMethod(audioWorker_, "initialize",
        Qt::QueuedConnection);
}

} // namespace kivo::qt::audio_bridge
