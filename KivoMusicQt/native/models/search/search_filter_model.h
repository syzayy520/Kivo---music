#pragma once

#include <QAbstractListModel>
#include <QList>
#include <QString>

struct SearchFilterItem {
    QString title;
};

class SearchFilterModel final : public QAbstractListModel {
    Q_OBJECT

public:
    enum Role {
        TitleRole = Qt::UserRole + 1
    };

    explicit SearchFilterModel(QObject* parent = nullptr);

    int rowCount(const QModelIndex& parent = {}) const override;
    QVariant data(const QModelIndex& index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

    void setItems(QList<SearchFilterItem> items);

private:
    QList<SearchFilterItem> m_items;
};
