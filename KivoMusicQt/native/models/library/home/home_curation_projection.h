#pragma once

#include "../album_list_model.h"
#include "../../../sources/music/music_file_record.h"

#include <QList>

class HomeCurationProjection final {
public:
    static QList<AlbumListItem> topPicks(const QList<MusicFileRecord>& records);
    static QList<AlbumListItem> recentlyAdded(
        const QList<MusicFileRecord>& records);
    static QList<AlbumListItem> rediscover(
        const QList<MusicFileRecord>& records);
    static QList<AlbumListItem> afterHours(
        const QList<MusicFileRecord>& records);
};
