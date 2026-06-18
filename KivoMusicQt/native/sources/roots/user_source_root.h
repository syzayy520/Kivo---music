// =============================================================================
// Kivo Music Qt - user_source_root.h
// 职责: 用户配置的音乐来源（通常是 NAS/SMB 网络共享）——纯数据
// =============================================================================

#pragma once

#include <QString>

namespace kivo::qt::sources {

// A user-configured music source folder (typically a NAS/SMB share). `path` is
// stored normalized as a forward-slash UNC ("//host/share/.."), which is the
// form MusicFileScanner recognizes as a network root.
struct UserSourceRoot {
    QString path;
    QString label;
    bool enabled = true;
};

} // namespace kivo::qt::sources
