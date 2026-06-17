#include "library_model_bundle.h"

#include "demo_library_seed.h"
#include "home/home_curation_projection.h"
#include "music_library_projection.h"
#include "../common/local_file_url.h"

#include <QVariantMap>

namespace {
// Same logic as music_library_projection.cpp: extract album title from "Artist - Album" format
QString extractAlbumTitle(const QString& albumDirectory) {
    const auto separator = albumDirectory.indexOf(" - ");
    if (separator <= 0) {
        return albumDirectory;
    }
    return albumDirectory.mid(separator + 3).trimmed();
}
}  // namespace


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
    // Always seed demo first so the window paints instantly; real scan results
    // (if any) override it here and again later via applyMusicRecords().
    seedDemo();
    applyMusicRecords(musicRecords);
}

void LibraryModelBundle::seedDemo() {
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

    // Populate m_records from demo songs so tracksForAlbum/tracksForArtist work
    m_records.clear();
    const auto demoSongs = DemoLibrarySeed::songs();
    for (const auto& track : demoSongs) {
        MusicFileRecord rec;
        rec.path = QStringLiteral("/demo/") + track.title;  // fake path
        rec.title = track.title;
        rec.artist = track.artist;
        rec.album = track.album;
        rec.artVariant = track.artVariant;
        rec.coverPath = QString();  // demo has no cover files
        rec.sourceLabel = QStringLiteral("Demo");
        m_records.append(rec);
    }
}

void LibraryModelBundle::applyMusicRecords(
    const QList<MusicFileRecord>& musicRecords) {
    qDebug() << "[applyMusicRecords] Called with" << musicRecords.size() << "records";
    if (musicRecords.isEmpty()) {
        qDebug() << "[applyMusicRecords] Empty records, keeping current state";
        return;  // keep the current state rather than blanking the library
    }

    // Clear demo data on first real scan
    if (m_records.isEmpty() || (m_records.size() > 0 && m_records.first().path.startsWith("/demo/"))) {
        qDebug() << "[applyMusicRecords] Clearing demo data before first real scan";
        m_records.clear();
    }

    // Append new records instead of replacing.
    // Metadata correction is performed in the metadata_intelligence layer; models
    // consume raw or safely-corrected records only. No in-place field swap here.
    m_records.append(musicRecords);
    qDebug() << "[applyMusicRecords] m_records now has" << m_records.size() << "items (cumulative)";

    // Log first 3 records of this batch for debugging
    for (int i = 0; i < qMin(3, musicRecords.size()); ++i) {
        const auto& r = musicRecords.at(i);
        qDebug() << "[applyMusicRecords] Batch Record" << i << ":"
                 << "title=" << r.title << "artist=" << r.artist << "album=" << r.album;
    }

    // Re-project ALL accumulated records
    const auto tracks = MusicLibraryProjection::tracks(m_records);
    const auto albums = MusicLibraryProjection::albums(m_records);
    m_albums.setItems(albums);
    m_topPicks.setItems(HomeCurationProjection::topPicks(m_records));
    m_pins.setItems(MusicLibraryProjection::pins(m_records));
    m_continueListening.setItems(
        MusicLibraryProjection::continueListening(m_records));
    m_recent.setItems(HomeCurationProjection::recentlyAdded(m_records));
    m_rediscover.setItems(
        HomeCurationProjection::rediscover(m_records));
    m_afterHours.setItems(
        HomeCurationProjection::afterHours(m_records));
    m_artists.setItems(MusicLibraryProjection::artists(m_records));
    m_songs.setItems(tracks);
    m_favorites.setItems(tracks);
    m_focus.setItems(tracks);
    m_videos.setItems({});
}

QVariantList LibraryModelBundle::tracksForArtist(const QString& artist) const {
    QVariantList out;
    for (const auto& r : m_records) {
        // m_records stores raw scan results (or records corrected upstream);
        // drill-down uses the stored artist as the single source of truth.
        if (r.artist != artist) {
            continue;
        }
        QVariantMap m;
        m[QStringLiteral("title")] = r.title;
        m[QStringLiteral("artist")] = r.artist;
        m[QStringLiteral("album")] = r.album;
        m[QStringLiteral("filePath")] = r.path;
        m[QStringLiteral("duration")] = QString();  // TODO: read actual duration
        m[QStringLiteral("artVariant")] = r.artVariant;
        m[QStringLiteral("coverUrl")] = LocalFileUrl::fromPath(r.coverPath);
        out.append(m);
    }
    return out;
}

QVariantList LibraryModelBundle::tracksForAlbum(const QString& album) const {
    qDebug() << "[tracksForAlbum] Searching for album:" << album;
    qDebug() << "[tracksForAlbum] Total m_records:" << m_records.size();

    // Log first 5 unique album names in m_records
    QSet<QString> seenAlbums;
    for (const auto& r : m_records) {
        if (seenAlbums.size() >= 5) break;
        if (!seenAlbums.contains(r.album)) {
            seenAlbums.insert(r.album);
            qDebug() << "[tracksForAlbum] Available album in records:" << r.album;
        }
    }

    QVariantList out;
    int matchAttempts = 0;
    for (const auto& r : m_records) {
        // Extract display title - MUST use same logic as albums projection!
        // If no cover, display title comes from record.title, not album field
        const auto displayTitle = r.coverPath.isEmpty()
            ? r.title
            : extractAlbumTitle(r.album);

        if (matchAttempts < 3) {
            qDebug() << "[tracksForAlbum] Record album=" << r.album
                     << "coverPath=" << (r.coverPath.isEmpty() ? "EMPTY" : "HAS_COVER")
                     << "extracted displayTitle=" << displayTitle
                     << "search album=" << album
                     << "match=" << (displayTitle == album);
            matchAttempts++;
        }

        if (displayTitle != album) continue;

        // m_records stores raw scan results (or records corrected upstream).
        QVariantMap m;
        m[QStringLiteral("title")] = r.title;
        m[QStringLiteral("artist")] = r.artist;
        m[QStringLiteral("album")] = r.album;
        m[QStringLiteral("filePath")] = r.path;
        m[QStringLiteral("duration")] = QString();  // TODO: read actual duration from file metadata
        m[QStringLiteral("artVariant")] = r.artVariant;
        m[QStringLiteral("coverUrl")] = LocalFileUrl::fromPath(r.coverPath);
        out.append(m);
    }
    qDebug() << "[tracksForAlbum] Found" << out.size() << "tracks for album:" << album;
    return out;
}

QVariantList LibraryModelBundle::searchTracks(const QString& query) const {
    const QString q = query.trimmed().toLower();
    QVariantList out;
    if (q.isEmpty()) {
        return out;
    }
    constexpr int kMaxResults = 60;
    for (const auto& r : m_records) {
        if (!r.title.toLower().contains(q)
            && !r.artist.toLower().contains(q)
            && !r.album.toLower().contains(q)) {
            continue;
        }
        QVariantMap m;
        m[QStringLiteral("title")] = r.title;
        m[QStringLiteral("detail")] = r.artist;
        m[QStringLiteral("meta")] =
            r.formatLabel.isEmpty() ? r.sourceLabel : r.formatLabel;
        m[QStringLiteral("filePath")] = r.path;
        m[QStringLiteral("artVariant")] = r.artVariant;
        m[QStringLiteral("coverUrl")] = LocalFileUrl::fromPath(r.coverPath);
        out.append(m);
        if (out.size() >= kMaxResults) {
            break;
        }
    }
    return out;
}

QStringList LibraryModelBundle::trackFilePaths(const QString& kind) const {
    const TrackListModel* model = nullptr;
    if (kind == QLatin1String("songs"))     model = &m_songs;
    else if (kind == QLatin1String("favorites")) model = &m_favorites;
    else if (kind == QLatin1String("focus"))     model = &m_focus;
    else if (kind == QLatin1String("videos"))    model = &m_videos;
    return model ? model->filePaths() : QStringList{};
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
