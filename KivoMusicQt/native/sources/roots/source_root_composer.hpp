// =============================================================================
// Kivo Music Qt - source_root_composer.hpp
// 职责: 把内置根 + 用户根合成为实际扫描根列表（纯函数，无 I/O）
// =============================================================================

#pragma once

#include <QList>
#include <QString>

#include "user_source_root.h"
#include "../music/music_source_root.h"

namespace kivo::qt::sources {

// Composes the effective scan roots: the built-in Local Music folder (always
// present) followed by the user's ENABLED network roots, de-duplicated by
// normalized path (case-insensitive). Pure — no I/O, no persistence.
class SourceRootComposer final {
public:
    [[nodiscard]] static QList<MusicSourceRoot> compose(
        const QList<UserSourceRoot>& userRoots);

    // Normalize user input to a forward-slash UNC ("//host/share"). Accepts
    // smb:// scheme, backslash UNC ("\\host\share"), forward-slash UNC, and a
    // bare "host/share". Returns empty for blank input.
    [[nodiscard]] static QString normalizeNetworkPath(const QString& input);

    // A sensible display label derived from a normalized path: the last path
    // segment (share/folder), falling back to "Network Folder".
    [[nodiscard]] static QString deriveLabel(const QString& normalizedPath);
};

} // namespace kivo::qt::sources
