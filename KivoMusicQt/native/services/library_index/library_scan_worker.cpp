// =============================================================================
// Kivo Music Qt - library_scan_worker.cpp
// =============================================================================

#include "library_scan_worker.hpp"

#include <QSqlDatabase>
#include <QSqlQuery>

#include "../../sources/music/music_file_scanner.h"
#include "library_repository.hpp"

namespace kivo::qt::services {

namespace {

// Persist on THIS (worker) thread via its own SQLite connection. SQLite
// connections are per-thread, so we must not reuse DatabaseManager's UI-thread
// "kivo_main" connection here. Opened, used, and removed within this call.
void persistLibrary(const QList<MusicFileRecord>& records, const QString& dbPath) {
    if (dbPath.isEmpty() || records.isEmpty()) {
        return;
    }
    const QString conn = QStringLiteral("kivo_library_worker");
    {
        QSqlDatabase db = QSqlDatabase::contains(conn)
            ? QSqlDatabase::database(conn, /*open=*/false)
            : QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"), conn);
        db.setDatabaseName(dbPath);
        if (db.isOpen() || db.open()) {
            QSqlQuery pragma(db);
            pragma.exec(QStringLiteral("PRAGMA journal_mode=WAL"));
            pragma.exec(QStringLiteral("PRAGMA foreign_keys=ON"));
            if (LibraryRepository(conn).upsertTracks(records)) {
                // Flush the just-committed rows from the WAL into the main .db file.
                // DatabaseManager's "kivo_main" connection stays open for the whole
                // app lifetime, so closing THIS connection never triggers the usual
                // last-connection checkpoint — without this, a hard kill would leave
                // every scanned row stranded in the -wal sidecar (the .db file itself
                // stays at its schema size and is incomplete on its own). PASSIVE is
                // deliberate: it copies frames without waiting on kivo_main's reader
                // and never returns SQLITE_BUSY, so persistence can't regress here.
                pragma.exec(QStringLiteral("PRAGMA wal_checkpoint(PASSIVE)"));
            }
            db.close();
        }
    }
    // The QSqlDatabase copy is out of scope now — safe to drop the connection.
    QSqlDatabase::removeDatabase(conn);
}

} // namespace

void LibraryScanWorker::run(
    QList<MusicSourceRoot> roots,
    int maxFiles,
    QString dbPath,
    std::uint64_t epoch) {
    // Superseded while queued? A newer start() (or shutdown) already bumped
    // latestRequested_, so skip entirely — no scan, no stale resultsReady. This
    // is what makes rapid source edits collapse to a single scan instead of N.
    if (latestRequested_.load(std::memory_order_acquire) != epoch) {
        return;
    }
    // We are the newest request: clear the abort flag and walk.
    cancelled_.store(false, std::memory_order_relaxed);
    const auto records = MusicFileScanner::scan(roots, maxFiles, &cancelled_);
    // Aborted mid-walk (shutdown) or superseded by an even-newer request: drop the
    // partial/stale results rather than persist them or signal a dying/changed UI.
    if (cancelled_.load(std::memory_order_relaxed) ||
        latestRequested_.load(std::memory_order_acquire) != epoch) {
        return;
    }
    persistLibrary(records, dbPath);
    emit scanFinished(records);
}

} // namespace kivo::qt::services
