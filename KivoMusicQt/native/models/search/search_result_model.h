#pragma once

#include <QAbstractListModel>
#include <QList>
#include <QString>

struct SearchResultItem {
    QString title;
    QString detail;
    QString category;
    int artVariant = 0;
    QString coverUrl;
    QString filePath;
};

class SearchResultModel final : public QAbstractListModel {
    Q_OBJECT

public:
    enum Role {
        TitleRole = Qt::UserRole + 1,
        DetailRole,
        CategoryRole,
        ArtVariantRole,
        CoverUrlRole,
        FilePathRole
    };

    explicit SearchResultModel(QObject* parent = nullptr);

    int rowCount(const QModelIndex& parent = {}) const override;
    QVariant data(const QModelIndex& index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

    void setItems(QList<SearchResultItem> items);

private:
    QList<SearchResultItem> m_items;
};
