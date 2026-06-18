// =============================================================================
// Kivo Music Qt - audio_playback_controller_impl.hpp
// AudioCoreBridge: Internal Qt controller state and operations
// =============================================================================

#pragma once

#include "../audio_playback_controller.hpp"
#include "../handle/engine_handle.hpp"
#include "../loader/audio_core_library_guard.hpp"
#include "../snapshot/format_snapshot.hpp"
#include "../snapshot/playback_snapshot.hpp"
#include "../snapshot/truth_snapshot.hpp"
#include "../state/bridge_session_clock.hpp"
#include "../queue/play_queue.hpp"
#include "../queue/playback_mode.hpp"
#include "../pump/audio_pump_worker.hpp"

#include <QColor>
#include <QFileInfo>
#include <QTimer>
#include <chrono>
#include <memory>
#include <mutex>
#include <optional>

namespace kivo::qt::audio_bridge {

class AudioPlaybackControllerImpl {
public:
    explicit AudioPlaybackControllerImpl(AudioPlaybackController* owner);
    ~AudioPlaybackControllerImpl();

    void initialize();
    void openFile(const QString& filePath);
    void openFiles(const QStringList& filePaths);
    // Replace the queue with this exact list and play from startIndex — "play this
    // album/artist/search context" (openFiles only appends + always plays first).
    void playTracks(const QStringList& filePaths, int startIndex);
    void play();
    void pause();
    void stop();
    void seekTo(double progress);
    void skipBy(int seconds);
    void setVolume(double volume);
    void setEqBands(const QList<double>& gains);
    
    // Queue management
    void playNext();
    void playPrevious();
    void togglePlaybackMode();
    void toggleShuffle();
    void cycleRepeat();
    [[nodiscard]] int playbackModeValue() const;
    
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
    bool truthEvidenceComplete() const;
    bool truthBitPerfectConfirmed() const;
    QString truthVerdict() const;
    QString truthRequestedMode() const;
    QString truthActualMode() const;
    QString truthSampleRate() const;
    QString truthBitDepth() const;
    QString truthEngineInPath() const;
    QString truthConversionActive() const;
    QString truthProcessingActive() const;
    QString truthRejectionReason() const;
    QString coverArtPath() const;
    QColor coverArtColor() const;
    int coverVariantSeed() const;

private:
    void startSnapshotPolling();
    void stopSnapshotPolling();
    void pollSnapshot();
    void updateFromSnapshot();
    void extractMetadata(const QString& filePath);
    void notifyError(const QString& message);
    void onEndOfStream();
    void applyResolvedCover(uint64_t gen, const QString& coverUrl, const QColor& coverColor);
    [[nodiscard]] bool submitCommand(uint32_t kind, uint64_t requestedFrame, const char* label);
    // One producer pump step (decode→enqueue). Returns true if it should pump
    // again promptly (produced frames, queue not full). Runs on the dedicated
    // pump thread — NOT the UI thread — so UI jank can't starve audio.
    [[nodiscard]] bool pumpStep();

    // Stable hash matching JS djb2-style in GeneratedAlbumArtwork.
    // Called from extractMetadata() and exposed via coverVariantSeed Q_PROPERTY.
    [[nodiscard]] static int computeVariantSeed(const QString& artist, const QString& title);

    AudioPlaybackController* owner_ = nullptr;
    std::unique_ptr<loader::AudioCoreLibraryGuard> libraryGuard_;
    std::optional<handle::EngineHandle> engineHandle_;
    state::BridgeSessionClock sessionClock_;
    QTimer* snapshotTimer_ = nullptr;
    // Dedicated producer-pump thread (decouples audio production from the UI
    // thread). Declared after engineHandle_/libraryGuard_ so it is destroyed
    // FIRST — joined before the engine it pumps is torn down.
    std::unique_ptr<AudioPumpWorker> pumpWorker_;

    // Queue and mode
    PlayQueue* playQueue_ = nullptr;
    PlaybackMode* playbackMode_ = nullptr;

    // Performance: dynamic polling interval
    static constexpr int kDetailPollEvery = 10;  // detail (format/truth) poll cadence
    int idleCounter_ = 0;
    int detailPollCounter_ = 0;          // detail-poll cadence counter (member, resettable on open)
    uint64_t lastSuccessfulDrains_ = 0;  // edge-trigger baseline for end-of-stream detection
    bool timebaseSnapshotSupported_ = false;  // core advertises the ABI 1.1.0 timebase tail
    bool truthSnapshotSupported_ = false;      // core advertises the ABI 1.2.0 truth getter

    mutable std::mutex stateMutex_;
    // 空 = 无曲目。占位文案属于表现层(QML 负责本地化),后端只出数据,不出 UI 文案。
    QString title_;
    QString artist_;
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
    QColor coverArtColor_;
    int coverVariantSeed_{0};

    snapshot::PlaybackSnapshot lastPlaybackSnapshot_;
    snapshot::FormatSnapshot lastFormatSnapshot_;
    snapshot::TruthSnapshot lastTruthSnapshot_;
    std::chrono::milliseconds duration_{0};
};

} // namespace kivo::qt::audio_bridge
