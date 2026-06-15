// =============================================================================
// Kivo Music Qt - play_queue.cpp
// 职责: 播放队列——QAbstractListModel 实现
// =============================================================================

#include "play_queue.hpp"
#include <QFileInfo>

namespace kivo::qt::audio_bridge {

PlayQueue::PlayQueue(QObject* parent) : QAbstractListModel(parent) {
    currentIndex_ = -1;
}

int PlayQueue::rowCount(const QModelIndex& parent) const {
    Q_UNUSED(parent);
    QMutexLocker locker(&mutex_);
    return static_cast<int>(queue_.size());
}

QVariant PlayQueue::data(const QModelIndex& index, int role) const {
    QMutexLocker locker(&mutex_);
    if (!index.isValid() || index.row() < 0 || index.row() >= queue_.size())
        return {};

    const QString& path = queue_.at(index.row());
    switch (role) {
        case FilePathRole: return path;
        case DisplayNameRole: return displayName(path);
        case IndexRole: return index.row();
        default: return {};
    }
}

QHash<int, QByteArray> PlayQueue::roleNames() const {
    return {
        {FilePathRole, "filePath"},
        {DisplayNameRole, "displayName"},
        {IndexRole, "trackIndex"}
    };
}

QString PlayQueue::displayName(const QString& filePath) const {
    QFileInfo fi(filePath);
    const QString base = fi.completeBaseName();
    const QStringList parts = base.split(" - ");
    if (parts.size() >= 2) {
        return parts[1].trimmed();
    }
    return base;
}

void PlayQueue::addTrack(const QString& filePath) {
    {
        QMutexLocker locker(&mutex_);
        const int pos = queue_.size();
        beginInsertRows(QModelIndex(), pos, pos);
        queue_.append(filePath);
        endInsertRows();

        if (queue_.size() == 1) {
            currentIndex_ = 0;
            emit currentTrackChanged();
        }
    }
    emit queueChanged();
}

void PlayQueue::addTracks(const QStringList& filePaths) {
    if (filePaths.isEmpty()) return;

    {
        QMutexLocker locker(&mutex_);
        const int oldSize = queue_.size();
        beginInsertRows(QModelIndex(), oldSize, oldSize + filePaths.size() - 1);
        queue_.append(filePaths);
        endInsertRows();

        if (oldSize == 0 && !queue_.isEmpty()) {
            currentIndex_ = 0;
            emit currentTrackChanged();
        }
    }
    emit queueChanged();
}

void PlayQueue::removeTrack(int index) {
    QMutexLocker locker(&mutex_);
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
    QMutexLocker locker(&mutex_);
    if (queue_.isEmpty()) return;
    beginResetModel();
    queue_.clear();
    endResetModel();
    currentIndex_ = -1;
    emit currentTrackChanged();
    emit queueChanged();
}

void PlayQueue::moveTrack(int fromIndex, int toIndex) {
    QMutexLocker locker(&mutex_);
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
    QMutexLocker locker(&mutex_);
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
    QMutexLocker locker(&mutex_);
    if (queue_.isEmpty()) return false;
    if (currentIndex_ > 0) {
        currentIndex_--;
        emit currentTrackChanged();
        return true;
    }
    return false;
}

void PlayQueue::setCurrentIndex(int index) {
    QMutexLocker locker(&mutex_);
    if (index >= 0 && index < queue_.size()) {
        currentIndex_ = index;
        emit currentTrackChanged();
    }
}

QString PlayQueue::currentTrack() const {
    QMutexLocker locker(&mutex_);
    if (currentIndex_ >= 0 && currentIndex_ < queue_.size())
        return queue_.at(currentIndex_);
    return {};
}

bool PlayQueue::hasNext() const {
    QMutexLocker locker(&mutex_);
    return currentIndex_ + 1 < queue_.size();
}

bool PlayQueue::hasPrevious() const {
    QMutexLocker locker(&mutex_);
    return currentIndex_ > 0;
}

QStringList PlayQueue::getQueue() const {
    QMutexLocker locker(&mutex_);
    QStringList result;
    result.reserve(queue_.size());
    for (const auto& p : queue_) result.append(p);
    return result;
}

QString PlayQueue::getTrackAt(int index) const {
    QMutexLocker locker(&mutex_);
    if (index >= 0 && index < queue_.size())
        return queue_.at(index);
    return {};
}

} // namespace kivo::qt::audio_bridge
