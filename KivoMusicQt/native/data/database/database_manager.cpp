// =============================================================================
// Kivo Music Qt - database_manager.cpp
// 职责: SQLite 数据库生命周期管理实现
// =============================================================================

#include "database_manager.hpp"

#include <QCoreApplication>
#include <QDir>
#include <QSqlError>
#include <QStandardPaths>
#include <QDebug>

namespace kivo::qt::data {

DatabaseManager& DatabaseManager::instance() {
    static DatabaseManager inst;
    return inst;
}

DatabaseManager::DatabaseManager(QObject* parent) : QObject(parent) {}

DatabaseManager::~DatabaseManager() {
    if (db_.isOpen()) {
        db_.close();
    }
}

bool DatabaseManager::initialize(const QString& dbPath) {
    QMutexLocker locker(&mutex_);

    if (initialized_) return true;

    const QString path = dbPath.isEmpty() ? defaultDbPath() : dbPath;

    // Ensure directory exists
    QDir().mkpath(QFileInfo(path).absolutePath());

    db_ = QSqlDatabase::addDatabase("QSQLITE", "kivo_main");
    db_.setDatabaseName(path);

    if (!db_.open()) {
        qWarning() << "DatabaseManager: failed to open" << path << db_.lastError().text();
        emit error(db_.lastError().text());
        return false;
    }

    // Enable WAL mode for better concurrent read performance
    {
        QSqlQuery q(db_);
        q.exec("PRAGMA journal_mode=WAL");
        q.exec("PRAGMA foreign_keys=ON");
    }

    if (!createTables()) {
        qWarning() << "DatabaseManager: table creation failed";
        return false;
    }

    initialized_ = true;
    emit opened();
    return true;
}

QSqlDatabase DatabaseManager::database() const {
    return db_;
}

bool DatabaseManager::isOpen() const {
    return initialized_ && db_.isOpen();
}

int DatabaseManager::currentVersion() const {
    return schemaVersion_;
}

QString DatabaseManager::defaultDbPath() const {
    const QString appData = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
    return appData + "/kivo_music.db";
}

bool DatabaseManager::createTables() {
    QSqlQuery q(db_);

    // ── Schema version tracking ──────────────────────────
    if (!q.exec(
        "CREATE TABLE IF NOT EXISTS schema_version ("
        "  version INTEGER PRIMARY KEY,"
        "  applied_at TEXT NOT NULL DEFAULT (datetime('now'))"
        ")")) {
        qWarning() << "schema_version table:" << q.lastError().text();
        return false;
    }

    // ── Playlists ────────────────────────────────────────
    if (!q.exec(
        "CREATE TABLE IF NOT EXISTS playlists ("
        "  id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "  name TEXT NOT NULL,"
        "  created_at TEXT NOT NULL DEFAULT (datetime('now')),"
        "  updated_at TEXT NOT NULL DEFAULT (datetime('now'))"
        ")")) {
        qWarning() << "playlists table:" << q.lastError().text();
        return false;
    }

    // ── Playlist tracks (M:N relationship) ──────────────
    if (!q.exec(
        "CREATE TABLE IF NOT EXISTS playlist_tracks ("
        "  id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "  playlist_id INTEGER NOT NULL REFERENCES playlists(id) ON DELETE CASCADE,"
        "  file_path TEXT NOT NULL,"
        "  title TEXT NOT NULL DEFAULT '',"
        "  artist TEXT NOT NULL DEFAULT '',"
        "  album TEXT NOT NULL DEFAULT '',"
        "  duration_ms INTEGER NOT NULL DEFAULT 0,"
        "  track_order INTEGER NOT NULL DEFAULT 0,"
        "  added_at TEXT NOT NULL DEFAULT (datetime('now'))"
        ")")) {
        qWarning() << "playlist_tracks table:" << q.lastError().text();
        return false;
    }

    // ── Library index ────────────────────────────────────
    if (!q.exec(
        "CREATE TABLE IF NOT EXISTS library ("
        "  id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "  file_path TEXT UNIQUE NOT NULL,"
        "  title TEXT NOT NULL DEFAULT '',"
        "  artist TEXT NOT NULL DEFAULT '',"
        "  album TEXT NOT NULL DEFAULT '',"
        "  format TEXT NOT NULL DEFAULT '',"
        "  sample_rate INTEGER NOT NULL DEFAULT 0,"
        "  bit_depth INTEGER NOT NULL DEFAULT 0,"
        "  channels INTEGER NOT NULL DEFAULT 0,"
        "  duration_ms INTEGER NOT NULL DEFAULT 0,"
        "  file_size INTEGER NOT NULL DEFAULT 0,"
        "  cover_path TEXT NOT NULL DEFAULT '',"
        "  added_at TEXT NOT NULL DEFAULT (datetime('now')),"
        "  last_played TEXT"
        ")")) {
        qWarning() << "library table:" << q.lastError().text();
        return false;
    }

    // ── Play history ─────────────────────────────────────
    if (!q.exec(
        "CREATE TABLE IF NOT EXISTS play_history ("
        "  id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "  file_path TEXT NOT NULL,"
        "  title TEXT NOT NULL DEFAULT '',"
        "  artist TEXT NOT NULL DEFAULT '',"
        "  played_at TEXT NOT NULL DEFAULT (datetime('now'))"
        ")")) {
        qWarning() << "play_history table:" << q.lastError().text();
        return false;
    }

    // ── App settings key-value store ─────────────────────
    if (!q.exec(
        "CREATE TABLE IF NOT EXISTS app_settings ("
        "  key TEXT PRIMARY KEY,"
        "  value TEXT NOT NULL"
        ")")) {
        qWarning() << "app_settings table:" << q.lastError().text();
        return false;
    }

    // Insert initial schema version
    q.exec("INSERT OR IGNORE INTO schema_version (version) VALUES (1)");
    schemaVersion_ = 1;

    return true;
}

bool DatabaseManager::beginTransaction() {
    return db_.transaction();
}

bool DatabaseManager::commitTransaction() {
    return db_.commit();
}

bool DatabaseManager::rollbackTransaction() {
    return db_.rollback();
}

bool DatabaseManager::migrate(int targetVersion) {
    Q_UNUSED(targetVersion);
    // Future: versioned migrations
    return true;
}

} // namespace kivo::qt::data
