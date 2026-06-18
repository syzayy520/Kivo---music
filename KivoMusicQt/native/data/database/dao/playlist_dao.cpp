// =============================================================================
// Kivo Music Qt - playlist_dao.cpp
// 职责: 播放列表 SQLite DAO 实现
// =============================================================================

#include "playlist_dao.hpp"
#include "../database_manager.hpp"

#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

namespace kivo::qt::data {

PlaylistDao::PlaylistDao(QObject* parent) : QObject(parent) {}

QVector<PlaylistRecord> PlaylistDao::getAllPlaylists() {
    QVector<PlaylistRecord> result;
    auto db = DatabaseManager::instance().database();
    QSqlQuery q(db);
    q.prepare(
        "SELECT p.id, p.name, p.created_at, p.updated_at, "
        "  (SELECT COUNT(*) FROM playlist_tracks WHERE playlist_id = p.id) AS track_count "
        "FROM playlists p ORDER BY p.name");
    if (!q.exec()) return result;

    while (q.next()) {
        PlaylistRecord r;
        r.id = q.value(0).toInt();
        r.name = q.value(1).toString();
        r.createdAt = q.value(2).toDateTime();
        r.updatedAt = q.value(3).toDateTime();
        r.trackCount = q.value(4).toInt();
        result.append(r);
    }
    return result;
}

PlaylistRecord PlaylistDao::getPlaylist(int id) {
    PlaylistRecord r;
    auto db = DatabaseManager::instance().database();
    QSqlQuery q(db);
    q.prepare("SELECT id, name, created_at, updated_at FROM playlists WHERE id = ?");
    q.addBindValue(id);
    if (q.exec() && q.next()) {
        r.id = q.value(0).toInt();
        r.name = q.value(1).toString();
        r.createdAt = q.value(2).toDateTime();
        r.updatedAt = q.value(3).toDateTime();
    }
    return r;
}

int PlaylistDao::createPlaylist(const QString& name) {
    auto db = DatabaseManager::instance().database();
    QSqlQuery q(db);
    q.prepare("INSERT INTO playlists (name) VALUES (?)");
    q.addBindValue(name);
    if (q.exec()) {
        int id = q.lastInsertId().toInt();
        emit playlistCreated(id, name);
        return id;
    }
    return -1;
}

bool PlaylistDao::renamePlaylist(int id, const QString& newName) {
    auto db = DatabaseManager::instance().database();
    QSqlQuery q(db);
    q.prepare("UPDATE playlists SET name = ?, updated_at = datetime('now') WHERE id = ?");
    q.addBindValue(newName);
    q.addBindValue(id);
    return q.exec();
}

bool PlaylistDao::deletePlaylist(int id) {
    auto db = DatabaseManager::instance().database();
    QSqlQuery q(db);
    q.prepare("DELETE FROM playlists WHERE id = ?");
    q.addBindValue(id);
    if (q.exec()) {
        emit playlistDeleted(id);
        return true;
    }
    return false;
}

QVector<PlaylistTrackRecord> PlaylistDao::getTracks(int playlistId) {
    QVector<PlaylistTrackRecord> result;
    auto db = DatabaseManager::instance().database();
    QSqlQuery q(db);
    q.prepare(
        "SELECT id, playlist_id, file_path, title, artist, album, duration_ms, track_order "
        "FROM playlist_tracks WHERE playlist_id = ? ORDER BY track_order");
    q.addBindValue(playlistId);
    if (!q.exec()) return result;

    while (q.next()) {
        PlaylistTrackRecord t;
        t.id = q.value(0).toInt();
        t.playlistId = q.value(1).toInt();
        t.filePath = q.value(2).toString();
        t.title = q.value(3).toString();
        t.artist = q.value(4).toString();
        t.album = q.value(5).toString();
        t.durationMs = q.value(6).toInt();
        t.trackOrder = q.value(7).toInt();
        result.append(t);
    }
    return result;
}

bool PlaylistDao::addTrack(int playlistId, const PlaylistTrackRecord& track) {
    auto db = DatabaseManager::instance().database();
    QSqlQuery q(db);

    // Get next order
    int nextOrder = 0;
    QSqlQuery countQ(db);
    countQ.prepare("SELECT MAX(track_order) FROM playlist_tracks WHERE playlist_id = ?");
    countQ.addBindValue(playlistId);
    if (countQ.exec() && countQ.next()) {
        nextOrder = countQ.value(0).toInt() + 1;
    }

    // Coalesce null QStrings → '' to avoid NOT NULL constraint failures.
    auto nn = [](const QString& s) { return s.isNull() ? QString(QLatin1String("")) : s; };

    q.prepare(
        "INSERT INTO playlist_tracks (playlist_id, file_path, title, artist, album, duration_ms, track_order) "
        "VALUES (?, ?, ?, ?, ?, ?, ?)");
    q.addBindValue(playlistId);
    q.addBindValue(nn(track.filePath));
    q.addBindValue(nn(track.title));
    q.addBindValue(nn(track.artist));
    q.addBindValue(nn(track.album));
    q.addBindValue(track.durationMs);
    q.addBindValue(nextOrder);

    if (q.exec()) {
        emit trackAdded(playlistId);
        return true;
    }
    return false;
}

bool PlaylistDao::addTracks(int playlistId, const QVector<PlaylistTrackRecord>& tracks) {
    auto db = DatabaseManager::instance().database();
    QSqlQuery q(db);

    int nextOrder = 0;
    QSqlQuery countQ(db);
    countQ.prepare("SELECT MAX(track_order) FROM playlist_tracks WHERE playlist_id = ?");
    countQ.addBindValue(playlistId);
    if (countQ.exec() && countQ.next()) {
        nextOrder = countQ.value(0).toInt() + 1;
    }

    auto nn = [](const QString& s) { return s.isNull() ? QString(QLatin1String("")) : s; };
    DatabaseManager::instance().beginTransaction();
    for (const auto& track : tracks) {
        q.prepare(
            "INSERT INTO playlist_tracks (playlist_id, file_path, title, artist, album, duration_ms, track_order) "
            "VALUES (?, ?, ?, ?, ?, ?, ?)");
        q.addBindValue(playlistId);
        q.addBindValue(nn(track.filePath));
        q.addBindValue(nn(track.title));
        q.addBindValue(nn(track.artist));
        q.addBindValue(nn(track.album));
        q.addBindValue(track.durationMs);
        q.addBindValue(nextOrder++);
        if (!q.exec()) {
            DatabaseManager::instance().rollbackTransaction();
            return false;
        }
    }
    DatabaseManager::instance().commitTransaction();
    emit trackAdded(playlistId);
    return true;
}

bool PlaylistDao::removeTrack(int trackId) {
    auto db = DatabaseManager::instance().database();
    QSqlQuery q(db);

    // Get playlist id before deleting
    q.prepare("SELECT playlist_id FROM playlist_tracks WHERE id = ?");
    q.addBindValue(trackId);
    int playlistId = -1;
    if (q.exec() && q.next()) {
        playlistId = q.value(0).toInt();
    }

    q.prepare("DELETE FROM playlist_tracks WHERE id = ?");
    q.addBindValue(trackId);
    if (q.exec()) {
        if (playlistId >= 0) emit trackRemoved(playlistId);
        return true;
    }
    return false;
}

bool PlaylistDao::reorderTrack(int trackId, int newOrder) {
    auto db = DatabaseManager::instance().database();
    QSqlQuery q(db);
    q.prepare("UPDATE playlist_tracks SET track_order = ? WHERE id = ?");
    q.addBindValue(newOrder);
    q.addBindValue(trackId);
    return q.exec();
}

bool PlaylistDao::clearTracks(int playlistId) {
    auto db = DatabaseManager::instance().database();
    QSqlQuery q(db);
    q.prepare("DELETE FROM playlist_tracks WHERE playlist_id = ?");
    q.addBindValue(playlistId);
    if (q.exec()) {
        emit trackRemoved(playlistId);
        return true;
    }
    return false;
}

} // namespace kivo::qt::data
