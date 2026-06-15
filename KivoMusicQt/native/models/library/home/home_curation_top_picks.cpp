// =============================================================================
// Kivo Music Qt - home_curation_top_picks.cpp
// Top Picks projection for home page
// =============================================================================

#include "home_curation_projection.h"
#include "home_curation_helpers.h"

QList<AlbumListItem> HomeCurationProjection::topPicks(
    const QList<MusicFileRecord>& records) {
    QList<AlbumListItem> items;
    QSet<QString> keys;
    QSet<QString> usedSources;
    const auto orderedRecords = coveredRecordsFirst(records);
    const auto deferredAlbumKeys = firstAlbumKeys(orderedRecords, 2);
    constexpr auto limit = 14;

    for (auto index = 0; index < orderedRecords.size(); ++index) {
        const auto& record = orderedRecords.at(index);
        const auto album = makeAlbumItem(record, sourceMoodNote(record, index));
        if (deferredAlbumKeys.contains(itemKey(album))) {
            continue;
        }

        if (!usedSources.contains(record.sourceLabel)) {
            appendUniqueItem(&items, &keys, album, limit);
            usedSources.insert(record.sourceLabel);
        }

        if (index % 2 == 1) {
            const auto track = makeTrackItem(record, sourceMoodNote(record, index + 1));
            if (deferredAlbumKeys.contains(itemKey(track))) {
                continue;
            }
            appendUniqueItem(&items, &keys, track, limit);
        }

        if (items.size() >= 8 && usedSources.size() > 1) {
            break;
        }
    }

    for (auto index = 0; index < orderedRecords.size(); ++index) {
        const auto& record = orderedRecords.at(index);
        const auto album = makeAlbumItem(record, sourceMoodNote(record, items.size()));
        if (deferredAlbumKeys.contains(itemKey(album))) {
            continue;
        }
        appendUniqueItem(&items, &keys, album, limit);
    }

    for (auto index = 0; index < orderedRecords.size() && items.size() < 6; ++index) {
        const auto& record = orderedRecords.at(index);
        const auto album = makeAlbumItem(record, sourceMoodNote(record, items.size()));
        if (!deferredAlbumKeys.contains(itemKey(album))) {
            continue;
        }
        appendUniqueItem(&items, &keys, album, limit);
    }

    return items;
}
