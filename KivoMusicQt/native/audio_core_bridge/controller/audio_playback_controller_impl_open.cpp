#include "audio_playback_controller_impl.hpp"
#include "../source/file_audio_source.hpp"
#include "../state/default_open_configuration.hpp"
#include "../../models/common/local_file_url.h"
#include "../../sources/music/music_cover_finder.h"

#include <QDir>
#include <QFileInfo>
#include <QUrl>

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
    extractMetadata(normalizedPath);

    {
        const QString localCover = MusicCoverFinder::findFor(fileInfo);
        std::lock_guard<std::mutex> lock(stateMutex_);
        coverArtPath_ = localCover.isEmpty()
            ? QString()
            : LocalFileUrl::fromPath(localCover);
    }
    emit owner_->coverArtPathChanged();

    startSnapshotPolling();
    play();

    {
        std::lock_guard<std::mutex> lock(stateMutex_);
        loading_ = false;
    }
    emit owner_->loadingChanged();
    emit owner_->fileOpened();
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
    }

    emit owner_->titleChanged();
    emit owner_->artistChanged();
    emit owner_->albumChanged();
    emit owner_->formatTextChanged();
}

void AudioPlaybackControllerImpl::notifyError(const QString& message) {
    QMetaObject::invokeMethod(owner_, [owner = owner_, message]() {
        emit owner->errorOccurred(message);
    }, Qt::QueuedConnection);
}

} // namespace kivo::qt::audio_bridge
