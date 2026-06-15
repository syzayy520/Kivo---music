// =============================================================================
// Kivo Music Qt - home_curation_rediscover.cpp
// Rediscover projection for home page
// =============================================================================

#include "home_curation_projection.h"
#include "home_curation_helpers.h"

QList<AlbumListItem> HomeCurationProjection::rediscover(
    const QList<MusicFileRecord>& records) {
    QList<AlbumListItem> items;
    QSet<QString> keys;
    constexpr auto limit = 12;

    for (auto index = records.size() - 1; index >= 0; --index) {
        appendUniqueItem(
            &items, &keys,
            makeTrackItem(records.at(index), QStringLiteral("Worth Revisiting")),
            limit);
    }

    return coveredFirst(items);
}
