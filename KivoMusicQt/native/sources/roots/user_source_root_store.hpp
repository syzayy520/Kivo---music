// =============================================================================
// Kivo Music Qt - user_source_root_store.hpp
// 职责: 用户网络来源根的持久化（QSettings）
// =============================================================================

#pragma once

#include <QList>

#include "user_source_root.h"

namespace kivo::qt::sources {

// Persists the user's configured network source roots via QSettings
// (HKCU\Software\Kivo\Kivo Music on Windows). UI-thread only — QSettings access
// is cheap and synchronous here; the actual scanning happens off-thread.
class UserSourceRootStore final {
public:
    [[nodiscard]] QList<UserSourceRoot> load() const;
    void save(const QList<UserSourceRoot>& roots) const;
};

} // namespace kivo::qt::sources
