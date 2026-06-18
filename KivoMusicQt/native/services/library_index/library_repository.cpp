// =============================================================================
// Kivo Music Qt - library_repository.cpp
// =============================================================================

#include "library_repository.hpp"

#include <QLatin1String>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QVariant>

#include <utility>

namespace kivo::qt::services {

LibraryRepository::LibraryRepository(QString connectionName)
    : connectionName_(std::move(connectionName)) {}

bool LibraryRepository::upsertTracks(
    const QList<MusicFileRecord>& records) const {
    QSqlDatabase db = QSqlDatabase::database(connectionName_, /*open=*/false);
    if (!db.isOpen()) {
        return false;
    }
    db.transaction();
    QSqlQuery query(db);
    query.prepare(QStringLiteral(
        "INSERT INTO library(file_path, title, artist, album, format, cover_path) "
        "VALUES(?, ?, ?, ?, ?, ?) "
        "ON CONFLICT(file_path) DO UPDATE SET "
        "title = excluded.title, artist = excluded.artist, "
        "album = excluded.album, format = excluded.format, "
        "cover_path = excluded.cover_path"));
    // Bind NULL-safe: the scanner leaves cover_path (and possibly other fields) as
    // a *null* QString when absent, and Qt's SQLITE driver binds a null QString as
    // SQL NULL — which violates the schema's `... TEXT NOT NULL` columns. (A column
    // DEFAULT only fills OMITTED columns, never an explicitly-bound NULL.) Coalesce
    // to a non-null empty string so "no value" round-trips as '' per the v1 schema;
    // a single null cover_path used to abort the whole transaction → nothing saved.
    const auto bindText = [&query](const QString& value) {
        query.addBindValue(value.isNull() ? QString(QLatin1String("")) : value);
    };
    for (const auto& record : records) {
        bindText(record.path);
        bindText(record.title);
        bindText(record.artist);
        bindText(record.album);
        bindText(record.formatLabel);
        bindText(record.coverPath);
        if (!query.exec()) {
            db.rollback();
            return false;
        }
    }
    return db.commit();
}

QList<MusicFileRecord> LibraryRepository::allTracks() const {
    QList<MusicFileRecord> out;
    QSqlDatabase db = QSqlDatabase::database(connectionName_, /*open=*/false);
    if (!db.isOpen()) {
        return out;
    }
    QSqlQuery query(db);
    if (!query.exec(QStringLiteral(
            "SELECT file_path, title, artist, album, format, cover_path "
            "FROM library ORDER BY artist, album, title"))) {
        return out;
    }
    while (query.next()) {
        MusicFileRecord record;
        record.path = query.value(0).toString();
        record.title = query.value(1).toString();
        record.artist = query.value(2).toString();
        record.album = query.value(3).toString();
        record.formatLabel = query.value(4).toString();
        record.coverPath = query.value(5).toString();
        out.append(record);
    }
    return out;
}

} // namespace kivo::qt::services
