// =============================================================================
// Kivo Music Qt - local_subnet.hpp
// 职责: 从本机网卡推导出局域网候选主机 IP（用于 SMB 自动发现的扫描范围）
// =============================================================================

#pragma once

#include <QStringList>

namespace kivo::qt::sources {

// Derives the set of LAN host IPs to probe for SMB auto-discovery.
class LocalSubnet final {
public:
    // For each up, non-loopback IPv4 interface, returns every host address in
    // its local /24 (.1–.254), excluding the machine's own address. Results are
    // de-duplicated across interfaces and capped to keep the sweep bounded.
    [[nodiscard]] static QStringList candidateHosts();
};

} // namespace kivo::qt::sources
