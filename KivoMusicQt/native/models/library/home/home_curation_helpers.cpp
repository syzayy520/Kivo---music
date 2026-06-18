// =============================================================================
// Kivo Music Qt - home_curation_helpers.cpp
// Common helpers for home page curation projections
// =============================================================================

#include "home_curation_projection.h"

#include "../../common/local_file_url.h"

#include <QFileInfo>
#include <QSet>

namespace {

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

QString cleanNote(const QString& note) {
    return note.trimmed();
}

} // namespace

QString itemKey(const AlbumListItem& item) {
    if (!item.coverUrl.isEmpty()) {
        return QStringLiteral("cover:") + item.coverUrl;
    }
    return item.title.toCaseFolded() + QStringLiteral("|")
        + item.subtitle.toCaseFolded();
}

AlbumListItem makeAlbumItem(const MusicFileRecord& record, const QString& note) {
    const auto title = record.coverPath.isEmpty()
        ? record.title
        : albumTitleFromDirectory(record.album);
    const auto artist = record.coverPath.isEmpty()
        ? record.artist
        : albumArtistFromRecord(record);
    return {
        title.isEmpty() ? record.title : title,
        artist,
        cleanNote(note),
        record.artVariant,
        LocalFileUrl::fromPath(record.coverPath),
        record.path
    };
}

AlbumListItem makeTrackItem(const MusicFileRecord& record, const QString& note) {
    return {
        record.title,
        record.artist,
        cleanNote(note),
        record.artVariant,
        LocalFileUrl::fromPath(record.coverPath),
        record.path
    };
}

void appendUniqueItem(
    QList<AlbumListItem>* items,
    QSet<QString>* keys,
    const AlbumListItem& item,
    int limit) {
    if (items->size() >= limit || item.title.trimmed().isEmpty()) {
        return;
    }

    const auto key = itemKey(item);
    if (keys->contains(key)) {
        return;
    }

    keys->insert(key);
    items->append(item);
}

QSet<QString> firstAlbumKeys(
    const QList<MusicFileRecord>& records,
    int limit) {
    QSet<QString> keys;
    for (const auto& record : records) {
        if (keys.size() >= limit) {
            break;
        }
        const auto key = itemKey(makeAlbumItem(record, {}));
        if (!key.isEmpty()) {
            keys.insert(key);
        }
    }
    return keys;
}

bool looksNightRelated(const MusicFileRecord& record) {
    const auto haystack = (
        record.title + QStringLiteral(" ") + record.album).toCaseFolded();
    return haystack.contains(QStringLiteral("night"))
        || haystack.contains(QStringLiteral("dark"))
        || haystack.contains(QStringLiteral("moon"))
        || haystack.contains(QStringLiteral("midnight"))
        || haystack.contains(QStringLiteral("blue"));
}

QString sourceMoodNote(const MusicFileRecord& record, int slot) {
    // 去流媒体算法味:用本地资料库的人话标签,而非 "Recommended song" 这类推荐腔。
    // (当前 UI 默认中文;语言切换 .ts 基建落地后再统一走 translate()。)
    const auto source = record.sourceLabel.toCaseFolded();
    if (looksNightRelated(record)) {
        return QStringLiteral("深夜聆听");
    }
    if (source.contains(QStringLiteral("nas"))) {
        return slot % 2 == 0
            ? QStringLiteral("来自 NAS")
            : QStringLiteral("网络资料库");
    }
    if (slot % 3 == 0) {
        return QStringLiteral("很久没听");
    }
    if (slot % 3 == 1) {
        return QStringLiteral("本地精选");
    }
    return QStringLiteral("最近常听");
}

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

QList<MusicFileRecord> coveredRecordsFirst(const QList<MusicFileRecord>& records) {
    QList<MusicFileRecord> covered;
    QList<MusicFileRecord> fallback;
    for (const auto& record : records) {
        if (record.coverPath.isEmpty()) {
            fallback.append(record);
        } else {
            covered.append(record);
        }
    }
    covered.append(fallback);
    return covered;
}
