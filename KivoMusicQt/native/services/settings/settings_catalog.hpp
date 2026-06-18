// =============================================================================
// Kivo Music Qt - settings_catalog.hpp
// 职责: 设置分类注册表——驱动设置外壳的导航栏与搜索索引
// =============================================================================

#pragma once

#include <QObject>
#include <QString>
#include <QStringList>
#include <QVariantList>

namespace kivo::qt::settings {

// The single registry of settings categories. The settings shell renders its
// category rail and builds its search index entirely from `categories`, so a new
// feature self-registers by appending ONE descriptor here — the rail, grouping,
// and search all pick it up with no shell edits.
//
// Each category is a QVariantMap: { id, title, group, tier, icon, qmlSource,
// keywords, weight }. `group` buckets the rail (media/system/services/
// diagnostics); `weight` orders within a group; `qmlSource` is the page loaded
// into the detail pane, relative to ui/qml/pages/settings/.
class SettingsCatalog final : public QObject {
    Q_OBJECT
    Q_PROPERTY(QVariantList categories READ categories CONSTANT)
    Q_PROPERTY(QVariantList groups READ groups CONSTANT)

public:
    explicit SettingsCatalog(QObject* parent = nullptr);

    // Category descriptors: { id, title, group, icon, qmlSource, keywords, weight }.
    [[nodiscard]] QVariantList categories() const { return categories_; }
    // Group descriptors: { id, title, rank }. The rail reads group display title
    // and ordering FROM HERE (not hardcoded), so a new group is fully data-driven
    // — register one group + its categories and the rail picks them up unchanged.
    [[nodiscard]] QVariantList groups() const { return groups_; }

private:
    void registerGroup(const QString& id, const QString& title, int rank);
    void registerCategory(
        const QString& id,
        const QString& title,
        const QString& group,
        const QString& icon,
        const QString& qmlSource,
        const QStringList& keywords,
        int weight);

    QVariantList categories_;
    QVariantList groups_;
};

} // namespace kivo::qt::settings
