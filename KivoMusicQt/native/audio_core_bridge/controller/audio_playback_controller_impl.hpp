// =============================================================================
// Kivo Music Qt - audio_playback_controller_impl.hpp
// AudioCoreBridge: Controller — delegates Host ABI calls to worker thread
// =============================================================================

#pragma once

#include "../audio_playback_controller.hpp"
#include "../queue/play_queue.hpp"
#include "../queue/playback_mode.hpp"
#include "../worker/audio_core_worker.hpp"

#include <QThread>
#include <memory>
#include <mutex>

namespace kivo::qt::audio_bridge {

class AudioPlaybackControllerImpl : public QObject {
    Q_OBJECT

public:
    explicit AudioPlaybackControllerImpl(AudioPlaybackController* owner);
    ~AudioPlaybackControllerImpl() override;

    void initialize();
    void openFile(const QString& filePath);
    void openFiles(const QStringList& filePaths);
    void play();
    void pause();
    void stop();
    void seekTo(double progress);
    void skipBy(int seconds);
    void setVolume(double volume);
    
    // Queue management
    void playNext();
    void playPrevious();
    void togglePlaybackMode();
    
    // Accessors
    PlayQueue* playQueue() const;
    PlaybackMode* playbackMode() const;

    QString title() const;
    QString artist() const;
    QString album() const;
    QString formatText() const;
    QString elapsedText() const;
    QString durationText() const;
    double progress() const;
    double volume() const;
    bool isPlaying() const;
    bool isLoading() const;
    QString bitPerfectStatus() const;
    QString coverArtPath() const;

private slots:
    void onWorkerPlaybackSnapshot(const worker::WorkerPlaybackSnapshot& snap);
    void onWorkerFormatSnapshot(const worker::WorkerFormatSnapshot& snap);
    void onWorkerTruthSnapshot(const worker::WorkerTruthSnapshot& snap);
    void onWorkerError(const QString& message);
    void onWorkerLoadingChanged(bool loading);

private:
    void extractMetadata(const QString& filePath);
    void notifyError(const QString& message);
    void onEndOfStream();

    AudioPlaybackController* owner_ = nullptr;

    // Worker thread (owns all Host ABI calls)
    QThread* workerThread_ = nullptr;
    worker::AudioCoreWorker* audioWorker_ = nullptr;

    // Queue and mode (GUI thread, QML-accessible)
    PlayQueue* playQueue_ = nullptr;
    PlaybackMode* playbackMode_ = nullptr;

    mutable std::mutex stateMutex_;
    QString title_ = "No Track";
    QString artist_ = "Open an audio file";
    QString album_;
    QString formatText_ = "Ready";
    QString elapsedText_ = "--:--";
    QString durationText_ = "--:--";
    double progress_ = 0.0;
    double volume_ = 0.7;
    bool playing_ = false;
    bool loading_ = false;
    QString bitPerfectStatus_ = "--";
    QString coverArtPath_;
};

} // namespace kivo::qt::audio_bridge
