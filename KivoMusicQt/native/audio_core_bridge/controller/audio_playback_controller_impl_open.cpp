// =============================================================================
// Kivo Music Qt - audio_playback_controller_impl_open.cpp
// openFile enqueues to worker; metadata extraction stays on controller
// =============================================================================

#include "audio_playback_controller_impl.hpp"
#include "../../models/common/local_file_url.h"
#include "../../sources/music/music_cover_finder.h"

#include <QDir>
#include <QFileInfo>
#include <QUrl>

namespace {
QString normalizeLocalFilePath(const QString& input) {
    const QString value = input.trimmed();
    if (value.isEmpty()) return {};

    const QUrl url(value);
    if (url.isValid() && url.isLocalFile())
        return QDir::toNativeSeparators(url.toLocalFile());

    if (value.startsWith(QStringLiteral("file:"), Qt::CaseInsensitive)) {
        const QUrl tolerantUrl = QUrl::fromUserInput(value);
        if (tolerantUrl.isValid() && tolerantUrl.isLocalFile())
            return QDir::toNativeSeparators(tolerantUrl.toLocalFile());
    }
    return QDir::toNativeSeparators(value);
}
} // namespace

namespace kivo::qt::audio_bridge {

void AudioPlaybackControllerImpl::openFile(const QString& filePath) {
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

    // Queue management
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
        if (!found) playQueue_->addTrack(normalizedPath);
    }

    {
        std::lock_guard<std::mutex> lock(stateMutex_);
        loading_ = true;
    }
    emit owner_->loadingChanged();

    // Extract metadata synchronously (no Host ABI)
    extractMetadata(normalizedPath);

    // Find cover art (no Host ABI)
    {
        const QString localCover = MusicCoverFinder::findFor(fileInfo);
        std::lock_guard<std::mutex> lock(stateMutex_);
        coverArtPath_ = localCover.isEmpty()
            ? QString() : LocalFileUrl::fromPath(localCover);
    }
    emit owner_->coverArtPathChanged();

    // Delegate openSource + auto-play to worker thread
    if (audioWorker_) {
        QMetaObject::invokeMethod(audioWorker_, "openFile",
            Qt::QueuedConnection, Q_ARG(QString, normalizedPath));
    } else {
        notifyError("Audio worker not available");
        std::lock_guard<std::mutex> lock(stateMutex_);
        loading_ = false;
        emit owner_->loadingChanged();
    }
}

void AudioPlaybackControllerImpl::openFiles(const QStringList& filePaths) {
    QStringList normalized;
    normalized.reserve(filePaths.size());
    for (const QString& path : filePaths) {
        const QString p = normalizeLocalFilePath(path);
        if (!p.isEmpty()) normalized.append(p);
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
            ? "Local audio" : fileInfo.suffix().toUpper();
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
