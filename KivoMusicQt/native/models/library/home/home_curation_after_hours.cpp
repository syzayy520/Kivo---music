// =============================================================================
// Kivo Music Qt - home_curation_after_hours.cpp
// After Hours projection for home page
// =============================================================================

#include "home_curation_projection.h"
#include "home_curation_helpers.h"

QList<AlbumListItem> HomeCurationProjection::afterHours(
    const QList<MusicFileRecord>& records) {
    QList<AlbumListItem> items;
    QSet<QString> keys;
    constexpr auto limit = 12;

    for (const auto& record : records) {
        if (looksNightRelated(record)) {
            appendUniqueItem(
                &items, &keys,
                makeTrackItem(record, QStringLiteral("After Hours")),
                limit);
        }
    }

    for (auto index = 2; index < records.size(); index += 3) {
        appendUniqueItem(
            &items, &keys,
            makeTrackItem(records.at(index), QStringLiteral("After Hours")),
            limit);
    }

    return coveredFirst(items);
}
