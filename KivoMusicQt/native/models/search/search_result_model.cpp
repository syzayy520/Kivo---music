#include "search_result_model.h"

SearchResultModel::SearchResultModel(QObject* parent)
    : QAbstractListModel(parent) {}

int SearchResultModel::rowCount(const QModelIndex& parent) const {
    if (parent.isValid()) {
        return 0;
    }
    return static_cast<int>(m_items.size());
}

QVariant SearchResultModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid() || index.row() < 0 || index.row() >= m_items.size()) {
        return {};
    }

    const auto& item = m_items.at(index.row());
    switch (role) {
    case TitleRole:
        return item.title;
    case DetailRole:
        return item.detail;
    case CategoryRole:
        return item.category;
    case ArtVariantRole:
        return item.artVariant;
    case CoverUrlRole:
        return item.coverUrl;
    default:
        return {};
    }
}

QHash<int, QByteArray> SearchResultModel::roleNames() const {
    return {
        {TitleRole, "title"},
        {DetailRole, "detail"},
        {CategoryRole, "category"},
        {ArtVariantRole, "artVariant"},
        {CoverUrlRole, "coverUrl"},
    };
}

void SearchResultModel::setItems(QList<SearchResultItem> items) {
    beginResetModel();
    m_items = std::move(items);
    endResetModel();
}
