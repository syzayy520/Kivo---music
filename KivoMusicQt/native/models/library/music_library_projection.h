#pragma once

#include "album_list_model.h"
#include "artist_list_model.h"
#include "track_list_model.h"
#include "../../sources/music/music_file_record.h"

#include <QList>

class MusicLibraryProjection final {
public:
    static QList<TrackListItem> tracks(const QList<MusicFileRecord>& records);
    static QList<AlbumListItem> albums(const QList<MusicFileRecord>& records);
    static QList<AlbumListItem> pins(const QList<MusicFileRecord>& records);
    static QList<AlbumListItem> continueListening(const QList<MusicFileRecord>& records);
    static QList<ArtistListItem> artists(const QList<MusicFileRecord>& records);
};
