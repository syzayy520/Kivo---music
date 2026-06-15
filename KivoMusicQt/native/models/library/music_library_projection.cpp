#include "music_library_projection.h"

#include "../common/local_file_url.h"

#include <QFileInfo>
#include <QHash>
#include <QSet>

namespace {

QList<AlbumListItem> coveredFirst(QList<AlbumListItem> items) {
    QList<AlbumListItem> covered;
    QList<AlbumListItem> fallback;
    for (const auto& item : items) {
        if (item.coverUrl.isEmpty()) {
            fallback.append(item);
        } else {
            covered.append(item);
        }
    }
    covered.append(fallback);
    return covered;
}

QList<TrackListItem> coveredFirst(QList<TrackListItem> items) {
    QList<TrackListItem> covered;
    QList<TrackListItem> fallback;
    for (const auto& item : items) {
        if (item.coverUrl.isEmpty()) {
            fallback.append(item);
        } else {
            covered.append(item);
        }
    }
    covered.append(fallback);
    return covered;
}

QString albumTitleFromDirectory(const QString& albumDirectory) {
    const auto separator = albumDirectory.indexOf(" - ");
    if (separator <= 0) {
        return albumDirectory;
    }
    return albumDirectory.mid(separator + 3).trimmed();
}

QString albumArtistFromRecord(const MusicFileRecord& record) {
    if (record.artist != "Unknown Artist") {
        return record.artist;
    }

    const auto separator = record.album.indexOf(" - ");
    if (separator <= 0) {
        return record.artist;
    }
    return record.album.left(separator).trimmed();
}

} // namespace

QList<TrackListItem> MusicLibraryProjection::tracks(
    const QList<MusicFileRecord>& records) {
    QList<TrackListItem> items;
    for (const auto& record : records) {
        items.append({
            record.title,
            record.artist,
            record.formatLabel + " - " + record.sourceLabel,
            "",
            record.artVariant,
            LocalFileUrl::fromPath(record.coverPath),
            record.path
        });
    }
    return coveredFirst(items);
}

QList<AlbumListItem> MusicLibraryProjection::albums(
    const QList<MusicFileRecord>& records) {
    QList<AlbumListItem> items;
    QSet<QString> coveredAlbumKeys;
    for (const auto& record : records) {
        if (!record.coverPath.isEmpty()) {
            const auto key = QFileInfo(record.coverPath).absoluteFilePath();
            if (coveredAlbumKeys.contains(key)) {
                continue;
            }
            coveredAlbumKeys.insert(key);
        }

        const auto displayTitle = record.coverPath.isEmpty()
            ? record.title
            : albumTitleFromDirectory(record.album);
        const auto displayArtist = record.coverPath.isEmpty()
            ? record.artist
            : albumArtistFromRecord(record);
        items.append({
            displayTitle.isEmpty() ? record.title : displayTitle,
            displayArtist,
            record.formatLabel + " - " + record.sourceLabel,
            record.artVariant,
            LocalFileUrl::fromPath(record.coverPath),
            record.path
        });
    }
    return coveredFirst(items);
}

QList<AlbumListItem> MusicLibraryProjection::pins(
    const QList<MusicFileRecord>& records) {
    auto items = albums(records);
    if (items.size() > 16) {
        items.erase(items.begin() + 16, items.end());
    }
    return items;
}

QList<AlbumListItem> MusicLibraryProjection::continueListening(
    const QList<MusicFileRecord>& records) {
    auto items = albums(records);
    if (items.size() > 2) {
        items.erase(items.begin() + 2, items.end());
    }
    return items;
}

QList<ArtistListItem> MusicLibraryProjection::artists(
    const QList<MusicFileRecord>& records) {
    QHash<QString, int> counts;
    QHash<QString, int> variants;
    QHash<QString, QString> covers;
    for (const auto& record : records) {
        counts[record.artist] += 1;
        variants.insert(record.artist, record.artVariant);
        if (!record.coverPath.isEmpty() && !covers.contains(record.artist)) {
            covers.insert(record.artist, record.coverPath);
        }
    }

    QList<ArtistListItem> items;
    for (auto it = counts.cbegin(); it != counts.cend(); ++it) {
        items.append({
            it.key(),
            QString::number(it.value()) + " tracks",
            variants.value(it.key()),
            LocalFileUrl::fromPath(covers.value(it.key()))
        });
    }
    return items;
}
