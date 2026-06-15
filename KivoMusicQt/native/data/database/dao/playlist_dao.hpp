// =============================================================================
// Kivo Music Qt - playlist_dao.hpp
// 职责: 播放列表数据访问对象——CRUD 操作，替代 playlist_manager JSON
// =============================================================================

#pragma once

#include <QObject>
#include <QString>
#include <QStringList>
#include <QVector>
#include <QDateTime>

namespace kivo::qt::data {

struct PlaylistRecord {
    int id = 0;
    QString name;
    QDateTime createdAt;
    QDateTime updatedAt;
    int trackCount = 0;
};

struct PlaylistTrackRecord {
    int id = 0;
    int playlistId = 0;
    QString filePath;
    QString title;
    QString artist;
    QString album;
    int durationMs = 0;
    int trackOrder = 0;
};

class PlaylistDao : public QObject {
    Q_OBJECT

public:
    explicit PlaylistDao(QObject* parent = nullptr);

    // Playlist CRUD
    QVector<PlaylistRecord> getAllPlaylists();
    PlaylistRecord getPlaylist(int id);
    int createPlaylist(const QString& name);
    bool renamePlaylist(int id, const QString& newName);
    bool deletePlaylist(int id);

    // Track management
    QVector<PlaylistTrackRecord> getTracks(int playlistId);
    bool addTrack(int playlistId, const PlaylistTrackRecord& track);
    bool addTracks(int playlistId, const QVector<PlaylistTrackRecord>& tracks);
    bool removeTrack(int trackId);
    bool reorderTrack(int trackId, int newOrder);
    bool clearTracks(int playlistId);

signals:
    void playlistCreated(int id, const QString& name);
    void playlistDeleted(int id);
    void trackAdded(int playlistId);
    void trackRemoved(int playlistId);
};

} // namespace kivo::qt::data
