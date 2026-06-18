// =============================================================================
// Kivo Music Qt - settings_store.hpp
// 职责: 应用设置的持久化（QSettings 的类型化薄封装）——纯持久化，无 UI/领域逻辑
// =============================================================================

#pragma once

#include <QString>
#include <QVariant>

namespace kivo::qt::settings {

// Thin typed wrapper over the app's QSettings (org "Kivo" / app "Kivo Music" =>
// HKCU\Software\Kivo\Kivo Music on Windows). Generalizes UserSourceRootStore's
// pattern into a reusable persistence primitive for every settings domain.
// Stateless: each call opens a cheap QSettings, exactly like UserSourceRootStore.
class SettingsStore {
public:
    [[nodiscard]] QVariant value(const QString& key, const QVariant& fallback = {}) const;
    void setValue(const QString& key, const QVariant& value);

    [[nodiscard]] bool contains(const QString& key) const;
    void remove(const QString& key);          // revert a single key to its default
    void removeGroup(const QString& prefix);  // revert a whole group (e.g. "appearance")

    // Typed conveniences — read with an explicit per-call default.
    [[nodiscard]] bool boolValue(const QString& key, bool fallback) const;
    [[nodiscard]] int intValue(const QString& key, int fallback) const;
    [[nodiscard]] double doubleValue(const QString& key, double fallback) const;
    [[nodiscard]] QString stringValue(const QString& key, const QString& fallback) const;
};

} // namespace kivo::qt::settings
