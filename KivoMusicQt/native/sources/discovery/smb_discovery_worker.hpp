// =============================================================================
// Kivo Music Qt - smb_discovery_worker.hpp
// 职责: 在工作线程上扫描局域网 SMB 主机并枚举共享（不触碰任何 GUI）
// =============================================================================

#pragma once

#include <QObject>
#include <QString>

#include <atomic>

namespace kivo::qt::sources {

// Lives on a dedicated discovery thread. Probes the local subnet for hosts with
// SMB (TCP 445) open, then enumerates each reachable host's disk shares. Emits
// one shareFound per discovered share and a single finished at the end. Never
// touches a QAbstractItemModel or a QML-bound object directly.
class SmbDiscoveryWorker final : public QObject {
    Q_OBJECT

public:
    using QObject::QObject;

    // Invoked on the worker thread (via a queued functor). Blocking network
    // sweep; honors requestCancel() between and within stages.
    void run();

    // Thread-safe (atomic): set from the UI thread to abort an in-flight scan so
    // app shutdown never blocks on a slow sweep.
    void requestCancel() { cancelled_.store(true, std::memory_order_relaxed); }

signals:
    // host: the IP probed; share: the share name; uncPath: "//host/share".
    void shareFound(const QString& host, const QString& share, const QString& uncPath);
    void finished(int hostsReachable, int sharesFound);

private:
    std::atomic<bool> cancelled_{false};
};

} // namespace kivo::qt::sources
