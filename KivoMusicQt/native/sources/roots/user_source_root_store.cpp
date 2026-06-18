// =============================================================================
// Kivo Music Qt - user_source_root_store.cpp
// =============================================================================

#include "user_source_root_store.hpp"

#include <QSettings>

namespace kivo::qt::sources {

namespace {
constexpr auto kGroup = "musicSources";
constexpr auto kArray = "musicSources/roots";
constexpr auto kPath = "path";
constexpr auto kLabel = "label";
constexpr auto kEnabled = "enabled";
} // namespace

QList<UserSourceRoot> UserSourceRootStore::load() const {
    QList<UserSourceRoot> roots;
    QSettings settings;
    const int count = settings.beginReadArray(QString::fromLatin1(kArray));
    roots.reserve(count);
    for (int i = 0; i < count; ++i) {
        settings.setArrayIndex(i);
        UserSourceRoot root;
        root.path = settings.value(QString::fromLatin1(kPath)).toString();
        root.label = settings.value(QString::fromLatin1(kLabel)).toString();
        root.enabled = settings.value(QString::fromLatin1(kEnabled), true).toBool();
        if (!root.path.isEmpty()) {
            roots.append(root);
        }
    }
    settings.endArray();
    return roots;
}

void UserSourceRootStore::save(const QList<UserSourceRoot>& roots) const {
    QSettings settings;
    // Clear the whole group first so a now-shorter list cannot leave orphaned
    // higher-index entries behind (beginWriteArray does not guarantee removal).
    settings.remove(QString::fromLatin1(kGroup));
    settings.beginWriteArray(QString::fromLatin1(kArray));
    for (int i = 0; i < roots.size(); ++i) {
        settings.setArrayIndex(i);
        settings.setValue(QString::fromLatin1(kPath), roots.at(i).path);
        settings.setValue(QString::fromLatin1(kLabel), roots.at(i).label);
        settings.setValue(QString::fromLatin1(kEnabled), roots.at(i).enabled);
    }
    settings.endArray();
}

} // namespace kivo::qt::sources
