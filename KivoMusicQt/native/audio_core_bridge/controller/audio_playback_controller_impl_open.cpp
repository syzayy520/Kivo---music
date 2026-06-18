#include "audio_playback_controller_impl.hpp"
#include "../source/file_audio_source.hpp"
#include "../state/default_open_configuration.hpp"
#include "../../sources/music/music_cover_finder.h"
#include "../../sources/music/embedded_cover_extractor.h"
#include "../../data/image/cover_color_extractor.hpp"
#include "../../data/image/cover_image_provider.hpp"

#include <QDir>
#include <QFileInfo>
#include <QPointer>
#include <QStandardPaths>
#include <QUrl>
#include <QtConcurrent>

namespace {

QString normalizeLocalFilePath(const QString& input) {
    const QString value = input.trimmed();
    if (value.isEmpty()) {
        return {};
    }

    const QUrl url(value);
    if (url.isValid() && url.isLocalFile()) {
        return QDir::toNativeSeparators(url.toLocalFile());
    }

    if (value.startsWith(QStringLiteral("file:"), Qt::CaseInsensitive)) {
        const QUrl tolerantUrl = QUrl::fromUserInput(value);
        if (tolerantUrl.isValid() && tolerantUrl.isLocalFile()) {
            return QDir::toNativeSeparators(tolerantUrl.toLocalFile());
        }
    }

    return QDir::toNativeSeparators(value);
}

} // namespace

namespace kivo::qt::audio_bridge {

void AudioPlaybackControllerImpl::openFile(const QString& filePath) {
    if (!engineHandle_ || !libraryGuard_) {
        notifyError("Audio engine not initialized");
        return;
    }

    const QString normalizedPath = normalizeLocalFilePath(filePath);
    if (normalizedPath.isEmpty()) {
        notifyError("No file path provided");
        return;
    }

    QFileInfo fileInfo(normalizedPath);
    if (!fileInfo.exists()) {
        notifyError(QString("File not found: %1").arg(normalizedPath));
        return;
    }

    if (!fileInfo.isFile()) {
        notifyError(QString("Not a file: %1").arg(normalizedPath));
        return;
    }

    if (!fileInfo.isReadable()) {
        notifyError(QString("File is not readable: %1").arg(normalizedPath));
        return;
    }

    if (playQueue_->isEmpty()) {
        playQueue_->addTrack(normalizedPath);
    } else {
        bool found = false;
        for (int i = 0; i < playQueue_->count(); ++i) {
            if (playQueue_->getTrackAt(i) == normalizedPath) {
                playQueue_->setCurrentIndex(i);
                found = true;
                break;
            }
        }
        if (!found) {
            playQueue_->addTrack(normalizedPath);
        }
    }

    {
        std::lock_guard<std::mutex> lock(stateMutex_);
        loading_ = true;
    }
    emit owner_->loadingChanged();

    const uint64_t sessionGeneration = sessionClock_.nextSessionGeneration();
    const uint64_t sourceGeneration = sessionClock_.nextSourceGeneration();
    const uint64_t commandId = sessionClock_.nextCommandId();

    auto sourceLease = source::FileAudioSourceLease::open(
        normalizedPath,
        sourceGeneration,
        sourceGeneration);
    if (sourceLease.isFailed()) {
        notifyError(sourceLease.error().message);
        {
            std::lock_guard<std::mutex> lock(stateMutex_);
            loading_ = false;
        }
        emit owner_->loadingChanged();
        return;
    }

    auto configuration = state::makeDefaultOpenConfiguration();
    // Open at the controller's CURRENT volume so the engine and the UI agree
    // from the first frame (instead of a hardcoded default gain).
    {
        std::lock_guard<std::mutex> lock(stateMutex_);
        configuration.software_volume_linear_gain = volume_;
    }
    auto result = libraryGuard_->functions().openSource(
        engineHandle_->raw(),
        commandId,
        sessionGeneration,
        &sourceLease.value().abiSource(),
        &configuration);

    if (result != KIVO_AUDIO_RESULT_OK) {
        notifyError(QString("Failed to open file through audio core: %1")
            .arg(static_cast<int>(result)));
        {
            std::lock_guard<std::mutex> lock(stateMutex_);
            loading_ = false;
        }
        emit owner_->loadingChanged();
        return;
    }

    sourceLease.value().releaseToHost();
    extractMetadata(normalizedPath);  // pure string parse — cheap, stays inline

    // Cover discovery enumerates a directory (disk I/O) — run it off the UI
    // thread. Deliver back via a queued call guarded by sessionGeneration so a
    // late result for a track the user already skipped past is dropped. QPointer
    // guards the controller's lifetime across the worker hop; Qt drops the queued
    // call if the controller is gone by the time the UI thread would run it.
    {
        const uint64_t gen = sessionGeneration;
        QPointer<AudioPlaybackController> owner(owner_);
        auto* self = this;
        QtConcurrent::run([owner, self, gen, normalizedPath]() {
            const QFileInfo info(normalizedPath);
            QString localCover = MusicCoverFinder::findFor(info);
            if (localCover.isEmpty()) {
                // No folder image — pull THIS file's embedded cover (FLAC) so the
                // now-playing hero + ambient color use the real art, not a fallback.
                const QString coverCacheDir =
                    QStandardPaths::writableLocation(QStandardPaths::CacheLocation)
                    + QStringLiteral("/covers");
                localCover = EmbeddedCoverExtractor::extractTo(info, coverCacheDir);
            }
            // Pre-populate the image provider cache off the UI thread so QML's
            // first Image request is a cache hit (no render-thread disk decode).
            if (!localCover.isEmpty()) {
                if (auto* p = kivo::qt::data::globalArtworkProvider()) {
                    p->preload(localCover);
                }
            }
            // Emit image://artwork/<path> so QML goes through the provider
            // rather than file:// (provider handles caching + size negotiation).
            const QString artworkUrl = localCover.isEmpty()
                ? QString()
                : QStringLiteral("image://artwork/") + localCover;
            // Extract the artwork's vibrant color here (off the UI thread) for the
            // immersive now-playing background.
            const QColor color = data::CoverColorExtractor::dominant(localCover);
            if (!owner) {
                return;
            }
            QMetaObject::invokeMethod(
                owner,
                [self, gen, artworkUrl, color]() { self->applyResolvedCover(gen, artworkUrl, color); },
                Qt::QueuedConnection);
        });
    }

    // P1-1: seed the end-of-stream baseline to the engine's current cumulative
    // drain count for THIS new session, so the first poll cannot mistake a
    // historical drain for a fresh completion. Force the next detail poll to
    // refresh format/truth immediately (kills the ~450ms stale-badge window
    // after an auto-advance). UI-thread only — no locking needed.
    {
        auto seed = snapshot::PlaybackSnapshotReader::read(
            libraryGuard_->functions(), engineHandle_->raw(),
            timebaseSnapshotSupported_);
        if (seed.isOk()) {
            lastSuccessfulDrains_ = seed.value().successfulDrains;
        }
        detailPollCounter_ = kDetailPollEvery;
    }

    // Truth must fail-closed across track changes: clear the structured truth
    // snapshot NOW so the gold Bit-Perfect badge + sample-rate/bit-depth can't
    // linger from the previous track during the open→first-poll window (up to
    // ~200ms when idle-throttled). The next detail poll repopulates real evidence.
    {
        std::lock_guard<std::mutex> lock(stateMutex_);
        lastTruthSnapshot_ = {};
        bitPerfectStatus_ = lastTruthSnapshot_.lifecycleBadge();
    }
    emit owner_->bitPerfectStatusChanged();
    emit owner_->truthChanged();

    startSnapshotPolling();
    play();

    {
        std::lock_guard<std::mutex> lock(stateMutex_);
        loading_ = false;
    }
    emit owner_->loadingChanged();
    emit owner_->fileOpened();
}

void AudioPlaybackControllerImpl::applyResolvedCover(
    uint64_t gen,
    const QString& coverUrl,
    const QColor& coverColor) {
    // Runs on the UI thread (queued). Drop stale results from a track the user
    // already skipped past.
    if (gen != sessionClock_.sessionGeneration()) {
        return;
    }
    {
        std::lock_guard<std::mutex> lock(stateMutex_);
        coverArtPath_ = coverUrl;
        coverArtColor_ = coverColor;
    }
    emit owner_->coverArtPathChanged();
    emit owner_->coverArtColorChanged();
}

QColor AudioPlaybackControllerImpl::coverArtColor() const {
    std::lock_guard<std::mutex> lock(stateMutex_);
    return coverArtColor_;
}

void AudioPlaybackControllerImpl::openFiles(const QStringList& filePaths) {
    QStringList normalized;
    normalized.reserve(filePaths.size());
    for (const QString& path : filePaths) {
        const QString normalizedPath = normalizeLocalFilePath(path);
        if (!normalizedPath.isEmpty()) {
            normalized.append(normalizedPath);
        }
    }

    if (normalized.isEmpty()) {
        notifyError("No playable files were provided");
        return;
    }

    playQueue_->addTracks(normalized);
    openFile(normalized.first());
}

void AudioPlaybackControllerImpl::playTracks(
    const QStringList& filePaths, int startIndex) {
    QStringList normalized;
    normalized.reserve(filePaths.size());
    for (const QString& path : filePaths) {
        const QString n = normalizeLocalFilePath(path);
        if (!n.isEmpty()) {
            normalized.append(n);
        }
    }
    if (normalized.isEmpty()) {
        return;
    }
    // REPLACE the queue (not append) so "Play" plays exactly this context, and
    // start at the chosen track so auto-advance continues through the rest.
    playQueue_->clear();
    playQueue_->addTracks(normalized);
    int idx = startIndex;
    if (idx < 0) idx = 0;
    if (idx >= normalized.size()) idx = normalized.size() - 1;
    playQueue_->setCurrentIndex(idx);
    openFile(normalized.at(idx));  // already in queue → sets index, plays, no dup
}

void AudioPlaybackControllerImpl::extractMetadata(const QString& filePath) {
    QFileInfo fileInfo(filePath);
    const QString fileName = fileInfo.completeBaseName();
    const QStringList parts = fileName.split(" - ", Qt::SkipEmptyParts);

    {
        std::lock_guard<std::mutex> lock(stateMutex_);
        if (parts.size() >= 2) {
            artist_ = parts[0].trimmed();
            title_ = parts[1].trimmed();
        } else {
            artist_ = "Unknown Artist";
            title_ = fileName;
        }
        album_ = "Local File";
        formatText_ = fileInfo.suffix().isEmpty()
            ? "Local audio"
            : fileInfo.suffix().toUpper();
        coverVariantSeed_ = computeVariantSeed(artist_, title_);
    }

    emit owner_->titleChanged();
    emit owner_->artistChanged();
    emit owner_->albumChanged();
    emit owner_->formatTextChanged();
    emit owner_->coverVariantSeedChanged();
}

void AudioPlaybackControllerImpl::notifyError(const QString& message) {
    QMetaObject::invokeMethod(owner_, [owner = owner_, message]() {
        emit owner->errorOccurred(message);
    }, Qt::QueuedConnection);
}

} // namespace kivo::qt::audio_bridge
