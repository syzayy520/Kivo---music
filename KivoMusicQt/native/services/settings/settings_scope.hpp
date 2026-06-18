// =============================================================================
// Kivo Music Qt - settings_scope.hpp
// 职责: 设置项的作用域分类（决定同步/备份策略）
// =============================================================================

#pragma once

namespace kivo::qt::settings {

// Classifies where a setting "belongs", which governs future sync/backup:
//   Machine   — tied to THIS device; never synced (output device, cache path,
//               DB location, window geometry).
//   Portable  — follows the user; safe to sync/back up (theme, EQ presets,
//               shortcuts, language).
//   PerEntity — scoped to a specific entity (a source, a cloud backend, …).
// Defining this now keeps later sync/backup additive and non-breaking.
enum class SettingsScope {
    Machine,
    Portable,
    PerEntity,
};

} // namespace kivo::qt::settings
