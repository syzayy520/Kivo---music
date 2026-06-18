// =============================================================================
// Kivo Music Qt - audio_playback_controller.cpp
// AudioCoreBridge: Public Qt controller facade
// =============================================================================

#include "audio_playback_controller.hpp"
#include "controller/audio_playback_controller_impl.hpp"

namespace kivo::qt::audio_bridge {

AudioPlaybackController::AudioPlaybackController(QObject* parent)
    : QObject(parent),
      impl_(std::make_unique<AudioPlaybackControllerImpl>(this)) {
    impl_->initialize();
}

AudioPlaybackController::~AudioPlaybackController() = default;

QString AudioPlaybackController::title() const { return impl_->title(); }
QString AudioPlaybackController::artist() const { return impl_->artist(); }
QString AudioPlaybackController::album() const { return impl_->album(); }
QString AudioPlaybackController::formatText() const { return impl_->formatText(); }
QString AudioPlaybackController::elapsedText() const { return impl_->elapsedText(); }
QString AudioPlaybackController::durationText() const { return impl_->durationText(); }
double AudioPlaybackController::progress() const { return impl_->progress(); }
double AudioPlaybackController::volume() const { return impl_->volume(); }
bool AudioPlaybackController::isPlaying() const { return impl_->isPlaying(); }
bool AudioPlaybackController::isLoading() const { return impl_->isLoading(); }
QString AudioPlaybackController::bitPerfectStatus() const { return impl_->bitPerfectStatus(); }
bool AudioPlaybackController::truthEvidenceComplete() const { return impl_->truthEvidenceComplete(); }
bool AudioPlaybackController::truthBitPerfectConfirmed() const { return impl_->truthBitPerfectConfirmed(); }
QString AudioPlaybackController::truthVerdict() const { return impl_->truthVerdict(); }
QString AudioPlaybackController::truthRequestedMode() const { return impl_->truthRequestedMode(); }
QString AudioPlaybackController::truthActualMode() const { return impl_->truthActualMode(); }
QString AudioPlaybackController::truthSampleRate() const { return impl_->truthSampleRate(); }
QString AudioPlaybackController::truthBitDepth() const { return impl_->truthBitDepth(); }
QString AudioPlaybackController::truthEngineInPath() const { return impl_->truthEngineInPath(); }
QString AudioPlaybackController::truthConversionActive() const { return impl_->truthConversionActive(); }
QString AudioPlaybackController::truthProcessingActive() const { return impl_->truthProcessingActive(); }
QString AudioPlaybackController::truthRejectionReason() const { return impl_->truthRejectionReason(); }
QString AudioPlaybackController::playbackModeString() const { return impl_->playbackMode()->modeString(); }
int AudioPlaybackController::playbackModeValue() const { return impl_->playbackModeValue(); }
QString AudioPlaybackController::coverArtPath() const { return impl_->coverArtPath(); }
QColor AudioPlaybackController::coverArtColor() const { return impl_->coverArtColor(); }
int AudioPlaybackController::coverVariantSeed() const { return impl_->coverVariantSeed(); }

void AudioPlaybackController::setVolume(double volume) { impl_->setVolume(volume); }
void AudioPlaybackController::setEqBands(const QList<double>& gains) { impl_->setEqBands(gains); }

void AudioPlaybackController::openFile(const QString& filePath) { impl_->openFile(filePath); }
void AudioPlaybackController::openFiles(const QStringList& filePaths) { impl_->openFiles(filePaths); }
void AudioPlaybackController::playTracks(const QStringList& filePaths, int startIndex) { impl_->playTracks(filePaths, startIndex); }
void AudioPlaybackController::play() { impl_->play(); }
void AudioPlaybackController::pause() { impl_->pause(); }

void AudioPlaybackController::togglePlayPause() {
    if (isPlaying()) {
        pause();
    } else {
        play();
    }
}

void AudioPlaybackController::stop() { impl_->stop(); }
void AudioPlaybackController::seekTo(double progress) { impl_->seekTo(progress); }
void AudioPlaybackController::skipForward(int seconds) { impl_->skipBy(seconds); }
void AudioPlaybackController::skipBackward(int seconds) { impl_->skipBy(-seconds); }

// Queue control
void AudioPlaybackController::playNext() { impl_->playNext(); }
void AudioPlaybackController::playPrevious() { impl_->playPrevious(); }
void AudioPlaybackController::togglePlaybackMode() { impl_->togglePlaybackMode(); }
void AudioPlaybackController::toggleShuffle() { impl_->toggleShuffle(); }
void AudioPlaybackController::cycleRepeat() { impl_->cycleRepeat(); }

// Accessors
PlayQueue* AudioPlaybackController::playQueue() const { return impl_->playQueue(); }
PlaybackMode* AudioPlaybackController::playbackMode() const { return impl_->playbackMode(); }

} // namespace kivo::qt::audio_bridge
