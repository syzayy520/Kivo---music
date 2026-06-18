// =============================================================================
// Kivo Music Qt - smb_discovery_controller.cpp
// =============================================================================

#include "smb_discovery_controller.hpp"

#include <QThread>
#include <QVariantMap>

#include "smb_discovery_worker.hpp"
#include "../roots/source_manager_controller.hpp"

namespace kivo::qt::sources {

SmbDiscoveryController::SmbDiscoveryController(
    SourceManagerController* sources,
    QObject* parent)
    : QObject(parent),
      sources_(sources),
      workerThread_(new QThread(this)),
      worker_(new SmbDiscoveryWorker),  // no parent — ownership moves to thread
      statusText_(QStringLiteral("扫描局域网以发现 NAS 共享")) {
    worker_->moveToThread(workerThread_);
    connect(workerThread_, &QThread::finished,
            worker_, &QObject::deleteLater);
    // Cross-thread (worker -> UI): AutoConnection resolves to Queued.
    connect(worker_, &SmbDiscoveryWorker::shareFound,
            this, &SmbDiscoveryController::onShareFound);
    connect(worker_, &SmbDiscoveryWorker::finished,
            this, &SmbDiscoveryController::onFinished);
    workerThread_->start();
}

SmbDiscoveryController::~SmbDiscoveryController() {
    if (worker_) {
        worker_->requestCancel();
    }
    workerThread_->quit();
    // Bounded join: the per-host probe budget + cancel checks make the worker
    // return promptly. If a host's NetShareEnum RPC blocks past the budget, do
    // NOT terminate() — killing the thread mid-allocation can corrupt the heap
    // shared with the exiting UI thread. Detach and leak the QThread instead; the
    // OS reclaims it at process exit and the worker's emit is suppressed (cancel).
    if (!workerThread_->wait(3000)) {
        workerThread_->setParent(nullptr);  // intentionally leaked (shutdown only)
    }
}

void SmbDiscoveryController::startScan() {
    if (scanning_) {
        return;
    }
    results_.clear();
    emit resultsChanged();
    setStatus(QStringLiteral("正在扫描局域网…"));
    setScanning(true);
    QMetaObject::invokeMethod(worker_, [w = worker_]() { w->run(); },
                              Qt::QueuedConnection);
}

void SmbDiscoveryController::cancelScan() {
    if (!scanning_) {
        return;
    }
    if (worker_) {
        worker_->requestCancel();
    }
    setStatus(QStringLiteral("正在停止…"));
}

void SmbDiscoveryController::addDiscovered(int index) {
    if (index < 0 || index >= results_.size() || sources_ == nullptr) {
        return;
    }
    QVariantMap entry = results_.at(index).toMap();
    const QString path = entry.value(QStringLiteral("path")).toString();
    const QString label = entry.value(QStringLiteral("share")).toString();
    if (path.isEmpty()) {
        return;
    }
    sources_->addNetworkFolder(path, label);
    entry[QStringLiteral("added")] = true;
    results_[index] = entry;
    emit resultsChanged();
}

void SmbDiscoveryController::onShareFound(
    const QString& host,
    const QString& share,
    const QString& uncPath) {
    QVariantMap entry;
    entry[QStringLiteral("host")] = host;
    entry[QStringLiteral("share")] = share;
    entry[QStringLiteral("path")] = uncPath;
    entry[QStringLiteral("added")] = alreadyConfigured(uncPath);
    results_.append(entry);
    emit resultsChanged();
}

void SmbDiscoveryController::onFinished(int hostsReachable, int sharesFound) {
    setScanning(false);
    if (sharesFound > 0) {
        setStatus(QStringLiteral("发现 %1 个共享（%2 台主机）")
                      .arg(sharesFound)
                      .arg(hostsReachable));
    } else {
        setStatus(QStringLiteral("未发现可用共享，可在上方手动添加"));
    }
}

void SmbDiscoveryController::setScanning(bool value) {
    if (scanning_ != value) {
        scanning_ = value;
        emit scanningChanged();
    }
}

void SmbDiscoveryController::setStatus(const QString& text) {
    if (statusText_ != text) {
        statusText_ = text;
        emit statusChanged();
    }
}

bool SmbDiscoveryController::alreadyConfigured(const QString& uncPath) const {
    if (sources_ == nullptr) {
        return false;
    }
    const QString key = uncPath.toLower();
    const QVariantList roots = sources_->roots();
    for (const QVariant& item : roots) {
        if (item.toMap().value(QStringLiteral("path")).toString().toLower() == key) {
            return true;
        }
    }
    return false;
}

} // namespace kivo::qt::sources
