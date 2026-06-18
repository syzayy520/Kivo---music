// =============================================================================
// Kivo Music Qt - library_scan_service.hpp
// 职责: UI 线程侧的库扫描门面——拥有工作线程，把结果以队列信号投递回 UI 线程
// =============================================================================

#pragma once

#include <QList>
#include <QObject>

#include <cstdint>

#include "../../sources/music/music_file_record.h"
#include "../../sources/music/music_source_root.h"

class QThread;

namespace kivo::qt::services {

class LibraryScanWorker;

// UI-thread object. Owns a dedicated worker thread that runs the blocking scan;
// resultsReady is delivered on the UI thread (where models may be mutated).
class LibraryScanService final : public QObject {
    Q_OBJECT

public:
    explicit LibraryScanService(QObject* parent = nullptr);
    ~LibraryScanService() override;

    // Kick off a scan on the worker thread. Safe to call from the UI thread.
    // dbPath (may be empty) lets the worker persist results to the library DB.
    void start(
        const QList<MusicSourceRoot>& roots,
        int maxFiles,
        const QString& dbPath = {});

signals:
    // Emitted on the UI thread (this object's affinity) when the scan completes.
    void resultsReady(QList<MusicFileRecord> records);

private:
    QThread* workerThread_ = nullptr;
    LibraryScanWorker* worker_ = nullptr;
    // Monotonic request id (UI thread only). Each start() bumps it so the worker
    // can skip any run() that a later start() has already superseded.
    std::uint64_t epochCounter_ = 0;
};

} // namespace kivo::qt::services
