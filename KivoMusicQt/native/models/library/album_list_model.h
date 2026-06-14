#pragma once

#include <QAbstractListModel>
#include <QList>
#include <QString>

struct AlbumListItem {
    QString title;
    QString subtitle;
    QString note;
    int artVariant = 0;
    QString coverUrl;
};

class AlbumListModel final : public QAbstractListModel {
    Q_OBJECT
    Q_PROPERTY(int count READ count NOTIFY countChanged)

public:
    enum Role {
        TitleRole = Qt::UserRole + 1,
        SubtitleRole,
        NoteRole,
        ArtVariantRole,
        CoverUrlRole
    };

    explicit AlbumListModel(QObject* parent = nullptr);

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

    int count() const;
    void setItems(QList<AlbumListItem> items);

signals:
    void countChanged();

private:
    QList<AlbumListItem> m_items;
};
