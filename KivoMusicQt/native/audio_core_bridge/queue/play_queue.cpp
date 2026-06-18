// =============================================================================
// Kivo Music Qt - play_queue.cpp
// 职责: 播放队列——QAbstractListModel 实现
// 线程: 仅 UI 线程访问（QAbstractListModel 本就不可跨线程）。不加锁——在
//       begin/endInsertRows 期间持锁会被附加视图重入 rowCount()/data() 而自死锁。
// =============================================================================

#include "play_queue.hpp"
#include "../../sources/music/music_filename_parser.h"
#include <QFileInfo>

namespace kivo::qt::audio_bridge {

PlayQueue::PlayQueue(QObject* parent) : QAbstractListModel(parent) {
    currentIndex_ = -1;
}

int PlayQueue::rowCount(const QModelIndex& parent) const {
    Q_UNUSED(parent);
    return static_cast<int>(queue_.size());
}

QVariant PlayQueue::data(const QModelIndex& index, int role) const {
    if (!index.isValid() || index.row() < 0 || index.row() >= queue_.size())
        return {};

    const QString& path = queue_.at(index.row());
    switch (role) {
        case FilePathRole:    return path;
        case DisplayNameRole: return displayName(path);
        case ArtistRole:      return MusicFilenameParser::parse(QFileInfo(path).completeBaseName()).artist;
        case IndexRole:       return index.row();
        default:              return {};
    }
}

QHash<int, QByteArray> PlayQueue::roleNames() const {
    return {
        {FilePathRole,    "filePath"},
        {DisplayNameRole, "displayName"},
        {ArtistRole,      "artist"},
        {IndexRole,       "trackIndex"}
    };
}

QString PlayQueue::displayName(const QString& filePath) const {
    // Use the SAME parser as the library so the queue shows the song TITLE (the
    // last "Artist - Title" segment), not the artist. One source of truth.
    return MusicFilenameParser::parse(QFileInfo(filePath).completeBaseName()).title;
}

void PlayQueue::addTrack(const QString& filePath) {
    const int pos = queue_.size();
    beginInsertRows(QModelIndex(), pos, pos);
    queue_.append(filePath);
    endInsertRows();

    if (queue_.size() == 1) {
        currentIndex_ = 0;
        emit currentTrackChanged();
    }
    emit queueChanged();
}

void PlayQueue::addTracks(const QStringList& filePaths) {
    if (filePaths.isEmpty()) return;

    const int oldSize = queue_.size();
    beginInsertRows(QModelIndex(), oldSize, oldSize + filePaths.size() - 1);
    queue_.append(filePaths);
    endInsertRows();

    if (oldSize == 0 && !queue_.isEmpty()) {
        currentIndex_ = 0;
        emit currentTrackChanged();
    }
    emit queueChanged();
}

void PlayQueue::removeTrack(int index) {
    if (index < 0 || index >= queue_.size()) return;

    beginRemoveRows(QModelIndex(), index, index);
    queue_.removeAt(index);
    endRemoveRows();

    if (index == currentIndex_) {
        if (queue_.isEmpty()) currentIndex_ = -1;
        else if (currentIndex_ >= queue_.size()) currentIndex_ = queue_.size() - 1;
        emit currentTrackChanged();
    } else if (index < currentIndex_) {
        currentIndex_--;
    }

    emit queueChanged();
}

void PlayQueue::clear() {
    if (queue_.isEmpty()) return;
    beginResetModel();
    queue_.clear();
    endResetModel();
    currentIndex_ = -1;
    emit currentTrackChanged();
    emit queueChanged();
}

void PlayQueue::moveTrack(int fromIndex, int toIndex) {
    if (fromIndex < 0 || fromIndex >= queue_.size() ||
        toIndex < 0 || toIndex >= queue_.size()) return;
    if (fromIndex == toIndex) return;

    beginMoveRows(QModelIndex(), fromIndex, fromIndex, QModelIndex(), toIndex > fromIndex ? toIndex + 1 : toIndex);
    QString track = queue_.takeAt(fromIndex);
    queue_.insert(toIndex, track);
    endMoveRows();

    if (currentIndex_ == fromIndex) currentIndex_ = toIndex;
    else if (fromIndex < currentIndex_ && toIndex >= currentIndex_) currentIndex_--;
    else if (fromIndex > currentIndex_ && toIndex <= currentIndex_) currentIndex_++;

    emit queueChanged();
}

bool PlayQueue::next() {
    if (queue_.isEmpty()) return false;
    if (currentIndex_ + 1 < queue_.size()) {
        currentIndex_++;
        emit currentTrackChanged();
        return true;
    }
    emit endOfQueue();
    return false;
}

bool PlayQueue::previous() {
    if (queue_.isEmpty()) return false;
    if (currentIndex_ > 0) {
        currentIndex_--;
        emit currentTrackChanged();
        return true;
    }
    return false;
}

void PlayQueue::setCurrentIndex(int index) {
    if (index >= 0 && index < queue_.size()) {
        currentIndex_ = index;
        emit currentTrackChanged();
    }
}

QString PlayQueue::currentTrack() const {
    if (currentIndex_ >= 0 && currentIndex_ < queue_.size())
        return queue_.at(currentIndex_);
    return {};
}

bool PlayQueue::hasNext() const {
    return currentIndex_ + 1 < queue_.size();
}

bool PlayQueue::hasPrevious() const {
    return currentIndex_ > 0;
}

QStringList PlayQueue::getQueue() const {
    QStringList result;
    result.reserve(queue_.size());
    for (const auto& p : queue_) result.append(p);
    return result;
}

QString PlayQueue::getTrackAt(int index) const {
    if (index >= 0 && index < queue_.size())
        return queue_.at(index);
    return {};
}

} // namespace kivo::qt::audio_bridge
