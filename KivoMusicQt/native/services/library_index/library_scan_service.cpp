// =============================================================================
// Kivo Music Qt - library_scan_service.cpp
// =============================================================================

#include "library_scan_service.hpp"

#include <QThread>

#include "library_scan_worker.hpp"

namespace kivo::qt::services {

LibraryScanService::LibraryScanService(QObject* parent)
    : QObject(parent),
      workerThread_(new QThread(this)),
      worker_(new LibraryScanWorker) {  // no parent — ownership moves to thread
    worker_->moveToThread(workerThread_);
    // Worker is destroyed on its own thread once it finishes.
    connect(workerThread_, &QThread::finished,
            worker_, &QObject::deleteLater);
    // Cross-thread (worker -> this UI object): AutoConnection resolves to Queued,
    // so resultsReady is emitted on the UI thread.
    connect(worker_, &LibraryScanWorker::scanFinished,
            this, &LibraryScanService::resultsReady);
    workerThread_->start();
}

LibraryScanService::~LibraryScanService() {
    // Abort any in-flight scan FIRST so the join below can't hang waiting for a
    // stuck/slow network walk to finish (the bounded reachability probe + the
    // per-file cancel check make the worker return promptly).
    if (worker_) {
        worker_->requestCancel();
    }
    workerThread_->quit();
    // Bounded join: the reachability probe + per-file cancel make the worker
    // return promptly in normal cases. If a mounted-but-hung network share blocks
    // a single QDirIterator syscall past the budget (uninterruptible), do NOT
    // terminate() — killing the thread mid-allocation can corrupt the heap shared
    // with the still-exiting UI thread. Instead detach the QThread from this
    // parent and leak it: the OS reclaims it at process exit, the worker's emit is
    // already suppressed (requestCancel), and worker_ still deleteLater's itself
    // once the syscall returns and its event loop drains the queued quit().
    if (!workerThread_->wait(3000)) {
        workerThread_->setParent(nullptr);  // intentionally leaked (shutdown only)
    }
}

void LibraryScanService::start(
    const QList<MusicSourceRoot>& roots,
    int maxFiles,
    const QString& dbPath) {
    // Tag this request with the next epoch and mark it the newest. request()
    // aborts any in-flight scan AND ensures every already-queued (older) run()
    // bails on dequeue, so rapid re-scans (e.g. editing sources) collapse to one
    // scan instead of running each superseded request to completion.
    const std::uint64_t epoch = ++epochCounter_;
    if (worker_) {
        worker_->request(epoch);
    }
    // Copies are captured on the UI thread (happens-before the worker run) and
    // moved into the worker thread's event loop — never write worker_ directly.
    QMetaObject::invokeMethod(
        worker_,
        [w = worker_, roots, maxFiles, dbPath, epoch]() {
            w->run(roots, maxFiles, dbPath, epoch);
        },
        Qt::QueuedConnection);
}

} // namespace kivo::qt::services
