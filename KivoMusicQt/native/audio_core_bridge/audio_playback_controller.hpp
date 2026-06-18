// =============================================================================
// Kivo Music Qt - audio_playback_controller.hpp
// AudioCoreBridge: Top-level Qt controller
// =============================================================================

#pragma once

#include <QColor>
#include <QList>
#include <QObject>
#include <QString>
#include <QStringList>
#include <memory>

// PlayQueue / PlaybackMode are exposed below as Q_PROPERTY / Q_INVOKABLE pointer
// types. Qt's moc registers metatypes for PlayQueue* / PlaybackMode*, and on
// Qt 6.5 that registration static_asserts the pointed-to type is fully defined
// ("Pointer Meta Types must point to fully-defined types"). So we need the full
// definitions here — NOT forward declarations — and the includes MUST sit
// outside the namespace (never #include inside a namespace block).
#include "queue/play_queue.hpp"
#include "queue/playback_mode.hpp"

namespace kivo::qt::audio_bridge {

// Pimpl: only ever held by pointer here, so a forward declaration is correct.
class AudioPlaybackControllerImpl;

// Qt-facing audio playback controller
// Thread-safe: all public methods can be called from UI thread
class AudioPlaybackController : public QObject {
    Q_OBJECT

    // Properties for QML binding
    Q_PROPERTY(QString title READ title NOTIFY titleChanged)
    Q_PROPERTY(QString artist READ artist NOTIFY artistChanged)
    Q_PROPERTY(QString album READ album NOTIFY albumChanged)
    Q_PROPERTY(QString formatText READ formatText NOTIFY formatTextChanged)
    Q_PROPERTY(QString elapsedText READ elapsedText NOTIFY elapsedTextChanged)
    Q_PROPERTY(QString durationText READ durationText NOTIFY durationTextChanged)
    Q_PROPERTY(double progress READ progress NOTIFY progressChanged)
    Q_PROPERTY(double volume READ volume WRITE setVolume NOTIFY volumeChanged)
    Q_PROPERTY(bool playing READ isPlaying NOTIFY playingChanged)
    Q_PROPERTY(bool loading READ isLoading NOTIFY loadingChanged)
    Q_PROPERTY(QString bitPerfectStatus READ bitPerfectStatus NOTIFY bitPerfectStatusChanged)

    // Audio-truth surface (ABI 1.2.0, evidence-gated). All default to a
    // not-confirmed / "Unknown" state so the absence of evidence never overclaims.
    Q_PROPERTY(bool truthEvidenceComplete READ truthEvidenceComplete NOTIFY truthChanged)
    Q_PROPERTY(bool truthBitPerfectConfirmed READ truthBitPerfectConfirmed NOTIFY truthChanged)
    Q_PROPERTY(QString truthVerdict READ truthVerdict NOTIFY truthChanged)
    Q_PROPERTY(QString truthRequestedMode READ truthRequestedMode NOTIFY truthChanged)
    Q_PROPERTY(QString truthActualMode READ truthActualMode NOTIFY truthChanged)
    Q_PROPERTY(QString truthSampleRate READ truthSampleRate NOTIFY truthChanged)
    Q_PROPERTY(QString truthBitDepth READ truthBitDepth NOTIFY truthChanged)
    Q_PROPERTY(QString truthEngineInPath READ truthEngineInPath NOTIFY truthChanged)
    Q_PROPERTY(QString truthConversionActive READ truthConversionActive NOTIFY truthChanged)
    Q_PROPERTY(QString truthProcessingActive READ truthProcessingActive NOTIFY truthChanged)
    Q_PROPERTY(QString truthRejectionReason READ truthRejectionReason NOTIFY truthChanged)

    // Playback mode property
    Q_PROPERTY(QString playbackMode READ playbackModeString NOTIFY playbackModeChanged)
    // Stable integer mode (0=Sequential,1=Shuffle,2=RepeatOne,3=RepeatAll) so QML
    // can drive button active-state without depending on a localized string or a
    // QML-registered enum.
    Q_PROPERTY(int playbackModeValue READ playbackModeValue NOTIFY playbackModeChanged)
    Q_PROPERTY(PlayQueue* playQueueModel READ playQueue CONSTANT)
    Q_PROPERTY(PlaybackMode* playbackModeModel READ playbackMode CONSTANT)

    // Cover art property
    Q_PROPERTY(QString coverArtPath READ coverArtPath NOTIFY coverArtPathChanged)
    // Vibrant color extracted from the artwork — drives the immersive now-playing
    // background. Invalid (default QColor) until a cover with color resolves.
    Q_PROPERTY(QColor coverArtColor READ coverArtColor NOTIFY coverArtColorChanged)
    // Stable hash of artist+title for GeneratedAlbumArtwork variant selection.
    // Computed in C++ on track change — replaces per-binding JS hash in QML.
    Q_PROPERTY(int coverVariantSeed READ coverVariantSeed NOTIFY coverVariantSeedChanged)

public:
    explicit AudioPlaybackController(QObject* parent = nullptr);
    ~AudioPlaybackController() override;

    // Property getters
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
    QString playbackModeString() const;
    int playbackModeValue() const;
    QString coverArtPath() const;
    QColor coverArtColor() const;
    int coverVariantSeed() const;

    // Property setters
    Q_INVOKABLE void setVolume(double volume);
    // EQ: applies 10-band parametric gains (dB). Pending audio-engine support;
    // the signal path is established so the UI is not silently lost.
    Q_INVOKABLE void setEqBands(const QList<double>& gains);

    // Queue access (for QML)
    Q_INVOKABLE PlayQueue* playQueue() const;
    Q_INVOKABLE PlaybackMode* playbackMode() const;

public slots:
    // Playback control (callable from QML)
    void openFile(const QString& filePath);
    void openFiles(const QStringList& filePaths); // Open multiple files
    // Replace the queue with this list and play from startIndex ("play context").
    void playTracks(const QStringList& filePaths, int startIndex);
    void play();
    void pause();
    void togglePlayPause();
    void stop();
    void seekTo(double progress);
    void skipForward(int seconds = 10);
    void skipBackward(int seconds = 10);
    
    // Queue control
    void playNext();
    void playPrevious();
    void togglePlaybackMode();
    // Intent-named mode controls for the two transport buttons (the backend is one
    // 4-state enum; these map shuffle/repeat intent onto it).
    void toggleShuffle();
    void cycleRepeat();

signals:
    // Property change notifications
    void titleChanged();
    void artistChanged();
    void albumChanged();
    void formatTextChanged();
    void elapsedTextChanged();
    void durationTextChanged();
    void progressChanged();
    void volumeChanged();
    void playingChanged();
    void loadingChanged();
    void bitPerfectStatusChanged();
    void truthChanged();
    void playbackModeChanged();
    void coverArtPathChanged();
    void coverArtColorChanged();
    void coverVariantSeedChanged();

    // Events
    void errorOccurred(const QString& message);
    void endOfStream();
    void fileOpened();
    void queueFinished(); // All tracks in queue finished

private:
    std::unique_ptr<AudioPlaybackControllerImpl> impl_;
};

} // namespace kivo::qt::audio_bridge
