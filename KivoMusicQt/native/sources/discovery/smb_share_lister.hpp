// =============================================================================
// Kivo Music Qt - smb_share_lister.hpp
// 职责: 列出某个 SMB 主机上的磁盘共享（Win32 NetShareEnum 封装）
// =============================================================================

#pragma once

#include <QString>
#include <QStringList>

namespace kivo::qt::sources {

// Lists the file (disk) shares exposed by a reachable SMB host. Best-effort:
// returns empty on access-denied / unreachable / non-Windows. Excludes the
// administrative and special shares (IPC$, ADMIN$, C$, print$, …).
class SmbShareLister final {
public:
    // `host` is an IP or hostname (no leading backslashes required).
    [[nodiscard]] static QStringList diskShares(const QString& host);
};

} // namespace kivo::qt::sources
