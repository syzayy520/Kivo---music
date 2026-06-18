#pragma once

#include <QAbstractListModel>
#include <QList>
#include <QString>

struct TrackListItem {
    QString title;
    QString artist;
    QString album;
    QString duration;
    int artVariant = 0;
    QString coverUrl;
    QString filePath;
};

class TrackListModel final : public QAbstractListModel {
    Q_OBJECT
    Q_PROPERTY(int count READ count NOTIFY countChanged)

public:
    enum Role {
        TitleRole = Qt::UserRole + 1,
        ArtistRole,
        AlbumRole,
        DurationRole,
        ArtVariantRole,
        CoverUrlRole,
        FilePathRole
    };

    explicit TrackListModel(QObject* parent = nullptr);

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

    int count() const;
    void setItems(QList<TrackListItem> items);
    QStringList filePaths() const;

signals:
    void countChanged();

private:
    QList<TrackListItem> m_items;
};
