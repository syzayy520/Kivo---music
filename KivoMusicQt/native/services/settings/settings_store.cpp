// =============================================================================
// Kivo Music Qt - settings_store.cpp
// =============================================================================

#include "settings_store.hpp"

#include <QSettings>

namespace kivo::qt::settings {

QVariant SettingsStore::value(const QString& key, const QVariant& fallback) const {
    return QSettings().value(key, fallback);
}

void SettingsStore::setValue(const QString& key, const QVariant& value) {
    QSettings().setValue(key, value);
}

bool SettingsStore::contains(const QString& key) const {
    return QSettings().contains(key);
}

void SettingsStore::remove(const QString& key) {
    QSettings().remove(key);
}

void SettingsStore::removeGroup(const QString& prefix) {
    QSettings().remove(prefix);
}

bool SettingsStore::boolValue(const QString& key, bool fallback) const {
    return QSettings().value(key, fallback).toBool();
}

int SettingsStore::intValue(const QString& key, int fallback) const {
    return QSettings().value(key, fallback).toInt();
}

double SettingsStore::doubleValue(const QString& key, double fallback) const {
    return QSettings().value(key, fallback).toDouble();
}

QString SettingsStore::stringValue(const QString& key, const QString& fallback) const {
    return QSettings().value(key, fallback).toString();
}

} // namespace kivo::qt::settings
