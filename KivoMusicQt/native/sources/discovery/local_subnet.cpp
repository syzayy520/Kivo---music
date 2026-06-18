// =============================================================================
// Kivo Music Qt - local_subnet.cpp
// =============================================================================

#include "local_subnet.hpp"

#include <QHostAddress>
#include <QNetworkInterface>
#include <QSet>

namespace kivo::qt::sources {

namespace {
// Hard cap on the number of probed hosts so an unusual interface configuration
// can never turn discovery into an unbounded sweep.
constexpr int kMaxCandidates = 1024;
} // namespace

QStringList LocalSubnet::candidateHosts() {
    QStringList hosts;
    QSet<QString> seen;
    QSet<quint32> selfAddresses;

    const auto interfaces = QNetworkInterface::allInterfaces();

    // Collect this machine's own IPv4 addresses first so we can exclude them.
    for (const QNetworkInterface& iface : interfaces) {
        for (const QNetworkAddressEntry& entry : iface.addressEntries()) {
            const QHostAddress ip = entry.ip();
            if (ip.protocol() == QAbstractSocket::IPv4Protocol) {
                selfAddresses.insert(ip.toIPv4Address());
            }
        }
    }

    for (const QNetworkInterface& iface : interfaces) {
        const auto flags = iface.flags();
        if (!flags.testFlag(QNetworkInterface::IsUp)
            || !flags.testFlag(QNetworkInterface::IsRunning)
            || flags.testFlag(QNetworkInterface::IsLoopBack)) {
            continue;
        }

        for (const QNetworkAddressEntry& entry : iface.addressEntries()) {
            const QHostAddress ip = entry.ip();
            if (ip.protocol() != QAbstractSocket::IPv4Protocol) {
                continue;
            }
            const quint32 raw = ip.toIPv4Address();
            // Skip link-local (169.254/16) — not a usable NAS subnet.
            if ((raw & 0xFFFF0000u) == 0xA9FE0000u) {
                continue;
            }

            // Sweep the local /24 around this interface address.
            const quint32 base = raw & 0xFFFFFF00u;
            for (quint32 host = 1; host <= 254; ++host) {
                const quint32 candidate = base | host;
                if (selfAddresses.contains(candidate)) {
                    continue;
                }
                const QString text = QHostAddress(candidate).toString();
                if (seen.contains(text)) {
                    continue;
                }
                seen.insert(text);
                hosts.append(text);
                if (hosts.size() >= kMaxCandidates) {
                    return hosts;
                }
            }
        }
    }

    return hosts;
}

} // namespace kivo::qt::sources
