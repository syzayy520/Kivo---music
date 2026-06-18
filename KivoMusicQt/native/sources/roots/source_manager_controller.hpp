// =============================================================================
// Kivo Music Qt - source_manager_controller.hpp
// 职责: QML 侧的音乐来源管理门面（context property "sourceManager"）
// 线程: 仅 UI 线程（QSettings + QML）。扫描在别处的工作线程进行。
// =============================================================================

#pragma once

#include <QList>
#include <QObject>
#include <QVariantList>

#include "user_source_root.h"
#include "user_source_root_store.hpp"
#include "../music/music_source_root.h"

namespace kivo::qt::sources {

// QML-facing facade for managing music source folders. Owns the persisted user
// network roots and composes them with the built-in Local Music folder to
// produce the effective scan roots. Editing a root persists immediately and
// emits sourcesChanged() so the host can re-run the library scan.
class SourceManagerController final : public QObject {
    Q_OBJECT
    // List for the Settings UI. Each entry is a QVariantMap:
    //   { path, label, enabled, removable, network }
    // Built-in roots are not removable; user network roots are.
    Q_PROPERTY(QVariantList roots READ roots NOTIFY rootsChanged)

public:
    explicit SourceManagerController(QObject* parent = nullptr);

    [[nodiscard]] QVariantList roots() const { return rootsView_; }

    // Effective scan roots (built-in + enabled user roots), de-duplicated.
    // C++-side; consumed by the host to (re)start the library scan.
    [[nodiscard]] QList<MusicSourceRoot> effectiveRoots() const;

    // Add a network folder (UNC / smb:// / bare host\share). Normalized,
    // de-duplicated, and persisted. Blank or duplicate input is ignored.
    Q_INVOKABLE void addNetworkFolder(
        const QString& path,
        const QString& label = {});
    // Remove a USER root by its index in the `roots` list (built-ins ignored).
    Q_INVOKABLE void removeRoot(int viewIndex);
    // Enable/disable a USER root by its index in the `roots` list.
    Q_INVOKABLE void setRootEnabled(int viewIndex, bool enabled);

signals:
    void rootsChanged();    // the `roots` view changed — UI should refresh
    void sourcesChanged();  // effective roots changed — host should rescan

private:
    void rebuildView();
    void persistAndNotify();
    [[nodiscard]] int toUserIndex(int viewIndex) const;

    UserSourceRootStore store_;
    QList<MusicSourceRoot> builtIn_;
    QList<UserSourceRoot> userRoots_;
    QVariantList rootsView_;
};

} // namespace kivo::qt::sources
