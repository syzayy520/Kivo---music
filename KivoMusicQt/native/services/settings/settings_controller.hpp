// =============================================================================
// Kivo Music Qt - settings_controller.hpp
// 职责: QML 侧的通用设置门面（简单页级设置的 get/set/reset），context "settingsController"
// =============================================================================

#pragma once

#include <QObject>
#include <QString>
#include <QVariant>

#include "settings_store.hpp"

namespace kivo::qt::settings {

// Generic QML-facing facade over SettingsStore for simple, page-level settings
// that don't warrant their own domain controller. Domains with side effects
// (audio reconfig, source rescans, …) keep their dedicated controllers; this is
// the lightweight path for plain key/value preferences.
class SettingsController final : public QObject {
    Q_OBJECT

public:
    explicit SettingsController(QObject* parent = nullptr) : QObject(parent) {}

    Q_INVOKABLE [[nodiscard]] QVariant value(
        const QString& key, const QVariant& fallback = {}) const {
        QVariant v = store_.value(key, fallback);
        // QSettings(NativeFormat) persists bool/number as strings on Windows, so a
        // stored false reads back as the QString "false" — which QML coerces to a
        // truthy bool (any non-empty string). Coerce the result to the fallback's
        // type so QML gets a real bool/int, not a string. (Qt maps "false"/"0"/""
        // → false.) This is why a once-dark theme could never return to light.
        if (fallback.isValid() && v.isValid()
            && v.metaType() != fallback.metaType()) {
            QVariant converted = v;
            if (converted.convert(fallback.metaType()))
                return converted;
        }
        return v;
    }
    Q_INVOKABLE void setValue(const QString& key, const QVariant& value) {
        store_.setValue(key, value);
        emit changed(key);
    }
    Q_INVOKABLE void reset(const QString& key) {
        store_.remove(key);
        emit changed(key);
    }

signals:
    void changed(const QString& key);

private:
    SettingsStore store_;
};

} // namespace kivo::qt::settings
