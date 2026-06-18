// =============================================================================
// Kivo Music Qt - smb_share_lister.cpp
// =============================================================================

#include "smb_share_lister.hpp"

#ifdef Q_OS_WIN
#  ifndef WIN32_LEAN_AND_MEAN
#    define WIN32_LEAN_AND_MEAN
#  endif
#  ifndef NOMINMAX
#    define NOMINMAX
#  endif
#  include <windows.h>
#  include <lm.h>          // NetShareEnum, SHARE_INFO_1, STYPE_*
#  pragma comment(lib, "netapi32.lib")
#endif

#include <QtGlobal>

namespace kivo::qt::sources {

QStringList SmbShareLister::diskShares(const QString& host) {
    QStringList shares;

#ifdef Q_OS_WIN
    if (host.isEmpty()) {
        return shares;
    }

    // NetShareEnum accepts a UNC server name; the leading "\\" is optional but
    // we normalize to it for clarity.
    const QString server = QStringLiteral("\\\\") + host;
    std::wstring serverW = server.toStdWString();

    PSHARE_INFO_1 buffer = nullptr;
    DWORD entriesRead = 0;
    DWORD totalEntries = 0;
    DWORD resumeHandle = 0;

    const NET_API_STATUS status = NetShareEnum(
        const_cast<LPWSTR>(serverW.c_str()),
        1,
        reinterpret_cast<LPBYTE*>(&buffer),
        MAX_PREFERRED_LENGTH,
        &entriesRead,
        &totalEntries,
        &resumeHandle);

    // ERROR_MORE_DATA would mean a truncated set; MAX_PREFERRED_LENGTH asks the
    // API to allocate as much as needed, so a single pass is sufficient here.
    if ((status == NERR_Success || status == ERROR_MORE_DATA) && buffer != nullptr) {
        PSHARE_INFO_1 entry = buffer;
        for (DWORD i = 0; i < entriesRead; ++i, ++entry) {
            // Disk shares only; skip special/administrative shares (IPC$, ADMIN$,
            // C$, …) flagged by STYPE_SPECIAL or a trailing '$'.
            const DWORD type = entry->shi1_type & 0x0FFFFFFFu;
            const bool special = (entry->shi1_type & STYPE_SPECIAL) != 0;
            if (type != STYPE_DISKTREE || special) {
                continue;
            }
            const QString name = QString::fromWCharArray(entry->shi1_netname);
            if (name.isEmpty() || name.endsWith(QLatin1Char('$'))) {
                continue;
            }
            shares.append(name);
        }
    }

    if (buffer != nullptr) {
        NetApiBufferFree(buffer);
    }
#else
    Q_UNUSED(host);
#endif

    return shares;
}

} // namespace kivo::qt::sources
