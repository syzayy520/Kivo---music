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
#include "../state/bridge_session_clock.hpp"
#include "../queue/play_queue.hpp"
#include "../queue/playback_mode.hpp"

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

private:
    void startSnapshotPolling();
    void stopSnapshotPolling();
    void pollSnapshot();
    void updateFromSnapshot();
    void extractMetadata(const QString& filePath);
    void notifyError(const QString& message);
    void onEndOfStream();
    [[nodiscard]] bool submitCommand(uint32_t kind, uint64_t requestedFrame, const char* label);
    [[nodiscard]] bool pumpOnce();

    AudioPlaybackController* owner_ = nullptr;
    std::unique_ptr<loader::AudioCoreLibraryGuard> libraryGuard_;
    std::optional<handle::EngineHandle> engineHandle_;
    state::BridgeSessionClock sessionClock_;
    QTimer* snapshotTimer_ = nullptr;
    
    // Queue and mode
    PlayQueue* playQueue_ = nullptr;
    PlaybackMode* playbackMode_ = nullptr;

    // Performance: dynamic polling interval
    int idleCounter_ = 0;

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

    snapshot::PlaybackSnapshot lastPlaybackSnapshot_;
    snapshot::FormatSnapshot lastFormatSnapshot_;
    std::chrono::milliseconds duration_{0};
};

} // namespace kivo::qt::audio_bridge
