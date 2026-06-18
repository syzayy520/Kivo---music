#include "track_list_model.h"

#include <utility>

TrackListModel::TrackListModel(QObject* parent)
    : QAbstractListModel(parent) {}

int TrackListModel::rowCount(const QModelIndex& parent) const {
    if (parent.isValid()) {
        return 0;
    }
    return static_cast<int>(m_items.size());
}

QVariant TrackListModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid() || index.row() < 0 || index.row() >= m_items.size()) {
        return {};
    }

    const auto& item = m_items.at(index.row());
    switch (role) {
    case TitleRole:
        return item.title;
    case ArtistRole:
        return item.artist;
    case AlbumRole:
        return item.album;
    case DurationRole:
        return item.duration;
    case ArtVariantRole:
        return item.artVariant;
    case CoverUrlRole:
        return item.coverUrl;
    case FilePathRole:
        return item.filePath;
    default:
        return {};
    }
}

QHash<int, QByteArray> TrackListModel::roleNames() const {
    return {
        {TitleRole, "title"},
        {ArtistRole, "artist"},
        {AlbumRole, "album"},
        {DurationRole, "duration"},
        {ArtVariantRole, "artVariant"},
        {CoverUrlRole, "coverUrl"},
        {FilePathRole, "filePath"},
    };
}

int TrackListModel::count() const {
    return static_cast<int>(m_items.size());
}

QStringList TrackListModel::filePaths() const {
    QStringList result;
    result.reserve(m_items.size());
    for (const auto& item : m_items) result.append(item.filePath);
    return result;
}

void TrackListModel::setItems(QList<TrackListItem> items) {
    beginResetModel();
    m_items = std::move(items);
    endResetModel();
    emit countChanged();
}
