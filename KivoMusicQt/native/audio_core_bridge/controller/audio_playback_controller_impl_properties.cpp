// =============================================================================
// Kivo Music Qt - audio_playback_controller_impl_properties.cpp
// AudioCoreBridge: Thread-safe property reads
// =============================================================================

#include "audio_playback_controller_impl.hpp"

namespace kivo::qt::audio_bridge {

// Matches the JS djb2-style hash used in TransportTrackInfo/MiniPlayer QML:
//   hash = ((hash << 5) - hash) + charCodeAt(i); hash |= 0; Math.abs(hash)
// Computed once in C++ on track change instead of per-binding in QML.
int AudioPlaybackControllerImpl::computeVariantSeed(
    const QString& artist, const QString& title) {
    const QString str = artist + QLatin1Char('_') + title;
    quint32 h = 0;
    for (const QChar c : str) {
        h = (h << 5) - h + static_cast<quint32>(c.unicode());
    }
    return static_cast<int>(h & 0x7fffffffu);  // always ≥ 0
}

QString AudioPlaybackControllerImpl::title() const {
    std::lock_guard<std::mutex> lock(stateMutex_);
    return title_;
}

QString AudioPlaybackControllerImpl::artist() const {
    std::lock_guard<std::mutex> lock(stateMutex_);
    return artist_;
}

QString AudioPlaybackControllerImpl::album() const {
    std::lock_guard<std::mutex> lock(stateMutex_);
    return album_;
}

QString AudioPlaybackControllerImpl::formatText() const {
    std::lock_guard<std::mutex> lock(stateMutex_);
    return formatText_;
}

QString AudioPlaybackControllerImpl::elapsedText() const {
    std::lock_guard<std::mutex> lock(stateMutex_);
    return elapsedText_;
}

QString AudioPlaybackControllerImpl::durationText() const {
    std::lock_guard<std::mutex> lock(stateMutex_);
    return durationText_;
}

double AudioPlaybackControllerImpl::progress() const {
    std::lock_guard<std::mutex> lock(stateMutex_);
    return progress_;
}

double AudioPlaybackControllerImpl::volume() const {
    std::lock_guard<std::mutex> lock(stateMutex_);
    return volume_;
}

bool AudioPlaybackControllerImpl::isPlaying() const {
    std::lock_guard<std::mutex> lock(stateMutex_);
    return playing_;
}

bool AudioPlaybackControllerImpl::isLoading() const {
    std::lock_guard<std::mutex> lock(stateMutex_);
    return loading_;
}

QString AudioPlaybackControllerImpl::bitPerfectStatus() const {
    std::lock_guard<std::mutex> lock(stateMutex_);
    return bitPerfectStatus_;
}

bool AudioPlaybackControllerImpl::truthEvidenceComplete() const {
    std::lock_guard<std::mutex> lock(stateMutex_);
    return lastTruthSnapshot_.evidenceComplete;
}

bool AudioPlaybackControllerImpl::truthBitPerfectConfirmed() const {
    std::lock_guard<std::mutex> lock(stateMutex_);
    return lastTruthSnapshot_.isBitPerfectConfirmed();
}

QString AudioPlaybackControllerImpl::truthVerdict() const {
    std::lock_guard<std::mutex> lock(stateMutex_);
    return lastTruthSnapshot_.verdictText();
}

QString AudioPlaybackControllerImpl::truthRequestedMode() const {
    std::lock_guard<std::mutex> lock(stateMutex_);
    return lastTruthSnapshot_.requestedModeText();
}

QString AudioPlaybackControllerImpl::truthActualMode() const {
    std::lock_guard<std::mutex> lock(stateMutex_);
    return lastTruthSnapshot_.actualModeText();
}

QString AudioPlaybackControllerImpl::truthSampleRate() const {
    std::lock_guard<std::mutex> lock(stateMutex_);
    return lastTruthSnapshot_.sampleRateText();
}

QString AudioPlaybackControllerImpl::truthBitDepth() const {
    std::lock_guard<std::mutex> lock(stateMutex_);
    return lastTruthSnapshot_.bitDepthText();
}

QString AudioPlaybackControllerImpl::truthEngineInPath() const {
    std::lock_guard<std::mutex> lock(stateMutex_);
    return lastTruthSnapshot_.engineInPathText();
}

QString AudioPlaybackControllerImpl::truthConversionActive() const {
    std::lock_guard<std::mutex> lock(stateMutex_);
    return lastTruthSnapshot_.conversionActiveText();
}

QString AudioPlaybackControllerImpl::truthProcessingActive() const {
    std::lock_guard<std::mutex> lock(stateMutex_);
    return lastTruthSnapshot_.processingActiveText();
}

QString AudioPlaybackControllerImpl::truthRejectionReason() const {
    std::lock_guard<std::mutex> lock(stateMutex_);
    return lastTruthSnapshot_.rejectionReasonText();
}

QString AudioPlaybackControllerImpl::coverArtPath() const {
    std::lock_guard<std::mutex> lock(stateMutex_);
    return coverArtPath_;
}

int AudioPlaybackControllerImpl::coverVariantSeed() const {
    std::lock_guard<std::mutex> lock(stateMutex_);
    return coverVariantSeed_;
}

} // namespace kivo::qt::audio_bridge
