#pragma once

#include <QAbstractListModel>
#include <QList>
#include <QString>

struct ArtistListItem {
    QString name;
    QString detail;
    int artVariant = 0;
    QString coverUrl;
};

class ArtistListModel final : public QAbstractListModel {
    Q_OBJECT
    Q_PROPERTY(int count READ count NOTIFY countChanged)

public:
    enum Role {
        NameRole = Qt::UserRole + 1,
        DetailRole,
        ArtVariantRole,
        CoverUrlRole
    };

    explicit ArtistListModel(QObject* parent = nullptr);

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

    int count() const;
    void setItems(QList<ArtistListItem> items);

signals:
    void countChanged();

private:
    QList<ArtistListItem> m_items;
};
