#pragma once

#include "album_list_model.h"
#include "artist_list_model.h"
#include "track_list_model.h"
#include "../../sources/music/music_file_record.h"

#include <QObject>
#include <QVariantList>
#include <QList>

class LibraryModelBundle final : public QObject {
    Q_OBJECT
    Q_PROPERTY(AlbumListModel* albums READ albums CONSTANT)
    Q_PROPERTY(AlbumListModel* topPicks READ topPicks CONSTANT)
    Q_PROPERTY(AlbumListModel* pins READ pins CONSTANT)
    Q_PROPERTY(AlbumListModel* continueListening READ continueListening CONSTANT)
    Q_PROPERTY(AlbumListModel* recent READ recent CONSTANT)
    Q_PROPERTY(AlbumListModel* rediscover READ rediscover CONSTANT)
    Q_PROPERTY(AlbumListModel* afterHours READ afterHours CONSTANT)
    Q_PROPERTY(ArtistListModel* artists READ artists CONSTANT)
    Q_PROPERTY(TrackListModel* songs READ songs CONSTANT)
    Q_PROPERTY(TrackListModel* favorites READ favorites CONSTANT)
    Q_PROPERTY(TrackListModel* focus READ focus CONSTANT)
    Q_PROPERTY(TrackListModel* videos READ videos CONSTANT)

public:
    explicit LibraryModelBundle(
        const QList<MusicFileRecord>& musicRecords,
        QObject* parent = nullptr);

    // Swap in real scan results (UI thread only — resets the models). A no-op
    // when empty, so a failed/empty scan keeps the demo shelves on screen.
    void applyMusicRecords(const QList<MusicFileRecord>& musicRecords);

    // Drill-down: every track by this artist, as a QVariantList of {title, artist,
    // album, filePath, coverUrl, artVariant} maps (QML Repeater consumes directly).
    Q_INVOKABLE QVariantList tracksForArtist(const QString& artist) const;
    Q_INVOKABLE QVariantList tracksForAlbum(const QString& album) const;

    // Live search over the real library: case-insensitive match on title/artist/
    // album. Returns SearchResultRow-shaped maps {title, detail, meta, filePath,
    // coverUrl, artVariant}, capped so a broad query stays responsive.
    Q_INVOKABLE QVariantList searchTracks(const QString& query) const;

    // All file paths from a named track model ("songs"/"favorites"/"focus"/"videos").
    Q_INVOKABLE QStringList trackFilePaths(const QString& kind) const;

    AlbumListModel* albums();
    AlbumListModel* topPicks();
    AlbumListModel* pins();
    AlbumListModel* continueListening();
    AlbumListModel* recent();
    AlbumListModel* rediscover();
    AlbumListModel* afterHours();
    ArtistListModel* artists();
    TrackListModel* songs();
    TrackListModel* favorites();
    TrackListModel* focus();
    TrackListModel* videos();

private:
    void seedDemo();

    QList<MusicFileRecord> m_records;  // raw scan results, for on-demand drill-down
    AlbumListModel m_albums;
    AlbumListModel m_topPicks;
    AlbumListModel m_pins;
    AlbumListModel m_continueListening;
    AlbumListModel m_recent;
    AlbumListModel m_rediscover;
    AlbumListModel m_afterHours;
    ArtistListModel m_artists;
    TrackListModel m_songs;
    TrackListModel m_favorites;
    TrackListModel m_focus;
    TrackListModel m_videos;
};
