// =============================================================================
// Kivo Music Qt - audio_playback_controller.hpp
// AudioCoreBridge: Top-level Qt controller
// =============================================================================

#pragma once

#include <QObject>
#include <QString>
#include <QStringList>
#include <memory>

namespace kivo::qt::audio_bridge {

class AudioPlaybackControllerImpl;
class PlayQueue;
class PlaybackMode;

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
    
    // Playback mode property
    Q_PROPERTY(QString playbackMode READ playbackModeString NOTIFY playbackModeChanged)
    Q_PROPERTY(PlayQueue* playQueueModel READ playQueue CONSTANT)
    Q_PROPERTY(PlaybackMode* playbackModeModel READ playbackMode CONSTANT)

    // Cover art property
    Q_PROPERTY(QString coverArtPath READ coverArtPath NOTIFY coverArtPathChanged)

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
    QString playbackModeString() const;
    QString coverArtPath() const;

    // Property setter
    Q_INVOKABLE void setVolume(double volume);

    // Queue access (for QML)
    Q_INVOKABLE PlayQueue* playQueue() const;
    Q_INVOKABLE PlaybackMode* playbackMode() const;

public slots:
    // Playback control (callable from QML)
    void openFile(const QString& filePath);
    void openFiles(const QStringList& filePaths); // Open multiple files
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
    void playbackModeChanged();
    void coverArtPathChanged();

    // Events
    void errorOccurred(const QString& message);
    void endOfStream();
    void fileOpened();
    void queueFinished(); // All tracks in queue finished

private:
    std::unique_ptr<AudioPlaybackControllerImpl> impl_;
};

} // namespace kivo::qt::audio_bridge
