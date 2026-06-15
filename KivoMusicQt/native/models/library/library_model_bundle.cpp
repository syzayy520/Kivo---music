#include "library_model_bundle.h"

#include "demo_library_seed.h"
#include "home/home_curation_projection.h"
#include "music_library_projection.h"

LibraryModelBundle::LibraryModelBundle(
    const QList<MusicFileRecord>& musicRecords,
    QObject* parent)
    : QObject(parent),
      m_albums(),
      m_topPicks(),
      m_pins(),
      m_continueListening(),
      m_recent(),
      m_rediscover(),
      m_afterHours(),
      m_artists(),
      m_songs(),
      m_favorites(),
      m_focus(),
      m_videos() {
    if (!musicRecords.isEmpty()) {
        const auto tracks = MusicLibraryProjection::tracks(musicRecords);
        const auto albums = MusicLibraryProjection::albums(musicRecords);
        m_albums.setItems(albums);
        m_topPicks.setItems(HomeCurationProjection::topPicks(musicRecords));
        m_pins.setItems(MusicLibraryProjection::pins(musicRecords));
        m_continueListening.setItems(
            MusicLibraryProjection::continueListening(musicRecords));
        m_recent.setItems(HomeCurationProjection::recentlyAdded(musicRecords));
        m_rediscover.setItems(
            HomeCurationProjection::rediscover(musicRecords));
        m_afterHours.setItems(
            HomeCurationProjection::afterHours(musicRecords));
        m_artists.setItems(MusicLibraryProjection::artists(musicRecords));
        m_songs.setItems(tracks);
        m_favorites.setItems(tracks);
        m_focus.setItems(tracks);
        m_videos.setItems({});
        return;
    }

    m_albums.setItems(DemoLibrarySeed::albums());
    m_topPicks.setItems(DemoLibrarySeed::pins());
    m_pins.setItems(DemoLibrarySeed::pins());
    m_continueListening.setItems(DemoLibrarySeed::continueListening());
    m_recent.setItems(DemoLibrarySeed::recentAlbums());
    m_rediscover.setItems(DemoLibrarySeed::recentAlbums());
    m_afterHours.setItems(DemoLibrarySeed::albums());
    m_artists.setItems(DemoLibrarySeed::artists());
    m_songs.setItems(DemoLibrarySeed::songs());
    m_favorites.setItems(DemoLibrarySeed::favorites());
    m_focus.setItems(DemoLibrarySeed::focus());
    m_videos.setItems(DemoLibrarySeed::videos());
}

AlbumListModel* LibraryModelBundle::albums() {
    return &m_albums;
}

AlbumListModel* LibraryModelBundle::topPicks() {
    return &m_topPicks;
}

AlbumListModel* LibraryModelBundle::pins() {
    return &m_pins;
}

AlbumListModel* LibraryModelBundle::continueListening() {
    return &m_continueListening;
}

AlbumListModel* LibraryModelBundle::recent() {
    return &m_recent;
}

AlbumListModel* LibraryModelBundle::rediscover() {
    return &m_rediscover;
}

AlbumListModel* LibraryModelBundle::afterHours() {
    return &m_afterHours;
}

ArtistListModel* LibraryModelBundle::artists() {
    return &m_artists;
}

TrackListModel* LibraryModelBundle::songs() {
    return &m_songs;
}

TrackListModel* LibraryModelBundle::favorites() {
    return &m_favorites;
}

TrackListModel* LibraryModelBundle::focus() {
    return &m_focus;
}

TrackListModel* LibraryModelBundle::videos() {
    return &m_videos;
}
