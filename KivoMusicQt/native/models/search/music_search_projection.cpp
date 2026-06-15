#include "music_search_projection.h"

#include "../common/local_file_url.h"

QList<SearchResultItem> MusicSearchProjection::results(
    const QList<MusicFileRecord>& records,
    int maxItems) {
    QList<SearchResultItem> items;
    for (const auto& record : records) {
        if (items.size() >= maxItems) {
            break;
        }

        items.append({
            record.title,
            record.artist + " - " + record.sourceLabel,
            record.formatLabel,
            record.artVariant,
            LocalFileUrl::fromPath(record.coverPath),
            record.path
        });
    }
    return items;
}
