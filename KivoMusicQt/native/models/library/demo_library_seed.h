#pragma once

#include "album_list_model.h"
#include "artist_list_model.h"
#include "track_list_model.h"

class DemoLibrarySeed final {
public:
    static QList<AlbumListItem> albums();
    static QList<AlbumListItem> pins();
    static QList<AlbumListItem> continueListening();
    static QList<AlbumListItem> recentAlbums();
    static QList<ArtistListItem> artists();
    static QList<TrackListItem> songs();
    static QList<TrackListItem> favorites();
    static QList<TrackListItem> focus();
    static QList<TrackListItem> videos();
};
