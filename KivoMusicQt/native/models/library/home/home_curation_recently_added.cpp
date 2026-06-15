// =============================================================================
// Kivo Music Qt - home_curation_recently_added.cpp
// Recently Added projection for home page
// =============================================================================

#include "home_curation_projection.h"
#include "home_curation_helpers.h"

QList<AlbumListItem> HomeCurationProjection::recentlyAdded(
    const QList<MusicFileRecord>& records) {
    QList<AlbumListItem> items;
    QSet<QString> keys;
    constexpr auto limit = 16;

    for (const auto& record : records) {
        appendUniqueItem(
            &items, &keys,
            makeAlbumItem(record, record.sourceLabel),
            limit);
    }

    return coveredFirst(items);
}
