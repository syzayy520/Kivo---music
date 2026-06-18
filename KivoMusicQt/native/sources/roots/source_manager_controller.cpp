// =============================================================================
// Kivo Music Qt - source_manager_controller.cpp
// =============================================================================

#include "source_manager_controller.hpp"

#include <QVariantMap>

#include "source_root_composer.hpp"
#include "../music/music_source_roots.h"

namespace kivo::qt::sources {

SourceManagerController::SourceManagerController(QObject* parent)
    : QObject(parent),
      builtIn_(MusicSourceRoots::builtIn()),
      userRoots_(store_.load()) {
    rebuildView();
}

QList<MusicSourceRoot> SourceManagerController::effectiveRoots() const {
    return SourceRootComposer::compose(userRoots_);
}

void SourceManagerController::addNetworkFolder(
    const QString& path,
    const QString& label) {
    const QString normalized = SourceRootComposer::normalizeNetworkPath(path);
    if (normalized.isEmpty()) {
        return;
    }

    const QString key = normalized.toLower();
    for (const auto& builtin : builtIn_) {
        if (builtin.path.toLower() == key) {
            return;  // already covered by a built-in root
        }
    }
    for (const auto& user : userRoots_) {
        if (user.path.toLower() == key) {
            return;  // already configured
        }
    }

    UserSourceRoot root;
    root.path = normalized;
    root.label = label.trimmed();
    root.enabled = true;
    userRoots_.append(root);
    persistAndNotify();
}

void SourceManagerController::removeRoot(int viewIndex) {
    const int userIdx = toUserIndex(viewIndex);
    if (userIdx < 0) {
        return;
    }
    userRoots_.removeAt(userIdx);
    persistAndNotify();
}

void SourceManagerController::setRootEnabled(int viewIndex, bool enabled) {
    const int userIdx = toUserIndex(viewIndex);
    if (userIdx < 0 || userRoots_.at(userIdx).enabled == enabled) {
        return;
    }
    userRoots_[userIdx].enabled = enabled;
    persistAndNotify();
}

int SourceManagerController::toUserIndex(int viewIndex) const {
    const int userIdx = viewIndex - static_cast<int>(builtIn_.size());
    if (userIdx < 0 || userIdx >= userRoots_.size()) {
        return -1;
    }
    return userIdx;
}

void SourceManagerController::rebuildView() {
    rootsView_.clear();
    rootsView_.reserve(builtIn_.size() + userRoots_.size());

    for (const auto& builtin : builtIn_) {
        QVariantMap entry;
        entry[QStringLiteral("path")] = builtin.path;
        entry[QStringLiteral("label")] = builtin.label;
        entry[QStringLiteral("enabled")] = true;
        entry[QStringLiteral("removable")] = false;
        entry[QStringLiteral("network")] = false;
        rootsView_.append(entry);
    }
    for (const auto& user : userRoots_) {
        QVariantMap entry;
        entry[QStringLiteral("path")] = user.path;
        entry[QStringLiteral("label")] = user.label.isEmpty()
            ? SourceRootComposer::deriveLabel(user.path)
            : user.label;
        entry[QStringLiteral("enabled")] = user.enabled;
        entry[QStringLiteral("removable")] = true;
        entry[QStringLiteral("network")] = true;
        rootsView_.append(entry);
    }
}

void SourceManagerController::persistAndNotify() {
    store_.save(userRoots_);
    rebuildView();
    emit rootsChanged();
    emit sourcesChanged();
}

} // namespace kivo::qt::sources
