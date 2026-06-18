// =============================================================================
// Kivo Music Qt - library_scan_worker.hpp
// 职责: 在库工作线程上运行文件扫描（不触碰任何 GUI/模型对象）
// =============================================================================

#pragma once

#include <QList>
#include <QObject>

#include <atomic>
#include <cstdint>

#include "../../sources/music/music_file_record.h"
#include "../../sources/music/music_source_root.h"

namespace kivo::qt::services {

// Lives on the library worker thread (moved there by LibraryScanService). It only
// produces value-typed records — it never touches a QAbstractListModel or emits a
// QML-bound signal.
class LibraryScanWorker final : public QObject {
    Q_OBJECT

public:
    using QObject::QObject;

    // Invoked on the worker thread (via a queued functor). Blocking disk walk;
    // then persists to the library DB via a connection opened on THIS thread
    // (dbPath empty => skip persistence). `epoch` identifies this request; if a
    // newer request (or shutdown) has arrived by the time this dequeues, it bails
    // without scanning or emitting (see request()/requestCancel()).
    void run(QList<MusicSourceRoot> roots, int maxFiles, QString dbPath,
             std::uint64_t epoch);

    // Mark a NEW scan request (UI thread): record it as the newest epoch and
    // abort any in-flight scan so the superseded one drops out promptly.
    void request(std::uint64_t epoch) {
        latestRequested_.store(epoch, std::memory_order_release);
        cancelled_.store(true, std::memory_order_relaxed);
    }

    // Shutdown abort (UI thread): supersede EVERY queued/in-flight run so none
    // scans or emits, and the worker thread returns promptly.
    void requestCancel() {
        latestRequested_.store(~std::uint64_t{0}, std::memory_order_release);
        cancelled_.store(true, std::memory_order_relaxed);
    }

signals:
    void scanFinished(QList<MusicFileRecord> records);

private:
    std::atomic<bool> cancelled_{false};
    std::atomic<std::uint64_t> latestRequested_{0};
};

} // namespace kivo::qt::services
