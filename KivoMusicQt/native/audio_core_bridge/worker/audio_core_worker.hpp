// =============================================================================
// Kivo Music Qt - audio_core_worker.hpp
// Owns Host ABI calls on a dedicated QThread; communicates via signals
// =============================================================================

#pragma once

#include <QObject>
#include <QTimer>
#include <QString>
#include <memory>
#include <mutex>
#include <atomic>

#include "audio_core_worker_command.hpp"
#include "audio_core_worker_result.hpp"
#include "audio_core_worker_snapshot.hpp"
#include "../loader/audio_core_library_guard.hpp"
#include "../handle/engine_handle.hpp"

namespace kivo::qt::audio_bridge::worker {

class AudioCoreWorker : public QObject {
    Q_OBJECT

public:
    explicit AudioCoreWorker(QObject* parent = nullptr);
    ~AudioCoreWorker() override;

    bool isReady() const { return ready_.load(); }

public slots:
    void initialize();
    void shutdown();
    void openFile(const QString& filePath);
    void play();
    void pause();
    void stop();
    void seekTo(double progress);
    void skipBy(int seconds);
    void setVolume(double volume);

signals:
    void engineReady();
    void engineOffline(const QString& reason);
    void workerResult(const WorkerResult& result);

    // Snapshot emissions (lightweight copies, sent via queued connection)
    void playbackSnapshot(const WorkerPlaybackSnapshot& snap);
    void formatSnapshot(const WorkerFormatSnapshot& snap);
    void truthSnapshot(const WorkerTruthSnapshot& snap);

    // Events
    void fileOpened();
    void endOfStream();
    void workerError(const QString& message);
    void loadingChanged(bool loading);

private slots:
    void pollStep();

private:
    [[nodiscard]] bool submitToEngine(uint32_t kind, uint64_t seekFrame, const char* label);
    void pumpOnce();
    void readAndEmitSnapshots();
    void notifyFailure(const QString& label, int resultCode, const QString& detail);
    void enqueueCommand(WorkerCommand cmd);

    // DLL / engine (owned by worker thread)
    std::unique_ptr<loader::AudioCoreLibraryGuard> libraryGuard_;
    std::optional<handle::EngineHandle> engineHandle_;

    // Polling timer (in worker thread)
    QTimer* pollTimer_ = nullptr;

    // Worker state
    std::atomic<bool> ready_{false};

    // Snapshot diff state (no external mutex needed — only worker thread touches)
    WorkerPlaybackSnapshot lastPlayback_;
    bool wasPlaying_ = false;
    int idleCounter_ = 0;
    int pollCounter_ = 0;

    // Session clock
    uint64_t sessionGen_{0};
    uint64_t sourceGen_{0};
    uint64_t nextCommandId_{0};

    // Volume
    double volume_{0.7};

    // Command inbox (serialised via queued connection + mutex for enqueue)
    std::mutex cmdMutex_;
    bool hasPending_{false};
    WorkerCommand pending_;
};

} // namespace kivo::qt::audio_bridge::worker
