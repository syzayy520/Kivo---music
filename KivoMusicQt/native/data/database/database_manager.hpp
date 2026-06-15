// =============================================================================
// Kivo Music Qt - database_manager.hpp
// 职责: SQLite 数据库生命周期管理（打开/关闭/迁移/事务）
// 单例模式，线程安全
// =============================================================================

#pragma once

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QMutex>
#include <QString>

namespace kivo::qt::data {

class DatabaseManager : public QObject {
    Q_OBJECT

public:
    static DatabaseManager& instance();

    // 初始化数据库（应用启动时调用一次）
    bool initialize(const QString& dbPath = {});

    // 获取数据库连接（用于 DAO 层）
    QSqlDatabase database() const;

    // 执行迁移（版本号管理）
    bool migrate(int targetVersion);

    // 事务操作
    bool beginTransaction();
    bool commitTransaction();
    bool rollbackTransaction();

    // 状态
    bool isOpen() const;
    int currentVersion() const;

signals:
    void opened();
    void migrationCompleted(int fromVersion, int toVersion);
    void error(const QString& message);

private:
    explicit DatabaseManager(QObject* parent = nullptr);
    ~DatabaseManager() override;
    DatabaseManager(const DatabaseManager&) = delete;
    DatabaseManager& operator=(const DatabaseManager&) = delete;

    bool createTables();
    bool runMigrationV1();
    QString defaultDbPath() const;

    QSqlDatabase db_;
    mutable QMutex mutex_;
    bool initialized_ = false;
    int schemaVersion_ = 0;
};

} // namespace kivo::qt::data
