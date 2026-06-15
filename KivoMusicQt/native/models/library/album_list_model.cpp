#include "album_list_model.h"

#include <utility>

AlbumListModel::AlbumListModel(QObject* parent)
    : QAbstractListModel(parent) {}

int AlbumListModel::rowCount(const QModelIndex& parent) const {
    if (parent.isValid()) {
        return 0;
    }
    return static_cast<int>(m_items.size());
}

QVariant AlbumListModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid() || index.row() < 0 || index.row() >= m_items.size()) {
        return {};
    }

    const auto& item = m_items.at(index.row());
    switch (role) {
    case TitleRole:
        return item.title;
    case SubtitleRole:
        return item.subtitle;
    case NoteRole:
        return item.note;
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

QHash<int, QByteArray> AlbumListModel::roleNames() const {
    return {
        {TitleRole, "title"},
        {SubtitleRole, "subtitle"},
        {NoteRole, "note"},
        {ArtVariantRole, "artVariant"},
        {CoverUrlRole, "coverUrl"},
        {FilePathRole, "filePath"},
    };
}

int AlbumListModel::count() const {
    return static_cast<int>(m_items.size());
}

void AlbumListModel::setItems(QList<AlbumListItem> items) {
    beginResetModel();
    m_items = std::move(items);
    endResetModel();
    emit countChanged();
}
