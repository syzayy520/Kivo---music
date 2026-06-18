// =============================================================================
// Kivo Music Qt - smb_discovery_worker.cpp
// =============================================================================

#include "smb_discovery_worker.hpp"

#include <QtConcurrent>
#include <QTcpSocket>
#include <QThreadPool>

#include "local_subnet.hpp"
#include "smb_share_lister.hpp"

namespace kivo::qt::sources {

namespace {
constexpr int kSmbPort = 445;
constexpr int kProbeTimeoutMs = 400;   // per-host TCP connect budget
constexpr int kSweepConcurrency = 32;  // parallel probes
} // namespace

void SmbDiscoveryWorker::run() {
    cancelled_.store(false, std::memory_order_relaxed);

    const QStringList hosts = LocalSubnet::candidateHosts();
    if (hosts.isEmpty() || cancelled_.load(std::memory_order_relaxed)) {
        emit finished(0, 0);
        return;
    }

    // ── Stage 1: bounded concurrent TCP-445 reachability sweep ──────────
    // Each probe short-circuits to "unreachable" if cancellation was requested,
    // so an in-flight blockingMapped drains quickly on shutdown.
    std::atomic<bool>* cancelFlag = &cancelled_;
    const auto probe = [cancelFlag](const QString& host) -> QString {
        if (cancelFlag->load(std::memory_order_relaxed)) {
            return QString();
        }
        QTcpSocket socket;
        socket.connectToHost(host, kSmbPort);
        const bool up = socket.waitForConnected(kProbeTimeoutMs);
        socket.abort();
        return up ? host : QString();
    };

    QThreadPool pool;
    pool.setMaxThreadCount(kSweepConcurrency);
    const QList<QString> probed =
        QtConcurrent::blockingMapped(&pool, hosts, probe);

    if (cancelled_.load(std::memory_order_relaxed)) {
        emit finished(0, 0);
        return;
    }

    QStringList reachable;
    for (const QString& host : probed) {
        if (!host.isEmpty()) {
            reachable.append(host);
        }
    }

    // ── Stage 2: enumerate disk shares on each reachable host ───────────
    int sharesFound = 0;
    for (const QString& host : reachable) {
        if (cancelled_.load(std::memory_order_relaxed)) {
            emit finished(reachable.size(), sharesFound);
            return;
        }
        const QStringList shares = SmbShareLister::diskShares(host);
        for (const QString& share : shares) {
            const QString unc =
                QStringLiteral("//") + host + QLatin1Char('/') + share;
            emit shareFound(host, share, unc);
            ++sharesFound;
        }
    }

    emit finished(reachable.size(), sharesFound);
}

} // namespace kivo::qt::sources
