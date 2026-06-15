#include "music_cover_finder.h"

#include <QDir>
#include <QStringList>

namespace {

QString findNamedCover(const QDir& directory) {
    const QStringList imageFilters = {
        "*.jpg",
        "*.jpeg",
        "*.png",
        "*.webp",
    };
    const auto files = directory.entryInfoList(
        imageFilters,
        QDir::Files | QDir::Readable,
        QDir::Name);
    const QStringList preferredNames = {
        "cover",
        "folder",
        "front",
        "album",
        "artwork",
    };

    for (const auto& preferred : preferredNames) {
        for (const auto& file : files) {
            if (file.completeBaseName().compare(
                    preferred,
                    Qt::CaseInsensitive) == 0) {
                return file.absoluteFilePath();
            }
        }
    }
    return {};
}

} // namespace

QString MusicCoverFinder::findFor(const QFileInfo& audioFile) {
    const QDir audioDirectory = audioFile.dir();
    const auto sameDirectoryCover = findNamedCover(audioDirectory);
    if (!sameDirectoryCover.isEmpty()) {
        return sameDirectoryCover;
    }

    QDir artworkDirectory(audioDirectory.absoluteFilePath("Artwork"));
    if (artworkDirectory.exists()) {
        return findNamedCover(artworkDirectory);
    }

    return {};
}

bool MusicCoverFinder::supportsEmbeddedArtwork() {
    // Embedded artwork extraction requires FFmpeg SDK (KIVO_FFMPEG_ROOT).
    // Currently only sidecar cover files are supported.
    return false;
}
