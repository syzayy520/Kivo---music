// =============================================================================
// Kivo Music Qt - play_queue.hpp
// 职责: 播放队列——QAbstractListModel，直接绑定 QML ListView
// =============================================================================

#pragma once

#include <QAbstractListModel>
#include <QString>
#include <QStringList>
#include <QVector>

namespace kivo::qt::audio_bridge {

class PlayQueue : public QAbstractListModel {
    Q_OBJECT
    Q_PROPERTY(int currentIndex READ currentIndex NOTIFY currentTrackChanged)
    Q_PROPERTY(int count READ count NOTIFY queueChanged)

    enum Roles {
        FilePathRole = Qt::UserRole + 1,
        DisplayNameRole,
        ArtistRole,
        IndexRole
    };

public:
    explicit PlayQueue(QObject* parent = nullptr);
    ~PlayQueue() override = default;

    // QAbstractListModel 接口
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    // Queue management
    Q_INVOKABLE void addTrack(const QString& filePath);
    Q_INVOKABLE void addTracks(const QStringList& filePaths);
    Q_INVOKABLE void removeTrack(int index);
    Q_INVOKABLE void clear();
    Q_INVOKABLE void moveTrack(int fromIndex, int toIndex);

    // Playback control
    Q_INVOKABLE bool next();
    Q_INVOKABLE bool previous();
    Q_INVOKABLE void setCurrentIndex(int index);

    // Getters
    Q_INVOKABLE int currentIndex() const { return currentIndex_; }
    Q_INVOKABLE QString currentTrack() const;
    Q_INVOKABLE int count() const { return static_cast<int>(queue_.size()); }
    Q_INVOKABLE bool isEmpty() const { return queue_.isEmpty(); }
    Q_INVOKABLE bool hasNext() const;
    Q_INVOKABLE bool hasPrevious() const;
    Q_INVOKABLE QStringList getQueue() const;
    Q_INVOKABLE QString getTrackAt(int index) const;

signals:
    void queueChanged();
    void currentTrackChanged();
    void endOfQueue();

private:
    QString displayName(const QString& filePath) const;

    // PlayQueue is a QAbstractListModel: it is touched ONLY on the UI thread
    // (openFile / onEndOfStream / QML). No mutex — a QAbstractListModel cannot be
    // made thread-safe by one, and holding a lock across begin/endInsertRows
    // self-deadlocks (the attached view re-enters rowCount()/data()).
    QVector<QString> queue_;
    int currentIndex_ = -1;
};

} // namespace kivo::qt::audio_bridge
