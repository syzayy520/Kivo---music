#include "artist_list_model.h"

#include <utility>

ArtistListModel::ArtistListModel(QObject* parent)
    : QAbstractListModel(parent) {}

int ArtistListModel::rowCount(const QModelIndex& parent) const {
    if (parent.isValid()) {
        return 0;
    }
    return static_cast<int>(m_items.size());
}

QVariant ArtistListModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid() || index.row() < 0 || index.row() >= m_items.size()) {
        return {};
    }

    const auto& item = m_items.at(index.row());
    switch (role) {
    case NameRole:
        return item.name;
    case DetailRole:
        return item.detail;
    case ArtVariantRole:
        return item.artVariant;
    case CoverUrlRole:
        return item.coverUrl;
    default:
        return {};
    }
}

QHash<int, QByteArray> ArtistListModel::roleNames() const {
    return {
        {NameRole, "name"},
        {DetailRole, "detail"},
        {ArtVariantRole, "artVariant"},
        {CoverUrlRole, "coverUrl"},
    };
}

int ArtistListModel::count() const {
    return static_cast<int>(m_items.size());
}

void ArtistListModel::setItems(QList<ArtistListItem> items) {
    beginResetModel();
    m_items = std::move(items);
    endResetModel();
    emit countChanged();
}
