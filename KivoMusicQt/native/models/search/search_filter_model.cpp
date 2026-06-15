#include "search_filter_model.h"

SearchFilterModel::SearchFilterModel(QObject* parent)
    : QAbstractListModel(parent) {}

int SearchFilterModel::rowCount(const QModelIndex& parent) const {
    if (parent.isValid()) {
        return 0;
    }
    return static_cast<int>(m_items.size());
}

QVariant SearchFilterModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid() || index.row() < 0 || index.row() >= m_items.size()) {
        return {};
    }

    const auto& item = m_items.at(index.row());
    if (role == TitleRole) {
        return item.title;
    }
    return {};
}

QHash<int, QByteArray> SearchFilterModel::roleNames() const {
    return {
        {TitleRole, "title"},
    };
}

void SearchFilterModel::setItems(QList<SearchFilterItem> items) {
    beginResetModel();
    m_items = std::move(items);
    endResetModel();
}
