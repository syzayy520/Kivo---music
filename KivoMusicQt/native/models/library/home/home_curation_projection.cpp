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

AlbumListItem albumItem(const MusicFileRecord& record, const QString& note) {
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
        LocalFileUrl::fromPath(record.coverPath)
    };
}

AlbumListItem trackItem(const MusicFileRecord& record, const QString& note) {
    return {
        record.title,
        record.artist,
        cleanNote(note),
        record.artVariant,
        LocalFileUrl::fromPath(record.coverPath)
    };
}

QString itemKey(const AlbumListItem& item) {
    if (!item.coverUrl.isEmpty()) {
        return QStringLiteral("cover:") + item.coverUrl;
    }
    return item.title.toCaseFolded() + QStringLiteral("|")
        + item.subtitle.toCaseFolded();
}

void appendUnique(
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

bool looksNightRelated(const MusicFileRecord& record) {
    const auto haystack = (
        record.title + QStringLiteral(" ") + record.album).toCaseFolded();
    return haystack.contains(QStringLiteral("night"))
        || haystack.contains(QStringLiteral("dark"))
        || haystack.contains(QStringLiteral("moon"))
        || haystack.contains(QStringLiteral("midnight"))
        || haystack.contains(QStringLiteral("blue"));
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

} // namespace

QList<AlbumListItem> HomeCurationProjection::topPicks(
    const QList<MusicFileRecord>& records) {
    QList<AlbumListItem> items;
    QSet<QString> keys;
    QSet<QString> usedSources;
    const auto orderedRecords = coveredRecordsFirst(records);
    constexpr auto limit = 14;

    for (auto index = 0; index < orderedRecords.size(); ++index) {
        const auto& record = orderedRecords.at(index);
        if (!usedSources.contains(record.sourceLabel)) {
            appendUnique(
                &items,
                &keys,
                albumItem(record, QStringLiteral("Recommended Album")),
                limit);
            usedSources.insert(record.sourceLabel);
        }

        if (index % 2 == 1) {
            appendUnique(
                &items,
                &keys,
                trackItem(record, QStringLiteral("Recommended Song")),
                limit);
        }

        if (items.size() >= 8 && usedSources.size() > 1) {
            break;
        }
    }

    for (const auto& record : orderedRecords) {
        appendUnique(
            &items,
            &keys,
            albumItem(record, QStringLiteral("From Your Library")),
            limit);
    }

    return coveredFirst(items);
}

QList<AlbumListItem> HomeCurationProjection::recentlyAdded(
    const QList<MusicFileRecord>& records) {
    QList<AlbumListItem> items;
    QSet<QString> keys;
    constexpr auto limit = 16;

    for (const auto& record : records) {
        appendUnique(
            &items,
            &keys,
            albumItem(record, record.sourceLabel),
            limit);
    }

    return coveredFirst(items);
}

QList<AlbumListItem> HomeCurationProjection::rediscover(
    const QList<MusicFileRecord>& records) {
    QList<AlbumListItem> items;
    QSet<QString> keys;
    constexpr auto limit = 12;

    for (auto index = records.size() - 1; index >= 0; --index) {
        appendUnique(
            &items,
            &keys,
            trackItem(records.at(index), QStringLiteral("Worth Revisiting")),
            limit);
    }

    return coveredFirst(items);
}

QList<AlbumListItem> HomeCurationProjection::afterHours(
    const QList<MusicFileRecord>& records) {
    QList<AlbumListItem> items;
    QSet<QString> keys;
    constexpr auto limit = 12;

    for (const auto& record : records) {
        if (looksNightRelated(record)) {
            appendUnique(
                &items,
                &keys,
                trackItem(record, QStringLiteral("After Hours")),
                limit);
        }
    }

    for (auto index = 2; index < records.size(); index += 3) {
        appendUnique(
            &items,
            &keys,
            trackItem(records.at(index), QStringLiteral("After Hours")),
            limit);
    }

    return coveredFirst(items);
}
