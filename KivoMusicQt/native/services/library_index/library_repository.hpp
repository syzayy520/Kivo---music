// =============================================================================
// Kivo Music Qt - library_repository.hpp
// 职责: library 表的读写（按连接名操作，便于每线程各自持有连接）
// =============================================================================

#pragma once

#include <QList>
#include <QString>

#include "../../sources/music/music_file_record.h"

namespace kivo::qt::services {

// Operates on an ALREADY-OPEN QSqlDatabase identified by connection name. Holds
// no connection itself, so the same repository code runs on the UI thread
// ("kivo_main", read) and the library worker thread (its own connection, write)
// without sharing a connection across threads.
class LibraryRepository final {
public:
    explicit LibraryRepository(QString connectionName);

    // Insert-or-update by file_path (UNIQUE). Transactional. Maps the v1 library
    // columns; sourceLabel/artVariant are not persisted in v1 (re-derived by the
    // projections + the async rescan).
    bool upsertTracks(const QList<MusicFileRecord>& records) const;

    [[nodiscard]] QList<MusicFileRecord> allTracks() const;

private:
    QString connectionName_;
};

} // namespace kivo::qt::services
