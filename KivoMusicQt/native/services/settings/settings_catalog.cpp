// =============================================================================
// Kivo Music Qt - settings_catalog.cpp
// =============================================================================

#include "settings_catalog.hpp"

#include <QVariantMap>

namespace kivo::qt::settings {

SettingsCatalog::SettingsCatalog(QObject* parent) : QObject(parent) {
    // ── Groups (display title + rail order) — data-driven, not hardcoded in QML ──
    // Source language is English; kivo_zh_CN.ts maps these to 简体中文.
    registerGroup(QStringLiteral("media"), tr("Media Hub"), 0);
    registerGroup(QStringLiteral("system"), tr("System"), 1);
    registerGroup(QStringLiteral("services"), tr("Account & Services"), 2);
    registerGroup(QStringLiteral("diagnostics"), tr("Diagnostics"), 3);

    // ── Categories ───────────────────────────────────────────────────────
    registerCategory(
        QStringLiteral("sources"), tr("Sources"), QStringLiteral("media"),
        QStringLiteral("🗄️"),
        QStringLiteral("categories/sources/SourcesPage.qml"),
        {QStringLiteral("folder"), QStringLiteral("nas"), QStringLiteral("smb"),
         QStringLiteral("network"), QStringLiteral("cloud"), QStringLiteral("scan")},
        10);
    registerCategory(
        QStringLiteral("audio"), tr("Audio Engine"), QStringLiteral("media"),
        QStringLiteral("🔊"),
        QStringLiteral("categories/audio/AudioEnginePage.qml"),
        {QStringLiteral("output"), QStringLiteral("device"), QStringLiteral("wasapi"),
         QStringLiteral("exclusive"), QStringLiteral("bit-perfect"), QStringLiteral("replaygain")},
        20);

    registerCategory(
        QStringLiteral("appearance"), tr("Appearance"), QStringLiteral("system"),
        QStringLiteral("🎨"),
        QStringLiteral("categories/appearance/AppearancePage.qml"),
        {QStringLiteral("theme"), QStringLiteral("dark"), QStringLiteral("light"),
         QStringLiteral("accent"), QStringLiteral("color")},
        10);

    registerCategory(
        QStringLiteral("language"), tr("Language"), QStringLiteral("system"),
        QStringLiteral("🌐"),
        QStringLiteral("categories/language/LanguagePage.qml"),
        {QStringLiteral("language"), QStringLiteral("locale"), QStringLiteral("english"),
         QStringLiteral("chinese"), QStringLiteral("zh_CN"), QStringLiteral("i18n")},
        20);

    registerCategory(
        QStringLiteral("about"), tr("About & Legal"), QStringLiteral("diagnostics"),
        QStringLiteral("ℹ️"),
        QStringLiteral("categories/about/AboutLegalPage.qml"),
        {QStringLiteral("about"), QStringLiteral("privacy"), QStringLiteral("license"),
         QStringLiteral("eula"), QStringLiteral("version")},
        90);
}

void SettingsCatalog::registerGroup(
    const QString& id, const QString& title, int rank) {
    QVariantMap entry;
    entry[QStringLiteral("id")] = id;
    entry[QStringLiteral("title")] = title;
    entry[QStringLiteral("rank")] = rank;
    groups_.append(entry);
}

void SettingsCatalog::registerCategory(
    const QString& id,
    const QString& title,
    const QString& group,
    const QString& icon,
    const QString& qmlSource,
    const QStringList& keywords,
    int weight) {
    QVariantMap entry;
    entry[QStringLiteral("id")] = id;
    entry[QStringLiteral("title")] = title;
    entry[QStringLiteral("group")] = group;
    entry[QStringLiteral("icon")] = icon;
    entry[QStringLiteral("qmlSource")] = qmlSource;
    entry[QStringLiteral("keywords")] = keywords;
    entry[QStringLiteral("weight")] = weight;
    categories_.append(entry);
}

} // namespace kivo::qt::settings
